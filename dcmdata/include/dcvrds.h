/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrds.h
**
** Purpose:
** Interface of class DcmDecimalString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:04 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrds.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRDS_H
#define DCVRDS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmDecimalString : public DcmByteString 
{
  public:
    DcmDecimalString(const DcmTag &tag, const Uint32 len = 0);
    DcmDecimalString( const DcmDecimalString &newDS );
    virtual ~DcmDecimalString();

    virtual DcmEVR ident() const { return EVR_DS; }
};


#endif // DCVRDS_H

/*
** CVS/RCS Log:
** $Log: dcvrds.h,v $
** Revision 1.3  1996-01-05 13:23:04  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
