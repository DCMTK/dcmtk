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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmElement
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:05:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcelem.cc,v $
 *  CVS/RCS Revision: $Revision: 1.29 $
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

E_Condition DcmElement::detachValueField(OFBool copy)
{
    E_Condition l_error = EC_Normal;
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

E_Condition DcmElement::getUint8(Uint8 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getSint16(Sint16 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getUint16(Uint16 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getSint32(Sint32 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getUint32(Uint32 & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getFloat32(Float32 & /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getFloat64(Float64 & /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getTagVal(DcmTagKey & /*val*/, const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition 
DcmElement::getOFString(
    OFString &/*val*/,
    const unsigned long /*pos*/,
    OFBool /*normalize*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getString(char * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition 
DcmElement::getOFStringArray(
    OFString & /*val*/,
    OFBool /*normalize*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getUint8Array(Uint8 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getSint16Array(Sint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getUint16Array(Uint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getSint32Array(Sint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getUint32Array(Uint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getFloat32Array(Float32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::getFloat64Array(Float64 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


void * DcmElement::getValue(const E_ByteOrder newByteOrder)
{
    Uint8 * value = NULL;
    if (newByteOrder == EBO_unknown)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag =  EC_Normal;

        if (Length != 0)
        {
            if (!fValue)
                errorFlag = this -> loadValue();

            if (errorFlag == EC_Normal)
            {
                if (newByteOrder != fByteOrder)
                {
                    swapIfNecessary(newByteOrder, fByteOrder, fValue, 
                                    Length, Tag.getVR().getValueWidth());
                    fByteOrder = newByteOrder;
                }

                if (errorFlag == EC_Normal)
                    value = fValue;
            }
        }
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
        OFBool isStreamNew = OFFalse;

        if (!readStream && fLoadValue)
        {
            readStream = fLoadValue -> fStreamConstructor -> NewDcmStream();
            isStreamNew = OFTrue;
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
        value = new Uint8[Length+1];    // protocol error: odd value length
        if (value)
            value[Length] = 0;
        Length++;               // make Length even
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

E_Condition DcmElement::putOFStringArray(const OFString& /* stringValue*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}

E_Condition DcmElement::putString(const char * /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putSint16(const Sint16 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putUint16(const Uint16 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putSint32(const Sint32 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putUint32(const Uint32 /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putFloat32(const Float32 /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putFloat64(const Float64 /*val*/, 
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putTagVal(const DcmTagKey & /*val*/, 
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putUint8Array(const Uint8 * /*val*/, 
                                      const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putSint16Array(const Sint16 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putUint16Array(const Uint16 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putSint32Array(const Sint32 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putUint32Array(const Uint32 * /*val*/, 
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putFloat32Array(const Float32 * /*val*/, 
                                        const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


E_Condition DcmElement::putFloat64Array(const Float64 * /*val*/, 
                                        const unsigned long /*num*/)
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


E_Condition DcmElement::read(DcmStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding /*glenc*/,
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
                    if (errorFlag != EC_Normal)
                    {
                        /* Print an error message when too few bytes are available in the file in order to
                         * distinguish this problem from any other generic "InvalidStream" problem. */
                        CERR << "ERROR: " << getTag().getTagName() << getTag().getXTag() << " larger ("
                             << Length << ") that remaining bytes in file" << endl;
                    }
                }

                if (fValue)
                    delete[] fValue;
                fTransferState = ERW_inWork;
            }
                        
            if (fTransferState == ERW_inWork && !fLoadValue)
                errorFlag = this -> loadValue(&inStream);

            if (fTransferredBytes == Length || fLoadValue)
                fTransferState = ERW_ready;
        }
    }
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


E_Condition DcmElement::write(DcmStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType /*enctype*/)
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
** Revision 1.29  2000-03-03 14:05:32  meichel
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
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
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
