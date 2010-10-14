/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRDocument
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.73 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmsr/dsrxmld.h"
#include "dcmtk/dcmsr/dsrpnmtn.h"
#include "dcmtk/dcmsr/dsrdattn.h"
#include "dcmtk/dcmsr/dsrdtitn.h"
#include "dcmtk/dcmsr/dsrtimtn.h"


DSRDocument::DSRDocument(const E_DocumentType documentType)
  : DocumentTree(documentType),
    FinalizedFlag(OFFalse),
    PreliminaryFlagEnum(PF_invalid),
    CompletionFlagEnum(CF_invalid),
    VerificationFlagEnum(VF_invalid),
    SpecificCharacterSetEnum(CS_invalid),
    SOPClassUID(DCM_SOPClassUID),
    SOPInstanceUID(DCM_SOPInstanceUID),
    SpecificCharacterSet(DCM_SpecificCharacterSet),
    InstanceCreationDate(DCM_InstanceCreationDate),
    InstanceCreationTime(DCM_InstanceCreationTime),
    InstanceCreatorUID(DCM_InstanceCreatorUID),
    CodingSchemeIdentification(),
    StudyInstanceUID(DCM_StudyInstanceUID),
    StudyDate(DCM_StudyDate),
    StudyTime(DCM_StudyTime),
    ReferringPhysicianName(DCM_ReferringPhysicianName),
    StudyID(DCM_StudyID),
    AccessionNumber(DCM_AccessionNumber),
    StudyDescription(DCM_StudyDescription),
    PatientName(DCM_PatientName),
    PatientID(DCM_PatientID),
    PatientBirthDate(DCM_PatientBirthDate),
    PatientSex(DCM_PatientSex),
    Manufacturer(DCM_Manufacturer),
    ManufacturerModelName(DCM_ManufacturerModelName),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    SoftwareVersions(DCM_SoftwareVersions),
    Modality(DCM_Modality),
    SeriesInstanceUID(DCM_SeriesInstanceUID),
    SeriesNumber(DCM_SeriesNumber),
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
    PertinentOtherEvidence(DCM_PertinentOtherEvidenceSequence)
{
    /* set initial values for a new SOP instance */
    updateAttributes();
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
    SpecificCharacterSetEnum = CS_invalid;
    /* clear all DICOM attributes */
    SOPClassUID.clear();
    SOPInstanceUID.clear();
    SpecificCharacterSet.clear();
    InstanceCreationDate.clear();
    InstanceCreationTime.clear();
    InstanceCreatorUID.clear();
    CodingSchemeIdentification.clear();
    StudyInstanceUID.clear();
    StudyDate.clear();
    StudyTime.clear();
    ReferringPhysicianName.clear();
    StudyID.clear();
    AccessionNumber.clear();
    StudyDescription.clear();
    PatientName.clear();
    PatientID.clear();
    PatientBirthDate.clear();
    PatientSex.clear();
    Manufacturer.clear();
    ManufacturerModelName.clear();
    DeviceSerialNumber.clear();
    SoftwareVersions.clear();
    Modality.clear();
    SeriesInstanceUID.clear();
    SeriesNumber.clear();
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
        OFString tmpString;
        /* update only some DICOM attributes */
        updateAttributes(OFFalse /*updateAll*/);

        // --- print some general document information ---

        if (!(flags & PF_printNoDocumentHeader))
        {
            /* document type/title */
            stream << documentTypeToDocumentTitle(getDocumentType(), tmpString) << OFendl << OFendl;
            /* patient related information */
            if (!PatientName.isEmpty())
            {
                stream << "Patient             : " << getPrintStringFromElement(PatientName, tmpString);
                OFString patientStr;
                if (!PatientSex.isEmpty())
                    patientStr += getPrintStringFromElement(PatientSex, tmpString);
                if (!PatientBirthDate.isEmpty())
                {
                   if (!patientStr.empty())
                       patientStr += ", ";
                   patientStr += getPrintStringFromElement(PatientBirthDate, tmpString);
                }
                if (!PatientID.isEmpty())
                {
                   if (!patientStr.empty())
                       patientStr += ", ";
                   patientStr += '#';
                   patientStr += getPrintStringFromElement(PatientID, tmpString);
                }
                if (!patientStr.empty())
                    stream << " (" << patientStr << ")";
                stream << OFendl;
            }
            /* referring physician */
            if (!ReferringPhysicianName.isEmpty())
                stream << "Referring Physician : " << getPrintStringFromElement(ReferringPhysicianName, tmpString) << OFendl;
            /* study related information  */
            if (!StudyDescription.isEmpty())
            {
                stream << "Study               : " << getPrintStringFromElement(StudyDescription, tmpString);
                if (!StudyID.isEmpty())
                    stream << " (#" << getPrintStringFromElement(StudyID, tmpString) << ")";
                stream << OFendl;
            }
            /* manufacturer and device */
            if (!Manufacturer.isEmpty())
            {
                stream << "Manufacturer        : " << getPrintStringFromElement(Manufacturer, tmpString);
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
                stream << OFendl;
            }
            /* Key Object Selection Documents do not contain the SR Document General Module */
            if (getDocumentType() != DT_KeyObjectSelectionDocument)
            {
                /* preliminary flag */
                if (!PreliminaryFlag.isEmpty())
                    stream << "Preliminary Flag    : " << getStringValueFromElement(PreliminaryFlag, tmpString) << OFendl;
                /* completion flag */
                stream << "Completion Flag     : " << getStringValueFromElement(CompletionFlag, tmpString) << OFendl;
                if (!CompletionFlagDescription.isEmpty())
                    stream << "                      " << getPrintStringFromElement(CompletionFlagDescription, tmpString) << OFendl;
                /* predecessor documents */
                if (!PredecessorDocuments.empty())
                    stream << "Predecessor Docs    : " << PredecessorDocuments.getNumberOfInstances() << OFendl;
            }
            /* identical documents */
            if (!IdenticalDocuments.empty())
                stream << "Identical Docs      : " << IdenticalDocuments.getNumberOfInstances() << OFendl;
            if (getDocumentType() != DT_KeyObjectSelectionDocument)
            {
                /* verification flag */
                stream << "Verification Flag   : " << getStringValueFromElement(VerificationFlag, tmpString) << OFendl;
                /* verifying observer */
                const size_t obsCount = getNumberOfVerifyingObservers();
                for (size_t i = 1; i <= obsCount; i++)
                {
                    OFString dateTime, obsName, organization;
                    DSRCodedEntryValue obsCode;
                    if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization).good())
                    {
                        if (i == 1)
                            stream << "Verifying Observers : " << dateTime << ": " << obsName;
                        else
                            stream << "                      " << dateTime << ": " << obsName;
                        if (obsCode.isValid())
                        {
                            stream << " ";
                            obsCode.print(stream, (flags & PF_printAllCodes) > 0 /*printCodeValue*/);
                        }
                        stream << ", " << organization << OFendl;
                    }
                }
            }
            /* content date and time */
            if (!ContentDate.isEmpty() && !ContentTime.isEmpty())
            {
                stream << "Content Date/Time   : " << getPrintStringFromElement(ContentDate, tmpString) << " ";
                stream <<                             getPrintStringFromElement(ContentTime, tmpString) << OFendl;
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
    /* re-initialize SR document */
    clear();
    /* check SOP class UID and modality first */
    result = checkDatasetForReading(dataset, documentType);
    /* dataset is OK */
    if (result.good())
    {
        OFCondition searchCond = EC_Normal;
        OFCondition obsSearchCond = EC_Normal;

        /* type 3 element and attributes which have already been checked are not checked */

        // --- SOP Common Module ---
        getElementFromDataset(dataset, SOPClassUID);   /* already checked */
        getAndCheckElementFromDataset(dataset, SOPInstanceUID, "1", "1", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SpecificCharacterSet, "1-n", "1C", "SOPCommonModule");
        if (SpecificCharacterSet.getVM() > 1)
            DCMSR_WARN("Multiple values for Specific Character Set are not supported");
        getAndCheckElementFromDataset(dataset, InstanceCreationDate, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreationTime, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreatorUID, "1", "3", "SOPCommonModule");
        CodingSchemeIdentification.read(dataset);

        // --- General Study Module ---
        getAndCheckElementFromDataset(dataset, StudyInstanceUID, "1", "1", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyDate, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyTime, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, ReferringPhysicianName, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyID, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, AccessionNumber, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyDescription, "1", "3", "GeneralStudyModule");

        // --- Patient Module ---
        getAndCheckElementFromDataset(dataset, PatientName, "1", "2", "PatientModule");
        getAndCheckElementFromDataset(dataset, PatientID, "1", "2", "PatientModule");
        getAndCheckElementFromDataset(dataset, PatientBirthDate, "1", "2", "PatientModule");
        getAndCheckElementFromDataset(dataset, PatientSex, "1", "2", "PatientModule");

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

        // --- SR Document Series Module / Key Object Document Series Module ---
        getElementFromDataset(dataset, Modality);   /* already checked */
        if (documentType == DT_KeyObjectSelectionDocument)
        {
            getAndCheckElementFromDataset(dataset, SeriesInstanceUID, "1", "1", "KeyObjectDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesNumber, "1", "1", "KeyObjectDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesDescription, "1", "3", "KeyObjectDocumentSeriesModule");
            /* need to check sequence in two steps (avoids additional getAndCheck... method) */
            searchCond = getElementFromDataset(dataset, ReferencedPerformedProcedureStep);
            checkElementValue(ReferencedPerformedProcedureStep, "1", "2", searchCond, "KeyObjectDocumentSeriesModule");
        } else {
            getAndCheckElementFromDataset(dataset, SeriesInstanceUID, "1", "1", "SRDocumentSeriesModule");
            getAndCheckElementFromDataset(dataset, SeriesNumber, "1", "1", "SRDocumentSeriesModule");
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
            PredecessorDocuments.read(dataset);
            /* need to check sequence in two steps (avoids additional getAndCheck... method) */
            searchCond = getElementFromDataset(dataset, PerformedProcedureCode);
            checkElementValue(PerformedProcedureCode, "1", "2", searchCond, "SRDocumentGeneralModule");
            PertinentOtherEvidence.read(dataset);
        }
        IdenticalDocuments.read(dataset);
        CurrentRequestedProcedureEvidence.read(dataset);
        /* remove possible signature sequences */
        removeAttributeFromSequence(VerifyingObserver, DCM_MACParametersSequence);
        removeAttributeFromSequence(VerifyingObserver, DCM_DigitalSignaturesSequence);
        removeAttributeFromSequence(PerformedProcedureCode, DCM_MACParametersSequence);
        removeAttributeFromSequence(PerformedProcedureCode, DCM_DigitalSignaturesSequence);

        /* update internal enumerated values and perform additional checks */
        OFString tmpString;
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
            /* get and check VerificationFlag / VerifyingObserverSequence */
            VerificationFlagEnum = enumeratedValueToVerificationFlag(getStringValueFromElement(VerificationFlag, tmpString));
            if (VerificationFlagEnum == VF_invalid)
                printUnknownValueWarningMessage("VerificationFlag", tmpString.c_str());
            else if (VerificationFlagEnum == VF_Verified)
                checkElementValue(VerifyingObserver, "1-n", "1", obsSearchCond, "SRDocumentGeneralModule");
        }
        SpecificCharacterSetEnum = definedTermToCharacterSet(getStringValueFromElement(SpecificCharacterSet, tmpString));
        /* check SpecificCharacterSet */
        if ((SpecificCharacterSetEnum == CS_invalid) && !tmpString.empty())
            printUnknownValueWarningMessage("SpecificCharacterSet", tmpString.c_str());

        /* read SR document tree */
        if (result.good())
            result = DocumentTree.read(dataset, documentType, flags);
    }
    return result;
}


OFCondition DSRDocument::write(DcmItem &dataset,
                               DcmStack *markedItems)
{
    OFCondition result = EC_Normal;
    /* only write valid documents */
    if (isValid())
    {
        /* update all DICOM attributes */
        updateAttributes();

        /* write general document attributes */

        // --- SOP Common Module ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPClassUID), "1", "1", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPInstanceUID), "1", "1", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmCodeString(SpecificCharacterSet), "1-n", "1C", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmDate(InstanceCreationDate), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmTime(InstanceCreationTime), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(InstanceCreatorUID), "1", "3", "SOPCommonModule");
        CodingSchemeIdentification.write(dataset);

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

        // --- SR Document Series Module / Key Object Document Series Module ---
        if (getDocumentType() == DT_KeyObjectSelectionDocument)
        {
            addElementToDataset(result, dataset, new DcmCodeString(Modality), "1", "1", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(SeriesInstanceUID), "1", "1", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmIntegerString(SeriesNumber), "1", "1", "KeyObjectDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmLongString(SeriesDescription), "1", "3", "KeyObjectDocumentSeriesModule");
            /* always write empty sequence since not yet fully supported */
            ReferencedPerformedProcedureStep.clear();
            addElementToDataset(result, dataset, new DcmSequenceOfItems(ReferencedPerformedProcedureStep), "1", "2", "KeyObjectDocumentSeriesModule");
        } else {
            addElementToDataset(result, dataset, new DcmCodeString(Modality), "1", "1", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(SeriesInstanceUID), "1", "1", "SRDocumentSeriesModule");
            addElementToDataset(result, dataset, new DcmIntegerString(SeriesNumber), "1", "1", "SRDocumentSeriesModule");
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
            PredecessorDocuments.write(dataset);        /* optional */
            /* always write empty sequence since not yet fully supported */
            PerformedProcedureCode.clear();
            addElementToDataset(result, dataset, new DcmSequenceOfItems(PerformedProcedureCode), "1", "2", "SRDocumentGeneralModule");
            if (result.good())
                result = PertinentOtherEvidence.write(dataset);
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
                    /* create new document of specified type (also checks for support) */
                    result = createNewDocument(sopClassUIDToDocumentType(getSOPClassUID()));
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
                    const char *encString = characterSetToXMLName(SpecificCharacterSetEnum);
                    if ((strcmp(encString, "?") == 0) || doc.setEncodingHandler(encString).bad())
                        DCMSR_WARN("Character set '" << tmpString << "' not supported");
                } else {
                    /* only one "charset" node allowed */
                    doc.printUnexpectedNodeWarning(cursor);
                }
            }
            else if (doc.matchNode(cursor, "modality"))
            {
                OFString tmpString;
                /* compare the XML node content */
                if (doc.getStringFromNodeContent(cursor, tmpString) != documentTypeToModality(getDocumentType()))
                    DCMSR_WARN("Invalid value for 'modality' ... ignoring");
            }
            else if (doc.matchNode(cursor, "referringphysician"))
            {
                /* goto sub-element "name" */
                const DSRXMLCursor childNode = doc.getNamedNode(cursor.getChild(), "name");
                if (childNode.valid())
                {
                    /* Referring Physician's Name */
                    OFString tmpString;
                    DSRPNameTreeNode::getValueFromXMLNodeContent(doc, childNode.getChild(), tmpString);
                    ReferringPhysicianName.putString(tmpString.c_str());
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
            else if (doc.matchNode(cursor, "document"))
                result = readXMLDocumentData(doc, cursor.getChild(), flags);
            else if (doc.matchNode(cursor, "device"))
            {
                doc.getElementFromNodeContent(doc.getNamedNode(cursor.getChild(), "manufacturer"), Manufacturer, NULL, OFTrue /*encoding*/);
                doc.getElementFromNodeContent(doc.getNamedNode(cursor.getChild(), "model"), ManufacturerModelName, NULL, OFTrue /*encoding*/);
                doc.getElementFromNodeContent(doc.getNamedNode(cursor.getChild(), "serial", OFFalse /*required*/), DeviceSerialNumber, NULL, OFTrue /*encoding*/);
                doc.getElementFromNodeContent(doc.getNamedNode(cursor.getChild(), "version", OFFalse /*required*/), SoftwareVersions, NULL, OFTrue /*encoding*/);
            }
            else if (doc.getElementFromNodeContent(cursor, Manufacturer, "manufacturer", OFTrue /*encoding*/).bad())
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
                PatientName.putString(tmpString.c_str());
            }
            else if (doc.matchNode(cursor, "birthday"))
            {
                /* Patient's Birth Date */
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedNode(cursor.getChild(), "date"), tmpString);
                PatientBirthDate.putString(tmpString.c_str());
            }
            else if (doc.getElementFromNodeContent(cursor, PatientID, "id").bad() &&
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
                                          const size_t /*flags*/)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString tmpString;
        /* get Study Instance UID from XML attribute */
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
                doc.getElementFromNodeContent(doc.getNamedNode(cursor.getChild(), "number"), AccessionNumber);
            }
            else if (doc.matchNode(cursor, "date"))
            {
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, cursor, tmpString);
                StudyDate.putString(tmpString.c_str());
            }
            else if (doc.matchNode(cursor, "time"))
            {
                DSRTimeTreeNode::getValueFromXMLNodeContent(doc, cursor, tmpString);
                StudyTime.putString(tmpString.c_str());
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
                                           const size_t /*flags*/)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        /* get Series Instance UID from XML attribute */
        result = doc.getElementFromAttribute(cursor, SeriesInstanceUID, "uid");
        /* goto first sub-element */
        cursor.gotoChild();
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.getElementFromNodeContent(cursor, SeriesNumber, "number").bad() &&
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
                                             const size_t /*flags*/)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString tmpString;
        /* get SOP Instance UID from XML attribute */
        result = doc.getElementFromAttribute(cursor, SOPInstanceUID, "uid");
        /* goto first sub-element */
        cursor.gotoChild();
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.matchNode(cursor, "creation"))
            {
                /* Instance Creation Date */
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedNode(cursor.getChild(), "date"), tmpString);
                InstanceCreationDate.putString(tmpString.c_str());
                /* Instance Creation Time */
                DSRTimeTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedNode(cursor.getChild(), "time"), tmpString);
                InstanceCreationTime.putString(tmpString.c_str());
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
                    const DSRXMLCursor childCursor = doc.getNamedNode(cursor.getChild(), "description", OFFalse /*required*/);
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
                const DSRXMLCursor childCursor = cursor.getChild();
                /* Content Date */
                DSRDateTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedNode(childCursor, "date"), tmpString);
                ContentDate.putString(tmpString.c_str());
                /* Content Time */
                DSRTimeTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedNode(childCursor, "time"), tmpString);
                ContentTime.putString(tmpString.c_str());
                /* proceed with document tree */
                result = DocumentTree.readXML(doc, childCursor, flags);
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
                                                      const size_t /*flags*/)
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
                    OFString datetimeString, nameString, orgaString;
                    DSRCodedEntryValue codeValue;
                    DSRXMLCursor childCursor = cursor.getChild();
                    /* iterate over all child nodes */
                    while (childCursor.valid())
                    {
                        /* check for known element tags */
                        if (doc.matchNode(childCursor, "code"))
                        {
                            /* Verifying Observer Code */
                            codeValue.readXML(doc, childCursor);
                        }
                        else if (doc.matchNode(childCursor, "name"))
                        {
                            /* Verifying Observer Name */
                            DSRPNameTreeNode::getValueFromXMLNodeContent(doc, childCursor.getChild(), nameString);
                        }
                        else if (doc.matchNode(childCursor, "datetime"))
                        {
                            /* Verification Datetime */
                            DSRDateTimeTreeNode::getValueFromXMLNodeContent(doc, childCursor, datetimeString);
                        } else {
                            /* Verifying Observer Organization */
                            doc.getStringFromNodeContent(childCursor, orgaString, "organization", OFTrue /*encoding*/, OFFalse /*clearString*/);
                        }
                        /* proceed with next node */
                        childCursor.gotoNext();
                    }
                    /* put string values into the sequence item */
                    putStringValueToDataset(*ditem, DCM_VerificationDateTime, datetimeString);
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
        /* used for multiple purposes */
        OFString tmpString;
        /* update DICOM attributes */
        updateAttributes();

        // --- XML document structure (start) ---

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
        /* retrieve name of SOP class */
        const char *sopClass = dcmFindNameOfUID(tmpString.c_str());
        if (sopClass != NULL)
            stream << sopClass;
        stream << "</sopclass>" << OFendl;
        writeStringFromElementToXML(stream, SpecificCharacterSet, "charset", (flags & XF_writeEmptyTags) > 0);
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

        if ((flags & XF_writeEmptyTags) || !ReferringPhysicianName.isEmpty())
        {
            stream << "<referringphysician>" << OFendl;
            writeStringFromElementToXML(stream, ReferringPhysicianName, "name", (flags & XF_writeEmptyTags) > 0);
            stream << "</referringphysician>" << OFendl;
        }

        stream << "<patient>" << OFendl;
        writeStringFromElementToXML(stream, PatientID, "id", (flags & XF_writeEmptyTags) > 0);
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

        if ((flags & XF_writeEmptyTags) || !CodingSchemeIdentification.empty())
        {
            stream << "<coding>" << OFendl;
            CodingSchemeIdentification.writeXML(stream, flags);
            stream << "</coding>" << OFendl;
        }
        if ((flags & XF_writeEmptyTags) || !CurrentRequestedProcedureEvidence.empty())
        {
            stream << "<evidence type=\"Current Requested Procedure\">" << OFendl;
            CurrentRequestedProcedureEvidence.writeXML(stream, flags);
            stream << "</evidence>" << OFendl;
        }
        if (getDocumentType() != DT_KeyObjectSelectionDocument)
        {
            if ((flags & XF_writeEmptyTags) || !PertinentOtherEvidence.empty())
            {
                stream << "<evidence type=\"Pertinent Other\">" << OFendl;
                PertinentOtherEvidence.writeXML(stream, flags);
                stream << "</evidence>" << OFendl;
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
                    DcmDateTime::getISOFormattedDateTimeFromString(dateTime, tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/,
                        OFFalse /*timeZone*/, OFFalse /*createMissingPart*/, "T" /*dateTimeSeparator*/);
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

            if ((flags & XF_writeEmptyTags) || !PredecessorDocuments.empty())
            {
                stream << "<predecessor>" << OFendl;
                PredecessorDocuments.writeXML(stream, flags);
                stream << "</predecessor>" << OFendl;
            }
        }
        if ((flags & XF_writeEmptyTags) || !IdenticalDocuments.empty())
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
    }
    if (!patientStr.empty())
        stream << " (" << patientStr << ")";
}


void DSRDocument::renderHTMLReferenceList(STD_NAMESPACE ostream &stream,
                                          DSRSOPInstanceReferenceList &refList,
                                          const size_t /*flags*/)
{
    /* goto first list item (if not empty) */
    if (refList.gotoFirstItem().good())
    {
        OFString tmpString;
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
                stream << documentTypeToDocumentTitle(sopClassUIDToDocumentType(sopClass), tmpString);
                stream << "</a></td>" << OFendl;
            } else
                stream << "<td><i>invalid document reference</i></td>" << OFendl;
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
        updateAttributes(OFFalse /* updateAll */);

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
            /* study related information */
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
                if (!PredecessorDocuments.empty())
                {
                    stream << "<tr>" << OFendl;
                    stream << "<td><b>Predecessor Docs:</b></td>" << OFendl;
                    renderHTMLReferenceList(stream, PredecessorDocuments, flags);
                    stream << "</tr>" << OFendl;
                }
            }
            /* identical documents */
            if (!IdenticalDocuments.empty())
            {
                stream << "<tr>" << OFendl;
                stream << "<td><b>Identical Docs:</b></td>" << OFendl;
                renderHTMLReferenceList(stream, IdenticalDocuments, flags);
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


DSRTypes::E_CompletionFlag DSRDocument::getCompletionFlag() const
{
    return CompletionFlagEnum;
}


const char *DSRDocument::getCompletionFlagDescription() const
{
    return getStringValueFromElement(CompletionFlagDescription);
}


const OFString &DSRDocument::getCompletionFlagDescription(OFString &description) const
{
    return getStringValueFromElement(CompletionFlagDescription, description);
}


DSRTypes::E_VerificationFlag DSRDocument::getVerificationFlag() const
{
    return VerificationFlagEnum;
}


size_t DSRDocument::getNumberOfVerifyingObservers()
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
    return PredecessorDocuments;
}


DSRSOPInstanceReferenceList &DSRDocument::getIdenticalDocuments()
{
    return IdenticalDocuments;
}


DSRSOPInstanceReferenceList &DSRDocument::getCurrentRequestedProcedureEvidence()
{
    return CurrentRequestedProcedureEvidence;
}


DSRSOPInstanceReferenceList &DSRDocument::getPertinentOtherEvidence()
{
    return PertinentOtherEvidence;
}


DSRCodingSchemeIdentificationList &DSRDocument::getCodingSchemeIdentification()
{
    return CodingSchemeIdentification;
}


// --- get attributes (C string) ---

const char *DSRDocument::getModality() const
{
    return getStringValueFromElement(Modality);
}


const char *DSRDocument::getSOPClassUID() const
{
    return getStringValueFromElement(SOPClassUID);
}


const char *DSRDocument::getStudyInstanceUID() const
{
    return getStringValueFromElement(StudyInstanceUID);
}


const char *DSRDocument::getSeriesInstanceUID() const
{
    return getStringValueFromElement(SeriesInstanceUID);
}


const char *DSRDocument::getSOPInstanceUID() const
{
    return getStringValueFromElement(SOPInstanceUID);
}


const char *DSRDocument::getInstanceCreatorUID() const
{
    return getStringValueFromElement(InstanceCreatorUID);
}


const char *DSRDocument::getSpecificCharacterSet() const
{
    return getStringValueFromElement(SpecificCharacterSet);
}


const char *DSRDocument::getPatientName() const
{
    return getStringValueFromElement(PatientName);
}


const char *DSRDocument::getPatientBirthDate() const
{
    return getStringValueFromElement(PatientBirthDate);
}


const char *DSRDocument::getPatientSex() const
{
    return getStringValueFromElement(PatientSex);
}


const char *DSRDocument::getReferringPhysicianName() const
{
    return getStringValueFromElement(ReferringPhysicianName);
}


const char *DSRDocument::getStudyDescription() const
{
    return getStringValueFromElement(StudyDescription);
}


const char *DSRDocument::getSeriesDescription() const
{
    return getStringValueFromElement(SeriesDescription);
}


const char *DSRDocument::getManufacturer() const
{
    return getStringValueFromElement(Manufacturer);
}


const char *DSRDocument::getManufacturerModelName() const
{
    return getStringValueFromElement(ManufacturerModelName);
}


const char *DSRDocument::getDeviceSerialNumber() const
{
    return getStringValueFromElement(DeviceSerialNumber);
}


const char *DSRDocument::getSoftwareVersions() const
{
    return getStringValueFromElement(SoftwareVersions);
}


const char *DSRDocument::getStudyDate() const
{
    return getStringValueFromElement(StudyDate);
}


const char *DSRDocument::getStudyTime() const
{
    return getStringValueFromElement(StudyTime);
}


const char *DSRDocument::getInstanceCreationDate() const
{
    return getStringValueFromElement(InstanceCreationDate);
}


const char *DSRDocument::getInstanceCreationTime() const
{
    return getStringValueFromElement(InstanceCreationTime);
}


const char *DSRDocument::getContentDate() const
{
    return getStringValueFromElement(ContentDate);
}


const char *DSRDocument::getContentTime() const
{
    return getStringValueFromElement(ContentTime);
}


const char *DSRDocument::getStudyID() const
{
    return getStringValueFromElement(StudyID);
}


const char *DSRDocument::getPatientID() const
{
    return getStringValueFromElement(PatientID);
}


const char *DSRDocument::getSeriesNumber() const
{
    return getStringValueFromElement(SeriesNumber);
}


const char *DSRDocument::getInstanceNumber() const
{
    return getStringValueFromElement(InstanceNumber);
}


const char *DSRDocument::getAccessionNumber() const
{
    return getStringValueFromElement(AccessionNumber);
}


// --- get attributes (C++ string) ---

const OFString &DSRDocument::getModality(OFString &value) const
{
    return getStringValueFromElement(Modality, value);
}


const OFString &DSRDocument::getSOPClassUID(OFString &value) const
{
    return getStringValueFromElement(SOPClassUID, value);
}


const OFString &DSRDocument::getStudyInstanceUID(OFString &value) const
{
    return getStringValueFromElement(StudyInstanceUID, value);
}


const OFString &DSRDocument::getSeriesInstanceUID(OFString &value) const
{
    return getStringValueFromElement(SeriesInstanceUID, value);
}


const OFString &DSRDocument::getSOPInstanceUID(OFString &value) const
{
    return getStringValueFromElement(SOPInstanceUID, value);
}


const OFString &DSRDocument::getInstanceCreatorUID(OFString &value) const
{
    return getStringValueFromElement(InstanceCreatorUID, value);
}


const OFString &DSRDocument::getSpecificCharacterSet(OFString &value) const
{
    return getStringValueFromElement(SpecificCharacterSet, value);
}


const OFString &DSRDocument::getPatientName(OFString &value) const
{
    return getStringValueFromElement(PatientName, value);
}


const OFString &DSRDocument::getPatientBirthDate(OFString &value) const
{
    return getStringValueFromElement(PatientBirthDate, value);
}


const OFString &DSRDocument::getPatientSex(OFString &value) const
{
    return getStringValueFromElement(PatientSex, value);
}


const OFString &DSRDocument::getReferringPhysicianName(OFString &value) const
{
    return getStringValueFromElement(ReferringPhysicianName, value);
}


const OFString &DSRDocument::getStudyDescription(OFString &value) const
{
    return getStringValueFromElement(StudyDescription, value);
}


const OFString &DSRDocument::getSeriesDescription(OFString &value) const
{
    return getStringValueFromElement(SeriesDescription, value);
}


const OFString &DSRDocument::getManufacturer(OFString &value) const
{
    return getStringValueFromElement(Manufacturer, value);
}


const OFString &DSRDocument::getManufacturerModelName(OFString &value) const
{
    return getStringValueFromElement(ManufacturerModelName, value);
}


const OFString &DSRDocument::getDeviceSerialNumber(OFString &value) const
{
    return getStringValueFromElement(DeviceSerialNumber, value);
}


const OFString &DSRDocument::getSoftwareVersions(OFString &value) const
{
    return getStringValueFromElement(SoftwareVersions, value);
}


const OFString &DSRDocument::getStudyDate(OFString &value) const
{
    return getStringValueFromElement(StudyDate, value);
}


const OFString &DSRDocument::getStudyTime(OFString &value) const
{
    return getStringValueFromElement(StudyTime, value);
}


const OFString &DSRDocument::getInstanceCreationDate(OFString &value) const
{
    return getStringValueFromElement(InstanceCreationDate, value);
}


const OFString &DSRDocument::getInstanceCreationTime(OFString &value) const
{
    return getStringValueFromElement(InstanceCreationTime, value);
}


const OFString &DSRDocument::getContentDate(OFString &value) const
{
    return getStringValueFromElement(ContentDate, value);
}


const OFString &DSRDocument::getContentTime(OFString &value) const
{
    return getStringValueFromElement(ContentTime, value);
}


const OFString &DSRDocument::getStudyID(OFString &value) const
{
    return getStringValueFromElement(StudyID, value);
}


const OFString &DSRDocument::getPatientID(OFString &value) const
{
    return getStringValueFromElement(PatientID, value);
}


const OFString &DSRDocument::getSeriesNumber(OFString &value) const
{
    return getStringValueFromElement(SeriesNumber, value);
}


const OFString &DSRDocument::getInstanceNumber(OFString &value) const
{
    return getStringValueFromElement(InstanceNumber, value);
}


const OFString &DSRDocument::getAccessionNumber(OFString &value) const
{
    return getStringValueFromElement(AccessionNumber, value);
}


// --- set attributes ---

OFCondition DSRDocument::setSpecificCharacterSet(const OFString &value)
{
    SpecificCharacterSetEnum = definedTermToCharacterSet(value);
    /* might add check for correct format (VR) later on */
    return SpecificCharacterSet.putString(value.c_str());
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


OFCondition DSRDocument::setCompletionFlagDescription(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* not applicable to Key Object Selection Documents */
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
        if (value.empty())
        {
            CompletionFlagDescription.clear();
            result = EC_Normal;
        } else
            result = CompletionFlagDescription.putString(value.c_str());
    }
    return result;
}


OFCondition DSRDocument::setPatientName(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return PatientName.putString(value.c_str());
}


OFCondition DSRDocument::setPatientBirthDate(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return PatientBirthDate.putString(value.c_str());
}


OFCondition DSRDocument::setPatientSex(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return PatientSex.putString(value.c_str());
}


OFCondition DSRDocument::setReferringPhysicianName(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return ReferringPhysicianName.putString(value.c_str());
}


OFCondition DSRDocument::setStudyDescription(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return StudyDescription.putString(value.c_str());
}


OFCondition DSRDocument::setSeriesDescription(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return SeriesDescription.putString(value.c_str());
}


OFCondition DSRDocument::setManufacturer(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return Manufacturer.putString(value.c_str());
}


OFCondition DSRDocument::setManufacturerModelName(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return ManufacturerModelName.putString(value.c_str());
}


OFCondition DSRDocument::setDeviceSerialNumber(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return DeviceSerialNumber.putString(value.c_str());
}


OFCondition DSRDocument::setSoftwareVersions(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return SoftwareVersions.putString(value.c_str());
}


OFCondition DSRDocument::setContentDate(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return ContentDate.putString(value.c_str());
}


OFCondition DSRDocument::setContentTime(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return ContentTime.putString(value.c_str());
}


OFCondition DSRDocument::setStudyID(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return StudyID.putString(value.c_str());
}


OFCondition DSRDocument::setPatientID(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return PatientID.putString(value.c_str());
}


OFCondition DSRDocument::setSeriesNumber(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return SeriesNumber.putString(value.c_str());
}


OFCondition DSRDocument::setInstanceNumber(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return InstanceNumber.putString(value.c_str());
}


OFCondition DSRDocument::setAccessionNumber(const OFString &value)
{
    /* might add check for correct format (VR) later on */
    return AccessionNumber.putString(value.c_str());
}


// --- document management functions

void DSRDocument::createNewStudy()
{
    StudyInstanceUID.clear();
    /* also creates new study (since UID is empty) and SOP instance */
    createNewSeries();
}


void DSRDocument::createNewSeries()
{
    SeriesInstanceUID.clear();
    /* also creates new series (since UID is empty) */
    createNewSOPInstance();
}


OFCondition DSRDocument::createNewSeriesInStudy(const OFString &studyUID)
{
    OFCondition result = EC_IllegalParameter;
    if (!studyUID.empty())
    {
        StudyInstanceUID.putString(studyUID.c_str());
        /* also creates new SOP instance */
        createNewSeries();
        result = EC_Normal;
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
    OFCondition result = DocumentTree.changeDocumentType(documentType);
    if (result.good())
    {
        /* clear object (all member variables) */
        clear();
        /* set initial values for a new SOP instance */
        createNewSOPInstance();
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
            result = PredecessorDocuments.addItem(getStringValueFromElement(StudyInstanceUID),
                                                  getStringValueFromElement(SeriesInstanceUID),
                                                  getStringValueFromElement(SOPClassUID),
                                                  getStringValueFromElement(SOPInstanceUID));
            if (result.good())
            {
                IdenticalDocuments.clear();
                /* set completion flag to PARTIAL, delete description */
                CompletionFlagEnum = CF_Partial;
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


OFCondition DSRDocument::completeDocument(const OFString &description)
{
    OFCondition result = EC_IllegalCall;
    /* not applicable to Key Object Selection Documents */
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
        /* if document is not already completed */
        if (CompletionFlagEnum != CF_Complete)
        {
            /* completed for now and ever */
            CompletionFlagEnum = CF_Complete;
            /* completion flag description */
            setCompletionFlagDescription(description);
            result = EC_Normal;
        }
    }
    return result;
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const OFString &organization)
{
    /* empty CodedEntryValue and VerificationDateTime */
    return verifyDocument(observerName, DSRCodedEntryValue() /*dummy*/, organization, "" /*dateTime*/);
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const OFString &organization,
                                        const OFString &dateTime)
{
    /* empty CodedEntryValue */
    return verifyDocument(observerName, DSRCodedEntryValue() /*dummy*/, organization, dateTime);
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const DSRCodedEntryValue &observerCode,
                                        const OFString &organization)
{
    /* empty VerificationDateTime */
    return verifyDocument(observerName, observerCode, organization, "" /*dateTime*/);
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const DSRCodedEntryValue &observerCode,
                                        const OFString &organization,
                                        const OFString &dateTime)
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
                    result = EC_Normal;
                } else
                    result = EC_MemoryExhausted;
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


void DSRDocument::updateAttributes(const OFBool updateAll)
{
    if (updateAll)
    {
        /* retrieve SOP class UID from internal document type */
        SOPClassUID.putString(documentTypeToSOPClassUID(getDocumentType()));
        /* put modality string depending on document type */
        Modality.putString(documentTypeToModality(getDocumentType()));

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
            OFString tmpString;
            SOPInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
            /* set instance creation date to current date (YYYYMMDD) */
            InstanceCreationDate.putString(currentDate(tmpString).c_str());
            /* set instance creation time to current time (HHMMSS) */
            InstanceCreationTime.putString(currentTime(tmpString).c_str());
            /* set instance creator UID to identify instances that have been created by this toolkit */
            InstanceCreatorUID.putString(OFFIS_INSTANCE_CREATOR_UID);
        }
        /* create new study instance UID if required */
        if (StudyInstanceUID.isEmpty())
            StudyInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
        /* create new series instance UID if required */
        if (SeriesInstanceUID.isEmpty())
            SeriesInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));

        /* check and set content date if required */
        if (ContentDate.isEmpty())
            ContentDate.putString(getStringValueFromElement(InstanceCreationDate));
        /* check and set content time if required */
        if (ContentTime.isEmpty())
            ContentTime.putString(getStringValueFromElement(InstanceCreationTime));
    }
    if (getDocumentType() != DT_KeyObjectSelectionDocument)
    {
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


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoc.cc,v $
 *  Revision 1.73  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.72  2010-09-30 09:00:20  joergr
 *  Renamed class and enumeration related to the Key Object Selection Document.
 *
 *  Revision 1.71  2010-09-29 15:16:50  joergr
 *  Enhanced checking and reporting of standard violations in write() methods.
 *
 *  Revision 1.70  2010-09-29 10:07:41  joergr
 *  Added support for the recently introduced, optional PreliminaryFlag.
 *
 *  Revision 1.69  2010-09-29 08:32:26  joergr
 *  Used more specific "moduleName" for getAndCheckElementFromDataset() and
 *  checkElementValue().
 *
 *  Revision 1.68  2010-09-28 16:25:49  joergr
 *  Added support for Enhanced General Equipment Module which is required for
 *  both X-Ray Radiation Dose SR and Colon CAD SR.
 *  Use new isEmpty() method instead of length in order to determine whether the
 *  element value is empty (e.g. for checking the presence of type 3 attributes).
 *
 *  Revision 1.67  2010-08-09 13:26:56  joergr
 *  Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *  the official "keyword" is used for the attribute name which results in a
 *  number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *  Revision 1.66  2010-07-01 13:40:35  joergr
 *  Moved SeriesDescription (0008,103E) from General Series to SR Document Series
 *  Module (according to CP 703).
 *
 *  Revision 1.65  2009-10-14 10:49:32  joergr
 *  Fixed minor issues in log output. Also updated copyright date (if required).
 *
 *  Revision 1.64  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.63  2008-12-19 11:10:52  joergr
 *  Fixed wrong string comparison which caused a compiler error on openSUSE 11.1.
 *
 *  Revision 1.62  2008-07-17 12:00:09  joergr
 *  Replaced call to getSequenceFromDataset() by getElementFromDataset().
 *
 *  Revision 1.61  2008-04-29 13:59:21  joergr
 *  Harmonized capitalization of warning messages.
 *
 *  Revision 1.60  2008-03-11 11:10:53  joergr
 *  Fixed wrong comments.
 *
 *  Revision 1.59  2007/11/15 16:44:12  joergr
 *  Added meta header element for the generator of the HTML/XHTML document.
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.58  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.57  2006/07/25 14:20:37  joergr
 *  Added new optional flags for the HTML rendering of SR documents:
 *  HF_alwaysExpandChildrenInline, HF_useCodeDetailsTooltip and
 *  HF_renderSectionTitlesInline.
 *  Added new checking routine: The VerifyingObserverSequence is required if the
 *  value of VerificationFlag is "VERIFIED" (type 1C).
 *  Changed content and layout of document header in textual dump and HTML output,
 *  e.g. added StudyDescription and StudyID.
 *
 *  Revision 1.56  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.55  2005/12/08 15:47:48  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.54  2004/11/29 17:15:29  joergr
 *  Added warning message when character set is unknown, unsupported  or cannot
 *  be mapped to the output format. Added support for UTF-8 character set.
 *
 *  Revision 1.53  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.52  2004/04/16 13:24:22  joergr
 *  Restructured code to avoid default parameter values for "complex types" like
 *  OFString. Required for Sun CC 2.0.1.
 *
 *  Revision 1.51  2004/01/16 10:05:01  joergr
 *  Adapted XML output format of Date, Time and Datetime to XML Schema (ISO)
 *  requirements. Made readXML() more robust with regard to expected XML structure.
 *
 *  Revision 1.50  2004/01/05 14:37:23  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.49  2003/11/28 16:51:44  joergr
 *  Changed output format of CodingSchemeVersion in print() and renderHTML().
 *  Now using square brackets instead of comma to separate from CodingScheme.
 *
 *  Revision 1.48  2003/10/31 13:30:10  joergr
 *  Changed behaviour: do not output empty list of predecessor or identical
 *  documents in XML format unless flag XF_writeEmptyTags is set.
 *
 *  Revision 1.47  2003/10/27 11:25:31  joergr
 *  Made text of a warning message consistent with other messages in this module
 *  (always begin with an upper-case character).
 *
 *  Revision 1.46  2003/10/14 15:36:55  joergr
 *  Moved declaration of loop variable 'i' into the header of the for loop to get
 *  rid of warnings reported by Borland C++ 5.5.
 *
 *  Revision 1.45  2003/09/10 13:18:43  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdenticationSequence as
 *  required by CP 324.
 *
 *  Revision 1.44  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.43  2003/08/07 15:34:22  joergr
 *  Distinguish more strictly between OFBool and int (required when HAVE_CXX_BOOL
 *  is defined).
 *
 *  Revision 1.42  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.41  2003/08/07 13:27:31  joergr
 *  Added readXML functionality. Made method isFinalized() const.
 *  Added new option --add-schema-reference to command line tool dsr2xml. XML
 *  Schema not yet completed!
 *  Added new option to createRevisedVersion() which allows to keep the current
 *  list of predecessor documents.
 *  Changed interface to access the list of predecessor documents. Now using the
 *  existing class DSRSOPInstanceReferenceList.
 *  Added preliminary support for the Identical Documents Sequence.
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
 *  Follow more strictly the rules for Key Object Selection Documents (e.g.
 *  there is a special Key Object Document Series Module).
 *
 *  Revision 1.40  2003/04/01 14:59:41  joergr
 *  Added support for XML namespaces.
 *
 *  Revision 1.39  2003/01/06 10:30:28  joergr
 *  Updated URL to dcmtk homepage.
 *
 *  Revision 1.38  2002/12/05 13:52:27  joergr
 *  Added further checks when reading SR documents (e.g. value of VerificationFlag,
 *  CompletionsFlag, ContinuityOfContent and SpecificCharacterSet).
 *
 *  Revision 1.37  2002/07/22 14:22:33  joergr
 *  Added new print flag to suppress the output of general document information.
 *
 *  Revision 1.36  2002/06/20 12:18:53  meichel
 *  Adapted toolkit to name changes in latest data dictionary
 *
 *  Revision 1.35  2002/05/07 12:52:48  joergr
 *  Added support for the Current Requested Procedure Evidence Sequence and the
 *  Pertinent Other Evidence Sequence to the dcmsr module.
 *  Added output of SOP class name to XML document.
 *
 *  Revision 1.34  2002/05/02 14:08:35  joergr
 *  Added support for standard and non-standard string streams (which one is
 *  supported is detected automatically via the configure mechanism).
 *
 *  Revision 1.33  2002/04/16 13:51:37  joergr
 *  Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *  Revision 1.32  2002/01/08 10:26:49  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *  Changed prefix of UIDs created for series and studies (now using constants
 *  SITE_SERIES_UID_ROOT and SITE_STUDY_UID_ROOT which are supposed to be used
 *  in these cases).
 *
 *  Revision 1.31  2001/11/09 16:14:55  joergr
 *  Added new command line option allowing to encode codes as XML attributes
 *  (instead of tags).
 *
 *  Revision 1.30  2001/10/10 15:29:52  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.29  2001/10/02 12:07:07  joergr
 *  Adapted module "dcmsr" to the new class OFCondition. Introduced module
 *  specific error codes.
 *
 *  Revision 1.28  2001/09/28 14:14:14  joergr
 *  Added check whether ios::nocreate exists.
 *
 *  Revision 1.27  2001/09/26 13:04:19  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.26  2001/06/20 15:03:45  joergr
 *  Added minimal support for new SOP class Key Object Selection Document
 *  (suppl. 59).
 *
 *  Revision 1.25  2001/05/22 13:14:40  joergr
 *  Clear unsupported type 2 sequences before writing them to a dataset.
 *
 *  Revision 1.24  2001/04/03 08:25:20  joergr
 *  Added new command line option: ignore relationship content constraints
 *  specified for each SR document class.
 *
 *  Revision 1.23  2001/01/29 17:37:44  joergr
 *  Added methods to support a new state of finalized and unfinalized documents.
 *
 *  Revision 1.22  2001/01/25 11:49:42  joergr
 *  Always remove signature sequences from certain dataset sequences (e.g.
 *  VerifyingObserver or PredecessorDocuments).
 *
 *  Revision 1.21  2001/01/18 15:54:48  joergr
 *  Added support for digital signatures.
 *  Encode PN components in separate XML tags.
 *
 *  Revision 1.20  2000/12/12 17:21:39  joergr
 *  Added explicit typecast to keep SunCC 2.0.1 quiet.
 *
 *  Revision 1.19  2000/12/08 13:45:38  joergr
 *  Renamed createNewSeries(studyUID) to createNewSeriesInStudy(studyUID).
 *
 *  Revision 1.18  2000/11/16 13:33:03  joergr
 *  Corrected behaviour of updateDicomAttributes().
 *
 *  Revision 1.17  2000/11/14 17:27:29  joergr
 *  Added method to remove verification information.
 *
 *  Revision 1.16  2000/11/14 16:36:24  joergr
 *  Added methods to set the content date/time.
 *
 *  Revision 1.15  2000/11/14 11:45:55  joergr
 *  Corrected behaviour of method createRevisedVersion().
 *
 *  Revision 1.14  2000/11/13 14:19:12  joergr
 *  Updated comments.
 *
 *  Revision 1.13  2000/11/13 10:27:00  joergr
 *  dded output of optional observation datetime to rendered HTML page.
 *
 *  Revision 1.12  2000/11/10 18:10:26  joergr
 *  Corrected behaviour of createNewSOPInstance() and createRevisedDocument().
 *
 *  Revision 1.11  2000/11/10 17:45:32  joergr
 *  Added new methods to set the completion flag description, create new study/
 *  series UIDs. Added missing type 2 sequence to dataset. Corrected wrong format
 *  of predecessor documents sequence. Changed behaviour of completion/verification
 *  flags. Improved HTML and print/dump output.
 *
 *  Revision 1.10  2000/11/09 20:34:00  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.9  2000/11/07 18:27:46  joergr
 *  Enhanced rendered HTML output of date, time, datetime and pname.
 *
 *  Revision 1.8  2000/11/01 16:33:09  joergr
 *  Added support for conversion to XML.
 *  Added support for Cascading Style Sheet (CSS) used optionally for HTML
 *  rendering. Optimized HTML rendering.
 *  Enhanced support for specific character sets.
 *
 *  Revision 1.7  2000/10/26 14:28:54  joergr
 *  Added new flag specifying whether to add a "dcmtk" footnote to the rendered
 *  HTML document or not.
 *
 *  Revision 1.6  2000/10/24 15:04:11  joergr
 *  Changed HTML hyperlinks to referenced objects from "dicom://" to "file://"
 *  to facilitate access from Java.
 *
 *  Revision 1.5  2000/10/18 17:15:43  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.4  2000/10/17 12:37:14  joergr
 *  Added methods to retrieve information on predecessor documents and
 *  verifying observers.
 *  Changed behaviour of createRevisedVersion(): replace all existing sequence
 *  items.
 *
 *  Revision 1.3  2000/10/16 16:32:18  joergr
 *  Added missing get/setSeriesDescription() methods.
 *
 *  Revision 1.2  2000/10/16 12:02:48  joergr
 *  Made method creating a new SOP instance public. Added check for correct SOP
 *  instance UID and SOP class UID to validity check.
 *
 *  Revision 1.1  2000/10/13 07:52:18  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
