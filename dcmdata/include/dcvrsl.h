/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrsl.h
**
** Purpose:
** Interface of class DcmSignedLong
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:09 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrsl.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRSL_H
#define DCVRSL_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmSignedLong : public DcmElement 
{
  public:
    DcmSignedLong(const DcmTag &tag, const Uint32 len = 0);
    DcmSignedLong(const DcmSignedLong& old );
    virtual ~DcmSignedLong(void);

    virtual DcmEVR ident(void) const { return EVR_SL; }
    virtual void print(const int level = 0);
    virtual unsigned long getVM(void);

    virtual E_Condition put(const Sint32 * sintVal,
                            const unsigned long numSints);     // number of longs

    virtual E_Condition put(const Sint32 sintVal);		// for one long only

    virtual E_Condition put(const Sint32 sintVal,		    // one Sint32
                            const unsigned long numSint);	// at any position

	virtual E_Condition get(Sint32 * & sintVals);
	virtual E_Condition get(Sint32 & sintVal, const unsigned long pos = 0);

    Sint32 * get(void);
    Sint32 get(const unsigned long position);

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRSL_H

/*
** CVS/RCS Log:
** $Log: dcvrsl.h,v $
** Revision 1.3  1996-01-05 13:23:09  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
