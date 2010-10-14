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
 *    classes: DSRTCoordTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtcotn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRTCoordTreeNode::DSRTCoordTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_TCoord),
   DSRTemporalCoordinatesValue()
{
}


DSRTCoordTreeNode::~DSRTCoordTreeNode()
{
}


void DSRTCoordTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRTemporalCoordinatesValue::clear();
}


OFBool DSRTCoordTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && DSRTemporalCoordinatesValue::isValid();
}


OFBool DSRTCoordTreeNode::isShort(const size_t flags) const
{
    return DSRTemporalCoordinatesValue::isShort(flags);
}


OFCondition DSRTCoordTreeNode::print(STD_NAMESPACE ostream &stream,
                                     const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        result = DSRTemporalCoordinatesValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRTCoordTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                        const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /*closingBracket*/);
    stream << " type=\"" << temporalRangeTypeToEnumeratedValue(getTemporalRangeType()) << "\"";
    stream << ">" << OFendl;
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    DSRTemporalCoordinatesValue::writeXML(stream, flags);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRTCoordTreeNode::readContentItem(DcmItem &dataset)
{
    /* read TemporalCoordinates */
    return DSRTemporalCoordinatesValue::read(dataset);
}


OFCondition DSRTCoordTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write TemporalCoordinates */
    return DSRTemporalCoordinatesValue::write(dataset);
}


OFCondition DSRTCoordTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                  DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        OFString tmpString;
        /* read 'type' and check validity */
        result = setTemporalRangeType(enumeratedValueToTemporalRangeType(doc.getStringFromAttribute(cursor, tmpString, "type")));
        if (result.good())
        {
            /* proceed reading the temporal coordinates */
            result = DSRTemporalCoordinatesValue::readXML(doc, cursor);
        } else
            printUnknownValueWarningMessage("TCOORD type", tmpString.c_str());
    }
    return result;
}


OFCondition DSRTCoordTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                     STD_NAMESPACE ostream &annexStream,
                                                     const size_t /*nestingLevel*/,
                                                     size_t &annexNumber,
                                                     const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render TemporalCoordinates */
    if (result.good())
    {
        result = DSRTemporalCoordinatesValue::renderHTML(docStream, annexStream, annexNumber, flags);
        docStream << OFendl;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcotn.cc,v $
 *  Revision 1.17  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.15  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.14  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.13  2005/12/08 15:48:12  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.11  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.10  2003/08/07 13:55:43  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.9  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.8  2001/11/09 16:18:26  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.7  2001/10/10 15:30:03  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.6  2001/09/26 13:04:25  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.5  2001/05/07 16:14:25  joergr
 *  Updated CVS header.
 *
 *  Revision 1.4  2001/02/02 14:41:51  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.3  2000/11/07 18:33:31  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.2  2000/11/01 16:37:04  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.1  2000/10/26 14:40:28  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
