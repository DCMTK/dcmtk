/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrus.h
**
** Purpose:
** Interface of class DcmUnsignedShort
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:12 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrus.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRUS_H
#define DCVRUS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmUnsignedShort : public DcmElement 
{
  public:
    DcmUnsignedShort(const DcmTag &tag, const Uint32 len = 0);
    DcmUnsignedShort(const DcmUnsignedShort& old );
    virtual ~DcmUnsignedShort(void);

    virtual DcmEVR ident(void) const { return EVR_US; }
    virtual void print(const int level = 0);
    virtual unsigned long getVM(void);

    virtual E_Condition put(const Uint16 * uintVal,
                            const unsigned long numUints);     // number of longs

    virtual E_Condition put(const Uint16 uintVal);		// for one long only

    virtual E_Condition put(const Uint16 uintVal,		    // one Uint16
                            const unsigned long position);	// at any position


	virtual E_Condition get(Uint16 & uintVal, const unsigned long pos = 0);
	virtual E_Condition get(Uint16 * & uintVals);

    Uint16 * get(void);
    Uint16 get(const unsigned long position);

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRUS_H

/*
** CVS/RCS Log:
** $Log: dcvrus.h,v $
** Revision 1.3  1996-01-05 13:23:12  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
