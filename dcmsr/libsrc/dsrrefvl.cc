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
 *    classes: DSRReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:52:23 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrrefvl.h"


DSRReferenceValue::DSRReferenceValue()
  : SOPClassUID(),
    SOPInstanceUID()
{
}


DSRReferenceValue::DSRReferenceValue(const OFString &sopClassUID,
                                     const OFString &sopInstanceUID)
  : SOPClassUID(),
    SOPInstanceUID()
{
    /* use the set methods for checking purposes */
    setValue(sopClassUID, sopInstanceUID);
}


DSRReferenceValue::DSRReferenceValue(const DSRReferenceValue &referenceValue)
  : SOPClassUID(referenceValue.SOPClassUID),
    SOPInstanceUID(referenceValue.SOPInstanceUID)
{
    /* do not check since this would unexpected to the user */
}

    
DSRReferenceValue::~DSRReferenceValue()
{
}


DSRReferenceValue &DSRReferenceValue::operator=(const DSRReferenceValue &referenceValue)
{
    /* do not check since this would unexpected to the user */
    SOPClassUID = referenceValue.SOPClassUID;
    SOPInstanceUID = referenceValue.SOPInstanceUID;
    return *this;
}


void DSRReferenceValue::clear()
{
    SOPClassUID.clear();
    SOPInstanceUID.clear();
}


OFBool DSRReferenceValue::isValid() const
{
    return checkSOPClassUID(SOPClassUID) && checkSOPInstanceUID(SOPInstanceUID);
}


E_Condition DSRReferenceValue::print(ostream &stream,
                                     const size_t /* flags */) const
{
    const char *string = dcmFindNameOfUID(SOPClassUID.c_str());
    if (string != NULL)
        stream << " = " << string;
    else
        stream << " = unknown SOP class";
    return EC_Normal;
}


E_Condition DSRReferenceValue::readItem(DcmItem &dataset,
                                        OFConsole * /* logStream */)
{
    /* read ReferencedSOPClassUID */
    E_Condition result = DSRTypes::getStringValueFromDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID);
    /* read ReferencedSOPInstanceUID */
    if (result == EC_Normal)
        result = DSRTypes::getStringValueFromDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID);
    return result;
}


E_Condition DSRReferenceValue::writeItem(DcmItem &dataset,
                                         OFConsole * /*logStream */) const
{
    /* write ReferencedSOPClassUID */
    E_Condition result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID);
    /* write ReferencedSOPInstanceUID */
    if (result == EC_Normal)
        result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID);
    return result;
}


E_Condition DSRReferenceValue::readSequence(DcmItem &dataset,
                                            OFConsole *logStream)
{
    DcmStack stack;
    /* read ReferencedSOPSequence */
    E_Condition result = dataset.search(DCM_ReferencedSOPSequence, stack, ESM_fromHere, OFFalse);
    if (result == EC_Normal)
    {
        DcmSequenceOfItems *dseq = (DcmSequenceOfItems *)stack.top();
        if (dseq != NULL)
        {
            /* check for more or less than 1 item */
            if (dseq->card() < 1)
                DSRTypes::printWarningMessage(logStream, "ReferencedSOPSequence is empty");
            else if (dseq->card() > 1)
                DSRTypes::printWarningMessage(logStream, "ReferencedSOPSequence contains more than 1 item");
            /* read first item */
            DcmItem *ditem = dseq->getItem(0);
            if (ditem != NULL)
                result = readItem(*ditem, logStream);
            else
                result = EC_CorruptedData;
        } else
            result = EC_CorruptedData;
    }
    return result;
}


E_Condition DSRReferenceValue::writeSequence(DcmItem &dataset,
                                             OFConsole *logStream) const
{
    E_Condition result = EC_MemoryExhausted;
    /* write ReferencedSOPSequence */
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedSOPSequence);
    if (dseq != NULL)
    {
        DcmItem *ditem = new DcmItem();
        if (ditem != NULL)
        {
            /* write item */
            result = writeItem(*ditem, logStream);
            if (result == EC_Normal)
                dseq->insert(ditem);
            else
                delete ditem;
        } else
            result = EC_MemoryExhausted;
        /* write sequence */
        if (result == EC_Normal)
            dataset.insert(dseq);
        else
            delete dseq;
    }
    return result;
}


E_Condition DSRReferenceValue::renderHTML(ostream &docStream,
                                          ostream & /* annexStream */,
                                          size_t & /* annexNumber */,
                                          const size_t /* flags */,
                                          OFConsole * /* logStream */) const
{
    docStream << "<a href=\"dicom://localhost/composite/" << SOPInstanceUID << "\">";
    const char *string = dcmFindNameOfUID(SOPClassUID.c_str());
    if (string != NULL)
        docStream << string;
    else
        docStream << "unknown SOP class";
    docStream << "</a>" << endl;
    return EC_Normal;
}


E_Condition DSRReferenceValue::setSOPClassUID(const OFString &sopClassUID)
{
    E_Condition result = EC_IllegalCall;
    if (checkSOPClassUID(sopClassUID))
    {
        SOPClassUID = sopClassUID;
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRReferenceValue::getValue(DSRReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


E_Condition DSRReferenceValue::setSOPInstanceUID(const OFString &sopInstanceUID)
{
    E_Condition result = EC_IllegalCall;
    if (checkSOPInstanceUID(sopInstanceUID))
    {
        SOPInstanceUID = sopInstanceUID;
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRReferenceValue::setValue(const OFString &sopClassUID,
                                        const OFString &sopInstanceUID)
{
    E_Condition result = setSOPClassUID(sopClassUID);
    if (result == EC_Normal)
        result = setSOPInstanceUID(sopInstanceUID);
    return result;
}


E_Condition DSRReferenceValue::setValue(const DSRReferenceValue &referenceValue)
{
    E_Condition result = setSOPClassUID(referenceValue.SOPClassUID);
    if (result == EC_Normal)
        result = setSOPInstanceUID(referenceValue.SOPInstanceUID);
    return result;
}


OFBool DSRReferenceValue::checkSOPClassUID(const OFString &sopClassUID) const
{
    return (sopClassUID.length() > 0);
}


OFBool DSRReferenceValue::checkSOPInstanceUID(const OFString &sopInstanceUID) const
{
    return (sopInstanceUID.length() > 0);
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrrefvl.cc,v $
 *  Revision 1.1  2000-10-13 07:52:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
