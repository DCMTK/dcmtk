/*
**
** Author: Gerd Ehlers	    01.11.94 -- Created
**         Andreas Barth    04.12.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcpxitem.h
**
** Purpose:
** Interface of class DcmPixelItem
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:43 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcpxitem.h,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCPXITEM_H
#define DCPXITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcvrobow.h"


//
// CLASS DcmPixelItem
// This is a pseudo item, that has a value with representation OB 
// and has no sub elements. Since a DcmOtherByteOtherWord is defined as a 
// Dicom structure with a value of representation OW/OB it is better to
// derive this class from DcmOtherByteOtherWord.


class DcmPixelItem : public DcmOtherByteOtherWord

{
  protected:
    virtual E_Condition writeTagAndLength(DcmStream & outStream, 
					  const E_TransferSyntax oxfer,	
					  Uint32 & writtenBytes);
  public:
    DcmPixelItem(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelItem(const DcmPixelItem &old);
    virtual ~DcmPixelItem();

    DcmPixelItem &operator=(const DcmPixelItem &obj) { DcmOtherByteOtherWord::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_pixelItem; }

};


#endif // DCPXITEM_H

/*
** CVS/RCS Log:
** $Log: dcpxitem.h,v $
** Revision 1.6  1998-11-12 16:47:43  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.5  1997/05/22 16:57:10  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.4  1996/01/29 13:38:13  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:22:58  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
