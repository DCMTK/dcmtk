/*
**
** Author: Andreas Barth
**
** Kuratorium OFFIS e.V.
**
** Module: dcvrpobw.h
**
** Purpose:
** Implementation of class DcmPolymorphOBOW for Tags that can change their VR
** between OB and OW (e.g. Tag PixelData, OverlayData). This class shall 
** not be used directly in applications. No identification exists.
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 07:54:06 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrpobw.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrpobw.h"

DcmPolymorphOBOW::DcmPolymorphOBOW(
    const DcmTag & tag, 
    const Uint32 len)
    : DcmOtherByteOtherWord(tag, len)
{
    if (Tag.getEVR() == EVR_ox) Tag.setVR(EVR_OW);
    changeVR = OFFalse;
}

DcmPolymorphOBOW::DcmPolymorphOBOW(
    const DcmPolymorphOBOW & oldObj)
    : DcmOtherByteOtherWord(oldObj)
{
    changeVR = oldObj.changeVR;
}

DcmPolymorphOBOW::~DcmPolymorphOBOW()
{
}

void 
DcmPolymorphOBOW::transferEnd()
{
    changeVR = OFFalse;
    DcmOtherByteOtherWord::transferEnd();
}

E_Condition DcmPolymorphOBOW::putUint8Array(
    const Uint8 * byteValue,
    const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    if (numBytes)
    {
	if (byteValue)
	{
	    errorFlag = putValue(byteValue, sizeof(Uint8)*Uint32(numBytes));
	    this -> alignValue();
	}
	else
	    errorFlag = EC_CorruptedData;
	if (Tag.getEVR() == EVR_OW && fByteOrder == EBO_BigEndian)
	    fByteOrder = EBO_LittleEndian;
	    
    }
    else
	this -> putValue(NULL, 0);

    return errorFlag;
}


E_Condition DcmPolymorphOBOW::putUint16Array(
    const Uint16 * wordValue,
    const unsigned long numWords)      
{
    errorFlag = EC_Normal;
    if (numWords)
    {
	if (wordValue)
	    errorFlag = putValue(wordValue, sizeof(Uint16)*Uint32(numWords));
	else
	    errorFlag = EC_CorruptedData;
	if (Tag.getEVR() == EVR_OB && fByteOrder == EBO_BigEndian)
	    fByteOrder = EBO_LittleEndian;
    }
    else
	errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}



void
DcmPolymorphOBOW::transferInit()
{
    changeVR = OFFalse;
    DcmOtherByteOtherWord::transferInit();
}

E_Condition 
DcmPolymorphOBOW::write(
    DcmStream & outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype)
{
    DcmXfer oXferSyn(oxfer);

    if (fTransferState == ERW_init &&
	Tag.getEVR() == EVR_OB && oXferSyn.isImplicitVR() && 
	fByteOrder == EBO_BigEndian)
    {
	// VR is OB and it will be written as OW. Byte order of
	// value field is ob if it is in little endian transfer syntax
	Tag.setVR(EVR_OW);
	fByteOrder = EBO_LittleEndian;
	changeVR = OFTrue;
    }
    
    errorFlag = DcmOtherByteOtherWord::write(outStream, oxfer, enctype);
    
    if (fTransferState == ERW_ready && changeVR)
    {
	// VR must be OB again. We need not to swap if byte order
	// is little endian. The changing of the the byte order is not
	// needed in reality since an implicit big endian transfer syntax
	// does not exist.
	fByteOrder = EBO_LittleEndian;
	Tag.setVR(EVR_OB);
    }
    return errorFlag;
}

/*
** CVS/RCS Log:
** $Log: dcvrpobw.cc,v $
** Revision 1.1  1997-07-21 07:54:06  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
**
**
*/
