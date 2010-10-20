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
 *  Purpose: Implementation of class DcmAgeString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvras.h"


// ********************************


DcmAgeString::DcmAgeString(const DcmTag &tag,
                           const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(4);
}


DcmAgeString::DcmAgeString(const DcmAgeString &old)
  : DcmByteString(old)
{
}


DcmAgeString::~DcmAgeString()
{
}

DcmAgeString &DcmAgeString::operator=(const DcmAgeString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmAgeString::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmAgeString &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmAgeString::ident() const
{
    return EVR_AS;
}


OFCondition DcmAgeString::checkValue(const OFString &vm,
                                     const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmAgeString::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmAgeString::checkStringValue(const OFString &value,
                                           const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "as", 1);
}


/*
** CVS/RCS Log:
** $Log: dcvras.cc,v $
** Revision 1.16  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.15  2010-10-14 13:14:09  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.14  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.13  2009-08-03 09:02:59  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.12  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.11  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.10  2005/12/08 15:41:46  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.9  2002/12/06 13:20:48  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.8  2001/06/01 15:49:14  meichel
** Updated copyright header
**
** Revision 1.7  2000/03/08 16:26:45  meichel
** Updated copyright header.
**
** Revision 1.6  1999/03/31 09:25:46  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1998/11/12 16:48:22  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.4  1997/07/03 15:10:08  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:45  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
