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
** Update Date:		$Date: 1996-01-05 13:27:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrae.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
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
Bdebug((5, "dcvrae:DcmApplicationEntity::DcmApplicationEntity"
           "(DcmTag&,len=%ld)", len ));

    maxLength = 16;
Edebug(());
}


// ********************************


DcmApplicationEntity::DcmApplicationEntity( const DcmApplicationEntity &newAE )
: DcmByteString( newAE, EVR_AE )
{
Bdebug((5, "dcvrae:DcmApplicationEntity::DcmApplicationEntity"
           "(DcmApplicationEntity&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmApplicationEntity::~DcmApplicationEntity()
{
Bdebug((5, "dcvrae:DcmApplicationEntity::~DcmApplicationEntity()" ));
Edebug(());

}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrae.cc,v $
** Revision 1.3  1996-01-05 13:27:45  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
