/*
**
** Author: Gerd Ehlers	    05.05.94 -- Created
**         Andreas Barth	04.12.95 -- New Stream 
** Kuratorium OFFIS e.V.
**
** Module: dcvras.h
**
** Purpose:
** Interface of class DcmAgeString
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvras.h,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRAS_H
#define DCVRAS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmAgeString : public DcmByteString 
{
  public:
    DcmAgeString(const DcmTag &tag, const Uint32 len = 0);
    DcmAgeString(const DcmAgeString &newAS);
    virtual ~DcmAgeString(void);

    DcmAgeString &operator=(const DcmAgeString &obj) { DcmByteString::operator=(obj); return *this; }
    
    virtual DcmEVR ident(void) const { return EVR_AS; }
};


#endif // DCVRAS_H

/*
** CVS/RCS Log:
** $Log: dcvras.h,v $
** Revision 1.4  1998-11-12 16:47:45  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1996/01/05 13:23:02  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
