/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrda.h
**
** Purpose:
** Interface of class DcmDate
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:47 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrda.h,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRDA_H
#define DCVRDA_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include "dctypes.h"
#include "dcbytstr.h"



class DcmDate : public DcmByteString 
{
  public:
    DcmDate(const DcmTag &tag, const Uint32 len = 0);
    DcmDate( const DcmDate &newDA );
    virtual ~DcmDate();

    DcmDate &operator=(const DcmDate &obj) { DcmByteString::operator=(obj); return *this; }

    virtual DcmEVR ident() const { return EVR_DA; }
};


#endif // DCVRDA_H

/*
** CVS/RCS Log:
** $Log: dcvrda.h,v $
** Revision 1.4  1998-11-12 16:47:47  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1996/01/05 13:23:04  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
