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
 *    classes: DSRTextTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtextn.h"
#include "dcmtk/dcmsr/dsrstrvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRTextTreeNode::DSRTextTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Text),
   DSRStringValue()
{
}


DSRTextTreeNode::DSRTextTreeNode(const E_RelationshipType relationshipType,
                                 const OFString &stringValue)
 : DSRDocumentTreeNode(relationshipType, VT_Text),
   DSRStringValue(stringValue)
{
}


DSRTextTreeNode::~DSRTextTreeNode()
{
}


void DSRTextTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRTextTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


OFBool DSRTextTreeNode::isShort(const size_t /*flags*/) const
{
    return (getValue().length() <= 40);
}


OFCondition DSRTextTreeNode::print(STD_NAMESPACE ostream &stream,
                                   const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        if (flags & PF_shortenLongItemValues)
            DSRStringValue::print(stream, 30);     // text output is limited to 30 characters
        else
            DSRStringValue::print(stream);
    }
    return result;
}


OFCondition DSRTextTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                      const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    writeStringValueToXML(stream, getValue(), "value", (flags & XF_writeEmptyTags) > 0);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRTextTreeNode::readContentItem(DcmItem &dataset)
{
    /* read TextValue */
    return DSRStringValue::read(dataset, DCM_TextValue);
}


OFCondition DSRTextTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write TextValue */
    return DSRStringValue::write(dataset, DCM_TextValue);
}


OFCondition DSRTextTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor)
{
    /* retrieve value from XML element "value" */
    return DSRStringValue::readXML(doc, doc.getNamedNode(cursor.gotoChild(), "value"), OFTrue /*encoding*/);
}


OFCondition DSRTextTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream & /*annexStream*/,
                                                   const size_t /*nestingLevel*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t flags) const
{
    OFString htmlString;
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render TextValue */
    if (flags & HF_renderItemInline)
        docStream << "\"" << convertToHTMLString(getValue(), htmlString, flags) << "\"" << OFendl;
    else
        docStream << convertToHTMLString(getValue(), htmlString, flags, OFTrue /*newlineAllowed*/) << OFendl;
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtextn.cc,v $
 *  Revision 1.27  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.26  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.25  2007-11-15 16:45:42  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.24  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.23  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.22  2005/12/08 15:48:15  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.21  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.20  2004/01/05 14:37:23  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
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
 *  Revision 1.16  2003/08/07 14:09:19  joergr
 *  Added readXML functionality.
 *  Distinguish more strictly between OFBool and int (required when HAVE_CXX_BOOL
 *  is defined).
 *
 *  Revision 1.15  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.14  2001/11/09 16:19:36  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.13  2001/10/10 15:30:05  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.12  2001/09/28 14:13:24  joergr
 *  Added missing relationship/target content item to text tree node.
 *
 *  Revision 1.11  2001/09/26 13:04:26  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.10  2001/05/07 16:14:25  joergr
 *  Updated CVS header.
 *
 *  Revision 1.9  2001/02/02 14:41:51  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.8  2000/11/09 20:34:02  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.7  2000/11/07 18:33:32  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.6  2000/11/01 16:37:05  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.5  2000/10/26 14:35:09  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:46  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:23:12  joergr
 *  Moved read and write methods to base class.
 *
 *  Revision 1.2  2000/10/16 12:00:39  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
