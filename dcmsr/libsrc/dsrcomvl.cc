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
 *    classes: DSRCompositeReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:27:23 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrcomvl.h"


DSRCompositeReferenceValue::DSRCompositeReferenceValue()
  : SOPClassUID(),
    SOPInstanceUID()
{
}


DSRCompositeReferenceValue::DSRCompositeReferenceValue(const OFString &sopClassUID,
                                                       const OFString &sopInstanceUID)
  : SOPClassUID(),
    SOPInstanceUID()
{
    /* use the set methods for checking purposes */
    setReference(sopClassUID, sopInstanceUID);
}


DSRCompositeReferenceValue::DSRCompositeReferenceValue(const DSRCompositeReferenceValue &referenceValue)
  : SOPClassUID(referenceValue.SOPClassUID),
    SOPInstanceUID(referenceValue.SOPInstanceUID)
{
    /* do not check since this would unexpected to the user */
}

    
DSRCompositeReferenceValue::~DSRCompositeReferenceValue()
{
}


DSRCompositeReferenceValue &DSRCompositeReferenceValue::operator=(const DSRCompositeReferenceValue &referenceValue)
{
    /* do not check since this would unexpected to the user */
    SOPClassUID = referenceValue.SOPClassUID;
    SOPInstanceUID = referenceValue.SOPInstanceUID;
    return *this;
}


void DSRCompositeReferenceValue::clear()
{
    SOPClassUID.clear();
    SOPInstanceUID.clear();
}


OFBool DSRCompositeReferenceValue::isValid() const
{
    return checkSOPClassUID(SOPClassUID) && checkSOPInstanceUID(SOPInstanceUID);
}


OFBool DSRCompositeReferenceValue::isEmpty() const
{
    return (SOPClassUID.length() == 0) && (SOPInstanceUID.length() == 0);
}


E_Condition DSRCompositeReferenceValue::print(ostream &stream,
                                              const size_t flags) const
{
    const char *string = dcmFindNameOfUID(SOPClassUID.c_str());
    stream << "(";
    if (string != NULL)
        stream << string;
    else
        stream << "\"" << SOPClassUID << "\"";
    stream << ",";
    if (flags & DSRTypes::PF_printSOPInstanceUID)
        stream << "\"" << SOPInstanceUID << "\"";
    stream << ")";
    return EC_Normal;
}


E_Condition DSRCompositeReferenceValue::readItem(DcmItem &dataset,
                                                 OFConsole *logStream)
{
    /* read ReferencedSOPClassUID */
    E_Condition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID, "1", "1", logStream, "ReferencedSOPSequence");
    /* read ReferencedSOPInstanceUID */
    if (result == EC_Normal)
        result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID, "1", "1", logStream, "ReferencedSOPSequence");
    return result;
}


E_Condition DSRCompositeReferenceValue::writeItem(DcmItem &dataset,
                                                  OFConsole * /*logStream */) const
{
    /* write ReferencedSOPClassUID */
    E_Condition result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID);
    /* write ReferencedSOPInstanceUID */
    if (result == EC_Normal)
        result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID);
    return result;
}


E_Condition DSRCompositeReferenceValue::readSequence(DcmItem &dataset,
                                                     const OFString &type,
                                                     OFConsole *logStream)
{
    /* read ReferencedSOPSequence */
    DcmSequenceOfItems dseq(DCM_ReferencedSOPSequence);
    E_Condition result = DSRTypes::getSequenceFromDataset(dataset, dseq);
    DSRTypes::checkElementValue(dseq, "1", type, logStream, result, "content item");
    if (result == EC_Normal)
    {
        /* read first item */
        DcmItem *ditem = dseq.getItem(0);
        if (ditem != NULL)
            result = readItem(*ditem, logStream);
        else
            result = EC_CorruptedData;
    }
    return result;
}


E_Condition DSRCompositeReferenceValue::writeSequence(DcmItem &dataset,
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


E_Condition DSRCompositeReferenceValue::renderHTML(ostream &docStream,
                                                   ostream & /* annexStream */,
                                                   size_t & /* annexNumber */,
                                                   const size_t /* flags */,
                                                   OFConsole * /* logStream */) const
{
    docStream << "<a href=\"file://dicom/composite/" << SOPClassUID << "/" << SOPInstanceUID << "\">";
    const char *string = dcmFindNameOfUID(SOPClassUID.c_str());
    if (string != NULL)
        docStream << string;
    else
        docStream << "unknown composite object";
    docStream << "</a>" << endl;
    return EC_Normal;
}


E_Condition DSRCompositeReferenceValue::getValue(DSRCompositeReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


E_Condition DSRCompositeReferenceValue::setValue(const DSRCompositeReferenceValue &referenceValue)
{    
    return setReference(referenceValue.SOPClassUID, referenceValue.SOPInstanceUID);
}


E_Condition DSRCompositeReferenceValue::setReference(const OFString &sopClassUID,
                                                     const OFString &sopInstanceUID)
{
    E_Condition result = EC_IllegalCall;
    /* check both values before setting them */
    if (checkSOPClassUID(sopClassUID) && checkSOPInstanceUID(sopInstanceUID))
    {
        SOPClassUID = sopClassUID;
        SOPInstanceUID = sopInstanceUID;
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRCompositeReferenceValue::setSOPClassUID(const OFString &sopClassUID)
{
    E_Condition result = EC_IllegalCall;
    if (checkSOPClassUID(sopClassUID))
    {
        SOPClassUID = sopClassUID;
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRCompositeReferenceValue::setSOPInstanceUID(const OFString &sopInstanceUID)
{
    E_Condition result = EC_IllegalCall;
    if (checkSOPInstanceUID(sopInstanceUID))
    {
        SOPInstanceUID = sopInstanceUID;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRCompositeReferenceValue::checkSOPClassUID(const OFString &sopClassUID) const
{
    return DSRTypes::checkForValidUIDFormat(sopClassUID);
}


OFBool DSRCompositeReferenceValue::checkSOPInstanceUID(const OFString &sopInstanceUID) const
{
    return DSRTypes::checkForValidUIDFormat(sopInstanceUID);
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcomvl.cc,v $
 *  Revision 1.4  2000-10-26 14:27:23  joergr
 *  Added check routine for valid UID strings.
 *
 *  Revision 1.3  2000/10/24 15:04:11  joergr
 *  Changed HTML hyperlinks to referenced objects from "dicom://" to "file://"
 *  to facilitate access from Java.
 *
 *  Revision 1.2  2000/10/23 15:01:06  joergr
 *  Added SOP class UID to hyperlink in method renderHTML().
 *
 *  Revision 1.1  2000/10/20 10:14:57  joergr
 *  Renamed class DSRReferenceValue to DSRCompositeReferenceValue.
 *
 *  Revision 1.4  2000/10/19 16:05:46  joergr
 *  Renamed some set methods.
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.3  2000/10/18 17:20:45  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:08:02  joergr
 *  Reformatted print output.
 *  Added new options: number nested items instead of indenting them, print SOP
 *  instance UID of referenced composite objects.
 *
 *  Revision 1.1  2000/10/13 07:52:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
