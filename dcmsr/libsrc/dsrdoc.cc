/*
 *
 *  Copyright (C) 2000-2001, OFFIS
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRDocument
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-11-09 16:14:55 $
 *  CVS/RCS Revision: $Revision: 1.31 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoc.h"

#include <fstream.h>


DSRDocument::DSRDocument(const E_DocumentType documentType)
  : DocumentTree(documentType),
    LogStream(NULL),
    FinalizedFlag(OFFalse),
    CompletionFlagEnum(CF_invalid),
    VerificationFlagEnum(VF_invalid),
    SpecificCharacterSetEnum(CS_invalid),
    SOPClassUID(DCM_SOPClassUID),
    SOPInstanceUID(DCM_SOPInstanceUID),
    SpecificCharacterSet(DCM_SpecificCharacterSet),
    InstanceCreationDate(DCM_InstanceCreationDate),
    InstanceCreationTime(DCM_InstanceCreationTime),
    InstanceCreatorUID(DCM_InstanceCreatorUID),
    StudyInstanceUID(DCM_StudyInstanceUID),
    StudyDate(DCM_StudyDate),
    StudyTime(DCM_StudyTime),
    ReferringPhysiciansName(DCM_ReferringPhysiciansName),
    StudyID(DCM_StudyID),
    AccessionNumber(DCM_AccessionNumber),
    StudyDescription(DCM_StudyDescription),
    SeriesDescription(DCM_SeriesDescription),
    PatientsName(DCM_PatientsName),
    PatientID(DCM_PatientID),
    PatientsBirthDate(DCM_PatientsBirthDate),
    PatientsSex(DCM_PatientsSex),
    Manufacturer(DCM_Manufacturer),
    Modality(DCM_Modality),
    SeriesInstanceUID(DCM_SeriesInstanceUID),
    SeriesNumber(DCM_SeriesNumber),
    ReferencedStudyComponent(DCM_ReferencedStudyComponentSequence),
    InstanceNumber(DCM_InstanceNumber),
    CompletionFlag(DCM_CompletionFlag),
    CompletionFlagDescription(DCM_CompletionFlagDescription),
    VerificationFlag(DCM_VerificationFlag),
    ContentDate(DCM_ContentDate),
    ContentTime(DCM_ContentTime),
    VerifyingObserver(DCM_VerifyingObserverSequence),
    PredecessorDocuments(DCM_PredecessorDocumentsSequence),
    PerformedProcedureCode(DCM_PerformedProcedureCodeSequence)
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
    StudyInstanceUID.clear();
    StudyDate.clear();
    StudyTime.clear();
    ReferringPhysiciansName.clear();
    StudyID.clear();
    AccessionNumber.clear();
    StudyDescription.clear();
    SeriesDescription.clear();
    PatientsName.clear();
    PatientID.clear();
    PatientsBirthDate.clear();
    PatientsSex.clear();
    Manufacturer.clear();
    Modality.clear();
    SeriesInstanceUID.clear();
    SeriesNumber.clear();
    ReferencedStudyComponent.clear();
    InstanceNumber.clear();
    CompletionFlag.clear();
    CompletionFlagDescription.clear();
    VerificationFlag.clear();
    ContentDate.clear();
    ContentTime.clear();
    VerifyingObserver.clear();
    PredecessorDocuments.clear();
    PerformedProcedureCode.clear();
}


OFBool DSRDocument::isValid()
{
    /* document is valid if document tree is valid and ... */
    return DocumentTree.isValid() && (SOPClassUID.getLength() > 0) && (SOPInstanceUID.getLength() > 0);
}


OFBool DSRDocument::isFinalized()
{
    return FinalizedFlag;
}


OFCondition DSRDocument::print(ostream &stream,
                               const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (isValid())
    {
        OFString string;
        /* update only some DICOM attributes */
        updateAttributes(OFFalse /* updateAll */);

        // --- print some general document information ---

        /* document type/title */
        stream << documentTypeToDocumentTitle(getDocumentType(), string) << endl << endl;
        /* patient related information */
        if (PatientsName.getLength() > 0)
        {
            stream << "Patient            : " << getPrintStringFromElement(PatientsName, string);
            OFString patientStr;
            if (PatientsSex.getLength() > 0)
                patientStr += getPrintStringFromElement(PatientsSex, string);
            if (PatientsBirthDate.getLength() > 0)
            {
               if (patientStr.length() > 0)
                   patientStr += ", ";
               patientStr += getPrintStringFromElement(PatientsBirthDate, string);
            }
            if (PatientID.getLength() > 0)
            {
               if (patientStr.length() > 0)
                   patientStr += ", ";
               patientStr += '#';
               patientStr += getPrintStringFromElement(PatientID, string);
            }
            if (patientStr.length() > 0)
                stream << " (" << patientStr << ")";
            stream << endl;
        }
        /* referring physician */
        if (ReferringPhysiciansName.getLength() > 0)
            stream << "Referring Physician: " << getPrintStringFromElement(ReferringPhysiciansName, string) << endl;
        /* manufacturer */
        if (Manufacturer.getLength() > 0)
            stream << "Manufacturer       : " << getPrintStringFromElement(Manufacturer, string) << endl;
        /* completion flag */
        stream << "Completion Flag    : " << completionFlagToEnumeratedValue(CompletionFlagEnum) << endl;
        if (CompletionFlagDescription.getLength() > 0)
            stream << "                     " << getPrintStringFromElement(CompletionFlagDescription, string) << endl;
        /* predecessor documents */
        if (getNumberOfPredecessorDocuments() > 0)
            stream << "Predecessor Docs   : " << getNumberOfPredecessorDocuments() << endl;
        /* verification flag */
        stream << "Verification Flag  : " << verificationFlagToEnumeratedValue(VerificationFlagEnum) << endl;
        /* verifying observer */
        const size_t obsCount = getNumberOfVerifyingObservers();
        for (size_t i = 1; i <= obsCount; i++)
        {
            OFString dateTime, obsName, organization;
            DSRCodedEntryValue obsCode;
            if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization).good())
            {
                stream << "                     " << dateTime << ": " << obsName;
                if (obsCode.isValid())
                {
                    stream << " ";
                    obsCode.print(stream, flags & PF_printAllCodes /* printCodeValue */);
                }
                stream << ", " << organization << endl;
            }
        }
        /* content date and time */
        if ((ContentDate.getLength() > 0) && (ContentTime.getLength() > 0))
        {
            stream << "Content Date/Time  : " << getPrintStringFromElement(ContentDate, string) << " ";
            stream <<                            getPrintStringFromElement(ContentTime, string) << endl;
        }
        stream << endl;

        // --- dump document tree to stream ---
        result = DocumentTree.print(stream, flags);
    }
    return result;
}


OFCondition DSRDocument::checkDatasetForReading(DcmItem &dataset)
{
    OFCondition result = EC_Normal;
    OFString string;
    E_DocumentType documentType = DT_invalid;
    DcmUniqueIdentifier sopClassUID(DCM_SOPClassUID);
    DcmCodeString modality(DCM_Modality);
    /* check SOP class UID */
    result = getAndCheckElementFromDataset(dataset, sopClassUID, "1", "1", LogStream);
    if (result.good())
    {
        documentType = sopClassUIDToDocumentType(getStringValueFromElement(sopClassUID, string));
        if (documentType == DT_invalid)
        {
            printErrorMessage(LogStream, "SOP Class UID does not match one of the known SR document classes");
            result = SR_EC_UnknownDocumentType;
        }
        else if (!isDocumentTypeSupported(documentType))
        {
            printErrorMessage(LogStream, "Unsupported SOP Class UID (not yet implemented)");
            result = SR_EC_UnsupportedValue;
        }
    }
    /* check modality */
    if (result.good())
    {
        result = getAndCheckElementFromDataset(dataset, modality, "1", "1", LogStream);
        if (result.good())
        {
            if (getStringValueFromElement(modality, string) != documentTypeToModality(documentType))
            {
                OFString message = "Modality does not match '";
                message += documentTypeToModality(documentType);
                message += "' for ";
                message += documentTypeToReadableName(documentType);
                printErrorMessage(LogStream, message.c_str());
                result = SR_EC_InvalidValue;
            }
        }
    }
    return result;
}


OFCondition DSRDocument::read(DcmItem &dataset,
                              const size_t flags)
{
    OFCondition result = EC_Normal;
    /* re-initialize SR document */
    clear();
    /* check SOP class UID and modality first */
    result = checkDatasetForReading(dataset);
    /* dataset is OK */
    if (result.good())
    {
        OFCondition searchCond = EC_Normal;

        /* type 3 element and attributes which have already been checked are not checked */

        // --- SOP Common Module ---
        getElementFromDataset(dataset, SOPClassUID);   /* already checked */
        getAndCheckElementFromDataset(dataset, SOPInstanceUID, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, SpecificCharacterSet, "1-n", "1C", LogStream);
        if (SpecificCharacterSet.getVM() > 1)
            printWarningMessage(LogStream, "Multiple values for 'SpecificCharacterSet' are not supported");
        getAndCheckElementFromDataset(dataset, InstanceCreationDate, "1", "3", LogStream);
        getAndCheckElementFromDataset(dataset, InstanceCreationTime, "1", "3", LogStream);
        getAndCheckElementFromDataset(dataset, InstanceCreatorUID, "1", "3", LogStream);

        // --- General Study Module ---
        getAndCheckElementFromDataset(dataset, StudyInstanceUID, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, StudyDate, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, StudyTime, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, ReferringPhysiciansName, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, StudyID, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, AccessionNumber, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, StudyDescription, "1", "3", LogStream);

        // --- General series Module ---
        getAndCheckElementFromDataset(dataset, SeriesDescription, "1", "3", LogStream);

        // --- Patient Module ---
        getAndCheckElementFromDataset(dataset, PatientsName, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, PatientID, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, PatientsBirthDate, "1", "2", LogStream);
        getAndCheckElementFromDataset(dataset, PatientsSex, "1", "2", LogStream);

        // --- General Equipment Module ---
        getAndCheckElementFromDataset(dataset, Manufacturer, "1", "2", LogStream);

        // --- SR Document Series Module ---
        getElementFromDataset(dataset, Modality);   /* already checked */
        getAndCheckElementFromDataset(dataset, SeriesInstanceUID, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, SeriesNumber, "1", "1", LogStream);
        /* need to check sequence in two steps (avoids additional getAndCheck... method) */
        searchCond = getSequenceFromDataset(dataset, ReferencedStudyComponent);
        checkElementValue(ReferencedStudyComponent, "1", "2", LogStream, searchCond);
        /* remove possible signature sequences */
        removeAttributeFromSequence(ReferencedStudyComponent, DCM_MACParametersSequence);
        removeAttributeFromSequence(ReferencedStudyComponent, DCM_DigitalSignaturesSequence);

        // --- SR Document General Module (M) ---
        getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, CompletionFlag, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, CompletionFlagDescription, "1", "3", LogStream);
        getAndCheckElementFromDataset(dataset, VerificationFlag, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, ContentDate, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, ContentTime, "1", "1", LogStream);
        getSequenceFromDataset(dataset, VerifyingObserver);
        getSequenceFromDataset(dataset, PredecessorDocuments);
        /* need to check sequence in two steps (avoids additional getAndCheck... method) */
        searchCond = getSequenceFromDataset(dataset, PerformedProcedureCode);
        checkElementValue(PerformedProcedureCode, "1", "2", LogStream, searchCond);
        /* remove possible signature sequences */
        removeAttributeFromSequence(VerifyingObserver, DCM_MACParametersSequence);
        removeAttributeFromSequence(VerifyingObserver, DCM_DigitalSignaturesSequence);
        removeAttributeFromSequence(PredecessorDocuments, DCM_MACParametersSequence);
        removeAttributeFromSequence(PredecessorDocuments, DCM_DigitalSignaturesSequence);
        removeAttributeFromSequence(PerformedProcedureCode, DCM_MACParametersSequence);
        removeAttributeFromSequence(PerformedProcedureCode, DCM_DigitalSignaturesSequence);

        /* update internal enumerated values */
        OFString string;
        CompletionFlagEnum = enumeratedValueToCompletionFlag(getStringValueFromElement(CompletionFlag, string));
        VerificationFlagEnum = enumeratedValueToVerificationFlag(getStringValueFromElement(VerificationFlag, string));
        SpecificCharacterSetEnum = definedTermToCharacterSet(getStringValueFromElement(SpecificCharacterSet, string));

        /* read SR document tree */
        if (result.good())
        {
            E_DocumentType documentType = sopClassUIDToDocumentType(getStringValueFromElement(SOPClassUID, string));
            result = DocumentTree.read(dataset, documentType, flags);
        }
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
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPClassUID));
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPInstanceUID));
        if (SpecificCharacterSet.getLength() > 0)    /* optional */
            addElementToDataset(result, dataset, new DcmCodeString(SpecificCharacterSet));
        if (InstanceCreationDate.getLength() > 0)    /* optional */
            addElementToDataset(result, dataset, new DcmDate(InstanceCreationDate));
        if (InstanceCreationTime.getLength() > 0)    /* optional */
            addElementToDataset(result, dataset, new DcmTime(InstanceCreationTime));
        if (InstanceCreatorUID.getLength() > 0)      /* optional */
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(InstanceCreatorUID));

        // --- General Study Module ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(StudyInstanceUID));
        addElementToDataset(result, dataset, new DcmDate(StudyDate));
        addElementToDataset(result, dataset, new DcmTime(StudyTime));
        addElementToDataset(result, dataset, new DcmPersonName(ReferringPhysiciansName));
        addElementToDataset(result, dataset, new DcmShortString(StudyID));
        addElementToDataset(result, dataset, new DcmShortString(AccessionNumber));
        if (StudyDescription.getLength() > 0)     /* optional */
            addElementToDataset(result, dataset, new DcmLongString(StudyDescription));

        // --- General series Module ---
        if (SeriesDescription.getLength() > 0)    /* optional */
            addElementToDataset(result, dataset, new DcmLongString(SeriesDescription));

        // --- Patient Module ---
        addElementToDataset(result, dataset, new DcmPersonName(PatientsName));
        addElementToDataset(result, dataset, new DcmLongString(PatientID));
        addElementToDataset(result, dataset, new DcmDate(PatientsBirthDate));
        addElementToDataset(result, dataset, new DcmCodeString(PatientsSex));

        // --- General Equipment Module ---
        addElementToDataset(result, dataset, new DcmLongString(Manufacturer));

        // --- SR Document Series Module ---
        addElementToDataset(result, dataset, new DcmCodeString(Modality));
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SeriesInstanceUID));
        addElementToDataset(result, dataset, new DcmIntegerString(SeriesNumber));
        /* always write empty sequence since not yet fully supported */
        ReferencedStudyComponent.clear();
        addElementToDataset(result, dataset, new DcmSequenceOfItems(ReferencedStudyComponent));

        // --- SR Document General Module (M) ---
        addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber));
        addElementToDataset(result, dataset, new DcmCodeString(CompletionFlag));
        if (CompletionFlagDescription.getLength() > 0)    /* optional */
            addElementToDataset(result, dataset, new DcmLongString(CompletionFlagDescription));
        addElementToDataset(result, dataset, new DcmCodeString(VerificationFlag));
        addElementToDataset(result, dataset, new DcmDate(ContentDate));
        addElementToDataset(result, dataset, new DcmTime(ContentTime));
        if (VerifyingObserver.card() > 0)                /* optional */
            addElementToDataset(result, dataset, new DcmSequenceOfItems(VerifyingObserver));
        if (PredecessorDocuments.card() > 0)             /* optional */
            addElementToDataset(result, dataset, new DcmSequenceOfItems(PredecessorDocuments));
        /* always write empty sequence since not yet fully supported */
        PerformedProcedureCode.clear();
        addElementToDataset(result, dataset, new DcmSequenceOfItems(PerformedProcedureCode));

        /* write SR document tree */
        if (result.good())
            result = DocumentTree.write(dataset, markedItems);
    } else
        result = SR_EC_InvalidDocument;
    return result;
}


OFCondition DSRDocument::writeXML(ostream &stream,
                                  const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    /* only write valid documents */
    if (isValid())
    {
        /* used for multiple purposes */
        OFString string;
        /* update DICOM attributes */
        updateAttributes();

        // --- XML document structure (start) ---

        stream << "<?xml version=\"1.0\"";
        /* optional character set */
        string = characterSetToXMLName(SpecificCharacterSetEnum);
        if (string.length() > 0)
            stream << " encoding=\"" << string << "\"";
        stream << "?>" << endl;

        stream << "<report type=\"" << documentTypeToReadableName(getDocumentType()) << "\">" << endl;

        // --- write some general document information ---

        stream << "<sopclass uid=\"" << getMarkupStringFromElement(SOPClassUID, string) << "\"/>" << endl;
        writeStringFromElementToXML(stream, Modality, "modality", flags & XF_writeEmptyTags);
        writeStringFromElementToXML(stream, Manufacturer, "manufacturer", flags & XF_writeEmptyTags);

        if ((flags & XF_writeEmptyTags) || (ReferringPhysiciansName.getLength() > 0))
        {
            stream << "<referringphysician>" << endl;
            writeStringFromElementToXML(stream, ReferringPhysiciansName, "name", flags & XF_writeEmptyTags);
            stream << "</referringphysician>" << endl;
        }

        stream << "<patient>" << endl;
        writeStringFromElementToXML(stream, PatientID, "id", flags & XF_writeEmptyTags);
        writeStringFromElementToXML(stream, PatientsName, "name", flags & XF_writeEmptyTags);
        if ((flags & XF_writeEmptyTags) || (PatientsBirthDate.getLength() > 0))
        {
            stream << "<birthday>" << endl;
            writeStringFromElementToXML(stream, PatientsBirthDate, "date", flags & XF_writeEmptyTags);
            stream << "</birthday>" << endl;
        }
        writeStringFromElementToXML(stream, PatientsSex, "sex", flags & XF_writeEmptyTags);
        stream << "</patient>" << endl;

        stream << "<study uid=\"" << getMarkupStringFromElement(StudyInstanceUID, string) << "\">" << endl;
        writeStringFromElementToXML(stream, StudyID, "id", flags & XF_writeEmptyTags);
        writeStringFromElementToXML(stream, StudyDate, "date", flags & XF_writeEmptyTags);
        writeStringFromElementToXML(stream, StudyTime, "time", flags & XF_writeEmptyTags);
        if ((flags & XF_writeEmptyTags) || (AccessionNumber.getLength() > 0))
        {
            stream << "<accession>" << endl;
            writeStringFromElementToXML(stream, AccessionNumber, "number", flags & XF_writeEmptyTags);
            stream << "</accession>" << endl;
        }
        writeStringFromElementToXML(stream, StudyDescription, "description", flags & XF_writeEmptyTags);
        stream << "</study>" << endl;

        stream << "<series uid=\"" << getMarkupStringFromElement(SeriesInstanceUID, string) << "\">" << endl;
        writeStringFromElementToXML(stream, SeriesNumber, "number", flags & XF_writeEmptyTags);
        writeStringFromElementToXML(stream, SeriesDescription, "description", flags & XF_writeEmptyTags);
        stream << "</series>" << endl;

        stream << "<instance uid=\"" << getMarkupStringFromElement(SOPInstanceUID, string) << "\">" << endl;
        writeStringFromElementToXML(stream, InstanceNumber, "number", flags & XF_writeEmptyTags);
        if ((flags & XF_writeEmptyTags) || (InstanceCreatorUID.getLength() > 0) ||
            (InstanceCreationDate.getLength() > 0) || (InstanceCreationTime.getLength() > 0))
        {
            stream << "<creation>" << endl;
            writeStringFromElementToXML(stream, InstanceCreatorUID, "uid", flags & XF_writeEmptyTags);
            writeStringFromElementToXML(stream, InstanceCreationDate, "date", flags & XF_writeEmptyTags);
            writeStringFromElementToXML(stream, InstanceCreationTime, "time", flags & XF_writeEmptyTags);
            stream << "</creation>" << endl;
        }
        stream << "</instance>" << endl;

        stream << "<document>" << endl;
        stream << "<completion flag=\"" << completionFlagToEnumeratedValue(CompletionFlagEnum) << "\">" << endl;
        writeStringFromElementToXML(stream, CompletionFlagDescription, "description", flags & XF_writeEmptyTags);
        stream << "</completion>" << endl;

        stream << "<verification flag=\"" << verificationFlagToEnumeratedValue(VerificationFlagEnum) << "\">" << endl;
        size_t i = 0;
        const size_t obsCount = getNumberOfVerifyingObservers();
        for (i = 1; i <= obsCount; i++)
        {
            stream << "<observer pos=\"" << i << "\">" << endl;
            OFString dateTime, obsName, organization;
            DSRCodedEntryValue obsCode;
            if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization).good())
            {
                writeStringValueToXML(stream, dateTime, "datetime", flags & XF_writeEmptyTags);
                if ((obsName.length() > 0) || (flags & XF_writeEmptyTags))
                    stream << "<name>" << endl << dicomToXMLPersonName(obsName, string) << "</name>" << endl;
                if (obsCode.isValid())
                {
                    if (flags & DSRTypes::XF_codeComponentsAsAttribute)
                        stream << "<code";     // bracket ">" is closed in next writeXML() routine
                    else
                        stream << "<code>" << endl;
                    obsCode.writeXML(stream, flags, LogStream);
                    stream << "</code>" << endl;
                }
                writeStringValueToXML(stream, organization, "organization", flags & XF_writeEmptyTags);
            }
            stream << "</observer>" << endl;
        }
        stream << "</verification>" << endl;

        const size_t predCount = getNumberOfPredecessorDocuments();
        for (i = 1; i <= predCount; i++)
        {
            stream << "<predecessor pos=\"" << i << "\">" << endl;
            OFString sopClassUID, sopInstanceUID;
            if (getPredecessorDocument(i, sopClassUID, sopInstanceUID).good())
            {
                stream << "<sopclass uid=\"" << sopClassUID << "\"/>" << endl;
                stream << "<instance uid=\"" << sopInstanceUID << "\"/>" << endl;
            }
            stream << "</predecessor>" << endl;
        }

        // --- write document content/tree to stream ---

        stream << "<content>" << endl;
        writeStringFromElementToXML(stream, ContentDate, "date", flags & XF_writeEmptyTags);
        writeStringFromElementToXML(stream, ContentTime, "time", flags & XF_writeEmptyTags);
        result = DocumentTree.writeXML(stream, flags);
        stream << "</content>" << endl;
        stream << "</document>" << endl;

        // --- XML document structure (end) ---

        stream << "</report>" << endl;
    }
    return result;
}


void DSRDocument::renderHTMLPatientData(ostream &stream,
                                        const size_t flags)
{
    OFString string, string2;
    OFString htmlString;
    const OFBool convertNonASCII = flags & HF_convertNonASCIICharacters;
    stream << convertToMarkupString(dicomToReadablePersonName(getStringValueFromElement(PatientsName, string), string2), htmlString, convertNonASCII);
    OFString patientStr;
    if (PatientsSex.getLength() > 0)
    {
        getPrintStringFromElement(PatientsSex, string);
        if (string == "M")
            patientStr += "male";
        else if (string == "F")
            patientStr += "female";
        else if (string == "O")
            patientStr += "other";
        else
            patientStr += convertToMarkupString(string, htmlString, convertNonASCII);
    }
    if (PatientsBirthDate.getLength() > 0)
    {
       if (patientStr.length() > 0)
           patientStr += ", ";
       patientStr += '*';
       patientStr += dicomToReadableDate(getStringValueFromElement(PatientsBirthDate, string), string2);
    }
    if (PatientID.getLength() > 0)
    {
       if (patientStr.length() > 0)
           patientStr += ", ";
       patientStr += '#';
       patientStr += convertToMarkupString(getStringValueFromElement(PatientID, string), htmlString, convertNonASCII);
    }
    if (patientStr.length() > 0)
        stream << " (" << patientStr << ")";
}


OFCondition DSRDocument::renderHTML(ostream &stream,
                                    const size_t flags,
                                    const char *styleSheet)
{
    OFCondition result = SR_EC_InvalidDocument;
    /* only render valid documents */
    if (isValid())
    {
        size_t newFlags = flags;
        if (flags & HF_version32Compatibility)
            newFlags |= HF_convertNonASCIICharacters;
        const OFBool convertNonASCII = newFlags & HF_convertNonASCIICharacters;

        /* used for multiple purposes */
        OFString string, string2;
        /* used for HTML string conversion */
        OFString htmlString;
        /* update only some DICOM attributes */
        updateAttributes(OFFalse /* updateAll */);

        // --- HTML document structure (start) ---

        /* optional document type definition */
        if (newFlags & HF_addDocumentTypeReference)
        {
            if (newFlags & HF_version32Compatibility)
                stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">" << endl;
            else {
                if (styleSheet != NULL)
                    stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">" << endl;
                else
                    stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\">" << endl;
            }
        }
        stream << "<html>" << endl;
        stream << "<head>" << endl;
        /* document title */
        stream << "<title>";
        if (newFlags & HF_renderPatientTitle)
            renderHTMLPatientData(stream, convertNonASCII);
        else
            stream << documentTypeToDocumentTitle(getDocumentType(), string);
        stream << "</title>" << endl;
        if (!(newFlags & HF_version32Compatibility))
        {
            /* optional cascading style sheet (HTML 4.0) */
            if (styleSheet != NULL)
            {
                if (newFlags & HF_copyStyleSheetContent)
                {
                    /* copy content from CSS file */
#ifdef HAVE_IOS_NOCREATE
                    ifstream cssFile(styleSheet, ios::in|ios::nocreate);
#else
                    ifstream cssFile(styleSheet, ios::in);
#endif
                    if (cssFile)
                    {
                        char c;
                        stream << "<style type=\"text/css\">" << endl;
                        stream << "<!--" << endl;
                        /* copy all characters */
                        while (cssFile.get(c))
                            stream << c;
                        stream << "//-->" << endl;
                        stream << "</style>" << endl;
                    } else {
                        OFString message = "Could not open CSS file \"";
                        message += styleSheet;
                        message += "\" ... ignoring";
                        printWarningMessage(LogStream, message.c_str());
                    }
                } else {
                    /* just add a reference to the CSS file (might be an URL) */
                    stream << "<link rel=stylesheet type=\"text/css\" href=\"" << styleSheet << "\">" << endl;
                }
            }
            /* optional character set (HTML 4.0) */
            string = characterSetToHTMLName(SpecificCharacterSetEnum);
            if (string.length() > 0)
            {
                stream << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
                stream << string << "\">" << endl;
            }
        }
        stream << "</head>" << endl;
        stream << "<body>" << endl;

        // --- render some general document information ---

        if (!(newFlags & HF_renderNoDocumentHeader))
        {
            /* for loop count */
            size_t i = 0;
            /* create a table for this purpose */
            stream << "<table>" << endl;
            /* patient related information */
            if (PatientsName.getLength() > 0)
            {
                stream << "<tr>" << endl;
                stream << "<td><b>Patient:</b></td>" << endl;
                stream << "<td>";
                renderHTMLPatientData(stream, convertNonASCII);
                stream << "</td>" << endl;
                stream << "</tr>" << endl;
            }
            /* referring physician */
            if (ReferringPhysiciansName.getLength() > 0)
            {
                stream << "<tr>" << endl;
                stream << "<td><b>Referring Physician:</b></td>" << endl;
                stream << "<td>" << convertToMarkupString(dicomToReadablePersonName(getStringValueFromElement(ReferringPhysiciansName, string), string2), htmlString, convertNonASCII);
                stream << "</td>" << endl;
                stream << "</tr>" << endl;
            }
            /* manufacturer */
            if (Manufacturer.getLength() > 0)
            {
                stream << "<tr>" << endl;
                stream << "<td><b>Manufacturer:</b></td>" << endl;
                stream << "<td>" << convertToMarkupString(getStringValueFromElement(Manufacturer, string), htmlString, convertNonASCII);
                stream << "</td>" << endl;
                stream << "</tr>" << endl;
            }
            /* completion flag */
            stream << "<tr>" << endl;
            stream << "<td><b>Completion Flag:</b></td>" << endl;
            stream << "<td>" << completionFlagToEnumeratedValue(CompletionFlagEnum) << "</td>" << endl;
            stream << "</tr>" << endl;
            /* completion flag description */
            if (CompletionFlagDescription.getLength() > 0)
            {
                stream << "<tr>" << endl;
                stream << "<td></td>" << endl;
                stream << "<td>" << convertToMarkupString(getStringValueFromElement(CompletionFlagDescription, string), htmlString, convertNonASCII);
                stream << "</td>" << endl;
                stream << "</tr>" << endl;
            }
            /* predecessor documents */
            const size_t preDocCount = getNumberOfPredecessorDocuments();
            if (preDocCount > 0)
            {
                stream << "<tr>" << endl;
                stream << "<td><b>Predecessor Docs:</b></td>" << endl;
                for (i = 1; i <= preDocCount; i++)
                {
                    if (i > 1)
                    {
                        stream << "<tr>" << endl;
                        stream << "<td></td>" << endl;
                    }
                    /* hyperlink to composite object */
                    OFString sopClass, sopInstance;
                    if (getPredecessorDocument(i, sopClass, sopInstance).good())
                    {
                        stream << "<td><a href=\"" << HTML_HYPERLINK_PREFIX_FOR_CGI;
                        stream << "?composite=" << sopClass << "+" << sopInstance << "\">";
                        stream << documentTypeToDocumentTitle(sopClassUIDToDocumentType(sopClass), string);
                        stream << "</a></td>" << endl;
                    } else
                        stream << "<td><i>invalid document reference</i></td>" << endl;
                    stream << "</tr>" << endl;
                }
            }
            /* verification flag */
            stream << "<tr>" << endl;
            stream << "<td><b>Verification Flag:</b></td>" << endl;
            stream << "<td>" << verificationFlagToEnumeratedValue(VerificationFlagEnum) << "</td>" << endl;
            stream << "</tr>" << endl;
            /* verifying observer */
            const size_t obsCount = getNumberOfVerifyingObservers();
            for (i = 1; i <= obsCount; i++)
            {
                OFString dateTime, obsName, organization;
                DSRCodedEntryValue obsCode;
                if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization).good())
                {
                    stream << "<tr>" << endl;
                    stream << "<td></td>" << endl;
                    stream << "<td>";
                    stream << dicomToReadableDateTime(dateTime, string2) << " - ";
                    stream << convertToMarkupString(dicomToReadablePersonName(obsName, string2), htmlString, convertNonASCII);
                    /* optional observer code */
                    if (obsCode.isValid() && ((newFlags & HF_renderAllCodes) == HF_renderAllCodes))
                    {
                        stream << " ";
                        if (obsCode.isValid())
                        {
                            stream << "(" << convertToMarkupString(obsCode.getCodeValue(), htmlString, convertNonASCII);
                            stream << "," << convertToMarkupString(obsCode.getCodingSchemeDesignator(), htmlString, convertNonASCII) << ",";
                            if (obsCode.getCodingSchemeVersion().length() > 0)
                                stream << convertToMarkupString(obsCode.getCodingSchemeVersion(), htmlString, convertNonASCII) << ",";
                            stream << "\"" << convertToMarkupString(obsCode.getCodeMeaning(), htmlString, convertNonASCII) << "\")";
                        }
                    }
                    stream << ", " << convertToMarkupString(organization, htmlString, convertNonASCII);
                    stream << "</td>" << endl;
                    stream << "</tr>" << endl;
                }
            }
            if ((ContentDate.getLength() > 0) && (ContentTime.getLength() > 0))
            {
                /* content date and time */
                stream << "<tr>" << endl;
                stream << "<td><b>Content Date/Time:</b></td>" << endl;
                stream << "<td>" << dicomToReadableDate(getStringValueFromElement(ContentDate, string), string2) << " ";
                stream << dicomToReadableTime(getStringValueFromElement(ContentTime, string), string2) << "</td>" << endl;
                stream << "</tr>" << endl;
            }
            /* end of table */
            stream << "</table>" << endl;

            stream << "<hr>" << endl;
        }

        // --- render document tree to stream ---

        /* create memory output stream for the annex */
        ostrstream annexStream;
        /* render document tree two the streams */
        result = DocumentTree.renderHTML(stream, annexStream, newFlags);
        /* append annex (with heading) to main document */
        if (result.good())
            result = appendStream(stream, annexStream, "<h1>Annex</h1>");

        // --- footnote ---

        if (newFlags & HF_renderDcmtkFootnote)
        {
            stream << "<hr>" << endl;

            stream << "<small>" << endl;
            stream << "This page was generated from a DICOM Structured Reporting document by ";
            stream << "<a href=\"http://www.offis.de/projekte/dicom/\">OFFIS dcmtk</a> " << OFFIS_DCMTK_VERSION << "." << endl;
            stream << "</small>" << endl;
        }

        // --- HTML document structure (end) ---

        stream << "</body>" << endl;
        stream << "</html>" << endl;
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
    return (size_t)VerifyingObserver.card();
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
        DcmItem *ditem = VerifyingObserver.getItem((unsigned long)(idx - 1));
        if (ditem != NULL)
        {
            result = getStringValueFromDataset(*ditem, DCM_VerificationDateTime, dateTime);
            if (result.good())
                result = getStringValueFromDataset(*ditem, DCM_VerifyingObserverName, observerName);
            if (result.good())
            {
                /* code is optional (type 2) */
                observerCode.readSequence(*ditem, DCM_VerifyingObserverIdentificationCodeSequence, "2" /* type */, LogStream);
                result = getStringValueFromDataset(*ditem, DCM_VerifyingOrganization, organization);
            }
            if (result.good())
            {
                if ((dateTime.length() == 0) || (observerName.length() == 0) || (organization.length() == 0))
                    result = SR_EC_InvalidValue;
            }
        }
    }
    return result;
}


size_t DSRDocument::getNumberOfPredecessorDocuments()
{
    size_t count = 0;
    const size_t studyCount = (size_t)PredecessorDocuments.card();
    /* for all studies */
    for (size_t study = 0; study < studyCount; study++)
    {
        DcmItem *studyItem = PredecessorDocuments.getItem(study);
        if (studyItem != NULL)
        {
            DcmSequenceOfItems seriesSeq(DCM_ReferencedSeriesSequence);
            if (getSequenceFromDataset(*studyItem, seriesSeq).good())
            {
                const size_t seriesCount = (size_t)seriesSeq.card();
                /* for all series in the study */
                for (size_t series = 0; series < seriesCount; series++)
                {
                    DcmItem *seriesItem = seriesSeq.getItem(series);
                    if (seriesItem != NULL)
                    {
                        DcmSequenceOfItems sopSeq(DCM_ReferencedSOPSequence);
                        if (getSequenceFromDataset(*seriesItem, sopSeq).good())
                        {
                            /* add number of referenced instances */
                            count += (size_t)sopSeq.card();
                        }
                    }
                }
            }
        }
    }
    return count;
}


OFCondition DSRDocument::getPredecessorDocument(const size_t idx,
                                                OFString &sopClassUID,
                                                OFString &sopInstanceUID)
{
    OFString studyDummy, seriesDummy;
    return getPredecessorDocument(idx, studyDummy, seriesDummy, sopClassUID, sopInstanceUID);
}


OFCondition DSRDocument::getPredecessorDocument(const size_t idx,
                                                OFString &studyInstanceUID,
                                                OFString &seriesInstanceUID,
                                                OFString &sopClassUID,
                                                OFString &sopInstanceUID)
{
    OFCondition result = EC_IllegalCall;
    /* clear all reference variables */
    studyInstanceUID.clear();
    seriesInstanceUID.clear();
    sopClassUID.clear();
    sopInstanceUID.clear();
    /* check for valid index */
    if ((idx > 0) && (idx <= getNumberOfPredecessorDocuments()))
    {
        size_t sopIdx = idx;
        size_t study = 0;
        const size_t studyCount = (size_t)PredecessorDocuments.card();
        while ((study < studyCount) && (result == EC_IllegalCall))
        {

            DcmItem *studyItem = PredecessorDocuments.getItem(study);
            if (studyItem != NULL)
            {
                DcmSequenceOfItems seriesSeq(DCM_ReferencedSeriesSequence);
                if (getSequenceFromDataset(*studyItem, seriesSeq).good())
                {
                    size_t series = 0;
                    const size_t seriesCount = (size_t)seriesSeq.card();
                    /* for all series in the study */
                    while ((series < seriesCount) && (result == EC_IllegalCall))
                    {
                        DcmItem *seriesItem = seriesSeq.getItem(series);
                        if (seriesItem != NULL)
                        {
                            DcmSequenceOfItems sopSeq(DCM_ReferencedSOPSequence);
                            if (getSequenceFromDataset(*seriesItem, sopSeq).good())
                            {
                                const size_t sopCount = (size_t)sopSeq.card();
                                /* specified entry found */
                                if (sopIdx <= sopCount)
                                {
                                    DcmItem *sopItem = sopSeq.getItem(sopIdx - 1);
                                    if (sopItem != NULL)
                                    {
                                        /* retrieve UIDs */
                                        getStringValueFromDataset(*studyItem, DCM_StudyInstanceUID, studyInstanceUID);
                                        getStringValueFromDataset(*seriesItem, DCM_SeriesInstanceUID, seriesInstanceUID);
                                        getStringValueFromDataset(*sopItem, DCM_ReferencedSOPClassUID, sopClassUID);
                                        getStringValueFromDataset(*sopItem, DCM_ReferencedSOPInstanceUID, sopInstanceUID);
                                        result = EC_Normal;
                                    } else
                                        result = EC_CorruptedData;
                                } else
                                    sopIdx -= sopCount;
                            } else
                                result = EC_CorruptedData;
                        } else
                            result = EC_CorruptedData;
                        series++;
                    }
                } else
                    result = EC_CorruptedData;
            } else
                result = EC_CorruptedData;
            study++;
        }
        /* check whether resulting UIDs are valid */
        if (result.good())
        {
            if ((studyInstanceUID.length() == 0) || (seriesInstanceUID.length() == 0) ||
                (sopClassUID.length() == 0) || (sopInstanceUID.length() == 0))
            {
                result = SR_EC_InvalidValue;
            }
        }
    } else
        result = EC_IllegalParameter;
    return result;
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


const char *DSRDocument::getPatientsName() const
{
    return getStringValueFromElement(PatientsName);
}


const char *DSRDocument::getPatientsBirthDate() const
{
    return getStringValueFromElement(PatientsBirthDate);
}


const char *DSRDocument::getPatientsSex() const
{
    return getStringValueFromElement(PatientsSex);
}


const char *DSRDocument::getReferringPhysiciansName() const
{
    return getStringValueFromElement(ReferringPhysiciansName);
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

const OFString &DSRDocument::getModality(OFString &string) const
{
    return getStringValueFromElement(Modality, string);
}


const OFString &DSRDocument::getSOPClassUID(OFString &string) const
{
    return getStringValueFromElement(SOPClassUID, string);
}


const OFString &DSRDocument::getStudyInstanceUID(OFString &string) const
{
    return getStringValueFromElement(StudyInstanceUID, string);
}


const OFString &DSRDocument::getSeriesInstanceUID(OFString &string) const
{
    return getStringValueFromElement(SeriesInstanceUID, string);
}


const OFString &DSRDocument::getSOPInstanceUID(OFString &string) const
{
    return getStringValueFromElement(SOPInstanceUID, string);
}


const OFString &DSRDocument::getInstanceCreatorUID(OFString &string) const
{
    return getStringValueFromElement(InstanceCreatorUID, string);
}


const OFString &DSRDocument::getSpecificCharacterSet(OFString &string) const
{
    return getStringValueFromElement(SpecificCharacterSet, string);
}


const OFString &DSRDocument::getPatientsName(OFString &string) const
{
    return getStringValueFromElement(PatientsName, string);
}


const OFString &DSRDocument::getPatientsBirthDate(OFString &string) const
{
    return getStringValueFromElement(PatientsBirthDate, string);
}


const OFString &DSRDocument::getPatientsSex(OFString &string) const
{
    return getStringValueFromElement(PatientsSex, string);
}


const OFString &DSRDocument::getReferringPhysiciansName(OFString &string) const
{
    return getStringValueFromElement(ReferringPhysiciansName, string);
}


const OFString &DSRDocument::getStudyDescription(OFString &string) const
{
    return getStringValueFromElement(StudyDescription, string);
}


const OFString &DSRDocument::getSeriesDescription(OFString &string) const
{
    return getStringValueFromElement(SeriesDescription, string);
}


const OFString &DSRDocument::getManufacturer(OFString &string) const
{
    return getStringValueFromElement(Manufacturer, string);
}


const OFString &DSRDocument::getStudyDate(OFString &string) const
{
    return getStringValueFromElement(StudyDate, string);
}


const OFString &DSRDocument::getStudyTime(OFString &string) const
{
    return getStringValueFromElement(StudyTime, string);
}


const OFString &DSRDocument::getInstanceCreationDate(OFString &string) const
{
    return getStringValueFromElement(InstanceCreationDate, string);
}


const OFString &DSRDocument::getInstanceCreationTime(OFString &string) const
{
    return getStringValueFromElement(InstanceCreationTime, string);
}


const OFString &DSRDocument::getContentDate(OFString &string) const
{
    return getStringValueFromElement(ContentDate, string);
}


const OFString &DSRDocument::getContentTime(OFString &string) const
{
    return getStringValueFromElement(ContentTime, string);
}


const OFString &DSRDocument::getStudyID(OFString &string) const
{
    return getStringValueFromElement(StudyID, string);
}


const OFString &DSRDocument::getPatientID(OFString &string) const
{
    return getStringValueFromElement(PatientID, string);
}


const OFString &DSRDocument::getSeriesNumber(OFString &string) const
{
    return getStringValueFromElement(SeriesNumber, string);
}


const OFString &DSRDocument::getInstanceNumber(OFString &string) const
{
    return getStringValueFromElement(InstanceNumber, string);
}


const OFString &DSRDocument::getAccessionNumber(OFString &string) const
{
    return getStringValueFromElement(AccessionNumber, string);
}


// --- set attributes ---

OFCondition DSRDocument::setSpecificCharacterSet(const OFString &string)
{
    SpecificCharacterSetEnum = definedTermToCharacterSet(string);
    /* might add check for correct format (VR) later on */
    return SpecificCharacterSet.putString(string.c_str());
}


OFCondition DSRDocument::setCompletionFlagDescription(const OFString &string)
{
    OFCondition result = EC_Normal;
    if (string.length() > 0)
        result = CompletionFlagDescription.putString(string.c_str());
    else
        CompletionFlagDescription.clear();
    return result;
}


OFCondition DSRDocument::setPatientsName(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientsName.putString(string.c_str());
}


OFCondition DSRDocument::setPatientsBirthDate(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientsBirthDate.putString(string.c_str());
}


OFCondition DSRDocument::setPatientsSex(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientsSex.putString(string.c_str());
}


OFCondition DSRDocument::setReferringPhysiciansName(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return ReferringPhysiciansName.putString(string.c_str());
}


OFCondition DSRDocument::setStudyDescription(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return StudyDescription.putString(string.c_str());
}


OFCondition DSRDocument::setSeriesDescription(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return SeriesDescription.putString(string.c_str());
}


OFCondition DSRDocument::setManufacturer(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return Manufacturer.putString(string.c_str());
}


OFCondition DSRDocument::setContentDate(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return ContentDate.putString(string.c_str());
}


OFCondition DSRDocument::setContentTime(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return ContentTime.putString(string.c_str());
}


OFCondition DSRDocument::setStudyID(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return StudyID.putString(string.c_str());
}


OFCondition DSRDocument::setPatientID(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientID.putString(string.c_str());
}


OFCondition DSRDocument::setSeriesNumber(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return SeriesNumber.putString(string.c_str());
}


OFCondition DSRDocument::setInstanceNumber(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return InstanceNumber.putString(string.c_str());
}


OFCondition DSRDocument::setAccessionNumber(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return AccessionNumber.putString(string.c_str());
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
    if (studyUID.length() > 0)
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


OFCondition DSRDocument::createRevisedVersion()
{
    OFCondition result = EC_Normal;
    /* check whether document is already completed */
    if (CompletionFlagEnum == CF_Complete)
    {
        DcmItem *ditem = new DcmItem();
        if (ditem != NULL)
        {
            /* write current document UIDs */
            ditem->insert(new DcmUniqueIdentifier(StudyInstanceUID));
            /* Referenced Series Sequence */
            DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedSeriesSequence);
            if (dseq != NULL)
            {
                DcmItem *ditem2 = new DcmItem();
                if (ditem2 != NULL)
                {
                    ditem2->insert(new DcmUniqueIdentifier(SeriesInstanceUID));
                    /* Referenced SOP Sequence */
                    DcmSequenceOfItems *dseq2 = new DcmSequenceOfItems(DCM_ReferencedSOPSequence);
                    if (dseq2 != NULL)
                    {
                        DcmItem *ditem3 = new DcmItem();
                        if (ditem3 != NULL)
                        {
                            /* Referenced SOP Class and Instance UID */
                            putStringValueToDataset(*ditem3, DCM_ReferencedSOPClassUID, getStringValueFromElement(SOPClassUID));
                            putStringValueToDataset(*ditem3, DCM_ReferencedSOPInstanceUID, getStringValueFromElement(SOPInstanceUID));
                            dseq2->insert(ditem3);
                        } else
                            result = EC_MemoryExhausted;
                        ditem2->insert(dseq2);
                    } else
                        result = EC_MemoryExhausted;
                    dseq->insert(ditem2);
                } else
                    result = EC_MemoryExhausted;
                ditem->insert(dseq);
                /* everything went OK */
                if (result.good())
                {
                    /* set completion flag to PARTIAL, delete description */
                    CompletionFlagEnum = CF_Partial;
                    CompletionFlagDescription.clear();
                    /* clear content date/time, will be set automatically in updateAttributes() */
                    ContentDate.clear();
                    ContentTime.clear();
                    /* clear list of verifying observers and set flag to UNVERIFIED */
                    removeVerification();
                    /* insert item into sequence (replace old ones) */
                    PredecessorDocuments.clear();
                    PredecessorDocuments.insert(ditem);
                    /* remove digital signatures from document tree */
                    DocumentTree.removeSignatures();
                    /* create new instance UID, update creation date/time and reset finalized flag */
                    createNewSOPInstance();
                }
            } else {
                delete ditem;
                result = EC_MemoryExhausted;
            }
        } else
            result = EC_MemoryExhausted;
    } else
        result = EC_IllegalCall;
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
    /* if document is not already completed */
    if (CompletionFlagEnum != CF_Complete)
    {
        /* completed for now and ever */
        CompletionFlagEnum = CF_Complete;
        /* completion flag description */
        setCompletionFlagDescription(description);
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const OFString &organization)
{
    /* empty CodedEntryValue */
    return verifyDocument(observerName, DSRCodedEntryValue(), organization);
}


OFCondition DSRDocument::verifyDocument(const OFString &observerName,
                                        const DSRCodedEntryValue &observerCode,
                                        const OFString &organization)
{
    OFCondition result = EC_IllegalCall;
    /* verify completed documents only */
    if (CompletionFlagEnum == CF_Complete)
    {
        /* empty strings are not allowed (type 1 attributes) */
        if ((observerName.length() > 0) && (organization.length() > 0))
        {
            DcmItem *ditem = new DcmItem();
            if (ditem != NULL)
            {
                /* write VerifyingObserverName */
                putStringValueToDataset(*ditem, DCM_VerifyingObserverName, observerName);
                /* write VerifyingObserverIdentificationCodeSequence (might be empty, type 2) */
                observerCode.writeSequence(*ditem, DCM_VerifyingObserverIdentificationCodeSequence, LogStream);
                /* write VerifyingOrganization */
                putStringValueToDataset(*ditem, DCM_VerifyingOrganization, organization);
                /* write VerificationDateTime */
                OFString string;
                currentDateTime(string);
                putStringValueToDataset(*ditem, DCM_VerificationDateTime, string);
                /* insert items into sequence */
                VerifyingObserver.insert(ditem);
                /* set VerificationFlag to VERIFIED */
                VerificationFlagEnum = VF_Verified;
                /* reset FinalizedFlag */
                FinalizedFlag = OFFalse;
            } else
                result = EC_MemoryExhausted;
            result = EC_Normal;
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


void DSRDocument::setLogStream(OFConsole *stream)
{
    /* store log stream */
    LogStream = stream;
    /* propagate to document tree */
    DocumentTree.setLogStream(stream);
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
        if (InstanceNumber.getLength() == 0)
            InstanceNumber.putString("1");
        /* create new series number if required (type 1) */
        if (SeriesNumber.getLength() == 0)
            SeriesNumber.putString("1");

        char uid[100];
        /* create new SOP instance UID if required */
        if (SOPInstanceUID.getLength() == 0)
        {
            SOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
            OFString string;
            /* set instance creation date to current date */
            InstanceCreationDate.putString(currentDate(string).c_str());
            /* set instance creation time to current time */
            InstanceCreationTime.putString(currentTime(string).c_str());
            /* set instance creator UID to identify instances that have been created by this toolkit */
            InstanceCreatorUID.putString(OFFIS_INSTANCE_CREATOR_UID);
        }
        /* create new study instance UID if required */
        if (StudyInstanceUID.getLength() == 0)
            StudyInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
        /* create new series instance UID if required */
        if (SeriesInstanceUID.getLength() == 0)
            SeriesInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));

        /* check and set content date if required */
        if (ContentDate.getLength() == 0)
            ContentDate.putString(getStringValueFromElement(InstanceCreationDate));
        /* check and set content time if required */
        if (ContentTime.getLength() == 0)
            ContentTime.putString(getStringValueFromElement(InstanceCreationTime));
    }
    /* check and adjust completion flag if required */
    if (CompletionFlagEnum == CF_invalid)
        CompletionFlagEnum = CF_Partial;
    CompletionFlag.putString(completionFlagToEnumeratedValue(CompletionFlagEnum));
    /* check and adjust verification flag if required */
    if (VerificationFlagEnum == VF_invalid)
        VerificationFlagEnum = VF_Unverified;
    VerificationFlag.putString(verificationFlagToEnumeratedValue(VerificationFlagEnum));
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoc.cc,v $
 *  Revision 1.31  2001-11-09 16:14:55  joergr
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
