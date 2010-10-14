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
 *    classes: DSRNumTreeNode
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
#include "dcmtk/dcmsr/dsrnumtn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRNumTreeNode::DSRNumTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Num),
   DSRNumericMeasurementValue()
{
}


DSRNumTreeNode::~DSRNumTreeNode()
{
}


void DSRNumTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRNumericMeasurementValue::clear();
}


OFBool DSRNumTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRNumericMeasurementValue::isValid() && getConceptName().isValid();
}


OFCondition DSRNumTreeNode::print(STD_NAMESPACE ostream &stream,
                                  const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        result = DSRNumericMeasurementValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRNumTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                     const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    DSRNumericMeasurementValue::writeXML(stream, flags);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRNumTreeNode::readContentItem(DcmItem &dataset)
{
    return DSRNumericMeasurementValue::readSequence(dataset);
}


OFCondition DSRNumTreeNode::writeContentItem(DcmItem &dataset) const
{
    return DSRNumericMeasurementValue::writeSequence(dataset);
}


OFCondition DSRNumTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                               DSRXMLCursor cursor)
{
    return DSRNumericMeasurementValue::readXML(doc, cursor);
}


OFCondition DSRNumTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                  STD_NAMESPACE ostream &annexStream,
                                                  const size_t /*nestingLevel*/,
                                                  size_t &annexNumber,
                                                  const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render Num */
    if (result.good())
    {
        result = DSRNumericMeasurementValue::renderHTML(docStream, annexStream, annexNumber, flags);
        docStream << OFendl;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrnumtn.cc,v $
 *  Revision 1.23  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.22  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.21  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.20  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.19  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.18  2005/12/08 15:47:59  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.17  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.16  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.15  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.14  2003/08/07 13:39:00  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.13  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.12  2001/11/19 13:16:59  joergr
 *  Adapted implementation according to correction proposal 266.
 *
 *  Revision 1.11  2001/11/09 16:16:48  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.10  2001/10/10 15:29:57  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.9  2001/09/26 13:04:22  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.8  2001/05/07 16:14:24  joergr
 *  Updated CVS header.
 *
 *  Revision 1.7  2001/02/02 14:41:52  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.6  2000/11/07 18:33:30  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.5  2000/11/01 16:37:00  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.4  2000/10/26 14:32:09  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.3  2000/10/23 15:04:46  joergr
 *  Added clear() method.
 *
 *  Revision 1.2  2000/10/16 12:06:20  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:22  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
