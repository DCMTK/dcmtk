/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrtm.h
**
** Purpose:
** Interface of class DcmTime
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:10 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrtm.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRTM_H
#define DCVRTM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmTime : public DcmByteString 
{
  public:
    DcmTime(const DcmTag &tag, const Uint32 len = 0); 
    DcmTime(const DcmTime& old);
    virtual ~DcmTime(void);

    virtual DcmEVR ident(void) const { return EVR_TM; }
};


#endif // DCVRTM_H

/*
** CVS/RCS Log:
** $Log: dcvrtm.h,v $
** Revision 1.3  1996-01-05 13:23:10  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
