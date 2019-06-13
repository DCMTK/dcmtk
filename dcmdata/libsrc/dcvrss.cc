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
 *  Purpose: Implementation of class DcmSignedShort
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvrss.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmSignedShort::DcmSignedShort(const DcmTag &tag)
  : DcmElement(tag, 0)
{
}


DcmSignedShort::DcmSignedShort(const DcmTag &tag,
                               const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmSignedShort::DcmSignedShort(const DcmSignedShort &old)
  : DcmElement(old)
{
}


DcmSignedShort::~DcmSignedShort()
{
}


DcmSignedShort &DcmSignedShort::operator=(const DcmSignedShort &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


int DcmSignedShort::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmSignedShort* myThis = NULL;
    DcmSignedShort* myRhs = NULL;
    myThis = OFconst_cast(DcmSignedShort*, this);
    myRhs =  OFstatic_cast(DcmSignedShort*, OFconst_cast(DcmElement*, &rhs));

    /* compare number of values */
    unsigned long rhisNumValues = myThis->getNumberOfValues();
    unsigned long rhsNumValues = myRhs->getNumberOfValues();
    if (rhisNumValues < rhsNumValues)
    {
        return -1;
    }
    else if (rhisNumValues > rhsNumValues)
    {
        return 1;
    }

    /* iterate over all components and test equality */
    for (unsigned long count = 0; count < rhisNumValues; count++)
    {
        Sint16 val = 0;
        if (myThis->getSint16(val, count).good())
        {
            Sint16 rhsVal = 0;
            if (myRhs->getSint16(rhsVal, count).good())
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


OFCondition DcmSignedShort::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmSignedShort &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmSignedShort::ident() const
{
    return EVR_SS;
}


OFCondition DcmSignedShort::checkValue(const OFString &vm,
                                       const OFBool /*oldFormat*/)
{
    /* check VM only */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmSignedShort::getVM()
{
    return getNumberOfValues();
}


unsigned long DcmSignedShort::getNumberOfValues()
{
    return OFstatic_cast(unsigned long, getLengthField() / sizeof(Sint16));
}


// ********************************


void DcmSignedShort::print(STD_NAMESPACE ostream &out,
                           const size_t flags,
                           const int level,
                           const char * /*pixelFileName*/,
                           size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get signed integer data */
        Sint16 *sintVals;
        errorFlag = getSint16Array(sintVals);
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
                    if (i == 0)
                        sprintf(buffer, "%hd", *sintVals);
                    else
                        sprintf(buffer, "\\%hd", *sintVals);
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


OFCondition DcmSignedShort::getSint16(Sint16 &sintVal,
                                      const unsigned long pos)
{
    /* get signed integer data */
    Sint16 *sintValues = NULL;
    errorFlag = getSint16Array(sintValues);
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


OFCondition DcmSignedShort::getSint16Array(Sint16 *&sintVals)
{
    sintVals = OFstatic_cast(Sint16 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::getOFString(OFString &stringVal,
                                        const unsigned long pos,
                                        OFBool /*normalize*/)
{
    Sint16 sintVal;
    /* get the specified numeric value */
    errorFlag = getSint16(sintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "%i", sintVal);
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::putSint16(const Sint16 sintVal,
                                      const unsigned long pos)
{
    Sint16 val = sintVal;
    errorFlag = changeValue(&val, OFstatic_cast(Uint32, sizeof(Sint16) * pos), OFstatic_cast(Uint32, sizeof(Sint16)));
    return errorFlag;
}


OFCondition DcmSignedShort::putSint16Array(const Sint16 *sintVals,
                                           const unsigned long numSints)
{
    errorFlag = EC_Normal;
    if (numSints > 0)
    {
        /* check for valid data */
        if (sintVals != NULL)
            errorFlag = putValue(sintVals, OFstatic_cast(Uint32, sizeof(Sint16) * OFstatic_cast(size_t, numSints)));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmSignedShort::putString(const char *stringVal,
                                      const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Sint16 *field = new Sint16[vm];
        OFString value;
        size_t pos = 0;
        /* retrieve signed integer data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (value.empty() || (sscanf(value.c_str(), "%hd", &field[i]) != 1))
                errorFlag = EC_CorruptedData;
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putSint16Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Sint16)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % OFstatic_cast(Uint32, sizeof(Sint16))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}


OFBool DcmSignedShort::matches(const DcmElement& candidate,
                               const OFBool enableWildCardMatching) const
{
  OFstatic_cast(void,enableWildCardMatching);
  if (ident() == candidate.ident())
  {
    // some const casts to call the getter functions, I do not modify the values, I promise!
    DcmSignedShort& key = OFconst_cast(DcmSignedShort&,*this);
    DcmElement& can = OFconst_cast(DcmElement&,candidate);
    Sint16 a, b;
    for( unsigned long ui = 0; ui < key.getVM(); ++ui )
      for( unsigned long uj = 0; uj < can.getVM(); ++uj )
        if( key.getSint16( a, ui ).good() && can.getSint16( b, uj ).good() && a == b )
          return OFTrue;
    return key.getVM() == 0;
  }
  return OFFalse;
}
