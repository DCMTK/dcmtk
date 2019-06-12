/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Implementation of class DcmUnsigned64bitVeryLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvruv.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CINTTYPES
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmUnsigned64bitVeryLong::DcmUnsigned64bitVeryLong(const DcmTag &tag)
  : DcmElement(tag, 0)
{
}


DcmUnsigned64bitVeryLong::DcmUnsigned64bitVeryLong(const DcmTag &tag,
                                                   const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmUnsigned64bitVeryLong::DcmUnsigned64bitVeryLong(const DcmUnsigned64bitVeryLong &old)
  : DcmElement(old)
{
}


DcmUnsigned64bitVeryLong::~DcmUnsigned64bitVeryLong()
{
}


DcmUnsigned64bitVeryLong &DcmUnsigned64bitVeryLong::operator=(const DcmUnsigned64bitVeryLong &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


int DcmUnsigned64bitVeryLong::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmUnsigned64bitVeryLong* myThis = NULL;
    DcmUnsigned64bitVeryLong* myRhs = NULL;
    myThis = OFconst_cast(DcmUnsigned64bitVeryLong*, this);
    myRhs =  OFstatic_cast(DcmUnsigned64bitVeryLong*, OFconst_cast(DcmElement*, &rhs));

    /* compare number of values */
    unsigned long thisNumValues = myThis->getNumberOfValues();
    unsigned long rhsNumValues = myRhs->getNumberOfValues();
    if (thisNumValues < rhsNumValues)
    {
        return -1;
    }
    else if (thisNumValues > rhsNumValues)
    {
        return 1;
    }

    /* iterate over all components and test equality */
    for (unsigned long count = 0; count < thisNumValues; count++)
    {
        Uint64 val = 0;
        if (myThis->getUint64(val, count).good())
        {
            Uint64 rhsVal = 0;
            if (myRhs->getUint64(rhsVal, count).good())
            {
                if (val > rhsVal)
                {
                    return 1;
                }
                else if (val < rhsVal)
                {
                    return -1;
                }
            }
        }
    }

    /* all values as well as VM equal: objects are equal */
    return 0;
}


OFCondition DcmUnsigned64bitVeryLong::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmUnsigned64bitVeryLong &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmUnsigned64bitVeryLong::ident() const
{
    return EVR_UV;
}


OFCondition DcmUnsigned64bitVeryLong::checkValue(const OFString &vm,
                                                 const OFBool /*oldFormat*/)
{
    /* check VM only */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmUnsigned64bitVeryLong::getVM()
{
    return getNumberOfValues();
}


unsigned long DcmUnsigned64bitVeryLong::getNumberOfValues()
{
    return OFstatic_cast(unsigned long, getLengthField() / sizeof(Uint64));
}


// ********************************


void DcmUnsigned64bitVeryLong::print(STD_NAMESPACE ostream &out,
                                     const size_t flags,
                                     const int level,
                                     const char * /*pixelFileName*/,
                                     size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get unsigned integer data */
        Uint64 *uintVals;
        errorFlag = getUint64Array(uintVals);
        if (uintVals != NULL)
        {
            /* do not use getVM() because derived classes might always return 1 */
            const unsigned long count = getNumberOfValues();
            /* double-check length field for valid value */
            if (count > 0)
            {
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
#ifdef PRIu64
                        sprintf(buffer, "%" PRIu64, *uintVals);
                    else
                        sprintf(buffer, "\\%" PRIu64, *uintVals);
#elif SIZEOF_LONG == 8
                        sprintf(buffer, "%lu", *uintVals);
                    else
                        sprintf(buffer, "\\%lu", *uintVals);
#else // assume "long long" is 64 bits
                        sprintf(buffer, "%llu", *uintVals);
                    else
                        sprintf(buffer, "\\%llu", *uintVals);
#endif
                    /* check whether current value sticks to the length limit */
                    newLength = printedLength + OFstatic_cast(unsigned long, strlen(buffer));
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
            } else {
                /* count can be zero if we have an invalid element with less than eight bytes length */
                printInfoLine(out, flags, level, "(invalid value)");
            }
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmUnsigned64bitVeryLong::getUint64(Uint64 &uintVal,
                                                const unsigned long pos)
{
    /* get unsigned integer data */
    Uint64 *uintValues = NULL;
    errorFlag = getUint64Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        /* do not use getVM() because derived classes might always return 1 */
        else if (pos >= getNumberOfValues())
            errorFlag = EC_IllegalParameter;
        else
            uintVal = uintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        uintVal = 0;
    return errorFlag;
}


OFCondition DcmUnsigned64bitVeryLong::getUint64Array(Uint64 *&uintVals)
{
    uintVals = OFstatic_cast(Uint64 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmUnsigned64bitVeryLong::getOFString(OFString &stringVal,
                                                  const unsigned long pos,
                                                  OFBool /*normalize*/)
{
    Uint64 uintVal;
    /* get the specified numeric value */
    errorFlag = getUint64(uintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
#ifdef PRIu64
        sprintf(buffer, "%" PRIu64, uintVal);
#elif SIZEOF_LONG == 8
        sprintf(buffer, "%lu", uintVal);
#else // assume "long long" is 64 bits
        sprintf(buffer, "%llu", uintVal);
#endif
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmUnsigned64bitVeryLong::putUint64(const Uint64 uintVal,
                                                const unsigned long pos)
{
    Uint64 val = uintVal;
    errorFlag = changeValue(&val, OFstatic_cast(Uint32, sizeof(Uint64) * pos), OFstatic_cast(Uint32, sizeof(Uint64)));
    return errorFlag;
}


OFCondition DcmUnsigned64bitVeryLong::putUint64Array(const Uint64 *uintVals,
                                                     const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints > 0)
    {
        /* check for valid data */
        if (uintVals != NULL)
            errorFlag = putValue(uintVals, OFstatic_cast(Uint32, sizeof(Uint64) * OFstatic_cast(size_t, numUints)));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsigned64bitVeryLong::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmUnsigned64bitVeryLong::putString(const char *stringVal,
                                                const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Uint64 *field = new Uint64[vm];
        OFString value;
        size_t pos = 0;
        /* retrieve unsigned integer data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (value.empty() ||
#ifdef SCNu64
                (sscanf(value.c_str(), "%" SCNu64, &field[i]) != 1)
#elif SIZEOF_LONG == 8
                (sscanf(value.c_str(), "%lu", &field[i]) != 1)
#else // assume "long long" is 64 bits
                (sscanf(value.c_str(), "%llu", &field[i]) != 1)
#endif
                )
            {
                errorFlag = EC_CorruptedData;
            }
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putUint64Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsigned64bitVeryLong::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Uint64)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % OFstatic_cast(Uint32, sizeof(Uint64))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
