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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmElement
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-16 15:55:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcelem.cc,v $
 *  CVS/RCS Revision: $Revision: 1.37 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <string.h>

#include "dcelem.h"
#include "dcobject.h"
#include "dcdefine.h"
#include "dcswap.h"
#include "dcdebug.h"


class DcmLoadValueType
{

    friend class DcmElement;
        
private:
    DcmStreamConstructor * fStreamConstructor;
    Uint32 fOffset;

 // --- declarations to avoid compiler warnings
 
    DcmLoadValueType &operator=(const DcmLoadValueType &);

public:
    DcmLoadValueType(DcmStreamConstructor * constructor,
                     const Uint32 offset)
      : fStreamConstructor(constructor),
        fOffset(offset)
    {
    }


    DcmLoadValueType(const DcmLoadValueType & old)
      : fStreamConstructor(NULL),
        fOffset(old.fOffset)
    {
        if (old.fStreamConstructor)
            fStreamConstructor = old.fStreamConstructor->Copy();
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


OFCondition DcmElement::clear(void)
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
  : DcmObject(tag, len),
    fLoadValue(NULL),
    fValue(NULL),
    fByteOrder(gLocalByteOrder)
{
}


// ********************************


DcmElement::DcmElement(const DcmElement & elem)
  : DcmObject(elem),
    fLoadValue(NULL),
    fValue(NULL),
    fByteOrder(elem.fByteOrder)
{
    if (elem.fValue)
    {
        unsigned short pad = 0;

        DcmVR vr(elem.getVR());
        if (vr.isaString()) {
            pad = 1;
        } else {
            pad = 0;
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
            Length++;           // make Length even
        }
        else
            fValue = new Uint8[Length+pad];
                
        if (!fValue)
            errorFlag = EC_MemoryExhausted;

        if (pad && fValue)
            fValue[Length] = 0;

        memcpy(fValue, elem.fValue, size_t(Length+pad));
    }

    if (elem.fLoadValue)
        fLoadValue = new DcmLoadValueType(*elem.fLoadValue);
}


// ********************************

DcmElement &DcmElement::operator=(const DcmElement &obj)
{
    DcmObject::operator=(obj);
    fLoadValue = NULL;
    fValue = NULL;
    fByteOrder = obj.fByteOrder;

    if (obj.fValue)
    {
        unsigned short pad = 0;

        DcmVR vr(obj.getVR());
        if (vr.isaString()) {
            pad = 1;
        } else {
            pad = 0;
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
            Length++;           // make Length even
        }
        else
            fValue = new Uint8[Length+pad];
                
        if (!fValue)
            errorFlag = EC_MemoryExhausted;

        if (pad && fValue)
            fValue[Length] = 0;

        memcpy(fValue, obj.fValue, size_t(Length+pad));
    }

    if (obj.fLoadValue)
        fLoadValue = new DcmLoadValueType(*obj.fLoadValue);

    return *this;
}

// ********************************


DcmElement::~DcmElement()
{
    if (fValue)
        delete[] fValue;

    if (fLoadValue)
        delete fLoadValue;
}

Uint32 DcmElement::calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    DcmXfer xferSyn(xfer);
    return getLength(xfer, enctype) + xferSyn.sizeofTagHeader(getVR());
}


OFBool DcmElement::canWriteXfer(const E_TransferSyntax newXfer,
                              const E_TransferSyntax /*oldXfer*/)
{
    return newXfer != EXS_Unknown;
}

OFCondition DcmElement::detachValueField(OFBool copy)
{
    OFCondition l_error = EC_Normal;
    if (Length != 0)
    {
        if (copy)
        {
            if (!fValue)
                l_error = loadValue();
            Uint8 * newValue = new Uint8[Length];
            memcpy(newValue, fValue, size_t(Length));
            fValue = newValue;
        }
        else
        {
            fValue = NULL;
            Length = 0;
        }
    }
    return l_error;
}

OFCondition DcmElement::getUint8(Uint8 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint16(Sint16 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint16(Uint16 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint32(Sint32 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint32(Uint32 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat32(Float32 & /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat64(Float64 & /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getTagVal(DcmTagKey & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition 
DcmElement::getOFString(
    OFString &/*val*/,
    const unsigned long /*pos*/,
    OFBool /*normalize*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getString(char * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition 
DcmElement::getOFStringArray(
    OFString & /*val*/,
    OFBool /*normalize*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint8Array(Uint8 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint16Array(Sint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint16Array(Uint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint32Array(Sint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint32Array(Uint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat32Array(Float32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat64Array(Float64 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


void * DcmElement::getValue(const E_ByteOrder newByteOrder)
    /*
     * This function returns this element's value. The returned value corresponds
     * to the byte ordering (little or big endian) that was passed.
     *
     * Parameters:
     *   newByteOrder - [in] The byte ordering that shall be accounted for (little or big endian).
     */
{
    /* initialize return value */
    Uint8 * value = NULL;

    /* if the byte ordering is unknown, this is an illegal call */
    if (newByteOrder == EBO_unknown)
        errorFlag = EC_IllegalCall;
    else
    {
        /* in case this call is not illegal, we need to do something. First of all, set the error flag to ok */
        errorFlag =  EC_Normal;

        /* do something only if the length of this element's value does not equal (i.e. is greater than) 0 */
        if (Length != 0)
        {
            /* if the value has not yet been loaded, do so now */
            if (!fValue)
                errorFlag = this -> loadValue();

            /* íf everything is ok */
            if (errorFlag == EC_Normal)
            {
                /* if this element's value's byte ordering does not correspond to the */
                /* desired byte ordering, we need to rearrange this value's bytes and */
                /* set its byte order indicator variable correspondingly */
                if (newByteOrder != fByteOrder)
                {
                    swapIfNecessary(newByteOrder, fByteOrder, fValue, 
                                    Length, Tag.getVR().getValueWidth());
                    fByteOrder = newByteOrder;
                }

                /* if everything is ok, assign the current value to the result variable */
                if (errorFlag == EC_Normal)
                    value = fValue;
            }
        }
    }

    /* return result */
    return value;
}


OFCondition DcmElement::loadAllDataIntoMemory(void)
{
    errorFlag = EC_Normal;
    if (!fValue && Length != 0)
        errorFlag = this -> loadValue();

    return errorFlag;
}


OFCondition DcmElement::loadValue(DcmStream * inStream)
    /*
     * This function reads the data value of an attribute and stores the information which was
     * read in this. The information is either read from the inStream or (if inStream is NULL)
     * from a different stream which was created earlier and which is accessible through the
     * fLoadValue member variable. Note that if not all information for an attribute could be
     * read from the stream, the function returns EC_StreamNotifyClient.
     *
     * Parameters:
     *   inStream      - [in] The stream which contains the information.
     */
{
    /* initiailze return value */
    errorFlag = EC_Normal;

    /* only if the length of this element does not equal 0, read information */
    if (Length != 0)
    {
        DcmStream * readStream = inStream;
        OFBool isStreamNew = OFFalse;

        /* if the NULL pointer was passed and the fLoadValue contains a non NULL pointer */
        if (!readStream && fLoadValue)
        {
            /* we need to read information from the stream which is */
            /* accessible through fLoadValue. Hence, reassign readStream */
            readStream = fLoadValue -> fStreamConstructor -> NewDcmStream();

            /* set isStreamNew to OFTrue */
            isStreamNew = OFTrue;

            /*set readStream to the position where we are supposed to start reading */
            readStream -> Seek((Sint32)(fLoadValue -> fOffset));

            /* and delete the fLoadValue object which is superfluous now */
            delete fLoadValue;
            fLoadValue = NULL;
        }

        /* if we have a stream from which we can read */
        if (readStream)
        {
            /* check if the stream reported an error */
            errorFlag = readStream -> GetError();

            /* if we encountered the end of the stream, set the error flag correspondingly */
            if (errorFlag == EC_Normal && readStream -> EndOfStream())
                errorFlag = EC_EndOfStream;
            /* if we did not encounter the end of the stream and no error occured so far, go ahead */
            else if (errorFlag == EC_Normal)
            {
                /* if the object which holds this element's value does not yet exist, create it */
                if (!fValue)
                    fValue = newValueField();

                /* if there is still no such object, the memory must be exhausted */
                if (!fValue)
                    errorFlag = EC_MemoryExhausted;
                /* else (i.e. we have an object which can be used to capture this element's */
                /* value) we need to read a certain amount of bytes from the stream */
                else
                {
                    /* determine how many bytes are available in the input stream */
                    Uint32 readLength = readStream -> Avail();

                    /* determine how many bytes shall be read from the stream: If the amount */
                    /* of bytes that are available is greater than the amount of bytes that we */
                    /* have to read to complete this element's value, read only the amount of */
                    /* bytes that we have to read to complete this element's value; else read */
                    /* all the bytes that are available. */
                    readLength = readLength > Length - fTransferredBytes ? 
                        Length - fTransferredBytes : readLength;

                    /* read a corresponding amount of bytes from the stream, store the information in fvalue */
                    readStream -> ReadBytes(&fValue[fTransferredBytes], 
                                            readLength);

                    /* increase the counter that counts how many bytes were read */
                    fTransferredBytes += readStream -> TransferredBytes();

                    /* if we have read all the bytes which make up this element's value */
                    if (Length == fTransferredBytes)
                    {
                        /* call a function which performs certain operations on the information which was read */
                        postLoadValue();
                        errorFlag = EC_Normal;
                    }
                    /* else set the return value correspondingly */
                    else if (readStream -> EndOfStream())
                        errorFlag = EC_InvalidStream;
                    else
                        errorFlag = EC_StreamNotifyClient;
                }
            }

            /* if we created the stream from which information was read in this */
            /* function, we need to we need to delete this object here as well */
            if (isStreamNew)
                delete readStream;
        }
    }

    /* return result value */
    return errorFlag;
}


Uint8 * DcmElement::newValueField(void)
    /*
     * This function creates a byte array of Length bytes and returns this array.
     * In case Length is odd, an array of Length+1 bytes will be created and Length
     * will be increased by 1.
     *
     * Parameters:
     *     none.
     */
{
    Uint8 * value;

    /* if this element's lenght is odd */
    if (Length & 1)
    {
        /* create an array of Length+1 bytes */
        value = new Uint8[Length+1];    // protocol error: odd value length

        /* if creation was successful, set last byte to 0 (in order to initialize this byte) */
        /* (no value will be assigned to this byte later, since Length was odd) */
        if (value)
            value[Length] = 0;

        /* make Length even */
        Length++;
    }
    /* if this element's length is even, create a corresponding array of Lenght bytes */
    else
        value = new Uint8[Length];
 
    /* if creation was not successful set member error flag correspondingly */
    if (!value)
        errorFlag = EC_MemoryExhausted;

    /* return byte array */
    return value;
}


void DcmElement::postLoadValue(void)
{
}


OFCondition DcmElement::changeValue(const void * value, 
                                    const Uint32 position,
                                    const Uint32 num)
{
    OFBool done = OFFalse;
    errorFlag = EC_Normal;
    if (position % num != 0 || Length % num != 0 || position > Length)
        errorFlag = EC_IllegalCall;
    else if (position == Length)
    {
        if (Length == 0)
        {
            errorFlag = this -> putValue(value, num);
            done = OFTrue;
        }
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
                // swap to local byte order 
                swapIfNecessary(gLocalByteOrder, fByteOrder, fValue, 
                                Length, Tag.getVR().getValueWidth());
                fByteOrder = gLocalByteOrder;
                // copy old value in the beginning of new value
                memcpy(newValue, fValue, size_t(Length));
                // set parameter value in the extension
                memcpy(&newValue[Length], (const Uint8*)value, size_t(num));
                delete[] fValue;
                fValue = newValue;
                Length += num;
            }
            done = OFTrue;
        }
    }                   

    // copy value at position
    if (!done && errorFlag == EC_Normal)
    {
        // swap to local byte order
        swapIfNecessary(gLocalByteOrder, fByteOrder, fValue, 
                        Length, Tag.getVR().getValueWidth());
        memcpy(&fValue[position], (const Uint8 *)value, size_t(num));
        fByteOrder = gLocalByteOrder;
    }

    return errorFlag;
}

OFCondition DcmElement::putOFStringArray(const OFString& /* stringValue*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}

OFCondition DcmElement::putString(const char * /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint16(const Sint16 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint16(const Uint16 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint32(const Sint32 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint32(const Uint32 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat32(const Float32 /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat64(const Float64 /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putTagVal(const DcmTagKey & /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint8Array(const Uint8 * /*val*/, 
                                      const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint16Array(const Sint16 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint16Array(const Uint16 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint32Array(const Sint32 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint32Array(const Uint32 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat32Array(const Float32 * /*val*/, 
                                        const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat64Array(const Float64 * /*val*/, 
                                        const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putValue(const void * newValue, 
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

    if (length != 0)
    {
        fValue = this -> newValueField();

        if (fValue)
            memcpy(fValue, newValue, size_t(length));
        else
            errorFlag = EC_MemoryExhausted;
    }
    fByteOrder = gLocalByteOrder;
    return errorFlag;
}

OFCondition DcmElement::createEmptyValue(const Uint32 length)
{
    errorFlag = EC_Normal;        
    if (fValue) delete[] fValue;
    fValue = NULL;
    if (fLoadValue) delete fLoadValue;
    fLoadValue = NULL;
    Length = length;
                
    if (length != 0)
    {
      fValue = newValueField();
      if (fValue) memzero(fValue, size_t(length));
      else errorFlag = EC_MemoryExhausted;
    }

    fByteOrder = gLocalByteOrder;
    return errorFlag;
}

OFCondition DcmElement::read(DcmStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding /*glenc*/,
                             const Uint32 maxReadLength)
    /*
     * This function reads the data value of an attribute which is captured in the input
     * stream and captures this information in this. If not all information for an attribute
     * could be read from the stream, the function returns EC_StreamNotifyClient. Note that
     * if certain conditions are met, this function does not actually load the data value
     * but creates and stores an object that enables us to load this information later.
     *
     * Parameters:
     *   inStream      - [in] The stream which contains the information.
     *   ixfer         - [in] The transfer syntax which was used to encode the information in inStream.
     *   glenc         - [in] [optional parameter, default = EGL_noChange]. Encoding type for group length.
     *                        Specifies what will be done with group length tags.
     *   maxReadLength - [in] [optional parameter, default = DCM_MaxReadLength]. 
     */
{
    /* if this element's transfer state shows ERW_notInitialized, this is an illegal call */
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, go ahead and create a DcmXfer */
        /* object based on the transfer syntax which was passed */
        DcmXfer inXfer(ixfer);

        /* determine the transfer syntax's byte ordering */
        fByteOrder = inXfer.getByteOrder();

        /* check if the stream reported an error */
        errorFlag = inStream.GetError();

        /* if we encountered the end of the stream, set the error flag correspondingly */
        if (errorFlag == EC_Normal && inStream.EndOfStream())
            errorFlag = EC_EndOfStream;
        /* if we did not encounter the end of the stream and no error occured so far, go ahead */
        else if (errorFlag == EC_Normal)
        {
            /* if the transfer state is ERW_init, we need to prepare */
            /* the reading of this element's value from the stream */
            if (fTransferState == ERW_init)
            {
                /* if the Length of this element's value is greater than the amount of bytes we */
                /* can read from the stream and if the stream has random access, we want to create */
                /* a DcmStreamConstructor object that enables us to read this element's value later. */
                /* This new object will be stored (together with the position where we have to start */
                /* reading the value) in the member variable fLoadValue. */
                if (Length > maxReadLength && inStream.HasRandomAccess())
                {
                    /* create a stream constructor to read the value later */
                    DcmStreamConstructor * streamConstructor = 
                        inStream.NewConstructor();

                    /* get the current position in the stream where we are supposed to read information */
                    Uint32 offset = inStream.Tell();

                    /* if there is no DcmLoadValueType object yet, create one. This object */
                    /* contains all the information that enables us to read the value later */
                    if (!fLoadValue && streamConstructor && 
                        inStream.GetError() == EC_Normal)
                    {
                        fLoadValue = new DcmLoadValueType(streamConstructor, offset);
                        if (!fLoadValue)
                            errorFlag = EC_MemoryExhausted;
                    }

                    /* check if there are enough bytes available in the stream */
                    inStream.ClearError();
                    inStream.Seek((Sint32)(offset+Length));
                    errorFlag = inStream.GetError();
                    if (errorFlag != EC_Normal)
                    {
                        /* Print an error message when too few bytes are available in the file in order to
                         * distinguish this problem from any other generic "InvalidStream" problem. */
                         ofConsole.lockCerr() << "ERROR: " << Tag.getTagName() << Tag.getXTag() << " larger ("
                             << Length << ") that remaining bytes in file" << endl;
                         ofConsole.unlockCerr();
                    }
                }

                /* if there is already a value for this element, delete this value */
                if (fValue)
                    delete[] fValue;

                /* set the transfer state to ERW_inWork */
                fTransferState = ERW_inWork;
            }

            /* if the transfer state is ERW_inWork and we are not supposed */
            /* to read this element's value later, read the value now */
            if (fTransferState == ERW_inWork && !fLoadValue)
                errorFlag = this -> loadValue(&inStream);

            /* if the amount of transferred bytes equals the Length of this element */
            /* or the object which contains information to read the value of this */
            /* element later is existent, set the transfer state to ERW_ready */
            if (fTransferredBytes == Length || fLoadValue)
                fTransferState = ERW_ready;
        }
    }

    /* return result value */
    return errorFlag;
}


void DcmElement::swapValueField(
    size_t valueWidth)
{
    if (Length != 0)
    {
        if (!fValue)
            errorFlag = this -> loadValue();
        
        if (errorFlag == EC_Normal)
            swapBytes(fValue, Length, valueWidth);

    }
}

void DcmElement::transferInit(void)
{
    DcmObject::transferInit();
    fTransferredBytes = 0;
}


OFCondition DcmElement::write(DcmStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType /*enctype*/)
    /*
     * This function writes this element's value to the outstream which was passed. When writing information,
     * the byte ordering (little or big endian) of the transfer syntax which was passed will be accounted for.
     * In case the outstream does not provide enough space for all bytes of the current element's value, only
     * a certain part of the value will be written to the stream. This element's transfer state indicates if
     * the all bytes of value have already been written to the stream (ERW_ready), if the writing is still in
     * progress and more bytes need to be written to the stream (ERW_inWork) or if the writing of the bytes
     * of this element's value has not even begun yet (ERW_init). The member variable fTransferredBytes indicates
     * how many bytes (starting from byte 0) of this element's value have already been written to the stream.
     * This function will return EC_Normal, if the entire value of this element has been written to the stream, 
     * it will return EC_StreamNotifyClient, if there is no more space in the buffer and _not_ all bytes of this
     * element's value have been written, and it will return some other (error) value if there was an error.
     *
     * Parameters:
     *   outStream - [out] The stream that the information will be written to.
     *   oxfer     - [in] The transfer syntax which shall be used.
     */
{
  /* In case the transfer state is not initialized, this is an illegal call */
  if (fTransferState == ERW_notInitialized) errorFlag = EC_IllegalCall;
  else
  {
    /* if this is not an illegal call, we need to do something. First */
    /* of all, check the error state of the stream that was passed */
    errorFlag = outStream.GetError();

    /* only do something if the error state of the stream is ok */
    if (errorFlag == EC_Normal)
    {
      /* create an object that represents the transfer syntax */
      DcmXfer outXfer(oxfer);

      /* get this element's value. Mind the byte ordering (little */
      /* or big endian) of the transfer syntax which shall be used */
      Uint8 * value = (Uint8 *)(this -> getValue(outXfer.getByteOrder()));

      /* if this element's transfer state is ERW_init (i.e. it has not yet been written to */
      /* the stream) and if the outstream provides enough space for tag and length information */
      /* write tag and length information to it, do something */
      if (fTransferState == ERW_init && (errorFlag = outStream.Avail(DCM_TagInfoLength)) == EC_Normal)
      {
          /* if there is no value, Length (member variable) shall be set to 0 */
          if (!value) Length = 0;

          /* remember how many bytes have been written to the stream, currently none so far */
          Uint32 writtenBytes = 0;

          /* write tag and length information (and possibly also data type information) to the stream, */
          /* mind the transfer syntax and remember the amount of bytes that have been written */
          errorFlag = this -> writeTagAndLength(outStream, oxfer, writtenBytes);

          /* if the writing was successful, set this element's transfer */
          /* state to ERW_inWork and the amount of transferred bytes to 0 */
          if (errorFlag == EC_Normal)
          {
              fTransferState = ERW_inWork;
              fTransferredBytes = 0;
          }
      }

      /* if there is a value that has to be written to the stream */
      /* and if this element's transfer state is ERW_inWork */
      if (value && fTransferState == ERW_inWork)
      {
          /* figure out how many bytes of the current element's value shall be written to */
          /* the stream if there the total length of the value minus the amount of bytes */
          /* which have already been transferred is lower than or equal to the amount of */
          /* bytes which are available in the stream, go ahead and transfer all the remaining */
          /* bytes of the element's value. If the above mentioned condition is not met, write */
          /* only as many bytes of the element's value to the stream as are available in the stream. */
          Uint32 len = (Length - fTransferredBytes) <= outStream.Avail() ? (Length - fTransferredBytes) : outStream.Avail();

          /* if we figured out that we still can write value information to the stream, do so */
          if (len)
          {
            /* write len amount of bytes to the stream starting at value[fTransferredBytes] */
            /* (note that the bytes value[0] to value[fTransferredBytes-1] have already been */
            /* written to the stream) */
            outStream.WriteBytes(&value[fTransferredBytes], len);

            /* increase the amount of bytes which have been transfered correspondingly */
            fTransferredBytes += outStream.TransferredBytes();

            /* see if there is something fishy with the stream */
            errorFlag = outStream.GetError();
          }
          /* if we figured that no value information could be written to the stream, check if this */
          /* is because there is no value information (Length==0) or because there was no more space */
          /* in the stream. If the latter condition is met, set a corresponding return value. */
          else if (len != Length) errorFlag = EC_StreamNotifyClient;
          
          /* if the amount of transferred bytes equals the length of the element's value, the */
          /* entire value has been written to the stream. Thus, this element's transfer state */
          /* has to be set to ERW_ready. If this is not the case but the error flag still shows */
          /* an ok value, there was no more space in the stream and a corresponding return value */
          /* has to be set. (Isn't the "else if" part superfluous?!?) */
          if (fTransferredBytes == Length) fTransferState = ERW_ready;
          else if (errorFlag == EC_Normal) errorFlag = EC_StreamNotifyClient;
      }
    }
  }

    /* return result value */
  return errorFlag;
}


OFCondition DcmElement::writeSignatureFormat(DcmStream & outStream,
                                       const E_TransferSyntax oxfer,
                                       const E_EncodingType enctype)
{
  // for normal DICOM elements (everything except sequences), the data 
  // stream used for digital signature creation or verification is 
  // identical to the stream used for network communication or media 
  // storage.
  return write(outStream, oxfer, enctype);
}

// ********************************


/*
** CVS/RCS Log:
** $Log: dcelem.cc,v $
** Revision 1.37  2001-11-16 15:55:02  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.36  2001/11/01 14:55:36  wilkens
** Added lots of comments.
**
** Revision 1.35  2001/09/25 17:19:49  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.34  2001/06/01 15:49:03  meichel
** Updated copyright header
**
** Revision 1.33  2001/05/10 12:50:23  meichel
** Added protected createEmptyValue() method in class DcmElement.
**
** Revision 1.32  2000/11/07 16:56:19  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.31  2000/04/14 15:55:04  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.30  2000/03/08 16:26:34  meichel
** Updated copyright header.
**
** Revision 1.29  2000/03/03 14:05:32  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.28  2000/02/23 15:11:51  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.27  2000/02/01 10:12:06  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.26  1999/03/31 09:25:26  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.25  1999/03/22 15:46:24  meichel
** Printing explicit error message when DICOM file is too short.
**
** Revision 1.24  1998/11/12 16:48:15  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.23  1998/07/15 15:51:55  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.22  1998/01/14 15:22:35  hewett
** Replaced a switch construct to use to the isaString method.
**
** Revision 1.21  1997/09/11 15:24:39  hewett
** Added a putOFStringArray method.
**
** Revision 1.20  1997/08/29 08:32:54  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.19  1997/07/31 06:58:04  andreas
** new protected method swapValueField for DcmElement
**
** Revision 1.18  1997/07/24 13:10:51  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.17  1997/07/21 07:57:57  andreas
** - New method DcmElement::detachValueField to give control over the
**   value field to the calling part (see dcelem.h)
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.16  1997/07/07 07:46:19  andreas
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.15  1997/07/03 15:09:57  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.14  1997/05/27 13:48:58  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.13  1997/05/16 08:23:53  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.12  1997/05/15 12:29:02  andreas
** - Bug fix for changing binary element values. If a binary existing element
**   value changed, byte order was somtimes wrong.
**
** Revision 1.11  1997/04/18 08:17:16  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.10  1996/07/31 13:41:23  andreas
** *** empty log message ***
**
** Revision 1.9  1996/07/31 13:26:01  andreas
** -  Minor corrections: error code for swapping to or from byteorder unknown
**                       correct read of dataset in fileformat
**
** Revision 1.8  1996/07/29 17:14:26  andreas
** Faster Access with empty value fields
**
** Revision 1.7  1996/04/16 16:04:05  andreas
** - new put parameter DcmTagKey for DcmAttributeTag elements
** - better support for NULL element value
**
** Revision 1.6  1996/03/11 13:11:05  hewett
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
*/
