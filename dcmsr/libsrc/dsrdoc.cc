/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Update Date:      $Date: 2000-10-24 15:04:11 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoc.h"


DSRDocument::DSRDocument(const E_DocumentType documentType)
  : DocumentTree(documentType),
    LogStream(NULL),
    CompletionFlagEnum(CF_invalid),
    VerificationFlagEnum(VF_invalid),
    SOPClassUID(DCM_SOPClassUID),
    SOPInstanceUID(DCM_SOPInstanceUID),
    SpecificCharacterSet(DCM_SpecificCharacterSet),
    InstanceCreationDate(DCM_InstanceCreationDate),
    InstanceCreationTime(DCM_InstanceCreationTime),
    InstanceCreatorUID(DCM_InstanceCreatorUID),
    StudyInstanceUID(DCM_StudyInstanceUID),
    StudyDate(DCM_StudyInstanceUID),
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
    PredecessorDocuments(DCM_PredecessorDocumentsSequence)
{
    /* set initial values for a new SOP instance */
    createNewSOPInstance();
}


DSRDocument::~DSRDocument()
{
}


void DSRDocument::clear()
{
    /* clear SR document tree */
    DocumentTree.clear();
    /* clear enumerated values */
    CompletionFlagEnum = CF_invalid;
    VerificationFlagEnum = VF_invalid;
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
}


OFBool DSRDocument::isValid()
{
    /* document is valid if document tree is valid and ... */
    return DocumentTree.isValid() && (SOPClassUID.getLength() > 0) && (SOPInstanceUID.getLength() > 0);
}


E_Condition DSRDocument::print(ostream &stream,
                               const size_t flags)
{
    E_Condition result = EC_CorruptedData;
    if (isValid())
    {
        OFString string;
        /* update DICOM attributes */
        updateAttributes();

        // --- print some general document information ---

        /* document type/title */
        stream << documentTypeToReadableName(getDocumentType()) << " Document" << endl << endl;
        /* patient related information */
        if (PatientsName.getLength() > 0)
        {
            stream << "Patient            : " << getStringValueFromElement(PatientsName, string);
            if ((PatientsSex.getLength() > 0) || (PatientsBirthDate.getLength() > 0))
            {
                stream << " (";
                if (PatientsSex.getLength() > 0)
                {
                    stream << getStringValueFromElement(PatientsSex, string);
                    if (PatientsBirthDate.getLength() > 0)
                        stream << ", ";
                }
                if (PatientsBirthDate.getLength() > 0)
                    stream << getStringValueFromElement(PatientsBirthDate, string);
                stream << ")";
            }
            stream << endl;
        }
        /* referring physician */
        if (ReferringPhysiciansName.getLength() > 0)
            stream << "Referring Physician: " << getStringValueFromElement(ReferringPhysiciansName, string) << endl;
        /* manufacturer */
        if (Manufacturer.getLength() > 0)
            stream << "Manufacturer       : " << getStringValueFromElement(Manufacturer, string) << endl;
        /* completion flag */
        stream << "Completion Flag    : " << completionFlagToEnumeratedValue(CompletionFlagEnum) << endl;
        if (CompletionFlagDescription.getLength() > 0)
            stream << "                     " << getStringValueFromElement(CompletionFlagDescription, string) << endl;
        /* predecessor documents */
        if (getNumberOfPredecessorDocuments() > 0)
            stream << "Predecessor Doc's  : " << getNumberOfPredecessorDocuments() << endl;
        /* verification flag */
        stream << "Verification Flag  : " << verificationFlagToEnumeratedValue(VerificationFlagEnum) << endl;
        /* verifying observer */
        const size_t obsCount = getNumberOfVerifyingObservers();
        for (size_t i = 1; i <= obsCount; i++)
        {            
            OFString dateTime, obsName, organization;
            DSRCodedEntryValue obsCode;
            if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization) == EC_Normal)
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
        stream << "Content Date/Time  : " << getStringValueFromElement(ContentDate, string) << ", ";
        stream <<                            getStringValueFromElement(ContentTime, string) << endl << endl;

        // --- dump document tree to stream ---
        result = DocumentTree.print(stream, flags);
    }
    return result;
}


E_Condition DSRDocument::checkDatasetForReading(DcmItem &dataset)
{
    E_Condition result = EC_Normal;
    OFString string;
    DcmUniqueIdentifier sopClassUID(DCM_SOPClassUID);
    DcmCodeString modality(DCM_Modality);
    /* check SOP class UID */
    result = getAndCheckElementFromDataset(dataset, sopClassUID, "1", "1", LogStream);
    if (result == EC_Normal)
    {
        E_DocumentType documentType = sopClassUIDToDocumentType(getStringValueFromElement(sopClassUID, string));
        if (documentType == DT_invalid)
        {
            printErrorMessage(LogStream, "SOPClassUID does not match one of the known SR document classes");
            result = EC_IllegalCall;
        }
        else if (!isDocumentTypeSupported(documentType))
        {
            printErrorMessage(LogStream, "Unsupported SOP Class UID (not yet implemented)");
            result = EC_IllegalCall;
        }
    }
    /* check modality */
    if (result == EC_Normal)
    {
        result = getAndCheckElementFromDataset(dataset, modality, "1", "1", LogStream);
        if (result == EC_Normal)
        {
            if (getStringValueFromElement(modality, string) != "SR")
            {
                printErrorMessage(LogStream, "Modality does not match 'SR' for structured reporting");
                result = EC_IllegalCall;
            }
        }
    }
    return result;
}


E_Condition DSRDocument::read(DcmItem &dataset)
{
    E_Condition result = EC_Normal;
    /* re-initialize SR document */
    clear();
    /* check SOP class UID and modality first */
    result = checkDatasetForReading(dataset);
    /* dataset is OK */
    if (result == EC_Normal)
    {
        /* type 3 element and attributes which have already been checked are not checked */

        // --- SOP Common Module ---
        getElementFromDataset(dataset, SOPClassUID);   /* already checked */
        getAndCheckElementFromDataset(dataset, SOPInstanceUID, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, SpecificCharacterSet, "1-n", "1C", LogStream);
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
        E_Condition searchCond = getSequenceFromDataset(dataset, ReferencedStudyComponent);
        checkElementValue(ReferencedStudyComponent, "1", "2", LogStream, searchCond);

        // --- SR Document General Module (M) ---
        getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, CompletionFlag, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, CompletionFlagDescription, "1", "3", LogStream);
        getAndCheckElementFromDataset(dataset, VerificationFlag, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, ContentDate, "1", "1", LogStream);
        getAndCheckElementFromDataset(dataset, ContentTime, "1", "1", LogStream);
        getSequenceFromDataset(dataset, VerifyingObserver);
        getSequenceFromDataset(dataset, PredecessorDocuments);

        /* update internal enumerated values */
        OFString string;
        CompletionFlagEnum = enumeratedValueToCompletionFlag(getStringValueFromElement(CompletionFlag, string));
        VerificationFlagEnum = enumeratedValueToVerificationFlag(getStringValueFromElement(VerificationFlag, string));

        /* read SR document tree */
        if (result == EC_Normal)
        {
            E_DocumentType documentType = sopClassUIDToDocumentType(getStringValueFromElement(SOPClassUID, string));
            result = DocumentTree.read(dataset, documentType);
        }
    }
    return result;
}


E_Condition DSRDocument::write(DcmItem &dataset)
{
    E_Condition result = EC_Normal;
    /* only write valid documents */
    if (isValid())
    {
        /* update DICOM attributes */
        updateAttributes();

        /* write general document attributes */

        // --- SOP Common Module ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPClassUID));
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPInstanceUID));
        if (SpecificCharacterSet.getLength() >0)
            addElementToDataset(result, dataset, new DcmCodeString(SpecificCharacterSet));
        if (InstanceCreationDate.getLength() >0)
            addElementToDataset(result, dataset, new DcmDate(InstanceCreationDate));
        if (InstanceCreationTime.getLength() >0)
            addElementToDataset(result, dataset, new DcmTime(InstanceCreationTime));
        if (InstanceCreatorUID.getLength() >0)
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(InstanceCreatorUID));

        // --- General Study Module ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(StudyInstanceUID));
        addElementToDataset(result, dataset, new DcmDate(StudyDate));
        addElementToDataset(result, dataset, new DcmTime(StudyTime));
        addElementToDataset(result, dataset, new DcmPersonName(ReferringPhysiciansName));
        addElementToDataset(result, dataset, new DcmShortString(StudyID));
        addElementToDataset(result, dataset, new DcmShortString(AccessionNumber));
        if (StudyDescription.getLength() >0)
            addElementToDataset(result, dataset, new DcmLongString(StudyDescription));

        // --- General series Module ---
        if (SeriesDescription.getLength() >0)
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
        addElementToDataset(result, dataset, new DcmSequenceOfItems(ReferencedStudyComponent));

        // --- SR Document General Module (M) ---
        addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber));
        addElementToDataset(result, dataset, new DcmCodeString(CompletionFlag));
        if (CompletionFlagDescription.getLength() > 0)
            addElementToDataset(result, dataset, new DcmLongString(CompletionFlagDescription));
        addElementToDataset(result, dataset, new DcmCodeString(VerificationFlag));
        addElementToDataset(result, dataset, new DcmDate(ContentDate));
        addElementToDataset(result, dataset, new DcmTime(ContentTime));
        if (VerifyingObserver.card() > 0)
            addElementToDataset(result, dataset, new DcmSequenceOfItems(VerifyingObserver));
        if (PredecessorDocuments.card() > 0)
            addElementToDataset(result, dataset, new DcmSequenceOfItems(PredecessorDocuments));

        /* write SR document tree */
        if (result == EC_Normal)
            result = DocumentTree.write(dataset);
    } else
        result = EC_CorruptedData;
    return result;
}


E_Condition DSRDocument::writeXML(ostream & /* stream */)
{
    /* not yet implemented */
    return EC_IllegalCall;
}


E_Condition DSRDocument::renderHTML(ostream &stream,
                                    const size_t flags)
{
    E_Condition result = EC_CorruptedData;
    /* only render valid documents */
    if (isValid())
    {
        OFString string;
        /* update DICOM attributes */
        updateAttributes();

        // --- HTML document structure (start) ---
        
        stream << "<html>" << endl;
        stream << "<head>" << endl;
        /* document type/title */
        stream << "<title>" << documentTypeToReadableName(getDocumentType()) << " Document</title>" << endl;
        stream << "</head>" << endl;
        stream << "<body>" << endl;

        // --- render some general document information ---
    
        /* create a table for this purpose */
        stream << "<table>" << endl;
        /* patient related information */
        if (PatientsName.getLength() > 0)
        {
            stream << "<tr>" << endl;
            stream << "<td><b>Patient:</b></td>" << endl;
            stream << "<td>" << getStringValueFromElement(PatientsName, string);
            if ((PatientsSex.getLength() > 0) || (PatientsBirthDate.getLength() > 0))
            {
                stream << " (";
                if (PatientsSex.getLength() > 0)
                {
                    stream << getStringValueFromElement(PatientsSex, string);
                    if (PatientsBirthDate.getLength() > 0)
                        stream << ", ";
                }
                if (PatientsBirthDate.getLength() > 0)
                    stream << getStringValueFromElement(PatientsBirthDate, string);
                stream << ")";
            }
            stream << "</td>" << endl;
            stream << "</tr>" << endl;
        }
        /* referring physician */
        if (ReferringPhysiciansName.getLength() > 0)
        {
            stream << "<tr>" << endl;
            stream << "<td><b>Referring Physician:</b></td>" << endl;
            stream << "<td>" << getStringValueFromElement(ReferringPhysiciansName, string) << "</td>" << endl;
            stream << "</tr>" << endl;
        }
        /* manufacturer */
        if (Manufacturer.getLength() > 0)
        {
            stream << "<tr>" << endl;
            stream << "<td><b>Manufacturer:</b></td>" << endl;
            stream << "<td>" << getStringValueFromElement(Manufacturer, string) << "</td>" << endl;
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
            stream << "<td>" << getStringValueFromElement(CompletionFlagDescription, string) << "</td>" << endl;
            stream << "</tr>" << endl;
        }
        /* predecessor documents */
        if (getNumberOfPredecessorDocuments() > 0)
        {
            stream << "<tr>" << endl;
            stream << "<td><b>Predecessor Doc's:</b></td>" << endl;
            stream << "<td>" << getNumberOfPredecessorDocuments() << "</td>" << endl;
            stream << "</tr>" << endl;
        }
        /* verification flag */
        stream << "<tr>" << endl;
        stream << "<td><b>Verification Flag:</b></td>" << endl;
        stream << "<td>" << verificationFlagToEnumeratedValue(VerificationFlagEnum) << "</td>" << endl;
        stream << "</tr>" << endl;
        /* verifying observer */
        const size_t obsCount = getNumberOfVerifyingObservers();
        for (size_t i = 1; i <= obsCount; i++)
        {
            OFString dateTime, obsName, organization;
            DSRCodedEntryValue obsCode;
            if (getVerifyingObserver(i, dateTime, obsName, obsCode, organization) == EC_Normal)
            {
                stream << "<tr>" << endl;
                stream << "<td></td>" << endl;
                stream << "<td>";
                stream << dateTime << ": " << obsName;
                if (obsCode.isValid() && (flags & HF_renderAllCodes))
                {
                    stream << " ";
                    obsCode.print(stream, OFTrue /* printCodeValue */);
                }
                stream << ", " << organization;
                stream << "</td>" << endl;
                stream << "</tr>" << endl;
            }
        }
        /* content date and time */        
        stream << "<tr>" << endl;
        stream << "<td><b>Content Date/Time:</b></td>" << endl;
        stream << "<td>" << getStringValueFromElement(ContentDate, string) << ", ";
        stream << getStringValueFromElement(ContentTime, string) << "</td>" << endl;  
        stream << "</tr>" << endl;
        /* end of table */
        stream << "</table>" << endl;
    
        // --- render document tree to stream ---

        stream << "<hr>" << endl;
        
        /* create memory output stream for the annex */
        ostrstream annexStream;
        /* render document tree two the streams */
        result = DocumentTree.renderHTML(stream, annexStream, flags);
        /* append annex (with heading) to main document */
        if (result == EC_Normal)
            result = appendStream(stream, annexStream, "<h1>Annex</h1>");

        // --- footnote ---
        
        stream << "<hr>" << endl;
        stream << "<small>" << endl;
        stream << "This page has been created automatically by 'dsr2html' from the ";
        stream << "<a href=\"http://www.offis.de/projekte/dicom/\">OFFIS dcmtk</a> package";
        stream << " (version " << OFFIS_DCMTK_VERSION << ")." << endl;
        stream << "</small>" << endl;

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


DSRTypes::E_CompletionFlag DSRDocument::getCompletionFlag() const
{
    return CompletionFlagEnum;
}


const char *DSRDocument::getCompletionFlagDescription() const
{
    return getStringValueFromElement(CompletionFlagDescription);
}


const OFString &DSRDocument::getCompletionFlagDescription(OFString &string) const
{
    return getStringValueFromElement(CompletionFlagDescription, string);
}


DSRTypes::E_VerificationFlag DSRDocument::getVerificationFlag() const
{
    return VerificationFlagEnum;
}


size_t DSRDocument::getNumberOfVerifyingObservers()
{
    return (size_t)VerifyingObserver.card();
}

    
E_Condition DSRDocument::getVerifyingObserver(const size_t idx,
                                              OFString &dateTime,
                                              OFString &observerName,
                                              OFString &organization)
{
    DSRCodedEntryValue dummyCode;
    return getVerifyingObserver(idx, dateTime, observerName, dummyCode, organization);
}


E_Condition DSRDocument::getVerifyingObserver(const size_t idx,
                                              OFString &dateTime,
                                              OFString &observerName,
                                              DSRCodedEntryValue &observerCode,
                                              OFString &organization)
{
    E_Condition result = EC_IllegalCall;
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
            if (result == EC_Normal)
                result = getStringValueFromDataset(*ditem, DCM_VerifyingObserverName, observerName);
            if (result == EC_Normal)
            {
                /* code is optional (type 2) */
                observerCode.readSequence(*ditem, DCM_VerifyingObserverIdentificationCodeSequence, "2" /* type */, LogStream);
                result = getStringValueFromDataset(*ditem, DCM_VerifyingOrganization, organization);
            }
            if (result == EC_Normal)
            {
                if ((dateTime.length() == 0) || (observerName.length() == 0) || (organization.length() == 0))
                    result = EC_CorruptedData;
            }
        }
    }
    return result;
}


size_t DSRDocument::getNumberOfPredecessorDocuments()
{
    return (size_t)PredecessorDocuments.card();
}

    
E_Condition DSRDocument::getPredecessorDocument(const size_t idx,
                                                OFString &sopClassUID,
                                                OFString &sopInstanceUID)
{
    E_Condition result = EC_IllegalCall;
    /* clear all reference variables */
    sopClassUID.clear();
    sopInstanceUID.clear();
    /* get specified entry */
    if ((idx > 0) && (idx <= getNumberOfPredecessorDocuments()))
    {
        DcmItem *ditem = PredecessorDocuments.getItem((unsigned long)(idx - 1));
        if (ditem != NULL)
        {
            result = getStringValueFromDataset(*ditem, DCM_SOPClassUID, sopClassUID);
            if (result == EC_Normal)
                result = getStringValueFromDataset(*ditem, DCM_SOPInstanceUID, sopInstanceUID);
            if (result == EC_Normal)
            {
                if ((sopClassUID.length() == 0) || (sopInstanceUID.length() == 0))
                    result = EC_CorruptedData;
            }
        }
    }
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

E_Condition DSRDocument::setSpecificCharacterSet(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return SpecificCharacterSet.putString(string.c_str());
}


E_Condition DSRDocument::setPatientsName(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientsName.putString(string.c_str());
}


E_Condition DSRDocument::setPatientsBirthDate(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientsBirthDate.putString(string.c_str());
}


E_Condition DSRDocument::setPatientsSex(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientsSex.putString(string.c_str());
}


E_Condition DSRDocument::setReferringPhysiciansName(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return ReferringPhysiciansName.putString(string.c_str());
}


E_Condition DSRDocument::setStudyDescription(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return StudyDescription.putString(string.c_str());
}


E_Condition DSRDocument::setSeriesDescription(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return SeriesDescription.putString(string.c_str());
}


E_Condition DSRDocument::setManufacturer(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return Manufacturer.putString(string.c_str());
}


E_Condition DSRDocument::setStudyID(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return StudyID.putString(string.c_str());
}


E_Condition DSRDocument::setPatientID(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return PatientID.putString(string.c_str());
}


E_Condition DSRDocument::setSeriesNumber(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return SeriesNumber.putString(string.c_str());
}


E_Condition DSRDocument::setInstanceNumber(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return InstanceNumber.putString(string.c_str());
}


E_Condition DSRDocument::setAccessionNumber(const OFString &string)
{
    /* might add check for correct format (VR) later on */
    return AccessionNumber.putString(string.c_str());
}


// --- document management functions

void DSRDocument::createNewSOPInstance()
{
    char uid[100];
    /* create new SOP instance UID */
    SOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    OFString string;
    /* set instance creation date to current date */
    InstanceCreationDate.putString(currentDate(string).c_str());
    /* set instance creation time to current time */
    InstanceCreationTime.putString(currentTime(string).c_str());
    /* update other DICOM attributes */
    updateAttributes();
}


E_Condition DSRDocument::createNewDocument()
{
    /* create new document with the same type as the current one */
    return createNewDocument(getDocumentType());
}


E_Condition DSRDocument::createNewDocument(const E_DocumentType documentType)
{
    /* document type is stored only once (namely in the document tree) */
    E_Condition result = DocumentTree.changeDocumentType(documentType);
    if (result == EC_Normal)
    {
        /* clear object */
        clear();
        /* set initial values for a new SOP instance */
        createNewSOPInstance();
    }
    return result;
}


E_Condition DSRDocument::createRevisedVersion()
{
    E_Condition result= EC_Normal;
    /* check whether document is not yet completed */
    if (CompletionFlagEnum != CF_Complete)
    {
        DcmItem *ditem = new DcmItem();
        if (ditem != NULL)
        {
            /* write current document UIDs */
            ditem->insert(new DcmUniqueIdentifier(SOPClassUID));
            ditem->insert(new DcmUniqueIdentifier(SOPInstanceUID));
            /* insert item into sequence (replace old ones) */
            PredecessorDocuments.clear();
            PredecessorDocuments.insert(ditem);
            /* create new instance UID and update creation date/time */
            createNewSOPInstance();
        } else
            result = EC_MemoryExhausted;
    } else
        result = EC_IllegalCall;
    return result;
}


E_Condition DSRDocument::completeDocument()
{
    /* complete document with empty/absent completion description */
    return completeDocument("");
}


E_Condition DSRDocument::completeDocument(const OFString &description)
{
    E_Condition result = EC_IllegalCall;
    /* if document is not already completed */
    if (CompletionFlagEnum != CF_Complete)
    {
        /* completed for now and ever */
        CompletionFlagEnum = CF_Complete;
        /* completion flag description */
        if (description.length() > 0)
            CompletionFlagDescription.putString(description.c_str());
        else
            CompletionFlagDescription.clear();
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRDocument::verifyDocument(const OFString &observerName,
                                        const OFString &organization)
{
    /* empty CodedEntryValue */
    return verifyDocument(observerName, DSRCodedEntryValue(), organization);
}


E_Condition DSRDocument::verifyDocument(const OFString &observerName,
                                        const DSRCodedEntryValue &observerCode,
                                        const OFString &organization)
{
    E_Condition result = EC_IllegalCall;
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
        } else
            result = EC_MemoryExhausted;
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


void DSRDocument::updateAttributes()
{
    /* retrieve SOP class UID from internal document type */
    SOPClassUID.putString(documentTypeToSOPClassUID(getDocumentType()));
    /* put static modality string */
    Modality.putString("SR");

    /* create new instance number if required */
    if (InstanceNumber.getLength() == 0)
        InstanceNumber.putString("1");
    /* create new series number if required */
    if (SeriesNumber.getLength() == 0)
        SeriesNumber.putString("1");

    char uid[100];
    /* create new study instance UID if required */
    if (StudyInstanceUID.getLength() == 0)
        StudyInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    /* create new study instance UID if required */
    if (SeriesInstanceUID.getLength() == 0)
        SeriesInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));

    /* check and adjust completion flag if required */
    if (CompletionFlagEnum == CF_invalid)
        CompletionFlagEnum = CF_Partial;
    CompletionFlag.putString(completionFlagToEnumeratedValue(CompletionFlagEnum));
    /* check and adjust verification flag if required */
    if (VerificationFlagEnum == VF_invalid)
        VerificationFlagEnum = VF_Unverified;
    VerificationFlag.putString(verificationFlagToEnumeratedValue(VerificationFlagEnum));

    /* check and set content date if required */
    if (ContentDate.getLength() == 0)
        ContentDate.putString(getStringValueFromElement(InstanceCreationDate));
    /* check and set content time if required */
    if (ContentTime.getLength() == 0)
        ContentTime.putString(getStringValueFromElement(InstanceCreationTime));
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoc.cc,v $
 *  Revision 1.6  2000-10-24 15:04:11  joergr
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
