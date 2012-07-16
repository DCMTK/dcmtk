/*
 *
 *  Copyright (C) 2000-2012, OFFIS e.V.
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
 *    classes: DSRStringValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstrvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRStringValue::DSRStringValue()
  : Value()
{
}


DSRStringValue::DSRStringValue(const OFString &stringValue,
                               const OFBool check)
  : Value()
{
    /* use the set methods for checking purposes */
    setValue(stringValue, check);
}


DSRStringValue::DSRStringValue(const DSRStringValue &stringValue)
  : Value(stringValue.Value)
{
    /* do not check since this would be unexpected to the user */
}


DSRStringValue::~DSRStringValue()
{
}


DSRStringValue &DSRStringValue::operator=(const DSRStringValue &stringValue)
{
    /* do not check since this would be unexpected to the user */
    Value = stringValue.Value;
    return *this;
}


void DSRStringValue::clear()
{
    Value.clear();
}


OFBool DSRStringValue::isValid() const
{
    return checkCurrentValue().good();
}


void DSRStringValue::print(STD_NAMESPACE ostream &stream,
                           const size_t maxLength) const
{
    OFString printString;
    if ((maxLength > 3) && (Value.length() > maxLength))
        stream << "\"" << DSRTypes::convertToPrintString(Value.substr(0, maxLength - 3), printString) << "...\"";
    else
        stream << "\"" << DSRTypes::convertToPrintString(Value, printString) << "\"";
}


OFCondition DSRStringValue::read(DcmItem &dataset,
                                 const DcmTagKey &tagKey)
{
    /* read value */
    return DSRTypes::getAndCheckStringValueFromDataset(dataset, tagKey, Value, "1", "1", "content item");
}


OFCondition DSRStringValue::write(DcmItem &dataset,
                                  const DcmTagKey &tagKey) const
{
    /* write Value */
    return DSRTypes::putStringValueToDataset(dataset, tagKey, Value);
}


OFCondition DSRStringValue::readXML(const DSRXMLDocument &doc,
                                    DSRXMLCursor cursor,
                                    const OFBool encoding)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        /* retrieve value from XML element */
        doc.getStringFromNodeContent(cursor, Value, NULL /*name*/, encoding);
        /* check whether string value is valid */
        result = isValid() ? EC_Normal : SR_EC_InvalidValue;
    }
    return result;
}


OFCondition DSRStringValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                       const size_t flags) const
{
    OFString htmlString;
    if (!(flags & DSRTypes::HF_renderItemsSeparately))
    {
        if (flags & DSRTypes::HF_XHTML11Compatibility)
            docStream << "<span class=\"under\">";
        else if (flags & DSRTypes::HF_HTML32Compatibility)
            docStream << "<u>";
        else /* HTML 4.01 */
            docStream << "<span class=\"under\">";
    }
    docStream << DSRTypes::convertToHTMLString(Value, htmlString, flags);
    if (!(flags & DSRTypes::HF_renderItemsSeparately))
    {
        if (flags & DSRTypes::HF_HTML32Compatibility)
            docStream << "</u>";
        else
            docStream << "</span>";
    }
    return EC_Normal;
}


OFCondition DSRStringValue::setValue(const OFString &stringValue,
                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* check whether the passed value is valid */
        result = checkValue(stringValue);
    } else {
        /* make sure that the value is non-empty */
        result = DSRStringValue::checkValue(stringValue);
    }
    if (result.good())
        Value = stringValue;
    return result;
}


OFCondition DSRStringValue::checkValue(const OFString &stringValue) const
{
    /* all corresponding DICOM attributes are type 1, i.e. mandatory */
    return stringValue.empty() ? SR_EC_InvalidValue : EC_Normal;
}


OFCondition DSRStringValue::checkCurrentValue() const
{
    return checkValue(Value);
}
