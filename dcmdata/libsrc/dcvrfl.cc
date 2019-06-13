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
 *  Purpose: Implementation of class DcmFloatingPointSingle
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcvrfl.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmTag &tag)
  : DcmElement(tag, 0)
{
}


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmTag &tag,
                                               const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmFloatingPointSingle &old)
  : DcmElement(old)
{
}


DcmFloatingPointSingle::~DcmFloatingPointSingle()
{
}


DcmFloatingPointSingle &DcmFloatingPointSingle::operator=(const DcmFloatingPointSingle &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


int DcmFloatingPointSingle::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmFloatingPointSingle* myThis = NULL;
    DcmFloatingPointSingle* myRhs = NULL;
    myThis = OFconst_cast(DcmFloatingPointSingle*, this);
    myRhs = OFstatic_cast(DcmFloatingPointSingle*, OFconst_cast(DcmElement*, &rhs));

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

    // iterate over all components and test equality */
    for (unsigned long count = 0; count < thisNumValues; count++)
    {
        Float32 val = 0;
        if (myThis->getFloat32(val, count).good())
        {
            Float32 rhsVal = 0;
            if (myRhs->getFloat32(rhsVal, count).good())
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



OFCondition DcmFloatingPointSingle::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmFloatingPointSingle &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmFloatingPointSingle::ident() const
{
    return EVR_FL;
}


OFCondition DcmFloatingPointSingle::checkValue(const OFString &vm,
                                               const OFBool /*oldFormat*/)
{
    /* check VM only, further checks on the floating point values could be added later */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmFloatingPointSingle::getVM()
{
    return getNumberOfValues();
}


unsigned long DcmFloatingPointSingle::getNumberOfValues()
{
    return OFstatic_cast(unsigned long, getLengthField() / sizeof(Float32));
}


// ********************************


void DcmFloatingPointSingle::print(STD_NAMESPACE ostream &out,
                                   const size_t flags,
                                   const int level,
                                   const char * /*pixelFileName*/,
                                   size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get float data */
        Float32 *floatVals;
        errorFlag = getFloat32Array(floatVals);
        if (floatVals != NULL)
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
                for (unsigned int i = 0; i < count; i++, floatVals++)
                {
                    /* check whether first value is printed (omit delimiter) */
                    if (i == 0)
                        OFStandard::ftoa(buffer, sizeof(buffer), *floatVals, 0, 0, 8 /* FLT_DIG + 2 for DICOM FL */);
                    else
                    {
                        buffer[0] = '\\';
                        OFStandard::ftoa(buffer + 1, sizeof(buffer) - 1, *floatVals, 0, 0, 8 /* FLT_DIG + 2 for DICOM FL */);
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
                /* count can be zero if we have an invalid element with less than four bytes length */
                printInfoLine(out, flags, level, "(invalid value)");
            }
        } else
            printInfoLine(out, flags, level, "(no value available)" );
    } else
        printInfoLine(out, flags, level, "(not loaded)" );
}


// ********************************


OFCondition DcmFloatingPointSingle::getFloat32(Float32 &floatVal,
                                               const unsigned long pos)
{
    /* get float data */
    Float32 *floatValues = NULL;
    errorFlag = getFloat32Array(floatValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (floatValues == NULL)
            errorFlag = EC_IllegalCall;
        /* do not use getVM() because derived classes might always return 1 */
        else if (pos >= getNumberOfValues())
            errorFlag = EC_IllegalParameter;
        else
            floatVal = floatValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        floatVal = 0;
    return errorFlag;
}


OFCondition DcmFloatingPointSingle::getFloat32Array(Float32 *&floatVals)
{
    floatVals = OFstatic_cast(Float32 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::getOFString(OFString &value,
                                                const unsigned long pos,
                                                OFBool /*normalize*/)
{
    Float32 floatVal;
    /* get the specified numeric value */
    errorFlag = getFloat32(floatVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[64];
        OFStandard::ftoa(buffer, sizeof(buffer), floatVal, 0, 0, 8 /* FLT_DIG + 2 for DICOM FL */);
        /* assign result */
        value = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::putFloat32(const Float32 floatVal,
                                               const unsigned long pos)
{
    Float32 val = floatVal;
    errorFlag = changeValue(&val, OFstatic_cast(Uint32, sizeof(Float32) * pos), OFstatic_cast(Uint32, sizeof(Float32)));
    return errorFlag;
}


OFCondition DcmFloatingPointSingle::putFloat32Array(const Float32 *floatVals,
                                                    const unsigned long numFloats)
{
    errorFlag = EC_Normal;
    if (numFloats > 0)
    {
        /* check for valid float data */
        if (floatVals != NULL)
            errorFlag = putValue(floatVals, OFstatic_cast(Uint32, sizeof(Float32) * OFstatic_cast(size_t, numFloats)));
        else
            errorFlag = EC_CorruptedData;
    } else
        putValue(NULL, 0);

    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmFloatingPointSingle::putString(const char *stringVal,
                                              const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Float32 *field = new Float32[vm];
        OFBool success = OFFalse;
        OFString value;
        size_t pos = 0;
        /* retrieve float data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (!value.empty())
            {
                field[i] = OFstatic_cast(Float32, OFStandard::atof(value.c_str(), &success));
                if (!success)
                    errorFlag = EC_CorruptedData;
            } else
                errorFlag = EC_CorruptedData;
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putFloat32Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Float32)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % OFstatic_cast(Uint32, sizeof(Float32))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}


OFBool DcmFloatingPointSingle::matches(const DcmElement& candidate,
                                       const OFBool enableWildCardMatching) const
{
  OFstatic_cast(void,enableWildCardMatching);
  if (ident() == candidate.ident())
  {
    // some const casts to call the getter functions, I do not modify the values, I promise!
    DcmFloatingPointSingle& key = OFconst_cast(DcmFloatingPointSingle&,*this);
    DcmElement& can = OFconst_cast(DcmElement&,candidate);
    Float32 a, b;
    for( unsigned long ui = 0; ui < key.getVM(); ++ui )
      for( unsigned long uj = 0; uj < can.getVM(); ++uj )
        if( key.getFloat32( a, ui ).good() && can.getFloat32( b, uj ).good() && a == b )
          return OFTrue;
    return key.getVM() == 0;
  }
  return OFFalse;
}
