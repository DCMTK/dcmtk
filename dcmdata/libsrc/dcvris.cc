/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvris.cc
** 
** Purpose:
** Implementation of class DcmIntegerString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:49 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvris.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvris.h"
#include "dcdebug.h"


// ********************************


DcmIntegerString::DcmIntegerString(const DcmTag &tag,
								   const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmTag&,len=%ld)",
           len ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::DcmIntegerString( const DcmIntegerString& old )
: DcmByteString( old, EVR_IS )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmIntegerString&)" ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::~DcmIntegerString()
{
Bdebug((5, "dcvris:DcmIntegerString::~DcmIntegerString()" ));
Edebug(());

}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvris.cc,v $
** Revision 1.3  1996-01-05 13:27:49  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/



