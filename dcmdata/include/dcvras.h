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
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:02 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvras.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
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
    DcmAgeString( const DcmAgeString &newAS );
    virtual ~DcmAgeString(void);

    virtual DcmEVR ident(void) const { return EVR_AS; }
};


#endif // DCVRAS_H

/*
** CVS/RCS Log:
** $Log: dcvras.h,v $
** Revision 1.3  1996-01-05 13:23:02  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
