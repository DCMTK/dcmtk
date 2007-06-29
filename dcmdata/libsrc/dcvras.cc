/*
 *
 *  Copyright (C) 1994-2007, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmAgeString
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2007-06-29 14:17:49 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvras.cc,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
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


// ********************************


DcmEVR DcmAgeString::ident() const
{
    return EVR_AS;
}


/*
** CVS/RCS Log:
** $Log: dcvras.cc,v $
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

