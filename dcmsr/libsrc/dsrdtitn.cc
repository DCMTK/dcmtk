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
 *    classes: DSRDateTimeTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrdtitn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrdt.h"


DSRDateTimeTreeNode::DSRDateTimeTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_DateTime),
    DSRStringValue()
{
}


DSRDateTimeTreeNode::DSRDateTimeTreeNode(const E_RelationshipType relationshipType,
                                         const OFString &dateTimeValue,
                                         const OFBool check)
  : DSRDocumentTreeNode(relationshipType, VT_DateTime),
    DSRStringValue(dateTimeValue, check)
{
}


DSRDateTimeTreeNode::DSRDateTimeTreeNode(const DSRDateTimeTreeNode &node)
  : DSRDocumentTreeNode(node),
    DSRStringValue(node)
{
}


DSRDateTimeTreeNode::~DSRDateTimeTreeNode()
{
}


OFBool DSRDateTimeTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator==(OFstatic_cast(const DSRDateTimeTreeNode &, node).getValue());
    }
    return result;
}


OFBool DSRDateTimeTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = DSRStringValue::operator!=(OFstatic_cast(const DSRDateTimeTreeNode &, node).getValue());
    }
    return result;
}


DSRDateTimeTreeNode *DSRDateTimeTreeNode::clone() const
{
    return new DSRDateTimeTreeNode(*this);
}


void DSRDateTimeTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRDateTimeTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && getConceptName().isValid() && hasValidValue();
}


OFBool DSRDateTimeTreeNode::hasValidValue() const
{
    return checkCurrentValue().good();
}


OFCondition DSRDateTimeTreeNode::print(STD_NAMESPACE ostream &stream,
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


OFCondition DSRDateTimeTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                          const size_t flags) const
{
    OFString tmpString;
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    /* output time in ISO 8601 format */
    DcmDateTime::getISOFormattedDateTimeFromString(getValue(), tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/,
        OFTrue /*timeZone*/, OFFalse /*createMissingPart*/, "T" /*dateTimeSeparator*/, "" /*timeZoneSeparator*/);
    writeStringValueToXML(stream, tmpString, "value", (flags & XF_writeEmptyTags) > 0);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRDateTimeTreeNode::readContentItem(DcmItem &dataset,
                                                 const size_t flags)
{
    /* read DateTime */
    return DSRStringValue::read(dataset, DCM_DateTime, flags);
}


OFCondition DSRDateTimeTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write DateTime */
    return DSRStringValue::write(dataset, DCM_DateTime);
}


OFCondition DSRDateTimeTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
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


OFString &DSRDateTimeTreeNode::getValueFromXMLNodeContent(const DSRXMLDocument &doc,
                                                          DSRXMLCursor cursor,
                                                          OFString &dateTimeValue,
                                                          const OFBool clearString)
{
    if (clearString)
        dateTimeValue.clear();
    /* check whether node is valid */
    if (cursor.valid())
    {
        OFString tmpString;
        /* retrieve value from XML element */
        if (!doc.getStringFromNodeContent(cursor, tmpString).empty())
        {
            OFDateTime tmpDateTime;
            /* convert ISO to DICOM format */
            if (tmpDateTime.setISOFormattedDateTime(tmpString))
            {
                /* example of XML date/time format with time zone: 2010-12-31T15:30:00+01:00 */
                const OFBool hasTimeZone = (tmpString.length() >= 25);
                DcmDateTime::getDicomDateTimeFromOFDateTime(tmpDateTime, dateTimeValue,
                    OFTrue /*seconds*/, OFFalse /*fraction*/, hasTimeZone /*timeZone*/);
            }
        }
    }
    return dateTimeValue;
}


OFCondition DSRDateTimeTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                       STD_NAMESPACE ostream & /*annexStream*/,
                                                       const size_t /*nestingLevel*/,
                                                       size_t & /*annexNumber*/,
                                                       const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render DateTime */
    if (result.good())
    {
        OFString htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
        {
            if (flags & DSRTypes::HF_XHTML11Compatibility)
                docStream << "<span class=\"datetime\">";
            else if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "<u>";
            else /* HTML 4.01 */
                docStream << "<span class=\"under\">";
        }
        docStream << dicomToReadableDateTime(getValue(), htmlString);
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


OFCondition DSRDateTimeTreeNode::checkValue(const OFString &dateTimeValue) const
{
    /* first, make sure that the mandatory value is non-empty */
    OFCondition result = DSRStringValue::checkValue(dateTimeValue);
    /* then, check whether the passed value is valid */
    if (result.good())
        result = DcmDateTime::checkStringValue(dateTimeValue, "1");
    return result;
}
