/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Purpose:
 *  Interface of class DcmPolymorphOBOW for Tags that can change their VR
 *  between OB and OW (e.g. Tag PixelData, OverlayData). This class shall 
 *  not be used directly in applications. No identification exists.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:05 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrpobw.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRPOBW_H
#define DCVRPOBW_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrobow.h"

class DcmPolymorphOBOW : public DcmOtherByteOtherWord
{
private:
    OFBool changeVR;
    DcmEVR currentVR;    // current VR of value field (can change)

public:
    DcmPolymorphOBOW(
	const DcmTag & tag,
	const Uint32 len = 0);

    DcmPolymorphOBOW(
	const DcmPolymorphOBOW & old);

    virtual ~DcmPolymorphOBOW();

    DcmPolymorphOBOW &operator=(const DcmPolymorphOBOW &obj);

    virtual E_Condition read(
	DcmStream & inStream,
	const E_TransferSyntax ixfer,
	const E_GrpLenEncoding glenc,
	const Uint32 maxReadLength);

    virtual E_Condition write(
	DcmStream & outStream,
	const E_TransferSyntax oxfer,
	const E_EncodingType enctype = EET_UndefinedLength);

    virtual void transferInit();
    virtual void transferEnd();

    // get data as Uint8 Array
    virtual E_Condition getUint8Array(
	Uint8 * & bytes);

    // get data as Uint16 Array
    virtual E_Condition getUint16Array(
	Uint16 * & words);

    // put an Unit8 array. It is converted to OW if VR == OW
    virtual E_Condition putUint8Array(
	const Uint8 * byteValue,
	const unsigned long length);    

    // put an Unit8 array. It is converted to OB if VR == OB
    virtual E_Condition putUint16Array(
	const Uint16 * wordValue,
	const unsigned long length ); 

};
#endif

/*
** CVS/RCS Log:
** $Log: dcvrpobw.h,v $
** Revision 1.4  1999-03-31 09:25:05  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.3  1998/11/12 16:47:53  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.2  1997/07/31 06:59:00  andreas
** Error correction and additonal functionality for
** DcmPolymorphOBOW to support getting and putting of Uint8 and
** Uint16 data independent of the VR.
**
** Revision 1.1  1997/07/21 07:54:00  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
**
*/
