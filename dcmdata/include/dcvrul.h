/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrul.h
**
** Purpose:
** Interface of class DcmUnsignedLong
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:11 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrul.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRUL_H
#define DCVRUL_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"



class DcmUnsignedLong : public DcmElement 
{
public:
    DcmUnsignedLong(const DcmTag &tag, const Uint32 len = 0);
    DcmUnsignedLong(const DcmUnsignedLong& old );
    virtual ~DcmUnsignedLong(void);

    virtual DcmEVR ident(void) const { return EVR_UL; }
    virtual void print(const int level = 0);
    virtual unsigned long getVM(void);

    virtual E_Condition put(const Uint32 * uintVal,
                            const unsigned long numUints);     // number of longs

    virtual E_Condition put(const Uint32 uintVal);		// for one long only

    virtual E_Condition put(const Uint32 uintVal,		    // one Uint32
                            const unsigned long position);	// at any position

    virtual E_Condition get(Uint32 * & uintVals);
    virtual E_Condition get(Uint32 & uintVal, const unsigned long pos);

    Uint32 * get(void);
    Uint32 get(const unsigned long position);

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRUL_H

/*
** CVS/RCS Log:
** $Log: dcvrul.h,v $
** Revision 1.3  1996-01-05 13:23:11  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/


