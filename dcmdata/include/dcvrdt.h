/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrdt.h
**
** Purpose:
** Interface of class DcmDateTime
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:05 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrdt.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRDT_H
#define DCVRDT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmDateTime : public DcmByteString 
{
  public:
    DcmDateTime(const DcmTag &tag, const Uint32 len = 0);
    DcmDateTime( const DcmDateTime &newDT );
    virtual ~DcmDateTime();

    virtual DcmEVR ident() const { return EVR_DT; }
};


#endif // DCVRDT_H

/*
** CVS/RCS Log:
** $Log: dcvrdt.h,v $
** Revision 1.3  1996-01-05 13:23:05  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
