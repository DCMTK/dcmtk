/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmUnsignedLong
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvrul.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmUnsignedLong::DcmUnsignedLong(const DcmTag &tag,
                                 const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmUnsignedLong::DcmUnsignedLong(const DcmUnsignedLong &old)
  : DcmElement(old)
{
}


DcmUnsignedLong::~DcmUnsignedLong()
{
}


DcmUnsignedLong &DcmUnsignedLong::operator=(const DcmUnsignedLong &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


OFCondition DcmUnsignedLong::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUnsignedLong &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUnsignedLong::ident() const
{
    return EVR_UL;
}


OFCondition DcmUnsignedLong::checkValue(const OFString &vm,
                                        const OFBool /*oldFormat*/)
{
    /* check VM only */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmUnsignedLong::getVM()
{
    return getLengthField() / sizeof(Uint32);
}


// ********************************


void DcmUnsignedLong::print(STD_NAMESPACE ostream&out,
                            const size_t flags,
                            const int level,
                            const char * /*pixelFileName*/,
                            size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get unsigned integer data */
        Uint32 *uintVals;
        errorFlag = getUint32Array(uintVals);
        if (uintVals != NULL)
        {
            const unsigned long count = getVM();
            const unsigned long maxLength = (flags & DCMTypes::PF_shortenLongTagValues) ?
                DCM_OptPrintLineLength : OFstatic_cast(unsigned long, -1) /*unlimited*/;
            unsigned long printedLength = 0;
            unsigned long newLength = 0;
            char buffer[32];
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            for (unsigned int i = 0; i < count; i++, uintVals++)
            {
                /* check whether first value is printed (omit delimiter) */
                if (i == 0)
#if SIZEOF_LONG == 8
                    sprintf(buffer, "%u", *uintVals);
                else
                    sprintf(buffer, "\\%u", *uintVals);
#else
                    sprintf(buffer, "%lu", *uintVals);
                else
                    sprintf(buffer, "\\%lu", *uintVals);
#endif
                /* check whether current value sticks to the length limit */
                newLength = printedLength + strlen(buffer);
                if ((newLength <= maxLength) && ((i + 1 == count) || (newLength + 3 <= maxLength)))
                {
                    out << buffer;
                    printedLength = newLength;
                } else {
                    /* check whether output has been truncated */
                    if (i + 1 < count)
                    {
                        out << "...";
                        printedLength += 3;
                    }
                    break;
                }
            }
            /* print line end with length, VM and tag name */
            printInfoLineEnd(out, flags, printedLength);
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmUnsignedLong::getUint32(Uint32 &uintVal,
                                       const unsigned long pos)
{
    /* get unsigned integer data */
    Uint32 *uintValues = NULL;
    errorFlag = getUint32Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            uintVal = uintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        uintVal = 0;
    return errorFlag;
}


OFCondition DcmUnsignedLong::getUint32Array(Uint32 *&uintVals)
{
    uintVals = OFstatic_cast(Uint32 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::getOFString(OFString &stringVal,
                                         const unsigned long pos,
                                         OFBool /*normalize*/)
{
    Uint32 uintVal;
    /* get the specified numeric value */
    errorFlag = getUint32(uintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "%lu", OFstatic_cast(unsigned long, uintVal));
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::putUint32(const Uint32 uintVal,
                                       const unsigned long pos)
{
    Uint32 val = uintVal;
    errorFlag = changeValue(&val, sizeof(Uint32) * pos, sizeof(Uint32));
    return errorFlag;
}


OFCondition DcmUnsignedLong::putUint32Array(const Uint32 *uintVals,
                                            const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints > 0)
    {
        /* check for valid data */
        if (uintVals != NULL)
            errorFlag = putValue(uintVals, sizeof(Uint32) * OFstatic_cast(Uint32, numUints));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::putString(const char *stringVal)
{
    /* determine length of the string value */
    const Uint32 stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, stringLen);
}


OFCondition DcmUnsignedLong::putString(const char *stringVal,
                                       const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Uint32 *field = new Uint32[vm];
        OFString value;
        size_t pos = 0;
        /* retrieve unsigned integer data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (value.empty() ||
#if SIZEOF_LONG == 8
                (sscanf(value.c_str(), "%u", &field[i]) != 1)
#else
                (sscanf(value.c_str(), "%lu", &field[i]) != 1)
#endif
                )
            {
                errorFlag = EC_CorruptedData;
            }
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putUint32Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Uint32)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % (sizeof(Uint32))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
