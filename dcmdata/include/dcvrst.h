/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrst.h
**
** Purpose:
** Interface of class DcmShortText
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:10 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrst.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRST_H
#define DCVRST_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmShortText : public DcmCharString 
{
  public:
    DcmShortText(const DcmTag &tag, const Uint32 len = 0);
    DcmShortText(const DcmShortText& old );
    virtual ~DcmShortText(void);

    virtual DcmEVR ident(void) const { return EVR_ST; }
    virtual unsigned long getVM(void) { return 1L; }
};


#endif // DCVRST_H

/*
** CVS/RCS Log:
** $Log: dcvrst.h,v $
** Revision 1.3  1996-01-05 13:23:10  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
