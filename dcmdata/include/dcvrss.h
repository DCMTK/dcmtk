/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrss.h
**
** Purpose:
** Interface of class DcmSignedShort
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-29 13:38:18 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrss.h,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRSS_H
#define DCVRSS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmSignedShort : public DcmElement 
{
public:
    DcmSignedShort(const DcmTag &tag, const Uint32 len = 0);
    DcmSignedShort(const DcmSignedShort& old );
    virtual ~DcmSignedShort(void);

    virtual DcmEVR ident(void) const { return EVR_SS; }
    virtual void print(const int level = 0);
    virtual unsigned long getVM(void);

    virtual E_Condition put(const Sint16 * sintVal,
                            const unsigned long numSints);     // number of longs

    virtual E_Condition put(const Sint16 sintVal);	  // for one long only

    virtual E_Condition put(const Sint16 sintVal,	      // one Sint16
                            const unsigned long numSint);     // at any position

    virtual E_Condition put(const char * value);  // Sint16 as Strings

    virtual E_Condition get(Sint16 & sintVal, const unsigned long pos = 0);
    virtual E_Condition get(Sint16 * & sintVals);

    Sint16 * get(void);
    Sint16 get(const unsigned long position);

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRSS_H

/*
** CVS/RCS Log:
** $Log: dcvrss.h,v $
** Revision 1.4  1996-01-29 13:38:18  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:09  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/

