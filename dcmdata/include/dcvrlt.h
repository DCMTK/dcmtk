/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrlt.h
**
** Purpose:
** Interface of class DcmLongText
** 
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:07 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrlt.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRLT_H
#define DCVRLT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmLongText : public DcmCharString 
{
  public:
    DcmLongText(const DcmTag &tag, const Uint32 len = 0);
    DcmLongText( const DcmLongText& old );
    virtual ~DcmLongText();

    virtual DcmEVR ident(void) const { return EVR_LT; }
    virtual unsigned long getVM(void) { return 1L; }
};


#endif // DCVRLT_H

/*
** CVS/RCS Log:
** $Log: dcvrlt.h,v $
** Revision 1.3  1996-01-05 13:23:07  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
