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
 *    classes: DSRCodedEntryValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-19 16:03:20 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrcodvl.h"


DSRCodedEntryValue::DSRCodedEntryValue()
  : CodeValue(),
    CodingSchemeDesignator(),
    CodingSchemeVersion(),
    CodeMeaning(),
    PrivateCodingSchemeCreatorUID()
{
}


DSRCodedEntryValue::DSRCodedEntryValue(const OFString &codeValue,
                                       const OFString &codingSchemeDesignator, 
                                       const OFString &codeMeaning)
  : CodeValue(codeValue),
    CodingSchemeDesignator(codingSchemeDesignator),
    CodingSchemeVersion(),
    CodeMeaning(codeMeaning),
    PrivateCodingSchemeCreatorUID()
{
    /* check code */
    setCode(codeValue, codingSchemeDesignator, codeMeaning);
}


DSRCodedEntryValue::DSRCodedEntryValue(const OFString &codeValue,
                                       const OFString &codingSchemeDesignator,
                                       const OFString &codingSchemeVersion,
                                       const OFString &codeMeaning)
  : CodeValue(),
    CodingSchemeDesignator(),
    CodingSchemeVersion(),
    CodeMeaning(),
    PrivateCodingSchemeCreatorUID()
{
    /* check code */
    setCode(codeValue, codingSchemeDesignator, codingSchemeVersion, codeMeaning);
}


DSRCodedEntryValue::DSRCodedEntryValue(const DSRCodedEntryValue &codedEntryValue)
  : CodeValue(codedEntryValue.CodeValue),
    CodingSchemeDesignator(codedEntryValue.CodingSchemeDesignator),
    CodingSchemeVersion(codedEntryValue.CodingSchemeVersion),
    CodeMeaning(codedEntryValue.CodeMeaning),
    PrivateCodingSchemeCreatorUID(codedEntryValue.PrivateCodingSchemeCreatorUID)
{
    /* do not check since this would unexpected to the user */
}


DSRCodedEntryValue::~DSRCodedEntryValue()
{
}


DSRCodedEntryValue &DSRCodedEntryValue::operator=(const DSRCodedEntryValue &codedEntryValue)
{
    /* do not check since this would unexpected to the user */
    CodeValue = codedEntryValue.CodeValue;
    CodingSchemeDesignator = codedEntryValue.CodingSchemeDesignator;
    CodingSchemeVersion = codedEntryValue.CodingSchemeVersion;
    CodeMeaning = codedEntryValue.CodeMeaning;
    PrivateCodingSchemeCreatorUID = codedEntryValue.PrivateCodingSchemeCreatorUID;
    return *this;
}


OFBool DSRCodedEntryValue::operator==(const DSRCodedEntryValue &codedEntryValue) const
{
    return (CodeValue == codedEntryValue.CodeValue) &&
           (CodingSchemeDesignator == codedEntryValue.CodingSchemeDesignator) &&
           (CodingSchemeVersion == codedEntryValue.CodingSchemeVersion);
}


void DSRCodedEntryValue::clear()
{
    CodeValue.clear();
    CodingSchemeDesignator.clear();
    CodingSchemeVersion.clear();
    CodeMeaning.clear();
    PrivateCodingSchemeCreatorUID.clear();
}


OFBool DSRCodedEntryValue::isValid() const
{
    return checkCode(CodeValue, CodingSchemeDesignator, CodeMeaning);
}


OFBool DSRCodedEntryValue::isEmpty() const
{
    return (CodeValue.length() == 0) && (CodingSchemeDesignator.length() == 0) &&
           (CodingSchemeVersion.length() == 0) && (CodeMeaning.length() == 0);
}


OFBool DSRCodedEntryValue::isPrivateDcmtkCodingScheme() const
{
    return (CodingSchemeDesignator == OFFIS_CODING_SCHEME_DESIGNATOR) &&
           (PrivateCodingSchemeCreatorUID == OFFIS_PRIVATE_CODING_SCHEME_CREATOR_UID);
}


void DSRCodedEntryValue::print(ostream &stream,
                               const OFBool printCodeValue,
                               const OFBool printInvalid) const
{
    if (isValid())
    {
        stream << "(";
        if (printCodeValue)
        {
            stream << CodeValue << "," << CodingSchemeDesignator << ",";
            if (CodingSchemeVersion.length() > 0)
                stream << CodingSchemeVersion << ",";
        } else
            stream << ",,";
        stream << "\"" << CodeMeaning << "\")";
    }
    else if (printInvalid)
        stream << "invalid code";
}


E_Condition DSRCodedEntryValue::readItem(DcmItem &dataset,
                                         OFConsole *logStream,
                                         const char *moduleName)
{
    /* read BasicCodedEntryAttributes only */
    E_Condition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_CodeValue, CodeValue, "1", "1", logStream, moduleName);
    if (result == EC_Normal)
        result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_CodingSchemeDesignator, CodingSchemeDesignator, "1", "1", logStream, moduleName);
    if (result == EC_Normal)                                             /* conditional (type 1C) */
        DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_CodingSchemeVersion, CodingSchemeVersion, "1", "1C", logStream, moduleName);
    if (result == EC_Normal)
    {
        result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_CodeMeaning, CodeMeaning, "1", "1", logStream, moduleName);
        /* optional (type 3) */
        DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_PrivateCodingSchemeCreatorUID, PrivateCodingSchemeCreatorUID, "1", "3", logStream, moduleName);
    }
    /* tbd: might add check for correct code */
        
    return result;
}


E_Condition DSRCodedEntryValue::writeItem(DcmItem &dataset,
                                          OFConsole * /* logStream */) const
{
    /* write BasicCodedEntryAttributes only */
    E_Condition result = DSRTypes::putStringValueToDataset(dataset, DCM_CodeValue, CodeValue);
    if (result == EC_Normal)
        result = DSRTypes::putStringValueToDataset(dataset, DCM_CodingSchemeDesignator, CodingSchemeDesignator);
    if ((result == EC_Normal) && (CodingSchemeVersion.length() > 0))                /* conditional (type 1C) */
        result = DSRTypes::putStringValueToDataset(dataset, DCM_CodingSchemeVersion, CodingSchemeVersion);
    if (result == EC_Normal)
        result = DSRTypes::putStringValueToDataset(dataset, DCM_CodeMeaning, CodeMeaning);
    if ((result == EC_Normal) && (PrivateCodingSchemeCreatorUID.length() > 0))      /* optional (type 3) */
        result = DSRTypes::putStringValueToDataset(dataset, DCM_PrivateCodingSchemeCreatorUID, PrivateCodingSchemeCreatorUID);
    return result;
}


E_Condition DSRCodedEntryValue::readSequence(DcmItem &dataset,
                                             const DcmTagKey &tagKey,
                                             const OFString &type,
                                             OFConsole *logStream)
{
    /* read CodeSequence */
    DcmSequenceOfItems dseq(tagKey);
    E_Condition result = DSRTypes::getSequenceFromDataset(dataset, dseq);
    DSRTypes::checkElementValue(dseq, "1", type, logStream, result, "content item");
    if (result == EC_Normal)
    {
        DcmItem *ditem = dseq.getItem(0);
        if (ditem != NULL)
        {            
            /* read Code */
            result = readItem(*ditem, logStream, DcmTag(tagKey).getTagName());
        } else
            result = EC_CorruptedData;
    }
    return result;
}


E_Condition DSRCodedEntryValue::writeSequence(DcmItem &dataset,
                                              const DcmTagKey &tagKey,
                                              OFConsole *logStream) const
{
    E_Condition result = EC_MemoryExhausted;
    /* write CodeSequence */
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(tagKey);
    if (dseq != NULL)
    {
        /* check for empty value */
        if (isEmpty())
            result = EC_Normal;
        else
        {
            DcmItem *ditem = new DcmItem();
            if (ditem != NULL)
            {
                /* write Code */
                if (isValid())
                    result = writeItem(*ditem, logStream);
                if (result == EC_Normal)
                    dseq->insert(ditem);
                else
                    delete ditem;
            } else
                result = EC_MemoryExhausted;
        }
        if (result == EC_Normal)
            dataset.insert(dseq);
        else
            delete dseq;
    }
    return result;
}


E_Condition DSRCodedEntryValue::renderHTML(ostream &stream,
                                           OFConsole * /* logStream */) const
{
    stream << CodeMeaning << " (" << CodingSchemeDesignator << ", "; 
    if (CodingSchemeVersion.length() > 0)
        stream << CodingSchemeVersion << ", ";
    stream << CodeValue << ")";
    return EC_Normal;
}


E_Condition DSRCodedEntryValue::getValue(DSRCodedEntryValue &codedEntryValue) const
{
    codedEntryValue = *this;
    return EC_Normal;
}


E_Condition DSRCodedEntryValue::setValue(const DSRCodedEntryValue &codedEntryValue)
{
    return setCode(codedEntryValue.CodeValue, codedEntryValue.CodingSchemeDesignator,
        codedEntryValue.CodingSchemeVersion, codedEntryValue.CodeMeaning);
}


E_Condition DSRCodedEntryValue::setCode(const OFString &codeValue,
                                        const OFString &codingSchemeDesignator,
                                        const OFString &codeMeaning)
{
    return setCode(codeValue, codingSchemeDesignator, "", codeMeaning);
}


E_Condition DSRCodedEntryValue::setCode(const OFString &codeValue,
                                        const OFString &codingSchemeDesignator,
                                        const OFString &codingSchemeVersion,
                                        const OFString &codeMeaning)
{
    E_Condition result = EC_Normal;
    if (checkCode(codeValue, codingSchemeDesignator, codeMeaning))
    {
        /* copy attributes */
        CodeValue = codeValue;
        CodingSchemeDesignator = codingSchemeDesignator;
        CodingSchemeVersion = codingSchemeVersion;
        CodeMeaning = codeMeaning;
        PrivateCodingSchemeCreatorUID.clear();
        /* check for private OFFIS dcmtk CodingSchemeDesignator */
        if (CodingSchemeDesignator == OFFIS_CODING_SCHEME_DESIGNATOR)
            PrivateCodingSchemeCreatorUID = OFFIS_PRIVATE_CODING_SCHEME_CREATOR_UID;
    } else
        result = EC_IllegalCall;
    return result;
}


OFBool DSRCodedEntryValue::checkCode(const OFString &codeValue,
                                     const OFString &codingSchemeDesignator,
                                     const OFString &codeMeaning) const
{
    /* need to check correctness of the code (code dictionary?) */
    return (codeValue.length() > 0) && (codingSchemeDesignator.length() > 0) && (codeMeaning.length() > 0);
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcodvl.cc,v $
 *  Revision 1.3  2000-10-19 16:03:20  joergr
 *  Renamed some set methods.
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.2  2000/10/18 17:13:36  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.1  2000/10/13 07:52:17  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
