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
 *    classes: DSRTimeTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtimtn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrtm.h"


DSRTimeTreeNode::DSRTimeTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_Time),
    DSRStringValue()
{
}


DSRTimeTreeNode::DSRTimeTreeNode(const E_RelationshipType relationshipType,
                                 const OFString &timeValue,
                                 const OFBool check)
  : DSRDocumentTreeNode(relationshipType, VT_Time),
    DSRStringValue(timeValue, check)
{
}


DSRTimeTreeNode::DSRTimeTreeNode(const DSRTimeTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRStringValue(node)
{
}


DSRTimeTreeNode::~DSRTimeTreeNode()
{
}


OFBool DSRTimeTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator==(OFstatic_cast(const DSRTimeTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRTimeTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator!=(OFstatic_cast(const DSRTimeTreeNode &, node).getValue());
    }
    return result;
}


DSRTimeTreeNode *DSRTimeTreeNode::clone() const
{
    return new DSRTimeTreeNode(*this);
}


void DSRTimeTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRTimeTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && getConceptName().isValid() && hasValidValue();
}


OFBool DSRTimeTreeNode::hasValidValue() const
{
    return checkCurrentValue().good();
}


OFCondition DSRTimeTreeNode::print(STD_NAMESPACE ostream &stream,
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


OFCondition DSRTimeTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                      const size_t flags) const
{
    OFString tmpString;
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    /* output time in ISO 8601 format */
    DcmTime::getISOFormattedTimeFromString(getValue(), tmpString);
    writeStringValueToXML(stream, tmpString, "value", (flags & XF_writeEmptyTags) > 0);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRTimeTreeNode::readContentItem(DcmItem &dataset,
                                             const size_t flags)
{
    /* read Time */
    return DSRStringValue::read(dataset, DCM_Time, flags);
}


OFCondition DSRTimeTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write Time */
    return DSRStringValue::write(dataset, DCM_Time);
}


OFCondition DSRTimeTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
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


OFString &DSRTimeTreeNode::getValueFromXMLNodeContent(const DSRXMLDocument &doc,
                                                      DSRXMLCursor cursor,
                                                      OFString &timeValue,
                                                      const OFBool clearString)
{
    if (clearString)
        timeValue.clear();
    /* check whether node is valid */
    if (cursor.valid())
    {
        OFString tmpString;
        /* retrieve value from XML element */
        if (!doc.getStringFromNodeContent(cursor, tmpString).empty())
        {
            OFTime tmpTime;
            /* convert ISO to DICOM format */
            if (tmpTime.setISOFormattedTime(tmpString))
                DcmTime::getDicomTimeFromOFTime(tmpTime, timeValue);
        }
    }
    return timeValue;
}


OFCondition DSRTimeTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream & /*annexStream*/,
                                                   const size_t /*nestingLevel*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render Time */
    if (result.good())
    {
        OFString htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
        {
            if (flags & DSRTypes::HF_XHTML11Compatibility)
                docStream << "<span class=\"time\">";
            else if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "<u>";
            else /* HTML 4.01 */
                docStream << "<span class=\"under\">";
        }
        docStream << dicomToReadableTime(getValue(), htmlString);
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


OFCondition DSRTimeTreeNode::checkValue(const OFString &timeValue) const
{
    /* first, make sure that the mandatory value is non-empty */
    OFCondition result = DSRStringValue::checkValue(timeValue);
    /* then, check whether the passed value is valid */
    if (result.good())
        result = DcmTime::checkStringValue(timeValue, "1");
    return result;
}
