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
 *  Purpose: Implementation of class DcmIntegerString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/ofstd/ofstring.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


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
#if SIZEOF_LONG == 8
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


/*
** CVS/RCS Log:
** $Log: dcvris.cc,v $
** Revision 1.26  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.25  2010-10-14 13:14:10  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.24  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.23  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.22  2009-08-03 09:03:00  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.21  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.20  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.19  2005/12/08 15:41:54  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.18  2002/12/06 13:20:50  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.17  2002/11/27 12:06:57  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.16  2002/08/27 16:55:59  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.15  2002/04/25 10:30:35  joergr
** Removed getOFStringArray() implementation.
**
** Revision 1.14  2001/09/25 17:19:57  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.13  2001/06/01 15:49:17  meichel
** Updated copyright header
**
** Revision 1.12  2000/03/08 16:26:47  meichel
** Updated copyright header.
**
** Revision 1.11  1999/03/31 09:25:52  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.10  1998/11/12 16:48:25  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.9  1997/08/29 13:11:46  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.8  1997/08/29 08:32:58  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.7  1997/07/03 15:10:13  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.6  1997/05/30 06:45:00  andreas
** - fixed scanf format problem leading to warnings on 64 bit machines.
**
** Revision 1.5  1997/05/12 09:57:02  andreas
** - warnings deletetd
**
** Revision 1.4  1997/05/12 07:38:27  andreas
** - new get-Methods for DcmDecimalString: getFloat64 and
**   DcmIntegerString: getSint32
**
** Revision 1.3  1996/01/05 13:27:49  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
