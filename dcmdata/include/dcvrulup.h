/*
**
** Author: Gerd Ehlers      03.06.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrulup.h
**
** Purpose:
** Interface of class DcmUnsignedLongOffset
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:11 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrulup.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRULUP_H
#define DCVRULUP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcvrul.h"


class DcmUnsignedLongOffset : public DcmUnsignedLong 
{
  protected:
    DcmObject* nextRecord;

  public:
    DcmUnsignedLongOffset(const DcmTag &tag, const Uint32 len = 0);
    DcmUnsignedLongOffset(const DcmUnsignedLongOffset& old);
    virtual ~DcmUnsignedLongOffset(void);

    virtual DcmEVR ident(void) const;
    virtual void print(const int level = 0);

    virtual DcmObject*  setNextRecord(DcmObject* record);
    virtual DcmObject*  getNextRecord();
    virtual E_Condition clear();
    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRUSUP_H

/*
** CVS/RCS Log:
** $Log: dcvrulup.h,v $
** Revision 1.3  1996-01-05 13:23:11  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
