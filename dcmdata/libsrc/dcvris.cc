/*
 *
 *  Copyright (C) 1994-2025, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmIntegerString
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/ofstd/ofstring.h"

#define MAX_IS_LENGTH 12


// ********************************


DcmIntegerString::DcmIntegerString(const DcmTag &tag,
                                   const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_IS_LENGTH);
    setNonSignificantChars(" \\");
}


DcmIntegerString::DcmIntegerString(const DcmIntegerString &old)
  : DcmByteString(old)
{
}


DcmIntegerString::~DcmIntegerString()
{
}


DcmIntegerString &DcmIntegerString::operator=(const DcmIntegerString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmIntegerString::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmIntegerString &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmIntegerString::ident() const
{
    return EVR_IS;
}


OFCondition DcmIntegerString::checkValue(const OFString &vm,
                                         const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmIntegerString::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmIntegerString::getSint32(Sint32 &sintVal,
                                        const unsigned long pos)
{
    /* get integer string value */
    OFString str;
    OFCondition l_error = getOFString(str, pos, OFTrue);
    if (l_error.good())
    {
        /* convert string to integer value */
#ifdef SCNd32
        if (sscanf(str.c_str(), "%" SCNd32, &sintVal) != 1)
#elif SIZEOF_LONG == 8
        if (sscanf(str.c_str(), "%d", &sintVal) != 1)
#else
        if (sscanf(str.c_str(), "%ld", &sintVal) != 1)
#endif
            l_error = EC_CorruptedData;
    }
    return l_error;
}


// ********************************


OFCondition DcmIntegerString::getOFString(OFString &stringVal,
                                          const unsigned long pos,
                                          OFBool normalize)
{
    /* call inherited method */
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    /* normalize string if required */
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmIntegerString::checkStringValue(const OFString &value,
                                                const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "is", 8, MAX_IS_LENGTH);
}


// ********************************


OFCondition DcmIntegerString::writeJson(STD_NAMESPACE ostream &out,
                                        DcmJsonFormat &format)
{
    OFCondition status = EC_Normal;

    /* always write JSON Opener */
    writeJsonOpener(out, format);
    OFBool isValid;

    if (!isEmpty())
    {
        /* write element value */
        if (format.asBulkDataURI(getTag(), getLength()))
        {
            /* adjust byte order to little endian */
            Uint8 *byteValues = OFstatic_cast(Uint8 *, getValue(EBO_LittleEndian));
            status = format.writeBulkData(out, getTag(), getLengthField(), byteValues);
        }
        else
        {
            const unsigned long vm = getVM();
            if (vm > 0)
            {
                OFString value;
                OFString vmstring = "1";
                status = getOFString(value, 0L);
                if (status.bad())
                    return status;
                format.printValuePrefix(out);

                isValid = checkStringValue(value, vmstring).good();
                switch (format.getJsonNumStringPolicy())
                {
                  case DcmJsonFormat::NSP_auto:
                    if (isValid)
                        DcmJsonFormat::printNumberInteger(out, value);
                    else
                    {
                        DCMDATA_WARN("encountered illegal IS value '" << value << "', converting to JSON string");
                        DcmJsonFormat::printValueString(out, value);
                    }
                    break;
                  case DcmJsonFormat::NSP_always_number:
                    if (isValid) DcmJsonFormat::printNumberInteger(out, value);
                    else
                    {
                        DCMDATA_WARN("encountered illegal IS value '" << value << "', aborting conversion to JSON");
                        return EC_CannotWriteStringAsJSONNumber;
                    }
                    break;
                  case DcmJsonFormat::NSP_always_string:
                    if (!isValid)
                    {
                        DCMDATA_WARN("encountered illegal IS value '" << value << "', converting to JSON string");
                    }
                    DcmJsonFormat::printValueString(out, value);
                    break;
                }

                for (unsigned long valNo = 1; valNo < vm; ++valNo)
                {
                    status = getOFString(value, valNo);
                    if (status.bad())
                        return status;
                    format.printNextArrayElementPrefix(out);

                    isValid = checkStringValue(value, vmstring).good();
                    switch (format.getJsonNumStringPolicy())
                    {
                      case DcmJsonFormat::NSP_auto:
                        if (isValid)
                            DcmJsonFormat::printNumberInteger(out, value);
                        else
                        {
                            DCMDATA_WARN("encountered illegal IS value '" << value << "', converting to JSON string");
                            DcmJsonFormat::printValueString(out, value);
                        }
                        break;
                      case DcmJsonFormat::NSP_always_number:
                        if (isValid)
                            DcmJsonFormat::printNumberInteger(out, value);
                        else
                        {
                            DCMDATA_WARN("encountered illegal IS value '" << value << "', aborting conversion to JSON");
                            return EC_CannotWriteStringAsJSONNumber;
                        }
                        break;
                      case DcmJsonFormat::NSP_always_string:
                        if (!isValid)
                        {
                            DCMDATA_WARN("encountered illegal IS value '" << value << "', converting to JSON string");
                        }
                        DcmJsonFormat::printValueString(out, value);
                        break;
                    }
                }
                format.printValueSuffix(out);
            }
        }
    }

    /* write JSON Closer  */
    writeJsonCloser(out, format);
    /* always report success */
    return status;
}
