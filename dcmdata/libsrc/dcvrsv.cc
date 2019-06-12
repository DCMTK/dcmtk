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
 *  Purpose: Implementation of class DcmSigned64bitVeryLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvrsv.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CINTTYPES
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmSigned64bitVeryLong::DcmSigned64bitVeryLong(const DcmTag &tag)
  : DcmElement(tag, 0)
{
}


DcmSigned64bitVeryLong::DcmSigned64bitVeryLong(const DcmTag &tag,
                                               const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmSigned64bitVeryLong::DcmSigned64bitVeryLong(const DcmSigned64bitVeryLong &old)
  : DcmElement(old)
{
}


DcmSigned64bitVeryLong::~DcmSigned64bitVeryLong()
{
}


DcmSigned64bitVeryLong &DcmSigned64bitVeryLong::operator=(const DcmSigned64bitVeryLong &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


int DcmSigned64bitVeryLong::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmSigned64bitVeryLong* myThis = NULL;
    DcmSigned64bitVeryLong* myRhs = NULL;
    myThis = OFconst_cast(DcmSigned64bitVeryLong*, this);
    myRhs =  OFstatic_cast(DcmSigned64bitVeryLong*, OFconst_cast(DcmElement*, &rhs));

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
        Sint64 val = 0;
        if (myThis->getSint64(val, count).good())
        {
            Sint64 rhsVal = 0;
            if (myRhs->getSint64(rhsVal, count).good())
            {
                if (val > rhsVal)
                {
                    return 1;
                }
                else if (val < rhsVal)
                {
                    return -1;
                }
                /* otherwise they are equal, continue comparison */
            }
        }
    }

    /* all values as well as VM equal: objects are equal */
    return 0;
}


OFCondition DcmSigned64bitVeryLong::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmSigned64bitVeryLong &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmSigned64bitVeryLong::ident() const
{
    return EVR_SV;
}


OFCondition DcmSigned64bitVeryLong::checkValue(const OFString &vm,
                                               const OFBool /*oldFormat*/)
{
    /* check VM only */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmSigned64bitVeryLong::getVM()
{
    return getNumberOfValues();
}


unsigned long DcmSigned64bitVeryLong::getNumberOfValues()
{
    return OFstatic_cast(unsigned long, getLengthField() / sizeof(Sint64));
}


// ********************************


void DcmSigned64bitVeryLong::print(STD_NAMESPACE ostream &out,
                                   const size_t flags,
                                   const int level,
                                   const char * /*pixelFileName*/,
                                   size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get signed integer data */
        Sint64 *sintVals;
        errorFlag = getSint64Array(sintVals);
        if (sintVals != NULL)
        {
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
                for (unsigned int i = 0; i < count; i++, sintVals++)
                {
                    /* check whether first value is printed (omit delimiter) */
#ifdef PRId64
                    if (i == 0)
                        sprintf(buffer, "%" PRId64, *sintVals);
                    else
                        sprintf(buffer, "\\%" PRId64, *sintVals);
#elif SIZEOF_LONG == 8
                    if (i == 0)
                        sprintf(buffer, "%ld", *sintVals);
                    else
                        sprintf(buffer, "\\%ld", *sintVals);
#else // assume "long long" is 64 bits
                    if (i == 0)
                        sprintf(buffer, "%lld", *sintVals);
                    else
                        sprintf(buffer, "\\%lld", *sintVals);
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


OFCondition DcmSigned64bitVeryLong::getSint64(Sint64 &sintVal,
                                              const unsigned long pos)
{
    /* get signed integer data */
    Sint64 *sintValues = NULL;
    errorFlag = getSint64Array(sintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (sintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            sintVal = sintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        sintVal = 0;
    return errorFlag;
}


OFCondition DcmSigned64bitVeryLong::getSint64Array(Sint64 *&sintVals)
{
    sintVals = OFstatic_cast(Sint64 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmSigned64bitVeryLong::getOFString(OFString &stringVal,
                                                const unsigned long pos,
                                                OFBool /*normalize*/)
{
    Sint64 sintVal;
    /* get the specified numeric value */
    errorFlag = getSint64(sintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
#ifdef PRId64
        sprintf(buffer, "%" PRId64, sintVal);
#elif SIZEOF_LONG == 8
        sprintf(buffer, "%ld", sintVal);
#else // assume "long long" is 64 bits
        sprintf(buffer, "%lld", sintVal);
#endif
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmSigned64bitVeryLong::putSint64(const Sint64 sintVal,
                                              const unsigned long pos)
{
    Sint64 val = sintVal;
    errorFlag = changeValue(&val, OFstatic_cast(Uint32, sizeof(Sint64) * pos), OFstatic_cast(Uint32, sizeof(Sint64)));
    return errorFlag;
}


OFCondition DcmSigned64bitVeryLong::putSint64Array(const Sint64 *sintVals,
                                                   const unsigned long numSints)
{
    errorFlag = EC_Normal;
    if (numSints > 0)
    {
        /* check for valid data */
        if (sintVals != NULL)
            errorFlag = putValue(sintVals, OFstatic_cast(Uint32, sizeof(Sint64) * OFstatic_cast(size_t, numSints)));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSigned64bitVeryLong::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmSigned64bitVeryLong::putString(const char *stringVal,
                                              const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Sint64 *field = new Sint64[vm];
        OFString value;
        size_t pos = 0;
        /* retrieve signed integer data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (value.empty() ||
#ifdef SCNd64
                (sscanf(value.c_str(), "%" SCNd64, &field[i]) != 1)
#elif SIZEOF_LONG == 8
                (sscanf(value.c_str(), "%ld", &field[i]) != 1)
#else // assume "long long" is 64 bits
                (sscanf(value.c_str(), "%lld", &field[i]) != 1)
#endif
                )
            {
                errorFlag = EC_CorruptedData;
            }
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putSint64Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSigned64bitVeryLong::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Sint64)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % OFstatic_cast(Uint32, sizeof(Sint64))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
