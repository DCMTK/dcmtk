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
** Update Date:		$Date: 1996-01-05 13:27:54 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrtm.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
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
Bdebug((5, "dcvrtm:DcmTime::DcmTime(DcmTag&,len=%ld)", len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmTime::DcmTime(const DcmTime& old)
: DcmByteString(old, EVR_TM)
{
Bdebug((5, "dcvrtm:DcmTime::DcmTime(DcmTime&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmTime::~DcmTime(void)
{
Bdebug((5, "dcvrtm:DcmTime::~DcmTime()" ));
Edebug(());

}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrtm.cc,v $
** Revision 1.3  1996-01-05 13:27:54  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
