/*
**
** Author: Gerd Ehlers	    05.05.94 -- Created
**         Andreas Barth	04.12.95 -- New Stream 
** Kuratorium OFFIS e.V.
**
** Module: dcvras.cc
** 
** Purpose:
** Implementation of class DcmAgeString
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvras.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvras.h"
#include "dcdebug.h"



// ********************************


DcmAgeString::DcmAgeString(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvras:DcmAgeString::DcmAgeString(DcmTag&,len=%ld)", len ));

    maxLength = 4;
Edebug(());

}


// ********************************


DcmAgeString::DcmAgeString( const DcmAgeString &newAS )
: DcmByteString( newAS, EVR_AS )
{
Bdebug((5, "dcvras:DcmAgeString::DcmAgeString(DcmAgeString&)" ));

    maxLength = 4;
Edebug(());

}


// ********************************


DcmAgeString::~DcmAgeString()
{
Bdebug((5, "dcvras:DcmAgeString::~DcmAgeString()" ));
Edebug(());

}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvras.cc,v $
** Revision 1.3  1996-01-05 13:27:45  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/

