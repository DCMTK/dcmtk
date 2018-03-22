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
 *    classes: DSRNumTreeNode
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


DSRNumTreeNode::DSRNumTreeNode(const DSRNumTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRNumericMeasurementValue(node)
{
}


DSRNumTreeNode::~DSRNumTreeNode()
{
}


OFBool DSRNumTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRNumericMeasurementValue::operator==(OFstatic_cast(const DSRNumTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRNumTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRNumericMeasurementValue::operator!=(OFstatic_cast(const DSRNumTreeNode &, node).getValue());
    }
    return result;
}


DSRNumTreeNode *DSRNumTreeNode::clone() const
{
    return new DSRNumTreeNode(*this);
}


void DSRNumTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRNumericMeasurementValue::clear();
}


OFBool DSRNumTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && getConceptName().isValid() && hasValidValue();
}


OFBool DSRNumTreeNode::hasValidValue() const
{
    return DSRNumericMeasurementValue::isValid();
}


OFCondition DSRNumTreeNode::print(STD_NAMESPACE ostream &stream,
                                  const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
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


OFCondition DSRNumTreeNode::readContentItem(DcmItem &dataset,
                                            const size_t flags)
{
    return DSRNumericMeasurementValue::readSequence(dataset, flags);
}


OFCondition DSRNumTreeNode::writeContentItem(DcmItem &dataset) const
{
    return DSRNumericMeasurementValue::writeSequence(dataset);
}


OFCondition DSRNumTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                               DSRXMLCursor cursor,
                                               const size_t flags)
{
    return DSRNumericMeasurementValue::readXML(doc, cursor, flags);
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
