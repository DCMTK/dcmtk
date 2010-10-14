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
 *    classes: DSRPNameTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrpnmtn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRPNameTreeNode::DSRPNameTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_PName),
   DSRStringValue()
{
}


DSRPNameTreeNode::DSRPNameTreeNode(const E_RelationshipType relationshipType,
                                   const OFString &stringValue)
 : DSRDocumentTreeNode(relationshipType, VT_PName),
   DSRStringValue(stringValue)
{
}


DSRPNameTreeNode::~DSRPNameTreeNode()
{
}


void DSRPNameTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRPNameTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


OFCondition DSRPNameTreeNode::print(STD_NAMESPACE ostream &stream,
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


OFCondition DSRPNameTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                       const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    if (!getValue().empty() || (flags & XF_writeEmptyTags))
    {
        OFString tmpString;
        stream << "<value>" << OFendl << dicomToXMLPersonName(getValue(), tmpString) << OFendl << "</value>" << OFendl;
    }
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRPNameTreeNode::readContentItem(DcmItem &dataset)
{
    /* read PName */
    return DSRStringValue::read(dataset, DCM_PersonName);
}


OFCondition DSRPNameTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write PName */
    return DSRStringValue::write(dataset, DCM_PersonName);
}


OFCondition DSRPNameTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                 DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        /* goto sub-node "value" */
        cursor = doc.getNamedNode(cursor.getChild(), "value").getChild();
        if (cursor.valid())
        {
            /* retrieve person name from XML tag */
            OFString nameString;
            getValueFromXMLNodeContent(doc, cursor, nameString);
            /* set retrieved value */
            result = DSRStringValue::setValue(nameString);
        }
    }
    return result;
}


OFString &DSRPNameTreeNode::getValueFromXMLNodeContent(const DSRXMLDocument &doc,
                                                       DSRXMLCursor cursor,
                                                       OFString &nameValue)
{
    nameValue.clear();
    /* check whether node is valid */
    if (cursor.valid())
    {
        OFString first, middle, last, suffix, prefix;
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            doc.getStringFromNodeContent(cursor, prefix, "prefix", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, first, "first", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, middle, "middle", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, last, "last", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, suffix, "suffix", OFTrue /*encoding*/, OFFalse /*clearString*/);
            /* proceed with next node */
            cursor.gotoNext();
        }
        /* create DICOM Person Name (PN) from name components */
        DcmPersonName::getStringFromNameComponents(last, first, middle, prefix, suffix, nameValue);
    }
    return nameValue;
}


OFCondition DSRPNameTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                    STD_NAMESPACE ostream & /*annexStream*/,
                                                    const size_t /*nestingLevel*/,
                                                    size_t & /*annexNumber*/,
                                                    const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render PName */
    if (result.good())
    {
        OFString tmpString, htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
        {
            if (flags & DSRTypes::HF_XHTML11Compatibility)
                docStream << "<span class=\"pname\">";
            else if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "<u>";
            else /* HTML 4.01 */
                docStream << "<span class=\"under\">";
        }
        docStream << convertToHTMLString(dicomToReadablePersonName(getValue(), tmpString), htmlString, flags);
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
 *  $Log: dsrpnmtn.cc,v $
 *  Revision 1.27  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.26  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.25  2007-11-15 16:45:26  joergr
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.24  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.23  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.22  2005/12/08 15:48:01  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.21  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.20  2003/11/06 15:10:48  joergr
 *  Added missing line break in XML output.
 *
 *  Revision 1.19  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.18  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.17  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.16  2003/08/07 13:41:36  joergr
 *  Added readXML functionality.
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
 *
 *  Revision 1.15  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.14  2001/11/09 16:17:53  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.13  2001/10/10 15:29:58  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.12  2001/09/26 13:04:23  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.11  2001/05/07 16:14:24  joergr
 *  Updated CVS header.
 *
 *  Revision 1.10  2001/02/02 14:41:52  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.9  2001/01/18 15:56:18  joergr
 *  Encode PN components in separate XML tags.
 *
 *  Revision 1.8  2000/11/09 20:34:01  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.7  2000/11/07 18:31:13  joergr
 *  Enhanced support for by-reference relationships.
 *  Enhanced rendered HTML output of date, time, datetime and pname.
 *
 *  Revision 1.6  2000/11/01 16:37:02  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.5  2000/10/26 14:32:09  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:46  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:20:20  joergr
 *  Moved read and write methods to base class.
 *
 *  Revision 1.2  2000/10/16 12:06:21  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
