/*
**
** Author: Gerd Ehlers	    05.05.94 -- Created
**         Andreas Barth	04.12.95 -- New Stream 
** Kuratorium OFFIS e.V.
**
** Module: dcvrae.cc
** 
** Purpose:
** Implementation of class DcmApplicationEntity
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 15:10:07 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrae.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrae.h"
#include "dcdebug.h"



// ********************************


DcmApplicationEntity::DcmApplicationEntity(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
    maxLength = 16;
}


// ********************************


DcmApplicationEntity::DcmApplicationEntity( const DcmApplicationEntity &newAE )
: DcmByteString( newAE, EVR_AE )
{
    maxLength = 16;
}


// ********************************


DcmApplicationEntity::~DcmApplicationEntity()
{
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrae.cc,v $
** Revision 1.4  1997-07-03 15:10:07  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:45  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
