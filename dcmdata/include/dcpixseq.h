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
** Update Date:		$Date: 1997-05-22 16:57:10 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcpixseq.h,v $
** CVS/RCS Revision:	$Revision: 1.8 $
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
#include "dcerror.h"

class DcmPixelItem;

class DcmPixelSequence : public DcmSequenceOfItems 
{
protected:
    virtual E_Condition makeSubObject(DcmObject * & newObject, // out
				      const DcmTag & newTag,
				      const Uint32 newLength);	// in

public:
    DcmPixelSequence(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelSequence(const DcmPixelSequence &old);
    virtual ~DcmPixelSequence();

    virtual DcmEVR ident(void) const { return EVR_pixelSQ; }
    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0);
    virtual E_Condition insert(DcmPixelItem* item,
			       unsigned long where = DCM_EndOfListIndex);
    virtual E_Condition getItem(DcmPixelItem * & item, const unsigned long num);
    virtual E_Condition remove(DcmPixelItem * & item, const unsigned long num);
    virtual E_Condition remove(DcmPixelItem* item);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType /*enctype*/);


// This methods are not sensible for a pix-sequence
    virtual E_Condition insert(DcmItem* /*item*/,
			       unsigned long /*where*/ = DCM_EndOfListIndex,
			       BOOL /*before*/ = FALSE )
    {
	return EC_IllegalCall;
    }

    virtual DcmItem*	getItem(const unsigned long /*num*/) { return NULL; }
    virtual DcmItem*	remove(const unsigned long /*num*/) { return NULL; }
    virtual DcmItem*    remove(DcmItem* /*item*/) { return NULL; }

};

#endif // DCPIXSEQ_H

/*
** CVS/RCS Log:
** $Log: dcpixseq.h,v $
** Revision 1.8  1997-05-22 16:57:10  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.7  1997/05/06 09:22:37  hewett
** Added a "before" flag to the insertion of items for compatibility with
** insertion in normal Sequences.
**
** Revision 1.6  1996/08/05 08:45:27  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.5  1996/01/29 13:38:13  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/24 09:34:55  andreas
** Support for 64 bit long
**
** Revision 1.3  1996/01/05 13:22:58  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
