/*
 *
 *  Copyright (C) 2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRStringValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-19 16:07:14 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrstrvl.h"


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
   return OFTrue;
}


void DSRStringValue::print(ostream &stream,
                           const size_t maxLength) const
{
    if ((maxLength > 3) && (Value.length() > maxLength))
        stream << "\"" << Value.substr(0, maxLength - 3) << "...\"";
    else
        stream << "\"" << Value << "\"";
}


E_Condition DSRStringValue::read(DcmItem &dataset,
                                 const DcmTagKey &tagKey,
                                 OFConsole *logStream)
{
    /* tbd: check value */
    return DSRTypes::getAndCheckStringValueFromDataset(dataset, tagKey, Value, "1", "1", logStream, "content item");
}


E_Condition DSRStringValue::write(DcmItem &dataset,
                                  const DcmTagKey &tagKey,
                                  OFConsole * /* logStream */) const
{
    /* write Value */
    return DSRTypes::putStringValueToDataset(dataset, tagKey, Value);
}


E_Condition DSRStringValue::setValue(const OFString &stringValue)
{
    E_Condition result = EC_IllegalCall;
    if (checkValue(stringValue))
    {
        Value = stringValue;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRStringValue::checkValue(const OFString & /* stringValue */) const
{
    return OFTrue;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrstrvl.cc,v $
 *  Revision 1.3  2000-10-19 16:07:14  joergr
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
