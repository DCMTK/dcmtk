/*
**
** Author: Gerd Ehlers      01.05.94 
**         Andreas Barth    12.05.97 
** Kuratorium OFFIS e.V.
**
** Module: dcvrds.h
**
** Purpose:
** Interface of class DcmDecimalString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-05-12 07:37:44 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrds.h,v $
** CVS/RCS Revision:	$Revision: 1.4 $
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
    virtual E_Condition getFloat64(Float64 & val, const unsigned long pos = 0);
};


#endif // DCVRDS_H

/*
** CVS/RCS Log:
** $Log: dcvrds.h,v $
** Revision 1.4  1997-05-12 07:37:44  andreas
** - new get-Methods for DcmDecimalString: getFloat64 and
**   DcmIntegerString: getSint32
**
** Revision 1.3  1996/01/05 13:23:04  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
