/*
**
** Author: Gerd Ehlers	    01.05.94 -- Created
**         Andreas Barth	04.12.95 -- New Stream 
** Kuratorium OFFIS e.V.
**
** Module: dcvrsh.cc
** 
** Purpose:
** Implementation of class DcmShortString
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 15:10:17 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrsh.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "dcvrsh.h"
#include "dcdebug.h"


// ********************************


DcmShortString::DcmShortString( const DcmTag &tag, const Uint32 len)
: DcmCharString(tag, len )
{
    maxLength = 16;
}


// ********************************


DcmShortString::DcmShortString( const DcmShortString& old )
: DcmCharString( old, EVR_SH )
{
}


// ********************************


DcmShortString::~DcmShortString(void)
{
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrsh.cc,v $
** Revision 1.4  1997-07-03 15:10:17  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:52  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
