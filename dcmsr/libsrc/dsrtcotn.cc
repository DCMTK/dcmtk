/*
 *
 *  Copyright (C) 2000-2011, OFFIS e.V.
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
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
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
