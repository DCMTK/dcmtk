/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstrvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRStringValue::DSRStringValue()
  : Value()
{
}


DSRStringValue::DSRStringValue(const OFString &stringValue)
  : Value()
{
    /* use the set methods for checking purposes */
    setValue(stringValue);
}


DSRStringValue::DSRStringValue(const DSRStringValue &stringValue)
  : Value(stringValue.Value)
{
    /* do not check since this would unexpected to the user */
}


DSRStringValue::~DSRStringValue()
{
}


DSRStringValue &DSRStringValue::operator=(const DSRStringValue &stringValue)
{
    /* do not check since this would unexpected to the user */
    Value = stringValue.Value;
    return *this;
}


void DSRStringValue::clear()
{
    Value.clear();
}


OFBool DSRStringValue::isValid() const
{
    return checkValue(Value);
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
    /* tbd: check value */
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
        result = (isValid() ? EC_Normal : SR_EC_InvalidValue);
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


OFCondition DSRStringValue::setValue(const OFString &stringValue)
{
    OFCondition result = EC_IllegalParameter;
    if (checkValue(stringValue))
    {
        Value = stringValue;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRStringValue::checkValue(const OFString &stringValue) const
{
    return !stringValue.empty();
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrstrvl.cc,v $
 *  Revision 1.18  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.17  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.16  2007-11-15 16:45:26  joergr
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.15  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.14  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.13  2005/12/08 15:48:09  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.11  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.10  2003/08/07 13:53:01  joergr
 *  Added readXML functionality.
 *  Distinguish more strictly between OFBool and int (required when HAVE_CXX_BOOL
 *  is defined).
 *
 *  Revision 1.9  2001/10/10 15:30:01  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.8  2001/09/26 13:04:24  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:51:10  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/11/09 20:34:01  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.5  2000/11/01 16:37:04  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.4  2000/10/23 15:04:13  joergr
 *  Enhanced implementation of method isValid().
 *
 *  Revision 1.3  2000/10/19 16:07:14  joergr
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.2  2000/10/18 17:22:44  joergr
 *  Added read and write methods.
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.1  2000/10/13 07:52:25  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
