/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrlo.cc
** 
** Purpose:
** Implementation of class DcmLongString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:50 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrlo.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrlo.h"
#include "dcdebug.h"


// ********************************


DcmLongString::DcmLongString(const DcmTag &tag,
							 const Uint32 len)
: DcmCharString(tag, len)
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmTag&,len=%ld)", len ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::DcmLongString( const DcmLongString& old )
: DcmCharString( old, EVR_LO )
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmLongString&)" ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::~DcmLongString()
{
Bdebug((5, "dcvrlo:DcmLongString::~DcmLongString()" ));
Edebug(());

}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrlo.cc,v $
** Revision 1.3  1996-01-05 13:27:50  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/

