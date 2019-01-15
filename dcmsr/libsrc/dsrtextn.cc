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
 *    classes: DSRTextTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtextn.h"
#include "dcmtk/dcmsr/dsrstrvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrut.h"


DSRTextTreeNode::DSRTextTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_Text),
    DSRStringValue()
{
}


DSRTextTreeNode::DSRTextTreeNode(const E_RelationshipType relationshipType,
                                 const OFString &textValue,
                                 const OFBool check)
  : DSRDocumentTreeNode(relationshipType, VT_Text),
    DSRStringValue(textValue, check)
{
}


DSRTextTreeNode::DSRTextTreeNode(const DSRTextTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRStringValue(node)
{
}


DSRTextTreeNode::~DSRTextTreeNode()
{
}


OFBool DSRTextTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator==(OFstatic_cast(const DSRTextTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRTextTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator!=(OFstatic_cast(const DSRTextTreeNode &, node).getValue());
    }
    return result;
}


DSRTextTreeNode *DSRTextTreeNode::clone() const
{
    return new DSRTextTreeNode(*this);
}


void DSRTextTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRTextTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && getConceptName().isValid() && hasValidValue();
}


OFBool DSRTextTreeNode::hasValidValue() const
{
    return checkCurrentValue().good();
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
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
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


OFCondition DSRTextTreeNode::readContentItem(DcmItem &dataset,
                                             const size_t flags)
{
    /* read TextValue */
    return DSRStringValue::read(dataset, DCM_TextValue, flags);
}


OFCondition DSRTextTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write TextValue */
    return DSRStringValue::write(dataset, DCM_TextValue);
}


OFCondition DSRTextTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor,
                                                const size_t flags)
{
    /* retrieve value from XML element "value" */
    return DSRStringValue::readXML(doc, doc.getNamedChildNode(cursor, "value"), flags, OFTrue /*encoding*/);
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


OFCondition DSRTextTreeNode::checkValue(const OFString &textValue) const
{
    /* first, make sure that the mandatory value is non-empty */
    OFCondition result = DSRStringValue::checkValue(textValue);
    /* then, check whether the passed value is valid with regards to VR and VM.
     * tbd: unfortunately, we do not know the character set, so "UNKNOWN" is used. */
    if (result.good())
        result = DcmUnlimitedText::checkStringValue(textValue, "UNKNOWN");
    return result;
}
