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
** Update Date:		$Date: 1996-01-05 13:27:52 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrsh.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
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
Bdebug((5, "dcvrsh:DcmShortString::DcmShortString(DcmTag&,len=%ld)",
           len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmShortString::DcmShortString( const DcmShortString& old )
: DcmCharString( old, EVR_SH )
{
Bdebug((5, "dcvrsh:DcmShortString::DcmShortString(DcmShortString&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmShortString::~DcmShortString(void)
{
Bdebug((5, "dcvrsh:DcmShortString::~DcmShortString()" ));
Edebug(());

}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrsh.cc,v $
** Revision 1.3  1996-01-05 13:27:52  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
