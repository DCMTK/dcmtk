/*
**
** Author: Gerd Ehlers	    01.05.94 -- Created
**         Andreas Barth	04.12.95 -- New Stream 
** Kuratorium OFFIS e.V.
**
** Module: dcvrpn.cc
** 
** Purpose:
** Implementation of class DcmPersonName
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:52 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrpn.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "dcvrpn.h"
#include "dcdebug.h"


// ********************************


DcmPersonName::DcmPersonName(const DcmTag &tag, const Uint32 len)
: DcmCharString(tag, len)
{
Bdebug((5, "dcvrpn:DcmPersonName::DcmPersonName(DcmTag&,len=%ld)", len ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmPersonName::DcmPersonName(const DcmPersonName& old)
: DcmCharString( old, EVR_PN )
{
Bdebug((5, "dcvrpn:DcmPersonName::DcmPersonName(DcmPersonName&)" ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmPersonName::~DcmPersonName(void)
{
Bdebug((5, "dcvrpn:DcmPersonName::~DcmPersonName()" ));
Edebug(());

}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrpn.cc,v $
** Revision 1.3  1996-01-05 13:27:52  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
