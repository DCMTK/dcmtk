/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRDocument
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmsr/dsrxmld.h"
#include "dcmtk/dcmsr/dsrpnmtn.h"
#include "dcmtk/dcmsr/dsrdattn.h"
#include "dcmtk/dcmsr/dsrdtitn.h"
#include "dcmtk/dcmsr/dsrtimtn.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcvrdt.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define DCMSR_PRINT_HEADER_FIELD_START(header_name, delimiter)         \
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_HEADER_NAME)   \
    stream << header_name;                                             \
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)     \
    stream << delimiter;                                               \
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_HEADER_VALUE)

#define DCMSR_PRINT_HEADER_FIELD_END                                   \
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RESET)         \
    stream << OFendl;


/*------------------*
 *  implementation  *
 *------------------*/

DSRDocument::DSRDocument(const E_DocumentType documentType)
  : DocumentTree(documentType),
    FinalizedFlag(OFFalse),
    PreliminaryFlagEnum(PF_invalid),
    CompletionFlagEnum(CF_invalid),
    VerificationFlagEnum(VF_invalid),
    SpecificCharacterSetEnum(CS_default),
    SOPClassUID(DCM_SOPClassUID),
    SOPInstanceUID(DCM_SOPInstanceUID),
    SpecificCharacterSet(DCM_SpecificCharacterSet),
    InstanceCreationDate(DCM_InstanceCreationDate),
    InstanceCreationTime(DCM_InstanceCreationTime),
    InstanceCreatorUID(DCM_InstanceCreatorUID),
    CodingSchemeIdentification(),
    TimezoneOffsetFromUTC(DCM_TimezoneOffsetFromUTC),
    StudyInstanceUID(DCM_StudyInstanceUID),
    StudyDate(DCM_StudyDate),
    StudyTime(DCM_StudyTime),
    ReferringPhysicianName(DCM_ReferringPhysicianName),
    StudyID(DCM_StudyID),
    AccessionNumber(DCM_AccessionNumber),
    StudyDescription(DCM_StudyDescription),
    PatientName(DCM_PatientName),
    PatientID(DCM_PatientID),
    IssuerOfPatientID(DCM_IssuerOfPatientID),
    PatientBirthDate(DCM_PatientBirthDate),
    PatientSex(DCM_PatientSex),
    Manufacturer(DCM_Manufacturer),
    ManufacturerModelName(DCM_ManufacturerModelName),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    SoftwareVersions(DCM_SoftwareVersions),
    SynchronizationFrameOfReferenceUID(DCM_SynchronizationFrameOfReferenceUID),
    SynchronizationTrigger(DCM_SynchronizationTrigger),
    AcquisitionTimeSynchronized(DCM_AcquisitionTimeSynchronized),
    Modality(DCM_Modality),
    SeriesInstanceUID(DCM_SeriesInstanceUID),
    SeriesNumber(DCM_SeriesNumber),
    SeriesDate(DCM_SeriesDate),
    SeriesTime(DCM_SeriesTime),
    ProtocolName(DCM_ProtocolName),
    SeriesDescription(DCM_SeriesDescription),
    ReferencedPerformedProcedureStep(DCM_ReferencedPerformedProcedureStepSequence),
    InstanceNumber(DCM_InstanceNumber),
    PreliminaryFlag(DCM_PreliminaryFlag),
    CompletionFlag(DCM_CompletionFlag),
    CompletionFlagDescription(DCM_CompletionFlagDescription),
    VerificationFlag(DCM_VerificationFlag),
    ContentDate(DCM_ContentDate),
    ContentTime(DCM_ContentTime),
    VerifyingObserver(DCM_VerifyingObserverSequence),
    PredecessorDocuments(DCM_PredecessorDocumentsSequence),
    IdenticalDocuments(DCM_IdenticalDocumentsSequence),
    PerformedProcedureCode(DCM_PerformedProcedureCodeSequence),
    CurrentRequestedProcedureEvidence(DCM_CurrentRequestedProcedureEvidenceSequence),
    PertinentOtherEvidence(DCM_PertinentOtherEvidenceSequence),
    ReferencedInstances()
{
    DCMSR_DEBUG("Initializing all DICOM header attributes");
    /* set initial values for a new SOP instance */
    updateAttributes(OFTrue /*updateAll*/, OFFalse /*verboseMode*/);
}


DSRDocument::~DSRDocument()
{
}


void DSRDocument::clear()
{
    /* clear SR document tree */
    DocumentTree.clear();
    FinalizedFlag = OFFalse;
    /* clear enumerated values */
    PreliminaryFlagEnum = PF_invalid;
    CompletionFlagEnum = CF_invalid;
    VerificationFlagEnum = VF_invalid;
    SpecificCharacterSetEnum = CS_default;
    /* clear all DICOM attributes */
    SOPClassUID.clear();
    SOPInstanceUID.clear();
    SpecificCharacterSet.clear();
    InstanceCreationDate.clear();
    InstanceCreationTime.clear();
    InstanceCreatorUID.clear();
    CodingSchemeIdentification.clear();
    TimezoneOffsetFromUTC.clear();
    StudyInstanceUID.clear();
    StudyDate.clear();
    StudyTime.clear();
    ReferringPhysicianName.clear();
    StudyID.clear();
    AccessionNumber.clear();
    StudyDescription.clear();
    PatientName.clear();
    PatientID.clear();
    IssuerOfPatientID.clear();
    PatientBirthDate.clear();
    PatientSex.clear();
    Manufacturer.clear();
    ManufacturerModelName.clear();
    DeviceSerialNumber.clear();
    SoftwareVersions.clear();
    SynchronizationFrameOfReferenceUID.clear();
    SynchronizationTrigger.clear();
    AcquisitionTimeSynchronized.clear();
    Modality.clear();
    SeriesInstanceUID.clear();
    SeriesNumber.clear();
    SeriesDate.clear();
    SeriesTime.clear();
    ProtocolName.clear();
    SeriesDescription.clear();
    ReferencedPerformedProcedureStep.clear();
    InstanceNumber.clear();
    PreliminaryFlag.clear();
    CompletionFlag.clear();
    CompletionFlagDescription.clear();
    VerificationFlag.clear();
    ContentDate.clear();
    ContentTime.clear();
    VerifyingObserver.clear();
    PerformedProcedureCode.clear();
    /* clear list structures */
    PredecessorDocuments.clear();
    IdenticalDocuments.clear();
    CurrentRequestedProcedureEvidence.clear();
    PertinentOtherEvidence.clear();
    ReferencedInstances.clear();
}


OFBool DSRDocument::isValid()
{
    /* document is valid if the document tree is valid and ... */
    return DocumentTree.isValid() && !SOPClassUID.isEmpty() && !SOPInstanceUID.isEmpty();
}


OFBool DSRDocument::isFinalized() const
{
    return FinalizedFlag;
}


OFCondition DSRDocument::print(STD_NAMESPACE ostream &stream,
                               const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (isValid())
    {
        OFString tmpString, string2;
        /* update only some DICOM attributes */
        updateAttributes(OFFalse /*updateAll*/);

        // --- print some general document information ---

        if (!(flags & PF_printNoDocumentHeader))
        {
            /* document type/title */
            DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DOCUMENT_TYPE)
            stream << documentTypeToDocumentTitle(getDocumentType(), tmpString);
            DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RESET)
            stream << OFendl << OFendl;
            /* patient related information */
            if (!PatientName.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Patient            ", " : ")
                stream << getPrintStringFromElement(PatientName, tmpString);
                OFString patientStr;
                if (!PatientSex.isEmpty())
                    patientStr += getPrintStringFromElement(PatientSex, tmpString);
                if (!PatientBirthDate.isEmpty())
                {
                    if (!patientStr.empty())
                        patientStr += ", ";
                    patientStr += dicomToReadableDate(getStringValueFromElement(PatientBirthDate, tmpString), string2);
                }
                if (!PatientID.isEmpty())
                {
                    if (!patientStr.empty())
                        patientStr += ", ";
                    patientStr += '#';
                    patientStr += getPrintStringFromElement(PatientID, tmpString);
                    if (!IssuerOfPatientID.isEmpty())
                    {
                        patientStr += ":";
                        patientStr += getPrintStringFromElement(IssuerOfPatientID, tmpString);
                    }
                }
                if (!patientStr.empty())
                    stream << " (" << patientStr << ")";
                DCMSR_PRINT_HEADER_FIELD_END
            }
            /* referring physician */
            if (!ReferringPhysicianName.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Referring Physician", " : ")
                stream << getPrintStringFromElement(ReferringPhysicianName, tmpString);
                DCMSR_PRINT_HEADER_FIELD_END
            }
            /* study-related information */
            if (!StudyDescription.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Study              ", " : ")
                stream << getPrintStringFromElement(StudyDescription, tmpString);
                if (!StudyID.isEmpty())
                    stream << " (#" << getPrintStringFromElement(StudyID, tmpString) << ")";
                DCMSR_PRINT_HEADER_FIELD_END
            }
            /* series-related information */
            if (!SeriesDescription.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Series             ", " : ")
                stream << getPrintStringFromElement(SeriesDescription, tmpString);
                if (!SeriesNumber.isEmpty())
                    stream << " (#" << getPrintStringFromElement(SeriesNumber, tmpString) << ")";
                DCMSR_PRINT_HEADER_FIELD_END
            }
            /* protocol name */
            if (!ProtocolName.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Protocol           ", " : ")
                stream << getPrintStringFromElement(ProtocolName, tmpString);
                DCMSR_PRINT_HEADER_FIELD_END
            }
            /* manufacturer and device */
            if (!Manufacturer.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Manufacturer       ", " : ")
                stream << getPrintStringFromElement(Manufacturer, tmpString);
                OFString deviceStr;
                if (!ManufacturerModelName.isEmpty())
                    deviceStr += getPrintStringFromElement(ManufacturerModelName, tmpString);
                if (!DeviceSerialNumber.isEmpty())
                {
                    if (!deviceStr.empty())
                        deviceStr += ", ";
                    deviceStr += '#';
                    deviceStr += getPrintStringFromElement(DeviceSerialNumber, tmpString);
                }
                if (!deviceStr.empty())
                    stream << " (" << deviceStr << ")";
                DCMSR_PRINT_HEADER_FIELD_END
            }
            /* Key Object Selection Documents do not contain the SR Document General Module */
            if (getDocumentType() != DT_KeyObjectSelectionDocument)
            {
                /* preliminary flag */
                if (!PreliminaryFlag.isEmpty())
                {
                    DCMSR_PRINT_HEADER_FIELD_START("Preliminary Flag   ", " : ")
                    stream << getStringValueFromElement(PreliminaryFlag, tmpString);
                    DCMSR_PRINT_HEADER_FIELD_END
                }
                /* completion flag */
                DCMSR_PRINT_HEADER_FIELD_START("Completion Flag    ", " : ")
                stream << getStringValueFromElement(CompletionFlag, tmpString);
                DCMSR_PRINT_HEADER_FIELD_END
                if (!CompletionFlagDescription.isEmpty())
                {
                    DCMSR_PRINT_HEADER_FIELD_START("                   ", "   ")
                    stream << getPrintStringFromElement(CompletionFlagDescription, tmpString);
                    DCMSR_PRINT_HEADER_FIELD_END
                }
                /* predecessor documents */
                if (!PredecessorDocuments.isEmpty())
                {
                    DCMSR_PRINT_HEADER_FIELD_START("Predecessor Docs   ", " : ")
                    stream << PredecessorDocuments.getNumberOfInstances();
                    DCMSR_PRINT_HEADER_FIELD_END
                }
            }
            /* identical documents */
            if (!IdenticalDocuments.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Identical Docs     ", " : ")
                stream << IdenticalDocuments.getNumberOfInstances();
                DCMSR_PRINT_HEADER_FIELD_END
            }
            /* referenced instances */
            if (!ReferencedInstances.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("References Objects ", " : ")
                stream << ReferencedInstances.getNumberOfItems();
                DCMSR_PRINT_HEADER_FIELD_END
            }
            if (getDocumentType() != DT_KeyObjectSelectionDocument)
            {
                /* verification flag */
                DCMSR_PRINT_HEADER_FIELD_START("Verification Flag  ", " : ")
                stream << getStringValueFromElement(VerificationFlag, tmpString);
                DCMSR_PRINT_HEADER_FIELD_END
                /* verifying observer */
                const size_t obsCount = getNumberOfVerifyingObservers();
                for (size_t i = 1; i <= obsCount; i++)
                {
                    OFString dateTime, obsName, organization;
                    DSRCodedEntryValue obsCode;
                    if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization).good())
                    {
                        if (i == 1)
                        {
                            DCMSR_PRINT_HEADER_FIELD_START("Verifying Observers", " : ")
                        } else {
                            DCMSR_PRINT_HEADER_FIELD_START("                   ", "   ")
                        }
                        stream << dicomToReadableDateTime(dateTime, tmpString) << ", " << obsName;
                        if (obsCode.isValid() || (flags & PF_printInvalidCodes))
                        {
                            stream << " ";
                            obsCode.print(stream, (flags & PF_printAllCodes) > 0 /*printCodeValue*/, flags);
                        }
                        stream << ", " << organization;
                        DCMSR_PRINT_HEADER_FIELD_END
                    }
                }
            }
            /* content date and time */
            if (!ContentDate.isEmpty() && !ContentTime.isEmpty())
            {
                DCMSR_PRINT_HEADER_FIELD_START("Content Date/Time  ", " : ")
                stream << dicomToReadableDate(getStringValueFromElement(ContentDate, tmpString), string2) << " ";
                stream << dicomToReadableTime(getStringValueFromElement(ContentTime, tmpString), string2);
                DCMSR_PRINT_HEADER_FIELD_END
            }
            stream << OFendl;
        }

        // --- dump document tree to stream ---
        result = DocumentTree.print(stream, flags);
    }
    return result;
}


OFCondition DSRDocument::checkDatasetForReading(DcmItem &dataset,
                                                E_DocumentType &documentType)
{
    OFCondition result = EC_Normal;
    OFString tmpString;
    DcmUniqueIdentifier sopClassUID(DCM_SOPClassUID);
    DcmCodeString modality(DCM_Modality);
    /* check SOP class UID */
    result = getAndCheckElementFromDataset(dataset, sopClassUID, "1", "1", "SOPCommonModule");
    if (result.good())
    {
        documentType = sopClassUIDToDocumentType(getStringValueFromElement(sopClassUID, tmpString));
        DCMSR_DEBUG("Value of SOP Class UID: " << tmpString);
        if (documentType == DT_invalid)
        {
            DCMSR_ERROR("SOP Class UID does not match one of the known SR document classes");
            result = SR_EC_UnknownDocumentType;
        }
        else if (!isDocumentTypeSupported(documentType))
        {
            DCMSR_ERROR("Unsupported SOP Class UID (not yet implemented)");
            result = SR_EC_UnsupportedValue;
        }
    } else {
        /* no SOP Class UID means no document type */
        documentType = DT_invalid;
    }
    /* check modality */
    if (result.good())
    {
        if (documentType == DT_KeyObjectSelectionDocument)
            result = getAndCheckElementFromDataset(dataset, modality, "1", "1", "KeyObjectDocumentSeriesModule");
        else
            result = getAndCheckElementFromDataset(dataset, modality, "1", "1", "SRDocumentSeriesModule");
        if (result.good())
        {
            if (getStringValueFromElement(modality, tmpString) != documentTypeToModality(documentType))
            {
                DCMSR_ERROR("Modality does not match '" << documentTypeToModality(documentType) << "' for "
                    << documentTypeToReadableName(documentType));
            }
        }
    }
    return result;
}


OFCondition DSRDocument::read(DcmItem &dataset,
                              const size_t flags)
{
    OFCondition result = EC_Normal;
    E_DocumentType documentType = DT_invalid;
    DCMSR_DEBUG("Reading SR document from DICOM dataset");
    /* re-initialize SR document */
    clear();
    /* check SOP class UID and modality first */
    result = checkDatasetForReading(dataset, documentType);
    /* dataset is OK */
    if (result.good())
    {
        OFString tmpString;
        OFCondition searchCond = EC_Normal;
        OFCondition obsSearchCond = EC_Normal;

        /* type 3 element and attributes which have already been checked are not checked */

        // --- SOP Common Module ---
        getElementFromDataset(dataset, SOPClassUID);   /* already checked */
        getAndCheckElementFromDataset(dataset, SOPInstanceUID, "1", "1", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SpecificCharacterSet, "1-n", "1C", "SOPCommonModule");
        getStringValueFromElement(SpecificCharacterSet, tmpString, -1 /* all components */);
        /* currently, the VR checker in 'dcmdata' only supports ASCII and Latin-1 */
        if (!tmpString.empty() && (tmpString != "ISO_IR 6") && (tmpString != "ISO_IR 100"))
            DCMSR_WARN("The VR checker does not support this Specific Character Set: " << tmpString);
        getAndCheckElementFromDataset(dataset, InstanceCreationDate, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreationTime, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreatorUID, "1", "3", "SOPCommonModule");
        CodingSchemeIdentification.read(dataset, flags);
        if (requiresTimezoneModule(documentType))
        {
            // --- Timezone Module ---
            getAndCheckElementFromDataset(dataset, TimezoneOffsetFromUTC, "1", "1", "TimezoneModule");
        } else {
            // --- SOP Common Module ---
            getAndCheckElementFromDataset(dataset, TimezoneOffsetFromUTC, "1", "3", "SOPCommonModule");
        }

        // --- General Study and Patient Module ---
        readStudyData(dataset, flags);

        if (requiresEnhancedEquipmentModule(documentType))
        {
            // --- Enhanced General Equipment Module ---
            getAndCheckElementFromDataset(dataset, Manufacturer, "1", "1", "EnhancedGeneralEquipmentModule");
            getAndCheckElementFromDataset(dataset, ManufacturerModelName, "1", "1", "EnhancedGeneralEquipmentModule");
            getAndCheckElementFromDataset(dataset, DeviceSerialNumber, "1", "1", "EnhancedGeneralEquipmentModule");
            getAndCheckElementFromDataset(dataset, SoftwareVersions, "1-n", "1", "EnhancedGeneralEquipmentModule");
        } else {
            // --- General Equipment Module ---
            getAndCheckElementFromDataset(dataset, Manufacturer, "1", "2", "GeneralEquipmentModule");
            getAndCheckElementFromDataset(dataset, ManufacturerModelName, "1", "3", "GeneralEquipmentModule");
            getAndCheckElementFromDataset(dataset, DeviceSerialNumber, "1", "3", "GeneralEquipmentModule");
            getAndCheckElementFromDataset(dataset, SoftwareVersions, "1-n", "3", "GeneralEquipmentModule");
        }

        // --- Synchronization Module ---
        if (requiresSynchronizationModule(documentType) /* either the IOD requires this module */ ||
            dataset.tagExistsWithValue(DCM_SynchronizationFrameOfReferenceUID) || dataset.tagExistsWithValue(DCM_SynchronizationTrigger) ||
            dataset.tagExistsWithValue(DCM_AcquisitionTimeSynchronized) /* or all attributes should be absent */ )
        {
            getAndCheckElementFromDataset(dataset, SynchronizationFrameOfReferenceUID, "1", "1", "SynchronizationModule");
            getAndCheckElementFromDataset(dataset, SynchronizationTrigger, "1", "1", "SynchronizationModule");
            getAndCheckElementFromDataset(dataset, AcquisitionTimeSynchronized, "1", "1", "SynchronizationModule");
        }

        // --- SR Document Series Module / Key Object Document Series Module ---
        getElementFromDataset(dataset, Modality);   /* already checked */
        if (documentType == DT_KeyObjectSelectionDocument)
        {
            getAndCheckElementFromDataset(dataset, SeriesInstanceUID, "1", "1", "KeyObjectDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesNumber, "1", "1", "KeyObjectDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesDate, "1", "3", "KeyObjectDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesTime, "1", "3", "KeyObjectDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, ProtocolName, "1", "3", "KeyObjectDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesDescription, "1", "3", "KeyObjectDocumentSeriesModule");
            /* need to check sequence in two steps (avoids additional getAndCheck... method) */
            searchCond = getElementFromDataset(dataset, ReferencedPerformedProcedureStep);
            checkElementValue(ReferencedPerformedProcedureStep, "1", "2", searchCond, "KeyObjectDocumentSeriesModule");
        } else {
            getAndCheckElementFromDataset(dataset, SeriesInstanceUID, "1", "1", "SRDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesNumber, "1", "1", "SRDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesDate, "1", "3", "SRDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesTime, "1", "3", "SRDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, ProtocolName, "1", "3", "SRDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesDescription, "1", "3", "SRDocumentSeriesModule");
            /* need to check sequence in two steps (avoids additional getAndCheck... method) */
            searchCond = getElementFromDataset(dataset, ReferencedPerformedProcedureStep);
            checkElementValue(ReferencedPerformedProcedureStep, "1", "2", searchCond, "SRDocumentSeriesModule");
        }
        /* remove possible signature sequences */
        removeAttributeFromSequence(ReferencedPerformedProcedureStep, DCM_MACParametersSequence);
        removeAttributeFromSequence(ReferencedPerformedProcedureStep, DCM_DigitalSignaturesSequence);

        // --- SR Document General Module / Key Object Document Module ---
        if (documentType == DT_KeyObjectSelectionDocument)
        {
            getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "1", "KeyObjectDocumentModule");
            getAndCheckElementFromDataset(dataset, ContentDate, "1", "1", "KeyObjectDocumentModule");
            getAndCheckElementFromDataset(dataset, ContentTime, "1", "1", "KeyObjectDocumentModule");
        } else {
            getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "1", "SRDocumentGeneralModule");
            getAndCheckElementFromDataset(dataset, ContentDate, "1", "1", "SRDocumentGeneralModule");
            getAndCheckElementFromDataset(dataset, ContentTime, "1", "1", "SRDocumentGeneralModule");
            getAndCheckElementFromDataset(dataset, PreliminaryFlag, "1", "3", "SRDocumentGeneralModule");
            getAndCheckElementFromDataset(dataset, CompletionFlag, "1", "1", "SRDocumentGeneralModule");
            getAndCheckElementFromDataset(dataset, CompletionFlagDescription, "1", "3", "SRDocumentGeneralModule");
            getAndCheckElementFromDataset(dataset, VerificationFlag, "1", "1", "SRDocumentGeneralModule");
            obsSearchCond = getElementFromDataset(dataset, VerifyingObserver);
            PredecessorDocuments.read(dataset, flags);
            /* need to check sequence in two steps (avoids additional getAndCheck... method) */
            searchCond = getElementFromDataset(dataset, PerformedProcedureCode);
            checkElementValue(PerformedProcedureCode, "1-n", "2", searchCond, "SRDocumentGeneralModule");
            PertinentOtherEvidence.read(dataset, flags);
            ReferencedInstances.read(dataset, flags);
        }
        IdenticalDocuments.read(dataset, flags);
        CurrentRequestedProcedureEvidence.read(dataset, flags);
        /* remove possible signature sequences */
        removeAttributeFromSequence(VerifyingObserver, DCM_MACParametersSequence);
        removeAttributeFromSequence(VerifyingObserver, DCM_DigitalSignaturesSequence);
        removeAttributeFromSequence(PerformedProcedureCode, DCM_MACParametersSequence);
        removeAttributeFromSequence(PerformedProcedureCode, DCM_DigitalSignaturesSequence);

        /* update internal enumerated values and perform additional checks */

        /* Key Object Selection Documents do not contain the SR Document General Module */
        if (documentType != DT_KeyObjectSelectionDocument)
        {
            /* get and check PreliminaryFlag (if present) */
            if (!PreliminaryFlag.isEmpty())
            {
                PreliminaryFlagEnum = enumeratedValueToPreliminaryFlag(getStringValueFromElement(PreliminaryFlag, tmpString));
                if (PreliminaryFlagEnum == PF_invalid)
                    printUnknownValueWarningMessage("PreliminaryFlag", tmpString.c_str());
            }
            /* get and check CompletionFlag */
            CompletionFlagEnum = enumeratedValueToCompletionFlag(getStringValueFromElement(CompletionFlag, tmpString));
            if (CompletionFlagEnum == CF_invalid)
                printUnknownValueWarningMessage("CompletionFlag", tmpString.c_str());
            else if ((CompletionFlagEnum == CF_Partial) && (documentType == DT_XRayRadiationDoseSR))
                DCMSR_WARN("Invalid value for Completion Flag, should be 'COMPLETE' for X-Ray Radiation Dose SR");
            /* get and check VerificationFlag / VerifyingObserverSequence */
            VerificationFlagEnum = enumeratedValueToVerificationFlag(getStringValueFromElement(VerificationFlag, tmpString));
            if (VerificationFlagEnum == VF_invalid)
                printUnknownValueWarningMessage("VerificationFlag", tmpString.c_str());
            else if (VerificationFlagEnum == VF_Verified)
                checkElementValue(VerifyingObserver, "1-n", "1", obsSearchCond, "SRDocumentGeneralModule");
        }
        getStringValueFromElement(SpecificCharacterSet, tmpString, -1 /* all components */);
        SpecificCharacterSetEnum = definedTermToCharacterSet(tmpString);
        /* check SpecificCharacterSet */
        if ((SpecificCharacterSetEnum == CS_invalid) && !tmpString.empty())
            printUnknownValueWarningMessage("SpecificCharacterSet", tmpString.c_str());

        /* read SR document tree */
        if (result.good())
            result = DocumentTree.read(dataset, documentType, flags);
    }
    return result;
}


OFCondition DSRDocument::readPatientData(DcmItem &dataset,
                                         const size_t /*flags*/)
{
    // --- Patient Module ---
    getAndCheckElementFromDataset(dataset, PatientName, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientID, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, IssuerOfPatientID, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientBirthDate, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientSex, "1", "2", "PatientModule");
    /* always return success */
    return EC_Normal;
}


OFCondition DSRDocument::readStudyData(DcmItem &dataset,
                                       const size_t flags)
{
    // --- General Study Module ---
    getAndCheckElementFromDataset(dataset, StudyInstanceUID, "1", "1", "GeneralStudyModule");
    getAndCheckElementFromDataset(dataset, StudyDate, "1", "2", "GeneralStudyModule");
    getAndCheckElementFromDataset(dataset, StudyTime, "1", "2", "GeneralStudyModule");
    getAndCheckElementFromDataset(dataset, ReferringPhysicianName, "1", "2", "GeneralStudyModule");
    getAndCheckElementFromDataset(dataset, StudyID, "1", "2", "GeneralStudyModule");
    getAndCheckElementFromDataset(dataset, AccessionNumber, "1", "2", "GeneralStudyModule");
    getAndCheckElementFromDataset(dataset, StudyDescription, "1", "3", "GeneralStudyModule");
    /* also read data from Patient Module */
    return readPatientData(dataset, flags);
}


OFCondition DSRDocument::write(DcmItem &dataset,
                               DcmStack *markedItems)
{
    OFCondition result = EC_Normal;
    /* only write valid documents */
    if (isValid())
    {
        DCMSR_DEBUG("Writing SR document to DICOM dataset");
        /* update all DICOM attributes */
        updateAttributes();

        /* checking particular values */
        if ((CompletionFlagEnum == CF_Partial) && (getDocumentType() == DT_XRayRadiationDoseSR))
            DCMSR_WARN("Invalid value for Completion Flag, should be 'COMPLETE' for X-Ray Radiation Dose SR");

        /* write general document attributes */

        // --- SOP Common Module ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPClassUID), "1", "1", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPInstanceUID), "1", "1", "SOPCommonModule");
        /* never write specific character set for ASCII (default character repertoire) */
        if (SpecificCharacterSetEnum != CS_ASCII)
            addElementToDataset(result, dataset, new DcmCodeString(SpecificCharacterSet), "1-n", "1C", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmDate(InstanceCreationDate), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmTime(InstanceCreationTime), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(InstanceCreatorUID), "1", "3", "SOPCommonModule");
        CodingSchemeIdentification.write(dataset);
        if (requiresTimezoneModule(getDocumentType()))
        {
            // --- Timezone Module ---
            addElementToDataset(result, dataset, new DcmShortString(TimezoneOffsetFromUTC), "1", "1", "TimezoneModule");
        } else {
            // --- SOP Common Module ---
            addElementToDataset(result, dataset, new DcmShortString(TimezoneOffsetFromUTC), "1", "3", "SOPCommonModule");
        }

        // --- General Study Module ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(StudyInstanceUID), "1", "1", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmDate(StudyDate), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmTime(StudyTime), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(ReferringPhysicianName), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmShortString(StudyID), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmShortString(AccessionNumber), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmLongString(StudyDescription), "1", "3", "GeneralStudyModule");

        // --- Patient Module ---
        addElementToDataset(result, dataset, new DcmPersonName(PatientName), "1", "2", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(PatientID), "1", "2", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(IssuerOfPatientID), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmDate(PatientBirthDate), "1", "2", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientSex), "1", "2", "PatientModule");

        if (requiresEnhancedEquipmentModule(getDocumentType()))
        {
            // --- Enhanced General Equipment Module ---
            addElementToDataset(result, dataset, new DcmLongString(Manufacturer), "1", "1", "EnhancedGeneralEquipmentModule");
            addElementToDataset(result, dataset, new DcmLongString(ManufacturerModelName), "1", "1", "EnhancedGeneralEquipmentModule");
            addElementToDataset(result, dataset, new DcmLongString(DeviceSerialNumber), "1", "1", "EnhancedGeneralEquipmentModule");
            addElementToDataset(result, dataset, new DcmLongString(SoftwareVersions), "1-n", "1", "EnhancedGeneralEquipmentModule");
        } else {
            // --- General Equipment Module ---
            addElementToDataset(result, dataset, new DcmLongString(Manufacturer), "1", "2", "GeneralEquipmentModule");
            addElementToDataset(result, dataset, new DcmLongString(ManufacturerModelName), "1", "3", "GeneralEquipmentModule");
            addElementToDataset(result, dataset, new DcmLongString(DeviceSerialNumber), "1", "3", "GeneralEquipmentModule");
            addElementToDataset(result, dataset, new DcmLongString(SoftwareVersions), "1-n", "3", "GeneralEquipmentModule");
        }

        // --- Synchronization Module ---
        if (requiresSynchronizationModule(getDocumentType()) /* module required for some IODs */ ||
            !SynchronizationFrameOfReferenceUID.isEmpty() || !SynchronizationTrigger.isEmpty() || !AcquisitionTimeSynchronized.isEmpty())
        {
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(SynchronizationFrameOfReferenceUID), "1", "1", "SynchronizationModule");
            addElementToDataset(result, dataset, new DcmCodeString(SynchronizationTrigger), "1", "1", "SynchronizationModule");
            addElementToDataset(result, dataset, new DcmCodeString(AcquisitionTimeSynchronized), "1", "1", "SynchronizationModule");
        }

        // --- SR Document Series Module / Key Object Document Series Module ---
        if (getDocumentType() == DT_KeyObjectSelectionDocument)
        {
            addElementToDataset(result, dataset, new DcmCodeString(Modality), "1", "1", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(SeriesInstanceUID), "1", "1", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmIntegerString(SeriesNumber), "1", "1", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmDate(SeriesDate), "1", "3", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmTime(SeriesTime), "1", "3", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmLongString(ProtocolName), "1", "3", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmLongString(SeriesDescription), "1", "3", "KeyObjectDocumentSeriesModule");
            /* always write empty sequence since not yet fully supported */
            ReferencedPerformedProcedureStep.clear();
            addElementToDataset(result, dataset, new DcmSequenceOfItems(ReferencedPerformedProcedureStep), "1", "2", "KeyObjectDocumentSeriesModule");
        } else {
            addElementToDataset(result, dataset, new DcmCodeString(Modality), "1", "1", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(SeriesInstanceUID), "1", "1", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmIntegerString(SeriesNumber), "1", "1", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmDate(SeriesDate), "1", "3", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmTime(SeriesTime), "1", "3", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmLongString(ProtocolName), "1", "3", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmLongString(SeriesDescription), "1", "3", "SRDocumentSeriesModule");
            /* always write empty sequence since not yet fully supported */
            ReferencedPerformedProcedureStep.clear();
            addElementToDataset(result, dataset, new DcmSequenceOfItems(ReferencedPerformedProcedureStep), "1", "2", "SRDocumentSeriesModule");
        }

        // --- SR Document General Module / Key Object Document Module ---
        if (getDocumentType() == DT_KeyObjectSelectionDocument)
        {
            addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "1", "KeyObjectDocumentModule");
            addElementToDataset(result, dataset, new DcmDate(ContentDate), "1", "1", "KeyObjectDocumentModule");
            addElementToDataset(result, dataset, new DcmTime(ContentTime), "1", "1", "KeyObjectDocumentModule");
        } else {
            addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "1", "SRDocumentGeneralModule");
            addElementToDataset(result, dataset, new DcmDate(ContentDate), "1", "1", "SRDocumentGeneralModule");
            addElementToDataset(result, dataset, new DcmTime(ContentTime), "1", "1", "SRDocumentGeneralModule");
            addElementToDataset(result, dataset, new DcmCodeString(PreliminaryFlag), "1", "3", "SRDocumentGeneralModule");
            addElementToDataset(result, dataset, new DcmCodeString(CompletionFlag), "1", "1", "SRDocumentGeneralModule");
            addElementToDataset(result, dataset, new DcmLongString(CompletionFlagDescription), "1", "3", "SRDocumentGeneralModule");
            addElementToDataset(result, dataset, new DcmCodeString(VerificationFlag), "1", "1", "SRDocumentGeneralModule");
            if (VerificationFlagEnum == VF_Verified)
                addElementToDataset(result, dataset, new DcmSequenceOfItems(VerifyingObserver), "1-n", "1", "SRDocumentGeneralModule");
            if (result.good())
                PredecessorDocuments.write(dataset);    /* optional */
            /* always write empty sequence since not yet fully supported */
            PerformedProcedureCode.clear();
            addElementToDataset(result, dataset, new DcmSequenceOfItems(PerformedProcedureCode), "1-n", "2", "SRDocumentGeneralModule");
            if (result.good())
                result = PertinentOtherEvidence.write(dataset);
            if (result.good())
                result = ReferencedInstances.write(dataset);
        }

        if (result.good())
            IdenticalDocuments.write(dataset);          /* optional */
        if (result.good())
            result = CurrentRequestedProcedureEvidence.write(dataset);

        /* write SR document tree */
        if (result.good())
            result = DocumentTree.write(dataset, markedItems);
    } else
        result = SR_EC_InvalidDocument;
    return result;
}


OFCondition DSRDocument::readXML(const OFString &filename,
                                 const size_t flags)
{
    DSRXMLDocument doc;
    DCMSR_DEBUG("Reading SR document from XML format");
    /* read, parse and validate XML document */
    OFCondition result = doc.read(filename, flags);
    if (result.good())
    {
        /* re-initialize SR document */
        clear();
        /* start with document root node */
        DSRXMLCursor cursor(doc.getRootNode());
        /* check whether we really parse a "report" document */
        result = doc.checkNode(cursor, "report");
        if (result.good())
        {
            /* goto sub-element "sopclass" (first child node!) */
            result = doc.checkNode(cursor.gotoChild(), "sopclass");
            if (result.good())
            {
                /* determine document type (SOP class) */
                result = doc.getElementFromAttribute(cursor, SOPClassUID, "uid");
                if (result.good())
                {
                    OFString sopClassUID;
                    getSOPClassUID(sopClassUID);
                    /* create new document of specified type (also checks for support) */
                    result = createNewDocument(sopClassUIDToDocumentType(sopClassUID));
                    if (result.good())
                    {
                        /* proceed with document header */
                        result = readXMLDocumentHeader(doc, cursor.gotoNext(), flags);
                    } else
                        DCMSR_ERROR("Unknown/Unsupported SOP Class UID");
                }
            }
        }
    }
    return result;
}


OFCondition DSRDocument::readXMLDocumentHeader(DSRXMLDocument &doc,
                                               DSRXMLCursor cursor,
                                               const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (doc.valid() && cursor.valid())
    {
        result = EC_Normal;
        /* iterate over all nodes */
        while (cursor.valid() && result.good())
        {
            /* check for known element tags */
            if (doc.matchNode(cursor, "charset"))
            {
                /* use "charset" to decode special characters (has to be at the beginning) */
                if (!doc.encodingHandlerValid())
                {
                    OFString tmpString;
                    /* check for known character set */
                    setSpecificCharacterSet(doc.getStringFromNodeContent(cursor, tmpString));
                    if (tmpString.empty())
                        DCMSR_WARN("Empty value for 'charset' ... ignoring");
                    else {
                        const char *encString = characterSetToXMLName(SpecificCharacterSetEnum);
                        if ((strcmp(encString, "?") == 0) || doc.setEncodingHandler(encString).bad())
                            DCMSR_WARN("Character set '" << tmpString << "' not supported");
                    }
                } else {
                    /* only one "charset" node allowed */
                    doc.printUnexpectedNodeWarning(cursor);
                }
            }
            else if (doc.matchNode(cursor, "timezone"))
            {
                doc.getElementFromNodeContent(cursor, TimezoneOffsetFromUTC, NULL, OFTrue /*encoding*/);
            }
            else if (doc.matchNode(cursor, "modality"))
            {
                OFString tmpString;
                /* compare the XML node content */
                if (doc.getStringFromNodeContent(cursor, tmpString) != documentTypeToModality(getDocumentType()))
                    DCMSR_WARN("Invalid value for 'modality' ... ignoring");
            }
            else if (doc.matchNode(cursor, "device"))
            {
                doc.getElementFromNodeContent(doc.getNamedChildNode(cursor, "manufacturer", OFFalse /*required*/), Manufacturer, NULL, OFTrue /*encoding*/);
                doc.getElementFromNodeContent(doc.getNamedChildNode(cursor, "model"), ManufacturerModelName, NULL, OFTrue /*encoding*/);
                doc.getElementFromNodeContent(doc.getNamedChildNode(cursor, "serial", OFFalse /*required*/), DeviceSerialNumber, NULL, OFTrue /*encoding*/);
                doc.getElementFromNodeContent(doc.getNamedChildNode(cursor, "version", OFFalse /*required*/), SoftwareVersions, NULL, OFTrue /*encoding*/);
            }
            else if (doc.matchNode(cursor, "manufacturer"))
                doc.getElementFromNodeContent(cursor, Manufacturer, "manufacturer", OFTrue /*encoding*/);
            else if (doc.matchNode(cursor, "synchronization"))
            {
                doc.getElementFromAttribute(cursor, SynchronizationFrameOfReferenceUID, "uid");
                doc.getElementFromNodeContent(doc.getNamedChildNode(cursor, "trigger"), SynchronizationTrigger);
                doc.getElementFromNodeContent(doc.getNamedChildNode(cursor, "acquisitiontime"), AcquisitionTimeSynchronized);
            }
            else if (doc.matchNode(cursor, "referringphysician"))
            {
                /* goto sub-element "name" */
                const DSRXMLCursor childNode = doc.getNamedChildNode(cursor, "name");
                if (childNode.valid())
                {
                    /* Referring Physician's Name */
                    OFString tmpString;
                    DSRPNameTreeNode::getValueFromXMLNodeContent(doc, childNode.getChild(), tmpString);
                    ReferringPhysicianName.putOFStringArray(tmpString);
                }
            }
            else if (doc.matchNode(cursor, "patient"))
                result = readXMLPatientData(doc, cursor.getChild(), flags);
            else if (doc.matchNode(cursor, "study"))
                result = readXMLStudyData(doc, cursor, flags);
            else if (doc.matchNode(cursor, "series"))
                result = readXMLSeriesData(doc, cursor, flags);
            else if (doc.matchNode(cursor, "instance"))
                result = readXMLInstanceData(doc, cursor, flags);
            else if (doc.matchNode(cursor, "coding"))
            {
                const DSRXMLCursor childNode = cursor.getChild();
                if (childNode.valid())
                    result = CodingSchemeIdentification.readXML(doc, childNode, flags);
            }
            else if (doc.matchNode(cursor, "evidence"))
            {
                OFString typeString;
                /* check "type" attribute for corresponding sequence */
                if (doc.getStringFromAttribute(cursor, typeString, "type") == "Current Requested Procedure")
                    result = CurrentRequestedProcedureEvidence.readXML(doc, cursor.getChild(), flags);
                else if (typeString == "Pertinent Other")
                {
                    if (getDocumentType() != DT_KeyObjectSelectionDocument)
                        result = PertinentOtherEvidence.readXML(doc, cursor.getChild(), flags);
                    else
                        doc.printUnexpectedNodeWarning(cursor);
                } else // none of the standard defined evidence types
                    printUnknownValueWarningMessage("Evidence type", typeString.c_str());
            }
            else if (doc.matchNode(cursor, "reference"))
            {
                const DSRXMLCursor childNode = cursor.getChild();
                if (childNode.valid())
                    result = ReferencedInstances.readXML(doc, childNode, flags);
            }
            else if (doc.matchNode(cursor, "document"))
                result = readXMLDocumentData(doc, cursor.getChild(), flags);
            else
                doc.printUnexpectedNodeWarning(cursor);
            /* print node error message (if any) */
            doc.printGeneralNodeError(cursor, result);
            /* proceed with next node */
            cursor.gotoNext();
        }
    }
    return result;
}


OFCondition DSRDocument::readXMLPatientData(const DSRXMLDocument &doc,
                                            DSRXMLCursor cursor,
                                            const size_t /*flags*/)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString tmpString;
        result = EC_Normal;
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags (all type 2) */
            if (doc.matchNode(cursor, "name"))
            {
                /* Patient's Name */
                DSRPNameTreeNode::getValueFromXMLNodeContent(doc, cursor.getChild(), tmpString);
                PatientName.putOFStringArray(tmpString);
            }
            else if (doc.matchNode(cursor, "birthday"))
            {
                /* Patient's Birth Date */
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedChildNode(cursor, "date"), tmpString);
                PatientBirthDate.putOFStringArray(tmpString);
            }
            else if (doc.getElementFromNodeContent(cursor, PatientID, "id").bad() &&
                     doc.getElementFromNodeContent(cursor, IssuerOfPatientID, "issuer").bad() &&
                     doc.getElementFromNodeContent(cursor, PatientSex, "sex").bad())
            {
                doc.printUnexpectedNodeWarning(cursor);
            }
            /* proceed with next node */
            cursor.gotoNext();
        }
    }
    return result;
}


OFCondition DSRDocument::readXMLStudyData(const DSRXMLDocument &doc,
                                          DSRXMLCursor cursor,
                                          const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString tmpString;
        /* get Study Instance UID from XML attribute */
        if (flags & XF_acceptEmptyStudySeriesInstanceUID)
        {
            if (doc.getElementFromAttribute(cursor, StudyInstanceUID, "uid", OFFalse /*encoding*/, OFFalse /*required*/).bad())
                doc.printMissingAttributeWarning(cursor, "uid");
            result = EC_Normal;
        } else
            result = doc.getElementFromAttribute(cursor, StudyInstanceUID, "uid");
        /* goto first sub-element */
        cursor.gotoChild();
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.matchNode(cursor, "accession"))
            {
                /* goto sub-element "number" */
                doc.getElementFromNodeContent(doc.getNamedChildNode(cursor, "number"), AccessionNumber);
            }
            else if (doc.matchNode(cursor, "date"))
            {
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, cursor, tmpString);
                StudyDate.putOFStringArray(tmpString);
            }
            else if (doc.matchNode(cursor, "time"))
            {
                DSRTimeTreeNode::getValueFromXMLNodeContent(doc, cursor, tmpString);
                StudyTime.putOFStringArray(tmpString);
            }
            else if (doc.getElementFromNodeContent(cursor, StudyID, "id").bad() &&
                     doc.getElementFromNodeContent(cursor, StudyDescription, "description", OFTrue /*encoding*/).bad())
            {
                doc.printUnexpectedNodeWarning(cursor);
            }
            /* proceed with next node */
            cursor.gotoNext();
        }
        /* check required element values */
        checkElementValue(StudyInstanceUID, "1", "1");
    }
    return result;
}


OFCondition DSRDocument::readXMLSeriesData(const DSRXMLDocument &doc,
                                           DSRXMLCursor cursor,
                                           const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString tmpString;
        /* get Series Instance UID from XML attribute */
        if (flags & XF_acceptEmptyStudySeriesInstanceUID)
        {
            if (doc.getElementFromAttribute(cursor, SeriesInstanceUID, "uid", OFFalse /*encoding*/, OFFalse /*required*/).bad())
                doc.printMissingAttributeWarning(cursor, "uid");
            result = EC_Normal;
        } else
            result = doc.getElementFromAttribute(cursor, SeriesInstanceUID, "uid");
        /* goto first sub-element */
        cursor.gotoChild();
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.matchNode(cursor, "date"))
            {
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, cursor, tmpString);
                SeriesDate.putOFStringArray(tmpString);
            }
            else if (doc.matchNode(cursor, "time"))
            {
                DSRTimeTreeNode::getValueFromXMLNodeContent(doc, cursor, tmpString);
                SeriesTime.putOFStringArray(tmpString);
            }
            else if (doc.getElementFromNodeContent(cursor, SeriesNumber, "number").bad() &&
                doc.getElementFromNodeContent(cursor, ProtocolName, "protocol", OFTrue /*encoding*/).bad() &&
                doc.getElementFromNodeContent(cursor, SeriesDescription, "description", OFTrue /*encoding*/).bad())
            {
                doc.printUnexpectedNodeWarning(cursor);
            }
            /* proceed with next node */
            cursor.gotoNext();
        }
        /* check required element values */
        checkElementValue(SeriesInstanceUID, "1", "1");
        checkElementValue(SeriesNumber, "1", "1");
    }
    return result;
}


OFCondition DSRDocument::readXMLInstanceData(const DSRXMLDocument &doc,
                                             DSRXMLCursor cursor,
                                             const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString tmpString;
        /* get SOP Instance UID from XML attribute */
        if (flags & XF_acceptEmptyStudySeriesInstanceUID)
        {
            if (doc.getElementFromAttribute(cursor, SOPInstanceUID, "uid", OFFalse /*encoding*/, OFFalse /*required*/).bad())
                doc.printMissingAttributeWarning(cursor, "uid");
            result = EC_Normal;
        } else
            result = doc.getElementFromAttribute(cursor, SOPInstanceUID, "uid");
        /* goto first sub-element */
        cursor.gotoChild();
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.matchNode(cursor, "creation"))
            {
                /* Instance Creator UID */
                doc.getElementFromAttribute(cursor, InstanceCreatorUID, "uid", OFFalse /*encoding*/, OFFalse /*required*/);
                /* Instance Creation Date */
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedChildNode(cursor, "date"), tmpString);
                InstanceCreationDate.putOFStringArray(tmpString);
                /* Instance Creation Time */
                DSRTimeTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedChildNode(cursor, "time"), tmpString);
                InstanceCreationTime.putOFStringArray(tmpString);
            }
            else if (doc.getElementFromNodeContent(cursor, InstanceNumber, "number").bad())
                doc.printUnexpectedNodeWarning(cursor);
            /* proceed with next node */
            cursor.gotoNext();
        }
        /* check required element values */
        checkElementValue(SOPInstanceUID, "1", "1");
    }
    return result;
}


OFCondition DSRDocument::readXMLDocumentData(const DSRXMLDocument &doc,
                                             DSRXMLCursor cursor,
                                             const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString tmpString;
        const E_DocumentType documentType = getDocumentType();
        result = EC_Normal;
        /* iterate over all nodes */
        while (cursor.valid() && result.good())
        {
            /* check for known element tags
               (Key Object Selection Documents do not contain the SR Document General Module) */
            if ((documentType != DT_KeyObjectSelectionDocument) && doc.matchNode(cursor, "preliminary"))
            {
                /* Preliminary Flag */
                PreliminaryFlagEnum = enumeratedValueToPreliminaryFlag(doc.getStringFromAttribute(cursor, tmpString, "flag"));
                if (PreliminaryFlagEnum == PF_invalid)
                    printUnknownValueWarningMessage("PreliminaryFlag", tmpString.c_str());
            }
            else if ((documentType != DT_KeyObjectSelectionDocument) && doc.matchNode(cursor, "completion"))
            {
                /* Completion Flag */
                CompletionFlagEnum = enumeratedValueToCompletionFlag(doc.getStringFromAttribute(cursor, tmpString, "flag"));
                if (CompletionFlagEnum != CF_invalid)
                {
                    /* Completion Flag Description (optional) */
                    const DSRXMLCursor childCursor = doc.getNamedChildNode(cursor, "description", OFFalse /*required*/);
                    if (childCursor.valid())
                        doc.getElementFromNodeContent(childCursor, CompletionFlagDescription, NULL /*name*/, OFTrue /*encoding*/);
                } else
                    printUnknownValueWarningMessage("CompletionFlag", tmpString.c_str());
            }
            else if ((documentType != DT_KeyObjectSelectionDocument) && doc.matchNode(cursor, "verification"))
            {
                /* Verification Flag */
                VerificationFlagEnum = enumeratedValueToVerificationFlag(doc.getStringFromAttribute(cursor, tmpString, "flag"));
                if (VerificationFlagEnum != VF_invalid)
                {
                    /* Verifying Observers (required if VERIFIED) */
                    result = readXMLVerifyingObserverData(doc, cursor.getChild(), flags);
                    /* allow absence in case of UNVERIFIED */
                    if (VerificationFlagEnum == VF_Unverified)
                        result = EC_Normal;
                } else
                    printUnknownValueWarningMessage("VerificationFlag", tmpString.c_str());
            }
            else if ((documentType != DT_KeyObjectSelectionDocument) && doc.matchNode(cursor, "predecessor"))
            {
                /* Predecessor Documents Sequence (optional) */
                result = PredecessorDocuments.readXML(doc, cursor.getChild(), flags);
            }
            else if (doc.matchNode(cursor, "identical"))
            {
                /* Identical Documents Sequence (optional) */
                result = IdenticalDocuments.readXML(doc, cursor.getChild(), flags);
            }
            else if (doc.matchNode(cursor, "content"))
            {
                /* Content Date */
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedChildNode(cursor, "date"), tmpString);
                ContentDate.putOFStringArray(tmpString);
                /* Content Time */
                DSRTimeTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedChildNode(cursor, "time"), tmpString);
                ContentTime.putOFStringArray(tmpString);
                /* proceed with document tree */
                result = DocumentTree.readXML(doc, cursor.getChild(), flags);
            } else
                doc.printUnexpectedNodeWarning(cursor);
            /* print node error message (if any) */
            doc.printGeneralNodeError(cursor, result);
            /* proceed with next node */
            cursor.gotoNext();
        }
    }
    return result;
}


OFCondition DSRDocument::readXMLVerifyingObserverData(const DSRXMLDocument &doc,
                                                      DSRXMLCursor cursor,
                                                      const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        result = EC_Normal;
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.matchNode(cursor, "observer"))
            {
                DcmItem *ditem = new DcmItem();
                if (ditem != NULL)
                {
                    OFString dateTimeString, nameString, orgaString;
                    DSRCodedEntryValue codeValue;
                    DSRXMLCursor childCursor = cursor.getChild();
                    /* iterate over all child nodes */
                    while (childCursor.valid())
                    {
                        /* check for known element tags */
                        if (doc.matchNode(childCursor, "code"))
                        {
                            /* Verifying Observer Code */
                            codeValue.readXML(doc, childCursor, flags);
                        }
                        else if (doc.matchNode(childCursor, "name"))
                        {
                            /* Verifying Observer Name */
                            DSRPNameTreeNode::getValueFromXMLNodeContent(doc, childCursor.getChild(), nameString);
                        }
                        else if (doc.matchNode(childCursor, "datetime"))
                        {
                            /* Verification DateTime */
                            DSRDateTimeTreeNode::getValueFromXMLNodeContent(doc, childCursor, dateTimeString);
                        } else {
                            /* Verifying Observer Organization */
                            doc.getStringFromNodeContent(childCursor, orgaString, "organization", OFTrue /*encoding*/, OFFalse /*clearString*/);
                        }
                        /* proceed with next node */
                        childCursor.gotoNext();
                    }
                    /* put string values into the sequence item */
                    putStringValueToDataset(*ditem, DCM_VerificationDateTime, dateTimeString);
                    putStringValueToDataset(*ditem, DCM_VerifyingObserverName, nameString);
                    putStringValueToDataset(*ditem, DCM_VerifyingOrganization, orgaString);
                    /* write code value to sequence item (might be empty, type 2) */
                    codeValue.writeSequence(*ditem, DCM_VerifyingObserverIdentificationCodeSequence);
                    /* insert items into sequence */
                    VerifyingObserver.insert(ditem);
                }
            } else
                doc.printUnexpectedNodeWarning(cursor);
            /* proceed with next node */
            cursor.gotoNext();
        }
    }
    return result;
}


OFCondition DSRDocument::writeXML(STD_NAMESPACE ostream &stream,
                                  const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    /* only write valid documents */
    if (isValid())
    {
        DCMSR_DEBUG("Writing SR document to XML format");
        /* used for multiple purposes */
        OFString tmpString;
        /* update all DICOM attributes */
        updateAttributes();

        // --- XML document structure (start) ---

        stream << "<?xml version=\"1.0\"";
        /* optional character set */
        tmpString = characterSetToXMLName(SpecificCharacterSetEnum);
        if (!tmpString.empty() && (tmpString != "?"))
            stream << " encoding=\"" << tmpString << "\"";
        else if (!SpecificCharacterSet.isEmpty())
            DCMSR_WARN("Cannot map Specific Character Set to equivalent XML encoding");
        stream << "?>" << OFendl;

        stream << "<report";
        /* optional namespace declaration */
        if (flags & XF_useDcmsrNamespace)
            stream << " xmlns=\"" << DCMSR_XML_NAMESPACE_URI << "\"";
        /* optional XML Schema reference */
        if (flags & XF_addSchemaReference)
        {
            if (flags & XF_useDcmsrNamespace)
                stream << OFendl << "       ";
            stream << " xmlns:xsi=\"" XML_SCHEMA_INSTANCE_URI "\"" << OFendl << "       "
                   << " xsi:noNamespaceSchemaLocation=\"" DCMSR_XML_XSD_FILE "\"" << OFendl << "       ";
        }
        stream << " type=\"" << documentTypeToReadableName(getDocumentType()) << "\">" << OFendl;

        // --- write some general document information ---

        stream << "<sopclass uid=\"" << getMarkupStringFromElement(SOPClassUID, tmpString) << "\">";
        /* retrieve name of SOP class (if known) */
        stream << dcmFindNameOfUID(tmpString.c_str(), "" /* empty value as default */);
        stream << "</sopclass>" << OFendl;
        writeStringFromElementToXML(stream, SpecificCharacterSet, "charset", (flags & XF_writeEmptyTags) > 0);
        writeStringFromElementToXML(stream, TimezoneOffsetFromUTC, "timezone", (flags & XF_writeEmptyTags) > 0);
        writeStringFromElementToXML(stream, Modality, "modality", (flags & XF_writeEmptyTags) > 0);
        /* check for additional device information */
        if (!ManufacturerModelName.isEmpty())
        {
            stream << "<device>" << OFendl;
            writeStringFromElementToXML(stream, Manufacturer, "manufacturer", (flags & XF_writeEmptyTags) > 0);
            writeStringFromElementToXML(stream, ManufacturerModelName, "model", (flags & XF_writeEmptyTags) > 0);
            writeStringFromElementToXML(stream, DeviceSerialNumber, "serial", (flags & XF_writeEmptyTags) > 0);
            writeStringFromElementToXML(stream, SoftwareVersions, "version", (flags & XF_writeEmptyTags) > 0);
            stream << "</device>" << OFendl;
        } else
            writeStringFromElementToXML(stream, Manufacturer, "manufacturer", (flags & XF_writeEmptyTags) > 0);

        if ((flags & XF_writeEmptyTags) || !SynchronizationFrameOfReferenceUID.isEmpty() ||
            !SynchronizationTrigger.isEmpty() || !AcquisitionTimeSynchronized.isEmpty())
        {
            stream << "<synchronization";
            if (!SynchronizationFrameOfReferenceUID.isEmpty())
                stream << " uid=\"" << getMarkupStringFromElement(SynchronizationFrameOfReferenceUID, tmpString) << "\"";
            stream << ">" << OFendl;
            writeStringFromElementToXML(stream, SynchronizationTrigger, "trigger", (flags & XF_writeEmptyTags) > 0);
            writeStringFromElementToXML(stream, AcquisitionTimeSynchronized, "acquisitiontime", (flags & XF_writeEmptyTags) > 0);
            stream << "</synchronization>" << OFendl;
        }

        if ((flags & XF_writeEmptyTags) || !ReferringPhysicianName.isEmpty())
        {
            stream << "<referringphysician>" << OFendl;
            writeStringFromElementToXML(stream, ReferringPhysicianName, "name", (flags & XF_writeEmptyTags) > 0);
            stream << "</referringphysician>" << OFendl;
        }

        stream << "<patient>" << OFendl;
        writeStringFromElementToXML(stream, PatientID, "id", (flags & XF_writeEmptyTags) > 0);
        writeStringFromElementToXML(stream, IssuerOfPatientID, "issuer", (flags & XF_writeEmptyTags) > 0);
        writeStringFromElementToXML(stream, PatientName, "name", (flags & XF_writeEmptyTags) > 0);
        if ((flags & XF_writeEmptyTags) || !PatientBirthDate.isEmpty())
        {
            stream << "<birthday>" << OFendl;
            PatientBirthDate.getISOFormattedDate(tmpString);
            writeStringValueToXML(stream, tmpString, "date", (flags & XF_writeEmptyTags) > 0);
            stream << "</birthday>" << OFendl;
        }
        writeStringFromElementToXML(stream, PatientSex, "sex", (flags & XF_writeEmptyTags) > 0);
        stream << "</patient>" << OFendl;

        stream << "<study uid=\"" << getMarkupStringFromElement(StudyInstanceUID, tmpString) << "\">" << OFendl;
        writeStringFromElementToXML(stream, StudyID, "id", (flags & XF_writeEmptyTags) > 0);
        StudyDate.getISOFormattedDate(tmpString);
        writeStringValueToXML(stream, tmpString, "date", (flags & XF_writeEmptyTags) > 0);
        StudyTime.getISOFormattedTime(tmpString);
        writeStringValueToXML(stream, tmpString, "time", (flags & XF_writeEmptyTags) > 0);
        if ((flags & XF_writeEmptyTags) || !AccessionNumber.isEmpty())
        {
            stream << "<accession>" << OFendl;
            writeStringFromElementToXML(stream, AccessionNumber, "number", (flags & XF_writeEmptyTags) > 0);
            stream << "</accession>" << OFendl;
        }
        writeStringFromElementToXML(stream, StudyDescription, "description", (flags & XF_writeEmptyTags) > 0);
        stream << "</study>" << OFendl;

        stream << "<series uid=\"" << getMarkupStringFromElement(SeriesInstanceUID, tmpString) << "\">" << OFendl;
        writeStringFromElementToXML(stream, SeriesNumber, "number", (flags & XF_writeEmptyTags) > 0);
        SeriesDate.getISOFormattedDate(tmpString);
        writeStringValueToXML(stream, tmpString, "date", (flags & XF_writeEmptyTags) > 0);
        SeriesTime.getISOFormattedTime(tmpString);
        writeStringValueToXML(stream, tmpString, "time", (flags & XF_writeEmptyTags) > 0);
        writeStringFromElementToXML(stream, ProtocolName, "protocol", (flags & XF_writeEmptyTags) > 0);
        writeStringFromElementToXML(stream, SeriesDescription, "description", (flags & XF_writeEmptyTags) > 0);
        stream << "</series>" << OFendl;

        stream << "<instance uid=\"" << getMarkupStringFromElement(SOPInstanceUID, tmpString) << "\">" << OFendl;
        writeStringFromElementToXML(stream, InstanceNumber, "number", (flags & XF_writeEmptyTags) > 0);
        if ((flags & XF_writeEmptyTags) || !InstanceCreatorUID.isEmpty() ||
            !InstanceCreationDate.isEmpty() || !InstanceCreationTime.isEmpty())
        {
            stream << "<creation";
            if (!InstanceCreatorUID.isEmpty())
                stream << " uid=\"" << getMarkupStringFromElement(InstanceCreatorUID, tmpString) << "\"";
            stream << ">" << OFendl;
            InstanceCreationDate.getISOFormattedDate(tmpString);
            writeStringValueToXML(stream, tmpString, "date", (flags & XF_writeEmptyTags) > 0);
            InstanceCreationTime.getISOFormattedTime(tmpString);
            writeStringValueToXML(stream, tmpString, "time", (flags & XF_writeEmptyTags) > 0);
            stream << "</creation>" << OFendl;
        }
        stream << "</instance>" << OFendl;

        if ((flags & XF_writeEmptyTags) || !CodingSchemeIdentification.isEmpty())
        {
            stream << "<coding>" << OFendl;
            CodingSchemeIdentification.writeXML(stream, flags);
            stream << "</coding>" << OFendl;
        }
        if ((flags & XF_writeEmptyTags) || !CurrentRequestedProcedureEvidence.isEmpty())
        {
            stream << "<evidence type=\"Current Requested Procedure\">" << OFendl;
            CurrentRequestedProcedureEvidence.writeXML(stream, flags);
            stream << "</evidence>" << OFendl;
        }
        if (getDocumentType() != DT_KeyObjectSelectionDocument)
        {
            if ((flags & XF_writeEmptyTags) || !PertinentOtherEvidence.isEmpty())
            {
                stream << "<evidence type=\"Pertinent Other\">" << OFendl;
                PertinentOtherEvidence.writeXML(stream, flags);
                stream << "</evidence>" << OFendl;
            }
            if ((flags & XF_writeEmptyTags) || !ReferencedInstances.isEmpty())
            {
                stream << "<reference>" << OFendl;
                ReferencedInstances.writeXML(stream, flags);
                stream << "</reference>" << OFendl;
            }
        }

        stream << "<document>" << OFendl;
        if (getDocumentType() != DT_KeyObjectSelectionDocument)
        {
            if (!PreliminaryFlag.isEmpty())
                stream << "<preliminary flag=\"" << getStringValueFromElement(PreliminaryFlag, tmpString) << "\"/>" << OFendl;
            stream << "<completion flag=\"" << getStringValueFromElement(CompletionFlag, tmpString) << "\">" << OFendl;
            writeStringFromElementToXML(stream, CompletionFlagDescription, "description", (flags & XF_writeEmptyTags) > 0);
            stream << "</completion>" << OFendl;

            stream << "<verification flag=\"" << getStringValueFromElement(VerificationFlag, tmpString) << "\">" << OFendl;
            const size_t obsCount = getNumberOfVerifyingObservers();
            for (size_t i = 1; i <= obsCount; i++)
            {
                stream << "<observer pos=\"" << i << "\">" << OFendl;
                DSRCodedEntryValue obsCode;
                OFString dateTime, obsName, organization;
                if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization).good())
                {
                    /* output time in ISO 8601 format */
                    DcmDateTime::getISOFormattedDateTimeFromString(dateTime, tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/,
                        OFTrue /*timeZone*/, OFFalse /*createMissingPart*/, "T" /*dateTimeSeparator*/, "" /*timeZoneSeparator*/);
                    writeStringValueToXML(stream, tmpString, "datetime", (flags & XF_writeEmptyTags) > 0);
                    if (!obsName.empty() || (flags & XF_writeEmptyTags))
                        stream << "<name>" << OFendl << dicomToXMLPersonName(obsName, tmpString) << OFendl << "</name>" << OFendl;
                    if (obsCode.isValid())
                    {
                        if (flags & DSRTypes::XF_codeComponentsAsAttribute)
                            stream << "<code";     // bracket ">" is closed in next writeXML() call
                        else
                            stream << "<code>" << OFendl;
                        obsCode.writeXML(stream, flags);
                        stream << "</code>" << OFendl;
                    }
                    writeStringValueToXML(stream, organization, "organization", (flags & XF_writeEmptyTags) > 0);
                }
                stream << "</observer>" << OFendl;
            }
            stream << "</verification>" << OFendl;

            if ((flags & XF_writeEmptyTags) || !PredecessorDocuments.isEmpty())
            {
                stream << "<predecessor>" << OFendl;
                PredecessorDocuments.writeXML(stream, flags);
                stream << "</predecessor>" << OFendl;
            }
        }
        if ((flags & XF_writeEmptyTags) || !IdenticalDocuments.isEmpty())
        {
            stream << "<identical>" << OFendl;
            IdenticalDocuments.writeXML(stream, flags);
            stream << "</identical>" << OFendl;
        }

        // --- write document content/tree to stream ---

        stream << "<content>" << OFendl;
        ContentDate.getISOFormattedDate(tmpString);
        writeStringValueToXML(stream, tmpString, "date", (flags & XF_writeEmptyTags) > 0);
        ContentTime.getISOFormattedTime(tmpString);
        writeStringValueToXML(stream, tmpString, "time", (flags & XF_writeEmptyTags) > 0);
        result = DocumentTree.writeXML(stream, flags);
        stream << "</content>" << OFendl;
        stream << "</document>" << OFendl;

        // --- XML document structure (end) ---

        stream << "</report>" << OFendl;
    }
    return result;
}


void DSRDocument::renderHTMLPatientData(STD_NAMESPACE ostream &stream,
                                        const size_t flags)
{
    OFString tmpString, string2;
    OFString htmlString;
    stream << convertToHTMLString(dicomToReadablePersonName(getStringValueFromElement(PatientName, tmpString), string2), htmlString, flags);
    OFString patientStr;
    if (!PatientSex.isEmpty())
    {
        getPrintStringFromElement(PatientSex, tmpString);
        if (tmpString == "M")
            patientStr += "male";
        else if (tmpString == "F")
            patientStr += "female";
        else if (tmpString == "O")
            patientStr += "other";
        else
            patientStr += convertToHTMLString(tmpString, htmlString, flags);
    }
    if (!PatientBirthDate.isEmpty())
    {
        if (!patientStr.empty())
            patientStr += ", ";
        patientStr += '*';
        patientStr += dicomToReadableDate(getStringValueFromElement(PatientBirthDate, tmpString), string2);
    }
    if (!PatientID.isEmpty())
    {
        if (!patientStr.empty())
            patientStr += ", ";
        patientStr += '#';
        patientStr += convertToHTMLString(getStringValueFromElement(PatientID, tmpString), htmlString, flags);
        if (!IssuerOfPatientID.isEmpty())
        {
            patientStr += ":";
            patientStr += convertToHTMLString(getStringValueFromElement(IssuerOfPatientID, tmpString), htmlString, flags);
        }
    }
    if (!patientStr.empty())
        stream << " (" << patientStr << ")";
}


void DSRDocument::renderHTMLReferenceList(STD_NAMESPACE ostream &stream,
                                          DSRSOPInstanceReferenceList &refList,
                                          const size_t flags)
{
    /* goto first list item (if not empty) */
    if (refList.gotoFirstItem().good())
    {
        OFString tmpString;
        DSRCodedEntryValue codeValue;
        unsigned int i = 0;
        /* iterate over all list items */
        do {
            if (i > 0)
            {
                stream << "</tr>" << OFendl;
                stream << "<tr>" << OFendl;
                stream << "<td></td>" << OFendl;
            }
            /* hyperlink to composite object */
            OFString sopClass, sopInstance;
            if (!refList.getSOPClassUID(sopClass).empty() && !refList.getSOPInstanceUID(sopInstance).empty())
            {
                stream << "<td><a href=\"" << HTML_HYPERLINK_PREFIX_FOR_CGI;
                stream << "?composite=" << sopClass << "+" << sopInstance << "\">";
                /* check whether referenced object has a well-known SOP class */
                stream << dcmFindNameOfUID(sopClass.c_str(), "unknown composite object");
                stream << "</a>";
                /* try to get the purpose of reference code, which is optional */
                if (refList.getPurposeOfReference(codeValue).good() && !codeValue.getCodeMeaning().empty())
                    stream << " (" << DSRTypes::convertToHTMLString(codeValue.getCodeMeaning(), tmpString, flags) << ")";
                stream << "</td>" << OFendl;
            } else
                stream << "<td><i>invalid object reference</i></td>" << OFendl;
            i++;
        } while (refList.gotoNextItem().good());
    }
}


void DSRDocument::renderHTMLReferenceList(STD_NAMESPACE ostream &stream,
                                          DSRReferencedInstanceList &refList,
                                          const size_t flags)
{
    /* goto first list item (if not empty) */
    if (refList.gotoFirstItem().good())
    {
        OFString tmpString;
        DSRCodedEntryValue codeValue;
        unsigned int i = 0;
        /* iterate over all list items */
        do {
            if (i > 0)
            {
                stream << "</tr>" << OFendl;
                stream << "<tr>" << OFendl;
                stream << "<td></td>" << OFendl;
            }
            /* hyperlink to composite object */
            OFString sopClass, sopInstance;
            if (!refList.getSOPClassUID(sopClass).empty() && !refList.getSOPInstanceUID(sopInstance).empty())
            {
                stream << "<td><a href=\"" << HTML_HYPERLINK_PREFIX_FOR_CGI;
                stream << "?composite=" << sopClass << "+" << sopInstance << "\">";
                /* retrieve name of SOP class (if known) */
                stream << dcmFindNameOfUID(sopClass.c_str(), "unknown composite object");
                stream << "</a>";
                /* try to get the purpose of reference code (at least the code meaning) */
                if (refList.getPurposeOfReference(codeValue).good() && !codeValue.getCodeMeaning().empty())
                    stream << " (" << DSRTypes::convertToHTMLString(codeValue.getCodeMeaning(), tmpString, flags) << ")";
                stream << "</td>" << OFendl;
            } else
                stream << "<td><i>invalid reference</i></td>" << OFendl;
            i++;
        } while (refList.gotoNextItem().good());
    }
}


OFCondition DSRDocument::renderHTML(STD_NAMESPACE ostream &stream,
                                    const size_t flags,
                                    const char *styleSheet)
{
    OFCondition result = SR_EC_InvalidDocument;
    /* only render valid documents */
    if (isValid())
    {
        size_t newFlags = flags;
        if (flags & HF_XHTML11Compatibility)
            newFlags = (flags & ~DSRTypes::HF_HTML32Compatibility);
        else if (flags & HF_HTML32Compatibility)
        {
            /* fixes for HTML 3.2 */
            newFlags = (flags & ~HF_useCodeDetailsTooltip) | HF_convertNonASCIICharacters;
            /* ignore CSS (if any) */
            styleSheet = NULL;
        }

        /* used for multiple purposes */
        OFString tmpString, string2;
        /* used for HTML tmpString conversion */
        OFString htmlString;
        /* update only some DICOM attributes */
        updateAttributes(OFFalse /*updateAll*/);

        // --- HTML/XHTML document structure (start) ---

        if (newFlags & HF_XHTML11Compatibility)
        {
            stream << "<?xml version=\"1.0\"";
            /* optional character set */
            tmpString = characterSetToXMLName(SpecificCharacterSetEnum);
            if (!tmpString.empty())
            {
                if (tmpString != "?")
                    stream << " encoding=\"" << tmpString << "\"";
                else
                    DCMSR_WARN("Cannot map Specific Character Set to equivalent XML encoding");
            }
            stream << "?>" << OFendl;
        }

        /* optional document type definition */
        if (newFlags & HF_addDocumentTypeReference)
        {
            if (newFlags & HF_XHTML11Compatibility)
                stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << OFendl;
            else {
                if (newFlags & HF_HTML32Compatibility)
                    stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">" << OFendl;
                else
                    stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << OFendl;
            }
        }
        stream << "<html";
        if (newFlags & HF_XHTML11Compatibility)
            stream << " xmlns=\"http://www.w3.org/1999/xhtml\"";
        stream << ">" << OFendl;
        stream << "<head>" << OFendl;
        /* document title */
        stream << "<title>";
        if (newFlags & HF_renderPatientTitle)
            renderHTMLPatientData(stream, newFlags);
        else
            stream << documentTypeToDocumentTitle(getDocumentType(), tmpString);
        stream << "</title>" << OFendl;
        /* for HTML 4.01 and XHTML 1.1 only */
        if (!(newFlags & HF_HTML32Compatibility))
        {
            /* optional cascading style sheet */
            if (styleSheet != NULL)
            {
                if (newFlags & HF_copyStyleSheetContent)
                {
                    /* copy content from CSS file */
#ifdef HAVE_IOS_NOCREATE
                    STD_NAMESPACE ifstream cssFile(styleSheet, STD_NAMESPACE ios::in | STD_NAMESPACE ios::nocreate);
#else
                    STD_NAMESPACE ifstream cssFile(styleSheet, STD_NAMESPACE ios::in);
#endif
                    if (cssFile)
                    {
                        char c;
                        stream << "<style type=\"text/css\">" << OFendl;
                        stream << "<!--" << OFendl;
                        /* copy all characters */
                        while (cssFile.get(c))
                            stream << c;
                        stream << "//-->" << OFendl;
                        stream << "</style>" << OFendl;
                    } else
                        DCMSR_WARN("Could not open CSS file \"" << styleSheet << "\" ... ignoring");
                } else {
                    /* just add a reference to the CSS file (might be an URL) */
                    stream << "<link rel=\"stylesheet\" type=\"text/css\" href=\"" << styleSheet << "\"";
                    if (newFlags & HF_XHTML11Compatibility)
                        stream << " /";
                    stream << ">" << OFendl;
                }
            }
            /* optional character set */
            tmpString = characterSetToHTMLName(SpecificCharacterSetEnum);
            if (!tmpString.empty())
            {
                if (tmpString != "?")
                {
                    stream << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=" << tmpString << "\"";
                    if (newFlags & HF_XHTML11Compatibility)
                        stream << " /";
                    stream << ">" << OFendl;
                } else
                    DCMSR_WARN("Cannot map Specific Character Set to equivalent HTML charset");
            }
        }
        /* generator meta element referring to the DCMTK */
        if (!(newFlags & HF_omitGeneratorMetaElement))
        {
            stream << "<meta name=\"generator\" content=\"OFFIS DCMTK " << OFFIS_DCMTK_VERSION << "\"";
            if (newFlags & HF_XHTML11Compatibility)
                stream << " /";
            stream << ">" << OFendl;
        }
        stream << "</head>" << OFendl;
        stream << "<body>" << OFendl;

        // --- render some general document information ---

        if (!(newFlags & HF_renderNoDocumentHeader))
        {
            /* create a table for this purpose */
            stream << "<table>" << OFendl;
            /* patient related information */
            if (!PatientName.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Patient:</b></td>" << OFendl;
                stream << "<td>";
                renderHTMLPatientData(stream, newFlags);
                stream << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
            }
            /* referring physician */
            if (!ReferringPhysicianName.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Referring Physician:</b></td>" << OFendl;
                stream << "<td>" << convertToHTMLString(dicomToReadablePersonName(getStringValueFromElement(ReferringPhysicianName, tmpString), string2), htmlString, newFlags);
                stream << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
            }
            /* study-related information */
            if (!StudyDescription.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Study:</b></td>" << OFendl;
                stream << "<td>" << convertToHTMLString(getStringValueFromElement(StudyDescription, tmpString), htmlString, newFlags);
                if (!StudyID.isEmpty())
                    stream << " (#" << convertToHTMLString(getStringValueFromElement(StudyID, tmpString), htmlString, newFlags) << ")";
                stream << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
            }
            /* series-related information */
            if (!SeriesDescription.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Series:</b></td>" << OFendl;
                stream << "<td>" << convertToHTMLString(getStringValueFromElement(SeriesDescription, tmpString), htmlString, newFlags);
                if (!SeriesNumber.isEmpty())
                    stream << " (#" << convertToHTMLString(getStringValueFromElement(SeriesNumber, tmpString), htmlString, newFlags) << ")";
                stream << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
            }
            /* protocol name */
            if (!ProtocolName.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Protocol:</b></td>" << OFendl;
                stream << "<td>" << convertToHTMLString(getStringValueFromElement(ProtocolName, tmpString), htmlString, newFlags) << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
            }
            /* manufacturer */
            if (!Manufacturer.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Manufacturer:</b></td>" << OFendl;
                stream << "<td>" << convertToHTMLString(getStringValueFromElement(Manufacturer, tmpString), htmlString, newFlags);
                OFString deviceStr;
                if (!ManufacturerModelName.isEmpty())
                    deviceStr += convertToHTMLString(getStringValueFromElement(ManufacturerModelName, tmpString), htmlString, newFlags);
                if (!DeviceSerialNumber.isEmpty())
                {
                   if (!deviceStr.empty())
                       deviceStr += ", ";
                   deviceStr += '#';
                   deviceStr += convertToHTMLString(getStringValueFromElement(DeviceSerialNumber, tmpString), htmlString, newFlags);
                }
                if (!deviceStr.empty())
                    stream << " (" << deviceStr << ")";
                stream << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
            }
            if (getDocumentType() != DT_KeyObjectSelectionDocument)
            {
                /* preliminary flag */
                if (!PreliminaryFlag.isEmpty())
                {
                    stream << "<tr>" << OFendl;
                    stream << "<td><b>Preliminary Flag:</b></td>" << OFendl;
                    stream << "<td>" << getStringValueFromElement(PreliminaryFlag, tmpString) << "</td>" << OFendl;
                    stream << "</tr>" << OFendl;
                }
                /* completion flag */
                stream << "<tr>" << OFendl;
                stream << "<td><b>Completion Flag:</b></td>" << OFendl;
                stream << "<td>" << getStringValueFromElement(CompletionFlag, tmpString) << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
                /* completion flag description */
                if (!CompletionFlagDescription.isEmpty())
                {
                    stream << "<tr>" << OFendl;
                    stream << "<td></td>" << OFendl;
                    stream << "<td>" << convertToHTMLString(getStringValueFromElement(CompletionFlagDescription, tmpString), htmlString, newFlags);
                    stream << "</td>" << OFendl;
                    stream << "</tr>" << OFendl;
                }
                /* predecessor documents */
                if (!PredecessorDocuments.isEmpty())
                {
                    stream << "<tr>" << OFendl;
                    stream << "<td><b>Predecessor Docs:</b></td>" << OFendl;
                    renderHTMLReferenceList(stream, PredecessorDocuments, flags);
                    stream << "</tr>" << OFendl;
                }
            }
            /* identical documents */
            if (!IdenticalDocuments.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Identical Docs:</b></td>" << OFendl;
                renderHTMLReferenceList(stream, IdenticalDocuments, flags);
                stream << "</tr>" << OFendl;
            }
            /* referenced instances */
            if (!ReferencedInstances.isEmpty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Referenced Objects:</b></td>" << OFendl;
                renderHTMLReferenceList(stream, ReferencedInstances, flags);
                stream << "</tr>" << OFendl;
            }
            if (getDocumentType() != DT_KeyObjectSelectionDocument)
            {
                /* verification flag */
                stream << "<tr>" << OFendl;
                stream << "<td><b>Verification Flag:</b></td>" << OFendl;
                stream << "<td>" << getStringValueFromElement(VerificationFlag, tmpString) << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
                /* verifying observer */
                const size_t obsCount = getNumberOfVerifyingObservers();
                for (size_t i = 1; i <= obsCount; i++)
                {
                    OFString dateTime, obsName, organization;
                    DSRCodedEntryValue obsCode;
                    if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization).good())
                    {
                        stream << "<tr>" << OFendl;
                        if (i == 1)
                            stream << "<td><b>Verifying Observers:</b></td>" << OFendl;
                        else
                            stream << "<td></td>" << OFendl;
                        stream << "<td>";
                        stream << dicomToReadableDateTime(dateTime, string2) << " - ";
                        /* render code details as a tooltip (HTML 4.01 or above) */
                        if (obsCode.isValid() && (newFlags & DSRTypes::HF_useCodeDetailsTooltip))
                        {
                            if (newFlags & HF_XHTML11Compatibility)
                                stream << "<span class=\"code\" title=\"(";
                            else /* HTML 4.01 */
                                stream << "<span class=\"under\" title=\"(";
                            stream << convertToHTMLString(obsCode.getCodeValue(), htmlString, newFlags) << ", ";
                            stream << convertToHTMLString(obsCode.getCodingSchemeDesignator(), htmlString, newFlags);
                            if (!obsCode.getCodingSchemeVersion().empty())
                                stream << " [" << convertToHTMLString(obsCode.getCodingSchemeVersion(), htmlString, newFlags) << "]";
                            stream << ", &quot;" << convertToHTMLString(obsCode.getCodeMeaning(), htmlString, newFlags) << "&quot;)\">";
                        }
                        stream << convertToHTMLString(dicomToReadablePersonName(obsName, string2), htmlString, newFlags);
                        if (obsCode.isValid() && (newFlags & DSRTypes::HF_useCodeDetailsTooltip))
                            stream << "</span>";
                        else {
                            /* render code in a conventional manner */
                            if (obsCode.isValid() && ((newFlags & HF_renderAllCodes) == HF_renderAllCodes))
                            {
                                stream << " (" << convertToHTMLString(obsCode.getCodeValue(), htmlString, newFlags);
                                stream << ", " << convertToHTMLString(obsCode.getCodingSchemeDesignator(), htmlString, newFlags);
                                if (!obsCode.getCodingSchemeVersion().empty())
                                    stream << " [" << convertToHTMLString(obsCode.getCodingSchemeVersion(), htmlString, newFlags) << "]";
                                stream << ", &quot;" << convertToHTMLString(obsCode.getCodeMeaning(), htmlString, newFlags) << "&quot;)";
                            }
                        }
                        stream << ", " << convertToHTMLString(organization, htmlString, newFlags);
                        stream << "</td>" << OFendl;
                        stream << "</tr>" << OFendl;
                    }
                }
            }
            if (!ContentDate.isEmpty() && !ContentTime.isEmpty())
            {
                /* content date and time */
                stream << "<tr>" << OFendl;
                stream << "<td><b>Content Date/Time:</b></td>" << OFendl;
                stream << "<td>" << dicomToReadableDate(getStringValueFromElement(ContentDate, tmpString), string2) << " ";
                stream << dicomToReadableTime(getStringValueFromElement(ContentTime, tmpString), string2) << "</td>" << OFendl;
                stream << "</tr>" << OFendl;
            }
            /* end of table */
            stream << "</table>" << OFendl;

            if (newFlags & HF_XHTML11Compatibility)
                stream << "<hr />" << OFendl;
            else
                stream << "<hr>" << OFendl;
        }

        // --- render document tree to stream ---

        /* create memory output stream for the annex */
        OFOStringStream annexStream;
        /* render document tree two the streams */
        result = DocumentTree.renderHTML(stream, annexStream, newFlags);
        /* append annex (with heading) to main document */
        if (result.good())
            result = appendStream(stream, annexStream, "<h1>Annex</h1>");

        // --- footnote ---

        if (newFlags & HF_renderDcmtkFootnote)
        {
            if (newFlags & HF_XHTML11Compatibility)
            {
                stream << "<hr />" << OFendl;
                stream << "<div class=\"footnote\">" << OFendl;
            } else {
                stream << "<hr>" << OFendl;
                if (newFlags & HF_HTML32Compatibility)
                    stream << "<div>" << OFendl;
                else /* HTML 4.01 */
                    stream << "<div class=\"footnote\">" << OFendl;
                stream << "<small>" << OFendl;
            }
            stream << "This page was generated from a DICOM Structured Reporting document by ";
            stream << "<a href=\"" << DCMTK_INTERNET_URL << "\">OFFIS DCMTK</a> " << OFFIS_DCMTK_VERSION << "." << OFendl;
            if (!(newFlags & HF_XHTML11Compatibility))
                stream << "</small>" << OFendl;
            stream << "</div>" << OFendl;
        }

        // --- HTML document structure (end) ---

        stream << "</body>" << OFendl;
        stream << "</html>" << OFendl;
    }
    return result;
}


DSRTypes::E_DocumentType DSRDocument::getDocumentType() const
{
    return DocumentTree.getDocumentType();
}


OFCondition DSRDocument::setTree(const DSRDocumentTree &tree)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* make sure that the new tree is valid */
    if (tree.isValid())
    {
        /* replace current tree with given one */
        DocumentTree = tree;
        /* update IOD-specific DICOM attributes */
        updateAttributes(OFFalse /*updateAll*/);
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRDocument::setTreeFromRootTemplate(DSRRootTemplate &rootTemplate,
                                                 const OFBool expandTree)
{
    OFCondition result = EC_Normal;
    /* check whether to expand the included templates (if any) */
    if (expandTree)
    {
        DSRDocumentSubTree *tree = NULL;
        /* expand tree managed by the template and replace the currently stored tree */
        result = rootTemplate.getTree().createExpandedSubTree(tree);
        if (result.good())
            result = DocumentTree.changeDocumentType(rootTemplate.getDocumentType(), OFTrue /*deleteTree*/);
        if (result.good())
            result = DocumentTree.insertSubTree(tree, AM_belowCurrent, RT_unknown, OFFalse /*deleteIfFail*/);
        /* update IOD-specific DICOM attributes */
        updateAttributes(OFFalse /*updateAll*/);
        /* in case of error, free memory */
        if (result.bad())
            delete tree;
    } else {
        /* call the functions doing the real work */
        result = setTree(rootTemplate.getTree());
    }
    return result;
}


const char *DSRDocument::getSpecificCharacterSet() const
{
    /* never return NULL */
    return OFSTRING_GUARD(getStringValueFromElement(SpecificCharacterSet));
}


DSRTypes::E_CharacterSet DSRDocument::getSpecificCharacterSetType() const
{
    return SpecificCharacterSetEnum;
}


OFCondition DSRDocument::setSpecificCharacterSetType(const E_CharacterSet characterSet)
{
    SpecificCharacterSetEnum = characterSet;
    return SpecificCharacterSet.putString(characterSetToDefinedTerm(SpecificCharacterSetEnum));
}


DSRTypes::E_PreliminaryFlag DSRDocument::getPreliminaryFlag() const
{
    return PreliminaryFlagEnum;
}


OFCondition DSRDocument::setPreliminaryFlag(const E_PreliminaryFlag flag)
{
    OFCondition result = EC_IllegalCall;
    /* not applicable to Key Object Selection Documents */
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
        PreliminaryFlagEnum = flag;
        result = EC_Normal;
    }
    return result;
}


DSRTypes::E_CompletionFlag DSRDocument::getCompletionFlag() const
{
    return CompletionFlagEnum;
}


DSRTypes::E_VerificationFlag DSRDocument::getVerificationFlag() const
{
    return VerificationFlagEnum;
}


OFBool DSRDocument::hasVerifyingObservers() const
{
    return (VerifyingObserver.card() > 0);
}


size_t DSRDocument::getNumberOfVerifyingObservers() const
{
    return OFstatic_cast(size_t, VerifyingObserver.card());
}


OFCondition DSRDocument::getVerifyingObserver(const size_t idx,
                                              OFString &dateTime,
                                              OFString &observerName,
                                              OFString &organization)
{
    DSRCodedEntryValue dummyCode;
    return getVerifyingObserver(idx, dateTime, observerName, dummyCode, organization);
}


OFCondition DSRDocument::getVerifyingObserver(const size_t idx,
                                              OFString &dateTime,
                                              OFString &observerName,
                                              DSRCodedEntryValue &observerCode,
                                              OFString &organization)
{
    OFCondition result = EC_IllegalParameter;
    /* clear all reference variables */
    dateTime.clear();
    observerName.clear();
    observerCode.clear();
    organization.clear();
    /* get specified entry */
    if ((idx > 0) && (idx <= getNumberOfVerifyingObservers()))
    {
        /* access by index is currently not very efficient */
        DcmItem *ditem = VerifyingObserver.getItem(OFstatic_cast(unsigned long, idx - 1));
        if (ditem != NULL)
        {
            result = getStringValueFromDataset(*ditem, DCM_VerificationDateTime, dateTime);
            if (result.good())
                result = getStringValueFromDataset(*ditem, DCM_VerifyingObserverName, observerName);
            if (result.good())
            {
                /* code is optional (type 2) */
                observerCode.readSequence(*ditem, DCM_VerifyingObserverIdentificationCodeSequence, "2" /*type*/);
                result = getStringValueFromDataset(*ditem, DCM_VerifyingOrganization, organization);
            }
            if (result.good())
            {
                if (dateTime.empty() || observerName.empty() || organization.empty())
                    result = SR_EC_InvalidValue;
            }
        }
    }
    return result;
}


DSRSOPInstanceReferenceList &DSRDocument::getPredecessorDocuments()
{
    /* pass current value of specific character set */
    PredecessorDocuments.setSpecificCharacterSet(getSpecificCharacterSet());
    return PredecessorDocuments;
}


DSRSOPInstanceReferenceList &DSRDocument::getIdenticalDocuments()
{
    /* pass current value of specific character set */
    IdenticalDocuments.setSpecificCharacterSet(getSpecificCharacterSet());
    return IdenticalDocuments;
}


DSRSOPInstanceReferenceList &DSRDocument::getCurrentRequestedProcedureEvidence()
{
    /* pass current value of specific character set */
    CurrentRequestedProcedureEvidence.setSpecificCharacterSet(getSpecificCharacterSet());
    return CurrentRequestedProcedureEvidence;
}


DSRSOPInstanceReferenceList &DSRDocument::getPertinentOtherEvidence()
{
    /* pass current value of specific character set */
    PredecessorDocuments.setSpecificCharacterSet(getSpecificCharacterSet());
    return PertinentOtherEvidence;
}


DSRReferencedInstanceList &DSRDocument::getReferencedInstances()
{
    return ReferencedInstances;
}


DSRCodingSchemeIdentificationList &DSRDocument::getCodingSchemeIdentification()
{
    /* pass current value of specific character set */
    CodingSchemeIdentification.setSpecificCharacterSet(getSpecificCharacterSet());
    return CodingSchemeIdentification;
}


// --- get attributes (C++ string) ---

OFCondition DSRDocument::getSpecificCharacterSet(OFString &value,
                                                 const signed long pos) const
{
    return getStringValueFromElement(SpecificCharacterSet, value, pos);
}


OFCondition DSRDocument::getCompletionFlagDescription(OFString &value,
                                                     const signed long pos) const
{
    return getStringValueFromElement(CompletionFlagDescription, value, pos);
}


OFCondition DSRDocument::getModality(OFString &value,
                                     const signed long pos) const
{
    return getStringValueFromElement(Modality, value, pos);
}


OFCondition DSRDocument::getSOPClassUID(OFString &value,
                                        const signed long pos) const
{
    return getStringValueFromElement(SOPClassUID, value, pos);
}


OFCondition DSRDocument::getStudyInstanceUID(OFString &value,
                                             const signed long pos) const
{
    return getStringValueFromElement(StudyInstanceUID, value, pos);
}


OFCondition DSRDocument::getSeriesInstanceUID(OFString &value,
                                              const signed long pos) const
{
    return getStringValueFromElement(SeriesInstanceUID, value, pos);
}


OFCondition DSRDocument::getSOPInstanceUID(OFString &value,
                                           const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceUID, value, pos);
}


OFCondition DSRDocument::getInstanceCreatorUID(OFString &value,
                                               const signed long pos) const
{
    return getStringValueFromElement(InstanceCreatorUID, value, pos);
}


OFCondition DSRDocument::getTimezoneOffsetFromUTC(OFString &value,
                                                  const signed long pos) const
{
    return getStringValueFromElement(TimezoneOffsetFromUTC, value, pos);
}


OFCondition DSRDocument::getPatientName(OFString &value,
                                        const signed long pos) const
{
    return getStringValueFromElement(PatientName, value, pos);
}


OFCondition DSRDocument::getPatientBirthDate(OFString &value,
                                             const signed long pos) const
{
    return getStringValueFromElement(PatientBirthDate, value, pos);
}


OFCondition DSRDocument::getPatientSex(OFString &value,
                                       const signed long pos) const
{
    return getStringValueFromElement(PatientSex, value, pos);
}


OFCondition DSRDocument::getReferringPhysicianName(OFString &value,
                                                   const signed long pos) const
{
    return getStringValueFromElement(ReferringPhysicianName, value, pos);
}


OFCondition DSRDocument::getStudyDescription(OFString &value,
                                             const signed long pos) const
{
    return getStringValueFromElement(StudyDescription, value, pos);
}


OFCondition DSRDocument::getSeriesDescription(OFString &value,
                                              const signed long pos) const
{
    return getStringValueFromElement(SeriesDescription, value, pos);
}


OFCondition DSRDocument::getProtocolName(OFString &value,
                                         const signed long pos) const
{
    return getStringValueFromElement(ProtocolName, value, pos);
}


OFCondition DSRDocument::getManufacturer(OFString &value,
                                         const signed long pos) const
{
    return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DSRDocument::getManufacturerModelName(OFString &value,
                                                  const signed long pos) const
{
    return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DSRDocument::getDeviceSerialNumber(OFString &value,
                                               const signed long pos) const
{
    return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DSRDocument::getSoftwareVersions(OFString &value,
                                             const signed long pos) const
{
    return getStringValueFromElement(SoftwareVersions, value, pos);
}


OFCondition DSRDocument::getSynchronizationFrameOfReferenceUID(OFString &value,
                                                               const signed long pos) const
{
    return getStringValueFromElement(SynchronizationFrameOfReferenceUID, value, pos);
}


OFCondition DSRDocument::getSynchronizationTrigger(OFString &value,
                                                   const signed long pos) const
{
    return getStringValueFromElement(SynchronizationTrigger, value, pos);
}


OFCondition DSRDocument::getAcquisitionTimeSynchronized(OFString &value,
                                                        const signed long pos) const
{
    return getStringValueFromElement(AcquisitionTimeSynchronized, value, pos);
}


OFCondition DSRDocument::getStudyDate(OFString &value,
                                      const signed long pos) const
{
    return getStringValueFromElement(StudyDate, value, pos);
}


OFCondition DSRDocument::getStudyTime(OFString &value,
                                      const signed long pos) const
{
    return getStringValueFromElement(StudyTime, value, pos);
}


OFCondition DSRDocument::getSeriesDate(OFString &value,
                                       const signed long pos) const
{
    return getStringValueFromElement(SeriesDate, value, pos);
}


OFCondition DSRDocument::getSeriesTime(OFString &value,
                                       const signed long pos) const
{
    return getStringValueFromElement(SeriesTime, value, pos);
}


OFCondition DSRDocument::getInstanceCreationDate(OFString &value,
                                                 const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationDate, value, pos);
}


OFCondition DSRDocument::getInstanceCreationTime(OFString &value,
                                                 const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationTime, value, pos);
}


OFCondition DSRDocument::getContentDate(OFString &value,
                                        const signed long pos) const
{
    return getStringValueFromElement(ContentDate, value, pos);
}


OFCondition DSRDocument::getContentTime(OFString &value,
                                        const signed long pos) const
{
    return getStringValueFromElement(ContentTime, value, pos);
}


OFCondition DSRDocument::getStudyID(OFString &value,
                                    const signed long pos) const
{
    return getStringValueFromElement(StudyID, value, pos);
}


OFCondition DSRDocument::getPatientID(OFString &value,
                                      const signed long pos) const
{
    return getStringValueFromElement(PatientID, value, pos);
}


OFCondition DSRDocument::getIssuerOfPatientID(OFString &value,
                                              const signed long pos) const
{
    return getStringValueFromElement(IssuerOfPatientID, value, pos);
}


OFCondition DSRDocument::getSeriesNumber(OFString &value,
                                         const signed long pos) const
{
    return getStringValueFromElement(SeriesNumber, value, pos);
}


OFCondition DSRDocument::getInstanceNumber(OFString &value,
                                           const signed long pos) const
{
    return getStringValueFromElement(InstanceNumber, value, pos);
}


OFCondition DSRDocument::getAccessionNumber(OFString &value,
                                            const signed long pos) const
{
    return getStringValueFromElement(AccessionNumber, value, pos);
}


// --- set attributes ---

OFCondition DSRDocument::setSpecificCharacterSet(const OFString &value,
                                                 const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
    {
        /* try to map the given string to an enum value */
        SpecificCharacterSetEnum = definedTermToCharacterSet(value);
        if (SpecificCharacterSetEnum == CS_unknown)
            DCMSR_WARN("Setting unknown/unsupported value for Specific Character Set: " << value);
        /* in any case, store the passed string value */
        result = SpecificCharacterSet.putOFStringArray(value);
    }
    return result;
}


OFCondition DSRDocument::setCompletionFlagDescription(const OFString &value,
                                                      const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* not applicable to Key Object Selection Documents */
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
        if (check)
            result = DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet());
        if (result.good())
            result = CompletionFlagDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DSRDocument::setTimezoneOffsetFromUTC(const OFString &value,
                                                  const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = TimezoneOffsetFromUTC.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setPatientName(const OFString &value,
                                        const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = PatientName.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setPatientBirthDate(const OFString &value,
                                             const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthDate.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setPatientSex(const OFString &value,
                                       const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSex.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setReferringPhysicianName(const OFString &value,
                                                   const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = ReferringPhysicianName.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setStudyDescription(const OFString &value,
                                             const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = StudyDescription.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setSeriesDescription(const OFString &value,
                                              const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = SeriesDescription.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setProtocolName(const OFString &value,
                                         const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = ProtocolName.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setManufacturer(const OFString &value,
                                         const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = Manufacturer.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setManufacturerModelName(const OFString &value,
                                                  const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = ManufacturerModelName.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setDeviceSerialNumber(const OFString &value,
                                               const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = DeviceSerialNumber.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setSoftwareVersions(const OFString &value,
                                             const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = SoftwareVersions.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setSynchronizationFrameOfReferenceUID(const OFString &value,
                                                               const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SynchronizationFrameOfReferenceUID.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setSynchronizationTrigger(const OFString &value,
                                                   const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SynchronizationTrigger.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setAcquisitionTimeSynchronized(const OFString &value,
                                                        const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionTimeSynchronized.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setContentDate(const OFString &value,
                                        const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentDate.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setContentTime(const OFString &value,
                                        const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentTime.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setStudyDate(const OFString &value,
                                      const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDate.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setStudyTime(const OFString &value,
                                      const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyTime.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setSeriesDate(const OFString &value,
                                       const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesDate.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setSeriesTime(const OFString &value,
                                       const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesTime.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setStudyID(const OFString &value,
                                    const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = StudyID.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setPatientID(const OFString &value,
                                      const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = PatientID.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setIssuerOfPatientID(const OFString &value,
                                              const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = IssuerOfPatientID.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setSeriesNumber(const OFString &value,
                                         const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesNumber.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setInstanceNumber(const OFString &value,
                                           const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceNumber.putOFStringArray(value);
    return result;
}


OFCondition DSRDocument::setAccessionNumber(const OFString &value,
                                            const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1", getSpecificCharacterSet()) : EC_Normal;
    if (result.good())
        result = AccessionNumber.putOFStringArray(value);
    return result;
}


// --- document management functions

void DSRDocument::createNewStudy()
{
    /* clear all study-related attributes */
    StudyInstanceUID.clear();
    StudyDate.clear();
    StudyTime.clear();
    ReferringPhysicianName.clear();
    StudyID.clear();
    AccessionNumber.clear();
    StudyDescription.clear();
    /* also creates new study (since UID is empty) and SOP instance */
    createNewSeries();
}


void DSRDocument::createNewSeries()
{
    /* clear all series-related attributes */
    SeriesInstanceUID.clear();
    SeriesNumber.clear();
    SeriesDate.clear();
    SeriesTime.clear();
    SeriesDescription.clear();
    /* also creates new series (since UID is empty) */
    createNewSOPInstance();
}


OFCondition DSRDocument::createNewSeriesInStudy(const OFString &studyUID,
                                                const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    if (!studyUID.empty())
    {
        /* check parameter for conformance with VR and VM (if needed) */
        result = (check) ? DcmUniqueIdentifier::checkStringValue(studyUID, "1") : EC_Normal;
        if (result.good())
        {
            StudyInstanceUID.putOFStringArray(studyUID);
            /* also creates new SOP instance */
            createNewSeries();
        }
    }
    return result;
}


void DSRDocument::createNewSOPInstance()
{
    SOPInstanceUID.clear();
    /* reset FinalizedFlag */
    FinalizedFlag = OFFalse;
    /* update all DICOM attributes (incl. empty UIDs) */
    updateAttributes();
}


OFCondition DSRDocument::createNewDocument()
{
    /* create new document with the same type as the current one */
    return createNewDocument(getDocumentType());
}


OFCondition DSRDocument::createNewDocument(const E_DocumentType documentType)
{
    /* document type is stored only once (namely in the document tree) */
    OFCondition result = DocumentTree.changeDocumentType(documentType, OFTrue /*deleteTree*/);
    if (result.good())
    {
        /* clear object (all member variables) */
        clear();
        /* set initial values for a new SOP instance */
        createNewSOPInstance();
    }
    return result;
}


OFCondition DSRDocument::changeDocumentType(const E_DocumentType documentType)
{
    /* document type is stored only once (namely in the document tree) */
    OFCondition result = DocumentTree.changeDocumentType(documentType, OFFalse /*deleteTree*/);
    if (result.good())
    {
        /* update IOD-specific DICOM attributes */
        updateAttributes(OFFalse /*updateAll*/);
    }
    return result;
}


OFCondition DSRDocument::createRevisedVersion(const OFBool clearList)
{
    OFCondition result = EC_IllegalCall;
    /* not applicable to Key Object Selection Documents */
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
        /* check whether document is already completed */
        if (CompletionFlagEnum == CF_Complete)
        {
            if (clearList)
                PredecessorDocuments.clear();
            /* add current document */
            OFString studyUID, seriesUID, classUID, instanceUID;
            result = PredecessorDocuments.addItem(getStringValueFromElement(StudyInstanceUID, studyUID),
                                                  getStringValueFromElement(SeriesInstanceUID, seriesUID),
                                                  getStringValueFromElement(SOPClassUID, classUID),
                                                  getStringValueFromElement(SOPInstanceUID, instanceUID));
            if (result.good())
            {
                IdenticalDocuments.clear();
                /* reset completion flag, delete description */
                CompletionFlagEnum = CF_invalid;
                CompletionFlagDescription.clear();
                /* clear content date/time, will be set automatically in updateAttributes() */
                ContentDate.clear();
                ContentTime.clear();
                /* clear list of verifying observers and set flag to UNVERIFIED */
                removeVerification();
                /* remove digital signatures from document tree */
                DocumentTree.removeSignatures();
                /* create new instance UID, update creation date/time and reset finalized flag */
                createNewSOPInstance();
            }
        }
    }
    return result;
}


OFCondition DSRDocument::completeDocument()
{
    /* complete document with empty/absent completion description */
    return completeDocument("");
}


OFCondition DSRDocument::completeDocument(const OFString &description,
                                          const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* not applicable to Key Object Selection Documents */
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
        /* if document is not already completed */
        if (CompletionFlagEnum != CF_Complete)
        {
            /* check parameter for conformance with VR and VM (if needed) */
            result = (check) ? DcmLongString::checkStringValue(description, "1", getSpecificCharacterSet()) : EC_Normal;
            if (result.good())
            {
                /* completed for now and ever */
                CompletionFlagEnum = CF_Complete;
                /* completion flag description */
                setCompletionFlagDescription(description);
            }
        }
    }
    return result;
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const OFString &organization,
                                        const OFString &dateTime,
                                        const OFBool check)
{
    /* empty CodedEntryValue */
    return verifyDocument(observerName, DSRCodedEntryValue() /*dummy*/, organization, dateTime, check);
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const DSRCodedEntryValue &observerCode,
                                        const OFString &organization,
                                        const OFString &dateTime,
                                        const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* not applicable to Key Object Selection Documents */
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
        /* verify completed documents only */
        if (CompletionFlagEnum == CF_Complete)
        {
            /* empty strings are not allowed (type 1 attributes) */
            if (!observerName.empty() && !organization.empty())
            {
                /* check parameters for conformance with VR and VM (if needed) */
                if (check)
                {
                    if (observerCode.isEmpty() || observerCode.isValid())
                        result = EC_Normal;
                    if (result.good())
                        result = DcmPersonName::checkStringValue(observerName, "1", getSpecificCharacterSet());
                    if (result.good())
                        result = DcmLongString::checkStringValue(organization, "1", getSpecificCharacterSet());
                    if (result.good())
                        result = DcmDateTime::checkStringValue(dateTime, "1");
                } else {
                    /* no checks, so everything is fine */
                    result = EC_Normal;
                }
                if (result.good())
                {
                    DcmItem *ditem = new DcmItem();
                    if (ditem != NULL)
                    {
                        /* write VerifyingObserverName */
                        putStringValueToDataset(*ditem, DCM_VerifyingObserverName, observerName);
                        /* write VerifyingObserverIdentificationCodeSequence (might be empty, type 2) */
                        observerCode.writeSequence(*ditem, DCM_VerifyingObserverIdentificationCodeSequence);
                        /* write VerifyingOrganization */
                        putStringValueToDataset(*ditem, DCM_VerifyingOrganization, organization);
                        /* write VerificationDateTime */
                        if (dateTime.empty())
                        {
                            OFString tmpString;
                            currentDateTime(tmpString);
                            putStringValueToDataset(*ditem, DCM_VerificationDateTime, tmpString);
                        } else
                            putStringValueToDataset(*ditem, DCM_VerificationDateTime, dateTime);
                        /* insert items into sequence */
                        VerifyingObserver.insert(ditem);
                        /* set VerificationFlag to VERIFIED */
                        VerificationFlagEnum = VF_Verified;
                        /* reset FinalizedFlag */
                        FinalizedFlag = OFFalse;
                    } else
                        result = EC_MemoryExhausted;
                }
            } else
                result = EC_IllegalParameter;
        }
    }
    return result;
}


void DSRDocument::removeVerification()
{
    /* clear list of verifying observers and set flag to UNVERIFIED */
    VerifyingObserver.clear();
    VerificationFlagEnum = VF_Unverified;
    /* reset FinalizedFlag */
    FinalizedFlag = OFFalse;
}


OFCondition DSRDocument::finalizeDocument()
{
    OFCondition result = EC_IllegalCall;
    /* document can only be finalized if it is already completed */
    if (CompletionFlagEnum == CF_Complete)
    {
        /* set FinalizedFlag */
        FinalizedFlag = OFTrue;
        result = EC_Normal;
    }
    return result;
}


void DSRDocument::updateAttributes(const OFBool updateAll,
                                   const OFBool verboseMode)
{
    if (verboseMode)
        DCMSR_DEBUG("Updating " << (updateAll ? "all " : "") << "DICOM header attributes");
    const E_DocumentType documentType = getDocumentType();
    /* retrieve SOP class UID from internal document type */
    SOPClassUID.putString(documentTypeToSOPClassUID(documentType));
    /* put modality string depending on document type */
    Modality.putString(documentTypeToModality(documentType));
    if (updateAll)
    {
        OFString tmpString;
        /* determine local timezone (if required) */
        if (requiresTimezoneModule(documentType) && TimezoneOffsetFromUTC.isEmpty())
        {
            DCMSR_DEBUG("  Determining local timezone for Timezone Offset From UTC");
            TimezoneOffsetFromUTC.putOFStringArray(localTimezone(tmpString));
        }

        /* create new instance number if required (type 1) */
        if (InstanceNumber.isEmpty())
            InstanceNumber.putString("1");
        /* create new series number if required (type 1) */
        if (SeriesNumber.isEmpty())
            SeriesNumber.putString("1");

        char uid[100];
        /* create new SOP instance UID if required */
        if (SOPInstanceUID.isEmpty())
        {
            if (verboseMode)
                DCMSR_DEBUG("  Generating new value for SOP Instance UID");
            SOPInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
            /* set instance creation date to current date (YYYYMMDD) */
            InstanceCreationDate.putOFStringArray(currentDate(tmpString));
            /* set instance creation time to current time (HHMMSS) */
            InstanceCreationTime.putOFStringArray(currentTime(tmpString));
            /* set instance creator UID to identify instances that have been created by this toolkit */
            InstanceCreatorUID.putString(OFFIS_INSTANCE_CREATOR_UID);
        }
        /* create new study instance UID if required */
        if (StudyInstanceUID.isEmpty())
        {
            if (verboseMode)
                DCMSR_DEBUG("  Generating new value for Study Instance UID");
            StudyInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
        }
        /* create new series instance UID if required */
        if (SeriesInstanceUID.isEmpty())
        {
            if (verboseMode)
                DCMSR_DEBUG("  Generating new value for Series Instance UID");
            SeriesInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
        }

        /* check and set content date if required */
        if (ContentDate.isEmpty())
            ContentDate.putString(getStringValueFromElement(InstanceCreationDate));
        /* check and set content time if required */
        if (ContentTime.isEmpty())
            ContentTime.putString(getStringValueFromElement(InstanceCreationTime));
    }
    if (documentType == DT_KeyObjectSelectionDocument)
    {
        /* these flags are not used for Key Object Selection Documents */
        PreliminaryFlagEnum = PF_invalid;
        CompletionFlagEnum = CF_invalid;
        VerificationFlagEnum = VF_invalid;
        PreliminaryFlag.clear();
        CompletionFlag.clear();
        CompletionFlagDescription.clear();
        VerificationFlag.clear();
        VerifyingObserver.clear();
    } else {
        /* set preliminary flag */
        PreliminaryFlag.putString(preliminaryFlagToEnumeratedValue(PreliminaryFlagEnum));
        /* check and adjust completion flag if required */
        if (CompletionFlagEnum == CF_invalid)
            CompletionFlagEnum = CF_Partial;
        CompletionFlag.putString(completionFlagToEnumeratedValue(CompletionFlagEnum));
        /* check and adjust verification flag if required */
        if (VerificationFlagEnum == VF_invalid)
            VerificationFlagEnum = VF_Unverified;
        VerificationFlag.putString(verificationFlagToEnumeratedValue(VerificationFlagEnum));
    }
}
