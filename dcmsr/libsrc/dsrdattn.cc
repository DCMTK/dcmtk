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
 *    classes: DSRDateTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrdattn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrda.h"


DSRDateTreeNode::DSRDateTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_Date),
    DSRStringValue()
{
}


DSRDateTreeNode::DSRDateTreeNode(const E_RelationshipType relationshipType,
                                 const OFString &dateValue,
                                 const OFBool check)
  : DSRDocumentTreeNode(relationshipType, VT_Date),
    DSRStringValue(dateValue, check)
{

}


DSRDateTreeNode::DSRDateTreeNode(const DSRDateTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRStringValue(node)
{
}


DSRDateTreeNode::~DSRDateTreeNode()
{
}


OFBool DSRDateTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator==(OFstatic_cast(const DSRDateTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRDateTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator!=(OFstatic_cast(const DSRDateTreeNode &, node).getValue());
    }
    return result;
}


DSRDateTreeNode *DSRDateTreeNode::clone() const
{
    return new DSRDateTreeNode(*this);
}


void DSRDateTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRDateTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && getConceptName().isValid() && hasValidValue();
}


OFBool DSRDateTreeNode::hasValidValue() const
{
    return checkCurrentValue().good();
}


OFCondition DSRDateTreeNode::print(STD_NAMESPACE ostream &stream,
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


OFCondition DSRDateTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                      const size_t flags) const
{
    OFString tmpString;
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    /* output date in ISO 8601 format */
    DcmDate::getISOFormattedDateFromString(getValue(), tmpString);
    writeStringValueToXML(stream, tmpString, "value", (flags & XF_writeEmptyTags) > 0);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRDateTreeNode::readContentItem(DcmItem &dataset,
                                             const size_t flags)
{
    /* read Date */
    return DSRStringValue::read(dataset, DCM_Date, flags);
}


OFCondition DSRDateTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write Date */
    return DSRStringValue::write(dataset, DCM_Date);
}


OFCondition DSRDateTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor,
                                                const size_t /*flags*/)
{
    OFString tmpString;
    /* retrieve value from XML element "value" */
    OFCondition result = setValue(getValueFromXMLNodeContent(doc, doc.getNamedChildNode(cursor, "value"), tmpString));
    if (result == EC_IllegalParameter)
        result = SR_EC_InvalidValue;
    return result;
}


OFString &DSRDateTreeNode::getValueFromXMLNodeContent(const DSRXMLDocument &doc,
                                                      DSRXMLCursor cursor,
                                                      OFString &dateValue,
                                                      const OFBool clearString)
{
    if (clearString)
        dateValue.clear();
    /* check whether node is valid */
    if (cursor.valid())
    {
        OFString tmpString;
        /* retrieve value from XML element */
        if (!doc.getStringFromNodeContent(cursor, tmpString).empty())
        {
            OFDate tmpDate;
            /* convert ISO to DICOM format */
            if (tmpDate.setISOFormattedDate(tmpString))
                DcmDate::getDicomDateFromOFDate(tmpDate, dateValue);
        }
    }
    return dateValue;
}


OFCondition DSRDateTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream & /*annexStream*/,
                                                   const size_t /*nestingLevel*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render Date */
    if (result.good())
    {
        OFString htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
        {
            if (flags & DSRTypes::HF_XHTML11Compatibility)
                docStream << "<span class=\"date\">";
            else if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "<u>";
            else /* HTML 4.01 */
                docStream << "<span class=\"under\">";
        }
        docStream << dicomToReadableDate(getValue(), htmlString);
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


OFCondition DSRDateTreeNode::checkValue(const OFString &dateValue) const
{
    /* first, make sure that the mandatory value is non-empty */
    OFCondition result = DSRStringValue::checkValue(dateValue);
    /* then, check whether the passed value is valid */
    if (result.good())
        result = DcmDate::checkStringValue(dateValue, "1");
    return result;
}
