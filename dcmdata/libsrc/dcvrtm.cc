/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrtm.cc
** 
** Purpose:
** Implementation of class DcmTime
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 15:10:19 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrtm.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "dcvrtm.h"
#include "dcdebug.h"


// ********************************


DcmTime::DcmTime(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
    maxLength = 16;
}


// ********************************


DcmTime::DcmTime(const DcmTime& old)
: DcmByteString(old, EVR_TM)
{
    maxLength = 16;
}


// ********************************


DcmTime::~DcmTime(void)
{
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrtm.cc,v $
** Revision 1.4  1997-07-03 15:10:19  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:54  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
