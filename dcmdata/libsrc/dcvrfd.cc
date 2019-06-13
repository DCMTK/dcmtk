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
 *  Purpose: Implementation of class DcmFloatingPointDouble
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcvrfd.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************

DcmFloatingPointDouble::DcmFloatingPointDouble(const DcmTag &tag)
  : DcmElement(tag, 0)
{
}


DcmFloatingPointDouble::DcmFloatingPointDouble(const DcmTag &tag,
                                               const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmFloatingPointDouble::DcmFloatingPointDouble(const DcmFloatingPointDouble &old)
  : DcmElement(old)
{
}


DcmFloatingPointDouble::~DcmFloatingPointDouble()
{
}


DcmFloatingPointDouble &DcmFloatingPointDouble::operator=(const DcmFloatingPointDouble &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


int DcmFloatingPointDouble::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmFloatingPointDouble* myThis = NULL;
    DcmFloatingPointDouble* myRhs = NULL;
    myThis = OFconst_cast(DcmFloatingPointDouble*, this);
    myRhs = OFstatic_cast(DcmFloatingPointDouble*, OFconst_cast(DcmElement*, &rhs));

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
        Float64 val = 0;
        if (myThis->getFloat64(val, count).good())
        {
            Float64 rhsVal = 0;
            if (myRhs->getFloat64(rhsVal, count).good())
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


OFCondition DcmFloatingPointDouble::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmFloatingPointDouble &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmFloatingPointDouble::ident() const
{
    return EVR_FD;
}


OFCondition DcmFloatingPointDouble::checkValue(const OFString &vm,
                                               const OFBool /*oldFormat*/)
{
    /* check VM only, further checks on the floating point values could be added later */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmFloatingPointDouble::getVM()
{
    return getNumberOfValues();
}


unsigned long DcmFloatingPointDouble::getNumberOfValues()
{
    return OFstatic_cast(unsigned long, getLengthField() / sizeof(Float64));
}


// ********************************


void DcmFloatingPointDouble::print(STD_NAMESPACE ostream &out,
                                   const size_t flags,
                                   const int level,
                                   const char * /*pixelFileName*/,
                                   size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get double data */
        Float64 *doubleVals;
        errorFlag = getFloat64Array(doubleVals);
        if (doubleVals != NULL)
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
                char buffer[64];
                /* print line start with tag and VR */
                printInfoLineStart(out, flags, level);
                /* print multiple values */
                for (unsigned int i = 0; i < count; i++, doubleVals++)
                {
                    /* check whether first value is printed (omit delimiter) */
                    if (i == 0)
                        OFStandard::ftoa(buffer, sizeof(buffer), *doubleVals, 0, 0, 17 /* DBL_DIG + 2 for DICOM FD */);
                    else
                    {
                        buffer[0] = '\\';
                        OFStandard::ftoa(buffer + 1, sizeof(buffer) - 1, *doubleVals, 0, 0, 17 /* DBL_DIG + 2 for DICOM FD */);
                    }
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
            printInfoLine(out, flags, level, "(no value available)" );
    } else
        printInfoLine(out, flags, level, "(not loaded)" );
}


// ********************************


OFCondition DcmFloatingPointDouble::getFloat64(Float64 &doubleVal,
                                               const unsigned long pos)
{
    /* get double data */
    Float64 *doubleValues = NULL;
    errorFlag = getFloat64Array(doubleValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (doubleValues == NULL)
            errorFlag = EC_IllegalCall;
        /* do not use getVM() because derived classes might always return 1 */
        else if (pos >= getNumberOfValues())
            errorFlag = EC_IllegalParameter;
        else
            doubleVal = doubleValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        doubleVal = 0;
    return errorFlag;
}


OFCondition DcmFloatingPointDouble::getFloat64Array(Float64 *&doubleVals)
{
    doubleVals = OFstatic_cast(Float64 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::getOFString(OFString &stringVal,
                                                const unsigned long pos,
                                                OFBool /*normalize*/)
{
    Float64 doubleVal;
    /* get the specified numeric value */
    errorFlag = getFloat64(doubleVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[64];
        OFStandard::ftoa(buffer, sizeof(buffer), doubleVal, 0, 0, 17 /* DBL_DIG + 2 for DICOM FD */);
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::putFloat64(const Float64 doubleVal,
                                               const unsigned long pos)
{
    Float64 val = doubleVal;
    errorFlag = changeValue(&val, OFstatic_cast(Uint32, sizeof(Float64) * pos), OFstatic_cast(Uint32, sizeof(Float64)));
    return errorFlag;
}


OFCondition DcmFloatingPointDouble::putFloat64Array(const Float64 *doubleVals,
                                                    const unsigned long numDoubles)
{
    errorFlag = EC_Normal;
    if (numDoubles > 0)
    {
        /* check for valid data */
        if (doubleVals != NULL)
            errorFlag = putValue(doubleVals, OFstatic_cast(Uint32, sizeof(Float64) * OFstatic_cast(size_t, numDoubles)));
        else
            errorFlag = EC_CorruptedData;
    } else
        putValue(NULL, 0);

    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmFloatingPointDouble::putString(const char *stringVal,
                                              const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Float64 *field = new Float64[vm];
        OFBool success = OFFalse;
        OFString value;
        size_t pos = 0;
        /* retrieve double data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (!value.empty())
            {
                field[i] = OFStandard::atof(value.c_str(), &success);
                if (!success)
                    errorFlag = EC_CorruptedData;
            } else
                errorFlag = EC_CorruptedData;
        }
        /* set binary data as the element value */
        if (errorFlag == EC_Normal)
            errorFlag = putFloat64Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Float64)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % OFstatic_cast(Uint32, sizeof(Float64))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}


OFBool DcmFloatingPointDouble::matches(const DcmElement& candidate,
                                       const OFBool enableWildCardMatching) const
{
  OFstatic_cast(void,enableWildCardMatching);
  if (ident() == candidate.ident())
  {
    // some const casts to call the getter functions, I do not modify the values, I promise!
    DcmFloatingPointDouble& key = OFconst_cast(DcmFloatingPointDouble&,*this);
    DcmElement& can = OFconst_cast(DcmElement&,candidate);
    Float64 a, b;
    for( unsigned long ui = 0; ui < key.getVM(); ++ui )
      for( unsigned long uj = 0; uj < can.getVM(); ++uj )
        if( key.getFloat64( a, ui ).good() && can.getFloat64( b, uj ).good() && a == b )
          return OFTrue;
    return key.getVM() == 0;
  }
  return OFFalse;
}
