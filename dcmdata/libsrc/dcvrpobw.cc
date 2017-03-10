/*
 *
 *  Copyright (C) 1997-2017, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *  class DcmPolymorphOBOW for Tags that can change their VR
 *  between OB and OW (e.g. Tag PixelData, OverlayData). This class shall
 *  not be used directly in applications. No identification exists.
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrpobw.h"


DcmPolymorphOBOW::DcmPolymorphOBOW(
    const DcmTag & tag,
    const Uint32 len)
  : DcmOtherByteOtherWord(tag, len),
    changeVR(OFFalse),
    currentVR(EVR_OW)
{
    if (getTag().getEVR() == EVR_ox || getTag().getEVR() == EVR_lt) setTagVR(EVR_OW);
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
  if (this != &obj)
  {
    DcmOtherByteOtherWord::operator=(obj);
    changeVR = obj.changeVR;
    currentVR = obj.currentVR;
  }
  return *this;
}

OFCondition DcmPolymorphOBOW::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmPolymorphOBOW &, rhs);
  }
  return EC_Normal;
}

OFCondition
DcmPolymorphOBOW::getUint8Array(
    Uint8 * & bytes)
{
    errorFlag = EC_Normal;
    OFBool bchangeVR = OFFalse;
    if (currentVR == EVR_OW)
    {
        if (getByteOrder() == EBO_BigEndian)
        {
            swapValueField(sizeof(Uint16));
            setByteOrder(EBO_LittleEndian);
        }
        if (getTag().getEVR() == EVR_OW)
        {
            bchangeVR = OFTrue;
            setTagVR(EVR_OB);
            currentVR = EVR_OB;
        }
    }
    bytes = OFstatic_cast(Uint8 *, this -> getValue());
    if (bchangeVR)
        setTagVR(EVR_OW);

    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::getUint16Array(
    Uint16 * & words)
{
    errorFlag = EC_Normal;
    OFBool bchangeVR = OFFalse;
    if (currentVR == EVR_OB)
    {
        setByteOrder(EBO_LittleEndian);
        currentVR = EVR_OW;
        if (getTag().getEVR() == EVR_OB)
        {
            setTagVR(EVR_OW);
            bchangeVR = OFTrue;
        }
    }
    words = OFstatic_cast(Uint16 *, this -> getValue());
    if (bchangeVR)
        setTagVR(EVR_OB);

    return errorFlag;
}

OFCondition
DcmPolymorphOBOW::createUint8Array(
    const Uint32 numBytes,
    Uint8 * & bytes)
{
    currentVR = EVR_OB;
    setTagVR(EVR_OB);
    errorFlag = createEmptyValue(OFstatic_cast(Uint32, sizeof(Uint8) * OFstatic_cast(size_t, numBytes)));
    setByteOrder(gLocalByteOrder);
    if (EC_Normal == errorFlag)
        bytes = OFstatic_cast(Uint8 *, this->getValue());
    else
        bytes = NULL;
    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::createUint16Array(
    const Uint32 numWords,
    Uint16 * & words)
{
    currentVR = EVR_OW;
    setTagVR(EVR_OW);
    errorFlag = createEmptyValue(OFstatic_cast(Uint32, sizeof(Uint16) * OFstatic_cast(size_t, numWords)));
    setByteOrder(gLocalByteOrder);
    if (EC_Normal == errorFlag)
        words = OFstatic_cast(Uint16 *, this->getValue());
    else
        words = NULL;
    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::putUint8Array(
    const Uint8 * byteValue,
    const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    currentVR = getTag().getEVR();
    if (numBytes)
    {
        if (byteValue)
        {
            errorFlag = putValue(byteValue, OFstatic_cast(Uint32, sizeof(Uint8) * OFstatic_cast(size_t, numBytes)));
            if (errorFlag == EC_Normal)
            {
                if (getTag().getEVR() == EVR_OW && getByteOrder() == EBO_BigEndian)
                    setByteOrder(EBO_LittleEndian);
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


OFCondition
DcmPolymorphOBOW::putUint16Array(
    const Uint16 * wordValue,
    const unsigned long numWords)
{
    errorFlag = EC_Normal;
    currentVR = getTag().getEVR();
    if (numWords)
    {
        if (wordValue)
        {
            errorFlag = putValue(wordValue, OFstatic_cast(Uint32, sizeof(Uint16) * OFstatic_cast(size_t, numWords)));
            if (errorFlag == EC_Normal &&
                getTag().getEVR() == EVR_OB && getByteOrder() == EBO_BigEndian)
            {
                swapValueField(sizeof(Uint16));
                setByteOrder(EBO_LittleEndian);
            }
        }
        else
            errorFlag = EC_CorruptedData;
    }
    else
        errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::read(
    DcmInputStream & inStream,
    const E_TransferSyntax ixfer,
    const E_GrpLenEncoding glenc,
    const Uint32 maxReadLength)
{
    OFCondition l_error =
        DcmOtherByteOtherWord::read(inStream, ixfer, glenc, maxReadLength);

    if (getTransferState() == ERW_ready)
        currentVR = getTag().getEVR();

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

OFCondition DcmPolymorphOBOW::write(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
    DcmXfer oXferSyn(oxfer);
    if (getTransferState() == ERW_init)
    {
        if (getTag().getEVR() == EVR_OB && oXferSyn.isImplicitVR())
        {
          // This element was read or created as OB, but we are writing in
          // implicit VR transfer syntax (which always uses OW). Therefore,
          // change the VR associated with the tag to OW.
          setTagVR(EVR_OW);

          // If the data is currently in OB representation in memory,
          // adjust the VR to OW and update the current byte order.
          // OB data is equivalent to OW data in little endian byte order.
          if (currentVR == EVR_OB)
          {
            setByteOrder(EBO_LittleEndian);
            currentVR = EVR_OW;
          }

          // remember that we have changed the VR associated with the tag
          changeVR = OFTrue;
        }

        else if (getTag().getEVR() == EVR_OW && currentVR == EVR_OB)
        {
            // the element was originally read/created as OW
            // but is currently in OB format. Change back to OW.

            // OB data is equivalent to OW data in little endian byte order.
            setByteOrder(EBO_LittleEndian);
            currentVR = EVR_OW;
        }
    }
    errorFlag = DcmOtherByteOtherWord::write(outStream, oxfer, enctype, wcache);
    if (getTransferState() == ERW_ready && changeVR)
    {
        // Change the VR associated with the tag
        // (not the current VR!) back from OW to OB
        setTagVR(EVR_OB);
    }
    return errorFlag;
}

OFCondition DcmPolymorphOBOW::writeSignatureFormat(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
    DcmXfer oXferSyn(oxfer);
    if (getTransferState() == ERW_init)
    {
        if (getTag().getEVR() == EVR_OB && oXferSyn.isImplicitVR())
        {
          // This element was read or created as OB, but we are writing in
          // implicit VR transfer syntax (which always uses OW). Therefore,
          // change the VR associated with the tag to OW.
          setTagVR(EVR_OW);

          // If the data is currently in OB representation in memory,
          // adjust the VR to OW and update the current byte order.
          // OB data is equivalent to OW data in little endian byte order.
          if (currentVR == EVR_OB)
          {
            setByteOrder(EBO_LittleEndian);
            currentVR = EVR_OW;
          }

          // remember that we have changed the VR associated with the tag
          changeVR = OFTrue;
        }

        else if (getTag().getEVR() == EVR_OW && currentVR == EVR_OB)
        {
            // the element was originally read/created as OW
            // but is currently in OB format. Change back to OW.

            // OB data is equivalent to OW data in little endian byte order.
            setByteOrder(EBO_LittleEndian);
            currentVR = EVR_OW;
        }
    }
    errorFlag = DcmOtherByteOtherWord::writeSignatureFormat(outStream, oxfer, enctype, wcache);
    if (getTransferState() == ERW_ready && changeVR)
    {
        // Change the VR associated with the tag
        // (not the current VR!) back from OW to OB
        setTagVR(EVR_OB);
    }
    return errorFlag;
}
