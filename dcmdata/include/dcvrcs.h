/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrcs.h
**
** Purpose:
** Interface of class DcmCodeString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-09 11:06:17 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrcs.h,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRCS_H
#define DCVRCS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmCodeString : public DcmByteString 
{
  public:
    DcmCodeString(const DcmTag &tag, const Uint32 len = 0);
    DcmCodeString( const DcmCodeString &newCS );
    virtual ~DcmCodeString();

    virtual DcmEVR ident(void) const { return EVR_CS; }
};


#endif // DCVRCS_H

/*
** CVS/RCS Log:
** $Log: dcvrcs.h,v $
** Revision 1.4  1996-01-09 11:06:17  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:23:03  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
