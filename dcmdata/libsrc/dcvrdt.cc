/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrdt.cc
**
** Purpose:
** Implementation of class DcmDateTime
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:48 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrdt.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrdt.h"
#include "dcdebug.h"



// ********************************


DcmDateTime::DcmDateTime(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvrdt:DcmDateTime::DcmDateTime(DcmTag&,len=%ld)", len ));

    maxLength = 26;
Edebug(());

}


// ********************************


DcmDateTime::DcmDateTime( const DcmDateTime &newDT )
: DcmByteString( newDT, EVR_DT )
{
Bdebug((5, "dcvrdt:DcmDateTime::DcmDateTime(DcmDateTime&)" ));

    maxLength = 26;
Edebug(());

}


// ********************************


DcmDateTime::~DcmDateTime()
{
Bdebug((5, "dcvrdt:DcmDateTime::~DcmDateTime()" ));
Edebug(());

}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrdt.cc,v $
** Revision 1.3  1996-01-05 13:27:48  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
