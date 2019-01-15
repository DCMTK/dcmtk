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
 *    classes: DSRWaveformTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrwavtn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"


DSRWaveformTreeNode::DSRWaveformTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_Waveform),
    DSRWaveformReferenceValue()
{
}


DSRWaveformTreeNode::DSRWaveformTreeNode(const DSRWaveformTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRWaveformReferenceValue(node)
{
}


DSRWaveformTreeNode::~DSRWaveformTreeNode()
{
}


OFBool DSRWaveformTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRWaveformReferenceValue::operator==(OFstatic_cast(const DSRWaveformTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRWaveformTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRWaveformReferenceValue::operator!=(OFstatic_cast(const DSRWaveformTreeNode &, node).getValue());
    }
    return result;
}


DSRWaveformTreeNode *DSRWaveformTreeNode::clone() const
{
    return new DSRWaveformTreeNode(*this);
}


void DSRWaveformTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRWaveformReferenceValue::clear();
}


OFBool DSRWaveformTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && hasValidValue();
}


OFBool DSRWaveformTreeNode::hasValidValue() const
{
    return DSRWaveformReferenceValue::isValid();
}


OFBool DSRWaveformTreeNode::isShort(const size_t flags) const
{
    return DSRWaveformReferenceValue::isShort(flags);
}


OFCondition DSRWaveformTreeNode::print(STD_NAMESPACE ostream &stream,
                                       const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
        result = DSRWaveformReferenceValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRWaveformTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                          const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    stream << "<value>" << OFendl;
    DSRWaveformReferenceValue::writeXML(stream, flags);
    stream << "</value>" << OFendl;
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRWaveformTreeNode::readContentItem(DcmItem &dataset,
                                                 const size_t flags)
{
    /* read ReferencedSOPSequence */
    return DSRWaveformReferenceValue::readSequence(dataset, DCM_ReferencedSOPSequence, "1" /*type*/, flags);
}


OFCondition DSRWaveformTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write ReferencedSOPSequence */
    return DSRWaveformReferenceValue::writeSequence(dataset, DCM_ReferencedSOPSequence);
}


OFCondition DSRWaveformTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                    DSRXMLCursor cursor,
                                                    const size_t flags)
{
    /* retrieve value from XML element "value" */
    return DSRWaveformReferenceValue::readXML(doc, doc.getNamedChildNode(cursor, "value"), flags);
}


OFCondition DSRWaveformTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
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
        result = DSRWaveformReferenceValue::renderHTML(docStream, annexStream, annexNumber, flags);
        docStream << OFendl;
    }
    return result;
}
