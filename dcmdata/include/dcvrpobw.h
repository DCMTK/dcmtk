/*
**
** Author: Andreas Barth
**
** Kuratorium OFFIS e.V.
**
** Module: dcvrpobw.h
**
** Purpose:
** Interface of class DcmPolymorphOBOW for Tags that can change their VR
** between OB and OW (e.g. Tag PixelData, OverlayData). This class shall 
** not be used directly in applications. No identification exists.
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 07:54:00 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrpobw.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRPOBW_H
#define DCVRPOBW_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrobow.h"

class DcmPolymorphOBOW : public DcmOtherByteOtherWord
{
private:
    OFBool changeVR;

public:
    DcmPolymorphOBOW(
	const DcmTag & tag,
	const Uint32 len = 0);

    DcmPolymorphOBOW(
	const DcmPolymorphOBOW & old);

    virtual ~DcmPolymorphOBOW();

    virtual E_Condition write(
	DcmStream & outStream,
	const E_TransferSyntax oxfer,
	const E_EncodingType enctype = EET_UndefinedLength);

    virtual void transferInit();
    virtual void transferEnd();

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
** Revision 1.1  1997-07-21 07:54:00  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
**
**
*/
