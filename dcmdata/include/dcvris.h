/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvris.h
**
** Purpose:
** Interface of class DcmIntegerString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:06 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvris.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRIS_H
#define DCVRIS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmIntegerString : public DcmByteString 
{
  public:
    DcmIntegerString(const DcmTag &tag, const Uint32 len = 0);
    DcmIntegerString(const DcmIntegerString& old);
    virtual ~DcmIntegerString();

    virtual DcmEVR ident() const { return EVR_IS; }
};


#endif // DCVRIS_H

/*
** CVS/RCS Log:
** $Log: dcvris.h,v $
** Revision 1.3  1996-01-05 13:23:06  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
