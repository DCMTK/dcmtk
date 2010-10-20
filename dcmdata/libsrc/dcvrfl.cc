/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.36 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


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
    return getLengthField() / sizeof(Float32);
}


// ********************************


void DcmFloatingPointSingle::print(STD_NAMESPACE ostream&out,
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
            const unsigned long count = getLengthField() / sizeof(Float32) /* do not use getVM()! */;
            const unsigned long maxLength = (flags & DCMTypes::PF_shortenLongTagValues) ?
                DCM_OptPrintLineLength : OFstatic_cast(unsigned long, -1);
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
        else if (pos >= getLengthField() / sizeof(Float32) /* do not use getVM()! */)
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
    errorFlag = changeValue(&val, sizeof(Float32) * pos, sizeof(Float32));
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
            errorFlag = putValue(floatVals, sizeof(Float32) * OFstatic_cast(Uint32, numFloats));
        else
            errorFlag = EC_CorruptedData;
    } else
        putValue(NULL, 0);

    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        const unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Float32 *field = new Float32[vm];
            OFBool success = OFFalse;
            const char *s = stringVal;
            char *value;
            /* retrieve float data from character string */
            for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if (value != NULL)
                {
                    field[i] = OFstatic_cast(Float32, OFStandard::atof(value, &success));
                    if (!success)
                        errorFlag = EC_CorruptedData;
                    delete[] value;
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
            setLengthField(getLengthField() - (getLengthField() % (sizeof(Float32))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}


/*
** CVS/RCS Log:
** $Log: dcvrfl.cc,v $
** Revision 1.36  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.35  2010-10-14 13:14:10  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.34  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.33  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.32  2008-05-30 12:50:05  meichel
** Increased output precision to 8 (FLT_DIG+2) when converting an FL element to string.
**
** Revision 1.31  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.30  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.29  2005/12/08 15:41:53  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.28  2004/02/04 16:17:03  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.27  2002/12/06 13:12:39  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.26  2002/12/04 10:41:02  meichel
** Changed toolkit to use OFStandard::ftoa instead of sprintf for all
**   double to string conversions that are supposed to be locale independent
**
** Revision 1.25  2002/11/27 12:06:57  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.24  2002/06/26 17:19:34  joergr
** Added type cast to keep MSVC6 quiet.
**
** Revision 1.23  2002/06/20 12:06:18  meichel
** Changed toolkit to use OFStandard::atof instead of atof, strtod or
**   sscanf for all string to double conversions that are supposed to
**   be locale independent
**
** Revision 1.22  2002/04/25 10:30:02  joergr
** Added getOFString() implementation.
**
** Revision 1.21  2002/04/16 13:43:24  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.20  2001/09/25 17:19:57  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.19  2001/06/01 15:49:17  meichel
** Updated copyright header
**
** Revision 1.18  2000/04/14 15:55:08  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.17  2000/03/08 16:26:47  meichel
** Updated copyright header.
**
** Revision 1.16  2000/03/03 14:05:39  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.15  2000/02/10 10:52:24  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.14  2000/02/02 14:32:57  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.13  1999/03/31 09:25:52  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.12  1997/07/21 08:25:33  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.11  1997/07/03 15:10:13  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.10  1997/04/18 08:10:52  andreas
** - Corrected debugging code
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.9  1996/09/27 08:20:40  hewett
** Added explicit cast: (Float32)0.0
**
** Revision 1.8  1996/08/05 08:46:20  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:51  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/16 16:05:23  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/26 09:59:36  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:32  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:49  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
