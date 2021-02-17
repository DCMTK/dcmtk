/*
 *
 *  Copyright (C) 1994-2020, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmDecimalString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"


#define MAX_DS_LENGTH 16


// ********************************


DcmDecimalString::DcmDecimalString(const DcmTag &tag,
                                   const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_DS_LENGTH);
    setNonSignificantChars(" \\");
}


DcmDecimalString::DcmDecimalString(const DcmDecimalString &old)
  : DcmByteString(old)
{
}


DcmDecimalString::~DcmDecimalString()
{
}


DcmDecimalString &DcmDecimalString::operator=(const DcmDecimalString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmDecimalString::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmDecimalString &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmDecimalString::ident() const
{
    return EVR_DS;
}


OFCondition DcmDecimalString::checkValue(const OFString &vm,
                                         const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmDecimalString::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmDecimalString::getFloat64(Float64 &doubleVal,
                                         const unsigned long pos)
{
    /* get decimal string value */
    OFString str;
    OFCondition l_error = getOFString(str, pos, OFTrue /*normalize*/);
    if (l_error.good())
    {
        OFBool success = OFFalse;
        /* convert string to float value */
        doubleVal = OFStandard::atof(str.c_str(), &success);
        if (!success)
            l_error = EC_CorruptedData;
    }
    return l_error;
}


// ********************************


OFCondition DcmDecimalString::getFloat64Vector(OFVector<Float64> &doubleVals)
{
    /* get stored value */
    char *strVal = NULL;
    Uint32 strLen = 0;
    OFCondition l_error = getString(strVal, strLen);
    /* clear result variable */
    doubleVals.clear();
    if (l_error.good() && (strVal != NULL))
    {
        /* determine number of stored values */
        const unsigned long vm = getVM();
        if (vm > 0)
        {
            Float64 doubleVal;
            OFString doubleStr;
            char *p = strVal;
            OFBool success = OFFalse;
            /* avoid memory re-allocations by specifying the expected size */
            doubleVals.reserve(vm);
            /* iterate over the string value and search for delimiters */
            for (Uint32 i = 0; i <= strLen; i++)
            {
                if ((i == strLen) || (*p == '\\'))
                {
                    /* extract single value and convert it to floating point */
                    doubleStr.assign(strVal, p - strVal);
                    doubleVal = OFStandard::atof(doubleStr.c_str(), &success);
                    if (success)
                    {
                        /* store floating point value in result variable */
                        doubleVals.push_back(doubleVal);
                        strVal = p + 1;
                    }
                    else
                    {
                        l_error = EC_CorruptedData;
                        break;
                    }
                }
                ++p;
            }
        }
    }
    return l_error;
}



// ********************************


OFCondition DcmDecimalString::getOFString(OFString &stringVal,
                                          const unsigned long pos,
                                          OFBool normalize)
{
    /* call inherited method */
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos);
    /* normalize string if required */
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmDecimalString::putFloat64(const Float64 val,
                                         const unsigned long pos)
{
    return putFloat64Prec(val, pos, 6);
}


OFCondition DcmDecimalString::putFloat64Prec(const Float64 val,
                                             const unsigned long pos,
                                             const Uint8 prec,
                                             const OFBool cutTrailZeroes)
{
    if (prec > 100)
        return EC_IllegalParameter;

    // Reserve enough bytes for conversion (considering high precision values that
    // might be cut off later, i.e. 16 for integer part, 1 for dot, 14 max precision, 1 for NUL, i.e.
    char buf[16 + 1 + 14 +1];
    int written = OFStandard::snprintf(buf, 32, "%.*f", prec, val);
    if (written > 31 /* NUL not counting in */)
    {
        return EC_IllegalParameter;
    }

    OFString str(buf);
    if (cutTrailZeroes)
    {
        size_t dot = str.find_last_of('.');
        if (dot != OFString_npos)
        {
            size_t last_valid = str.find_last_not_of('0');
            if (last_valid != str.length() -1)
            {
                if (str[last_valid] == '.')
                    str = str.substr(0, last_valid);
                else
                    str = str.substr(0, last_valid+1);
            }
        }
    }
    if (str.length() > 16)
    {
        return EC_IllegalParameter;
    }

    return putOFStringAtPos(str.c_str(), pos);
}


// ********************************


OFCondition DcmDecimalString::writeXML(STD_NAMESPACE ostream &out,
                                       const size_t flags)
{
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* for the Native DICOM Model output, we do not need any specific DS handling */
        return DcmElement::writeXML(out, flags);
    } else {
        /* XML start tag: <element tag="gggg,eeee" vr="XX" ...> */
        writeXMLStartTag(out, flags);
        /* write element value (if loaded) */
        if (valueLoaded())
        {
            /* get string data (without normalization) */
            char *value = NULL;
            Uint32 length = 0;
            getString(value, length);
            if ((value != NULL) && (length > 0))
            {
                /* explicitly convert to OFString because of possible NULL bytes */
                OFString stringVal(value, length);
                const OFBool convertNonASCII = (flags & DCMTypes::XF_convertNonASCII) > 0;
                /* check whether conversion to XML markup string is required */
                if (OFStandard::checkForMarkupConversion(stringVal, convertNonASCII))
                    OFStandard::convertToMarkupStream(out, stringVal, convertNonASCII);
                else
                    out << value;
            }
        }
        /* XML end tag: </element> */
        writeXMLEndTag(out, flags);
        /* always report success */
        return EC_Normal;
    }
}


// ********************************


OFCondition DcmDecimalString::writeJson(STD_NAMESPACE ostream &out,
                                        DcmJsonFormat &format)
{
    /* always write JSON Opener */
    writeJsonOpener(out, format);

    if (!isEmpty())
    {
        /* write element value */
        OFString bulkDataValue;
        if (format.asBulkDataURI(getTag(), bulkDataValue))
        {
            format.printBulkDataURIPrefix(out);
            DcmJsonFormat::printString(out, bulkDataValue);
        }
        else
        {
            const unsigned long vm = getVM();
            if (vm > 0)
            {
                OFString value;
                OFString vmstring = "1";
                OFCondition status = getOFString(value, 0L);
                if (status.bad())
                    return status;
                format.printValuePrefix(out);
                // if the value is a proper number, write as JSON number,
                // otherwise write as JSON string.
                if (checkStringValue(value, vmstring).good())
                    DcmJsonFormat::printNumberDecimal(out, value);
                    else DcmJsonFormat::printValueString(out, value);
                for (unsigned long valNo = 1; valNo < vm; ++valNo)
                {
                    status = getOFString(value, valNo);
                    if (status.bad())
                        return status;
                    format.printNextArrayElementPrefix(out);
                    // if the value is a proper number, write as JSON number,
                    // otherwise write as JSON string.
                    if (checkStringValue(value, vmstring).good())
                        DcmJsonFormat::printNumberDecimal(out, value);
                        else DcmJsonFormat::printValueString(out, value);
                }
                format.printValueSuffix(out);
            }
        }
    }

    /* write JSON Closer  */
    writeJsonCloser(out, format);
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmDecimalString::checkStringValue(const OFString &value,
                                               const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "ds", 6, MAX_DS_LENGTH);
}
