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
 *    classes: DSRPNameTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrpnmtn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrpn.h"


DSRPNameTreeNode::DSRPNameTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_PName),
    DSRStringValue()
{
}


DSRPNameTreeNode::DSRPNameTreeNode(const E_RelationshipType relationshipType,
                                   const OFString &personNameValue,
                                   const OFBool check)
  : DSRDocumentTreeNode(relationshipType, VT_PName),
    DSRStringValue(personNameValue, check)
{
}


DSRPNameTreeNode::DSRPNameTreeNode(const DSRPNameTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRStringValue(node)
{
}


DSRPNameTreeNode::~DSRPNameTreeNode()
{
}


OFBool DSRPNameTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator==(OFstatic_cast(const DSRPNameTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRPNameTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator!=(OFstatic_cast(const DSRPNameTreeNode &, node).getValue());
    }
    return result;
}


DSRPNameTreeNode *DSRPNameTreeNode::clone() const
{
    return new DSRPNameTreeNode(*this);
}


void DSRPNameTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRPNameTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && getConceptName().isValid() && hasValidValue();
}


OFBool DSRPNameTreeNode::hasValidValue() const
{
    return checkCurrentValue().good();
}


OFCondition DSRPNameTreeNode::print(STD_NAMESPACE ostream &stream,
                                    const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
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


OFCondition DSRPNameTreeNode::readContentItem(DcmItem &dataset,
                                              const size_t flags)
{
    /* read PName */
    return DSRStringValue::read(dataset, DCM_PersonName, flags);
}


OFCondition DSRPNameTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write PName */
    return DSRStringValue::write(dataset, DCM_PersonName);
}


OFCondition DSRPNameTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                 DSRXMLCursor cursor,
                                                 const size_t /*flags*/)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        /* goto sub-node "value" */
        cursor = doc.getNamedChildNode(cursor, "value").getChild();
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


OFCondition DSRPNameTreeNode::checkValue(const OFString &personNameValue) const
{
    /* first, make sure that the mandatory value is non-empty */
    OFCondition result = DSRStringValue::checkValue(personNameValue);
    /* then, check whether the passed value is valid with regards to VR and VM.
     * tbd: unfortunately, we do not know the character set, so "UNKNOWN" is used. */
    if (result.good())
        result = DcmPersonName::checkStringValue(personNameValue, "1", "UNKNOWN");
    return result;
}
