/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrlo.h
**
** Purpose:
** Interface of class DcmLongString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:06 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrlo.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRLO_H
#define DCVRLO_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmLongString : public DcmCharString 
{
  public:
    DcmLongString(const DcmTag &tag, const Uint32 len = 0);
    DcmLongString( const DcmLongString& old );
    virtual ~DcmLongString();

    virtual DcmEVR ident() const { return EVR_LO; }
};


#endif // DCVRLO_H

/*
** CVS/RCS Log:
** $Log: dcvrlo.h,v $
** Revision 1.3  1996-01-05 13:23:06  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
