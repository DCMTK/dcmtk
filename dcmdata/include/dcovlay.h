/*
**
** Author: Andreas Barth
** Kuratorium OFFIS e.V.
**
** Module: dcovlay.h
**
** Purpose:
** Interface of class DcmOverlayData
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:41 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcovlay.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCOVLAY_H
#define DCOVLAY_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrpobw.h"


class DcmOverlayData : public DcmPolymorphOBOW
{
public:
    DcmOverlayData(
	const DcmTag & tag, 
	const Uint32 len = 0)
	: DcmPolymorphOBOW(tag, len) {}
    DcmOverlayData(
	const DcmOverlayData & oldObj)
	: DcmPolymorphOBOW(oldObj) {}
    virtual ~DcmOverlayData() {}

    DcmOverlayData &operator=(const DcmOverlayData &obj) { DcmPolymorphOBOW::operator=(obj); return *this; }

    virtual DcmEVR ident() const { return EVR_OverlayData; }
};

#endif

/*
** CVS/RCS Log:
** $Log: dcovlay.h,v $
** Revision 1.2  1998-11-12 16:47:41  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1997/07/21 07:54:00  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
**
** 
*/
