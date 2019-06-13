/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmUnsignedShort
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvrus.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmUnsignedShort::DcmUnsignedShort(const DcmTag &tag)
  : DcmElement(tag, 0)
{
}


DcmUnsignedShort::DcmUnsignedShort(const DcmTag &tag, const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmUnsignedShort::DcmUnsignedShort(const DcmUnsignedShort &old)
  : DcmElement(old)
{
}


DcmUnsignedShort::~DcmUnsignedShort()
{
}


DcmUnsignedShort &DcmUnsignedShort::operator=(const DcmUnsignedShort &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


int DcmUnsignedShort::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmUnsignedShort* myThis = NULL;
    DcmUnsignedShort* myRhs = NULL;
    myThis = OFconst_cast(DcmUnsignedShort*, this);
    myRhs =  OFstatic_cast(DcmUnsignedShort*, OFconst_cast(DcmElement*, &rhs));

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
        Uint16 val = 0;
        if (myThis->getUint16(val, count).good())
        {
            Uint16 rhsVal = 0;
            if (myRhs->getUint16(rhsVal, count).good())
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


OFCondition DcmUnsignedShort::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmUnsignedShort &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmUnsignedShort::ident() const
{
    return EVR_US;
}


OFCondition DcmUnsignedShort::checkValue(const OFString &vm,
                                         const OFBool /*oldFormat*/)
{
    /* check VM only */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmUnsignedShort::getVM()
{
    return getNumberOfValues();
}


unsigned long DcmUnsignedShort::getNumberOfValues()
{
    return OFstatic_cast(unsigned long, getLengthField() / sizeof(Uint16));
}


// ********************************


void DcmUnsignedShort::print(STD_NAMESPACE ostream &out,
                             const size_t flags,
                             const int level,
                             const char * /*pixelFileName*/,
                             size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get unsigned integer data */
        Uint16 *uintVals;
        errorFlag = getUint16Array(uintVals);
        if (uintVals != NULL)
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
                for (unsigned int i = 0; i < count; i++, uintVals++)
                {
                    /* check whether first value is printed (omit delimiter) */
                    if (i == 0)
                        sprintf(buffer, "%hu", *uintVals);
                    else
                        sprintf(buffer, "\\%hu", *uintVals);
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
                /* count can be zero if we have an invalid element with less than two bytes length */
                printInfoLine(out, flags, level, "(invalid value)");
            }
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmUnsignedShort::getUint16(Uint16 &uintVal,
                                        const unsigned long pos)
{
    /* get unsigned integer data */
    Uint16 *uintValues = NULL;
    errorFlag = getUint16Array(uintValues);
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


OFCondition DcmUnsignedShort::getUint16Array(Uint16 *&uintVals)
{
    uintVals = OFstatic_cast(Uint16 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedShort::getOFString(OFString &stringVal,
                                          const unsigned long pos,
                                          OFBool /*normalize*/)
{
    Uint16 uintVal;
    /* get the specified numeric value */
    errorFlag = getUint16(uintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "%hu", uintVal);
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedShort::putUint16(const Uint16 uintVal,
                                        const unsigned long pos)
{
    Uint16 val = uintVal;
    errorFlag = changeValue(&val, OFstatic_cast(Uint32, sizeof(Uint16) * pos), OFstatic_cast(Uint32, sizeof(Uint16)));
    return errorFlag;
}


OFCondition DcmUnsignedShort::putUint16Array(const Uint16 *uintVals,
                                             const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints > 0)
    {
        /* check for valid data */
        if (uintVals != NULL)
            errorFlag = putValue(uintVals, OFstatic_cast(Uint32, sizeof(Uint16) * OFstatic_cast(size_t, numUints)));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedShort::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmUnsignedShort::putString(const char *stringVal,
                                        const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Uint16 *field = new Uint16[vm];
        OFString value;
        size_t pos = 0;
        /* retrieve unsigned integer data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (value.empty() || (sscanf(value.c_str(), "%hu", &field[i]) != 1))
                errorFlag = EC_CorruptedData;
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putUint16Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedShort::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Uint16)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % OFstatic_cast(Uint32, sizeof(Uint16))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}


OFBool DcmUnsignedShort::matches(const DcmElement& candidate,
                                 const OFBool enableWildCardMatching) const
{
  OFstatic_cast(void,enableWildCardMatching);
  if (ident() == candidate.ident())
  {
    // some const casts to call the getter functions, I do not modify the values, I promise!
    DcmUnsignedShort& key = OFconst_cast(DcmUnsignedShort&,*this);
    DcmElement& can = OFconst_cast(DcmElement&,candidate);
    Uint16 a, b;
    for( unsigned long ui = 0; ui < key.getVM(); ++ui )
      for( unsigned long uj = 0; uj < can.getVM(); ++uj )
        if( key.getUint16( a, ui ).good() && can.getUint16( b, uj ).good() && a == b )
          return OFTrue;
    return key.getVM() == 0;
  }
  return OFFalse;
}
