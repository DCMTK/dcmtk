/*
 *
 *  Copyright (C) 2002-2006, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Interface class for simplified creation of a DICOMDIR
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2006-07-27 13:09:25 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dcddirif.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/dcmdata/dcmetinf.h"    /* for class DcmMetaInfo */
#include "dcmtk/dcmdata/dcpixel.h"     /* for class DcmPixelData */
#include "dcmtk/dcmdata/dcpxitem.h"    /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcpixseq.h"    /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcvrcs.h"      /* for class DcmCodeString */
#include "dcmtk/dcmdata/dcvrda.h"      /* for class DcmDate */
#include "dcmtk/dcmdata/dcvrtm.h"      /* for class DcmTime */

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofbmanip.h"    /* for class OFBitmanipTemplate */
#include "dcmtk/ofstd/ofcast.h"


/*------------------------*
 *  contant declarations  *
 *------------------------*/

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
    char c;
    size_t i = 0;
    size_t length = filename.length();
    /* disregard trailing point */
    if (mapFilenames && (length > 0) && (filename.at(length - 1) == '.'))
        length--;
    /* iterate over all charaters */
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
    for (size_t i = 0; i < length; i++)
    {
        const char c = hostFilename.at(i);
        if (c == PATH_SEPARATOR)
        {
            /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
            dicomFilename += '\\';
        }
        else if (isalpha(c))
        {
            /* filenames in DICOM must always be in uppercase */
            dicomFilename += toupper(c);
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
    for (size_t i = 0; i < length; i++)
    {
        const char c = dicomFilename.at(i);
        /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
        if (c == '\\')
            hostFilename += PATH_SEPARATOR;
        else if (mapToLower)
            hostFilename += tolower(c);
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
    }
    if (!result)
    {
        /* lowercase with trailing period */
        hostFilename += '.';
        result = OFStandard::fileExists(hostFilename);
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


// contruct tag name from given object
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


// contruct tag name from given object and sequence
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
        oss << constructTagName(fromSequence, tempStr) << "[" << itemNumber << "]";
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
            reason = "missing attribute: " +  constructTagNameWithSQ(elem2, fromSequence, itemNumber, tmpString);
    } else
        reason = "missing attribute: " +  constructTagNameWithSQ(elem1, fromSequence, itemNumber, tmpString);
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


// get directory record name from type
static OFString recordTypeToName(const E_DirRecType recordType)
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
    if (compare(sopClass, UID_StandaloneOverlayStorage))
        result = ERT_Overlay;
    else if (compare(sopClass, UID_StandaloneModalityLUTStorage))
        result = ERT_ModalityLut;
    else if (compare(sopClass, UID_StandaloneVOILUTStorage))
        result = ERT_VoiLut;
    else if (compare(sopClass, UID_StandaloneCurveStorage) ||
             compare(sopClass, UID_PETCurveStorage))
    {
        result = ERT_Curve;
    }
    else if (compare(sopClass, UID_BasicTextSR) ||
             compare(sopClass, UID_EnhancedSR) ||
             compare(sopClass, UID_ComprehensiveSR) ||
             compare(sopClass, UID_MammographyCADSR) ||
             compare(sopClass, UID_ChestCADSR) ||
             compare(sopClass, UID_ProcedureLogStorage) ||
             compare(sopClass, UID_XRayRadiationDoseSR))
    {
        result = ERT_SRDocument;
    }
    else if (compare(sopClass, UID_GrayscaleSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_ColorSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_PseudoColorSoftcopyPresentationStateStorage) ||
             compare(sopClass, UID_BlendingSoftcopyPresentationStateStorage))
    {
        result = ERT_Presentation;
    }
    else if (compare(sopClass, UID_TwelveLeadECGWaveformStorage) ||
             compare(sopClass, UID_GeneralECGWaveformStorage) ||
             compare(sopClass, UID_AmbulatoryECGWaveformStorage) ||
             compare(sopClass, UID_HemodynamicWaveformStorage) ||
             compare(sopClass, UID_CardiacElectrophysiologyWaveformStorage) ||
             compare(sopClass, UID_BasicVoiceAudioWaveformStorage))
    {
        result = ERT_Waveform;
    }
    else if (compare(sopClass, UID_RTDoseStorage))
        result = ERT_RTDose;
    else if (compare(sopClass, UID_RTStructureSetStorage))
        result = ERT_RTStructureSet;
    else if (compare(sopClass, UID_RTPlanStorage))
        result = ERT_RTPlan;
    else if (compare(sopClass, UID_RTBeamsTreatmentRecordStorage) ||
             compare(sopClass, UID_RTBrachyTreatmentRecordStorage) ||
             compare(sopClass, UID_RTTreatmentSummaryRecordStorage))
    {
        result = ERT_RTTreatRecord;
    }
    else if (compare(sopClass, UID_StoredPrintStorage))
        result = ERT_StoredPrint;
    else if (compare(sopClass, UID_KeyObjectSelectionDocument))
        result = ERT_KeyObjectDoc;
    else if (compare(sopClass, UID_SpatialRegistrationStorage))
        result = ERT_Registration;
    else if (compare(sopClass, UID_SpatialFiducialsStorage))
        result = ERT_Fiducial;
    else if (compare(sopClass, UID_RawDataStorage))
        result = ERT_RawData;
    else if (compare(sopClass, UID_MRSpectroscopyStorage))
        result = ERT_Spectroscopy;
    else if (compare(sopClass, UID_EncapsulatedPDFStorage))
        result = ERT_EncapDoc;
    else if (compare(sopClass, UID_RealWorldValueMappingStorage))
        result = ERT_ValueMap;
    else if (compare(sopClass, UID_HangingProtocolStorage))
        result = ERT_HangingProtocol;
    else if (compare(sopClass, UID_StereometricRelationshipStorage))
        result = ERT_Stereometric;
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
                result = insertWithISCriterion(parent, child, DCM_OverlayNumber);
                break;
            case ERT_Curve:
                /* try to insert based on CurveNumber */
                result = insertWithISCriterion(parent, child, DCM_CurveNumber);
                break;
            case ERT_ModalityLut:
            case ERT_VoiLut:
                /* try to insert based on LUTNumber */
                result = insertWithISCriterion(parent, child, DCM_LookupTableNumber);
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
                /* try to insert based on InstanceNumber */
                result = insertWithISCriterion(parent, child, DCM_InstanceNumber);
                break;
            case ERT_Series:
                /* try to insert based on SeriesNumber */
                result = insertWithISCriterion(parent, child, DCM_SeriesNumber);
                break;
            case ERT_Stereometric:
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



/*------------------*
 *  implementation  *
 *------------------*/

// constructor
DicomDirInterface::DicomDirInterface()
  : LogStream(NULL),
    DicomDir(NULL),
    ImagePlugin(NULL),
    ApplicationProfile(AP_Default),
    VerboseMode(OFFalse),
    BackupMode(OFTrue),
    AbortMode(OFFalse),
    MapFilenamesMode(OFFalse),
    InventMode(OFFalse),
    InventPatientIDMode(OFFalse),
    EncodingCheck(OFTrue),
    ResolutionCheck(OFTrue),
    TransferSyntaxCheck(OFTrue),
    ConsistencyCheck(OFTrue),
    IconImageMode(OFFalse),
    BackupFilename(),
    BackupCreated(OFFalse),
    IconSize(64),
    IconPrefix(),
    DefaultIcon(),
    JPEGSupport(OFFalse),
    RLESupport(OFFalse),
    AutoPatientNumber(0),
    AutoStudyNumber(0),
    AutoSeriesNumber(0),
    AutoInstanceNumber(1),
    AutoOverlayNumber(1),
    AutoLutNumber(1),
    AutoCurveNumber(1)
{
    /* check whether (possibly required) JPEG/RLE codecs are registered */
    JPEGSupport = DcmCodecList::canChangeCoding(EXS_JPEGProcess1TransferSyntax, EXS_LittleEndianExplicit) &&
                  DcmCodecList::canChangeCoding(EXS_JPEGProcess14SV1TransferSyntax, EXS_LittleEndianExplicit);
    RLESupport = DcmCodecList::canChangeCoding(EXS_RLELossless, EXS_LittleEndianExplicit);
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
    deleteDicomDirBackup();
    /* free all allocated memory */
    delete DicomDir;
    /* invalidate references */
    DicomDir = NULL;
}


// check whether the current DICOMDIR object is valid
OFBool DicomDirInterface::isDicomDirValid() const
{
    /* might add more sophisticated checks later on,
       e.g. require at least one referenced DICOM file */
    return (DicomDir != NULL);
}


// create a backup of the specified file
void DicomDirInterface::createDicomDirBackup(const char *filename)
{
    /* check whether DICOMDIR already exists */
    if (OFStandard::fileExists(filename))
    {
        /* rename existing DICOMDIR */
        BackupFilename = OFString(filename) + FNAME_BACKUP_EXTENSION;
        /* delete old backup file (if any) */
        deleteDicomDirBackup();
        if (VerboseMode)
            printMessage("creating DICOMDIR backup: ", BackupFilename.c_str());
        /* create backup file */
        if (copyFile(filename, BackupFilename.c_str()))
            BackupCreated = OFTrue;
        else
            printErrorMessage("cannot create backup of: ", filename);
    }
}


// delete backup file if one has been created
void DicomDirInterface::deleteDicomDirBackup()
{
    /* if a backup of the DICOMDIR exists */
    if (OFStandard::fileExists(BackupFilename))
    {
        if (VerboseMode)
        {
            if (BackupCreated)
                printMessage("deleting DICOMDIR backup: ", BackupFilename.c_str());
            else
                printMessage("deleting old DICOMDIR backup: ", BackupFilename.c_str());
        }
        /* delete the backup file */
        unlink(BackupFilename.c_str());
    }
    /* reset status variable */
    BackupCreated = OFFalse;
}


// create a new DICOMDIR object, i.e. replace any previously existing 'filename'
OFCondition DicomDirInterface::createNewDicomDir(const E_ApplicationProfile profile,
                                                 const char *filename,
                                                 const char *filesetID)
{
    OFCondition result = EC_IllegalParameter;
    if ((filename != NULL) && checkFilesetID(filesetID))
    {
        /* first remove any existing DICOMDIR from memory */
        cleanup();
        /* then create a backup if a DICOMDIR file already exists */
        if (OFStandard::fileExists(filename))
        {
            if (BackupMode)
                createDicomDirBackup(filename);
            /* and delete it because otherwise DcmDicomDir will parse it
               and try to append to existing records */
            unlink(filename);
        }
        /* select new application profile */
        result = selectApplicationProfile(profile);
        if (result.good())
        {
            if (VerboseMode)
            {
                /* create message */
                OFOStringStream oss;
                oss << "creating DICOMDIR file using " << getProfileName(ApplicationProfile)
                    << " profile: " << filename << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                printMessage(tmpString);
                OFSTRINGSTREAM_FREESTR(tmpString)
            }
            /* finally, create a new DICOMDIR object */
            DicomDir = new DcmDicomDir(filename, filesetID);
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
                                                const char *filename)
{
    OFCondition result = EC_IllegalParameter;
    if (filename != NULL)
    {
        /* first check whether DICOMDIR file already exists */
        if (OFStandard::fileExists(filename))
        {
            /* select new application profile */
            result = selectApplicationProfile(profile);
            if (result.good())
            {
                if (VerboseMode)
                {
                    /* create message */
                    OFOStringStream oss;
                    oss << "appending to DICOMDIR file using " << getProfileName(ApplicationProfile)
                        << " profile: " << filename << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                    printMessage(tmpString);
                    OFSTRINGSTREAM_FREESTR(tmpString)
                }
                /* then create a backup if required */
                if (BackupMode)
                    createDicomDirBackup(filename);
                /* finally, create a DICOMDIR object based on the existing file */
                DicomDir = new DcmDicomDir(filename);
                if (DicomDir != NULL)
                    result = DicomDir->error();
                else
                    result = EC_MemoryExhausted;
            }
        } else {
            /* create error message "No such file or directory" from error code */
            const char *text = NULL;
#if defined(_REENTRANT) && defined(HAVE_STRERROR_R)
            /* # tbd: HAVE_STRERROR_R not yet defined/tested */
            char buffer[255];
            if (strerror_r(ENOENT, buffer, sizeof(buffer)) == 0)
                text = buffer;
            else
#else
                /* warning: function call is not thread-safe */
                text = strerror(ENOENT);
            if (text == NULL)
#endif
                text = "(unknown error code)";
            /*  error code 18 is reserved for file read error messages (see dcerror.cc) */
            result = makeOFCondition(OFM_dcmdata, 18, OF_error, text);
            /* report an error */
            printFileErrorMessage(result, "cannot append to", filename);
        }
    }
    return result;
}


// write the current DICOMDIR object to file
OFCondition DicomDirInterface::writeDicomDir(const E_EncodingType encodingType,
                                             const E_GrpLenEncoding groupLength)
{
    OFCondition result = EC_IllegalCall;
    /* check whether DICOMDIR object is valid */
    if (isDicomDirValid())
    {
        if (VerboseMode)
            printMessage("writing file: ", DicomDir->getDirFileName());
        /* write DICOMDIR as Little Endian Explicit as required by the standard */
        result = DicomDir->write(DICOMDIR_DEFAULT_TRANSFERSYNTAX, encodingType, groupLength);
        /* delete backup copy in case the new file could be written without any errors */
        if (result.good())
            deleteDicomDirBackup();
        else {
            /* report an error */
            printFileErrorMessage(result, "writing", DicomDir->getDirFileName());
        }
    }
    return result;
}


// check whether the specified filename conforms to the DICOM standard requirements
OFBool DicomDirInterface::isFilenameValid(const char *filename,
                                          const OFBool allowEmpty)
{
    OFBool result = OFTrue;
    /* check for empty filename */
    if ((filename == NULL) || (strlen(filename) == 0))
    {
        if (!allowEmpty)
        {
            printErrorMessage("<empty string> not allowed as filename");
            result = OFFalse;
        }
    } else {
        size_t invalidChar = 0;
        /* check whether the file name path is ok and in local format */
        if ((filename[0] == PATH_SEPARATOR) /* absolute path? */ ||
            locateInvalidFilenameChars(filename, invalidChar, MapFilenamesMode))
        {
            /* create error message */
            OFOStringStream oss;
            oss << "invalid character(s) in filename: " << filename << endl;
            oss << OFString(7 /*Error: */ + 34 /*message*/ + invalidChar, ' ') << "^" << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            printErrorMessage(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
            result = OFFalse;
        }
        /* ensure that the maximum number of components is not being exceeded */
        if (componentCount(filename) > MAX_FNAME_COMPONENTS)
        {
            /* create error message */
            OFOStringStream oss;
            oss << "too many path components (max " << MAX_FNAME_COMPONENTS << ") in filename: "
                << filename << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            printErrorMessage(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
            result = OFFalse;
        }
        /* ensure that each component is not too large */
        if (isComponentTooLarge(filename, MAX_FNAME_COMPONENT_SIZE, MapFilenamesMode))
        {
            /* create error message */
            OFOStringStream oss;
            oss << "component too large (max " << MAX_FNAME_COMPONENT_SIZE << " characters) in filename: "
                << filename << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            printErrorMessage(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
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
    if ((charset != NULL) && (strlen(charset) > 0))
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
            printErrorMessage("unknown character set for fileset descriptor: ", charset);
    }
    return result;
}


// check the given file (dataset) regarding SOP class and transfer syntax
OFCondition DicomDirInterface::checkSOPClassAndXfer(DcmMetaInfo *metainfo,
                                                    DcmItem *dataset,
                                                    const char *filename)
{
    OFCondition result = EC_IllegalParameter;
    if ((filename != NULL) && (metainfo != NULL) && (dataset != NULL))
    {
        /* is sop class ok? */
        OFString mediaSOPClassUID;
        if (metainfo->findAndGetOFStringArray(DCM_MediaStorageSOPClassUID, mediaSOPClassUID).bad())
        {
            printErrorMessage("MediaStorageSOPClassUID missing in metainfo-header: ", filename);
            result = EC_TagNotFound;
        } else {
            /* check if the SOP Class is a known storage SOP class (an image, overlay, curve, etc.) */
            OFBool found = OFFalse;
            OFString expectedTransferSyntax = UID_LittleEndianExplicitTransferSyntax;
            switch (ApplicationProfile)
            {
                case AP_MPEG2MPatML:
                    expectedTransferSyntax = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
                    /* multi-frame composite IODs only! */
                    found = compare(mediaSOPClassUID, UID_XRayAngiographicImageStorage) ||
                            compare(mediaSOPClassUID, UID_XRayFluoroscopyImageStorage) ||
                            compare(mediaSOPClassUID, UID_EnhancedCTImageStorage) ||
                            compare(mediaSOPClassUID, UID_NuclearMedicineImageStorage) ||
                            compare(mediaSOPClassUID, UID_RTImageStorage) ||
                            compare(mediaSOPClassUID, UID_RTDoseStorage) ||
                            compare(mediaSOPClassUID, UID_UltrasoundMultiframeImageStorage) ||
                            compare(mediaSOPClassUID, UID_EnhancedMRImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeSingleBitSecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeTrueColorSecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_VideoEndoscopicImageStorage) ||
                            compare(mediaSOPClassUID, UID_VideoMicroscopicImageStorage) ||
                            compare(mediaSOPClassUID, UID_VideoPhotographicImageStorage) ||
                            compare(mediaSOPClassUID, UID_OphthalmicPhotography8BitImageStorage) ||
                            compare(mediaSOPClassUID, UID_OphthalmicPhotography16BitImageStorage);
                    break;
                case AP_BasicCardiac:
                    if (compare(mediaSOPClassUID, UID_XRayAngiographicImageStorage))
                    {
                        expectedTransferSyntax = UID_JPEGProcess14SV1TransferSyntax;
                        found = OFTrue;
                    } else {
                        found = compare(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);
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
/* NB: the following SOP classes have been retired with Supplement 80:
                                compare(mediaSOPClassUID, UID_StandaloneOverlayStorage) ||
                                compare(mediaSOPClassUID, UID_StandaloneCurveStorage) ||
                                compare(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);
*/
                    }
                    break;
                case AP_DentalRadiograph:
                    found = compare(mediaSOPClassUID, UID_DigitalIntraOralXRayImageStorageForPresentation) ||
                            compare(mediaSOPClassUID, UID_DigitalXRayImageStorageForPresentation);
                    break;
                case AP_CTandMR:
                    /* transfer syntax needs to be checked later */
                    found = compare(mediaSOPClassUID, UID_CTImageStorage) ||
                            compare(mediaSOPClassUID, UID_MRImageStorage) ||
                            compare(mediaSOPClassUID, UID_SecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeSingleBitSecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage) ||
                            compare(mediaSOPClassUID, UID_MultiframeTrueColorSecondaryCaptureImageStorage);
/* NB: the following SOP class has been retired with Supplement 80:
                            compare(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);
*/
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
                case AP_GeneralPurposeDVD:
                case AP_GeneralPurposeMIME:
                case AP_USBandFlash:
                default:
                {
                    /* is it an image ? */
                    for (int i = 0; i < numberOfDcmImageSOPClassUIDs && !found; i++)
                        found = compare(mediaSOPClassUID, dcmImageSOPClassUIDs[i]);
                    /* is it an overlay/curve/modality_lut/voi_lut etc. ? */
                    found = found || compare(mediaSOPClassUID, UID_StandaloneOverlayStorage);
                    found = found || compare(mediaSOPClassUID, UID_StandaloneCurveStorage);
                    found = found || compare(mediaSOPClassUID, UID_StandaloneModalityLUTStorage);
                    found = found || compare(mediaSOPClassUID, UID_StandaloneVOILUTStorage);
                    found = found || compare(mediaSOPClassUID, UID_PETCurveStorage);
                    /* is it one of the RT SOP Classes? */
                    found = found || compare(mediaSOPClassUID, UID_RTDoseStorage);
                    found = found || compare(mediaSOPClassUID, UID_RTStructureSetStorage);
                    found = found || compare(mediaSOPClassUID, UID_RTBeamsTreatmentRecordStorage);
                    found = found || compare(mediaSOPClassUID, UID_RTPlanStorage);
                    found = found || compare(mediaSOPClassUID, UID_RTBrachyTreatmentRecordStorage);
                    found = found || compare(mediaSOPClassUID, UID_RTTreatmentSummaryRecordStorage);
                    /* is it one of the structured reporting SOP Classes? */
                    found = found || compare(mediaSOPClassUID, UID_BasicTextSR);
                    found = found || compare(mediaSOPClassUID, UID_EnhancedSR);
                    found = found || compare(mediaSOPClassUID, UID_ComprehensiveSR);
                    found = found || compare(mediaSOPClassUID, UID_MammographyCADSR);
                    found = found || compare(mediaSOPClassUID, UID_ChestCADSR);
                    found = found || compare(mediaSOPClassUID, UID_ProcedureLogStorage);
                    found = found || compare(mediaSOPClassUID, UID_ProcedureLogStorage);
                    found = found || compare(mediaSOPClassUID, UID_XRayRadiationDoseSR);
                    /* is it one of the waveform SOP Classes? */
                    found = found || compare(mediaSOPClassUID, UID_TwelveLeadECGWaveformStorage);
                    found = found || compare(mediaSOPClassUID, UID_GeneralECGWaveformStorage);
                    found = found || compare(mediaSOPClassUID, UID_AmbulatoryECGWaveformStorage);
                    found = found || compare(mediaSOPClassUID, UID_HemodynamicWaveformStorage);
                    found = found || compare(mediaSOPClassUID, UID_CardiacElectrophysiologyWaveformStorage);
                    found = found || compare(mediaSOPClassUID, UID_BasicVoiceAudioWaveformStorage);
                    /* is it one of the spatial registration SOP Classes? */
                    found = found || compare(mediaSOPClassUID, UID_SpatialRegistrationStorage);
                    found = found || compare(mediaSOPClassUID, UID_SpatialFiducialsStorage);
                    /* is it any other SOP class? */
                    found = found || compare(mediaSOPClassUID, UID_GrayscaleSoftcopyPresentationStateStorage);
                    found = found || compare(mediaSOPClassUID, UID_ColorSoftcopyPresentationStateStorage);
                    found = found || compare(mediaSOPClassUID, UID_PseudoColorSoftcopyPresentationStateStorage);
                    found = found || compare(mediaSOPClassUID, UID_BlendingSoftcopyPresentationStateStorage);
                    found = found || compare(mediaSOPClassUID, UID_StoredPrintStorage);
                    found = found || compare(mediaSOPClassUID, UID_RawDataStorage);
                    found = found || compare(mediaSOPClassUID, UID_MRSpectroscopyStorage);
                    found = found || compare(mediaSOPClassUID, UID_EncapsulatedPDFStorage);
                    found = found || compare(mediaSOPClassUID, UID_RealWorldValueMappingStorage);
                    found = found || compare(mediaSOPClassUID, UID_HangingProtocolStorage);
                    found = found || compare(mediaSOPClassUID, UID_StereometricRelationshipStorage);
                    if (ApplicationProfile == AP_GeneralPurpose)
                    {
                        /* a detached patient mgmt sop class is also ok */
                        found = found || compare(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);
                    }
                }
            }
            if (found)
                result = EC_Normal;
            else
            {
                OFString sopClassName = dcmFindNameOfUID(mediaSOPClassUID.c_str());
                if (sopClassName.empty())
                    sopClassName = mediaSOPClassUID;
                /* create error message */
                OFOStringStream oss;
                oss << "invalid SOP class (" << sopClassName << ") for " << getProfileName(ApplicationProfile)
                    << " profile: " << filename << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                printErrorMessage(tmpString);
                OFSTRINGSTREAM_FREESTR(tmpString)
                result = EC_ApplicationProfileViolated;
            }
            if (result.good())
            {
                /* is transfer syntax ok? */
                OFString transferSyntax;
                if (metainfo->findAndGetOFStringArray(DCM_TransferSyntaxUID, transferSyntax).bad())
                {
                    printErrorMessage("TransferSyntaxUID missing in metainfo-header: ", filename);
                    result = EC_TagNotFound;
                }
                /* is transfer syntax supported */
                if (result.good())
                {
                    /* JPEG compression */
                    if (compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax) ||
                        compare(transferSyntax, UID_JPEGProcess1TransferSyntax))
                    {
                        if (!JPEGSupport)
                        {
                            printErrorMessage("JPEG compression not supported: ", filename);
                            result = EC_CannotChangeRepresentation;
                        }
                    }
                    /* RLE comporession */
                    if (compare(transferSyntax, UID_RLELosslessTransferSyntax))
                    {
                        if (!RLESupport)
                        {
                            printErrorMessage("RLE compression not supported: ", filename);
                            result = EC_CannotChangeRepresentation;
                        }
                    }
                }
                /* compare expected and actual transfer syntax */
                if (result.good())
                {
                    switch (ApplicationProfile)
                    {
                        case AP_GeneralPurposeMIME:
                            /* accept all transfer syntaxes */
                            break;
                        case AP_GeneralPurposeDVD:
                        case AP_USBandFlash:
                            /* need to check multiple transfer syntaxes */
                            found = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess1TransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess2_4TransferSyntax) ||
                                    compare(transferSyntax, UID_JPEG2000LosslessOnlyTransferSyntax) ||
                                    compare(transferSyntax, UID_JPEG2000TransferSyntax);
                            if (!found)
                            {
                                OFString xferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax);
                                OFString xferName2 = dcmFindNameOfUID(UID_JPEGProcess14SV1TransferSyntax);
                                OFString xferName3 = dcmFindNameOfUID(UID_JPEGProcess1TransferSyntax);
                                OFString xferName4 = dcmFindNameOfUID(UID_JPEGProcess2_4TransferSyntax);
                                OFString xferName5 = dcmFindNameOfUID(UID_JPEG2000LosslessOnlyTransferSyntax);
                                OFString xferName6 = dcmFindNameOfUID(UID_JPEG2000TransferSyntax);
                                /* create error message */
                                OFOStringStream oss;
                                oss << xferName1 << ", " << xferName2 << ", " << xferName3
                                    << ", " << xferName4 << ", " << xferName5 << " or " << xferName6
                                    << " expected: " << filename << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    printErrorMessage(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    printWarningMessage(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                            break;
                        case AP_XrayAngiographicDVD:
                            if (compare(mediaSOPClassUID, UID_XRayAngiographicImageStorage))
                            {
                                /* need to check multiple transfer syntaxes */
                                found = compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax) ||
                                        compare(transferSyntax, UID_JPEGProcess1TransferSyntax);
                                if (!found)
                                {
                                    OFString xferName1 = dcmFindNameOfUID(UID_JPEGProcess14SV1TransferSyntax);
                                    OFString xferName2 = dcmFindNameOfUID(UID_JPEGProcess1TransferSyntax);
                                    /* create error message */
                                    OFOStringStream oss;
                                    oss << xferName1 << " or " << xferName2 << " expected: "
                                         << filename << OFStringStream_ends;
                                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                    if (TransferSyntaxCheck)
                                    {
                                        printErrorMessage(tmpString);
                                        result = EC_ApplicationProfileViolated;
                                    } else
                                        printWarningMessage(tmpString);
                                    OFSTRINGSTREAM_FREESTR(tmpString)
                                }
                            }
                            break;
                        case AP_CTandMR:
                            /* need to check multiple transfer syntaxes */
                            found = compare(transferSyntax, UID_LittleEndianExplicitTransferSyntax) ||
                                    compare(transferSyntax, UID_JPEGProcess14SV1TransferSyntax);
                            if (!found)
                            {
                                OFString xferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax);
                                OFString xferName2 = dcmFindNameOfUID(UID_JPEGProcess14SV1TransferSyntax);
                                /* create error message */
                                OFOStringStream oss;
                                oss << xferName1 << " or " << xferName2 << " expected: "
                                     << filename << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    printErrorMessage(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    printWarningMessage(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
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
                                OFString xferName1 = dcmFindNameOfUID(UID_LittleEndianExplicitTransferSyntax);
                                OFString xferName2 = dcmFindNameOfUID(UID_RLELosslessTransferSyntax);
                                OFString xferName3 = dcmFindNameOfUID(UID_JPEGProcess1TransferSyntax);
                                /* create error message */
                                OFOStringStream oss;
                                oss << xferName1 << ", " << xferName2 << " or " << xferName3
                                    << " expected: " << filename << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    printErrorMessage(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    printWarningMessage(tmpString);
                                OFSTRINGSTREAM_FREESTR(tmpString)
                            }
                            break;
                        case AP_GeneralPurpose:
                        case AP_MPEG2MPatML:
                        case AP_DentalRadiograph:
                        default:
                        {
                            /* compare with expected transfer syntax */
                            found = compare(transferSyntax, expectedTransferSyntax);
                            if (!found)
                            {
                                OFString xferName = dcmFindNameOfUID(expectedTransferSyntax.c_str());
                                if (xferName.empty())
                                    xferName = expectedTransferSyntax;
                                /* create error message */
                                OFOStringStream oss;
                                oss << xferName << " expected: " << filename << OFStringStream_ends;
                                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                                if (TransferSyntaxCheck)
                                {
                                    printErrorMessage(tmpString);
                                    result = EC_ApplicationProfileViolated;
                                } else
                                    printWarningMessage(tmpString);
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
                                                           const char *filename)
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
    if (checkExistsWithStringValue(dataset, DCM_ImageType, "BIPLANE A", NULL /*no message*/) ||
        checkExistsWithStringValue(dataset, DCM_ImageType, "BIPLANE B", NULL /*no message*/))
    {
        /* create error message */
        OFOStringStream oss;
        oss << "BIPLANE images not allowed for " << getProfileName(ApplicationProfile)
            << " profile: " << filename << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printErrorMessage(tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        result = EC_ApplicationProfileViolated;
    }
    /* overlay data, if present, shall be encoded in OverlayData (60XX,3000) */
    for (unsigned int grp = 0x6000; grp < 0x601f; grp += 2)
    {
        /* check minimum number of attributes required for an overlay plane to be displayed */
        if (dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayRows.getElement())) &&
            dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayColumns.getElement())) &&
            dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitsAllocated.getElement())) &&
            dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitPosition.getElement())) &&
            !dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayData.getElement())))
        {
            /* create error message */
            OFOStringStream oss;
            oss << "embedded overlay data present in group 0x" << hex << grp
                << ", file: " << filename << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            printErrorMessage(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
            result = EC_ApplicationProfileViolated;
        }
    }
    return result;
}


// check whether dataset conforms to xray angiographic application profile
OFCondition DicomDirInterface::checkXrayAngiographicAttributes(DcmItem *dataset,
                                                               const OFString &sopClass,
                                                               const char *filename)
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
            result = EC_InvalidTag;
        else {
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
        for (unsigned int grp = 0x6000; grp < 0x601f; grp += 2)
        {
            /* check minimum number of attributes required for an overlay plane to be displayed */
            if (dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayRows.getElement())) &&
                dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayColumns.getElement())) &&
                dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitsAllocated.getElement())) &&
                dataset->tagExistsWithValue(DcmTagKey(grp, DCM_OverlayBitPosition.getElement())))
            {
                /* create error message */
                OFOStringStream oss;
                oss << "overlay group 0x" << hex << grp
                    << " present in file: " << filename << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                printErrorMessage(tmpString);
                OFSTRINGSTREAM_FREESTR(tmpString)
                result = EC_ApplicationProfileViolated;
            }
        }
    } else /* wrong SOP class */
        result = EC_ApplicationProfileViolated;
    return result;
}


// check whether dataset conforms to dental radiograph application profile
OFCondition DicomDirInterface::checkDentalRadiographAttributes(DcmItem *dataset,
                                                               const char *filename)
{
    OFCondition result = EC_Normal;
    /* check presence of type 2 elements */
    if (!checkExists(dataset, DCM_InstitutionName, filename) ||
        !checkExists(dataset, DCM_ManufacturersModelName, filename) ||
        !checkExists(dataset, DCM_DetectorID, filename) ||
        !checkExists(dataset, DCM_DetectorManufacturerName, filename) ||
        !checkExists(dataset, DCM_DetectorManufacturersModelName, filename))
    {
        result = EC_TagNotFound;
    }
    /* check for value constraints */
    else if (!checkExistsWithValue(dataset, DCM_BitsAllocated, filename) ||
             !checkExistsWithValue(dataset, DCM_BitsStored, filename))
    {
        result = EC_InvalidTag;
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
        if (((bs == 8) && (ba != 8)) || (bs != 8) && (ba != 16))
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
                                                      const char *filename)
{
    OFCondition result = EC_Normal;
    /* check presence of type 1 elements */
    if (!checkExistsWithValue(dataset, DCM_Rows, filename) ||
        !checkExistsWithValue(dataset, DCM_Columns, filename))
    {
        result = EC_InvalidTag;
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
            result = EC_InvalidTag;
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
                    result = EC_InvalidTag;
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
                                                         const char *filename)
{
    OFCondition result = EC_Normal;
    /* a US image */
    if (!checkExistsWithValue(dataset, DCM_PhotometricInterpretation, filename))
        result = EC_InvalidTag;
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
            /* report any error within the 'while' loop as an violoation of the application profile */
            result = EC_ApplicationProfileViolated;
        }
    }
    return result;
}


// check the given file (dataset) for mandatory attributes
OFCondition DicomDirInterface::checkMandatoryAttributes(DcmMetaInfo *metainfo,
                                                        DcmItem *dataset,
                                                        const char *filename)
{
    OFCondition result = EC_IllegalParameter;
    if ((filename != NULL) && (dataset != NULL))
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
        /* hanging protocol files are checked separately */
        if (recordType == ERT_HangingProtocol)
        {
            /* nothing to check since all type 1 and 2 attributes are identical */
        } else {
            /* PatientID is type 1 in DICOMDIR and type 2 in images. */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_PatientID, filename))
                    result = EC_InvalidTag;
            }
            /* PatientsName is type 2 in DICOMDIR and images */
            if (!checkExists(dataset, DCM_PatientsName, filename))
                result = EC_TagNotFound;
            /* StudyDate is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_StudyDate, filename))
                    result = EC_InvalidTag;
            }
            /* StudyTime is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_StudyTime, filename))
                    result = EC_InvalidTag;
            }
            /* StudyDescription is type 2 in DICOMDIR and type 3 in images.
               We can create an empty attribute in the directory
             */
            /* StudyInstanceUID is type 1 in DICOMDIR and images */
            if (!checkExistsWithValue(dataset, DCM_StudyInstanceUID, filename))
                result = EC_InvalidTag;
            /* StudyID is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_StudyID, filename))
                    result = EC_InvalidTag;
            }
            /* AccessionNumber is type 2 in DICOMDIR and type 3 in images
               We can create an empty attribute in the directory
            */
            /* Modality is type 1 in DICOMDIR and type 1 in images */
            if (!checkExistsWithValue(dataset, DCM_Modality, filename))
                result = EC_InvalidTag;
            /* SeriesInstanceUID is type 1 in DICOMDIR and type 1 in images */
            if (!checkExistsWithValue(dataset, DCM_SeriesInstanceUID, filename))
                result = EC_InvalidTag;
            /* SeriesNumber is type 1 in DICOMDIR and type 2 in images */
            if (!InventMode)
            {
                if (!checkExistsWithValue(dataset, DCM_SeriesNumber, filename))
                    result = EC_InvalidTag;
            }
            /* image and other numbers are type 1 in DICOMDIR but type 2 in images */
            switch (recordType)
            {
                case ERT_Overlay:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_OverlayNumber, filename))
                            result = EC_InvalidTag;
                    }
                    break;
                case ERT_ModalityLut:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_LookupTableNumber, filename))
                            result = EC_InvalidTag;
                    }
                    break;
                case ERT_VoiLut:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_LookupTableNumber, filename))
                            result = EC_InvalidTag;
                    }
                    break;
                case ERT_Curve:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_CurveNumber, filename))
                            result = EC_InvalidTag;
                    }
                    break;
                case ERT_SRDocument:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_CompletionFlag, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_VerificationFlag, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ConceptNameCodeSequence, filename))
                        result = EC_InvalidTag;
                    {
                        OFString tmpString;
                        if (compare(getStringFromDataset(dataset, DCM_VerificationFlag, tmpString), "VERIFIED"))
                        {
                            /* VerificationDateTime is required if verification flag is VERIFIED,
                               retrieve most recent (= last) entry from VerifyingObserverSequence */
                            DcmItem *ditem = NULL;
                            result = dataset->findAndGetSequenceItem(DCM_VerifyingObserverSequence, ditem, -1 /*last*/);
                            if (result.good())
                            {
                                if (!checkExistsWithValue(ditem, DCM_VerificationDateTime, filename))
                                    result = EC_InvalidTag;
                            }
                        }
                    }
                    break;
                case ERT_Presentation:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentLabel, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_PresentationCreationDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_PresentationCreationTime, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ReferencedSeriesSequence, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_Waveform:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_RTDose:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                            result = EC_InvalidTag;
                    }
                    if (!checkExistsWithValue(dataset, DCM_DoseSummationType, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_RTStructureSet:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                            result = EC_InvalidTag;
                    }
                    if (!checkExistsWithValue(dataset, DCM_StructureSetLabel, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_RTPlan:
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                            result = EC_InvalidTag;
                    }
                    if (!checkExistsWithValue(dataset, DCM_RTPlanLabel, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_RTTreatRecord:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_StoredPrint:
                    /* (nothing to do) */
                    break;
                case ERT_KeyObjectDoc:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ConceptNameCodeSequence, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_Registration:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentLabel, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_Fiducial:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentLabel, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_RawData:
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    /* InstanceNumber is type 2 in IOD and directory record! */
                    break;
                case ERT_Spectroscopy:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ImageType, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_NumberOfFrames, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_Rows, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_Columns, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_DataPointRows, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_DataPointColumns, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_EncapDoc:
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_MIMETypeOfEncapsulatedDocument, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_ValueMap:
                    if (!checkExistsWithValue(dataset, DCM_ContentDate, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentTime, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                        result = EC_InvalidTag;
                    if (!checkExistsWithValue(dataset, DCM_ContentLabel, filename))
                        result = EC_InvalidTag;
                    break;
                case ERT_Stereometric:
                    /* nothing to check */
                    break;
                default:
                    /* it can only be an image */
                    if (!InventMode)
                    {
                        if (!checkExistsWithValue(dataset, DCM_InstanceNumber, filename))
                            result = EC_InvalidTag;
                    }
                    /* check profile specific requirements */
                    if ((ApplicationProfile == AP_GeneralPurposeDVD) ||
                        (ApplicationProfile == AP_USBandFlash) ||
                        (ApplicationProfile == AP_MPEG2MPatML))
                    {
                        /* check presence of type 1 elements */
                        if (!checkExistsWithValue(dataset, DCM_Rows, filename) ||
                            !checkExistsWithValue(dataset, DCM_Columns, filename))
                        {
                            result = EC_InvalidTag;
                        }
                    }
                    else if (ApplicationProfile == AP_BasicCardiac)
                        result = checkBasicCardiacAttributes(dataset, filename);
                    else if ((ApplicationProfile == AP_XrayAngiographic) || (ApplicationProfile == AP_XrayAngiographicDVD))
                        result = checkXrayAngiographicAttributes(dataset, mediaSOPClassUID, filename);
                    else if (ApplicationProfile == AP_DentalRadiograph)
                        result = checkDentalRadiographAttributes(dataset, filename);
                    else if (ApplicationProfile == AP_CTandMR)
                        result = checkCTandMRAttributes(dataset, mediaSOPClassUID, filename);
                    else if ((ApplicationProfile == AP_UltrasoundIDSF) ||
                             (ApplicationProfile == AP_UltrasoundSCSF) ||
                             (ApplicationProfile == AP_UltrasoundCCSF) ||
                             (ApplicationProfile == AP_UltrasoundIDMF) ||
                             (ApplicationProfile == AP_UltrasoundSCMF) ||
                             (ApplicationProfile == AP_UltrasoundCCMF))
                    {
                        result = checkUltrasoundAttributes(dataset, transferSyntax, filename);
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
OFCondition DicomDirInterface::checkDicomFile(const char *filename,
                                              const char *directory)
{
    /* define fileformat object for the DICOM file to be loaded */
    DcmFileFormat fileformat;
    /* call the "real" function */
    return loadAndCheckDicomFile(filename, directory, fileformat);
}


// load DICOM file and check whether it is suitable for a DICOMDIR of the specified application profile
OFCondition DicomDirInterface::loadAndCheckDicomFile(const char *filename,
                                                     const char *directory,
                                                     DcmFileFormat &fileformat)
{
    OFCondition result = EC_IllegalParameter;
    /* create fully qualified pathname of the DICOM file to be added */
    OFString pathname;
    OFStandard::combineDirAndFilename(pathname, directory, filename, OFTrue /*allowEmptyDirName*/);
    if (VerboseMode)
        printMessage("checking file: ", pathname.c_str());
    /* check filename */
    if (isFilenameValid(filename))
    {
        /* load DICOM file */
        result = fileformat.loadFile(pathname.c_str());
        if (result.good())
        {
            /* check for correct part 10 file format */
            DcmMetaInfo *metainfo = fileformat.getMetaInfo();
            if ((metainfo == NULL) || (metainfo->card() == 0))
            {
                printErrorMessage("file not part 10 format (no metainfo-header): ", filename);
                result = EC_InvalidStream;
            }
            DcmDataset *dataset = fileformat.getDataset();
            if (dataset == NULL)
            {
                printErrorMessage("file contains no data (no dataset): ", filename);
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
            printFileErrorMessage(result, "reading", filename);
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
                    /* optional: check whether PatientsName also matches */
                    if (result && !compare(getStringFromDataset(record, DCM_PatientsName, recordString),
                                           getStringFromDataset(dataset, DCM_PatientsName, datasetString)))
                    {
                        if (InventPatientIDMode)
                        {
                            printWarningMessage("PatientsName inconsistent for PatientID: ", patientID.c_str());
                            /* remove current patient ID, will be replaced later */
                            dataset->putAndInsertString(DCM_PatientID, "");
                            result = OFFalse;
                        }
                    }
                } else {
                    /* if there is no value for PatientID in the dataset try using the PatientsName */
                    result = compare(getStringFromDataset(record, DCM_PatientsName, recordString),
                                     getStringFromDataset(dataset, DCM_PatientsName, datasetString));
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
                            result = compare(getStringFromFile(hostFilename.c_str(), DCM_StudyInstanceUID, recordString),
                                             getStringFromDataset(dataset, DCM_StudyInstanceUID, datasetString));
                        } else
                            printErrorMessage("cannot locate referenced file: ", refFilename.c_str());
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
                /* The attribute ReferencedSOPInstanceUID is automatically
                 * put into a Directory Record when a filename is present.
                */
                result = compare(getStringFromDataset(record, DCM_ReferencedSOPInstanceUIDInFile, recordString),
                                 getStringFromDataset(dataset, DCM_SOPInstanceUID, datasetString));
                break;
            default:
                printErrorMessage("record type not yet implemented");
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


// create new patient record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildPatientRecord(DcmItem *dataset,
                                                          const OFString &sourceFilename)
{
    /* create new patient record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Patient, NULL, sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_PatientID, record);
            copyElement(dataset, DCM_PatientsName, record);
            if ((ApplicationProfile == AP_GeneralPurposeDVD) ||
                (ApplicationProfile == AP_USBandFlash) ||
                (ApplicationProfile == AP_MPEG2MPatML))
            {
                /* additional type 1C keys specified by specific profiles */
                copyElement(dataset, DCM_PatientsBirthDate, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
                copyElement(dataset, DCM_PatientsSex, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
            }
            else if ((ApplicationProfile == AP_BasicCardiac) ||
                     (ApplicationProfile == AP_XrayAngiographic) ||
                     (ApplicationProfile == AP_XrayAngiographicDVD))
            {
                /* additional type 2 keys specified by specific profiles */
                copyStringWithDefault(dataset, DCM_PatientsBirthDate, record);
                copyStringWithDefault(dataset, DCM_PatientsSex, record);
            }
        } else {
            printRecordErrorMessage(record->error(), ERT_Patient, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printErrorMessage("out of memory (creating patient record)");
    return record;
}


// create new study record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStudyRecord(DcmItem *dataset,
                                                        const OFString &sourceFilename)
{
    /* create new study record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Study, NULL, sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            OFString tmpString;
            /* copy attribute values from dataset to study record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyStringWithDefault(dataset, DCM_StudyDate, record, alternativeStudyDate(dataset, tmpString).c_str(), OFTrue /*printWarning*/);
            copyStringWithDefault(dataset, DCM_StudyTime, record, alternativeStudyTime(dataset, tmpString).c_str(), OFTrue /*printWarning*/);
            copyElement(dataset, DCM_StudyDescription, record);
            copyElement(dataset, DCM_StudyInstanceUID, record);
            copyElement(dataset, DCM_StudyID, record);
            copyElement(dataset, DCM_AccessionNumber, record);
        } else {
            printRecordErrorMessage(record->error(), ERT_Study, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printErrorMessage("out of memory (creating study record)");
    return record;
}


// create new series record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildSeriesRecord(DcmItem *dataset,
                                                         const OFString &sourceFilename)
{
    /* create new series record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Series, NULL, sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to series record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_Modality, record);
            copyElement(dataset, DCM_SeriesInstanceUID, record);
            copyElement(dataset, DCM_SeriesNumber, record);
            if ((ApplicationProfile == AP_GeneralPurposeDVD) ||
                (ApplicationProfile == AP_USBandFlash) ||
                (ApplicationProfile == AP_MPEG2MPatML))
            {
                /* additional type 1C keys specified by specific profiles */
                copyElement(dataset, DCM_InstitutionName, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
                copyElement(dataset, DCM_InstitutionAddress, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
                copyElement(dataset, DCM_PerformingPhysiciansName, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
            }
            else if ((ApplicationProfile == AP_BasicCardiac) ||
                     (ApplicationProfile == AP_XrayAngiographic) ||
                     (ApplicationProfile == AP_XrayAngiographicDVD))
            {
                /* additional type 2 keys specified by specific profiles */
                copyStringWithDefault(dataset, DCM_InstitutionName, record);
                copyStringWithDefault(dataset, DCM_InstitutionAddress, record);
                copyStringWithDefault(dataset, DCM_PerformingPhysiciansName, record);
            }
        } else {
            printRecordErrorMessage(record->error(), ERT_Series, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printErrorMessage("out of memory (creating series record)");
    return record;
}


// create new overlay record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildOverlayRecord(DcmItem *dataset,
                                                          const OFString &referencedFileID,
                                                          const OFString &sourceFilename)
{
    /* create new overlay record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Overlay, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to overlay record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_OverlayNumber, record);
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


// create new modality lut record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildModalityLutRecord(DcmItem *dataset,
                                                              const OFString &referencedFileID,
                                                              const OFString &sourceFilename)
{
    /* create new modality lut record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_ModalityLut, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to modality lut record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_LookupTableNumber, record);
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


// create new voi lut record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildVoiLutRecord(DcmItem *dataset,
                                                         const OFString &referencedFileID,
                                                         const OFString &sourceFilename)
{
    /* create new voi lut record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_VoiLut, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to voi lut record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_LookupTableNumber, record);
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


// create new curve record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildCurveRecord(DcmItem *dataset,
                                                        const OFString &referencedFileID,
                                                        const OFString &sourceFilename)
{
    /* create new curve record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Curve, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to curve record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_CurveNumber, record);
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


// create new structure reporting record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStructReportRecord(DcmItem *dataset,
                                                               const OFString &referencedFileID,
                                                               const OFString &sourceFilename)
{
    /* create new struct report record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_SRDocument, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            OFString tmpString;
            /* copy attribute values from dataset to struct report record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_CompletionFlag, record);
            copyElement(dataset, DCM_VerificationFlag, record);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
            if (compare(getStringFromDataset(dataset, DCM_VerificationFlag, tmpString), "VERIFIED"))
            {
                /* VerificationDateTime is required if verification flag is VERIFIED,
                   retrieve most recent (= last) entry from VerifyingObserverSequence */
                DcmItem *ditem = NULL;
                if (dataset->findAndGetSequenceItem(DCM_VerifyingObserverSequence, ditem, -1 /*last*/).good())
                    copyElement(ditem, DCM_VerificationDateTime, record);
            }
            copyElement(dataset, DCM_ConceptNameCodeSequence, record);
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


// create new presentation state record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildPresentationRecord(DcmItem *dataset,
                                                               const OFString &referencedFileID,
                                                               const OFString &sourceFilename)
{
    /* create new presentation record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Presentation, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to presentation record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_ContentLabel, record);
            copyElement(dataset, DCM_PresentationCreationDate, record);
            copyElement(dataset, DCM_PresentationCreationTime, record);
            copyElement(dataset, DCM_ContentCreatorsName, record);
            copyElement(dataset, DCM_ReferencedSeriesSequence, record);
            copyStringWithDefault(dataset, DCM_ContentDescription, record);
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


// create new waveform record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildWaveformRecord(DcmItem *dataset,
                                                           const OFString &referencedFileID,
                                                           const OFString &sourceFilename)
{
    /* create new waveform record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Waveform, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to waveform record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
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


// create new rt dose record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTDoseRecord(DcmItem *dataset,
                                                         const OFString &referencedFileID,
                                                         const OFString &sourceFilename)
{
    /* create new rt dose record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_RTDose, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to rt dose record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_DoseSummationType, record);
            copyElement(dataset, DCM_DoseComment, record);
            copyElement(dataset, DCM_IconImageSequence, record, OFTrue /*optional*/);
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


// create new rt structure set record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTStructureSetRecord(DcmItem *dataset,
                                                                 const OFString &referencedFileID,
                                                                 const OFString &sourceFilename)
{
    /* create new rt structure set record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_RTStructureSet, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to rt structure set record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_StructureSetLabel, record);
            copyElement(dataset, DCM_StructureSetDate, record);
            copyElement(dataset, DCM_StructureSetTime, record);
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


// create new rt plan record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTPlanRecord(DcmItem *dataset,
                                                         const OFString &referencedFileID,
                                                         const OFString &sourceFilename)
{
    /* create new rt plan record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_RTPlan, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to rt plan record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_RTPlanLabel, record);
            copyElement(dataset, DCM_RTPlanDate, record);
            copyElement(dataset, DCM_RTPlanTime, record);
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


// create new rt treatment record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRTTreatmentRecord(DcmItem *dataset,
                                                              const OFString &referencedFileID,
                                                              const OFString &sourceFilename)
{
    /* create new rt treatment record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_RTTreatRecord, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to rt treatment record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_TreatmentDate, record);
            copyElement(dataset, DCM_TreatmentTime, record);
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


// create new stored print record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStoredPrintRecord(DcmItem *dataset,
                                                              const OFString &referencedFileID,
                                                              const OFString &sourceFilename)
{
    /* create new stored print record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_StoredPrint, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to stored print record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_IconImageSequence, record, OFTrue /*optional*/);
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


// create new key object doc record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildKeyObjectDocRecord(DcmItem *dataset,
                                                               const OFString &referencedFileID,
                                                               const OFString &sourceFilename)
{
    /* create new key object doc record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_KeyObjectDoc, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to key object doc record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
            copyElement(dataset, DCM_ConceptNameCodeSequence, record);
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


// create new registration record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRegistrationRecord(DcmItem *dataset,
                                                               const OFString &referencedFileID,
                                                               const OFString &sourceFilename)
{
    /* create new registration record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Registration, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to registration record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_ContentLabel, record);
            copyStringWithDefault(dataset, DCM_ContentDescription, record);
            copyStringWithDefault(dataset, DCM_ContentCreatorsName, record);
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


// create new fiducial record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildFiducialRecord(DcmItem *dataset,
                                                           const OFString &referencedFileID,
                                                           const OFString &sourceFilename)
{
    /* create new fiducial record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Fiducial, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to fiducial record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_ContentLabel, record);
            copyStringWithDefault(dataset, DCM_ContentDescription, record);
            copyStringWithDefault(dataset, DCM_ContentCreatorsName, record);
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


// create new raw data record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildRawDataRecord(DcmItem *dataset,
                                                          const OFString &referencedFileID,
                                                          const OFString &sourceFilename)
{
    /* create new raw data record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_RawData, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to raw data record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
            /* InstanceNumber is type 2 in IOD and directory record! */
            copyStringWithDefault(dataset, DCM_InstanceNumber, record);
            /* IconImageSequence (type 3) not supported */
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


// create new spectroscopy record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildSpectroscopyRecord(DcmItem *dataset,
                                                               const OFString &referencedFileID,
                                                               const OFString &sourceFilename)
{
    /* create new spectroscopy record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Spectroscopy, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to spectroscopy record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_ImageType, record);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_ReferencedImageEvidenceSequence, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_NumberOfFrames, record);
            copyElement(dataset, DCM_Rows, record);
            copyElement(dataset, DCM_Columns, record);
            copyElement(dataset, DCM_DataPointRows, record);
            copyElement(dataset, DCM_DataPointColumns, record);
            /* IconImageSequence (type 3) not supported */

            /* application profile specific attributes */
            if ((ApplicationProfile == AP_GeneralPurposeDVD) ||
                (ApplicationProfile == AP_USBandFlash))
            {
                /* type 1 */
                copyElement(dataset, DCM_Rows, record);
                copyElement(dataset, DCM_Columns, record);
                /* type 1C */
                copyElement(dataset, DCM_FrameOfReferenceUID, record, OFTrue /*optional*/);
                copyElement(dataset, DCM_SynchronizationFrameOfReferenceUID, record, OFTrue /*optional*/);
                copyElement(dataset, DCM_NumberOfFrames, record, OFTrue /*optional*/);
                copyElement(dataset, DCM_AcquisitionTimeSynchronized, record, OFTrue /*optional*/);
                copyElement(dataset, DCM_AcquisitionDatetime, record, OFTrue /*optional*/);
                // tbd: need to examine functional groups for the following attributes
                copyElement(dataset, DCM_ReferencedImageSequence, record, OFTrue /*optional*/);
                copyElement(dataset, DCM_ImagePositionPatient, record, OFTrue /*optional*/);
                copyElement(dataset, DCM_ImageOrientationPatient, record, OFTrue /*optional*/);
                copyElement(dataset, DCM_PixelSpacing, record, OFTrue /*optional*/);
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


// create new encap doc record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildEncapDocRecord(DcmItem *dataset,
                                                           const OFString &referencedFileID,
                                                           const OFString &sourceFilename)
{
    /* create new encap doc record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_EncapDoc, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to encap doc record */
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_MIMETypeOfEncapsulatedDocument, record);
            copyStringWithDefault(dataset, DCM_ContentDate, record);
            copyStringWithDefault(dataset, DCM_ContentTime, record);
            copyStringWithDefault(dataset, DCM_DocumentTitle, record);
            /* baseline context group 7020 is not checked */
            copyElement(dataset, DCM_ConceptNameCodeSequence, record, OFTrue /*optional*/);
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


// create new value map record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildValueMapRecord(DcmItem *dataset,
                                                           const OFString &referencedFileID,
                                                           const OFString &sourceFilename)
{
    /* create new value map record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_ValueMap, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to value map record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_ContentDate, record);
            copyElement(dataset, DCM_ContentTime, record);
            copyElement(dataset, DCM_InstanceNumber, record);
            copyElement(dataset, DCM_ContentLabel, record);
            copyStringWithDefault(dataset, DCM_ContentDescription, record);
            copyStringWithDefault(dataset, DCM_ContentCreatorsName, record);
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


// create new stereometric record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildStereometricRecord(DcmItem *dataset,
                                                               const OFString &referencedFileID,
                                                               const OFString &sourceFilename)
{
    /* create new value map record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Stereometric, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            /* copy attribute values from dataset to stereometric record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
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


// create new image record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildImageRecord(DcmItem *dataset,
                                                        const OFString &referencedFileID,
                                                        const OFString &sourceFilename)
{
    /* create new image record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_Image, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            OFBool iconImage = IconImageMode;
            unsigned int iconSize = (IconSize == 0) ? 64 : IconSize;
            /* Icon Image Sequence required for particular profiles */
            OFBool iconRequired = OFFalse;
            /* copy attribute values from dataset to image record */
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_InstanceNumber, record);
            /* application profile specific attributes */
            switch (ApplicationProfile)
            {
                case AP_GeneralPurpose:
                    /* type 1C */
                    copyElement(dataset, DCM_ImageType, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_ReferencedImageSequence, record, OFTrue /*optional*/);
                    break;
                case AP_GeneralPurposeDVD:
                case AP_USBandFlash:
                    /* type 1 */
                    copyElement(dataset, DCM_Rows, record);
                    copyElement(dataset, DCM_Columns, record);
                    /* type 1C */
                    copyElement(dataset, DCM_ImageType, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_CalibrationImage, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
                    copyElement(dataset, DCM_LossyImageCompressionRatio, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
                    copyElement(dataset, DCM_FrameOfReferenceUID, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_SynchronizationFrameOfReferenceUID, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_NumberOfFrames, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_AcquisitionTimeSynchronized, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_AcquisitionDatetime, record, OFTrue /*optional*/);
                    // tbd: need to examine functional groups for the following attributes
                    copyElement(dataset, DCM_ReferencedImageSequence, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_ImagePositionPatient, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_ImageOrientationPatient, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_PixelSpacing, record, OFTrue /*optional*/);
                    break;
                case AP_MPEG2MPatML:
                    /* type 1 */
                    copyElement(dataset, DCM_Rows, record);
                    copyElement(dataset, DCM_Columns, record);
                    /* type 1C */
                    copyElement(dataset, DCM_ImageType, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_LossyImageCompressionRatio, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
                    break;
                case AP_XrayAngiographic:
                case AP_XrayAngiographicDVD:
                    copyElement(dataset, DCM_LossyImageCompressionRatio, record, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
                case AP_BasicCardiac:
                    {
                        OFString tmpString;
                        OFBool xaImage = compare(getStringFromDataset(dataset, DCM_SOPClassUID, tmpString), UID_XRayAngiographicImageStorage);
                        /* type 1C: required for XA images (type 1 for Basic Cardiac Profile) */
                        copyElement(dataset, DCM_ImageType, record, !xaImage /*optional*/);
                        /* additional type 2 keys specified by specific profiles */
                        copyStringWithDefault(dataset, DCM_CalibrationImage, record);
                        /* type 1C: required if ImageType is "BIPLANE A" or "BIPLANE B" */
                        getStringFromDataset(dataset, DCM_ImageType, tmpString);
                        OFBool bpImage = compare(tmpString, "BIPLANE A") || compare(tmpString, "BIPLANE B");
                        copyElement(dataset, DCM_ReferencedImageSequence, record, !bpImage /*optional*/);
                        /* icon images */
                        iconImage = OFTrue;
                        iconRequired = OFTrue;
                        iconSize = 128;
                    }
                    break;
                case AP_CTandMR:
                    /* type 1 */
                    copyElement(dataset, DCM_Rows, record);
                    copyElement(dataset, DCM_Columns, record);
                    /* type 1C */
                    copyElement(dataset, DCM_ReferencedImageSequence, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_ImagePositionPatient, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_ImageOrientationPatient, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_FrameOfReferenceUID, record, OFTrue /*optional*/);
                    copyElement(dataset, DCM_PixelSpacing, record, OFTrue /*optional*/);
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
                        printErrorMessage("cannot create IconImageSequence");
                    else
                        printWarningMessage("cannot create IconImageSequence");
                }
            }
        } else {
            printRecordErrorMessage(record->error(), ERT_Series, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printErrorMessage("out of memory (creating image record)");
    return record;
}


// create new hanging protocol record and copy required values from dataset
DcmDirectoryRecord *DicomDirInterface::buildHangingProtocolRecord(DcmItem *dataset,
                                                                  const OFString &referencedFileID,
                                                                  const OFString &sourceFilename)
{
    /* create new hanging protocol record */
    DcmDirectoryRecord *record = new DcmDirectoryRecord(ERT_HangingProtocol, referencedFileID.c_str(), sourceFilename.c_str());
    if (record != NULL)
    {
        /* check whether new record is ok */
        if (record->error().good())
        {
            copyElement(dataset, DCM_SpecificCharacterSet, record, OFTrue /*optional*/);
            copyElement(dataset, DCM_HangingProtocolName, record);
            copyElement(dataset, DCM_HangingProtocolDescription, record);
            copyElement(dataset, DCM_HangingProtocolLevel, record);
            copyElement(dataset, DCM_HangingProtocolCreator, record);
            copyElement(dataset, DCM_HangingProtocolCreationDatetime, record);
            copyElement(dataset, DCM_HangingProtocolDefinitionSequence, record);
            copyElement(dataset, DCM_NumberOfPriorsReferenced, record);
            copyElement(dataset, DCM_HangingProtocolUserIdentificationCodeSequence, record, OFTrue /*optional*/);
        } else {
            printRecordErrorMessage(record->error(), ERT_HangingProtocol, "create");
            /* free memory */
            delete record;
            record = NULL;
        }
    } else
        printErrorMessage("out of memory (creating patient record)");
    return record;
}


// create icon image from PGM file
OFBool DicomDirInterface::getIconFromFile(const OFString &filename,
                                          Uint8 *pixel,
                                          const unsigned long count,
                                          const unsigned int width,
                                          const unsigned int height)
{
    OFBool result = OFFalse;
    /* check buffer and size */
    if ((pixel != NULL) && (count >= width * height))
    {
        /* open specified file */
        FILE *file = fopen(filename.c_str(), "rb");
        if (file != NULL)
        {
            /* according to the pgm format no line should be longer than 70 characters */
            const int maxline = 256;
            char line[maxline], text[maxline];
            /* read magic number */
            if ((fgets(line, maxline, file) != NULL) && (sscanf(line, "%s", text) > 0) && (strcmp(text, "P5") == 0))
            {
                OFBool corrupt = OFTrue;
                if ((fgets(line, maxline, file) != NULL) && (sscanf(line, "%s", text) > 0))
                {
                    unsigned int pgmWidth, pgmHeight = 0;
                    /* skip optional comment line and get width and height */
                    if (((*text != '#') || (fgets(line, maxline, file) != NULL)) &&
                        (sscanf(line, "%u %u", &pgmWidth, &pgmHeight) > 0) && (pgmWidth > 0) && (pgmHeight > 0))
                    {
                        unsigned int pgmMax = 0;
                        /* get maximum gray value */
                        if ((fgets(line, maxline, file) != NULL) && (sscanf(line, "%u", &pgmMax) > 0) && (pgmMax == 255))
                        {
                            const unsigned long pgmSize = pgmWidth * pgmHeight;
                            Uint8 *pgmData = new Uint8[pgmSize];
                            if (pgmData != NULL)
                            {
                                /* get pgm image data */
                                if (fread(pgmData, sizeof(Uint8), OFstatic_cast(size_t, pgmSize), file) == pgmSize)
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
                                        printErrorMessage("cannot scale external icon, no image support available");
                                    corrupt = OFFalse;
                                }
                                /* free memory */
                                delete[] pgmData;
                            } else {
                                printFileErrorMessage(EC_MemoryExhausted, "cannot allocate memory for pgm pixel data", NULL /*filename*/);
                                /* avoid double reporting of error message */
                                corrupt = OFFalse;
                            }
                        }
                    }
                }
                if (corrupt)
                    printErrorMessage("corrupt file format for external icon (not pgm binary)");
            } else
                printErrorMessage("wrong file format for external icon (pgm required)");
            fclose(file);
        } else
            printErrorMessage("cannot open file for external icon: ", filename.c_str());
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
        /* choose representitive frame */
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
        /* optimization for compressed multiframe images */
        if (fCount > 1)
        {
            DcmElement *delem = NULL;
            /* search for PixelData element on top-level */
            if (dataset->findAndGetElement(DCM_PixelData, delem, OFFalse /*searchIntoSub*/).good())
            {
                DcmPixelData *dpix = OFstatic_cast(DcmPixelData *, delem);
                /* get pixel data sequence (if available) */
                E_TransferSyntax xfer = EXS_Unknown;
                const DcmRepresentationParameter *param = NULL;
                dpix->getOriginalRepresentationKey(xfer, param);
                if ((xfer != EXS_Unknown) && DcmXfer(xfer).isEncapsulated())
                {
                    DcmPixelSequence *pixSeq = NULL;
                    if (dpix->getEncapsulatedRepresentation(xfer, param, pixSeq).good() && (pixSeq != NULL))
                    {
                        /* check whether each frame is stored in a separate pixel item */
                        if (pixSeq->card() == OFstatic_cast(unsigned long, fCount + 1))
                        {
                            DcmPixelItem *pixItem = NULL;
                            long i;
                            /* delete all pixel items (apart from item #0) before ... */
                            for (i = 1; i < frame; i++)
                            {
                                if (pixSeq->remove(pixItem, 1).good())
                                    delete pixItem;
                            }
                            /* ... and after representative frame/item */
                            for (i = frame; i < fCount; i++)
                            {
                                if (pixSeq->remove(pixItem, 2).good())
                                    delete pixItem;
                            }
                            /* and finally, only 1 frame remains */
                            frame = 1;
                            fCount = 1;
                            /* adjust attributes in dataset to avoid errors in dcmimage */
                            dataset->putAndInsertString(DCM_NumberOfFrames, "1");
                            dataset->putAndInsertUint16(DCM_RepresentativeFrameNumber, 1);
                        }
                    }
                }
            }
        }
        /* scale image (if required) and retrieve pixel data from dataset */
        result = ImagePlugin->scaleImage(dataset, pixel, count, OFstatic_cast(unsigned long, frame), width, height);
    }
    return result;
}


// add icon image sequence to record
OFCondition DicomDirInterface::addIconImage(DcmDirectoryRecord *record,
                                            DcmItem *dataset,
                                            const unsigned int size,
                                            const OFString &sourceFilename)
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
            ditem->putAndInsertUint16(DCM_Rows, height);
            ditem->putAndInsertUint16(DCM_Columns, width);
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
                if (!IconPrefix.empty())
                {
                    /* try to load external pgm icon */
                    iconOk = getIconFromFile(IconPrefix + sourceFilename, pixel, count, width, height);
                } else {
                    /* try to create icon from dataset */
                    iconOk = getIconFromDataset(dataset, pixel, count, width, height);
                    if (!iconOk)
                        printWarningMessage("cannot create monochrome icon from image file, using default");
                }
                /* could not create icon so far: use default icon (if specified) */
                if (!iconOk && !DefaultIcon.empty())
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
                                                 DcmItem *dataset,
                                                 const OFString &referencedFileID,
                                                 const OFString &sourceFilename)
{
    DcmDirectoryRecord *record = NULL;
    if (parent != NULL)
    {
        /* check whether record already exists */
        record = findExistingRecord(parent, recordType, dataset);
        if (record == NULL)
        {
            /* if not, create a new one */
            switch (recordType)
            {
                case ERT_Patient:
                    record = buildPatientRecord(dataset, sourceFilename);
                    break;
                case ERT_Study:
                    record = buildStudyRecord(dataset, sourceFilename);
                    break;
                case ERT_Series:
                    record = buildSeriesRecord(dataset, sourceFilename);
                    break;
                case ERT_Overlay:
                    record = buildOverlayRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_ModalityLut:
                    record = buildModalityLutRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_VoiLut:
                    record = buildVoiLutRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_Curve:
                    record = buildCurveRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_SRDocument:
                    record = buildStructReportRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_Presentation:
                    record = buildPresentationRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_Waveform:
                    record = buildWaveformRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_RTDose:
                    record = buildRTDoseRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_RTStructureSet:
                    record = buildRTStructureSetRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_RTPlan:
                    record = buildRTPlanRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_RTTreatRecord:
                    record = buildRTTreatmentRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_StoredPrint:
                    record = buildStoredPrintRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_KeyObjectDoc:
                    record = buildKeyObjectDocRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_Registration:
                    record = buildRegistrationRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_Fiducial:
                    record = buildFiducialRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_RawData:
                    record = buildRawDataRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_Spectroscopy:
                    record = buildSpectroscopyRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_EncapDoc:
                    record = buildEncapDocRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_ValueMap:
                    record = buildValueMapRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_HangingProtocol:
                    record = buildHangingProtocolRecord(dataset, referencedFileID, sourceFilename);
                    break;
                case ERT_Stereometric:
                    record = buildStereometricRecord(dataset, referencedFileID, sourceFilename);
                    break;
                default:
                    /* it can only be an image */
                    record = buildImageRecord(dataset, referencedFileID, sourceFilename);
            }
            if (record != NULL)
            {
                OFCondition status = EC_Normal;
                /* perform consistency check */
                if (ConsistencyCheck)
                {
                    /* abort on any inconsistancy */
                    if (warnAboutInconsistentAttributes(record, dataset, sourceFilename, AbortMode) && AbortMode)
                        status = EC_CorruptedData;
                }
                /* and insert it below parent record */
                if (status.good())
                    status = insertSortedUnder(parent, record);
                if (status.bad())
                {
                    printRecordErrorMessage(status, recordType, "insert");
                    /* free memory */
                    delete record;
                    record = NULL;
                }
            }
        }
        if (record != NULL)
        {
            /* check whether instance is already listed */
            if (record->getRecordsOriginFile() == NULL)
                record->setRecordsOriginFile(sourceFilename.c_str());
        }
    }
    return record;
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
                    if (!record->tagExistsWithValue(DCM_InstanceNumber))
                        setDefaultValue(record, DCM_InstanceNumber, AutoInstanceNumber++);
                    break;
                case ERT_Overlay:
                    if (!record->tagExistsWithValue(DCM_OverlayNumber))
                        setDefaultValue(record, DCM_OverlayNumber, AutoOverlayNumber++);
                    break;
                case ERT_ModalityLut:
                case ERT_VoiLut:
                    if (!record->tagExistsWithValue(DCM_LookupTableNumber))
                        setDefaultValue(record, DCM_LookupTableNumber, AutoLutNumber++);
                    break;
                case ERT_Curve:
                    if (!record->tagExistsWithValue(DCM_CurveNumber))
                        setDefaultValue(record, DCM_CurveNumber, AutoCurveNumber++);
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
OFCondition DicomDirInterface::addDicomFile(const char *filename,
                                            const char *directory)
{
    OFCondition result = EC_IllegalParameter;
    /* first make sure that a DICOMDIR object exists */
    if (DicomDir != NULL)
    {
        /* create fully qualified pathname of the DICOM file to be added */
        OFString pathname;
        OFStandard::combineDirAndFilename(pathname, directory, filename, OFTrue /*allowEmptyDirName*/);
        /* then check the file name, load the file and check the content */
        DcmFileFormat fileformat;
        result = loadAndCheckDicomFile(filename, directory, fileformat);
        if (result.good())
        {
            if (VerboseMode)
                printMessage("adding file: ", pathname.c_str());
            /* start creating the DICOMDIR directory structure */
            DcmDirectoryRecord *rootRecord = &(DicomDir->getRootRecord());
            DcmMetaInfo *metainfo = fileformat.getMetaInfo();
            DcmDataset *dataset = fileformat.getDataset();
            /* massage filename into DICOM format (DOS conventions for path separators, uppercase) */
            OFString fileID;
            hostToDicomFilename(filename, fileID);
            /* what kind of object (SOP Class) is stored in the file */
            OFString sopClass;
            metainfo->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass);
            /* if hanging protocol file then attach it to the root record and stop */
            if (compare(sopClass, UID_HangingProtocolStorage))
            {
                /* add a patient record below the root */
                DcmDirectoryRecord *hangingRecord = addRecord(rootRecord, ERT_HangingProtocol, dataset, fileID, pathname);
                if (hangingRecord == NULL)
                    result = EC_CorruptedData;
            } else {
                /* add a patient record below the root */
                DcmDirectoryRecord *patientRecord = addRecord(rootRecord, ERT_Patient, dataset, fileID, pathname);
                if (patientRecord != NULL)
                {
                    /* if patient management file then attach it to patient record and stop */
                    if (compare(sopClass, UID_DetachedPatientManagementMetaSOPClass))
                    {
                        result = patientRecord->assignToSOPFile(fileID.c_str(), pathname.c_str());
                        printFileErrorMessage(result, "cannot assign patient record to", pathname.c_str());
                    } else {
                        /* add a study record below the current patient record */
                        DcmDirectoryRecord *studyRecord = addRecord(patientRecord, ERT_Study, dataset, fileID, pathname);;
                        if (studyRecord != NULL)
                        {
                            /* add a series record below the current study record */
                            DcmDirectoryRecord *seriesRecord = addRecord(studyRecord, ERT_Series, dataset, fileID, pathname);;
                            if (seriesRecord != NULL)
                            {
                                /* add one of the instance record below the current series record */
                                if (addRecord(seriesRecord, sopClassToRecordType(sopClass), dataset, fileID, pathname) == NULL)
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


// set the fileset descriptor and character set
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
                if ((filename == NULL) || (strlen(filename) == 0))
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
                        if ((charset == NULL) || (strlen(charset) == 0))
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
OFCondition DicomDirInterface::setIconPrefix(const char *prefix)
{
    IconPrefix = prefix;
    return EC_Normal;
}


// set filename for default image icon which is used in case of error
OFCondition DicomDirInterface::setDefaultIcon(const char *filename)
{
    DefaultIcon = filename;
    return EC_Normal;
}


// enable/disable the verbose mode
OFBool DicomDirInterface::enableVerboseMode(const OFBool newMode)
{
    /* save current mode */
    OFBool oldMode = VerboseMode;
    /* set new mode */
    VerboseMode = newMode;
    /* return old mode */
    return oldMode;
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


// enable/disable consistency check, i.e. whether the file is checked for consistency
// with the DICOMDIR record
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


// set the log stream used for verbose and error messages
void DicomDirInterface::setLogStream(OFConsole *stream)
{
    /* store log stream */
    LogStream = stream;
}


// print a message to the console (stdout)
void DicomDirInterface::printMessage(const char *message,
                                     const char *suffix)
{
    if ((LogStream != NULL) && (message != NULL))
    {
        LogStream->lockCout() << message;
        if (suffix != NULL)
            LogStream->getCout() << suffix;
        LogStream->getCout() << endl;
        LogStream->unlockCout();
    }
}


// print an warning message to the console (stderr)
void DicomDirInterface::printWarningMessage(const char *message,
                                            const char *suffix)
{
    if ((LogStream != NULL) && (message != NULL))
    {
        LogStream->lockCerr() << "Warning: " << message;
        if (suffix != NULL)
            LogStream->getCerr() << suffix;
        LogStream->getCerr() << endl;
        LogStream->unlockCerr();
    }
}


// print an error message to the console (stderr)
void DicomDirInterface::printErrorMessage(const char *message,
                                          const char *suffix)
{
    if ((LogStream != NULL) && (message != NULL))
    {
        LogStream->lockCerr() << "Error: " << message;
        if (suffix != NULL)
            LogStream->getCerr() << suffix;
        LogStream->getCerr() << endl;
        LogStream->unlockCerr();
    }
}


// print an error message to the console (stderr) that the value of the given tag is unexpected
void DicomDirInterface::printUnexpectedValueMessage(const DcmTagKey &key,
                                                    const char *filename,
                                                    const OFBool errorMsg)
{
    if (LogStream != NULL)
    {
        LogStream->lockCerr();
        if (errorMsg)
            LogStream->getCerr() << "Error";
        else
            LogStream->getCerr() << "Warning";
        LogStream->getCerr() << ": attribute " << DcmTag(key).getTagName() << " " << key
                             << " has other value than expected";
        if (filename != NULL)
            LogStream->getCerr() << " in file: " << filename;
        LogStream->getCerr() << endl;
        LogStream->unlockCerr();
    }
}


// print an error message to the console (stderr) that a required attribute is missing/empty
void DicomDirInterface::printRequiredAttributeMessage(const DcmTagKey &key,
                                                      const char *filename,
                                                      const OFBool emptyMsg)
{
    if (LogStream != NULL)
    {
        LogStream->lockCerr() << "Error: required attribute " << DcmTag(key).getTagName() << " " << key << " ";
        if (emptyMsg)
            LogStream->getCerr() << "empty";    /* "has no value"? */
        else
            LogStream->getCerr() << "missing";
        if (filename != NULL)
            LogStream->getCerr() << " in file: " << filename;
        LogStream->getCerr() << endl;
        LogStream->unlockCerr();
    }
}


// print an error message to the console (stderr) that something went wrong with an attribute
void DicomDirInterface::printAttributeErrorMessage(const DcmTagKey &key,
                                                   const OFCondition &error,
                                                   const char *operation)
{
    if ((LogStream != NULL) && (error.bad()))
    {
        LogStream->lockCerr() << "Error: " << error.text() << ": ";
        if (operation != NULL)
            LogStream->getCerr() << "cannot " << operation << " ";
        LogStream->getCerr() << DcmTag(key).getTagName() << " " << key << endl;
        LogStream->unlockCerr();
    }
}


// print an error message to the console (stderr) that something went wrong with a given record
void DicomDirInterface::printRecordErrorMessage(const OFCondition &error,
                                                const E_DirRecType recordType,
                                                const char *operation)
{
    if ((LogStream != NULL) && (error.bad()))
    {
        LogStream->lockCerr() << "Error: " << error.text() << ": ";
        if (operation != NULL)
            LogStream->getCerr() << "cannot " << operation << " ";
        LogStream->getCerr() << recordTypeToName(recordType) << " directory record" << endl;
        LogStream->unlockCerr();
    }
}


// print the given error status to the console (stderr)
void DicomDirInterface::printFileErrorMessage(const OFCondition &error,
                                              const char *operation,
                                              const char *filename)
{
    if ((LogStream != NULL) && error.bad())
    {
        LogStream->lockCerr() << "Error: " << error.text() << ": ";
        if (operation != NULL)
            LogStream->getCerr() << operation << " ";  /* "reading", "writing", etc. */
        if (filename != NULL)
            LogStream->getCerr() << "file: " << filename;
        LogStream->getCerr() << endl;
        LogStream->unlockCerr();
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
        case AP_GeneralPurposeDVD:
            result = "STD-GEN-DVD-JPEG/J2K";
            break;
        case AP_GeneralPurposeMIME:
            result = "STD-GEN-MIME";
            break;
        case AP_USBandFlash:
            result = "STD-GEN-USB/MMC/CF/SD-JPEG/J2K";
            break;
        case AP_MPEG2MPatML:
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
OFBool DicomDirInterface::copyFile(const char *fromFilename,
                                   const char *toFilename)
{
    OFBool result = OFFalse;
    /* check filenames first */
    if ((fromFilename != NULL) && (toFilename != NULL))
    {
        /* open input file */
        FILE *fromFile = fopen(fromFilename, "rb");
        if (fromFile != NULL)
        {
            /* create output file */
            FILE *toFile = fopen(toFilename, "wb");
            if (toFile != NULL)
            {
                result = OFTrue;
                int c = 0;
                /* for all input file characters */
                while (result && ((c = getc(fromFile)) != EOF))
                {
                    /* copy character to the output file */
                    if (putc(c, toFile) == EOF)
                    {
                        /* create error message */
                        OFOStringStream oss;
                        oss << "Error: copying files: " << fromFilename << " to "
                            << toFilename << OFStringStream_ends;
                        OFSTRINGSTREAM_GETSTR(oss, tmpString)
                        printErrorMessage(tmpString);
                        OFSTRINGSTREAM_FREESTR(tmpString)
                        /* abort loop */
                        result = OFFalse;
                    }
                }
                /* close output file */
                fclose(toFile);
            } else
                printErrorMessage("Error: copying files, cannot create: ", toFilename);
            /* close input file */
            fclose(fromFile);
        } else
            printErrorMessage("Error: copying files, cannot open: ", fromFilename);
    }
    return result;
}


// see if all the attributes in record match the values in dataset
OFBool DicomDirInterface::warnAboutInconsistentAttributes(DcmDirectoryRecord *record,
                                                          DcmItem *dataset,
                                                          const OFString &sourceFilename,
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
                        /* do not check ContentSequence (see addConceptModContentItems()) */
                        if (delem->getTag() != DCM_ContentSequence)
                            result &= compareSequenceAttributes(dataset, tag, record, sourceFilename);
                    } else {
                        /* everything else can be compared as a string */
                        result &= compareStringAttributes(dataset, tag, record, sourceFilename);
                    }
                }
            }
            first = OFFalse;
        }
    }
    if (!result & abortCheck)
        printErrorMessage("aborting on first inconsistent file: ", sourceFilename.c_str());
    /* return OFTrue in case of any inconsistency */
    return !result;
}


// check whether given fileset ID is valid
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
            OFOStringStream oss;
            oss << "invalid character(s) in fileset ID: " << filesetID << endl;
            oss << OFString(7 /*Error: */ + 36 /*message*/ + invalidChar, ' ') << "^" << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            printErrorMessage(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
            result = OFFalse;
        }
        /* ensure that fileset ID is not too large */
        if (isComponentTooLarge(filesetID, OFstatic_cast(size_t, DcmVR(EVR_CS).getMaxValueLength()), MapFilenamesMode))
        {
            printErrorMessage("fileset ID too large: ", filesetID.c_str());
            result = OFFalse;
        }
    } else
        result = OFFalse;
    return result;
}


// check whether specified tag exists in the dataset
OFBool DicomDirInterface::checkExists(DcmItem *dataset,
                                      const DcmTagKey &key,
                                      const char *filename)
{
    /* check whether tag exists */
    OFBool result = OFFalse;
    if (dataset != NULL)
    {
        result = dataset->tagExists(key);
        if (!result && (filename != NULL))
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
                                                  const char *filename)
{
    /* first, check whether tag exists, and report an error if not */
    OFBool result = checkExists(dataset, key, filename);
    if (result)
    {
        /* then check whether tag has a value (is non-empty) */
        result = dataset->tagExistsWithValue(key);
        if (!result && (filename != NULL))
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
                                                     const char *filename)
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
        if (!result && (filename != NULL))
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
                                                      const char *filename,
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
            if (filename != NULL)
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
                                                     const char *filename,
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
            if (filename != NULL)
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


// get string value from file and report an error (if any)
OFString &DicomDirInterface::getStringFromFile(const char *filename,
                                               const DcmTagKey &key,
                                               OFString &result,
                                               OFBool searchIntoSub)
{
    result.clear();
    if (filename != NULL)
    {
        DcmFileFormat fileformat;
        if (VerboseMode)
            printMessage("investigating file: ", filename);
        /* load specified file */
        OFCondition status = fileformat.loadFile(filename);
        /* retrieve string value from dataset */
        if (status.good())
            getStringFromDataset(fileformat.getDataset(), key, result, searchIntoSub);
        else
            printFileErrorMessage(status, "reading", filename);
    }
    return result;
}


// copy element from dataset to directory record
void DicomDirInterface::copyElement(DcmItem *dataset,
                                    const DcmTagKey &key,
                                    DcmDirectoryRecord *record,
                                    const OFBool optional,
                                    const OFBool copyEmpty)
{
    if ((dataset != NULL) && (record != NULL))
    {
        /* check whether tag exists in source dataset (if optional) */
        if (!optional || (copyEmpty && dataset->tagExists(key)) || dataset->tagExistsWithValue(key))
        {
            DcmElement *delem = NULL;
            /* create copy of element from source dataset */
            OFCondition status = dataset->findAndCopyElement(key, delem);
            if (status.good())
            {
                /* ... and insert it into the destination dataset (record) */
                status = record->insert(delem, OFTrue /*replaceOld*/);
            } else if (status == EC_TagNotFound)
                status = record->insertEmptyElement(key);
            printAttributeErrorMessage(key, status, "insert");
        }
    }
}


// copy optional string value from dataset to directory record
void DicomDirInterface::copyStringWithDefault(DcmItem *dataset,
                                              const DcmTagKey &key,
                                              DcmDirectoryRecord *record,
                                              const char *defaultValue,
                                              const OFBool printWarning)
{
    if ((dataset != NULL) && (record != NULL))
    {
        OFCondition status;
        if (dataset->tagExistsWithValue(key))
        {
            OFString stringValue;
            /* retrieve string value from source dataset and put it into the destination dataset */
            status = record->putAndInsertString(key, getStringFromDataset(dataset, key, stringValue).c_str());
        } else {
            if (printWarning && (defaultValue != NULL))
            {
                /* create warning message */
                OFOStringStream oss;
                oss << "file " << record->getRecordsOriginFile() << ": " << DcmTag(key).getTagName()
                    << " missing, using alternative: " << defaultValue << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                printWarningMessage(tmpString);
                OFSTRINGSTREAM_FREESTR(tmpString)
            }
            /* put default value */
            status = record->putAndInsertString(key, defaultValue);
        }
        printAttributeErrorMessage(key, status, "insert");
    }
}


// compare string attributes from dataset and record and report any deviation
OFBool DicomDirInterface::compareStringAttributes(DcmItem *dataset,
                                                  DcmTagKey &key,
                                                  DcmDirectoryRecord *record,
                                                  const OFString &sourceFilename)
{
    OFBool result = OFFalse;
    /* check parameters first */
    if ((dataset != NULL) && (record != NULL))
    {
        OFString datasetString, recordString;
        /* compare string value from dataset and record */
        result = compare(getStringFromDataset(dataset, key, datasetString),
                         getStringFromDataset(record, key, recordString));
        if (!result)
        {
            OFString uniqueString;
            DcmTagKey uniqueKey = getRecordUniqueKey(record->getRecordType());
            getStringFromDataset(record, uniqueKey, uniqueString);
            /* create warning message */
            OFOStringStream oss;
            oss << "file inconsistent with existing DICOMDIR record" << endl;
            oss << "  " << recordTypeToName(record->getRecordType()) << " Record [Key: "
                << DcmTag(uniqueKey).getTagName() << " " << uniqueKey << "=\"" << uniqueString << "\"]" << endl;
            oss << "    Existing Record (origin: " << record->getRecordsOriginFile() << ") defines: " << endl;
            oss << "      " << DcmTag(key).getTagName() << " " << key << "=\"" << recordString << "\"" << endl;
            oss << "    File (" << sourceFilename << ") defines:" << endl;
            oss << "      " << DcmTag(key).getTagName() << " " << key << "=\"" << datasetString << "\"" << endl;
            oss << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            printWarningMessage(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
        }
    }
    return result;
}


// compare sequence attributes from dataset and record and report any deviation
OFBool DicomDirInterface::compareSequenceAttributes(DcmItem *dataset,
                                                    DcmTagKey &key,
                                                    DcmDirectoryRecord *record,
                                                    const OFString &sourceFilename)
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
                DcmTagKey uniqueKey = getRecordUniqueKey(record->getRecordType());
                getStringFromDataset(record, uniqueKey, uniqueString);
                /* create warning message */
                OFOStringStream oss;
                oss << "file inconsistent with existing DICOMDIR record" << endl;
                oss << "  " << recordTypeToName(record->getRecordType()) << " Record [Key: "
                    << DcmTag(uniqueKey).getTagName() << " " << uniqueKey << "=\"" << uniqueString << "\"]" << endl;
                oss << "    Reason: " << reason << endl;
                oss << "    Existing Record (origin: " << record->getRecordsOriginFile() << ") defines: " << endl;
                seq1->print(oss, 0, 4 /*indent*/);
                oss << "    File (" << sourceFilename << ") defines:" << endl;
                seq2->print(oss, 0, 4 /*indent*/);
                oss << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                printWarningMessage(tmpString);
                OFSTRINGSTREAM_FREESTR(tmpString)
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
        OFOStringStream oss;
        oss << recordTypeToName(record->getRecordType()) << " Record (origin: "
            << record->getRecordsOriginFile() << ") inventing " << DcmTag(key).getTagName()
            << ": " << buffer << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printWarningMessage(tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
    }
}


/*
 *  CVS/RCS Log:
 *  $Log: dcddirif.cc,v $
 *  Revision 1.17  2006-07-27 13:09:25  joergr
 *  Added support for DICOMDIR record type "STEREOMETRIC" (CP 628).
 *  Added full support for X-Ray Radiation Dose SR documents and Real World Value
 *  Mapping objects to DICOMDIR. Renamed ERT_StructReport to ERT_SRDocument.
 *
 *  Revision 1.16  2005/12/15 15:40:18  joergr
 *  Removed unsused parameter.
 *
 *  Revision 1.15  2005/12/08 15:41:00  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.14  2005/10/27 13:40:06  joergr
 *  Added support for Encapsulated Document, Real World Value Mapping and
 *  Hanging Protocol objects to DICOMDIR tools.
 *  Added support for new Color Presentation State objects to DICOMDIR tools.
 *  Minor code changes, e.g. reworked handling of type 2 attributes.
 *
 *  Revision 1.13  2005/10/25 08:55:34  meichel
 *  Updated list of UIDs and added support for new transfer syntaxes
 *    and storage SOP classes.
 *
 *  Revision 1.12  2005/06/13 14:46:05  joergr
 *  Fixed typo.
 *
 *  Revision 1.11  2005/06/13 14:39:07  joergr
 *  Added new options to disable check on pixel encoding and transfer syntax.
 *  Fixed bug: Images with non-standard spatial resolution were rejected even
 *  if "Resolution Check" was disabled.
 *
 *  Revision 1.10  2005/03/09 17:54:54  joergr
 *  Added support for new Media Storage Application Profiles according to DICOM
 *  PS 3.12-2004. Removed support for non-standard conformant "No profile".
 *  Added support for UTF-8 for the contents of the fileset descriptor file.
 *
 *  Revision 1.9  2004/04/16 12:55:53  joergr
 *  Minor modifications to keep Sun CC 2.0.1 happy.
 *
 *  Revision 1.8  2004/04/14 11:52:57  joergr
 *  Changed type of integer variable, added explicit type cast and introduced
 *  default case to switch statement to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.7  2004/04/06 18:04:30  joergr
 *  Added missing suffix "TransferSyntax" to some transfer syntax constants.
 *
 *  Revision 1.6  2004/02/13 17:36:54  joergr
 *  Added support for new directory records RAW DATA and SPECTROSCOPY introduced
 *  with CP 343.
 *
 *  Revision 1.5  2004/02/13 14:16:41  joergr
 *  Added support for new directory records REGISTRATION and FIDUCIAL introduced
 *  with supplement 73 (Spatial Registration Storage SOP Classes).
 *  Added support for Procedure Log Storage SOP class (supplement 66).
 *
 *  Revision 1.4  2004/02/13 11:48:13  joergr
 *  Adapted code for changed tag names (e.g. PresentationLabel -> ContentLabel).
 *
 *  Revision 1.3  2003/11/10 10:39:49  joergr
 *  Enhanced detection of invalid filenames: forbid leading path separator, i.e.
 *  absolute pathnames.
 *
 *  Revision 1.2  2003/11/05 18:32:23  joergr
 *  Fixed bug that prevented the addition of DICOM files missing the "sort key"
 *  (e.g. InstanceNumber).
 *
 *  Revision 1.1  2003/08/12 14:37:36  joergr
 *  Added new interface class for simplified creation of a DICOMDIR.
 *
 *
 */
