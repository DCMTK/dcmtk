/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrds.cc
**
** Purpose:
** Implementation of class DcmDecimalString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:47 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrds.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrds.h"
#include "dcdebug.h"


// ********************************


DcmDecimalString::DcmDecimalString(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmTag&,len=%ld)",
           len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::DcmDecimalString( const DcmDecimalString &newDS )
: DcmByteString( newDS, EVR_DS )
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmDecimalString&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::~DcmDecimalString()
{
Bdebug((5, "dcvrds:DcmDecimalString::~DcmDecimalString()" ));
Edebug(());

}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrds.cc,v $
** Revision 1.3  1996-01-05 13:27:47  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
