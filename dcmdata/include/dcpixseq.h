/*
**
** Author: Gerd Ehlers	    01.11.94 -- Created
**         Andreas Barth    04.12.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcpixseq.h
**
** Purpose:
** Interface of class DcmPixelSequence
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:22:58 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcpixseq.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcsequen.h"


class DcmPixelSequence : public DcmSequenceOfItems 
{
  protected:
	virtual E_Condition makeSubObject(DcmObject * & newObject,	// out
									  const DcmTag & newTag,
									  const unsigned long newLength);	// in

  public:
    DcmPixelSequence(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelSequence(const DcmPixelSequence &old);
    virtual ~DcmPixelSequence();

    virtual DcmEVR ident(void) const { return EVR_pixelSQ; }
    virtual void print(int level = 0);
};

#endif // DCPIXSEQ_H

/*
** CVS/RCS Log:
** $Log: dcpixseq.h,v $
** Revision 1.3  1996-01-05 13:22:58  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
