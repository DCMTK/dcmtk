/*
 *
 *  Copyright (C) 2002-2018, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Interface class for simplified creation of a DICOMDIR
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dcddirif.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/dcmdata/dcmetinf.h"   /* for class DcmMetaInfo */
#include "dcmtk/dcmdata/dcpixel.h"    /* for class DcmPixelData */
#include "dcmtk/dcmdata/dcpxitem.h"   /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcpixseq.h"   /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcvrcs.h"     /* for class DcmCodeString */
#include "dcmtk/dcmdata/dcvrda.h"     /* for class DcmDate */
#include "dcmtk/dcmdata/dcvrtm.h"     /* for class DcmTime */

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofbmanip.h"     /* for class OFBitmanipTemplate */
#include "dcmtk/ofstd/ofcast.h"


/*-------------------------*
 *  constant declarations  *
 *-------------------------*/

// an ISO 9660 format only allows 8 characters in file name
#define MAX_FNAME_COMPONENT_SIZE 8
// DICOM only allows max 8 path components in a file name
#define MAX_FNAME_COMPONENTS 8
// max. number of characters printed for string values in a warning message
#define MAX_PRINT_LENGTH 64
// filename extension for a backup file
#define FNAME_BACKUP_EXTENSION ".BAK"
// prefix used to automatically create patient IDs (+ 6 digits)
#define AUTO_PATIENTID_PREFIX "DCMTKPAT"
// prefix used to automatically create study IDs (+ 6 digits)
#define AUTO_STUDYID_PREFIX "DCMTKSTUDY"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

static OFBool compareSQAttributes(DcmSequenceOfItems *seq1,
                                  DcmSequenceOfItems *seq2,
                                  OFString &reason);


/*--------------------------*
 *  local helper functions  *
 *--------------------------*/

// count number of filename components
static int componentCount(const OFString &filename,
                          const char separator = PATH_SEPARATOR)
{
    const size_t length = filename.length();
    int count = (length > 0) ? 1 : 0;
    for (size_t i = 0; i < length; i++)
    {
        if (filename.at(i) == separator)
            count++;
    }
    return count;
}


// check whether filename component size is within allowed range
static OFBool isComponentTooLarge(const OFString &filename,
                                  const size_t componentLimit,
                                  const OFBool mapFilenames,
                                  const char separator = PATH_SEPARATOR)
{
    OFBool result = OFFalse;
    const size_t length = filename.length();
    if (length > 0)
    {
        size_t pos1 = 0;
        size_t pos2 = filename.find(separator);
        while (pos2 != OFString_npos)
        {
            /* check whether component length is within limit */
            if (pos2 - pos1 > componentLimit)
            {
                result = OFTrue;
                break;
            }
            pos1 = pos2 + 1;
            pos2 = filename.find(separator, pos1);
        }
        if (!result)
        {
            /* check last component: disregard trailing point */
            if (mapFilenames && (filename.at(length - 1) == '.'))
                pos1++;
            if (length - pos1 > componentLimit)
                result = OFTrue;
        }
    }
    return result;
}


// check whether filename contains invalid characters (and locate the first one)
static OFBool locateInvalidFilenameChars(const OFString &filename,
                                         size_t &invalidChar,
                                         const OFBool mapFilenames,
                                         const char separator = PATH_SEPARATOR)
{
    unsigned char c;
    size_t i = 0;
    size_t length = filename.length();
    /* disregard trailing point */
    if (mapFilenames && (length > 0) && (filename.at(length - 1) == '.'))
        length--;
    /* iterate over all characters */
    for (i = 0; i < length; i++)
    {
        c = filename.at(i);
        if ((c == '_') || isdigit(c) || (c == separator) ||
            (isalpha(c) && (isupper(c) || (islower(c) && mapFilenames))))
        {
            /* all ok */
        }
        else if ((c != '\\') || (separator != '\\'))
        {
            /* invalid character */
            break;
        }
    }
    /* return position of first invalid character (eos if all valid) */
    invalidChar = i;
    /* OFTrue in case of any invalid character */
    return (i != length);
}


// massage filename into DICOM format (DOS conventions for path separators, uppercase characters)
static OFString &hostToDicomFilename(const OFString &hostFilename,
                                     OFString &dicomFilename)
{
    dicomFilename.clear();
    const size_t length = hostFilename.length();
    dicomFilename.reserve(length);
    for (size_t i = 0; i < length; i++)
    {
        const unsigned char c = hostFilename.at(i);
        if (c == PATH_SEPARATOR)
        {
            /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
            dicomFilename += '\\';
        }
        else if (isalpha(c))
        {
            /* filenames in DICOM must always be in uppercase */
            dicomFilename += OFstatic_cast(char, toupper(c));
        }
        else if (isdigit(c) || (c == '_') || (c == '\\'))
        {
            /* only characters, digits, underscore and backslash (path separator) allowed */
            dicomFilename += c;
        }
    }
    return dicomFilename;
}


/* Massage filename into machine format (replace DOS conventions for path separators) */
static OFString &dicomToHostFilename(const OFString &dicomFilename,
                                     OFString &hostFilename,
                                     OFBool mapToLower = OFFalse)
{
    hostFilename.clear();
    const size_t length = dicomFilename.length();
    hostFilename.reserve(length);
    for (size_t i = 0; i < length; i++)
    {
        const unsigned char c = dicomFilename.at(i);
        /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
        if (c == '\\')
            hostFilename += PATH_SEPARATOR;
        else if (mapToLower)
            hostFilename += OFstatic_cast(char, tolower(c));
        else
            hostFilename += c;
    }
    return hostFilename;
}


// check whether given DICOM file exists
static OFBool locateDicomFile(const OFString &dicomFilename,
                              OFString &hostFilename)
{
    dicomToHostFilename(dicomFilename, hostFilename);
    OFBool result = OFStandard::fileExists(hostFilename);
    if (!result)
    {
        /* trailing period */
        hostFilename += '.';
        result = OFStandard::fileExists(hostFilename);
    }
    if (!result)
    {
        /* lowercase */
        dicomToHostFilename(dicomFilename, hostFilename, OFTrue /*mapToLower*/);
        result = OFStandard::fileExists(hostFilename);
        if (!result)
        {
            /* lowercase with trailing period */
            hostFilename += '.';
            result = OFStandard::fileExists(hostFilename);
        }
    }
    if (!result)
    {
        /* DICOM file not found */
        hostFilename.clear();
    }
    return result;
}


// compare two strings (empty strings are always regarded as unequal)
static OFBool compare(const OFString &string1,
                      const OFString &string2)
{
    if (string1.empty() || string2.empty())
        return OFFalse;
    return (string1.compare(string2) == 0) ? OFTrue : OFFalse;
}


// construct tag name from given object
static OFString &constructTagName(DcmObject *object,
                                  OFString &tagName)
{
    if (object != NULL)
    {
        DcmTag tag = object->getTag();
        tagName = tag.getTagName();
        // use tag name only if not equal to DcmTag_ERROR_TagName
        if (compare(tagName, DcmTag_ERROR_TagName))
        {
            char buffer[32];
            sprintf(buffer, "(0x%04x,0x%04x)", tag.getGTag(), tag.getETag());
            tagName = buffer;
        }
    } else
        tagName = "(NULL)";
    return tagName;
}


// construct tag name from given object and sequence
static OFString &constructTagNameWithSQ(DcmObject *object,
                                        DcmSequenceOfItems *fromSequence,
                                        const unsigned long itemNumber,
                                        OFString &tagName)
{
    OFString tempStr;
    if (fromSequence != NULL)
    {
        /* create text */
        OFOStringStream oss;
        oss << constructTagName(fromSequence, tempStr) << "[" << itemNumber << "]" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        tagName = tmpString;
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* append "." for tag name */
        if (object != NULL)
            tagName += '.';
    } else
        tagName.clear();
    if (object != NULL)
        tagName += constructTagName(object, tempStr);
    else if (tagName.empty())
        tagName = "(NULL)";
    return tagName;
}


// construct text message of two differing numerical values
static OFString &constructDifferentNumbersText(const unsigned long number1,
                                               const unsigned long number2,
                                               OFString &textValue)
{
    textValue.clear();
    /* create message text */
    OFOStringStream oss;
    oss << number1 << " != " << number2 << OFStringStream_ends;
    OFSTRINGSTREAM_GETSTR(oss, tmpString)
    textValue = tmpString;
    OFSTRINGSTREAM_FREESTR(tmpString)
    return textValue;
}


// compare given element values and report any deviation
static OFBool compareAttributes(DcmElement *elem1,
                                DcmElement *elem2,
                                DcmSequenceOfItems *fromSequence,
                                unsigned long itemNumber,
                                OFString &reason)
{
    reason.clear();
    OFString tmpString;
    /* check whether elements are valid */
    if (elem1 != NULL)
    {
        if (elem2 != NULL)
        {
            /* check whether tags are equal */
            if (elem1->getTag().getXTag() == elem2->getTag().getXTag())
            {
                DcmVR vr1(elem1->getVR());
                DcmVR vr2(elem2->getVR());
                /* are the VRs the same? */
                if (vr1.getEVR() == vr2.getEVR())
                {
                    const unsigned long length1 = elem1->getLength();
                    const unsigned long length2 = elem2->getLength();
                    /* are the lengths the same? */
                    if (length1 == length2)
                    {
                        /* are the contents the same? */
                        if (length1 > 0)
                        {
                            if (vr1.getEVR() == EVR_SQ)
                            {
                                /* compare embedded sequences recursively */
                                compareSQAttributes(OFstatic_cast(DcmSequenceOfItems *, elem1),
                                                    OFstatic_cast(DcmSequenceOfItems *, elem2), reason);
                            } else {
                                /* everything else is regarded as a string, might be useful to check binary data separately though */
                                OFString value1, value2;
                                /* compare string value from dataset and record */
                                if (elem1->getOFStringArray(value1).good() && elem2->getOFStringArray(value2).good())
                                {
                                    if (!compare(value1, value2))
                                    {
                                        reason = "different values: ";
                                        /* print differing values */
                                        if ((value1.length() < MAX_PRINT_LENGTH) && (value2.length() < MAX_PRINT_LENGTH))
                                            reason += "\"" + value1 + "\" != \"" + value2 + "\" ";
                                        reason += "(" + constructTagNameWithSQ(elem1, fromSequence, itemNumber, tmpString) + ")";
                                    }
                                }
                            }
                        }
                    } else {
                        reason = "different value lengths: " + constructDifferentNumbersText(length1, length2, tmpString);
                        reason += " (" + constructTagNameWithSQ(elem1, fromSequence, itemNumber, tmpString) + ")";
                    }
                } else
                    reason = "different VRs: " + OFString(vr1.getVRName()) + " != " + vr2.getVRName()
                             + " (" + constructTagNameWithSQ(elem1, fromSequence, itemNumber, tmpString) + ")";
            } else
                reason = "INTERNAL ERROR: different attributes: "
                         + constructTagNameWithSQ(elem1, fromSequence, itemNumber, tmpString) + " != "
                         + constructTagNameWithSQ(elem2, fromSequence, itemNumber, tmpString);
        } else
            reason = "missing attribute: " + constructTagNameWithSQ(elem2, fromSequence, itemNumber, tmpString);
    } else
        reason = "missing attribute: " + constructTagNameWithSQ(elem1, fromSequence, itemNumber, tmpString);
    /* non-empty reason variable indicates an error */
    return reason.empty();
}


// compare given items for equivalence and report any deviation
static OFBool compareItems(DcmItem *item1,
                           DcmItem *item2,
                           DcmSequenceOfItems *fromSequence,
                           const unsigned long itemNumber,
                           OFString &reason)
{
    reason.clear();
    OFString tmpString;
    /* check whether items are valid */
    if ((item1 != NULL) && (item2 != NULL))
    {
        const unsigned long card1 = item1->card();
        const unsigned long card2 = item2->card();
        /* check whether number of attributes is identical */
        if (card1 == card2)
        {
            unsigned long i = 0;
            OFBool first = OFTrue;
            DcmStack stack1, stack2;
            /* check whether attributes are equal */
            while (item1->nextObject(stack1, first).good() && item2->nextObject(stack2, first).good())
            {
                if (!compareAttributes(OFstatic_cast(DcmElement *, stack1.top()), OFstatic_cast(DcmElement *, stack2.top()), fromSequence, i++, reason))
                    break;
                first = OFFalse;
            }
        } else {
            reason = "different number of attributes in item: " + constructDifferentNumbersText(card1, card2, tmpString);
            reason += " (" + constructTagNameWithSQ(NULL /*object*/, fromSequence, itemNumber, tmpString) + ")";
        }
    } else
        reason = "missing item: " + constructTagNameWithSQ(NULL /*object*/, fromSequence, itemNumber, tmpString);
    /* non-empty reason variable indicates an error */
    return reason.empty();
}


// compare given sequences for equivalence and report any deviation
static OFBool compareSQAttributes(DcmSequenceOfItems *seq1,
                                  DcmSequenceOfItems *seq2,
                                  OFString &reason)
{
    reason.clear();
    OFString tmpString;
    /* check whether sequences are valid */
    if (seq1 != NULL)
    {
        if (seq2 != NULL)
        {
            /* check whether tags are equal */
            if (seq1->getTag().getXTag() == seq2->getTag().getXTag())
            {
                const unsigned long card1 = seq1->card();
                const unsigned long card2 = seq2->card();
                /* check whether number of items is identical */
                if (card1 == card2)
                {
                    unsigned long i = 0;
                    OFBool first = OFTrue;
                    DcmStack stack1, stack2;
                    /* check whether items are equal */
                    while (seq1->nextObject(stack1, first).good() && seq2->nextObject(stack2, first).good())
                    {
                        if (!compareItems(OFstatic_cast(DcmItem *, stack1.top()), OFstatic_cast(DcmItem *, stack2.top()), seq1, i++, reason))
                            break;
                        first = OFFalse;
                    }
                } else {
                    reason = "different number of items in sequence: " + constructDifferentNumbersText(card1, card2, tmpString);
                    reason += " (" + constructTagName(seq1, tmpString) + ")";
                }
            } else
                reason = "INTERNAL ERROR: different sequences: " + constructTagName(seq1, tmpString) + " != "
                                                                 + constructTagName(seq2, tmpString);
        } else
            reason = "missing sequence: " +  constructTagName(seq2, tmpString);
    } else
        reason = "missing sequence: " +  constructTagName(seq1, tmpString);
    /* non-empty reason variable indicates an error */
    return reason.empty();
}


// get directory record name from type (static)
OFString DicomDirInterface::recordTypeToName(const E_DirRecType recordType)
{
    const char *recordName = NULL;
    switch (recordType)
    {
        case ERT_root:
            recordName = "Root";
            break;
        case ERT_Curve:
            recordName = "Curve";
            break;
        case ERT_FilmBox:
            recordName = "FilmBox";
            break;
        case ERT_FilmSession:
            recordName = "FilmSession";
            break;
        case ERT_Image:
            recordName = "Image";
            break;
        case ERT_ImageBox:
            recordName = "ImageBox";
            break;
        case ERT_Interpretation:
            recordName = "Interpretation";
            break;
        case ERT_ModalityLut:
            recordName = "ModalityLUT";
            break;
        case ERT_Mrdr:
            recordName = "MRDR";
            break;
        case ERT_Overlay:
            recordName = "Overlay";
            break;
        case ERT_Patient:
            recordName = "Patient";
            break;
        case ERT_PrintQueue:
            recordName = "PrintQueue";
            break;
        case ERT_Private:
            recordName = "Private";
            break;
        case ERT_Results:
            recordName = "Results";
            break;
        case ERT_Series:
            recordName = "Series";
            break;
        case ERT_Study:
            recordName = "Study";
            break;
        case ERT_StudyComponent:
            recordName = "StudyComponent";
            break;
        case ERT_Topic:
            recordName = "Topic";
            break;
        case ERT_Visit:
            recordName = "Visit";
            break;
        case ERT_VoiLut:
            recordName = "VOILUT";
            break;
        case ERT_SRDocument:
            recordName = "SRDocument";
            break;
        case ERT_Presentation:
            recordName = "Presentation";
            break;
        case ERT_Waveform:
            recordName = "Waveform";
            break;
        case ERT_RTDose:
            recordName = "RTDose";
            break;
        case ERT_RTStructureSet:
            recordName = "RTStructureSet";
            break;
        case ERT_RTPlan:
            recordName = "RTPlan";
            break;
        case ERT_RTTreatRecord:
            recordName = "RTTreatRecord";
            break;
        case ERT_StoredPrint:
            recordName = "StoredPrint";
            break;
        case ERT_KeyObjectDoc:
            recordName = "KeyObjectDoc";
            break;
        case ERT_Registration:
            recordName = "Registration";
            break;
        case ERT_Fiducial:
            recordName = "Fiducial";
            break;
        case ERT_RawData:
            recordName = "RawData";
            break;
        case ERT_Spectroscopy:
            recordName = "Spectroscopy";
            break;
        case ERT_EncapDoc:
            recordName = "EncapDoc";
            break;
        case ERT_ValueMap:
            recordName = "ValueMap";
            break;
        case ERT_HangingProtocol:
            recordName = "HangingProtocol";
            break;
        case ERT_Stereometric:
            recordName = "Stereometric";
            break;
        case ERT_HL7StrucDoc:
            recordName = "HL7StrucDoc";
            break;
        case ERT_Palette:
            recordName = "Palette";
            break;
        case ERT_Surface:
            recordName = "Surface";
            break;
        case ERT_Measurement:
            recordName = "Measurement";
            break;
        case ERT_Implant:
            recordName = "Implant";
            break;
        case ERT_ImplantGroup:
            recordName = "ImplantGroup";
            break;
        case ERT_ImplantAssy:
            recordName = "ImplantAssy";
            break;
        case ERT_Plan:
            recordName = "Plan";
            break;
        case ERT_SurfaceScan:
            recordName = "SurfaceScan";
            break;
        case ERT_Tract:
            recordName = "Tract";
            break;
        case ERT_Assessment:
            recordName = "Assessment";
            break;
        default:
            recordName = "(unknown-directory-record-type)";
            break;
    }
    return recordName;
}


// get record type from SOP class
static E_DirRecType sopClassToRecordType(const OFString &sopClass)
{
    /* default: image SOP class */
    E_DirRecType result = ERT_Image;
    /* check whether any non-image SOP class */
    if (compare(sopClass, UID_RETIRED_StandaloneOverlayStorage))
        result = ERT_Overlay;
    else if (compare(sopClass, UID_RETIRED_StandaloneModalityLUTStorage))
        result = ERT_ModalityLut;
    else if (compare(sopClass, UID_RETIRED_StandaloneVOILUTStorage))
        result = ERT_VoiLut;
    else if (compare(sopClass, UID_RETIRED_StandaloneCurveStorage) ||
             compare(sopClass, UID_RETIRED_StandalonePETCurveStorage))
    {
        result = ERT_Curve;
    }
    else if (compare(sopClass, UID_BasicTextSRStorage) ||
             compare(sopClass, UID_EnhancedSRStorage) ||
             compare(sopClass, UID_ComprehensiveSRStorage) ||
             compare(sopClass, UID_Comprehensive3DSRStorage) ||
             compare(sopClass, UID_ExtensibleSRStorage) ||
             compare(sopClass, UID_ProcedureLogStorage) ||
             compare(sopClass, UID_MammographyCADSRStorage) ||
             compare(sopClass, UID_ChestCADSRStorage) ||
             compare(sopClass, UID_ColonCADSRStorage) ||
             compare(sopClass, UID_XRayRadiationDoseSRStorage) ||
             compare(sopClass, UID_RadiopharmaceuticalRadiationDoseSRStorage) ||
             compare(sopClass, UID_SpectaclePrescriptionReportStorage) ||
             compare(sopClass, UID_MacularGridThicknessAndVolumeReportStorage) ||
             compare(sopClass, UID_ImplantationPlanSRDocumentStorage) ||
             compare(sopClass, UID_AcquisitionContextSRStorage) ||
             compare(sopClass, UID_SimplifiedAdultEchoSRStorage) ||
             compare(sopClass, UID_PatientRadiationDoseSRStorage) ||
             compare(sopClass, UID_PerformedImagingAgentAdministrationSRStorage) ||
             compare(sopClass, UID_PlannedImagingAgentAdministrationSRStorage))
    {
        result = ERT_SRDocument;
    }
    else if (compare(sopClass, UID_GrayscaleSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_ColorSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_PseudoColorSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_BlendingSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_XAXRFGrayscaleSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_GrayscalePlanarMPRVolumetricPresentationStateStorage) ||
             compare(sopClass, UID_CompositingPlanarMPRVolumetricPresentationStateStorage) ||
             compare(sopClass, UID_AdvancedBlendingPresentationStateStorage) ||
             compare(sopClass, UID_VolumeRenderingVolumetricPresentationStateStorage) ||
             compare(sopClass, UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage) ||
             compare(sopClass, UID_MultipleVolumeRenderingVolumetricPresentationStateStorage) ||
             compare(sopClass, UID_BasicStructuredDisplayStorage))
    {
        result = ERT_Presentation;
    }
    else if (compare(sopClass, UID_TwelveLeadECGWaveformStorage) ||
             compare(sopClass, UID_GeneralECGWaveformStorage) ||
             compare(sopClass, UID_AmbulatoryECGWaveformStorage) ||
             compare(sopClass, UID_HemodynamicWaveformStorage) ||
             compare(sopClass, UID_CardiacElectrophysiologyWaveformStorage) ||
             compare(sopClass, UID_BasicVoiceAudioWaveformStorage) ||
             compare(sopClass, UID_GeneralAudioWaveformStorage) ||
             compare(sopClass, UID_ArterialPulseWaveformStorage) ||
             compare(sopClass, UID_RespiratoryWaveformStorage))
    {
        result = ERT_Waveform;
    }
    else if (compare(sopClass, UID_RTDoseStorage))
        result = ERT_RTDose;
    else if (compare(sopClass, UID_RTStructureSetStorage))
        result = ERT_RTStructureSet;
    else if (compare(sopClass, UID_RTPlanStorage) ||
             compare(sopClass, UID_RTIonPlanStorage))
    {
        result = ERT_RTPlan;
    }
    else if (compare(sopClass, UID_RTBeamsTreatmentRecordStorage) ||
             compare(sopClass, UID_RTBrachyTreatmentRecordStorage) ||
             compare(sopClass, UID_RTTreatmentSummaryRecordStorage) ||
             compare(sopClass, UID_RTIonBeamsTreatmentRecordStorage))
    {
        result = ERT_RTTreatRecord;
    }
    else if (compare(sopClass, UID_RETIRED_StoredPrintStorage))
        result = ERT_StoredPrint;
    else if (compare(sopClass, UID_KeyObjectSelectionDocumentStorage))
        result = ERT_KeyObjectDoc;
    else if (compare(sopClass, UID_SpatialRegistrationStorage) ||
             compare(sopClass, UID_DeformableSpatialRegistrationStorage))
    {
        result = ERT_Registration;
    }
    else if (compare(sopClass, UID_SpatialFiducialsStorage))
        result = ERT_Fiducial;
    else if (compare(sopClass, UID_RawDataStorage))
        result = ERT_RawData;
    else if (compare(sopClass, UID_MRSpectroscopyStorage))
        result = ERT_Spectroscopy;
    else if (compare(sopClass, UID_EncapsulatedPDFStorage) ||
             compare(sopClass, UID_EncapsulatedCDAStorage) ||
             compare(sopClass, UID_EncapsulatedSTLStorage))
    {
        result = ERT_EncapDoc;
    }
    else if (compare(sopClass, UID_RealWorldValueMappingStorage))
        result = ERT_ValueMap;
    else if (compare(sopClass, UID_HangingProtocolStorage))
        result = ERT_HangingProtocol;
    else if (compare(sopClass, UID_StereometricRelationshipStorage))
        result = ERT_Stereometric;
    else if (compare(sopClass, UID_ColorPaletteStorage))
        result = ERT_Palette;
    else if (compare(sopClass, UID_SurfaceSegmentationStorage))
        result = ERT_Surface;
    else if (compare(sopClass, UID_LensometryMeasurementsStorage) ||
             compare(sopClass, UID_AutorefractionMeasurementsStorage) ||
             compare(sopClass, UID_KeratometryMeasurementsStorage) ||
             compare(sopClass, UID_SubjectiveRefractionMeasurementsStorage) ||
             compare(sopClass, UID_VisualAcuityMeasurementsStorage) ||
             compare(sopClass, UID_OphthalmicAxialMeasurementsStorage) ||
             compare(sopClass, UID_IntraocularLensCalculationsStorage) ||
             compare(sopClass, UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage))
    {
        result = ERT_Measurement;
    }
    else if (compare(sopClass, UID_GenericImplantTemplateStorage))
        result = ERT_Implant;
    else if (compare(sopClass, UID_ImplantTemplateGroupStorage))
        result = ERT_ImplantGroup;
    else if (compare(sopClass, UID_ImplantAssemblyTemplateStorage))
        result = ERT_ImplantAssy;
    else if (compare(sopClass, UID_RTBeamsDeliveryInstructionStorage) ||
             compare(sopClass, UID_RTBrachyApplicationSetupDeliveryInstructionStorage))
    {
        result = ERT_Plan;
    }
    else if (compare(sopClass, UID_SurfaceScanMeshStorage) ||
             compare(sopClass, UID_SurfaceScanPointCloudStorage))
    {
        result = ERT_SurfaceScan;
    }
    else if (compare(sopClass, UID_TractographyResultsStorage))
        result = ERT_Tract;
    else if (compare(sopClass, UID_ContentAssessmentResultsStorage))
        result = ERT_Assessment;
    else if (compare(sopClass, UID_RTPhysicianIntentStorage) ||
             compare(sopClass, UID_RTSegmentAnnotationStorage))
    {
        result = ERT_Radiotherapy;
    }
    return result;
}


// get unique key for the given record type
static DcmTagKey getRecordUniqueKey(const E_DirRecType recordType)
{
    /* default for all other record types */
    DcmTagKey result = DCM_ReferencedSOPInstanceUIDInFile;
    /* special cases: patient, study, series */
    if (recordType == ERT_Patient)
        result = DCM_PatientID;
    else if (recordType == ERT_Study)
        result = DCM_StudyInstanceUID;
    else if (recordType == ERT_Series)
        result = DCM_SeriesInstanceUID;
    return result;
}


// copy content items which modify the concept name of the document root
static void addConceptModContentItems(DcmDirectoryRecord *record,
                                      DcmItem *dataset)
{
    /* Content Sequence, type 1C (see DICOM part 3)
       "Contains the Target Content Items that modify the Concept Name
        Code Sequence of the root Content Item (Document Title).
        Required if the root Content Item is the Source Content Item of
        HAS CONCEPT MOD relationships."
    */
    if ((record != NULL) && (dataset != NULL))
    {
        OFString tmpString;
        signed long i = 0;
        DcmItem *ditem = NULL;
        /* create new ContentSequence */
        DcmSequenceOfItems *newSeq = new DcmSequenceOfItems(DCM_ContentSequence);
        if (newSeq != NULL)
        {
            do {
                /* get sequence item (not very efficient, but it works) */
                if (dataset->findAndGetSequenceItem(DCM_ContentSequence, ditem, i++).good())
                {
                    /* check RelationshipType */
                    if (ditem->findAndGetOFString(DCM_RelationshipType, tmpString).good() &&
                        (tmpString.compare("HAS CONCEPT MOD") == 0))
                    {
                         /* copy content item */
                         DcmItem *newItem = new DcmItem(*ditem);
                         if (newItem != NULL)
                         {
                             if (newSeq->append(newItem).bad())
                                 delete newItem;
                         }
                    }
                }
            } while (ditem != NULL);
            /* try to insert content sequence into record (if not empty) */
            if ((newSeq->card() == 0) || (record->insert(newSeq, OFTrue /*replaceOld*/).bad()))
                delete newSeq;
        }
    }
}


// copy relevant attributes from the blending sequence
static void addBlendingSequence(DcmDirectoryRecord *record,
                                DcmItem *dataset)
{
    if ((record != NULL) && (dataset != NULL))
    {
        /* make sure that the sequence is really present in the original dataset */
        if (dataset->tagExistsWithValue(DCM_BlendingSequence))
        {
            signed long i = 0;
            DcmItem *ditem = NULL;
            /* create new BlendingSequence */
            DcmSequenceOfItems *newSeq = new DcmSequenceOfItems(DCM_BlendingSequence);
            if (newSeq != NULL)
            {
                do {
                    /* get sequence item (not very efficient, but it works) */
                    if (dataset->findAndGetSequenceItem(DCM_BlendingSequence, ditem, i++).good())
                    {
                        DcmItem *newItem = new DcmItem();
                        if (newItem != NULL)
                        {
                            if (newSeq->append(newItem).good())
                            {
                                ditem->findAndInsertCopyOfElement(DCM_StudyInstanceUID, newItem);
                                ditem->findAndInsertCopyOfElement(DCM_ReferencedSeriesSequence, newItem);
                            } else
                                delete newItem;
                        }
                    }
                } while ((ditem != NULL) && (i <= 2));  // terminate after two items
                /* try to insert blending sequence into record (if not empty) */
                if ((newSeq->card() == 0) || (record->insert(newSeq, OFTrue /*replaceOld*/).bad()))
                    delete newSeq;
            }
        }
    }
}


// insert child record into the parent's list based on the numeric value of the criterionKey
static OFCondition insertWithISCriterion(DcmDirectoryRecord *parent,
                                         DcmDirectoryRecord *child,
                                         const DcmTagKey &criterionKey)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((parent != NULL) && (child != NULL))
    {
        OFBool found = OFFalse;
        Sint32 childNumber = 0;
        Sint32 parentNumber = 0;
        /* retrieve numeric value */
        result = child->findAndGetSint32(criterionKey, childNumber);
        /* if available search for proper position */
        if (result.good())
        {
            DcmDirectoryRecord *record = NULL;
            /* iterate over all records in the parent list */
            while (!found && ((record = parent->nextSub(record)) != NULL))
            {
                /* check for proper position */
                if (record->findAndGetSint32(criterionKey, parentNumber).good() && (parentNumber > childNumber))
                    found = OFTrue;
            }
        }
        /* insert child record at determined position */
        if (found)
            result = parent->insertSubAtCurrentPos(child, OFTrue /*before*/);
        else /* or append at the end of the list */
            result = parent->insertSub(child);
    }
    return result;
}


// insert child record sorted under the parent record
static OFCondition insertSortedUnder(DcmDirectoryRecord *parent,
                                     DcmDirectoryRecord *child)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((parent != NULL) && (child != NULL))
    {
        switch (child->getRecordType())
        {
            case ERT_Image:
                /* try to insert based on Image/InstanceNumber */
                result = insertWithISCriterion(parent, child, DCM_InstanceNumber);
                break;
            case ERT_Overlay:
                /* try to insert based on OverlayNumber */
                result = insertWithISCriterion(parent, child, DCM_RETIRED_OverlayNumber);
                break;
            case ERT_Curve:
                /* try to insert based on CurveNumber */
                result = insertWithISCriterion(parent, child, DCM_RETIRED_CurveNumber);
                break;
            case ERT_ModalityLut:
            case ERT_VoiLut:
                /* try to insert based on LUTNumber */
                result = insertWithISCriterion(parent, child, DCM_RETIRED_LUTNumber);
                break;
            case ERT_SRDocument:
            case ERT_Presentation:
            case ERT_Waveform:
            case ERT_RTDose:
            case ERT_RTStructureSet:
            case ERT_RTPlan:
            case ERT_RTTreatRecord:
            case ERT_StoredPrint:
            case ERT_KeyObjectDoc:
            case ERT_Registration:
            case ERT_Fiducial:
            case ERT_RawData:
            case ERT_Spectroscopy:
            case ERT_EncapDoc:
            case ERT_ValueMap:
            case ERT_Surface:
            case ERT_Measurement:
            case ERT_SurfaceScan:
            case ERT_Tract:
            case ERT_Assessment:
            case ERT_Radiotherapy:
                /* try to insert based on InstanceNumber */
                result = insertWithISCriterion(parent, child, DCM_InstanceNumber);
                break;
            case ERT_Series:
                /* try to insert based on SeriesNumber */
                result = insertWithISCriterion(parent, child, DCM_SeriesNumber);
                break;
            case ERT_Stereometric:
            case ERT_Plan:
                /* no InstanceNumber or the like */
            default:
                /* append */
                result = parent->insertSub(child);
                break;
        }
    }
    return result;
}


// create alternative study date if absent in dataset
static OFString &alternativeStudyDate(DcmItem *dataset,
                                      OFString &result)
{
    if (dataset != NULL)
    {
        /* use another date if present */
        if (dataset->findAndGetOFStringArray(DCM_SeriesDate, result).bad() || result.empty())
        {
            if (dataset->findAndGetOFStringArray(DCM_AcquisitionDate, result).bad() || result.empty())
            {
                if (dataset->findAndGetOFStringArray(DCM_ContentDate, result).bad() || result.empty())
                {
                    /* use current date, "19000101" in case of error */
                    DcmDate::getCurrentDate(result);
                }
            }
        }
    } else
        result.clear();
    return result;
}


// create alternative study time if absent in dataset
static OFString &alternativeStudyTime(DcmItem *dataset,
                                      OFString &result)
{
    if (dataset != NULL)
    {
        /* use another time if present */
        if (dataset->findAndGetOFStringArray(DCM_SeriesTime, result).bad() || result.empty())
        {
            if (dataset->findAndGetOFStringArray(DCM_AcquisitionTime, result).bad() || result.empty())
            {
                if (dataset->findAndGetOFStringArray(DCM_ContentTime, result).bad() || result.empty())
                {
                    /* use current time, "0000" in case of error */
                    DcmTime::getCurrentTime(result);
                }
            }
        }
    } else
        result.clear();
    return result;
}


// check whether given SOP Class UID belongs to a multi-frame composite IOD
// (not sure whether all these IODs should really be treated as "movies"?)
static OFBool isMultiframeStorageSOPClass(const OFString &sopClassUID)
{
    return compare(sopClassUID, UID_BreastProjectionXRayImageStorageForPresentation) ||
           compare(sopClassUID, UID_BreastProjectionXRayImageStorageForProcessing) ||
           compare(sopClassUID, UID_BreastTomosynthesisImageStorage) ||
           compare(sopClassUID, UID_EnhancedCTImageStorage) ||
           compare(sopClassUID, UID_EnhancedMRColorImageStorage) ||
           compare(sopClassUID, UID_EnhancedMRImageStorage) ||
           compare(sopClassUID, UID_EnhancedPETImageStorage) ||
           compare(sopClassUID, UID_EnhancedUSVolumeStorage) ||
           compare(sopClassUID, UID_EnhancedXAImageStorage) ||
           compare(sopClassUID, UID_EnhancedXRFImageStorage) ||
           compare(sopClassUID, UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation) ||
           compare(sopClassUID, UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing) ||
           compare(sopClassUID, UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage) ||
           compare(sopClassUID, UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage) ||
           compare(sopClassUID, UID_MultiframeSingleBitSecondaryCaptureImageStorage) ||
           compare(sopClassUID, UID_MultiframeTrueColorSecondaryCaptureImageStorage) ||
           compare(sopClassUID, UID_NuclearMedicineImageStorage) ||
           compare(sopClassUID, UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage) ||
           compare(sopClassUID, UID_OphthalmicPhotography16BitImageStorage) ||
           compare(sopClassUID, UID_OphthalmicPhotography8BitImageStorage) ||
           compare(sopClassUID, UID_OphthalmicTomographyImageStorage) ||
           compare(sopClassUID, UID_ParametricMapStorage) ||
           compare(sopClassUID, UID_RTDoseStorage) ||
           compare(sopClassUID, UID_RTImageStorage) ||
           compare(sopClassUID, UID_UltrasoundMultiframeImageStorage) ||
           compare(sopClassUID, UID_VideoEndoscopicImageStorage) ||
           compare(sopClassUID, UID_VideoMicroscopicImageStorage) ||
           compare(sopClassUID, UID_VideoPhotographicImageStorage) ||
// in fact, the following IOD is a multi-frame image but the individual frames are rather "tiles"
//         compare(sopClassUID, UID_VLWholeSlideMicroscopyImageStorage) ||
           compare(sopClassUID, UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage) ||
           compare(sopClassUID, UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage) ||
           compare(sopClassUID, UID_XRay3DAngiographicImageStorage) ||
           compare(sopClassUID, UID_XRay3DCraniofacialImageStorage) ||
           compare(sopClassUID, UID_XRayAngiographicImageStorage) ||
           compare(sopClassUID, UID_XRayRadiofluoroscopicImageStorage);
}


/*------------------*
 *  implementation  *
 *------------------*/

// constructor
DicomDirInterface::DicomDirInterface()
  : DicomDir(NULL),
    ImagePlugin(NULL),
    ApplicationProfile(AP_Default),
    BackupMode(OFTrue),
    AbortMode(OFFalse),
    MapFilenamesMode(OFFalse),
    InventMode(OFFalse),
    InventPatientIDMode(OFFalse),
    RetiredSOPClassSupport(OFFalse),
    EncodingCheck(OFTrue),
    ResolutionCheck(OFTrue),
    TransferSyntaxCheck(OFTrue),
    FileFormatCheck(OFTrue),
    ConsistencyCheck(OFTrue),
    IconImageMode(OFFalse),
    FilesetUpdateMode(OFFalse),
    BackupFilename(),
    BackupCreated(OFFalse),
    IconSize(64),
    IconPrefix(),
    DefaultIcon(),
    RLESupport(OFFalse),
    JPEGSupport(OFFalse),
    JP2KSupport(OFFalse),
    AutoPatientNumber(0),
    AutoStudyNumber(0),
    AutoSeriesNumber(0),
    AutoInstanceNumber(1),
    AutoOverlayNumber(1),
    AutoLutNumber(1),
    AutoCurveNumber(1)
{
    /* check whether (possibly required) RLE/JPEG/JP2K decoders are registered */
    RLESupport  = DcmCodecList::canChangeCoding(EXS_RLELossless, EXS_LittleEndianExplicit);
    JPEGSupport = DcmCodecList::canChangeCoding(EXS_JPEGProcess1, EXS_LittleEndianExplicit) &&
                  DcmCodecList::canChangeCoding(EXS_JPEGProcess14SV1, EXS_LittleEndianExplicit);
    JP2KSupport = DcmCodecList::canChangeCoding(EXS_JPEG2000LosslessOnly, EXS_LittleEndianExplicit) &&
                  DcmCodecList::canChangeCoding(EXS_JPEG2000, EXS_LittleEndianExplicit);
}


// destructor
DicomDirInterface::~DicomDirInterface()
{
    /* reset object to its initial state (free memory) */
    cleanup();
}


// cleanup the object, i.e. free all memory
void DicomDirInterface::cleanup()
{
    /* free all allocated memory */
    delete DicomDir;
    /* invalidate references */
    DicomDir = NULL;
}


// check whether the current DICOMDIR object is valid
OFBool DicomDirInterface::isDicomDirValid() const
{
    /* tbd: might add more sophisticated checks later on, e.g. some application
     * profiles do not not allow for DICOMDIRs with no directory information.
     */
    return (DicomDir != NULL);
}


// create a backup of the specified file
void DicomDirInterface::createDicomDirBackup(const OFFilename &filename)
{
    /* check whether DICOMDIR already exists */
    if (OFStandard::fileExists(filename))
    {
        /* rename existing DICOMDIR */
        OFStandard::appendFilenameExtension(BackupFilename, filename, FNAME_BACKUP_EXTENSION);
        /* delete old backup file (if any) */
        deleteDicomDirBackup();
        DCMDATA_INFO("creating DICOMDIR backup: " << BackupFilename);
        /* create backup file */
        if (copyFile(filename, BackupFilename))
            BackupCreated = OFTrue;
        else
            DCMDATA_ERROR("cannot create backup of: " << filename);
    }
}


// delete backup file if one has been created
void DicomDirInterface::deleteDicomDirBackup()
{
    /* if a backup of the DICOMDIR exists */
    if (OFStandard::fileExists(BackupFilename))
    {
        if (BackupCreated)
            DCMDATA_INFO("deleting DICOMDIR backup: " << BackupFilename);
        else
            DCMDATA_INFO("deleting old DICOMDIR backup: " << BackupFilename);
        /* delete the backup file */
        OFStandard::deleteFile(BackupFilename);
    }
    /* reset status variable */
    BackupCreated = OFFalse;
}


// create a new DICOMDIR object, i.e. replace any previously existing 'filename'
OFCondition DicomDirInterface::createNewDicomDir(const E_ApplicationProfile profile,
                                                 const OFFilename &filename,
                                                 const OFString &filesetID)
{
    OFCondition result = EC_IllegalParameter;
    if (!filename.isEmpty() && checkFilesetID(filesetID))
    {
        FilesetUpdateMode = OFFalse;
        /* first remove any existing DICOMDIR from memory */
        cleanup();
        /* then create a backup if a DICOMDIR file already exists */
        if (OFStandard::fileExists(filename))
        {
            if (BackupMode)
                createDicomDirBackup(filename);
            /* and delete it because otherwise DcmDicomDir will parse it
               and try to append to existing records */
            OFStandard::deleteFile(filename);
        }
        /* select new application profile */
        result = selectApplicationProfile(profile);
        if (result.good())
        {
            DCMDATA_INFO("creating DICOMDIR file using " << getProfileName(ApplicationProfile)
                << " profile: " << filename);
            /* finally, create a new DICOMDIR object */
            DicomDir = new DcmDicomDir(filename, filesetID.c_str());
            if (DicomDir != NULL)
                result = DicomDir->error();
            else
                result = EC_MemoryExhausted;
        }
    }
    return result;
}


// create a DICOMDIR based on an existing one, i.e. append the new entries
OFCondition DicomDirInterface::appendToDicomDir(const E_ApplicationProfile profile,
                                                const OFFilename &filename)
{
    OFCondition result = EC_IllegalParameter;
    if (!filename.isEmpty())
    {
        FilesetUpdateMode = OFFalse;
        /* first remove any existing DICOMDIR from memory */
        cleanup();
        /* then check whether DICOMDIR file already exists */
        if (OFStandard::fileExists(filename))
        {
            /* then create a backup if required */
            if (BackupMode)
                createDicomDirBackup(filename);
            /* select new application profile */
            result = selectApplicationProfile(profile);
            if (result.good())
            {
                DCMDATA_INFO("appending to DICOMDIR file using " << getProfileName(ApplicationProfile)
                    << " profile: " << filename);
                /* finally, create a DICOMDIR object based on the existing file */
                DicomDir = new DcmDicomDir(filename);
                if (DicomDir != NULL)
                    result = DicomDir->error();
                else
                    result = EC_MemoryExhausted;
            }
        } else {
            /* create error message "No such file or directory" from error code */
            char buffer[255];
            const char *text = OFStandard::strerror(ENOENT, buffer, 255);
            if ((text == NULL) || (text[0] == '\0'))
                text = "(unknown error code)";
            /*  error code 18 is reserved for file read error messages (see dcerror.cc) */
            result = makeOFCondition(OFM_dcmdata, 18, OF_error, text);
            /* report an error */
            DCMDATA_ERROR(result.text() << ": cannot append to file: " << filename);
        }
    }
    return result;
}


// create a DICOMDIR based on an existing one, i.e. append new and update existing entries
OFCondition DicomDirInterface::updateDicomDir(const E_ApplicationProfile profile,
                                              const OFFilename &filename)
{
    OFCondition result = EC_IllegalParameter;
    if (!filename.isEmpty())
    {
        FilesetUpdateMode = OFTrue;
        /* first remove any existing DICOMDIR from memory */
        cleanup();
        /* then check whether DICOMDIR file already exists */
        if (OFStandard::fileExists(filename))
        {
            /* then create a backup if required */
            if (BackupMode)
                createDicomDirBackup(filename);
            /* select new application profile */
            result = selectApplicationProfile(profile);
            if (result.good())
            {
                DCMDATA_INFO("updating DICOMDIR file using " << getProfileName(ApplicationProfile)
                    << " profile: " << filename);
                /* finally, create a DICOMDIR object based on the existing file */
                DicomDir = new DcmDicomDir(filename);
                if (DicomDir != NULL)
                    result = DicomDir->error();
                else
                    result = EC_MemoryExhausted;
            }
        } else {
            /* create error message "No such file or directory" from error code */
            char buffer[255];
            const char *text = OFStandard::strerror(ENOENT, buffer, 255);
            if ((text == NULL) || (text[0] == '\0'))
                text = "(unknown error code)";
            /*  error code 18 is reserved for file read error messages (see dcerror.cc) */
            result = makeOFCondition(OFM_dcmdata, 18, OF_error, text);
            /* report an error */
            DCMDATA_ERROR(result.text() << ": cannot update file: " << filename);
        }
    }
    return result;
}


// write the current DICOMDIR object to file
OFCondition DicomDirInterface::writeDicomDir(const E_EncodingType encodingType,
                                             const E_GrpLenEncoding groupLength)
{
    OFCondition result = EC_InvalidDICOMDIR;
    /* check whether DICOMDIR object is valid */
    if (isDicomDirValid())
    {
        DCMDATA_INFO("writing file: " << DicomDir->getDirFileName());
        /* write DICOMDIR as Little Endian Explicit as required by the standard */
        result = DicomDir->write(DICOMDIR_DEFAULT_TRANSFERSYNTAX, encodingType, groupLength);
        /* delete backup copy in case the new file could be written without any errors */
        if (result.good())
            deleteDicomDirBackup();
        else {
            /* report an error */
            DCMDATA_ERROR(result.text() << ": writing file: " << DicomDir->getDirFileName());
        }
    }
    return result;
}


// check whether the specified filename conforms to the DICOM standard requirements
OFBool DicomDirInterface::isFilenameValid(const OFFilename &filename,
                                          const OFBool allowEmpty)
{
    OFBool result = OFTrue;
    /* get 8-bit version of the stored string */
    const char *fname = filename.getCharPointer();
    /* check for empty filename */
    if ((fname == NULL) || (fname[0] == '\0'))
    {
        if (!allowEmpty)
        {
            DCMDATA_ERROR("<empty string> not allowed as filename");
            result = OFFalse;
        }
    } else {
        size_t invalidChar = 0;
        /* check whether the file name path is ok and in local format */
        if ((fname[0] == PATH_SEPARATOR) /* absolute path? */ ||
            locateInvalidFilenameChars(fname, invalidChar, MapFilenamesMode))
        {
            DCMDATA_ERROR("invalid character(s) in filename: " << fname << OFendl
                << OFString(34 /*message*/ + invalidChar, ' ') << "^");
            result = OFFalse;
        }
        /* ensure that the maximum number of components is not being exceeded */
        if (componentCount(fname) > MAX_FNAME_COMPONENTS)
        {
            DCMDATA_ERROR("too many path components (max " << MAX_FNAME_COMPONENTS
                << ") in filename: " << fname);
            result = OFFalse;
        }
        /* ensure that each component is not too large */
        if (isComponentTooLarge(fname, MAX_FNAME_COMPONENT_SIZE, MapFilenamesMode))
        {
            /* create error message */
            DCMDATA_ERROR("component too large (max " << MAX_FNAME_COMPONENT_SIZE
                << " characters) in filename: " << fname);
            result = OFFalse;
        }
    }
    return result;
}


// check whether the specified character set is defined in the DICOM standard
OFBool DicomDirInterface::isCharsetValid(const char *charset)
{
    OFBool result = OFTrue;
    /* empty charset is also valid */
    if ((charset != NULL) && (charset[0] != '\0'))
    {
        /* check for valid charset */
        result = (strcmp(charset, "ISO_IR 100") == 0) ||
                 (strcmp(charset, "ISO_IR 101") == 0) ||
                 (strcmp(charset, "ISO_IR 109") == 0) ||
                 (strcmp(charset, "ISO_IR 110") == 0) ||
                 (strcmp(charset, "ISO_IR 144") == 0) ||
                 (strcmp(charset, "ISO_IR 127") == 0) ||
                 (strcmp(charset, "ISO_IR 126") == 0) ||
                 (strcmp(charset, "ISO_IR 138") == 0) ||
                 (strcmp(charset, "ISO_IR 148") == 0) ||
                 (strcmp(charset, "ISO_IR 166") == 0) ||
                 (strcmp(charset, "ISO_IR 13")  == 0) ||
                 (strcmp(charset, "ISO_IR 192") == 0);
        if (!result)
            DCMDATA_ERROR("unknown character set for file-set descriptor: " << charset);
    }
    return result;
}


// check the given file (dataset) regarding SOP class and transfer syntax
OFCondition DicomDirInterface::checkSOPClassAndXfer(DcmMetaInfo *metainfo,
                                                    DcmItem *dataset,
                                                    const OFFilename &filename)
{
    OFCondition result = EC_IllegalParameter;
    if ((metainfo != NULL) && (dataset != NULL) && !filename.isEmpty())
    {
        /* is sop class ok? */
        OFString mediaSOPClassUID;
        if (metainfo->findAndGetOFStringArray(DCM_MediaStorageSOPClassUID, mediaSOPClassUID).bad())
        {
            DCMDATA_ERROR("MediaStorageSOPClassUID missing in file meta information: " << filename);
            result = EC_TagNotFound;
        } else {
            /* check if the SOP Class is a known storage SOP class (an image, overlay, curve, etc.) */
            OFBool found = OFFalse;
            OFString expectedTransferSyntax = UID_LittleEndianExplicitTransferSyntax;
            switch (ApplicationProfile)
            {
                case AP_MPEG2MPatMLDVD:
                    expectedTransferSyntax = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
                    /* multi-frame composite IODs only! */
                    found = isMultiframeStorageSOPClass(mediaSOPClassUID);
                    break;
                case AP_BasicCardiac:
                    if (compare(mediaSOPClassUID, UID_XRayAngiographicImageStorage))
                    {
                        expectedTransferSyntax = UID_JPEGProcess14SV1TransferSyntax;
                        found = OFTrue;
                    }
                    else if (RetiredSOPClassSupport)
                    {
                        /* the following SOP class has been retired with DICOM 2006: */
                        found = compare(mediaSOPClassUID, UID_RETIRED_DetachedPatientManagementSOPClass);
                    }
                    break;
                case AP_XrayAngiographic:
                case AP_XrayAngiographicDVD:
                    if (compare(mediaSOPClassUID, UID_XRayAngiographicImageStorage))
                    {
                        /* JPEG lossy compression only allowed for DVD media (checked later) */
                        if (ApplicationProfile == AP_XrayAngiographic)
                            expectedTransferSyntax = UID_JPEGProcess14SV1TransferSyntax;
                        found = OFTrue;
                    } else {
                        found = compare(mediaSOPClassUID, UID_SecondaryCaptureImageStorage) ||
                                compare(mediaSOPClassUID, UID_GrayscaleSoftcopyPresentationStateStorage);
                        if (!found && RetiredSOPClassSupport)
                        {
                            /* the following SOP classes have been retired with DICOM 2004: */
                            found = compare(mediaSOPClassUID, UID_RETIRED_StandaloneOverlayStorage) ||
                                    compare(mediaSOPClassUID, UID_RETIRED_StandaloneCurveStorage) ||
                                    compare(mediaSOPClassUID, UID_RETIRED_DetachedPatientManagementSOPClass);
                        }
                    }
                    break;
                case AP_DentalRadiograph:
                    found = compare(mediaSOPClassUID, UID_DigitalIntraOralXRayImageStorageForPresentation) ||
                            compare(mediaSOPClassUID, UID_DigitalXRayImageStorageForPresentation) ||
                            compare(mediaSOPClassUID, UID_BasicStructuredDisplayStorage) ||
                            compare(mediaSOPClassUID, UID_GrayscaleSoftcopyPresentationStateStorage);
                    break;
                case AP_CTandMR:
                    /* transfer syntax needs to be checked later */
                    found = compare(mediaSOPClassUID, UID_CTImageStorage) ||
                            compare(mediaSOPClassUID, UID_MRImageStorage) ||
                            compare(mediaSOPClassUID, UID_SecondaryCaptureImageStorage) ||
                            /* the following SOP classes have been added with CP-1182 */
                            compare(mediaSOPClassUID, UID_GrayscaleSoftcopyPresentationStateStorage) ||
                            compare(mediaSOPClassUID, UID_XRayRadiationDoseSRStorage);
                    if (!found && RetiredSOPClassSupport)
                    {
                        /* the following SOP class has been retired with DICOM 2004: */
                        found = compare(mediaSOPClassUID, UID_RETIRED_DetachedPatientManagementSOPClass);
                    }
                    break;
                case AP_UltrasoundIDSF:
                case AP_UltrasoundSCSF:
                case AP_UltrasoundCCSF:
                    /* transfer syntax needs to be checked later */
                    found = compare(mediaSOPClassUID, UID_UltrasoundImageStorage);
                    break;
                case AP_UltrasoundIDMF:
                case AP_UltrasoundSCMF:
                case AP_UltrasoundCCMF:
                    /* transfer syntax needs to be checked later */
                    found = compare(mediaSOPClassUID, UID_UltrasoundImageStorage) ||
                            compare(mediaSOPClassUID, UID_UltrasoundMultiframeImageStorage);
                    break;
                case AP_TwelveLeadECG:
                    found = compare(mediaSOPClassUID, UID_TwelveLeadECGWaveformStorage);
                    break;
                case AP_HemodynamicWaveform:
                    found = compare(mediaSOPClassUID, UID_HemodynamicWaveformStorage);
                    break;
                case AP_GeneralPurpose:
                case AP_GeneralPurposeDVDJPEG:
                case AP_GeneralPurposeDVDJPEG2000:
                case AP_GeneralPurposeBDJPEG:
                case AP_GeneralPurposeBDJPEG2000:
                case AP_GeneralPurposeBDMPEG2MPatML:
                case AP_GeneralPurposeBDMPEG2MPatHL:
                case AP_GeneralPurposeBDMPEG4HPatLV41:
                case AP_GeneralPurposeBDMPEG4HPatLV41BD:
                case AP_GeneralPurposeBDMPEG4HPatLV42_2D:
                case AP_GeneralPurposeBDMPEG4HPatLV42_3D:
                case AP_GeneralPurposeBDMPEG4StereoHPatLV42:
                case AP_USBandFlashJPEG:
                case AP_USBandFlashJPEG2000:
                case AP_GeneralPurposeMIME:
                default:
                {
                    /* specify expected transfer syntax for the BD profiles (multi-frame IODs only) */
                    if (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatML)
                        expectedTransferSyntax = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
                    else if (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatHL)
                        expectedTransferSyntax = UID_MPEG2MainProfileAtHighLevelTransferSyntax;
                    else if (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41)
                        expectedTransferSyntax = UID_MPEG4HighProfileLevel4_1TransferSyntax;
                    else if (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41BD)
                        expectedTransferSyntax = UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax;
                    else if (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_2D)
                        expectedTransferSyntax = UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax;
                    else if (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_3D)
                        expectedTransferSyntax = UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax;
                    else if (ApplicationProfile == AP_GeneralPurposeBDMPEG4StereoHPatLV42)
                        expectedTransferSyntax = UID_MPEG4StereoHighProfileLevel4_2TransferSyntax;
                    /* is it an image ? */
                    for (int i = 0; i < numberOfDcmImageSOPClassUIDs && !found; i++)
                        found = compare(mediaSOPClassUID, dcmImageSOPClassUIDs[i]);
                    /* is it one of the RT SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_RTDoseStorage) ||
                                compare(mediaSOPClassUID, UID_RTStructureSetStorage) ||
                                compare(mediaSOPClassUID, UID_RTPlanStorage) ||
                                compare(mediaSOPClassUID, UID_RTTreatmentSummaryRecordStorage) ||
                                compare(mediaSOPClassUID, UID_RTBeamsTreatmentRecordStorage) ||
                                compare(mediaSOPClassUID, UID_RTBeamsDeliveryInstructionStorage) ||
                                compare(mediaSOPClassUID, UID_RTBrachyTreatmentRecordStorage) ||
                                compare(mediaSOPClassUID, UID_RTBrachyApplicationSetupDeliveryInstructionStorage) ||
                                compare(mediaSOPClassUID, UID_RTIonPlanStorage) ||
                                compare(mediaSOPClassUID, UID_RTIonBeamsTreatmentRecordStorage) ||
                                compare(mediaSOPClassUID, UID_RTPhysicianIntentStorage) ||
                                compare(mediaSOPClassUID, UID_RTSegmentAnnotationStorage);
                    }
                    /* is it one of the structured reporting SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_BasicTextSRStorage) ||
                                compare(mediaSOPClassUID, UID_EnhancedSRStorage) ||
                                compare(mediaSOPClassUID, UID_ComprehensiveSRStorage) ||
                                compare(mediaSOPClassUID, UID_Comprehensive3DSRStorage) ||
                                compare(mediaSOPClassUID, UID_ExtensibleSRStorage) ||
                                compare(mediaSOPClassUID, UID_ProcedureLogStorage) ||
                                compare(mediaSOPClassUID, UID_MammographyCADSRStorage) ||
                                compare(mediaSOPClassUID, UID_ChestCADSRStorage) ||
                                compare(mediaSOPClassUID, UID_ColonCADSRStorage) ||
                                compare(mediaSOPClassUID, UID_XRayRadiationDoseSRStorage) ||
                                compare(mediaSOPClassUID, UID_RadiopharmaceuticalRadiationDoseSRStorage) ||
                                compare(mediaSOPClassUID, UID_SpectaclePrescriptionReportStorage) ||
                                compare(mediaSOPClassUID, UID_MacularGridThicknessAndVolumeReportStorage) ||
                                compare(mediaSOPClassUID, UID_ImplantationPlanSRDocumentStorage) ||
                                compare(mediaSOPClassUID, UID_AcquisitionContextSRStorage) ||
                                compare(mediaSOPClassUID, UID_SimplifiedAdultEchoSRStorage) ||
                                compare(mediaSOPClassUID, UID_PatientRadiationDoseSRStorage) ||
                                compare(mediaSOPClassUID, UID_PerformedImagingAgentAdministrationSRStorage) ||
                                compare(mediaSOPClassUID, UID_PlannedImagingAgentAdministrationSRStorage);
                     }
                    /* is it one of the waveform SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_TwelveLeadECGWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_GeneralECGWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_AmbulatoryECGWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_HemodynamicWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_CardiacElectrophysiologyWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_BasicVoiceAudioWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_GeneralAudioWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_ArterialPulseWaveformStorage) ||
                                compare(mediaSOPClassUID, UID_RespiratoryWaveformStorage);
                    }
                    /* is it one of the presentation state SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_GrayscaleSoftcopyPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_ColorSoftcopyPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_PseudoColorSoftcopyPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_BlendingSoftcopyPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_XAXRFGrayscaleSoftcopyPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_GrayscalePlanarMPRVolumetricPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_CompositingPlanarMPRVolumetricPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_AdvancedBlendingPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_VolumeRenderingVolumetricPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage) ||
                                compare(mediaSOPClassUID, UID_MultipleVolumeRenderingVolumetricPresentationStateStorage);
                    }
                    /* is it one of the encapsulated document SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_EncapsulatedPDFStorage) ||
                                compare(mediaSOPClassUID, UID_EncapsulatedCDAStorage) ||
                                compare(mediaSOPClassUID, UID_EncapsulatedSTLStorage);
                    }
                    /* is it one of the spatial registration SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_SpatialRegistrationStorage) ||
                                compare(mediaSOPClassUID, UID_SpatialFiducialsStorage) ||
                                compare(mediaSOPClassUID, UID_DeformableSpatialRegistrationStorage);
                    }
                    /* is it one of the segmentation SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_SegmentationStorage) ||  // will be mapped to IMAGE record
                                compare(mediaSOPClassUID, UID_SurfaceSegmentationStorage);
                    }
                    /* is it one of the measurement SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_LensometryMeasurementsStorage) ||
                                compare(mediaSOPClassUID, UID_AutorefractionMeasurementsStorage) ||
                                compare(mediaSOPClassUID, UID_KeratometryMeasurementsStorage) ||
                                compare(mediaSOPClassUID, UID_SubjectiveRefractionMeasurementsStorage) ||
                                compare(mediaSOPClassUID, UID_VisualAcuityMeasurementsStorage) ||
                                compare(mediaSOPClassUID, UID_OphthalmicAxialMeasurementsStorage) ||
                                compare(mediaSOPClassUID, UID_IntraocularLensCalculationsStorage) ||
                                compare(mediaSOPClassUID, UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage);
                    }
                    /* is it one of the implant SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_GenericImplantTemplateStorage) ||
                                compare(mediaSOPClassUID, UID_ImplantAssemblyTemplateStorage) ||
                                compare(mediaSOPClassUID, UID_ImplantTemplateGroupStorage);
                    }
                    /* is it one of the surface scan SOP Classes? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_SurfaceScanMeshStorage) ||
                                compare(mediaSOPClassUID, UID_SurfaceScanPointCloudStorage);
                    }
                    /* is it any other SOP class? */
                    if (!found)
                    {
                        found = compare(mediaSOPClassUID, UID_KeyObjectSelectionDocumentStorage) ||
                                compare(mediaSOPClassUID, UID_RawDataStorage) ||
                                compare(mediaSOPClassUID, UID_MRSpectroscopyStorage) ||
                                compare(mediaSOPClassUID, UID_RealWorldValueMappingStorage) ||
                                compare(mediaSOPClassUID, UID_HangingProtocolStorage) ||
                                compare(mediaSOPClassUID, UID_BasicStructuredDisplayStorage) ||
                                compare(mediaSOPClassUID, UID_StereometricRelationshipStorage) ||
                                compare(mediaSOPClassUID, UID_ColorPaletteStorage) ||
                                compare(mediaSOPClassUID, UID_TractographyResultsStorage) ||
                                compare(mediaSOPClassUID, UID_ContentAssessmentResultsStorage);
                    }
                    /* the following SOP classes have been retired with previous editions of the DICOM standard */
                    if (!found && RetiredSOPClassSupport)
                    {
                        found = compare(mediaSOPClassUID, UID_RETIRED_StoredPrintStorage) ||
                                compare(mediaSOPClassUID, UID_RETIRED_StandaloneOverlayStorage) ||
                                compare(mediaSOPClassUID, UID_RETIRED_StandaloneCurveStorage) ||
                                compare(mediaSOPClassUID, UID_RETIRED_StandaloneModalityLUTStorage) ||
                                compare(mediaSOPClassUID, UID_RETIRED_StandaloneVOILUTStorage) ||
                                compare(mediaSOPClassUID, UID_RETIRED_StandalonePETCurveStorage);
                        if (!found && (ApplicationProfile == AP_GeneralPurpose))
                        {
                            /* a detached patient mgmt sop class is also ok */
                            found = compare(mediaSOPClassUID, UID_RETIRED_DetachedPatientManagementSOPClass);
                        }
                    }
                }
            }
            if (found)
                result = EC_Normal;
            else
            {
                OFString sopClassName = dcmFindNameOfUID(mediaSOPClassUID.c_str(), "");
                if (sopClassName.empty())
                    sopClassName = mediaSOPClassUID;
                DCMDATA_ERROR("invalid SOP class (" << sopClassName << ") for " << getProfileName(ApplicationProfile)
                     << " profile: " << filename);
                result = EC_ApplicationProfileViolated;
            }
            if (result.good())
            {
                /* is transfer syntax ok? */
                OFString transferSyntax;
                if (metainfo->findAndGetOFStringArray(DCM_TransferSyntaxUID, transferSyntax).bad())
                {
                    DCMDATA_ERROR("TransferSyntaxUID missing in file meta information: " << filename);
                    result = EC_TagNotFound;
                }
                /* is transfer syntax supported */
                if (result.good())
                {
                    /* RLE compression */
                    if (compare(transferSyntax, UID_RLELosslessTransferSyntax))
                    {
                        if (!RLESupport && IconImageMode)
                        {
                            DCMDATA_ERROR("RLE compression not supported: " << filename);
                            result = EC_CannotChangeRepresentation;
                        }
                    }
                    /* JPEG compression */
                    if (compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax) ||
                        compare(transferSyntax, UID_JPEGProcess1TransferSyntax))
                    {
                        if (!JPEGSupport && IconImageMode)
                        {
                            DCMDATA_ERROR("JPEG compression not supported: " << filename);
                            result = EC_CannotChangeRepresentation;
                        }
                    }
                    /* JPEG 2000 compression */
                    if (compare(transferSyntax, UID_JPEG2000LosslessOnlyTransferSyntax) ||
                        compare(transferSyntax, UID_JPEG2000TransferSyntax))
                    {
                        if (!JP2KSupport && IconImageMode)
                        {
                            DCMDATA_ERROR("JPEG 2000 compression not supported: " << filename);
                            result = EC_CannotChangeRepresentation;
                        }
                    }
                }
                /* compare expected and actual transfer syntax */
                if (result.good())
                {
                    const OFString xferName = dcmFindNameOfUID(transferSyntax.c_str(), "");
                    switch (ApplicationProfile)
                    {
                        case AP_GeneralPurposeMIME:
                            /* accept all transfer syntaxes */
                            break;
                        case AP_GeneralPurposeDVDJPEG:
                        case AP_GeneralPurposeBDJPEG:
                        case AP_USBandFlashJPEG:
                            /* need to check multiple transfer syntaxes */
                            found = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess1TransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess2_4TransferSyntax);
                            if (!found)
                            {
                                const OFString expXferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax, "");
                                const OFString expXferName2 = dcmFindNameOfUID(UID_JPEGProcess14SV1TransferSyntax, "");
                                const OFString expXferName3 = dcmFindNameOfUID(UID_JPEGProcess1TransferSyntax, "");
                                const OFString expXferName4 = dcmFindNameOfUID(UID_JPEGProcess2_4TransferSyntax, "");
                                /* create error message */
                                OFOStringStream oss;
                                oss << expXferName1 << ", " << expXferName2 << ", " << expXferName3 << " or "
                                    << expXferName4 << " expected but " << xferName << " found: " << filename
                                    << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    DCMDATA_ERROR(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    DCMDATA_WARN(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                            break;
                        case AP_GeneralPurposeDVDJPEG2000:
                        case AP_GeneralPurposeBDJPEG2000:
                        case AP_USBandFlashJPEG2000:
                            /* need to check multiple transfer syntaxes */
                            found = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax) ||
                                    compare(transferSyntax, UID_JPEG2000LosslessOnlyTransferSyntax) ||
                                    compare(transferSyntax, UID_JPEG2000TransferSyntax);
                            if (!found)
                            {
                                const OFString expXferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax, "");
                                const OFString expXferName2 = dcmFindNameOfUID(UID_JPEG2000LosslessOnlyTransferSyntax, "");
                                const OFString expXferName3 = dcmFindNameOfUID(UID_JPEG2000TransferSyntax, "");
                                /* create error message */
                                OFOStringStream oss;
                                oss << expXferName1 << ", " << expXferName2 << " or " << expXferName3
                                    << " expected but " << xferName << " found: " << filename << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    DCMDATA_ERROR(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    DCMDATA_WARN(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                            break;
                        case AP_GeneralPurposeBDMPEG2MPatML:
                        case AP_GeneralPurposeBDMPEG2MPatHL:
                        case AP_GeneralPurposeBDMPEG4HPatLV41:
                        case AP_GeneralPurposeBDMPEG4HPatLV41BD:
                            /* compare with expected transfer syntax */
                            found = compare(transferSyntax, expectedTransferSyntax);
                            if (found)
                            {
                                /* check for multi-frame composite IOD */
                                if (!isMultiframeStorageSOPClass(mediaSOPClassUID))
                                {
                                    /* create error message */
                                    OFOStringStream oss;
                                    oss << xferName << " only for multi-frame composite IODs: " << filename
                                        << OFStringStream_ends;
                                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                    if (TransferSyntaxCheck)
                                    {
                                        DCMDATA_ERROR(tmpString);
                                        result = EC_ApplicationProfileViolated;
                                    } else
                                        DCMDATA_WARN(tmpString);
                                    OFSTRINGSTREAM_FREESTR(tmpString)
                                }
                            } else {
                                found = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax);
                                /* create error message */
                                OFOStringStream oss;
                                /* check for multi-frame composite IOD */
                                if (isMultiframeStorageSOPClass(mediaSOPClassUID))
                                {
                                    const OFString expXferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax, "");
                                    const OFString expXferName2 = dcmFindNameOfUID(expectedTransferSyntax.c_str(), "");
                                    oss << expXferName1 << " or " << expXferName2 << " expected but " << xferName
                                        << " found: " << filename << OFStringStream_ends;
                                } else {
                                    const OFString expXferName = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax, "");
                                    oss << expXferName << " expected but " << xferName << " found: " << filename
                                        << OFStringStream_ends;
                                }
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    DCMDATA_ERROR(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    DCMDATA_WARN(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                            break;
                        case AP_GeneralPurposeBDMPEG4HPatLV42_2D:
                        case AP_GeneralPurposeBDMPEG4HPatLV42_3D:
                        case AP_GeneralPurposeBDMPEG4StereoHPatLV42:
                            /* compare with expected transfer syntax */
                            found = compare(transferSyntax, expectedTransferSyntax);
                            if (found)
                            {
                                /* check for multi-frame composite IOD */
                                if (!isMultiframeStorageSOPClass(mediaSOPClassUID))
                                {
                                    /* create error message */
                                    OFOStringStream oss;
                                    oss << xferName << " only for multi-frame composite IODs: " << filename
                                        << OFStringStream_ends;
                                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                    if (TransferSyntaxCheck)
                                    {
                                        DCMDATA_ERROR(tmpString);
                                        result = EC_ApplicationProfileViolated;
                                    } else
                                        DCMDATA_WARN(tmpString);
                                    OFSTRINGSTREAM_FREESTR(tmpString)
                                }
                            } else {
                                const OFString expXferName = dcmFindNameOfUID(expectedTransferSyntax.c_str(), "");
                                /* create error message */
                                OFOStringStream oss;
                                oss << expXferName << " expected but " << xferName << " found: " << filename
                                    << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    DCMDATA_ERROR(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    DCMDATA_WARN(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                            break;
                        case AP_XrayAngiographicDVD:
                            if (compare(mediaSOPClassUID, UID_XRayAngiographicImageStorage))
                            {
                                /* need to check multiple transfer syntaxes */
                                found = compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax) ||
                                        compare(transferSyntax, UID_JPEGProcess1TransferSyntax) ||
                                        compare(transferSyntax, UID_JPEGProcess2_4TransferSyntax);
                                if (!found)
                                {
                                    const OFString expXferName1 = dcmFindNameOfUID(UID_JPEGProcess14SV1TransferSyntax, "");
                                    const OFString expXferName2 = dcmFindNameOfUID(UID_JPEGProcess1TransferSyntax, "");
                                    const OFString expXferName3 = dcmFindNameOfUID(UID_JPEGProcess2_4TransferSyntax, "");
                                    /* create error message */
                                    OFOStringStream oss;
                                    oss << expXferName1 << ", " << expXferName2 << " or " << expXferName3
                                        << " expected but " << xferName << " found: " << filename << OFStringStream_ends;
                                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                    if (TransferSyntaxCheck)
                                    {
                                        DCMDATA_ERROR(tmpString);
                                        result = EC_ApplicationProfileViolated;
                                    } else
                                        DCMDATA_WARN(tmpString);
                                    OFSTRINGSTREAM_FREESTR(tmpString)
                                }
                            }
                            break;
                        case AP_CTandMR:
                            {
                                const OFBool isImage = compare(mediaSOPClassUID, UID_CTImageStorage) ||
                                                       compare(mediaSOPClassUID, UID_MRImageStorage) ||
                                                       compare(mediaSOPClassUID, UID_SecondaryCaptureImageStorage);
                                /* need to check multiple transfer syntaxes (JPEG only allowed for images) */
                                found = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax) ||
                                       (compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax) && isImage);
                                if (!found)
                                {
                                    const OFString expXferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax, "");
                                    const OFString expXferName2 = dcmFindNameOfUID(UID_JPEGProcess14SV1TransferSyntax, "");
                                    /* create error message */
                                    OFOStringStream oss;
                                    if (isImage)
                                    {
                                        oss << expXferName1 << " or " << expXferName2 << " expected but " << xferName
                                            << " found: " << filename << OFStringStream_ends;
                                    } else {
                                        oss << expXferName1 << " expected but " << xferName << " found: " << filename
                                            << OFStringStream_ends;
                                    }
                                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                    if (TransferSyntaxCheck)
                                    {
                                        DCMDATA_ERROR(tmpString);
                                        result = EC_ApplicationProfileViolated;
                                    } else
                                        DCMDATA_WARN(tmpString);
                                    OFSTRINGSTREAM_FREESTR(tmpString)
                                }
                            }
                            break;
                        case AP_UltrasoundIDSF:
                        case AP_UltrasoundSCSF:
                        case AP_UltrasoundCCSF:
                        case AP_UltrasoundIDMF:
                        case AP_UltrasoundSCMF:
                        case AP_UltrasoundCCMF:
                            /* need to check multiple transfer syntaxes */
                            found = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax) ||
                                    compare(transferSyntax, UID_RLELosslessTransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess1TransferSyntax);
                            if (!found)
                            {
                                const OFString expXferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax, "");
                                const OFString expXferName2 = dcmFindNameOfUID(UID_RLELosslessTransferSyntax, "");
                                const OFString expXferName3 = dcmFindNameOfUID(UID_JPEGProcess1TransferSyntax, "");
                                /* create error message */
                                OFOStringStream oss;
                                oss << expXferName1 << ", " << expXferName2 << " or " << expXferName3
                                    << " expected but " << xferName << " found: " << filename
                                    << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    DCMDATA_ERROR(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    DCMDATA_WARN(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                            break;
                        case AP_GeneralPurpose:
                        case AP_MPEG2MPatMLDVD:
                        case AP_BasicCardiac:
                        case AP_XrayAngiographic:
                        case AP_DentalRadiograph:
                        case AP_TwelveLeadECG:
                        case AP_HemodynamicWaveform:
                        default:
                        {
                            /* compare with expected transfer syntax */
                            found = compare(transferSyntax, expectedTransferSyntax);
                            if (!found)
                            {
                                const OFString expXferName = dcmFindNameOfUID(expectedTransferSyntax.c_str(),
                                    expectedTransferSyntax.c_str() /*defaultValue*/);
                                /* create error message */
                                OFOStringStream oss;
                                oss << expXferName << " expected but " << xferName << " found: " << filename
                                    << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    DCMDATA_ERROR(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    DCMDATA_WARN(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}


// check whether dataset conforms to basic cardiac application profile
OFCondition DicomDirInterface::checkBasicCardiacAttributes(DcmItem *dataset,
                                                           const OFFilename &filename)
{
    OFCondition result = EC_Normal;
    if (!checkExistsWithStringValue(dataset, DCM_Modality, "XA", filename))
        result = EC_ApplicationProfileViolated;
    if (!checkExistsWithMinMaxValue(dataset, DCM_Rows, 1, 512, filename, ResolutionCheck))
        result = EC_ApplicationProfileViolated;
    if (!checkExistsWithMinMaxValue(dataset, DCM_Columns, 1, 512, filename, ResolutionCheck))
        result = EC_ApplicationProfileViolated;
    if (!checkExistsWithIntegerValue(dataset, DCM_BitsAllocated, 8, filename, EncodingCheck))
        result = EC_ApplicationProfileViolated;
    if (!checkExistsWithIntegerValue(dataset, DCM_BitsStored, 8, filename, EncodingCheck))
        result = EC_ApplicationProfileViolated;
    /* overlay data, if present, shall be encoded in OverlayData (60XX,3000) */
    for (Uint16 grp = 0x6000; grp < 0x601f; grp = OFstatic_cast(Uint16, grp + 2))
    {
        /* check minimum number of attributes required for an overlay plane to be displayed */
        if (dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayRows.getElement())) &&
            dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayColumns.getElement())) &&
            dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitsAllocated.getElement())) &&
            dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitPosition.getElement())) &&
            !dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayData.getElement())))
        {
            DCMDATA_ERROR("embedded overlay data present in group 0x" << STD_NAMESPACE hex << grp
                << ", file: " << filename);
            result = EC_ApplicationProfileViolated;
        }
    }
    return result;
}


// check whether dataset conforms to xray angiographic application profile
OFCondition DicomDirInterface::checkXrayAngiographicAttributes(DcmItem *dataset,
                                                               const OFString &sopClass,
                                                               const OFFilename &filename)
{
    OFCondition result = EC_Normal;
    /* requirements depend on SOP class */
    if (compare(sopClass, UID_XRayAngiographicImageStorage))
    {
        /* a XA image */
        if (!checkExistsWithStringValue(dataset, DCM_Modality, "XA", filename))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithMinMaxValue(dataset, DCM_Rows, 1, 1024, filename, ResolutionCheck))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithMinMaxValue(dataset, DCM_Columns, 1, 1024, filename, ResolutionCheck))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithValue(dataset, DCM_BitsStored, filename))
            result = EC_MissingAttribute;
        else
        {
            long bs;
            dataset->findAndGetLongInt(DCM_BitsStored, bs);
            if ((bs != 8) && (bs != 10) && (bs != 12))
            {
                /* report an error or a warning */
                printUnexpectedValueMessage(DCM_BitsStored, filename, EncodingCheck);
                if (EncodingCheck)
                    result = EC_ApplicationProfileViolated;
            }
        }
    }
    else if (compare(sopClass, UID_SecondaryCaptureImageStorage))
    {
        /* a SC image */
        if (!checkExistsWithMinMaxValue(dataset, DCM_Rows, 1, 1024, filename, ResolutionCheck))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithMinMaxValue(dataset, DCM_Columns, 1, 1024, filename, ResolutionCheck))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithIntegerValue(dataset, DCM_SamplesPerPixel, 1, filename))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithStringValue(dataset, DCM_PhotometricInterpretation, "MONOCHROME2", filename))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithIntegerValue(dataset, DCM_BitsAllocated, 8, filename, EncodingCheck))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithIntegerValue(dataset, DCM_BitsStored, 8, filename, EncodingCheck))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithIntegerValue(dataset, DCM_HighBit, 7, filename, EncodingCheck))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithIntegerValue(dataset, DCM_PixelRepresentation, 0, filename))
            result = EC_ApplicationProfileViolated;
        /* check whether any overlay is present */
        for (Uint16 grp = 0x6000; grp < 0x601f; grp = OFstatic_cast(Uint16, grp + 2))
        {
            /* check minimum number of attributes required for an overlay plane to be displayed */
            if (dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayRows.getElement())) &&
                dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayColumns.getElement())) &&
                dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitsAllocated.getElement())) &&
                dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitPosition.getElement())))
            {
                /* create error message */
                DCMDATA_ERROR("overlay group 0x" << STD_NAMESPACE hex << grp
                    << " present in file: " << filename);
                result = EC_ApplicationProfileViolated;
            }
        }
    } else /* wrong SOP class */
        result = EC_ApplicationProfileViolated;
    return result;
}


// check whether dataset conforms to dental radiograph application profile
OFCondition DicomDirInterface::checkDentalRadiographAttributes(DcmItem *dataset,
                                                               const OFFilename &filename)
{
    OFCondition result = EC_Normal;
    /* check presence of type 2 elements */
    if (!checkExists(dataset, DCM_InstitutionName, filename) ||
        !checkExists(dataset, DCM_ManufacturerModelName, filename) ||
        !checkExists(dataset, DCM_DetectorID, filename) ||
        !checkExists(dataset, DCM_DetectorManufacturerName, filename) ||
        !checkExists(dataset, DCM_DetectorManufacturerModelName, filename))
    {
        result = EC_TagNotFound;
    }
    /* check for value constraints */
    else if (!checkExistsWithValue(dataset, DCM_BitsAllocated, filename) ||
             !checkExistsWithValue(dataset, DCM_BitsStored, filename))
    {
        result = EC_MissingAttribute;
    } else {
        long bs;
        dataset->findAndGetLongInt(DCM_BitsStored, bs);
        if ((bs != 8) && (bs != 10) && (bs != 12) && (bs != 16))
        {
            /* report an error or a warning */
            printUnexpectedValueMessage(DCM_BitsStored, filename, EncodingCheck);
            if (EncodingCheck)
                result = EC_ApplicationProfileViolated;
        }
        long ba;
        dataset->findAndGetLongInt(DCM_BitsAllocated, ba);
        if (((bs == 8) && (ba != 8)) || ((bs != 8) && (ba != 16)))
        {
            /* report an error or a warning */
            printUnexpectedValueMessage(DCM_BitsAllocated, filename, EncodingCheck);
            if (EncodingCheck)
                result = EC_ApplicationProfileViolated;
        }
    }
    return result;
}


// check whether dataset conforms to ct and mr application profile
OFCondition DicomDirInterface::checkCTandMRAttributes(DcmItem *dataset,
                                                      const OFString &sopClass,
                                                      const OFFilename &filename)
{
    OFCondition result = EC_Normal;
    /* check presence of type 1 elements */
    if (!checkExistsWithValue(dataset, DCM_Rows, filename) ||
        !checkExistsWithValue(dataset, DCM_Columns, filename))
    {
        result = EC_MissingAttribute;
    }
    /* requirements depend on SOP class */
    if (compare(sopClass, UID_CTImageStorage))
    {
        /* a CT image */
        if (!checkExistsWithStringValue(dataset, DCM_Modality, "CT", filename))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithStringValue(dataset, DCM_PhotometricInterpretation, "MONOCHROME2", filename))
            result = EC_ApplicationProfileViolated;
    }
    else if (compare(sopClass, UID_MRImageStorage))
    {
        /* a MR image */
        if (!checkExistsWithStringValue(dataset, DCM_Modality, "MR", filename))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithStringValue(dataset, DCM_PhotometricInterpretation, "MONOCHROME2", filename))
            result = EC_ApplicationProfileViolated;
        if (!checkExistsWithValue(dataset, DCM_BitsStored, filename) ||
            !checkExistsWithValue(dataset, DCM_HighBit, filename))
        {
            result = EC_MissingAttribute;
        } else {
            long bs;
            dataset->findAndGetLongInt(DCM_BitsStored, bs);
            if ((bs != 8) && (bs != 12) && (bs != 16))
            {
                /* report an error or a warning */
                printUnexpectedValueMessage(DCM_BitsStored, filename, EncodingCheck);
                if (EncodingCheck)
                    result = EC_ApplicationProfileViolated;
            }
            long hb;
            dataset->findAndGetLongInt(DCM_HighBit, hb);
            if (hb != bs - 1)
            {
                /* report an error or a warning */
                printUnexpectedValueMessage(DCM_HighBit, filename, EncodingCheck);
                if (EncodingCheck)
                    result = EC_ApplicationProfileViolated;
            }
        }
    }
    else if (compare(sopClass, UID_SecondaryCaptureImageStorage))
    {
        /* a SC image */
        if (!checkExistsWithIntegerValue(dataset, DCM_SamplesPerPixel, 1, filename))
            result = EC_ApplicationProfileViolated;
        if (!checkExists(dataset, DCM_PhotometricInterpretation, filename))
        {
            OFString pi;
            dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, pi);
            if (compare(pi, "MONOCHROME2"))
            {
                if (!checkExistsWithValue(dataset, DCM_BitsAllocated, filename) ||
                    !checkExistsWithValue(dataset, DCM_BitsStored, filename) ||
                    !checkExistsWithValue(dataset, DCM_HighBit, filename))
                {
                    result = EC_MissingAttribute;
                } else {
                    long ba;
                    dataset->findAndGetLongInt(DCM_BitsAllocated, ba);
                    if ((ba != 8) && (ba != 16))
                    {
                        /* report an error or a warning */
                        printUnexpectedValueMessage(DCM_BitsAllocated, filename, EncodingCheck);
                        if (EncodingCheck)
                            result = EC_ApplicationProfileViolated;
                    }
                    long bs;
                    dataset->findAndGetLongInt(DCM_BitsStored, bs);
                    if (bs != ba)
                    {
                        /* report an error or a warning */
                        printUnexpectedValueMessage(DCM_BitsStored, filename, EncodingCheck);
                        if (EncodingCheck)
                            result = EC_ApplicationProfileViolated;
                    }
                    long hb;
                    dataset->findAndGetLongInt(DCM_HighBit, hb);
                    if (hb != bs - 1)
                    {
                        /* report an error or a warning */
                        printUnexpectedValueMessage(DCM_HighBit, filename, EncodingCheck);
                        if (EncodingCheck)
                            result = EC_ApplicationProfileViolated;
                    }
                }
            }
            else if (compare(pi, "PALETTE COLOR"))
            {
                if (!checkExistsWithIntegerValue(dataset, DCM_BitsAllocated, 8, filename))
                    result = EC_ApplicationProfileViolated;
                if (!checkExistsWithIntegerValue(dataset, DCM_BitsStored, 8, filename))
                    result = EC_ApplicationProfileViolated;
                if (!checkExistsWithIntegerValue(dataset, DCM_HighBit, 7, filename))
                    result = EC_ApplicationProfileViolated;
            } else {
                /* report an error */
                printUnexpectedValueMessage(DCM_PhotometricInterpretation, filename);
                result = EC_ApplicationProfileViolated;
            }
        }
    } else /* wrong SOP class */
        result = EC_ApplicationProfileViolated;
    return result;
}


// check whether dataset conforms to one of the ultrasound application profiles
OFCondition DicomDirInterface::checkUltrasoundAttributes(DcmItem *dataset,
                                                         const OFString &transferSyntax,
                                                         const OFFilename &filename)
{
    OFCondition result = EC_Normal;
    /* a US image */
    if (!checkExistsWithValue(dataset, DCM_PhotometricInterpretation, filename))
        result = EC_MissingAttribute;
    else
    {
        OFString pi;
        /* check photometric interpretation */
        getStringFromDataset(dataset, DCM_PhotometricInterpretation, pi);
        const OFBool uncompressed = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax);
        const OFBool rle_lossless = compare(transferSyntax, UID_RLELosslessTransferSyntax);
        const OFBool jpeg_lossy = compare(transferSyntax, UID_JPEGProcess1TransferSyntax);
        OFBool valid = (compare(pi, "MONOCHROME2") && (uncompressed || rle_lossless)) ||
                       (compare(pi, "RGB") && (uncompressed || rle_lossless)) ||
                       (compare(pi, "PALETTE COLOR") && (uncompressed || rle_lossless)) ||
                       (compare(pi, "YBR_FULL") && rle_lossless) ||
                       (compare(pi, "YBR_FULL_422") && (uncompressed || jpeg_lossy)) ||
                       (compare(pi, "YBR_PARTIAL_422") && (uncompressed || jpeg_lossy));
        if (!valid)
        {
            /* report an error */
            printUnexpectedValueMessage(DCM_PhotometricInterpretation, filename);
            result = EC_ApplicationProfileViolated;
        }
    }
    if ((ApplicationProfile == AP_UltrasoundSCSF) ||
        (ApplicationProfile == AP_UltrasoundCCSF) ||
        (ApplicationProfile == AP_UltrasoundSCMF) ||
        (ApplicationProfile == AP_UltrasoundCCMF))
    {
        /* check for US region calibration module (SC and CC profiles) */
        OFBool ok = OFTrue;
        unsigned long i = 0;
        /* iterate over all sequence items */
        DcmItem *ditem = NULL;
        while (ok && dataset->findAndGetSequenceItem(DCM_SequenceOfUltrasoundRegions, ditem, i).good())
        {
            ok &= checkExistsWithValue(ditem, DCM_RegionLocationMinX0, filename);
            ok &= checkExistsWithValue(ditem, DCM_RegionLocationMinY0, filename);
            ok &= checkExistsWithValue(ditem, DCM_RegionLocationMaxX1, filename);
            ok &= checkExistsWithValue(ditem, DCM_RegionLocationMaxY1, filename);
            ok &= checkExistsWithValue(ditem, DCM_PhysicalUnitsXDirection, filename);
            ok &= checkExistsWithValue(ditem, DCM_PhysicalUnitsYDirection, filename);
            ok &= checkExistsWithValue(ditem, DCM_PhysicalDeltaX, filename);
            ok &= checkExistsWithValue(ditem, DCM_PhysicalDeltaY, filename);
            ok &= checkExistsWithValue(ditem, DCM_RegionSpatialFormat, filename);
            ok &= checkExistsWithValue(ditem, DCM_RegionDataType, filename);
            ok &= checkExistsWithValue(ditem, DCM_RegionFlags, filename);
            if ((ApplicationProfile == AP_UltrasoundCCSF) ||
                (ApplicationProfile == AP_UltrasoundCCMF))
            {
                /* check for pixel component organization (CC profile) */
                if (checkExistsWithValue(ditem, DCM_PixelComponentOrganization, filename))
                {
                    ok &= checkExistsWithValue(ditem, DCM_PixelComponentPhysicalUnits, filename);
                    ok &= checkExistsWithValue(ditem, DCM_PixelComponentDataType, filename);
                    long pco;
                    dataset->findAndGetLongInt(DCM_PixelComponentOrganization, pco);
                    if (pco == 0)
                    {
                        /* pixel component organization: bit aligned positions */
                        ok &= checkExistsWithValue(ditem, DCM_PixelComponentMask, filename);
                        ok &= checkExistsWithValue(ditem, DCM_NumberOfTableBreakPoints, filename);
                        ok &= checkExistsWithValue(ditem, DCM_TableOfXBreakPoints, filename);
                        ok &= checkExistsWithValue(ditem, DCM_TableOfYBreakPoints, filename);
                    } else if (pco == 1)
                    {
                        /* pixel component organization: ranges */
                        ok &= checkExistsWithValue(ditem, DCM_PixelComponentRangeStart, filename);
                        ok &= checkExistsWithValue(ditem, DCM_PixelComponentRangeStop, filename);
                        ok &= checkExistsWithValue(ditem, DCM_NumberOfTableBreakPoints, filename);
                        ok &= checkExistsWithValue(ditem, DCM_TableOfXBreakPoints, filename);
                        ok &= checkExistsWithValue(ditem, DCM_TableOfYBreakPoints, filename);
                    } else if (pco == 2)
                    {
                        /* pixel component organization: table look up */
                        ok &= checkExistsWithValue(ditem, DCM_NumberOfTableEntries, filename);
                        ok &= checkExistsWithValue(ditem, DCM_TableOfPixelValues, filename);
                        ok &= checkExistsWithValue(ditem, DCM_TableOfParameterValues, filename);
                    } else {
                        /* report an error */
                        printUnexpectedValueMessage(DCM_PixelComponentOrganization, filename);
                        ok = OFFalse;
                    }
                } else
                    ok = OFFalse;
            }
            i++;
        }
        if (ok)
        {
            if (i == 0)
            {
                /* no item found, sequence seems to be empty */
                printRequiredAttributeMessage(DCM_SequenceOfUltrasoundRegions, filename);
                result = EC_ApplicationProfileViolated;
            }
        } else {
            /* report any error within the 'while' loop as an violation of the application profile */
            result = EC_ApplicationProfileViolated;
        }
    }
    return result;
}


// check the given file (dataset) for mandatory attributes
OFCondition DicomDirInterface::checkMandatoryAttributes(DcmMetaInfo *metainfo,
                                                        DcmItem *dataset,
                                                        const OFFilename &filename)
{
    OFCondition result = EC_IllegalParameter;
    if ((metainfo != NULL) && (dataset != NULL))
    {
        /* are mandatory attributes for DICOMDIR available and valued? */
        result = EC_Normal;
        /* get the transfer syntax and SOP class
           (cannot fail, after checkSOPClassAndXfer has been passed) */
        OFString transferSyntax;
        OFString mediaSOPClassUID;
        metainfo->findAndGetOFStringArray(DCM_TransferSyntaxUID, transferSyntax);
        metainfo->findAndGetOFStringArray(DCM_MediaStorageSOPClassUID, mediaSOPClassUID);
        E_DirRecType recordType = sopClassToRecordType(mediaSOPClassUID);
        /* hanging protocol, palette and implant files are handled separately */
        if (recordType == ERT_HangingProtocol)
        {
            /* check whether all type 1 elements are really present */
            if (!checkExistsWithValue(dataset, DCM_HangingProtocolName, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_HangingProtocolDescription, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_HangingProtocolLevel, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_HangingProtocolCreator, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_HangingProtocolCreationDateTime, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_HangingProtocolDefinitionSequence, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_NumberOfPriorsReferenced, filename))
                result = EC_MissingAttribute;
        }
        else if (recordType == ERT_Palette)
        {
            /* check whether all type 1 elements are really present */
            if (!checkExistsWithValue(dataset, DCM_ContentLabel, filename))
                result = EC_MissingAttribute;
        }
        else if (recordType == ERT_Implant)
        {
            /* check whether all type 1 elements are really present */
            if (!checkExistsWithValue(dataset, DCM_Manufacturer, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_ImplantName, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_ImplantPartNumber, filename))
                result = EC_MissingAttribute;
        }
        else if (recordType == ERT_ImplantGroup)
        {
            /* check whether all type 1 elements are really present */
            if (!checkExistsWithValue(dataset, DCM_ImplantAssemblyTemplateName, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_ImplantAssemblyTemplateIssuer, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_ProcedureTypeCodeSequence, filename))
                result = EC_MissingAttribute;
        }
        else if (recordType == ERT_ImplantAssy)
        {
            /* check whether all type 1 elements are really present */
            if (!checkExistsWithValue(dataset, DCM_ImplantTemplateGroupName, filename))
                result = EC_MissingAttribute;
            if (!checkExistsWithValue(dataset, DCM_ImplantTemplateGroupIssuer, filename))
                result = EC_MissingAttribute;
        } else {
            /* PatientID is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_PatientID, filename))
                    result = EC_MissingAttribute;
            }
            /* PatientName is type 2 in DICOMDIR and images */
            if (!checkExists(dataset, DCM_PatientName, filename))
                result = EC_TagNotFound;
            /* StudyDate is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_StudyDate, filename))
                    result = EC_MissingAttribute;
            }
            /* StudyTime is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_StudyTime, filename))
                    result = EC_MissingAttribute;
            }
            /* StudyDescription is type 2 in DICOMDIR and type 3 in images.
               We can create an empty attribute in the directory
             */
            /* StudyInstanceUID is type 1 in DICOMDIR and images */
            if (!checkExistsWithValue(dataset, DCM_StudyInstanceUID, filename))
                result = EC_MissingAttribute;
            /* StudyID is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_StudyID, filename))
                    result = EC_MissingAttribute;
            }
            /* AccessionNumber is type 2 in DICOMDIR and type 3 in images
               We can create an empty attribute in the directory
            */
            /* Modality is type 1 in DICOMDIR and type 1 in images */
            if (!checkExistsWithValue(dataset, DCM_Modality, filename))
                result = EC_MissingAttribute;
            /* SeriesInstanceUID is type 1 in DICOMDIR and type 1 in images */
            if (!checkExistsWithValue(dataset, DCM_SeriesInstanceUID, filename))
                result = EC_MissingAttribute;
            /* SeriesNumber is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_SeriesNumber, filename))
                    result = EC_MissingAttribute;
            }
            /* image and other numbers are type 1 in DICOMDIR but type 2 in images */
            /* (basically, check whether all type 1 elements are really present) */
            switch (recordType)
            {
                case ERT_Overlay:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_RETIRED_OverlayNumber, filename))
                            result = EC_MissingAttribute;
                    }
                    break;
                case ERT_ModalityLut:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_RETIRED_LUTNumber, filename))
                            result = EC_MissingAttribute;
                    }
                    break;
                case ERT_VoiLut:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_RETIRED_LUTNumber, filename))
                            result = EC_MissingAttribute;
                    }
                    break;
                case ERT_Curve:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_RETIRED_CurveNumber, filename))
                            result = EC_MissingAttribute;
                    }
                    break;
                case ERT_SRDocument:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_CompletionFlag, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_VerificationFlag, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ConceptNameCodeSequence, filename))
                        result = EC_MissingAttribute;
                    {
                        OFString tmpString;
                        if (compare(getStringFromDataset(dataset, DCM_VerificationFlag, tmpString), "VERIFIED"))
                        {
                            /* VerificationDateTime is required if verification flag is VERIFIED,
                               retrieve most recent (= last) entry from VerifyingObserverSequence */
                            DcmItem *ditem = NULL;
                            OFCondition l_status = dataset->findAndGetSequenceItem(DCM_VerifyingObserverSequence, ditem, -1 /*last*/);
                            if (l_status.good())
                            {
                                if (!checkExistsWithValue(ditem, DCM_VerificationDateTime, filename))
                                    result = EC_MissingAttribute;
                            } else
                                result = l_status;
                        }
                    }
                    break;
                case ERT_Presentation:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentLabel, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_PresentationCreationDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_PresentationCreationTime, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_Waveform:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_RTDose:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                            result = EC_MissingAttribute;
                    }
                    if (!checkExistsWithValue(dataset, DCM_DoseSummationType, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_RTStructureSet:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                            result = EC_MissingAttribute;
                    }
                    if (!checkExistsWithValue(dataset, DCM_StructureSetLabel, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_RTPlan:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                            result = EC_MissingAttribute;
                    }
                    if (!checkExistsWithValue(dataset, DCM_RTPlanLabel, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_RTTreatRecord:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_StoredPrint:
                    /* (nothing to do) */
                    break;
                case ERT_KeyObjectDoc:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ConceptNameCodeSequence, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_RawData:
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_MissingAttribute;
                    /* InstanceNumber is type 2 in IOD and directory record! */
                    break;
                case ERT_Spectroscopy:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ImageType, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_NumberOfFrames, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_Rows, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_Columns, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_DataPointRows, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_DataPointColumns, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_EncapDoc:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_MIMETypeOfEncapsulatedDocument, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_Stereometric:
                case ERT_Plan:
                    /* nothing to check */
                    break;
                case ERT_Registration:
                case ERT_Fiducial:
                case ERT_ValueMap:
                case ERT_Surface:
                case ERT_Measurement:
                case ERT_Tract:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentLabel, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_SurfaceScan:
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_Assessment:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    if (!checkExistsWithValue(dataset, DCM_InstanceCreationDate, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_Radiotherapy:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_MissingAttribute;
                    break;
                case ERT_Image:
                default:
                    {
                        OFCondition l_status = EC_Normal;
                        /* it can only be an image */
                        if (!InventMode)
                        {
                            if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                                result = EC_MissingAttribute;
                        }
                        /* check profile specific requirements */
                        if ((ApplicationProfile == AP_GeneralPurposeDVDJPEG) ||
                            (ApplicationProfile == AP_GeneralPurposeDVDJPEG2000) ||
                            (ApplicationProfile == AP_GeneralPurposeBDJPEG) ||
                            (ApplicationProfile == AP_GeneralPurposeBDJPEG2000) ||
                            (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatML) ||
                            (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatHL) ||
                            (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41) ||
                            (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41BD) ||
                            (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_2D) ||
                            (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_3D) ||
                            (ApplicationProfile == AP_GeneralPurposeBDMPEG4StereoHPatLV42) ||
                            (ApplicationProfile == AP_USBandFlashJPEG) ||
                            (ApplicationProfile == AP_USBandFlashJPEG2000) ||
                            (ApplicationProfile == AP_MPEG2MPatMLDVD))
                        {
                            /* check presence of type 1 elements */
                            if (!checkExistsWithValue(dataset, DCM_Rows, filename) ||
                                !checkExistsWithValue(dataset, DCM_Columns, filename))
                            {
                                result = EC_MissingAttribute;
                            }
                        }
                        else if (ApplicationProfile == AP_BasicCardiac)
                            l_status = checkBasicCardiacAttributes(dataset, filename);
                        else if ((ApplicationProfile == AP_XrayAngiographic) || (ApplicationProfile == AP_XrayAngiographicDVD))
                            l_status = checkXrayAngiographicAttributes(dataset, mediaSOPClassUID, filename);
                        else if (ApplicationProfile == AP_DentalRadiograph)
                            l_status = checkDentalRadiographAttributes(dataset, filename);
                        else if (ApplicationProfile == AP_CTandMR)
                            l_status = checkCTandMRAttributes(dataset, mediaSOPClassUID, filename);
                        else if ((ApplicationProfile == AP_UltrasoundIDSF) ||
                                 (ApplicationProfile == AP_UltrasoundSCSF) ||
                                 (ApplicationProfile == AP_UltrasoundCCSF) ||
                                 (ApplicationProfile == AP_UltrasoundIDMF) ||
                                 (ApplicationProfile == AP_UltrasoundSCMF) ||
                                 (ApplicationProfile == AP_UltrasoundCCMF))
                        {
                            l_status = checkUltrasoundAttributes(dataset, transferSyntax, filename);
                        }
                        /* set final result status (if everything else was ok) */
                        if (result.good())
                            result = l_status;
                    }
            }
        }
    }
    return result;
}


// select specified application profile and adapt certain settings
OFCondition DicomDirInterface::selectApplicationProfile(const E_ApplicationProfile profile)
{
    OFCondition result = EC_Normal;
    /* special handling for particular profiles */
    if ((ApplicationProfile == AP_BasicCardiac) ||
        (ApplicationProfile == AP_XrayAngiographic) ||
        (ApplicationProfile == AP_XrayAngiographicDVD))
    {
        /* check for JPEG image support */
        if (!JPEGSupport)
            result = EC_IllegalCall;
    }
    /* store new profile */
    if (result.good())
        ApplicationProfile = profile;
    return result;
}


// check whether DICOM file is suitable for a DICOMDIR of the specified application profile
OFCondition DicomDirInterface::checkDicomFile(const OFFilename &filename,
                                              const OFFilename &directory,
                                              const OFBool checkFilename)
{
    /* define file format object for the DICOM file to be loaded */
    DcmFileFormat fileformat;
    /* call the "real" function */
    return loadAndCheckDicomFile(filename, directory, fileformat, checkFilename);
}


// load DICOM file and check whether it is suitable for a DICOMDIR of the specified application profile
OFCondition DicomDirInterface::loadAndCheckDicomFile(const OFFilename &filename,
                                                     const OFFilename &directory,
                                                     DcmFileFormat &fileformat,
                                                     const OFBool checkFilename)
{
    OFCondition result = EC_IllegalParameter;
    /* create fully qualified pathname of the DICOM file to be added */
    OFFilename pathname;
    OFStandard::combineDirAndFilename(pathname, directory, filename, OFTrue /*allowEmptyDirName*/);
    DCMDATA_INFO("checking file: " << pathname);
    /* check filename (if not disabled) */
    if (!checkFilename || isFilenameValid(filename))
    {
        /* load DICOM file */
        result = fileformat.loadFile(pathname);
        if (result.good())
        {
            /* check for correct part 10 file format */
            DcmMetaInfo *metainfo = fileformat.getMetaInfo();
            DcmDataset *dataset = fileformat.getDataset();
            if ((metainfo == NULL) || (metainfo->card() == 0))
            {
                /* create error message */
                OFOStringStream oss;
                oss << "file not in part 10 format (no file meta information): " << filename
                    << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                /* file meta information is required */
                if (FileFormatCheck)
                {
                    DCMDATA_ERROR(tmpString);
                    result = EC_FileMetaInfoHeaderMissing;
                } else {
                    DCMDATA_WARN(tmpString);
                    /* add missing file meta information */
                    if (dataset != NULL)
                        fileformat.validateMetaInfo(dataset->getOriginalXfer());
                }
                OFSTRINGSTREAM_FREESTR(tmpString)
            }
            /* check for empty dataset */
            if ((dataset == NULL) || (dataset->card() == 0))
            {
                DCMDATA_ERROR("file contains no data (no data set): " << filename);
                result = EC_CorruptedData;
            }
            /* only proceed if previous checks have been passed */
            if (result.good())
            {
                /* check for SOP class and transfer syntax */
                result = checkSOPClassAndXfer(metainfo, dataset, filename);
                if (result.good())
                {
                    /* check for mandatory attributes */
                    if (checkMandatoryAttributes(metainfo, dataset, filename).bad())
                        result = EC_ApplicationProfileViolated;
                }
            }
        } else {
            /* report an error */
            DCMDATA_ERROR(result.text() << ": reading file: " << filename);
        }
    }
    return result;
}


// check whether given record matches dataset
OFBool DicomDirInterface::recordMatchesDataset(DcmDirectoryRecord *record,
                                               DcmItem *dataset)
{
    OFBool result = OFFalse;
    if ((record != NULL) && (dataset != NULL))
    {
        OFString recordString, datasetString;
        /* check matching depending on record type */
        switch (record->getRecordType())
        {
            case ERT_Patient:
                if (checkExistsWithValue(dataset, DCM_PatientID))
                {
                    OFString patientID;
                    /* PatientID is the primary key */
                    result = compare(getStringFromDataset(record, DCM_PatientID, patientID),
                                     getStringFromDataset(dataset, DCM_PatientID, datasetString));
                    /* optional: check whether PatientName also matches */
                    if (result && !compare(getStringFromDataset(record, DCM_PatientName, recordString),
                                           getStringFromDataset(dataset, DCM_PatientName, datasetString)))
                    {
                        if (InventPatientIDMode)
                        {
                            DCMDATA_WARN("PatientName inconsistent for PatientID: " << patientID);
                            /* remove current patient ID, will be replaced later */
                            dataset->putAndInsertString(DCM_PatientID, "");
                            result = OFFalse;
                        }
                    }
                } else {
                    /* if there is no value for PatientID in the dataset try using the PatientName */
                    result = compare(getStringFromDataset(record, DCM_PatientName, recordString),
                                     getStringFromDataset(dataset, DCM_PatientName, datasetString));
                }
                break;
            case ERT_Study:
                if (checkExistsWithValue(record, DCM_StudyInstanceUID))
                {
                    result = compare(getStringFromDataset(record, DCM_StudyInstanceUID, recordString),
                                     getStringFromDataset(dataset, DCM_StudyInstanceUID, datasetString));
                } else {
                    /* the Study Instance UID can be in the referenced file instead */
                    OFString refFilename;
                    if (!getStringFromDataset(record, DCM_ReferencedFileID, refFilename).empty())
                    {
                        OFString hostFilename;
                        if (locateDicomFile(refFilename, hostFilename))
                        {
                            result = compare(getStringFromFile(hostFilename, DCM_StudyInstanceUID, recordString),
                                             getStringFromDataset(dataset, DCM_StudyInstanceUID, datasetString));
                        } else
                            DCMDATA_ERROR("cannot locate referenced file: " << refFilename);
                    }
                }
                break;
            case ERT_Series:
                result = compare(getStringFromDataset(record, DCM_SeriesInstanceUID, recordString),
                                 getStringFromDataset(dataset, DCM_SeriesInstanceUID, datasetString));
                break;
            case ERT_Image:
            case ERT_Overlay:
            case ERT_Curve:
            case ERT_ModalityLut:
            case ERT_VoiLut:
            case ERT_SRDocument:
            case ERT_Presentation:
            case ERT_Waveform:
            case ERT_RTDose:
            case ERT_RTStructureSet:
            case ERT_RTPlan:
            case ERT_RTTreatRecord:
            case ERT_StoredPrint:
            case ERT_KeyObjectDoc:
            case ERT_Registration:
            case ERT_Fiducial:
            case ERT_RawData:
            case ERT_Spectroscopy:
            case ERT_EncapDoc:
            case ERT_ValueMap:
            case ERT_HangingProtocol:
            case ERT_Stereometric:
            case ERT_Palette:
            case ERT_Surface:
            case ERT_Measurement:
            case ERT_Implant:
            case ERT_ImplantGroup:
            case ERT_ImplantAssy:
            case ERT_Plan:
            case ERT_SurfaceScan:
            case ERT_Tract:
            case ERT_Assessment:
            case ERT_Radiotherapy:
                /* The attribute ReferencedSOPInstanceUID is automatically
                 * put into a Directory Record when a filename is present.
                */
                result = compare(getStringFromDataset(record, DCM_ReferencedSOPInstanceUIDInFile, recordString),
                                 getStringFromDataset(dataset, DCM_SOPInstanceUID, datasetString));
                break;
            default:
                DCMDATA_ERROR("record type not yet implemented");
                break;
        }
    }
    return result;
}


// search for a given record
DcmDirectoryRecord *DicomDirInterface::findExistingRecord(DcmDirectoryRecord *parent,
                                                          const E_DirRecType recordType,
                                                          DcmItem* dataset)
{
    OFBool found = OFFalse;
    DcmDirectoryRecord *record = NULL;
    if (parent != NULL)
    {
        /* iterate over all records */
        while (!found && ((record = parent->nextSub(record)) != NULL))
        {
            if (record->getRecordType() == recordType)
                found = recordMatchesDataset(record, dataset);
        }
    }
    return (found) ? record : NULL;
}


// create or update patient record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildPatientRecord(DcmDirectoryRecord *record,
                                                          DcmFileFormat *fileformat,
                                                          const OFFilename &sourceFilename)
{
    /* create new patient record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Patient, NULL, sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* use type 1C instead of 1 in order to avoid unwanted overwriting */
            copyElementType1C(dataset, DCM_PatientID, record, sourceFilename);
            copyElementType2(dataset, DCM_PatientName, record, sourceFilename);
            if ((ApplicationProfile == AP_GeneralPurposeDVDJPEG) ||
                (ApplicationProfile == AP_GeneralPurposeDVDJPEG2000) ||
                (ApplicationProfile == AP_GeneralPurposeBDJPEG) ||
                (ApplicationProfile == AP_GeneralPurposeBDJPEG2000) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatML) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatHL) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41BD) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_2D) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_3D) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4StereoHPatLV42) ||
                (ApplicationProfile == AP_USBandFlashJPEG) ||
                (ApplicationProfile == AP_USBandFlashJPEG2000) ||
                (ApplicationProfile == AP_MPEG2MPatMLDVD))
            {
                /* additional type 1C keys specified by specific profiles */
                copyElementType1C(dataset, DCM_PatientBirthDate, record, sourceFilename);
                copyElementType1C(dataset, DCM_PatientSex, record, sourceFilename);
            }
            else if ((ApplicationProfile == AP_BasicCardiac) ||
                     (ApplicationProfile == AP_XrayAngiographic) ||
                     (ApplicationProfile == AP_XrayAngiographicDVD))
            {
                /* additional type 2 keys specified by specific profiles */
                copyElementType2(dataset, DCM_PatientBirthDate, record, sourceFilename);
                copyElementType2(dataset, DCM_PatientSex, record, sourceFilename);
            }
        } else {
            printRecordErrorMessage(record->error(), ERT_Patient, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        DCMDATA_ERROR("out of memory (creating patient record)");
    return record;
}


// create or update study record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStudyRecord(DcmDirectoryRecord *record,
                                                        DcmFileFormat *fileformat,
                                                        const OFFilename &sourceFilename)
{
    /* create new study record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Study, NULL, sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            OFString tmpString;
            /* copy attribute values from dataset to study record */
            copyStringWithDefault(dataset, DCM_StudyDate, record, sourceFilename, alternativeStudyDate(dataset, tmpString).c_str(), OFTrue /*printWarning*/);
            copyStringWithDefault(dataset, DCM_StudyTime, record, sourceFilename, alternativeStudyTime(dataset, tmpString).c_str(), OFTrue /*printWarning*/);
            copyElementType2(dataset, DCM_StudyDescription, record, sourceFilename);
            copyElementType1(dataset, DCM_StudyInstanceUID, record, sourceFilename);
            /* use type 1C instead of 1 in order to avoid unwanted overwriting */
            copyElementType1C(dataset, DCM_StudyID, record, sourceFilename);
            copyElementType2(dataset, DCM_AccessionNumber, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Study, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        DCMDATA_ERROR("out of memory (creating study record)");
    return record;
}


// create or update series record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildSeriesRecord(DcmDirectoryRecord *record,
                                                         DcmFileFormat *fileformat,
                                                         const OFFilename &sourceFilename)
{
    /* create new series record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Series, NULL, sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to series record */
            copyElementType1(dataset, DCM_Modality, record, sourceFilename);
            copyElementType1(dataset, DCM_SeriesInstanceUID, record, sourceFilename);
            /* use type 1C instead of 1 in order to avoid unwanted overwriting */
            copyElementType1C(dataset, DCM_SeriesNumber, record, sourceFilename);
            if ((ApplicationProfile == AP_GeneralPurposeDVDJPEG) ||
                (ApplicationProfile == AP_GeneralPurposeDVDJPEG2000) ||
                (ApplicationProfile == AP_GeneralPurposeBDJPEG) ||
                (ApplicationProfile == AP_GeneralPurposeBDJPEG2000) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatML) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatHL) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41BD) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_2D) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_3D) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4StereoHPatLV42) ||
                (ApplicationProfile == AP_USBandFlashJPEG) ||
                (ApplicationProfile == AP_USBandFlashJPEG2000) ||
                (ApplicationProfile == AP_MPEG2MPatMLDVD))
            {
                /* additional type 1C keys specified by specific profiles */
                copyElementType1C(dataset, DCM_InstitutionName, record, sourceFilename);
                copyElementType1C(dataset, DCM_InstitutionAddress, record, sourceFilename);
                copyElementType1C(dataset, DCM_PerformingPhysicianName, record, sourceFilename);
            }
            else if ((ApplicationProfile == AP_BasicCardiac) ||
                     (ApplicationProfile == AP_XrayAngiographic) ||
                     (ApplicationProfile == AP_XrayAngiographicDVD))
            {
                /* additional type 2 keys specified by specific profiles (type 1C or 3 in file) */
                copyStringWithDefault(dataset, DCM_InstitutionName, record, sourceFilename);
                copyStringWithDefault(dataset, DCM_InstitutionAddress, record, sourceFilename);
                copyStringWithDefault(dataset, DCM_PerformingPhysicianName, record, sourceFilename);
            }
        } else {
            printRecordErrorMessage(record->error(), ERT_Series, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        DCMDATA_ERROR("out of memory (creating series record)");
    return record;
}


// create or update overlay record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildOverlayRecord(DcmDirectoryRecord *record,
                                                          DcmFileFormat *fileformat,
                                                          const OFString &referencedFileID,
                                                          const OFFilename &sourceFilename)
{
    /* create new overlay record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Overlay, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to overlay record */
            copyElementType1(dataset, DCM_RETIRED_OverlayNumber, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Overlay, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Overlay, "create");
    return record;
}


// create or update modality lut record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildModalityLutRecord(DcmDirectoryRecord *record,
                                                              DcmFileFormat *fileformat,
                                                              const OFString &referencedFileID,
                                                              const OFFilename &sourceFilename)
{
    /* create new modality lut record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_ModalityLut, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to modality lut record */
            copyElementType1(dataset, DCM_RETIRED_LUTNumber, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_ModalityLut, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_ModalityLut, "create");
    return record;
}


// create or update voi lut record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildVoiLutRecord(DcmDirectoryRecord *record,
                                                         DcmFileFormat *fileformat,
                                                         const OFString &referencedFileID,
                                                         const OFFilename &sourceFilename)
{
    /* create new voi lut record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_VoiLut, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to voi lut record */
            copyElementType1(dataset, DCM_RETIRED_LUTNumber, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_VoiLut, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_VoiLut, "create");
    return record;
}


// create or update curve record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildCurveRecord(DcmDirectoryRecord *record,
                                                        DcmFileFormat *fileformat,
                                                        const OFString &referencedFileID,
                                                        const OFFilename &sourceFilename)
{
    /* create new curve record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Curve, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to curve record */
            copyElementType1(dataset, DCM_RETIRED_CurveNumber, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Curve, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Curve, "create");
    return record;
}


// create or update structure reporting record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStructReportRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new struct report record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_SRDocument, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            OFString tmpString;
            /* copy attribute values from dataset to struct report record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_CompletionFlag, record, sourceFilename);
            copyElementType1(dataset, DCM_VerificationFlag, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            if (compare(getStringFromDataset(dataset, DCM_VerificationFlag, tmpString), "VERIFIED"))
            {
                /* VerificationDateTime is required if verification flag is VERIFIED,
                   retrieve most recent (= last) entry from VerifyingObserverSequence */
                DcmItem *ditem = NULL;
                OFCondition status = dataset->findAndGetSequenceItem(DCM_VerifyingObserverSequence, ditem, -1 /*last*/);
                if (status.good())
                    copyElementType1(ditem, DCM_VerificationDateTime, record, sourceFilename);
                else
                    printAttributeErrorMessage(DCM_VerifyingObserverSequence, status, "retrieve");
            }
            copyElementType1(dataset, DCM_ConceptNameCodeSequence, record, sourceFilename);
            addConceptModContentItems(record, dataset);
        } else {
            printRecordErrorMessage(record->error(), ERT_SRDocument, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_SRDocument, "create");
    return record;
}


// create or update presentation state record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildPresentationRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new presentation record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Presentation, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to presentation record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType1(dataset, DCM_PresentationCreationDate, record, sourceFilename);
            copyElementType1(dataset, DCM_PresentationCreationTime, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
            copyElementType1C(dataset, DCM_ReferencedSeriesSequence, record, sourceFilename);
            addBlendingSequence(record, dataset);
        } else {
            printRecordErrorMessage(record->error(), ERT_Presentation, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Presentation, "create");
    return record;
}


// create or update waveform record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildWaveformRecord(DcmDirectoryRecord *record,
                                                           DcmFileFormat *fileformat,
                                                           const OFString &referencedFileID,
                                                           const OFFilename &sourceFilename)
{
    /* create new waveform record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Waveform, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to waveform record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Waveform, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Waveform, "create");
    return record;
}


// create or update rt dose record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTDoseRecord(DcmDirectoryRecord *record,
                                                         DcmFileFormat *fileformat,
                                                         const OFString &referencedFileID,
                                                         const OFFilename &sourceFilename)
{
    /* create new rt dose record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_RTDose, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to rt dose record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_DoseSummationType, record, sourceFilename);
            copyElementType3(dataset, DCM_DoseComment, record, sourceFilename);
            /* copy existing icon image (if present) */
            copyElementType3(dataset, DCM_IconImageSequence, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_RTDose, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_RTDose, "create");
    return record;
}


// create or update rt structure set record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTStructureSetRecord(DcmDirectoryRecord *record,
                                                                 DcmFileFormat *fileformat,
                                                                 const OFString &referencedFileID,
                                                                 const OFFilename &sourceFilename)
{
    /* create new rt structure set record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_RTStructureSet, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to rt structure set record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_StructureSetLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_StructureSetDate, record, sourceFilename);
            copyElementType2(dataset, DCM_StructureSetTime, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_RTStructureSet, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_RTStructureSet, "create");
    return record;
}


// create or update rt plan record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTPlanRecord(DcmDirectoryRecord *record,
                                                         DcmFileFormat *fileformat,
                                                         const OFString &referencedFileID,
                                                         const OFFilename &sourceFilename)
{
    /* create new rt plan record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_RTPlan, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to rt plan record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_RTPlanLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_RTPlanDate, record, sourceFilename);
            copyElementType2(dataset, DCM_RTPlanTime, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_RTPlan, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_RTPlan, "create");
    return record;
}


// create or update rt treatment record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTTreatmentRecord(DcmDirectoryRecord *record,
                                                              DcmFileFormat *fileformat,
                                                              const OFString &referencedFileID,
                                                              const OFFilename &sourceFilename)
{
    /* create new rt treatment record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_RTTreatRecord, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to rt treatment record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType2(dataset, DCM_TreatmentDate, record, sourceFilename);
            copyElementType2(dataset, DCM_TreatmentTime, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_RTTreatRecord, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_RTTreatRecord, "create");
    return record;
}


// create or update stored print record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStoredPrintRecord(DcmDirectoryRecord *record,
                                                              DcmFileFormat *fileformat,
                                                              const OFString &referencedFileID,
                                                              const OFFilename &sourceFilename)
{
    /* create new stored print record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_StoredPrint, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to stored print record */
            copyElementType2(dataset, DCM_InstanceNumber, record, sourceFilename);
            /* IconImageSequence (type 3) is not created for the referenced images */
        } else {
            printRecordErrorMessage(record->error(), ERT_StoredPrint, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_StoredPrint, "create");
    return record;
}


// create or update key object doc record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildKeyObjectDocRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new key object doc record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_KeyObjectDoc, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to key object doc record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_ConceptNameCodeSequence, record, sourceFilename);
            addConceptModContentItems(record, dataset);
        } else {
            printRecordErrorMessage(record->error(), ERT_KeyObjectDoc, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_KeyObjectDoc, "create");
    return record;
}


// create or update registration record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRegistrationRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new registration record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Registration, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to registration record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Registration, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Registration, "create");
    return record;
}


// create or update fiducial record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildFiducialRecord(DcmDirectoryRecord *record,
                                                           DcmFileFormat *fileformat,
                                                           const OFString &referencedFileID,
                                                           const OFFilename &sourceFilename)
{
    /* create new fiducial record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Fiducial, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to fiducial record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Fiducial, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Fiducial, "create");
    return record;
}


// create or update raw data record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRawDataRecord(DcmDirectoryRecord *record,
                                                          DcmFileFormat *fileformat,
                                                          const OFString &referencedFileID,
                                                          const OFFilename &sourceFilename)
{
    /* create new raw data record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_RawData, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to raw data record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType2(dataset, DCM_InstanceNumber, record, sourceFilename);
            /* IconImageSequence (type 3) is not created for the raw data */
        } else {
            printRecordErrorMessage(record->error(), ERT_RawData, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_RawData, "create");
    return record;
}


// create or update spectroscopy record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildSpectroscopyRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new spectroscopy record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Spectroscopy, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to spectroscopy record */
            copyElementType1(dataset, DCM_ImageType, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1C(dataset, DCM_ReferencedImageEvidenceSequence, record, sourceFilename);
            copyElementType1(dataset, DCM_NumberOfFrames, record, sourceFilename);
            copyElementType1(dataset, DCM_Rows, record, sourceFilename);
            copyElementType1(dataset, DCM_Columns, record, sourceFilename);
            copyElementType1(dataset, DCM_DataPointRows, record, sourceFilename);
            copyElementType1(dataset, DCM_DataPointColumns, record, sourceFilename);
            /* IconImageSequence (type 3) is not created for the spectroscopy data */
            /* application profile specific attributes */
            if ((ApplicationProfile == AP_GeneralPurposeDVDJPEG) ||
                (ApplicationProfile == AP_GeneralPurposeDVDJPEG2000) ||
                (ApplicationProfile == AP_GeneralPurposeBDJPEG) ||
                (ApplicationProfile == AP_GeneralPurposeBDJPEG2000) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatML) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG2MPatHL) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV41BD) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_2D) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4HPatLV42_3D) ||
                (ApplicationProfile == AP_GeneralPurposeBDMPEG4StereoHPatLV42) ||
                (ApplicationProfile == AP_USBandFlashJPEG) ||
                (ApplicationProfile == AP_USBandFlashJPEG2000))
            {
                copyElementType1(dataset, DCM_Rows, record, sourceFilename);
                copyElementType1(dataset, DCM_Columns, record, sourceFilename);
                copyElementType1C(dataset, DCM_FrameOfReferenceUID, record, sourceFilename);
                copyElementType1C(dataset, DCM_SynchronizationFrameOfReferenceUID, record, sourceFilename);
                copyElementType1C(dataset, DCM_NumberOfFrames, record, sourceFilename);
                copyElementType1C(dataset, DCM_AcquisitionTimeSynchronized, record, sourceFilename);
                copyElementType1C(dataset, DCM_AcquisitionDateTime, record, sourceFilename);
                // also need to examine functional groups for the following attributes
                copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_ReferencedImageSequence, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
                copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_ImagePositionPatient, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
                copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_ImageOrientationPatient, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
                copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_PixelSpacing, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
            }
        } else {
            printRecordErrorMessage(record->error(), ERT_Spectroscopy, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Spectroscopy, "create");
    return record;
}


// create or update encap doc record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildEncapDocRecord(DcmDirectoryRecord *record,
                                                           DcmFileFormat *fileformat,
                                                           const OFString &referencedFileID,
                                                           const OFFilename &sourceFilename)
{
    /* create new encap doc record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_EncapDoc, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to encap doc record */
            copyElementType2(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType2(dataset, DCM_DocumentTitle, record, sourceFilename);
            /* required if encapsulated document is an HL7 Structured Document */
            copyElementType1C(dataset, DCM_HL7InstanceIdentifier, record, sourceFilename);
            /* baseline context group 7020 is not checked */
            copyElementType2(dataset, DCM_ConceptNameCodeSequence, record, sourceFilename);
            copyElementType1(dataset, DCM_MIMETypeOfEncapsulatedDocument, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_EncapDoc, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_RawData, "create");
    return record;
}


// create or update value map record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildValueMapRecord(DcmDirectoryRecord *record,
                                                           DcmFileFormat *fileformat,
                                                           const OFString &referencedFileID,
                                                           const OFFilename &sourceFilename)
{
    /* create new value map record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_ValueMap, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to value map record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_ValueMap, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_ValueMap, "create");
    return record;
}


// create or update hanging protocol record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildHangingProtocolRecord(DcmDirectoryRecord *record,
                                                                  DcmFileFormat *fileformat,
                                                                  const OFString &referencedFileID,
                                                                  const OFFilename &sourceFilename)
{
    /* create new hanging protocol record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_HangingProtocol, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            copyElementType1(dataset, DCM_HangingProtocolName, record, sourceFilename);
            copyElementType1(dataset, DCM_HangingProtocolDescription, record, sourceFilename);
            copyElementType1(dataset, DCM_HangingProtocolLevel, record, sourceFilename);
            copyElementType1(dataset, DCM_HangingProtocolCreator, record, sourceFilename);
            copyElementType1(dataset, DCM_HangingProtocolCreationDateTime, record, sourceFilename);
            // open issue: shall we do further checks on the sequence content?
            copyElementType1(dataset, DCM_HangingProtocolDefinitionSequence, record, sourceFilename);
            copyElementType1(dataset, DCM_NumberOfPriorsReferenced, record, sourceFilename);
            copyElementType2(dataset, DCM_HangingProtocolUserIdentificationCodeSequence, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_HangingProtocol, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        DCMDATA_ERROR("out of memory (creating patient record)");
    return record;
}


// create or update stereometric record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStereometricRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new value map record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Stereometric, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* nothing to do */
        } else {
            printRecordErrorMessage(record->error(), ERT_Stereometric, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Stereometric, "create");
    return record;
}


// create or update palette record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildPaletteRecord(DcmDirectoryRecord *record,
                                                          DcmFileFormat *fileformat,
                                                          const OFString &referencedFileID,
                                                          const OFFilename &sourceFilename)
{
    /* create new palette record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Palette, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to palette record */
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Palette, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Palette, "create");
    return record;
}


// create or update surface record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildSurfaceRecord(DcmDirectoryRecord *record,
                                                          DcmFileFormat *fileformat,
                                                          const OFString &referencedFileID,
                                                          const OFFilename &sourceFilename)
{
    /* create new surface record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Surface, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to surface record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Surface, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Surface, "create");
    return record;
}


// create or update measurement record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildMeasurementRecord(DcmDirectoryRecord *record,
                                                              DcmFileFormat *fileformat,
                                                              const OFString &referencedFileID,
                                                              const OFFilename &sourceFilename)
{
    /* create new measurement record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Measurement, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to measurement record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Measurement, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Measurement, "create");
    return record;
}


// create or update implant record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildImplantRecord(DcmDirectoryRecord *record,
                                                          DcmFileFormat *fileformat,
                                                          const OFString &referencedFileID,
                                                          const OFFilename &sourceFilename)
{
    /* create new implant record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Implant, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to implant record */
            copyElementType1(dataset, DCM_Manufacturer, record, sourceFilename);
            copyElementType1(dataset, DCM_ImplantName, record, sourceFilename);
            copyElementType1C(dataset, DCM_ImplantSize, record, sourceFilename);
            copyElementType1(dataset, DCM_ImplantPartNumber, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Implant, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Implant, "create");
    return record;
}


// create or update implant group record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildImplantGroupRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new implant group record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_ImplantGroup, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to implant group record */
            copyElementType1(dataset, DCM_ImplantAssemblyTemplateName, record, sourceFilename);
            copyElementType1(dataset, DCM_ImplantAssemblyTemplateIssuer, record, sourceFilename);
            copyElementType1(dataset, DCM_ProcedureTypeCodeSequence, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_ImplantGroup, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_ImplantGroup, "create");
    return record;
}


// create or update implant assy record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildImplantAssyRecord(DcmDirectoryRecord *record,
                                                              DcmFileFormat *fileformat,
                                                              const OFString &referencedFileID,
                                                              const OFFilename &sourceFilename)
{
    /* create new implant assy record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_ImplantAssy, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to implant assy record */
            copyElementType1(dataset, DCM_ImplantTemplateGroupName, record, sourceFilename);
            copyElementType3(dataset, DCM_ImplantTemplateGroupDescription, record, sourceFilename);
            copyElementType1(dataset, DCM_ImplantTemplateGroupIssuer, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_ImplantAssy, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_ImplantAssy, "create");
    return record;
}


// create or update plan record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildPlanRecord(DcmDirectoryRecord *record,
                                                       DcmFileFormat *fileformat,
                                                       const OFString &referencedFileID,
                                                       const OFFilename &sourceFilename)
{
    /* create new plan record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Plan, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* nothing to do */
        } else {
            printRecordErrorMessage(record->error(), ERT_Plan, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Plan, "create");
    return record;
}


// create or update surface scan record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildSurfaceScanRecord(DcmDirectoryRecord *record,
                                                              DcmFileFormat *fileformat,
                                                              const OFString &referencedFileID,
                                                              const OFFilename &sourceFilename)
{
    /* create new surface scan record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_SurfaceScan, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to surface scan record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_SurfaceScan, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_SurfaceScan, "create");
    return record;
}


// create or update tract record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildTractRecord(DcmDirectoryRecord *record,
                                                        DcmFileFormat *fileformat,
                                                        const OFString &referencedFileID,
                                                        const OFFilename &sourceFilename)
{
    /* create new surface record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Tract, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to tract record */
            copyElementType1(dataset, DCM_ContentDate, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentTime, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_ContentLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Tract, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Tract, "create");
    return record;
}


// create or update assessment record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildAssessmentRecord(DcmDirectoryRecord *record,
                                                             DcmFileFormat *fileformat,
                                                             const OFString &referencedFileID,
                                                             const OFFilename &sourceFilename)
{
    /* create new surface record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Assessment, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to assessment record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1(dataset, DCM_InstanceCreationDate, record, sourceFilename);
            copyElementType2(dataset, DCM_InstanceCreationTime, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Assessment, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Assessment, "create");
    return record;
}


// create or update radiotherapy record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRadiotherapyRecord(DcmDirectoryRecord *record,
                                                               DcmFileFormat *fileformat,
                                                               const OFString &referencedFileID,
                                                               const OFFilename &sourceFilename)
{
    /* create new surface record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Radiotherapy, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            /* copy attribute values from dataset to radiotherapy record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            copyElementType1C(dataset, DCM_UserContentLabel, record, sourceFilename);
            copyElementType1C(dataset, DCM_UserContentLongLabel, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentDescription, record, sourceFilename);
            copyElementType2(dataset, DCM_ContentCreatorName, record, sourceFilename);
        } else {
            printRecordErrorMessage(record->error(), ERT_Radiotherapy, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printRecordErrorMessage(EC_MemoryExhausted, ERT_Radiotherapy, "create");
    return record;
}


// create or update image record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildImageRecord(DcmDirectoryRecord *record,
                                                        DcmFileFormat *fileformat,
                                                        const OFString &referencedFileID,
                                                        const OFFilename &sourceFilename)
{
    /* create new image record */
    if (record == NULL)
        record = new DcmDirectoryRecord(ERT_Image, referencedFileID.c_str(), sourceFilename, fileformat);
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            DcmDataset *dataset = fileformat->getDataset();
            OFBool iconImage = IconImageMode;
            unsigned int iconSize = (IconSize == 0) ? 64 : IconSize;
            /* Icon Image Sequence required for particular profiles */
            OFBool iconRequired = OFFalse;
            /* copy attribute values from dataset to image record */
            copyElementType1(dataset, DCM_InstanceNumber, record, sourceFilename);
            /* application profile specific attributes */
            switch (ApplicationProfile)
            {
                case AP_GeneralPurpose:
                    copyElementType1C(dataset, DCM_ImageType, record, sourceFilename);
                    copyElementType1C(dataset, DCM_ReferencedImageSequence, record, sourceFilename);
                    break;
                case AP_GeneralPurposeDVDJPEG:
                case AP_GeneralPurposeDVDJPEG2000:
                case AP_GeneralPurposeBDJPEG:
                case AP_GeneralPurposeBDJPEG2000:
                case AP_GeneralPurposeBDMPEG2MPatML:
                case AP_GeneralPurposeBDMPEG2MPatHL:
                case AP_GeneralPurposeBDMPEG4HPatLV41:
                case AP_GeneralPurposeBDMPEG4HPatLV41BD:
                case AP_GeneralPurposeBDMPEG4HPatLV42_2D:
                case AP_GeneralPurposeBDMPEG4HPatLV42_3D:
                case AP_GeneralPurposeBDMPEG4StereoHPatLV42:
                case AP_USBandFlashJPEG:
                case AP_USBandFlashJPEG2000:
                    copyElementType1(dataset, DCM_Rows, record, sourceFilename);
                    copyElementType1(dataset, DCM_Columns, record, sourceFilename);
                    copyElementType1C(dataset, DCM_ImageType, record, sourceFilename);
                    copyElementType1C(dataset, DCM_CalibrationImage, record, sourceFilename);
                    copyElementType1C(dataset, DCM_LossyImageCompressionRatio, record, sourceFilename);
                    copyElementType1C(dataset, DCM_FrameOfReferenceUID, record, sourceFilename);
                    copyElementType1C(dataset, DCM_SynchronizationFrameOfReferenceUID, record, sourceFilename);
                    copyElementType1C(dataset, DCM_NumberOfFrames, record, sourceFilename);
                    copyElementType1C(dataset, DCM_AcquisitionTimeSynchronized, record, sourceFilename);
                    copyElementType1C(dataset, DCM_AcquisitionDateTime, record, sourceFilename);
                    // also need to examine functional groups for the following attributes
                    copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_ReferencedImageSequence, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
                    copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_ImagePositionPatient, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
                    copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_ImageOrientationPatient, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
                    copyElementType1CFromDatasetOrSequenceItem(dataset, DCM_PixelSpacing, DCM_SharedFunctionalGroupsSequence, record, sourceFilename);
                    break;
                case AP_MPEG2MPatMLDVD:
                    copyElementType1(dataset, DCM_Rows, record, sourceFilename);
                    copyElementType1(dataset, DCM_Columns, record, sourceFilename);
                    copyElementType1C(dataset, DCM_ImageType, record, sourceFilename);
                    copyElementType1C(dataset, DCM_LossyImageCompressionRatio, record, sourceFilename);
                    break;
                case AP_XrayAngiographic:
                case AP_XrayAngiographicDVD:
                    copyElementType1C(dataset, DCM_LossyImageCompressionRatio, record, sourceFilename);
                case AP_BasicCardiac:
                    {
                        OFString tmpString;
                        OFBool xaImage = compare(getStringFromDataset(dataset, DCM_SOPClassUID, tmpString), UID_XRayAngiographicImageStorage);
                        if (xaImage)
                        {
                            /* type 1C: required for XA images (type 1 for Basic Cardiac Profile) */
                            copyElementType1(dataset, DCM_ImageType, record, sourceFilename);
                            /* type 1C: required if ImageType value 3 is "BIPLANE A" or "BIPLANE B" */
                            getStringComponentFromDataset(dataset, DCM_ImageType, tmpString, 2);
                            if (compare(tmpString, "BIPLANE A") || compare(tmpString, "BIPLANE B"))
                                copyElementType1(dataset, DCM_ReferencedImageSequence, record, sourceFilename);
                        }
                        /* additional type 2 keys specified by specific profiles (type 3 in image IOD) */
                        copyStringWithDefault(dataset, DCM_CalibrationImage, record, sourceFilename);
                        /* icon images */
                        iconImage = OFTrue;
                        iconRequired = OFTrue;
                        iconSize = 128;
                    }
                    break;
                case AP_CTandMR:
                    copyElementType1(dataset, DCM_Rows, record, sourceFilename);
                    copyElementType1(dataset, DCM_Columns, record, sourceFilename);
                    copyElementType1C(dataset, DCM_ReferencedImageSequence, record, sourceFilename);
                    copyElementType1C(dataset, DCM_ImagePositionPatient, record, sourceFilename);
                    copyElementType1C(dataset, DCM_ImageOrientationPatient, record, sourceFilename);
                    copyElementType1C(dataset, DCM_FrameOfReferenceUID, record, sourceFilename);
                    copyElementType1C(dataset, DCM_PixelSpacing, record, sourceFilename);
                    /* icon images */
                    iconImage = OFTrue;
                    iconSize = 64;
                    break;
                default:
                    /* no additional keys */
                    break;
            }
            /* create icon images */
            if (iconImage)
            {
                OFCondition status = addIconImage(record, dataset, iconSize, sourceFilename);
                if (status.bad())
                {
                    /* report error or warning */
                    if (iconRequired)
                        DCMDATA_ERROR("cannot create IconImageSequence");
                    else
                        DCMDATA_WARN("cannot create IconImageSequence");
                }
            }
        } else {
            printRecordErrorMessage(record->error(), ERT_Series, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        DCMDATA_ERROR("out of memory (creating image record)");
    return record;
}


// create icon image from PGM file
OFBool DicomDirInterface::getIconFromFile(const OFFilename &filename,
                                          Uint8 *pixel,
                                          const unsigned long count,
                                          const unsigned int width,
                                          const unsigned int height)
{
    OFBool result = OFFalse;
    /* check buffer and size */
    if (!filename.isEmpty() && (pixel != NULL) && (count >= width * height))
    {
        /* open specified file */
        OFFile file;
        if (file.fopen(filename, "rb"))
        {
            /* according to the pgm format no line should be longer than 70 characters */
            const int maxline = 256;
            char line[maxline];
            /* read magic number */
            if ((file.fgets(line, maxline) != NULL) && (strcmp(line, "P5\n") == 0))
            {
                OFBool corrupt = OFTrue;
                if ((file.fgets(line, maxline) != NULL) && (line[0] != '\0'))
                {
                    unsigned int pgmWidth, pgmHeight = 0;
                    /* skip optional comment line and get width and height */
                    if (((*line != '#') || (file.fgets(line, maxline) != NULL)) &&
                        (sscanf(line, "%u %u", &pgmWidth, &pgmHeight) > 0) && (pgmWidth > 0) && (pgmHeight > 0))
                    {
                        unsigned int pgmMax = 0;
                        /* get maximum gray value */
                        if ((file.fgets(line, maxline) != NULL) && (sscanf(line, "%u", &pgmMax) > 0) && (pgmMax == 255))
                        {
                            const unsigned long pgmSize = pgmWidth * pgmHeight;
                            Uint8 *pgmData = new Uint8[pgmSize];
                            if (pgmData != NULL)
                            {
                                /* get pgm image data */
                                if (file.fread(pgmData, sizeof(Uint8), OFstatic_cast(size_t, pgmSize)) == pgmSize)
                                {
                                    /* if already scaled, just copy the bitmap */
                                    if ((width == pgmWidth) && (height == pgmHeight) && (count == pgmSize))
                                    {
                                        OFBitmanipTemplate<Uint8>::copyMem(pgmData, pixel, count);
                                        result = OFTrue;
                                    }
                                    else if (ImagePlugin != NULL)
                                    {
                                        /* scale image to requested size */
                                        result = ImagePlugin->scaleData(pgmData, pgmWidth, pgmHeight, pixel, width, height);
                                    }
                                    if (!result)
                                        DCMDATA_ERROR("cannot scale external icon, no image support available");
                                    corrupt = OFFalse;
                                }
                                /* free memory */
                                delete[] pgmData;
                            } else {
                                DCMDATA_ERROR(OFCondition(EC_MemoryExhausted).text() << ": cannot allocate memory for pgm pixel data");
                                /* avoid double reporting of error message */
                                corrupt = OFFalse;
                            }
                        }
                    }
                }
                if (corrupt)
                    DCMDATA_ERROR("corrupt file format for external icon (not pgm binary)");
            } else
                DCMDATA_ERROR("wrong file format for external icon (pgm required)");
        } else
            DCMDATA_ERROR("cannot open file for external icon: " << filename);
    }
    return result;
}


// create icon image from DICOM dataset
OFBool DicomDirInterface::getIconFromDataset(DcmItem *dataset,
                                             Uint8 *pixel,
                                             const unsigned long count,
                                             const unsigned int width,
                                             const unsigned int height)
{
    OFBool result = OFFalse;
    /* check parameters (incl. availability of image plugin) */
    if ((ImagePlugin != NULL) && (dataset != NULL) && (pixel != NULL) && (count >= width * height))
    {
        /* choose representative frame */
        long fCount, frame;
        dataset->findAndGetLongInt(DCM_NumberOfFrames, fCount);
        dataset->findAndGetLongInt(DCM_RepresentativeFrameNumber, frame);
        if (fCount <= 0)
            fCount = 1;
        if (frame <= 0)
        {
            if (fCount > 3)
                frame = fCount / 3;     // recommended in PS3.11
            else
                frame = 1;
        } else if (frame > fCount)
            frame = fCount;
        /* scale image (if required) and retrieve pixel data from dataset */
        result = ImagePlugin->scaleImage(dataset, pixel, count, OFstatic_cast(unsigned long, frame),
            width, height, (fCount == 1) /*decompressAll*/);
    }
    return result;
}


// add icon image sequence to record
OFCondition DicomDirInterface::addIconImage(DcmDirectoryRecord *record,
                                            DcmItem *dataset,
                                            const unsigned int size,
                                            const OFFilename &sourceFilename)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((record != NULL) && (dataset != NULL))
    {
        DcmItem *ditem = NULL;
        /* create icon image sequence with one item */
        result = record->findOrCreateSequenceItem(DCM_IconImageSequence, ditem);
        if (result.good())
        {
            const unsigned int width = size;
            const unsigned int height = size;
            const unsigned long count = width * height;
            /* Image Pixel Module */
            ditem->putAndInsertUint16(DCM_SamplesPerPixel, 1);
            ditem->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
            ditem->putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, height));
            ditem->putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, width));
            ditem->putAndInsertUint16(DCM_BitsAllocated, 8);
            ditem->putAndInsertUint16(DCM_BitsStored, 8);
            ditem->putAndInsertUint16(DCM_HighBit, 7);
            ditem->putAndInsertUint16(DCM_PixelRepresentation, 0);
            /* Pixel Data */
            Uint8 *pixel = new Uint8[count];
            if (pixel != NULL)
            {
                OFBool iconOk = OFFalse;
                /* prefix for external icons specified? */
                if (!IconPrefix.isEmpty())
                {
                    /* try to load external pgm icon */
                    OFFilename filename;
                    OFStandard::appendFilenameExtension(filename, IconPrefix, sourceFilename);
                    iconOk = getIconFromFile(filename, pixel, count, width, height);
                } else {
                    /* try to create icon from dataset */
                    iconOk = getIconFromDataset(dataset, pixel, count, width, height);
                    if (!iconOk)
                        DCMDATA_WARN("cannot create monochrome icon from image file, using default");
                }
                /* could not create icon so far: use default icon (if specified) */
                if (!iconOk && !DefaultIcon.isEmpty())
                    iconOk = getIconFromFile(DefaultIcon, pixel, count, width, height);
                /* default not available: use black image */
                if (!iconOk)
                    OFBitmanipTemplate<Uint8>::zeroMem(pixel, count);
                /* create Pixel Data element and set pixel data */
                result = ditem->putAndInsertUint8Array(DCM_PixelData, pixel, count);
                /* free pixel data after it has been copied */
                delete[] pixel;
            } else
                result = EC_MemoryExhausted;
            /* remove entire icon image sequence in case of error */
            if (result.bad())
                record->findAndDeleteElement(DCM_IconImageSequence);
        }
    }
    return result;
}


// add child record to a given parent record
DcmDirectoryRecord *DicomDirInterface::addRecord(DcmDirectoryRecord *parent,
                                                 const E_DirRecType recordType,
                                                 DcmFileFormat *fileformat,
                                                 const OFString &referencedFileID,
                                                 const OFFilename &sourceFilename)
{
    DcmDirectoryRecord *record = NULL;
    DcmDataset *dataset = fileformat->getDataset();
    if (parent != NULL)
    {
        /* check whether record already exists */
        DcmDirectoryRecord *oldRecord = record = findExistingRecord(parent, recordType, dataset);
        if ((record == NULL) || FilesetUpdateMode)
        {
            /* in case an existing record is updated */
            if (record != NULL)
            {
                /* perform some consistency checks for instance records */
                if ((recordType != ERT_Patient) && (recordType != ERT_Study) && (recordType != ERT_Series))
                {
                    if (!checkReferencedSOPInstance(record, dataset, referencedFileID, sourceFilename))
                        return NULL;
                }
            }
            /* create a new one or update existing record */
            switch (recordType)
            {
                case ERT_Patient:
                    record = buildPatientRecord(record, fileformat, sourceFilename);
                    break;
                case ERT_Study:
                    record = buildStudyRecord(record, fileformat, sourceFilename);
                    break;
                case ERT_Series:
                    record = buildSeriesRecord(record, fileformat, sourceFilename);
                    break;
                case ERT_Overlay:
                    record = buildOverlayRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_ModalityLut:
                    record = buildModalityLutRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_VoiLut:
                    record = buildVoiLutRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Curve:
                    record = buildCurveRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_SRDocument:
                    record = buildStructReportRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Presentation:
                    record = buildPresentationRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Waveform:
                    record = buildWaveformRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_RTDose:
                    record = buildRTDoseRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_RTStructureSet:
                    record = buildRTStructureSetRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_RTPlan:
                    record = buildRTPlanRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_RTTreatRecord:
                    record = buildRTTreatmentRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_StoredPrint:
                    record = buildStoredPrintRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_KeyObjectDoc:
                    record = buildKeyObjectDocRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Registration:
                    record = buildRegistrationRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Fiducial:
                    record = buildFiducialRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_RawData:
                    record = buildRawDataRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Spectroscopy:
                    record = buildSpectroscopyRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_EncapDoc:
                    record = buildEncapDocRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_ValueMap:
                    record = buildValueMapRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_HangingProtocol:
                    record = buildHangingProtocolRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Stereometric:
                    record = buildStereometricRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Palette:
                    record = buildPaletteRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Surface:
                    record = buildSurfaceRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Measurement:
                    record = buildMeasurementRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Implant:
                    record = buildImplantRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_ImplantGroup:
                    record = buildImplantGroupRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_ImplantAssy:
                    record = buildImplantAssyRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Plan:
                    record = buildPlanRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_SurfaceScan:
                    record = buildSurfaceScanRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Tract:
                    record = buildTractRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Assessment:
                    record = buildAssessmentRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                case ERT_Radiotherapy:
                    record = buildRadiotherapyRecord(record, fileformat, referencedFileID, sourceFilename);
                    break;
                default:
                    /* it can only be an image */
                    record = buildImageRecord(record, fileformat, referencedFileID, sourceFilename);
            }
            if (record != NULL)
            {
                /* type 1C: required if an extended character set is used in one of the record keys */
                if (record->isAffectedBySpecificCharacterSet())
                    copyElementType1C(dataset, DCM_SpecificCharacterSet, record, sourceFilename);
                /* in case a new record has been created */
                if (record != oldRecord)
                {
                    /* insert it below parent record */
                    OFCondition status = insertSortedUnder(parent, record);
                    if (status.bad())
                    {
                        printRecordErrorMessage(status, recordType, "insert");
                        /* free memory */
                        delete record;
                        record = NULL;
                    }
                }
            }
        } else {
            /* instance record is already referenced by the DICOMDIR */
            if ((recordType != ERT_Patient) && (recordType != ERT_Study) && (recordType != ERT_Series))
            {
                /* create warning message */
                DCMDATA_WARN("file " << sourceFilename << ": directory record for this SOP instance already exists");
            }
            /* perform consistency check */
            if (ConsistencyCheck)
            {
                /* abort on any inconsistency */
                if (warnAboutInconsistentAttributes(record, dataset, sourceFilename, AbortMode) && AbortMode)
                    return NULL;
            }
        }
        if (record != NULL)
        {
            /* check whether instance is already listed */
            if (record->getRecordsOriginFile().isEmpty())
                record->setRecordsOriginFile(sourceFilename);
        }
    }
    return record;
}


// check referenced SOP instance for consistency with a new directory record
OFBool DicomDirInterface::checkReferencedSOPInstance(DcmDirectoryRecord *record,
                                                     DcmItem *dataset,
                                                     const OFString &referencedFileID,
                                                     const OFFilename &sourceFilename)
{
    OFBool result = OFTrue;
    if ((record != NULL) && (dataset != NULL))
    {
        OFString refFileID;
        /* check referenced file ID */
        if (record->findAndGetOFStringArray(DCM_ReferencedFileID, refFileID).good() &&
            !compare(refFileID, referencedFileID))
        {
            /* create error message */
            DCMDATA_ERROR("file " << sourceFilename << ": SOP instance already referenced "
                << "with different file ID (" << refFileID << ")");
            result = OFFalse;
        }
        /* check SOP class UID */
        if (!compareStringAttributes(dataset, DCM_SOPClassUID, record, DCM_ReferencedSOPClassUIDInFile, sourceFilename, OFTrue /*errorMsg*/))
            result = OFFalse;
    }
    return result;
}


// invent missing attributes on root level (and below)
void DicomDirInterface::inventMissingAttributes(DcmDirectoryRecord *parent,
                                                const OFBool recurse)
{
    if (parent != NULL)
    {
        DcmDirectoryRecord *record = NULL;
        /* iterate over all child records */
        while ((record = parent->nextSub(record)) != NULL)
        {
            /* only check patient records */
            if (record->getRecordType() == ERT_Patient)
            {
                if (!record->tagExistsWithValue(DCM_PatientID))
                    setDefaultValue(record, DCM_PatientID, AutoPatientNumber++, AUTO_PATIENTID_PREFIX);
                if (recurse)
                    inventMissingStudyLevelAttributes(record);
            }
        }
    }
}


// invent missing attributes on study level (and below)
void DicomDirInterface::inventMissingStudyLevelAttributes(DcmDirectoryRecord *parent)
{
    if (parent != NULL)
    {
        DcmDirectoryRecord *record = NULL;
        /* iterate over all child records */
        while ((record = parent->nextSub(record)) != NULL)
        {
            if (!record->tagExistsWithValue(DCM_StudyID))
                setDefaultValue(record, DCM_StudyID, AutoStudyNumber++, AUTO_STUDYID_PREFIX);
            inventMissingSeriesLevelAttributes(record);
        }
    }
}


// invent missing attributes on series level (and below)
void DicomDirInterface::inventMissingSeriesLevelAttributes(DcmDirectoryRecord *parent)
{
    if (parent != NULL)
    {
        DcmDirectoryRecord *record = NULL;
        /* iterate over all child records */
        while ((record = parent->nextSub(record)) != NULL)
        {
            if (!record->tagExistsWithValue(DCM_SeriesNumber))
                setDefaultValue(record, DCM_SeriesNumber, AutoSeriesNumber++);
            inventMissingInstanceLevelAttributes(record);
        }
    }
}


// invent missing attributes on instance level (and below)
void DicomDirInterface::inventMissingInstanceLevelAttributes(DcmDirectoryRecord *parent)
{
    if (parent != NULL)
    {
        DcmDirectoryRecord *record = NULL;
        /* iterate over all child records */
        while ((record = parent->nextSub(record)) != NULL)
        {
            switch (record->getRecordType())
            {
                case ERT_Image:
                case ERT_RTDose:
                case ERT_RTStructureSet:
                case ERT_RTPlan:
                case ERT_StoredPrint:
                case ERT_Surface:
                    if (!record->tagExistsWithValue(DCM_InstanceNumber))
                        setDefaultValue(record, DCM_InstanceNumber, AutoInstanceNumber++);
                    break;
                case ERT_Overlay:
                    if (!record->tagExistsWithValue(DCM_RETIRED_OverlayNumber))
                        setDefaultValue(record, DCM_RETIRED_OverlayNumber, AutoOverlayNumber++);
                    break;
                case ERT_ModalityLut:
                case ERT_VoiLut:
                    if (!record->tagExistsWithValue(DCM_RETIRED_LUTNumber))
                        setDefaultValue(record, DCM_RETIRED_LUTNumber, AutoLutNumber++);
                    break;
                case ERT_Curve:
                    if (!record->tagExistsWithValue(DCM_RETIRED_CurveNumber))
                        setDefaultValue(record, DCM_RETIRED_CurveNumber, AutoCurveNumber++);
                    break;
                case ERT_SRDocument:
                case ERT_Presentation:
                case ERT_Waveform:
                case ERT_RTTreatRecord:
                case ERT_KeyObjectDoc:
                case ERT_Registration:
                case ERT_Fiducial:
                case ERT_RawData:
                case ERT_Spectroscopy:
                case ERT_EncapDoc:
                case ERT_ValueMap:
                case ERT_Stereometric:
                case ERT_Measurement:
                case ERT_Plan:
                case ERT_SurfaceScan:
                case ERT_Tract:
                case ERT_Assessment:
                    /* nothing to do */
                    break;
                default:
                    /* should never happen */
                    break;
            }
        }
    }
}


// add DICOM file to the current DICOMDIR object
OFCondition DicomDirInterface::addDicomFile(const OFFilename &filename,
                                            const OFFilename &directory)
{
    OFCondition result = EC_IllegalParameter;
    /* first, make sure that a DICOMDIR object exists */
    if (DicomDir != NULL)
    {
        /* create fully qualified pathname of the DICOM file to be added */
        OFFilename pathname;
        OFStandard::combineDirAndFilename(pathname, directory, filename, OFTrue /*allowEmptyDirName*/);
        /* then check the file name, load the file and check the content */
        DcmFileFormat fileformat;
        result = loadAndCheckDicomFile(filename, directory, fileformat, OFTrue /*checkFilename*/);
        if (result.good())
        {
            DCMDATA_INFO("adding file: " << pathname);
            /* start creating the DICOMDIR directory structure */
            DcmDirectoryRecord *rootRecord = &(DicomDir->getRootRecord());
            DcmMetaInfo *metainfo = fileformat.getMetaInfo();
            /* massage filename into DICOM format (DOS conventions for path separators, uppercase) */
            OFString fileID;
            hostToDicomFilename(OFSTRING_GUARD(filename.getCharPointer()), fileID);
            /* what kind of object (SOP Class) is stored in the file */
            OFString sopClass;
            metainfo->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass);
            /* if hanging protocol, palette or implant file then attach it to the root record and stop */
            if (compare(sopClass, UID_HangingProtocolStorage))
            {
                /* add a hanging protocol record below the root */
                if (addRecord(rootRecord, ERT_HangingProtocol, &fileformat, fileID, pathname) == NULL)
                    result = EC_CorruptedData;
            }
            else if (compare(sopClass, UID_ColorPaletteStorage))
            {
                /* add a palette record below the root */
                if (addRecord(rootRecord, ERT_Palette, &fileformat, fileID, pathname) == NULL)
                    result = EC_CorruptedData;
            }
            else if (compare(sopClass, UID_GenericImplantTemplateStorage))
            {
                /* add an implant record below the root */
                if (addRecord(rootRecord, ERT_Implant, &fileformat, fileID, pathname) == NULL)
                    result = EC_CorruptedData;
            }
            else if (compare(sopClass, UID_ImplantAssemblyTemplateStorage))
            {
                /* add an implant group record below the root */
                if (addRecord(rootRecord, ERT_ImplantGroup, &fileformat, fileID, pathname) == NULL)
                    result = EC_CorruptedData;
            }
            else if (compare(sopClass, UID_ImplantTemplateGroupStorage))
            {
                /* add an implant assy record below the root */
                if (addRecord(rootRecord, ERT_ImplantAssy, &fileformat, fileID, pathname) == NULL)
                    result = EC_CorruptedData;
            } else {
                /* add a patient record below the root */
                DcmDirectoryRecord *patientRecord = addRecord(rootRecord, ERT_Patient, &fileformat, fileID, pathname);
                if (patientRecord != NULL)
                {
                    /* if patient management file then attach it to patient record and stop */
                    if (compare(sopClass, UID_RETIRED_DetachedPatientManagementMetaSOPClass))
                    {
                        result = patientRecord->assignToSOPFile(fileID.c_str(), pathname);
                        DCMDATA_ERROR(result.text() << ": cannot assign patient record to file: " << pathname);
                    } else {
                        /* add a study record below the current patient record */
                        DcmDirectoryRecord *studyRecord = addRecord(patientRecord, ERT_Study, &fileformat, fileID, pathname);;
                        if (studyRecord != NULL)
                        {
                            /* add a series record below the current study record */
                            DcmDirectoryRecord *seriesRecord = addRecord(studyRecord, ERT_Series, &fileformat, fileID, pathname);;
                            if (seriesRecord != NULL)
                            {
                                /* add one of the instance record below the current series record */
                                if (addRecord(seriesRecord, sopClassToRecordType(sopClass), &fileformat, fileID, pathname) == NULL)
                                    result = EC_CorruptedData;
                            } else
                                result = EC_CorruptedData;
                        } else
                            result = EC_CorruptedData;
                    }
                } else
                    result = EC_CorruptedData;
                /* invent missing attributes on all levels or PatientID only */
                if (InventMode)
                    inventMissingAttributes(rootRecord);
                else if (InventPatientIDMode)
                    inventMissingAttributes(rootRecord, OFFalse /*recurse*/);
            }
        }
    }
    return result;
}


// set the file-set descriptor and character set
OFCondition DicomDirInterface::setFilesetDescriptor(const char *filename,
                                                    const char *charset)
{
    OFCondition result = EC_IllegalCall;
    if (DicomDir != NULL)
    {
        /* check filename and character set */
        if (isFilenameValid(filename, OFTrue /*allowEmpty*/) && isCharsetValid(charset))
        {
            /* get dataset of the current DICOMDIR */
            DcmDataset *dataset = DicomDir->getDirFileFormat().getDataset();
            if (dataset != NULL)
            {
                /* set FileSetDescriptorFileID */
                if ((filename == NULL) || (filename[0] == '\0'))
                {
                    /* remove attribute from the dataset */
                    dataset->findAndDeleteElement(DCM_FileSetDescriptorFileID);
                    dataset->findAndDeleteElement(DCM_SpecificCharacterSetOfFileSetDescriptorFile);
                    result = EC_Normal;
                } else {
                    /* massage filename into DICOM format (DOS conventions for path separators, uppercase) */
                    OFString fileID;
                    hostToDicomFilename(filename, fileID);
                    /* replace attribute value */
                    result = dataset->putAndInsertString(DCM_FileSetDescriptorFileID, fileID.c_str());
                    if (result.good())
                    {
                        /* set SpecificCharacterSetOfFileSetDescriptorFile */
                        if ((charset == NULL) || (charset[0] == '\0'))
                        {
                            /* remove attribute from the dataset */
                            dataset->findAndDeleteElement(DCM_SpecificCharacterSetOfFileSetDescriptorFile);
                            result = EC_Normal;
                        } else {
                            /* replace attribute value */
                            result = dataset->putAndInsertString(DCM_SpecificCharacterSetOfFileSetDescriptorFile, charset);
                        }
                    }
                }
            }
        } else
            result = EC_IllegalParameter;
        // tbd: check whether file exists?
    }
    return result;
}


// set size of icon images (width and height in pixels)
OFCondition DicomDirInterface::setIconSize(const unsigned int size)
{
    OFCondition result = EC_IllegalParameter;
    /* check valid range */
    if ((size > 0) && (size <= 256))
    {
        IconSize = size;
        result = EC_Normal;
    }
    return result;
}


// set filename prefix for icon images (if not retrieved from DICOM image)
OFCondition DicomDirInterface::setIconPrefix(const OFFilename &prefix)
{
    IconPrefix = prefix;
    return EC_Normal;
}


// set filename for default image icon which is used in case of error
OFCondition DicomDirInterface::setDefaultIcon(const OFFilename &filename)
{
    DefaultIcon = filename;
    return EC_Normal;
}


// enable/disable the abort mode, i.e. abort on first inconsistent file (otherwise warn)
OFBool DicomDirInterface::enableAbortMode(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = AbortMode;
    /* set new mode */
    AbortMode = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable filename mapping mode, i.e. whether non-conforming filename are adjusted automatically
OFBool DicomDirInterface::enableMapFilenamesMode(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = MapFilenamesMode;
    /* set new mode */
    MapFilenamesMode = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable invent mode, i.e. whether missing attribute values are invented
OFBool DicomDirInterface::enableInventMode(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = InventMode;
    /* set new mode */
    InventMode = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable invent patient ID mode, i.e. whether missing patient ID is invented
OFBool DicomDirInterface::enableInventPatientIDMode(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = InventPatientIDMode;
    /* set new mode */
    InventPatientIDMode = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable retired SOP class support, i.e. whether retired SOP classes are supported
OFBool DicomDirInterface::enableRetiredSOPClassSupport(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = RetiredSOPClassSupport;
    /* set new mode */
    RetiredSOPClassSupport = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable icon image mode, i.e. whether to create icon images
OFBool DicomDirInterface::enableIconImageMode(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = IconImageMode;
    /* set new mode */
    IconImageMode = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable the backup mode, i.e. whether to create a backup of the DICOMDIR
OFBool DicomDirInterface::disableBackupMode(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = BackupMode;
    /* set new mode */
    BackupMode = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable pixel encoding check, i.e. whether the pixel encoding is checked
// for particular application profiles
OFBool DicomDirInterface::disableEncodingCheck(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = EncodingCheck;
    /* set new mode */
    EncodingCheck = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable resolution check, i.e. whether the spatial resolution is checked
// for particular application profiles
OFBool DicomDirInterface::disableResolutionCheck(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = ResolutionCheck;
    /* set new mode */
    ResolutionCheck = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable transfer syntax check, i.e. whether the transfer syntax is checked
// for particular application profiles
OFBool DicomDirInterface::disableTransferSyntaxCheck(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = TransferSyntaxCheck;
    /* set new mode */
    TransferSyntaxCheck = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable DICOM file format check, i.e. whether the file is checked for the
// presence of the file meta information (according to DICOM part 10)
OFBool DicomDirInterface::disableFileFormatCheck(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = FileFormatCheck;
    /* set new mode */
    FileFormatCheck = newMode;
    /* return old mode */
    return oldMode;
}


// enable/disable consistency check, i.e. whether the file is checked for consistency
// with the directory record
OFBool DicomDirInterface::disableConsistencyCheck(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = ConsistencyCheck;
    /* set new mode */
    ConsistencyCheck = newMode;
    /* return old mode */
    return oldMode;
}


// add support for DicomImage class(es) - required for icon images
OFBool DicomDirInterface::addImageSupport(DicomDirImagePlugin *plugin)
{
    // store pointer to plugin
    ImagePlugin = plugin;
    return (ImagePlugin != NULL);
}


// print an error message to the console (stderr) that the value of the given tag is unexpected
void DicomDirInterface::printUnexpectedValueMessage(const DcmTagKey &key,
                                                    const OFFilename &filename,
                                                    const OFBool errorMsg)
{
    OFString str;
    if (!filename.isEmpty())
    {
        str = " in file: ";
        str += OFSTRING_GUARD(filename.getCharPointer());
    }
    if (errorMsg)
    {
        DCMDATA_ERROR("attribute " << DcmTag(key).getTagName() << " " << key
            << " has other value than expected" << str);
    } else {
        DCMDATA_WARN("attribute " << DcmTag(key).getTagName() << " " << key
            << " has other value than expected" << str);
    }
}


// print an error message to the console (stderr) that a required attribute is missing/empty
void DicomDirInterface::printRequiredAttributeMessage(const DcmTagKey &key,
                                                      const OFFilename &filename,
                                                      const OFBool emptyMsg)
{
    OFString str;
    if (!filename.isEmpty())
    {
        str = " in file: ";
        str += OFSTRING_GUARD(filename.getCharPointer());
    }
    DCMDATA_ERROR("required attribute " << DcmTag(key).getTagName() << " " << key << " "
        << (emptyMsg ? "empty" : "missing") << str);
}


// print an error message to the console (stderr) that something went wrong with an attribute
void DicomDirInterface::printAttributeErrorMessage(const DcmTagKey &key,
                                                   const OFCondition &error,
                                                   const char *operation)
{
    if (error.bad())
    {
        OFString str;
        if (operation != NULL)
        {
            str = "cannot ";
            str += operation;
            str += " ";
        }
        DCMDATA_ERROR(error.text() << ": " << str << DcmTag(key).getTagName() << " " << key);
    }
}


// print an error message to the console (stderr) that something went wrong with a given record
void DicomDirInterface::printRecordErrorMessage(const OFCondition &error,
                                                const E_DirRecType recordType,
                                                const char *operation)
{
    if (error.bad())
    {
        OFString str;
        if (operation != NULL)
        {
            str = "cannot ";
            str += operation;
            str += " ";
        }
        DCMDATA_ERROR(error.text() << ": " << str << recordTypeToName(recordType) << " directory record");
    }
}


// return the name of the specified application profile
const char *DicomDirInterface::getProfileName(const E_ApplicationProfile profile)
{
    const char *result = "";
    switch(profile)
    {
        case AP_GeneralPurpose:
            result = "STD-GEN-CD/DVD-RAM";
            break;
        case AP_GeneralPurposeDVDJPEG:
            result = "STD-GEN-DVD-JPEG";
            break;
        case AP_GeneralPurposeDVDJPEG2000:
            result = "STD-GEN-DVD-J2K";
            break;
        case AP_GeneralPurposeBDJPEG:
            result = "STD-GEN-BD-JPEG";
            break;
        case AP_GeneralPurposeBDJPEG2000:
            result = "STD-GEN-BD-J2K";
            break;
        case AP_GeneralPurposeBDMPEG2MPatML:
            result = "STD-GEN-BD-MPEG2-MPML";
            break;
        case AP_GeneralPurposeBDMPEG2MPatHL:
            result = "STD-GEN-BD-MPEG2-MPHL";
            break;
        case AP_GeneralPurposeBDMPEG4HPatLV41:
            result = "STD-GEN-BD-MPEG4-HPLV41";
            break;
        case AP_GeneralPurposeBDMPEG4HPatLV41BD:
            result = "STD-GEN-BD-MPEG4-HPLV41BD";
            break;
        case AP_GeneralPurposeBDMPEG4HPatLV42_2D:
            result = "STD-GEN-BD-MPEG4-HPLV42-2D";
            break;
        case AP_GeneralPurposeBDMPEG4HPatLV42_3D:
            result = "STD-GEN-BD-MPEG4-HPLV42-3D";
            break;
        case AP_GeneralPurposeBDMPEG4StereoHPatLV42:
            result = "STD-GEN-BD-MPEG4-SHPLV42";
            break;
        case AP_USBandFlashJPEG:
            result = "STD-GEN-USB/MMC/CF/SD-JPEG";
            break;
        case AP_USBandFlashJPEG2000:
            result = "STD-GEN-USB/MMC/CF/SD-J2K";
            break;
        case AP_GeneralPurposeMIME:
            result = "STD-GEN-MIME";
            break;
        case AP_MPEG2MPatMLDVD:
            result = "STD-DVD-MPEG2-MPML";
            break;
        case AP_BasicCardiac:
            result = "STD-XABC-CD";
            break;
        case AP_XrayAngiographic:
            result = "STD-XA1K-CD";
            break;
        case AP_XrayAngiographicDVD:
            result = "STD-XA1K-DVD";
            break;
        case AP_DentalRadiograph:
            result = "STD-DEN-CD";
            break;
        case AP_CTandMR:
            result = "STD-CTMR-xxxx";
            break;
        case AP_UltrasoundIDSF:
            result = "STD-US-ID-SF-xxxx";
            break;
        case AP_UltrasoundSCSF:
            result = "STD-US-SC-SF-xxxx";
            break;
        case AP_UltrasoundCCSF:
            result = "STD-US-CC-SF-xxxx";
            break;
        case AP_UltrasoundIDMF:
            result = "STD-US-ID-MF-xxxx";
            break;
        case AP_UltrasoundSCMF:
            result = "STD-US-SC-MF-xxxx";
            break;
        case AP_UltrasoundCCMF:
            result = "STD-US-CC-MF-xxxx";
            break;
        case AP_TwelveLeadECG:
            result = "STD-WVFM-ECG-FD";
            break;
        case AP_HemodynamicWaveform:
            result = "STD-WVFM-HD-FD";
            break;
    }
    return result;
}


// copy contents of specified file
OFBool DicomDirInterface::copyFile(const OFFilename &fromFilename,
                                   const OFFilename &toFilename)
{
    OFBool result = OFStandard::copyFile(fromFilename, toFilename);
    /* check for errors */
    if (!result)
    {
        /* create error message from error code */
        DCMDATA_ERROR("copying files: " << fromFilename << " to " << toFilename
            << ": " << OFStandard::getLastSystemErrorCode().message());
    }
    return result;
}


// see if all the attributes in record match the values in dataset
OFBool DicomDirInterface::warnAboutInconsistentAttributes(DcmDirectoryRecord *record,
                                                          DcmItem *dataset,
                                                          const OFFilename &sourceFilename,
                                                          const OFBool abortCheck)
{
    OFBool result = OFFalse;
    /* check parameters first */
    if ((record != NULL) && (dataset != NULL))
    {
        result = OFTrue;   // default: no inconsistency
        DcmTagKey tag;
        DcmStack stack;
        OFBool first = OFTrue;
        DcmElement *delem = NULL;
        /* iterate over all record elements */
        while (record->nextObject(stack, first).good() && (result || !abortCheck))
        {
            delem = OFstatic_cast(DcmElement *, stack.top());
            if ((delem != NULL) && (delem->getLength() > 0))
            {
                /* record attribute has a value */
                tag = delem->getTag().getXTag();
                if (dataset->tagExistsWithValue(tag))
                {
                    if (delem->getTag().getEVR() == EVR_SQ)
                    {
                        /* do not check particular sequences (because they will always deviate) */
                        if ((delem->getTag() != DCM_ContentSequence) && (delem->getTag() != DCM_BlendingSequence))
                            result &= compareSequenceAttributes(dataset, tag, record, sourceFilename);
                    } else {
                        /* everything else can be compared as a string */
                        result &= compareStringAttributes(dataset, tag, record, tag, sourceFilename);
                    }
                }
            }
            first = OFFalse;
        }
    }
    if (!result && abortCheck)
        DCMDATA_ERROR("aborting on first inconsistent file: " << sourceFilename);
    /* return OFTrue in case of any inconsistency */
    return !result;
}


// check whether given file-set ID is valid
OFBool DicomDirInterface::checkFilesetID(const OFString &filesetID)
{
    OFBool result = OFTrue;
    if (!filesetID.empty())
    {
        size_t invalidChar = 0;
        /* are the characters ok? */
        if (!DcmCodeString::checkVR(filesetID, &invalidChar, OFFalse /*checkLength*/))
        {
            /* create error message */
            DCMDATA_ERROR("invalid character(s) in file-set ID: " << filesetID << OFendl
                << OFString(37 /*message*/ + invalidChar, ' ') << "^");
            result = OFFalse;
        }
        /* ensure that file-set ID is not too large */
        if (isComponentTooLarge(filesetID, OFstatic_cast(size_t, DcmVR(EVR_CS).getMaxValueLength()), MapFilenamesMode))
        {
            DCMDATA_ERROR("file-set ID too large: " << filesetID);
            result = OFFalse;
        }
    }
    return result;
}


// check whether specified tag exists in the dataset
OFBool DicomDirInterface::checkExists(DcmItem *dataset,
                                      const DcmTagKey &key,
                                      const OFFilename &filename)
{
    /* check whether tag exists */
    OFBool result = OFFalse;
    if (dataset != NULL)
    {
        result = dataset->tagExists(key);
        if (!result && !filename.isEmpty())
        {
            /* report an error */
            printRequiredAttributeMessage(key, filename);
        }
    }
    return result;
}


// check whether specified tag exists with a value in the dataset
OFBool DicomDirInterface::checkExistsWithValue(DcmItem *dataset,
                                               const DcmTagKey &key,
                                               const OFFilename &filename)
{
    /* first, check whether tag exists, and report an error if not */
    OFBool result = checkExists(dataset, key, filename);
    if (result)
    {
        /* then check whether tag has a value (is non-empty) */
        result = dataset->tagExistsWithValue(key);
        if (!result && !filename.isEmpty())
        {
            /* report an error */
            printRequiredAttributeMessage(key, filename, OFTrue /*emptyMsg*/);
        }
    }
    return result;
}


// check whether specified tag exists in the dataset and has the expected string value
OFBool DicomDirInterface::checkExistsWithStringValue(DcmItem *dataset,
                                                     const DcmTagKey &key,
                                                     const OFString &value,
                                                     const OFFilename &filename)
{
    /* first, check whether tag exists, and report an error if not */
    OFBool result = checkExists(dataset, key, filename);
    if (result)
    {
        OFString str;
        /* retrieve the string value from the element */
        dataset->findAndGetOFStringArray(key, str);
        /* compare with expected value */
        result = compare(str, value);
        if (!result && !filename.isEmpty())
        {
            /* report an error */
            printUnexpectedValueMessage(key, filename);
        }
    }
    return result;
}


// check whether specified tag exists in the dataset and has the expected integer value
OFBool DicomDirInterface::checkExistsWithIntegerValue(DcmItem *dataset,
                                                      const DcmTagKey &key,
                                                      const long value,
                                                      const OFFilename &filename,
                                                      const OFBool reject)
{
    /* first, check whether tag exists, and report an error if not */
    OFBool result = checkExists(dataset, key, filename);
    if (result)
    {
        long i;
        /* retrieve the integer value from the element */
        dataset->findAndGetLongInt(key, i);
        /* compare with expected value */
        result = (i == value);
        if (!result)
        {
            if (!filename.isEmpty())
            {
                /* report an error or a warning */
                printUnexpectedValueMessage(key, filename, reject /*errorMsg*/);
            }
            /* do not reject invalid values */
            if (!reject)
                result = OFTrue;
        }
    }
    return result;
}


// check whether specified tag exists in the dataset and has an integer value in the expected range
OFBool DicomDirInterface::checkExistsWithMinMaxValue(DcmItem *dataset,
                                                     const DcmTagKey &key,
                                                     const long min,
                                                     const long max,
                                                     const OFFilename &filename,
                                                     const OFBool reject)
{
    /* first, check whether tag exists, and report an error if not */
    OFBool result = checkExists(dataset, key, filename);
    if (result)
    {
        long i;
        /* retrieve the integer value from the element */
        dataset->findAndGetLongInt(key, i);
        /* compare with expected value range */
        result = (i >= min) && (i <= max);
        if (!result)
        {
            if (!filename.isEmpty())
            {
                /* report an error or a warning */
                printUnexpectedValueMessage(key, filename, reject /*errorMsg*/);
            }
            /* do not reject invalid values */
            if (!reject)
                result = OFTrue;
        }
    }
    return result;
}


// get string value from dataset and report an error (if any)
OFString &DicomDirInterface::getStringFromDataset(DcmItem *dataset,
                                                  const DcmTagKey &key,
                                                  OFString &result,
                                                  OFBool searchIntoSub)
{
    result.clear();
    if (dataset != NULL)
    {
        /* get string value from dataset and report if tag is missing */
        OFCondition status = dataset->findAndGetOFStringArray(key, result, searchIntoSub);
        printAttributeErrorMessage(key, status, "retrieve");
    }
    return result;
}


// get string value component from dataset and report an error (if any)
OFString &DicomDirInterface::getStringComponentFromDataset(DcmItem *dataset,
                                                           const DcmTagKey &key,
                                                           OFString &result,
                                                           const unsigned long pos,
                                                           OFBool searchIntoSub)
{
    result.clear();
    if (dataset != NULL)
    {
        /* get string value component from dataset and report if tag or component is missing */
        OFCondition status = dataset->findAndGetOFString(key, result, pos, searchIntoSub);
        if (status.bad())
        {
            DCMDATA_ERROR(status.text() << ": cannot retrieve value " << (pos + 1)
                << " of " << DcmTag(key).getTagName() << " " << key);
        }
    }
    return result;
}


// get string value from file and report an error (if any)
OFString &DicomDirInterface::getStringFromFile(const OFFilename &filename,
                                               const DcmTagKey &key,
                                               OFString &result,
                                               OFBool searchIntoSub)
{
    result.clear();
    if (!filename.isEmpty())
    {
        DcmFileFormat fileformat;
        DCMDATA_INFO("investigating file: " << filename);
        /* load specified file */
        OFCondition status = fileformat.loadFile(filename);
        /* retrieve string value from dataset */
        if (status.good())
            getStringFromDataset(fileformat.getDataset(), key, result, searchIntoSub);
        else
            DCMDATA_ERROR(status.text() << ": reading file: " << filename);
    }
    return result;
}


// copy element from given dataset to directory record
void DicomDirInterface::copyElement(DcmItem *dataset,
                                    const DcmTagKey &key,
                                    DcmDirectoryRecord *record,
                                    const OFFilename &sourceFilename,
                                    const OFBool optional,
                                    const OFBool copyEmpty)
{
    if ((dataset != NULL) && (record != NULL))
    {
        /* check whether tag exists in source dataset (if optional) */
        if (!optional || (copyEmpty && dataset->tagExists(key)) || dataset->tagExistsWithValue(key))
        {
            DcmElement *delem = NULL;
            /* get copy of element from source dataset */
            OFCondition status = dataset->findAndGetElement(key, delem, OFFalse /*searchIntoSub*/, OFTrue /*createCopy*/);
            if (status.good())
            {
                /* ... and insert it into the destination dataset (record) */
                status = record->insert(delem, OFTrue /*replaceOld*/);
                if (status.good())
                {
                    DcmTag tag(key);
                    /* check for correct VR in the dataset */
                    if (delem->getVR() != tag.getEVR())
                    {
                        /* create warning message */
                        DCMDATA_WARN("file " << sourceFilename << ": possibly wrong VR: "
                            << tag.getTagName() << " " << key << " with "
                            << DcmVR(delem->getVR()).getVRName() << " found, expected "
                            << tag.getVRName() << " instead");
                    }
                } else
                    delete delem;
            } else if (status == EC_TagNotFound)
                status = record->insertEmptyElement(key);
            printAttributeErrorMessage(key, status, "insert");
        }
    }
}


// copy element from given dataset or first item of the given sequence to directory record
void DicomDirInterface::copyElementType1CFromDatasetOrSequenceItem(DcmItem *dataset,
                                                                   const DcmTagKey &elementKey,
                                                                   const DcmTagKey &sequenceKey,
                                                                   DcmDirectoryRecord *record,
                                                                   const OFFilename &sourceFilename)
{
    if ((dataset != NULL) && (record != NULL))
    {
        DcmElement *delem = NULL;
        OFCondition status = EC_IllegalCall;
        /* check whether tag exists in given dataset */
        if (dataset->tagExistsWithValue(elementKey))
        {
            /* get copy of element from given dataset */
            status = dataset->findAndGetElement(elementKey, delem, OFFalse /*searchIntoSub*/, OFTrue /*createCopy*/);
        } else {
            /* alternatively, check whether tag exists in first item of the given sequence element */
            DcmItem *ditem = NULL;
            if (dataset->findAndGetSequenceItem(sequenceKey, ditem, 0).good())
            {
                /* get copy of element from sequence item (on arbitrary nesting level) */
                status = ditem->findAndGetElement(elementKey, delem, OFTrue /*searchIntoSub*/, OFTrue /*createCopy*/);
            }
        }
        /* check whether element could be found */
        if (delem != NULL)
        {
            if (status.good() && !delem->isEmpty())
            {
                /* ... and insert it into the destination dataset (record) */
                status = record->insert(delem, OFTrue /*replaceOld*/);
                if (status.good())
                {
                    DcmTag tag(elementKey);
                    /* check for correct VR in the dataset */
                    if (delem->getVR() != tag.getEVR())
                    {
                        /* create warning message */
                        DCMDATA_WARN("file " << sourceFilename << ": possibly wrong VR: "
                            << tag.getTagName() << " " << elementKey << " with "
                            << DcmVR(delem->getVR()).getVRName() << " found, expected "
                            << tag.getVRName() << " instead");
                    }
                } else
                    delete delem;
            } else
                delete delem;
            printAttributeErrorMessage(elementKey, status, "insert");
        }
    }
}


// copy optional string value from dataset to directory record
void DicomDirInterface::copyStringWithDefault(DcmItem *dataset,
                                              const DcmTagKey &key,
                                              DcmDirectoryRecord *record,
                                              const OFFilename &sourceFilename,
                                              const char *defaultValue,
                                              const OFBool printWarning)
{
    if ((dataset != NULL) && (record != NULL))
    {
        OFCondition status = EC_Normal;
        if (dataset->tagExistsWithValue(key))
        {
            OFString stringValue;
            /* retrieve string value from source dataset and put it into the destination dataset */
            status = record->putAndInsertOFStringArray(key, getStringFromDataset(dataset, key, stringValue));
        } else {
            if (printWarning && (defaultValue != NULL))
            {
                /* create warning message */
                DCMDATA_WARN("file " << sourceFilename << ": " << DcmTag(key).getTagName() << " "
                    << key << " missing, using alternative: " << defaultValue);
            }
            /* put default value */
            status = record->putAndInsertString(key, defaultValue);
        }
        printAttributeErrorMessage(key, status, "insert");
    }
}


// compare string attributes from dataset and record and report any deviation
OFBool DicomDirInterface::compareStringAttributes(DcmItem *dataset,
                                                  const DcmTagKey &datKey,
                                                  DcmDirectoryRecord *record,
                                                  const DcmTagKey &recKey,
                                                  const OFFilename &sourceFilename,
                                                  const OFBool errorMsg)
{
    OFBool result = OFFalse;
    /* check parameters first */
    if ((dataset != NULL) && (record != NULL))
    {
        OFString datasetString, recordString;
        /* compare string value from dataset and record */
        result = compare(getStringFromDataset(dataset, datKey, datasetString),
                         getStringFromDataset(record, recKey, recordString));
        if (!result)
        {
            OFString uniqueString;
            OFFilename originFilename = record->getRecordsOriginFile();
            const DcmTagKey uniqueKey = getRecordUniqueKey(record->getRecordType());
            getStringFromDataset(record, uniqueKey, uniqueString);
            if (originFilename.isEmpty())
                originFilename = "<unknown>";
            /* create warning message */
            OFOStringStream oss;
            oss << "file inconsistent with existing DICOMDIR record" << OFendl;
            oss << "  " << recordTypeToName(record->getRecordType()) << " Record [Key: "
                << DcmTag(uniqueKey).getTagName() << " " << uniqueKey << "=\"" << uniqueString << "\"]" << OFendl;
            oss << "    Existing Record (origin: " << originFilename << ") defines: " << OFendl;
            oss << "      " << DcmTag(recKey).getTagName() << " " << recKey << "=\"" << recordString << "\"" << OFendl;
            oss << "    File (" << sourceFilename << ") defines:" << OFendl;
            oss << "      " << DcmTag(datKey).getTagName() << " " << datKey << "=\"" << datasetString << "\"" << OFendl;
            oss << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            if (errorMsg)
                DCMDATA_ERROR(tmpString);
            else
                DCMDATA_WARN(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
        }
    }
    return result;
}


// compare sequence attributes from dataset and record and report any deviation
OFBool DicomDirInterface::compareSequenceAttributes(DcmItem *dataset,
                                                    DcmTagKey &key,
                                                    DcmDirectoryRecord *record,
                                                    const OFFilename &sourceFilename)
{
    OFBool result = OFFalse;
    /* check parameters first */
    if ((dataset != NULL) && (record != NULL))
    {
        DcmSequenceOfItems *seq1, *seq2;
        /* compare sequence value from dataset and record */
        if (record->findAndGetSequence(key, seq1).good() &&
            dataset->findAndGetSequence(key, seq2).good())
        {
            OFString reason;
            result = compareSQAttributes(seq1, seq2, reason);
            if (!result)
            {
                OFString uniqueString;
                OFFilename originFilename = record->getRecordsOriginFile();
                const DcmTagKey uniqueKey = getRecordUniqueKey(record->getRecordType());
                getStringFromDataset(record, uniqueKey, uniqueString);
                if (originFilename.isEmpty())
                    originFilename = "<unknown>";
                /* create warning message */
                DCMDATA_WARN("file inconsistent with existing DICOMDIR record" << OFendl
                    << "  " << recordTypeToName(record->getRecordType()) << " Record [Key: "
                    << DcmTag(uniqueKey).getTagName() << " " << uniqueKey << "=\"" << uniqueString << "\"]" << OFendl
                    << "    Reason: " << reason << OFendl
                    << "    Existing Record (origin: " << originFilename << ") defines: " << OFendl
                    << DcmObject::PrintHelper(*seq1, 0, 4 /* indent */)
                    << "    File (" << sourceFilename << ") defines:" << OFendl
                    << DcmObject::PrintHelper(*seq2, 0, 4 /* indent */));
            }
        }
    }
    return result;
}


// set the specified default value (number or prefix and number) to the given tag
void DicomDirInterface::setDefaultValue(DcmDirectoryRecord *record,
                                        const DcmTagKey &key,
                                        const unsigned long number,
                                        const char *prefix)
{
    if (record != NULL)
    {
        char buffer[64];
        if (prefix != NULL)
        {
            /* use at most 10 chars from prefix */
            OFStandard::strlcpy(buffer, prefix, 10 + 1);
            /* append a 6 digits number */
            sprintf(buffer + strlen(buffer), "%06lu", number);
        } else {
            /* create a number string only */
            sprintf(buffer, "%lu", number);
        }
        record->putAndInsertString(key, buffer);
        /* create warning message */
        DCMDATA_WARN(recordTypeToName(record->getRecordType()) << " Record (origin: "
            << record->getRecordsOriginFile() << ") inventing " << DcmTag(key).getTagName()
            << ": " << buffer);
    }
}
