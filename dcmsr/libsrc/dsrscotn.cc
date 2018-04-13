/*
 *
 *  Copyright (C) 2000-2018, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRSCoordTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrscotn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRSCoordTreeNode::DSRSCoordTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_SCoord),
    DSRSpatialCoordinatesValue()
{
}


DSRSCoordTreeNode::DSRSCoordTreeNode(const DSRSCoordTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRSpatialCoordinatesValue(node)
{
}


DSRSCoordTreeNode::~DSRSCoordTreeNode()
{
}


OFBool DSRSCoordTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRSpatialCoordinatesValue::operator==(OFstatic_cast(const DSRSCoordTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRSCoordTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRSpatialCoordinatesValue::operator!=(OFstatic_cast(const DSRSCoordTreeNode &, node).getValue());
    }
    return result;
}


DSRSCoordTreeNode *DSRSCoordTreeNode::clone() const
{
    return new DSRSCoordTreeNode(*this);
}


void DSRSCoordTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRSpatialCoordinatesValue::clear();
}


OFBool DSRSCoordTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && hasValidValue();
}


OFBool DSRSCoordTreeNode::hasValidValue() const
{
    return DSRSpatialCoordinatesValue::isValid();
}


OFBool DSRSCoordTreeNode::isShort(const size_t flags) const
{
    return DSRSpatialCoordinatesValue::isShort(flags);
}


OFCondition DSRSCoordTreeNode::print(STD_NAMESPACE ostream &stream,
                                     const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
        result = DSRSpatialCoordinatesValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRSCoordTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                        const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /*closingBracket*/);
    stream << " type=\"" << graphicTypeToEnumeratedValue(getGraphicType()) << "\"";
    stream << ">" << OFendl;
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    DSRSpatialCoordinatesValue::writeXML(stream, flags);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRSCoordTreeNode::readContentItem(DcmItem &dataset,
                                               const size_t flags)
{
    /* read SpatialCoordinates */
    return DSRSpatialCoordinatesValue::read(dataset, flags);
}


OFCondition DSRSCoordTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write SpatialCoordinates */
    return DSRSpatialCoordinatesValue::write(dataset);
}


OFCondition DSRSCoordTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                  DSRXMLCursor cursor,
                                                  const size_t flags)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        OFString tmpString;
        /* read 'type' and check validity */
        result = setGraphicType(enumeratedValueToGraphicType(doc.getStringFromAttribute(cursor, tmpString, "type")));
        if (result.good())
        {
            /* proceed reading the spatial coordinates */
            result = DSRSpatialCoordinatesValue::readXML(doc, cursor, flags);
        } else
            printUnknownValueWarningMessage("SCOORD type", tmpString.c_str());
    }
    return result;
}


OFCondition DSRSCoordTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                     STD_NAMESPACE ostream &annexStream,
                                                     const size_t /*nestingLevel*/,
                                                     size_t &annexNumber,
                                                     const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render SpatialCoordinates */
    if (result.good())
    {
        result = DSRSpatialCoordinatesValue::renderHTML(docStream, annexStream, annexNumber, flags);
        docStream << OFendl;
    }
    return result;
}
