/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrst.cc
** 
** Purpose:
** Implementation of class DcmShortText
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 15:10:19 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrst.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "dcvrst.h"
#include "dcdebug.h"


// ********************************


DcmShortText::DcmShortText(const DcmTag &tag, const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = 1024;
}


// ********************************


DcmShortText::DcmShortText( const DcmShortText& old )
: DcmCharString( old, EVR_ST )
{
    maxLength = 1024;
}


// ********************************


DcmShortText::~DcmShortText()
{
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrst.cc,v $
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

