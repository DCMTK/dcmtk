/*
 *
 *  Copyright (C) 1994-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of class DcmOverlayData
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:48:42 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcovlay.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
** Revision 1.5  2001-06-01 15:48:42  meichel
** Updated copyright header
**
** Revision 1.4  2000/03/08 16:26:16  meichel
** Updated copyright header.
**
** Revision 1.3  1999/03/31 09:24:43  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1998/11/12 16:47:41  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1997/07/21 07:54:00  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
** 
*/
