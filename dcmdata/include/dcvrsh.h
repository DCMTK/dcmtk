/*
**
** Author: Gerd Ehlers	    01.05.94 -- Created
**         Andreas Barth	04.12.95 -- New Stream 
** Kuratorium OFFIS e.V.
**
** Module: dcvrsh.h
**
** Purpose:
** Interface of class DcmShortString
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:08 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrsh.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRSH_H
#define DCVRSH_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmShortString : public DcmCharString 
{
  public:
    DcmShortString(const DcmTag &tag, const Uint32 len = 0);
    DcmShortString(const DcmShortString& old);
    virtual ~DcmShortString(void);

    virtual DcmEVR ident(void) const { return EVR_SH; }
};


#endif // DCVRSH_H

/*
** CVS/RCS Log:
** $Log: dcvrsh.h,v $
** Revision 1.3  1996-01-05 13:23:08  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
