/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    04.12.95 -- Unique design of Value Field
** Kuratorium OFFIS e.V.
**
** Module: dcelem.cc
**
** Purpose:
** Implementation of class DcmElement
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-11 13:11:05 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcelem.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdlib.h>

#include "dcelem.h"
#include "dcobject.h"
#include "dcdefine.h"
#include "dcdebug.h"


class DcmLoadValueType
{

    friend class DcmElement;
	
private:
    DcmStreamConstructor * fStreamConstructor;
    Uint32 fOffset;

public:
    DcmLoadValueType(DcmStreamConstructor * constructor,
		     const Uint32 offset)
    {
	fStreamConstructor = constructor;
	fOffset = offset;
    }


    DcmLoadValueType(const DcmLoadValueType & old)
    {
	if (fStreamConstructor)
	    fStreamConstructor = old.fStreamConstructor->Copy();
	fOffset = old.fOffset;
    }


    ~DcmLoadValueType(void)
    {
	if (fStreamConstructor)
	    delete fStreamConstructor;
    }
};




// 
// CLASS DcmElement
//


E_Condition DcmElement::clear(void)
{
    errorFlag = EC_Normal;
    if (fValue)
	delete[] fValue;
    fValue = NULL;

    if (fLoadValue)
	delete fLoadValue;
    fLoadValue = NULL;
    Length = 0;
    return errorFlag;
}


DcmElement::DcmElement(const DcmTag &tag, const Uint32 len)
    : DcmObject(tag, len)
{
    Bdebug((6, "dcelem:DcmElement::DcmElement(DcmTag&,len=%ld)", len ));
    debug(( 8, "Object pointer this=0x%p", this ));

    fValue = NULL;
    fLoadValue = NULL;
    fTransferredBytes = 0;
    fByteOrder = EBO_unknown;

    Edebug(());
}


// ********************************


DcmElement::DcmElement(const DcmElement & elem)
    : DcmObject(elem)
{
    Bdebug((6, "dcelem:DcmElement::DcmElement(const DcmObject&)" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    if (elem.fValue)
    {
	unsigned short pad = 0;

	// Here it is not possible to use elem.getVR() because elem is
	// constant and getVR changes the errorFlag
	// So we use the following:
	DcmTag tag(elem.getTag());
	switch(tag.getEVR())
	{
	case EVR_AE:
	case EVR_AS:
	case EVR_CS:
	case EVR_DA:
	case EVR_DS:
	case EVR_DT:
	case EVR_IS:
	case EVR_LO:
	case EVR_LT:
	case EVR_PN:
	case EVR_SH:
	case EVR_ST:
	case EVR_TM:
	case EVR_UI:
	    pad = 1;
	    break;

	default:
	    pad = 0;
	    break;
	}

	// The next lines are a special version of newValueField().
	// newValueField() cannot be used because it is virtual and it does
	// not allocate enough bytes for strings. The number of pad bytes
	// is added to the Length for this purpose.

	if (Length & 1)
	{
	    fValue = new Uint8[Length+1+pad]; // protocol error: odd value length
	    if (fValue)
		fValue[Length] = 0;
	    Length++;		// make Length even
	}
	else
	    fValue = new Uint8[Length+pad];
		
	if (!fValue)
	    errorFlag = EC_MemoryExhausted;

	if (pad && fValue)
	    fValue[Length] = 0;

	memcpy(fValue, elem.fValue, Length+pad);
    }
    else
	fValue = NULL;

    if (elem.fLoadValue)
	fLoadValue = new DcmLoadValueType(*elem.fLoadValue);
    else
	fLoadValue = NULL;

    fTransferredBytes = elem.fTransferredBytes;
    fByteOrder = elem.fByteOrder;
	
    Edebug(());
}


// ********************************


DcmElement::~DcmElement()
{
    Bdebug((6, "dcelem:DcmElement::~DcmElement()"));
    debug(( 8, "Object pointer this=0x%p", this ));

    if (fValue)
	delete[] fValue;

    if (fLoadValue)
	delete fLoadValue;

    Edebug(());
}


E_Condition DcmElement::get(Uint8 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Sint16 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Uint16 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Sint32 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Uint32 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Float32 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Float64 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(DcmTag & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(char * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Uint8 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Sint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Uint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Sint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Uint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Float32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::get(Float64 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


void * DcmElement::getValue(const E_ByteOrder newByteOrder)
{
    errorFlag =  EC_Normal;
    Uint8 * value = NULL;

    if (!fValue)
	errorFlag = this -> loadValue();

    if (errorFlag == EC_Normal)
    {
	if (newByteOrder != fByteOrder)
	{
	    this -> swapIfNecessary(newByteOrder, fByteOrder, fValue, Length, 
				    Tag->getVR().getValueWidth());
	    fByteOrder = newByteOrder;
	}
	value = fValue;
    }
    return value;
}


E_Condition DcmElement::loadAllDataIntoMemory(void)
{
    errorFlag = EC_Normal;
    if (!fValue && Length != 0)
	errorFlag = this -> loadValue();

    return errorFlag;
}


E_Condition DcmElement::loadValue(DcmStream * inStream)
{
    errorFlag = EC_Normal;
    if (Length != 0)
    {
	DcmStream * readStream = inStream;
	BOOL isStreamNew = FALSE;

	if (!readStream && fLoadValue)
	{
	    readStream = fLoadValue -> fStreamConstructor -> NewDcmStream();
	    isStreamNew = TRUE;
	    readStream -> Seek((Sint32)(fLoadValue -> fOffset));
	    delete fLoadValue;
	    fLoadValue = NULL;
	}

	if (readStream)
	{
	    errorFlag = readStream -> GetError();
	    if (errorFlag == EC_Normal && readStream -> EndOfStream())
		errorFlag = EC_EndOfStream;
	    else if (errorFlag == EC_Normal)
	    {
		if (!fValue)
		    fValue = newValueField();

		if (!fValue)
		    errorFlag = EC_MemoryExhausted;
		else
		{
		    Uint32 readLength = readStream -> Avail();
		    readLength = readLength > Length - fTransferredBytes ? 
			Length - fTransferredBytes : readLength;

		    readStream -> ReadBytes(&fValue[fTransferredBytes], 
					    readLength);

		    fTransferredBytes += readStream -> TransferredBytes();

		    if (Length == fTransferredBytes)
		    {
			postLoadValue();
			errorFlag = EC_Normal;
		    }
		    else if (readStream -> EndOfStream())
			errorFlag = EC_InvalidStream;
		    else
			errorFlag = EC_StreamNotifyClient;
		}
	    }
	    if (isStreamNew)
		delete readStream;
	}
    }
    return errorFlag;
}


Uint8 * DcmElement::newValueField(void)
{
    Uint8 * value;
    if (Length & 1)
    {
	value = new Uint8[Length+1];	// protocol error: odd value length
	if (value)
	    value[Length] = 0;
	Length++;		// make Length even
    }
    else
	value = new Uint8[Length];
 
    if (!value)
	errorFlag = EC_MemoryExhausted;

    return value;
}


void DcmElement::postLoadValue(void)
{
}


E_Condition DcmElement::changeValue(const void * value, 
				    const Uint32 position,
				    const Uint32 num)
{
    errorFlag = EC_Normal;
    if (position % num != 0 || Length % num != 0 || position > Length)
	errorFlag = EC_IllegalCall;
    else if (position == Length)
    {
	if (Length == 0)
	    errorFlag = this -> putValue(value, num);
	else
	{
	    // load value (if not loaded yet)
	    if (!fValue)
		this -> loadValue();

	    // allocate new memory for value
	    Uint8 * newValue = new Uint8[Length + num];
	    if (!newValue)
		errorFlag = EC_MemoryExhausted;

	    if (errorFlag == EC_Normal)
	    {
		// copy old value in the beginning of new value
		memcpy(newValue, fValue, Length);
		// set the extension to 0
		memzero(&newValue[Length], num);
		delete[] fValue;
		fValue = newValue;
		Length += num;
	    }
	}
    }			

    // copy value at position
    if (errorFlag == EC_Normal)
	memcpy(&fValue[position], (const Uint8 *)value, num);

    return errorFlag;
}


E_Condition DcmElement::put(const char * /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Sint16 /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Uint16 /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Sint32 /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Uint32 /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Float32 /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Float64 /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const DcmTag & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Sint16 /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Uint16 /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Sint32 /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Uint32 /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Float32 /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Float64 /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const DcmTag & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Uint8 * /*val*/, const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Sint16 * /*val*/, const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Uint16 * /*val*/, const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Sint32 * /*val*/, const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Uint32 * /*val*/, const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Float32 * /*val*/, const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::put(const Float64 * /*val*/, const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putValue(const void * newValue, 
				 const Uint32 length)
{
    errorFlag = EC_Normal;
	
    if (fValue)
	delete[] fValue;
    fValue = NULL;
   
    if (fLoadValue)
	delete fLoadValue;
    fLoadValue = NULL;

    Length = length;
    fValue = this -> newValueField();

    if (fValue)
	memcpy(fValue, newValue, length);
    else
	errorFlag = EC_MemoryExhausted;

    fByteOrder = gLocalByteOrder;
    return errorFlag;
}


E_Condition DcmElement::read(DcmStream & inStream,
			     const E_TransferSyntax ixfer,
			     const E_GrpLenEncoding /*gltype*/,
			     const Uint32 maxReadLength)
{
    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	DcmXfer inXfer(ixfer);
	fByteOrder = inXfer.getByteOrder();

	errorFlag = inStream.GetError();
	if (errorFlag == EC_Normal && inStream.EndOfStream())
	    errorFlag = EC_EndOfStream;

	else if (errorFlag == EC_Normal)
	{
	    if (fTransferState == ERW_init)
	    {
		if (Length > maxReadLength && inStream.HasRandomAccess())
		{
					
		    DcmStreamConstructor * streamConstructor = 
			inStream.NewConstructor();
		    Uint32 offset = inStream.Tell();

		    if (!fLoadValue && streamConstructor && 
			inStream.GetError() == EC_Normal)
		    {
			fLoadValue = new DcmLoadValueType(streamConstructor, offset);
			if (!fLoadValue)
			    errorFlag = EC_MemoryExhausted;
		    }
					
		    inStream.ClearError();
		    inStream.Seek((Sint32)(offset+Length));
		    errorFlag = inStream.GetError();
		}

		if (fValue)
		    delete[] fValue;
		fTransferState = ERW_inWork;
	    }
			
	    if (fTransferState == ERW_inWork &&	!fLoadValue)
		errorFlag = this -> loadValue(&inStream);

	    if (fTransferredBytes == Length || fLoadValue)
		fTransferState = ERW_ready;
	}
    }
    return errorFlag;
}


void DcmElement::transferInit(void)
{
    DcmObject::transferInit();
    fTransferredBytes = 0;
}


E_Condition DcmElement::write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType /*enctype*/,
			      const E_GrpLenEncoding /*glttype*/)
{
    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	errorFlag = outStream.GetError();
	if (errorFlag == EC_Normal)
	{
	    DcmXfer outXfer(oxfer);
	    Uint8 * value = (Uint8 *)(this -> getValue(outXfer.getByteOrder()));
	    if (fTransferState == ERW_init && 
		(errorFlag = outStream.Avail(DCM_TagInfoLength)) == EC_Normal)
	    {
		if (!value)
		    Length = 0;

		Uint32 writtenBytes = 0;
		errorFlag = this -> writeTagAndLength(outStream,
						      oxfer,
						      writtenBytes);

		if (errorFlag == EC_Normal)
		{
		    fTransferState = ERW_inWork;
		    fTransferredBytes = 0;
		}
	    }

	    if (value && fTransferState == ERW_inWork)
	    {
		Uint32 len = 
		    (Length - fTransferredBytes) <= outStream.Avail() ?
		    (Length - fTransferredBytes) : outStream.Avail();

		if (len)
		{
		    outStream.WriteBytes(&value[fTransferredBytes], len);
		    fTransferredBytes += outStream.TransferredBytes();
		    errorFlag = outStream.GetError();
		}
		else if (len != Length)
		    errorFlag = EC_StreamNotifyClient;

		if (fTransferredBytes == Length)
		    fTransferState = ERW_ready;
		else if (errorFlag == EC_Normal)
		    errorFlag = EC_StreamNotifyClient;
	    }
	}
    }
    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcelem.cc,v $
** Revision 1.6  1996-03-11 13:11:05  hewett
** Changed prototypes to allow get() and put() of char strings.
**
** Revision 1.5  1996/01/09 11:06:45  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.4  1996/01/05 14:00:24  andreas
** - add forgotten initialization for the byte order
**
** Revision 1.3  1996/01/05 13:27:36  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
