/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
 *    classes: DSRImageTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrimgtn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"


DSRImageTreeNode::DSRImageTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_Image),
    DSRImageReferenceValue()
{
}


DSRImageTreeNode::DSRImageTreeNode(const DSRImageTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRImageReferenceValue(node)
{
}


DSRImageTreeNode::~DSRImageTreeNode()
{
}


OFBool DSRImageTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRImageReferenceValue::operator==(OFstatic_cast(const DSRImageTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRImageTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRImageReferenceValue::operator!=(OFstatic_cast(const DSRImageTreeNode &, node).getValue());
    }
    return result;
}


DSRImageTreeNode *DSRImageTreeNode::clone() const
{
    return new DSRImageTreeNode(*this);
}


void DSRImageTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRImageReferenceValue::clear();
}


OFBool DSRImageTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && hasValidValue();
}


OFBool DSRImageTreeNode::hasValidValue() const
{
    return DSRImageReferenceValue::isValid();
}


OFBool DSRImageTreeNode::isShort(const size_t flags) const
{
    return DSRImageReferenceValue::isShort(flags);
}


OFCondition DSRImageTreeNode::print(STD_NAMESPACE ostream &stream,
                                    const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
        result = DSRImageReferenceValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRImageTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                       const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    stream << "<value>" << OFendl;
    DSRImageReferenceValue::writeXML(stream, flags);
    stream << "</value>" << OFendl;
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRImageTreeNode::readContentItem(DcmItem &dataset,
                                              const size_t flags)
{
    /* read ReferencedSOPSequence */
    return DSRImageReferenceValue::readSequence(dataset, DCM_ReferencedSOPSequence, "1" /*type*/, flags);
}


OFCondition DSRImageTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write ReferencedSOPSequence */
    return DSRImageReferenceValue::writeSequence(dataset, DCM_ReferencedSOPSequence);
}


OFCondition DSRImageTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                 DSRXMLCursor cursor,
                                                 const size_t flags)
{
    /* retrieve value from XML element "value" */
    return DSRImageReferenceValue::readXML(doc, doc.getNamedChildNode(cursor, "value"), flags);
}


OFCondition DSRImageTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                    STD_NAMESPACE ostream &annexStream,
                                                    const size_t /*nestingLevel*/,
                                                    size_t &annexNumber,
                                                    const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render Reference */
    if (result.good())
    {
        result = DSRImageReferenceValue::renderHTML(docStream, annexStream, annexNumber, flags);
        docStream << OFendl;
    }
    return result;
}
