/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrlt.cc
** 
** Purpose:
** Implementation of class DcmLongText
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 15:10:15 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrlt.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrlt.h"
#include "dcdebug.h"


// ********************************


DcmLongText::DcmLongText(const DcmTag &tag,
                         const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = 10240;
}


// ********************************


DcmLongText::DcmLongText(const DcmLongText& old)
: DcmCharString( old, EVR_LT )
{
    maxLength = 10240;
}


// ********************************


DcmLongText::~DcmLongText()
{
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrlt.cc,v $
** Revision 1.4  1997-07-03 15:10:15  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:50  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
