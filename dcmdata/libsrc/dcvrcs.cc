/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrcs.cc
**
** Purpose:
** Implementation of class DcmCodeString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:46 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrcs.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrcs.h"
#include "dcdebug.h"



// ********************************


DcmCodeString::DcmCodeString(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvrcs:DcmCodeString::DcmCodeString(DcmTag&,len=%ld)", len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmCodeString::DcmCodeString( const DcmCodeString &newCS )
: DcmByteString( newCS, EVR_CS )
{
Bdebug((5, "dcvrcs:DcmCodeString::DcmCodeString(DcmCodeString&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmCodeString::~DcmCodeString()
{
Bdebug((5, "dcvrcs:DcmCodeString::~DcmCodeString()" ));
Edebug(());

}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrcs.cc,v $
** Revision 1.3  1996-01-05 13:27:46  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
