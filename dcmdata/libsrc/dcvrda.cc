/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Purpose: class DcmDate
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:46 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrda.cc,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrda.h"
#include "dcdebug.h"


// ********************************


DcmDate::DcmDate(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
    maxLength = 10;
}


// ********************************


DcmDate::DcmDate( const DcmDate &newDA )
: DcmByteString(newDA)
{
}


// ********************************


DcmDate::~DcmDate()
{
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrda.cc,v $
** Revision 1.7  2000-03-08 16:26:46  meichel
** Updated copyright header.
**
** Revision 1.6  1999/03/31 09:25:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1998/11/12 16:48:23  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.4  1997/07/03 15:10:10  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:47  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
