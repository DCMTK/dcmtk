/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrfd.h
**
** Purpose:
** Interface of class DcmFloatingPointDouble
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:05 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrfd.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRFD_H
#define DCVRFD_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmFloatingPointDouble : public DcmElement 
{
  public:
    DcmFloatingPointDouble(const DcmTag &tag, const Uint32 len = 0);
    DcmFloatingPointDouble( const DcmFloatingPointDouble &newFD );
    virtual ~DcmFloatingPointDouble();

    virtual DcmEVR ident() const { return EVR_FD; }
    virtual void print(const int level = 0);
    virtual unsigned long getVM(void);

    virtual E_Condition put(const Float64 * doubleVal,
                            const unsigned long numDoubles); // number of doubles

    virtual E_Condition put(const Float64 doubleval);   // for one double only

    virtual E_Condition put(const Float64 doubleval,       // one double
                            const unsigned long position); // at any position

	virtual E_Condition get(Float64 * & doubleVals);
	virtual E_Condition get(Float64 & doubleVal, const unsigned long pos = 0);
    Float64 * get(void);
    Float64 get(const unsigned long position);

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRFD_H

/*
** CVS/RCS Log:
** $Log: dcvrfd.h,v $
** Revision 1.3  1996-01-05 13:23:05  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
