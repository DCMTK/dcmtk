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
 *    classes: DSRCompositeReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcomvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


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
    return SOPClassUID.empty() && SOPInstanceUID.empty();
}


OFCondition DSRCompositeReferenceValue::print(STD_NAMESPACE ostream &stream,
                                              const size_t flags) const
{
    const char *className = dcmFindNameOfUID(SOPClassUID.c_str());
    stream << "(";
    if (className != NULL)
        stream << className;
    else
        stream << "\"" << SOPClassUID << "\"";
    stream << ",";
    if (flags & DSRTypes::PF_printSOPInstanceUID)
        stream << "\"" << SOPInstanceUID << "\"";
    stream << ")";
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::readXML(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    /* go one node level down */
    if (cursor.gotoChild().valid())
    {
        /* retrieve SOP Class UID and SOP Instance UID from XML tag (required) */
        doc.getStringFromAttribute(doc.getNamedNode(cursor, "sopclass"), SOPClassUID, "uid");
        doc.getStringFromAttribute(doc.getNamedNode(cursor, "instance"), SOPInstanceUID, "uid");
        /* check whether value is valid */
        result = (isValid() ? EC_Normal : SR_EC_InvalidValue);
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::writeXML(STD_NAMESPACE ostream &stream,
                                                 const size_t flags) const
{
    if ((flags & DSRTypes::XF_writeEmptyTags) || !isEmpty())
    {
        stream << "<sopclass uid=\"" << SOPClassUID << "\">";
        /* retrieve name of SOP class */
        const char *sopClass = dcmFindNameOfUID(SOPClassUID.c_str());
        if (sopClass != NULL)
            stream << sopClass;
        stream << "</sopclass>" << OFendl;
        stream << "<instance uid=\"" << SOPInstanceUID << "\"/>" << OFendl;
    }
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::readItem(DcmItem &dataset)
{
    /* read ReferencedSOPClassUID */
    OFCondition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID, "1", "1", "ReferencedSOPSequence");
    /* read ReferencedSOPInstanceUID */
    if (result.good())
        result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID, "1", "1", "ReferencedSOPSequence");
    return result;
}


OFCondition DSRCompositeReferenceValue::writeItem(DcmItem &dataset) const
{
    /* write ReferencedSOPClassUID */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID);
    /* write ReferencedSOPInstanceUID */
    if (result.good())
        result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID);
    return result;
}


OFCondition DSRCompositeReferenceValue::readSequence(DcmItem &dataset,
                                                     const OFString &type)
{
    /* read ReferencedSOPSequence */
    DcmSequenceOfItems dseq(DCM_ReferencedSOPSequence);
    OFCondition result = DSRTypes::getElementFromDataset(dataset, dseq);
    DSRTypes::checkElementValue(dseq, "1", type, result, "content item");
    if (result.good())
    {
        /* read first item */
        DcmItem *ditem = dseq.getItem(0);
        if (ditem != NULL)
            result = readItem(*ditem);
        else
            result = SR_EC_InvalidDocumentTree;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::writeSequence(DcmItem &dataset) const
{
    OFCondition result = EC_MemoryExhausted;
    /* write ReferencedSOPSequence */
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedSOPSequence);
    if (dseq != NULL)
    {
        DcmItem *ditem = new DcmItem();
        if (ditem != NULL)
        {
            /* write item */
            result = writeItem(*ditem);
            if (result.good())
                dseq->insert(ditem);
            else
                delete ditem;
        } else
            result = EC_MemoryExhausted;
        /* write sequence */
        if (result.good())
            result = dataset.insert(dseq, OFTrue /*replaceOld*/);
        if (result.bad())
            delete dseq;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream & /*annexStream*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t /*flags*/) const
{
    /* render reference */
    docStream << "<a href=\"" << HTML_HYPERLINK_PREFIX_FOR_CGI;
    docStream << "?composite=" << SOPClassUID << "+" << SOPInstanceUID << "\">";
    const char *className = dcmFindNameOfUID(SOPClassUID.c_str());
    if (className != NULL)
        docStream << className;
    else
        docStream << "unknown composite object";
    docStream << "</a>";
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::getValue(DSRCompositeReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::setValue(const DSRCompositeReferenceValue &referenceValue)
{
    return setReference(referenceValue.SOPClassUID, referenceValue.SOPInstanceUID);
}


OFCondition DSRCompositeReferenceValue::setReference(const OFString &sopClassUID,
                                                     const OFString &sopInstanceUID)
{
    OFCondition result = EC_IllegalParameter;
    /* check both values before setting them */
    if (checkSOPClassUID(sopClassUID) && checkSOPInstanceUID(sopInstanceUID))
    {
        SOPClassUID = sopClassUID;
        SOPInstanceUID = sopInstanceUID;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::setSOPClassUID(const OFString &sopClassUID)
{
    OFCondition result = EC_IllegalParameter;
    if (checkSOPClassUID(sopClassUID))
    {
        SOPClassUID = sopClassUID;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::setSOPInstanceUID(const OFString &sopInstanceUID)
{
    OFCondition result = EC_IllegalParameter;
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
 *  Revision 1.20  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.19  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.18  2008-07-17 12:00:09  joergr
 *  Replaced call to getSequenceFromDataset() by getElementFromDataset().
 *
 *  Revision 1.17  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.16  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.15  2005/12/08 15:47:44  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.14  2003/08/07 13:14:26  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.13  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.12  2002/05/07 12:51:30  joergr
 *  Added output of SOP class name to XML document.
 *
 *  Revision 1.11  2001/10/10 15:29:50  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.10  2001/10/02 12:07:07  joergr
 *  Adapted module "dcmsr" to the new class OFCondition. Introduced module
 *  specific error codes.
 *
 *  Revision 1.9  2001/09/28 14:09:51  joergr
 *  Check return value of DcmItem::insert() statements to avoid memory leaks
 *  when insert procedure failes.
 *
 *  Revision 1.8  2001/09/26 13:04:18  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:51:08  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/11/06 11:31:46  joergr
 *  Changes structure of HTML hyperlinks to composite objects (now using pseudo
 *  CGI script).
 *
 *  Revision 1.5  2000/11/01 16:30:32  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.4  2000/10/26 14:27:23  joergr
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
