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
 *    classes: DSRDateTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrdattn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRDateTreeNode::DSRDateTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Date),
   DSRStringValue()
{
}


DSRDateTreeNode::DSRDateTreeNode(const E_RelationshipType relationshipType,
                                 const OFString &stringValue)
 : DSRDocumentTreeNode(relationshipType, VT_Date),
   DSRStringValue(stringValue)
{
}


DSRDateTreeNode::~DSRDateTreeNode()
{
}


void DSRDateTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRDateTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


OFCondition DSRDateTreeNode::print(STD_NAMESPACE ostream &stream,
                                   const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        DSRStringValue::print(stream);
    }
    return result;
}


OFCondition DSRDateTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                      const size_t flags) const
{
    OFString tmpString;
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    /* output date in ISO 8601 format */
    DcmDate::getISOFormattedDateFromString(getValue(), tmpString);
    writeStringValueToXML(stream, tmpString, "value", (flags & XF_writeEmptyTags) > 0);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRDateTreeNode::readContentItem(DcmItem &dataset)
{
    /* read Date */
    return DSRStringValue::read(dataset, DCM_Date);
}


OFCondition DSRDateTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write Date */
    return DSRStringValue::write(dataset, DCM_Date);
}


OFCondition DSRDateTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor)
{
    OFString tmpString;
    /* retrieve value from XML element "value" */
    OFCondition result = setValue(getValueFromXMLNodeContent(doc, doc.getNamedNode(cursor.gotoChild(), "value"), tmpString));
    if (result == EC_IllegalParameter)
        result = SR_EC_InvalidValue;
    return result;
}


OFString &DSRDateTreeNode::getValueFromXMLNodeContent(const DSRXMLDocument &doc,
                                                      DSRXMLCursor cursor,
                                                      OFString &dateValue,
                                                      const OFBool clearString)
{
    if (clearString)
        dateValue.clear();
    /* check whether node is valid */
    if (cursor.valid())
    {
        OFString tmpString;
        /* retrieve value from XML element */
        if (!doc.getStringFromNodeContent(cursor, tmpString).empty())
        {
            OFDate tmpDate;
            /* convert ISO to DICOM format */
            if (tmpDate.setISOFormattedDate(tmpString))
                DcmDate::getDicomDateFromOFDate(tmpDate, dateValue);
        }
    }
    return dateValue;
}


OFCondition DSRDateTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream & /*annexStream*/,
                                                   const size_t /*nestingLevel*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render Date */
    if (result.good())
    {
        OFString htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
        {
            if (flags & DSRTypes::HF_XHTML11Compatibility)
                docStream << "<span class=\"date\">";
            else if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "<u>";
            else /* HTML 4.01 */
                docStream << "<span class=\"under\">";
        }
        docStream << dicomToReadableDate(getValue(), htmlString);
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
        {
            if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "</u>";
            else
                docStream << "</span>";
        }
        docStream << OFendl;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrdattn.cc,v $
 *  Revision 1.23  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.22  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.21  2007-11-15 16:45:26  joergr
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.20  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.19  2005/12/08 15:47:47  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.18  2004/01/16 10:17:04  joergr
 *  Adapted XML output format of Date, Time and Datetime to XML Schema (ISO
 *  8601) requirements.
 *
 *  Revision 1.17  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.16  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.15  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.14  2003/08/07 13:16:51  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.13  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.12  2001/11/09 16:14:32  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.11  2001/10/10 15:29:51  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.10  2001/09/26 13:04:19  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.9  2001/05/07 16:14:23  joergr
 *  Updated CVS header.
 *
 *  Revision 1.8  2001/02/02 14:41:54  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.7  2000/11/07 18:30:21  joergr
 *  Enhanced support for by-reference relationships.
 *  Enhanced rendered HTML output of date, time, datetime and pname.
 *
 *  Revision 1.6  2000/11/01 16:31:39  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.5  2000/10/26 14:28:14  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:45  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:15:09  joergr
 *  Moved read and write methods to base class.
 *
 *  Revision 1.2  2000/10/16 12:01:56  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:18  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
