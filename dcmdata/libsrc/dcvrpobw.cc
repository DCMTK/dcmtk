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
** Last Update:         $Author: meichel $
** Update Date:         $Date: 1998-11-12 16:48:28 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrpobw.cc,v $
** CVS/RCS Revision:    $Revision: 1.5 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrpobw.h"

DcmPolymorphOBOW::DcmPolymorphOBOW(
    const DcmTag & tag, 
    const Uint32 len)
  : DcmOtherByteOtherWord(tag, len),
    changeVR(OFFalse),
    currentVR(EVR_OW)
{
    if (Tag.getEVR() == EVR_ox) Tag.setVR(EVR_OW);
}

DcmPolymorphOBOW::DcmPolymorphOBOW(const DcmPolymorphOBOW & oldObj)
: DcmOtherByteOtherWord(oldObj)
, changeVR(oldObj.changeVR)
, currentVR(oldObj.currentVR)
{
}

DcmPolymorphOBOW::~DcmPolymorphOBOW()
{
}

DcmPolymorphOBOW &DcmPolymorphOBOW::operator=(const DcmPolymorphOBOW & obj)
{
  DcmOtherByteOtherWord::operator=(obj);
  changeVR = obj.changeVR;
  currentVR = obj.currentVR;
}

E_Condition 
DcmPolymorphOBOW::getUint8Array(
    Uint8 * & bytes)
{
    errorFlag = EC_Normal;
    OFBool bchangeVR = OFFalse;
    if (currentVR == EVR_OW) 
    {
        if (fByteOrder == EBO_BigEndian)
        {
            swapValueField(sizeof(Uint16));
            fByteOrder = EBO_LittleEndian;
        }
        if (Tag.getEVR() == EVR_OW)
        {
            bchangeVR = OFTrue;
            Tag.setVR(EVR_OB);
            currentVR = EVR_OB;
        }
    }
    bytes = (Uint8 *)this -> getValue();
    if (bchangeVR)
        Tag.setVR(EVR_OW);

    return errorFlag;
}


E_Condition 
DcmPolymorphOBOW::getUint16Array(
    Uint16 * & words)
{
    errorFlag = EC_Normal;
    OFBool bchangeVR = OFFalse;
    if (currentVR == EVR_OB) 
    {
        fByteOrder = EBO_LittleEndian;
        currentVR = EVR_OW;
        if (Tag.getEVR() == EVR_OB)
        {
            Tag.setVR(EVR_OW);
            bchangeVR = OFTrue;
        }
    }
    words = (Uint16 *)this -> getValue();
    if (bchangeVR)
        Tag.setVR(EVR_OB);
        
    return errorFlag;
}

E_Condition 
DcmPolymorphOBOW::putUint8Array(
    const Uint8 * byteValue,
    const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    currentVR = Tag.getEVR();
    if (numBytes)
    {
        if (byteValue)
        {
            errorFlag = putValue(byteValue, sizeof(Uint8)*Uint32(numBytes));
            if (errorFlag == EC_Normal)
            {
                if (Tag.getEVR() == EVR_OW && fByteOrder == EBO_BigEndian)
                    fByteOrder = EBO_LittleEndian;
                this -> alignValue();
            }
        }
        else
            errorFlag = EC_CorruptedData;
            
    }
    else
        this -> putValue(NULL, 0);

    return errorFlag;
}


E_Condition 
DcmPolymorphOBOW::putUint16Array(
    const Uint16 * wordValue,
    const unsigned long numWords)      
{
    errorFlag = EC_Normal;
    currentVR = Tag.getEVR();
    if (numWords)
    {
        if (wordValue)
        {
            errorFlag = putValue(wordValue, sizeof(Uint16)*Uint32(numWords));
            if (errorFlag == EC_Normal &&
                Tag.getEVR() == EVR_OB && fByteOrder == EBO_BigEndian)
            {
                swapValueField(sizeof(Uint16));
                fByteOrder = EBO_LittleEndian;
            }
        }
        else
            errorFlag = EC_CorruptedData;
    }
    else
        errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


E_Condition 
DcmPolymorphOBOW::read(
    DcmStream & inStream,
    const E_TransferSyntax ixfer,
    const E_GrpLenEncoding glenc,
    const Uint32 maxReadLength)
{
    E_Condition l_error = 
        DcmOtherByteOtherWord::read(inStream, ixfer, glenc, maxReadLength);

    if (fTransferState == ERW_ready)
        currentVR = Tag.getEVR();

    return l_error;
}

void 
DcmPolymorphOBOW::transferEnd()
{
    changeVR = OFFalse;
    DcmOtherByteOtherWord::transferEnd();
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

    if (fTransferState == ERW_init)
    {
        if (Tag.getEVR() == EVR_OB && oXferSyn.isImplicitVR() && 
            fByteOrder == EBO_BigEndian)
        {
            // VR is OB and it will be written as OW in LittleEndianImplicit. 
            Tag.setVR(EVR_OW);
            if (currentVR == EVR_OB)
                fByteOrder = EBO_LittleEndian;
            currentVR = EVR_OB;
            changeVR = OFTrue;
        }
        else if (Tag.getEVR() == EVR_OW && currentVR == EVR_OB)
        {
            fByteOrder = EBO_LittleEndian;
            currentVR = EVR_OW;
        }
    }
    
    errorFlag = DcmOtherByteOtherWord::write(outStream, oxfer, enctype);
    
    if (fTransferState == ERW_ready && changeVR)
    {
        // VR must be OB again. No Swapping is needed since the written
        // transfer syntax was LittleEndianImplicit and so no swapping
        // took place.
        Tag.setVR(EVR_OB);
    }
    return errorFlag;
}

/*
** CVS/RCS Log:
** $Log: dcvrpobw.cc,v $
** Revision 1.5  1998-11-12 16:48:28  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.4  1998/07/15 15:52:11  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.3  1997/09/18 07:28:11  meichel
** Name clash for "changeVR" attribute/local variable removed.
**
** Revision 1.2  1997/07/31 06:59:03  andreas
** Error correction and additonal functionality for
** DcmPolymorphOBOW to support getting and putting of Uint8 and
** Uint16 data independent of the VR.
**
** Revision 1.1  1997/07/21 07:54:06  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
**
**
*/
