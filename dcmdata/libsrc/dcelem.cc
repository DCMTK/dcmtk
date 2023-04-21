/*
 *
 *  Copyright (C) 1994-2023, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmElement
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdefine.h"

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcfcache.h"    /* for class DcmFileCache */
#include "dcmtk/dcmdata/dcwcache.h"    /* for class DcmWriteCache */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/vrscan.h"
#include "dcmtk/dcmdata/dcpath.h"

#include <cstring>                      /* for memset() */

#define SWAPBUFFER_SIZE 16  /* sufficient for all DICOM VRs as per the 2007 edition */

//
// CLASS DcmElement
//

DcmElement::DcmElement(const DcmTag &tag,
                       const Uint32 len)
  : DcmObject(tag, len),
    fByteOrder(gLocalByteOrder),
    fLoadValue(NULL),
    fValue(NULL)
{
}


DcmElement::DcmElement(const DcmElement &elem)
  : DcmObject(elem),
    fByteOrder(elem.fByteOrder),
    fLoadValue(NULL),
    fValue(NULL)
{
    if (elem.fValue)
    {
        DcmVR vr(elem.getVR());
        const unsigned short pad = (vr.isaString()) ? OFstatic_cast(unsigned short, 1) : OFstatic_cast(unsigned short, 0);

        // The next lines are a special version of newValueField().
        // newValueField() cannot be used because it is virtual and it does
        // not allocate enough bytes for strings. The number of pad bytes
        // is added to the Length for this purpose.
        if (getLengthField() & 1)
        {
#ifdef HAVE_STD__NOTHROW
            // we want to use a non-throwing new here if available
            // If the allocation fails, we report an EC_MemoryExhausted error
            // back to the caller.
            fValue = new (std::nothrow) Uint8[getLengthField() + 1 + pad];    // protocol error: odd value length
#else
            /* make sure that the pointer is set to NULL in case of error */
            try
            {
                fValue = new Uint8[getLengthField() + 1 + pad];    // protocol error: odd value length
            }
            catch (STD_NAMESPACE bad_alloc const &)
            {
                fValue = NULL;
            }
#endif
            if (fValue)
                fValue[getLengthField()] = 0;
            setLengthField(getLengthField() + 1);              // make Length even
        }
        else
        {
#ifdef HAVE_STD__NOTHROW
            // we want to use a non-throwing new here if available.
            // If the allocation fails, we report an EC_MemoryExhausted error
            // back to the caller.
            fValue = new (std::nothrow) Uint8[getLengthField() + pad];
#else
            /* make sure that the pointer is set to NULL in case of error */
            try
            {
                fValue = new Uint8[getLengthField() + pad];
            }
            catch (STD_NAMESPACE bad_alloc const &)
            {
                fValue = NULL;
            }
#endif
        }

        if (!fValue)
            errorFlag = EC_MemoryExhausted;

        if (pad && fValue)
            fValue[getLengthField()] = 0;

        if (fValue)
            memcpy(fValue, elem.fValue, size_t(getLengthField() + pad));
    }

    if (elem.fLoadValue)
        fLoadValue = elem.fLoadValue->clone();
}


DcmElement &DcmElement::operator=(const DcmElement &obj)
{
  if (this != &obj)
  {
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
    // if created with the nothrow version it must also be deleted with
    // the nothrow version else memory error.
    operator delete[] (fValue, std::nothrow);
#else
    delete[] fValue;
#endif
    delete fLoadValue;
    fLoadValue = NULL;
    fValue = NULL;

    DcmObject::operator=(obj);
    fByteOrder = obj.fByteOrder;

    if (obj.fValue)
    {
        DcmVR vr(obj.getVR());
        const unsigned short pad = (vr.isaString()) ? OFstatic_cast(unsigned short, 1) : OFstatic_cast(unsigned short, 0);

        // The next lines are a special version of newValueField().
        // newValueField() cannot be used because it is virtual and it does
        // not allocate enough bytes for strings. The number of pad bytes
        // is added to the Length for this purpose.

        if (getLengthField() & 1)
        {
#ifdef HAVE_STD__NOTHROW
            // we want to use a non-throwing new here if available.
            // If the allocation fails, we report an EC_MemoryExhausted error
            // back to the caller.
            fValue = new (std::nothrow) Uint8[getLengthField() + 1 + pad];    // protocol error: odd value length
#else
            /* make sure that the pointer is set to NULL in case of error */
            try
            {
                fValue = new Uint8[getLengthField() + 1 + pad];    // protocol error: odd value length
            }
            catch (STD_NAMESPACE bad_alloc const &)
            {
                fValue = NULL;
            }
#endif
            if (fValue)
                fValue[getLengthField()] = 0;
            setLengthField(getLengthField() + 1);              // make Length even
        }
        else
        {
#ifdef HAVE_STD__NOTHROW
            // we want to use a non-throwing new here if available.
            // If the allocation fails, we report an EC_MemoryExhausted error
            // back to the caller.
            fValue = new (std::nothrow) Uint8[getLengthField() + pad];
#else
            /* make sure that the pointer is set to NULL in case of error */
            try
            {
                fValue = new Uint8[getLengthField() + pad];
            }
            catch (STD_NAMESPACE bad_alloc const &)
            {
                fValue = NULL;
            }
#endif
        }

        if (!fValue)
            errorFlag = EC_MemoryExhausted;

        if (pad && fValue)
            fValue[getLengthField()] = 0;
        if (fValue)
            memcpy(fValue, obj.fValue, size_t(getLengthField()+pad));
    }

    if (obj.fLoadValue)
        fLoadValue = obj.fLoadValue->clone();

  }
  return *this;
}


int DcmElement::compare(const DcmElement& rhs) const
{
    if (this == &rhs)
        return 0;

    DcmElement* myThis = OFconst_cast(DcmElement*, this);
    DcmElement* myRhs = OFconst_cast(DcmElement*, &rhs);

    DcmTagKey thisKey = (*myThis).getTag();
    DcmTagKey rhsKey = (*myRhs).getTag();

    if ( thisKey > rhsKey )
    {
        return 1;
    }
    else if (thisKey < rhsKey)
    {
        return -1;
    }

    if ( this->ident() != rhs.ident() )
        return -1;

    return 0;
}


OFCondition DcmElement::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmElement &, rhs);
    }
    return EC_Normal;
}


DcmElement::~DcmElement()
{
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
    // if created with the nothrow version it must also be deleted with
    // the nothrow version else memory error.
    operator delete[] (fValue, std::nothrow);
#else
    delete[] fValue;
#endif
    delete fLoadValue;
}


// ********************************


OFCondition DcmElement::clear()
{
    errorFlag = EC_Normal;
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
    // if created with the nothrow version it must also be deleted with
    // the nothrow version else memory error.
    operator delete[] (fValue, std::nothrow);
#else
    delete[] fValue;
#endif
    fValue = NULL;
    delete fLoadValue;
    fLoadValue = NULL;
    setLengthField(0);
    return errorFlag;
}


OFCondition DcmElement::checkValue(const OFString & /*vm*/,
                                   const OFBool /*oldFormat*/)
{
    return EC_IllegalCall;
}


// ********************************


Uint32 DcmElement::calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    DcmXfer xferSyn(xfer);
    DcmEVR vr = getVR();

    /* These VRs don't use extended length encoding, but when writing, they are
     * converted to EVR_UN, which DOES use extended length encoding.
     * (EVR_na should never happen here, it's just handled for completeness)
     */
    if (vr == EVR_UNKNOWN2B || vr == EVR_na)
        vr = EVR_UN;

    /* compute length of element value */
    const Uint32 elemLength = getLength(xfer, enctype);

    /* Create an object that represents this object's "valid" data type */
    DcmVR myvalidvr(vr);

    if ((elemLength) > 0xffff && (! myvalidvr.usesExtendedLengthEncoding()) && xferSyn.isExplicitVR())
    {
      /* special case: we are writing in explicit VR, the VR of this
       * element uses a 2-byte length encoding, but the element length is
       * too large for a 2-byte length field. We need to write this element
       * as VR=UN (or VR=OB) and adjust the length calculation accordingly.
       * Since UN and OB always have the same header length, we can simply
       * assume that we are using UN.
       */
       vr = EVR_UN;
    }

    /* now compute length of header */
    const Uint32 headerLength = xferSyn.sizeofTagHeader(vr);

    if (OFStandard::check32BitAddOverflow(headerLength, elemLength))
      return DCM_UndefinedLength;
    else
      return headerLength + elemLength;
}


OFBool DcmElement::canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax /*oldXfer*/)
{
    return (newXfer != EXS_Unknown);
}


OFCondition DcmElement::detachValueField(OFBool copy)
{
    OFCondition l_error = EC_Normal;
    if (getLengthField() != 0)
    {
        if (copy)
        {
            if (!fValue)
                l_error = loadValue();
            if (l_error.good())
            {
                Uint8 * newValue;
#ifdef HAVE_STD__NOTHROW
                // we want to use a non-throwing new here if available
                newValue = new (std::nothrow) Uint8[getLengthField()];
#else
                /* make sure that the pointer is set to NULL in case of error */
                try
                {
                    newValue = new Uint8[getLengthField()];
                }
                catch (STD_NAMESPACE bad_alloc const &)
                {
                    newValue = NULL;
                }
#endif
                if (newValue)
                {
                    memcpy(newValue, fValue, size_t(getLengthField()));
                    fValue = newValue;
                } else {
                    /* the copy could not be created, so return an error */
                    l_error = EC_MemoryExhausted;
                }
            }
        } else {
            fValue = NULL;
            setLengthField(0);
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmElement::getUint8(Uint8 & /*val*/,
                                 const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint16(Sint16 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint16(Uint16 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint32(Sint32 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint32(Uint32 & /*val*/,
                                  const unsigned long /*pos*/)
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


OFCondition DcmElement::getSint64(Sint64 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint64(Uint64 & /*val*/,
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


OFCondition DcmElement::getTagVal(DcmTagKey & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getOFString(OFString & /*val*/,
                                    const unsigned long /*pos*/,
                                    OFBool /*normalize*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getString(char * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getString(char * & /*val*/,
                                  Uint32 & /*len*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getOFStringArray(OFString &value,
                                         OFBool normalize)
{
    /* this is a general implementation which is only used when the derived
       VR class does not reimplement it
     */
    errorFlag = EC_Normal;
    const unsigned long count = getVM();
    /* initialize result string */
    value.clear();
    if (count > 0)
    {
        OFString string;
        unsigned long i = 0;
        /* reserve number of bytes expected (heuristic) */
        value.reserve(OFstatic_cast(unsigned int, getLength()));
        /* iterate over all values and convert them to a character string */
        while ((i < count) && (errorFlag = getOFString(string, i, normalize)).good())
        {
            if (i > 0)
                value += '\\';
            /* append current value to the result string */
            value += string;
            i++;
        }
    }
    return errorFlag;
}


OFCondition DcmElement::getUint8Array(Uint8 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint16Array(Sint16 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint16Array(Uint16 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint32Array(Sint32 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint32Array(Uint32 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat32Array(Float32 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint64Array(Sint64 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint64Array(Uint64 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat64Array(Float64 * & /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


void *DcmElement::getValue(const E_ByteOrder newByteOrder)
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
        if (getLengthField() != 0)
        {
            /* if the value has not yet been loaded, do so now */
            if (!fValue)
                errorFlag = loadValue();
            /* if everything is ok */
            if (errorFlag.good())
            {
                /* if this element's value's byte ordering does not correspond to the */
                /* desired byte ordering, we need to rearrange this value's bytes and */
                /* set its byte order indicator variable correspondingly */
                if (newByteOrder != fByteOrder)
                {
                    swapIfNecessary(newByteOrder, fByteOrder, fValue,
                                    getLengthField(), getTag().getVR().getValueWidth());
                    fByteOrder = newByteOrder;
                }
                /* if everything is ok, assign the current value to the result variable */
                if (errorFlag.good())
                    value = fValue;
            }
        }
    }
    /* return result */
    return value;
}


// ********************************


OFCondition DcmElement::loadAllDataIntoMemory()
{
    errorFlag = EC_Normal;
    if (!fValue && (getLengthField() != 0))
        errorFlag = loadValue();
    return errorFlag;
}


OFCondition DcmElement::loadValue(DcmInputStream *inStream)
{
    /* initialize return value */
    errorFlag = EC_Normal;
    /* only if the length of this element does not equal 0, read information */
    if (getLengthField() != 0)
    {
        DcmInputStream *readStream = inStream;
        OFBool isStreamNew = OFFalse;

        // if the NULL pointer was passed (i.e. we're not in the middle of
        // a read() cycle, and fValue is NULL (i.e. the attribute value still
        // remains in file and fLoadValue is not NULL (i.e. we know how to
        // load the value from that file, then let's do it..
        if (!readStream && fLoadValue && !fValue)
        {
            /* we need to read information from the stream which is */
            /* accessible through fLoadValue. Hence, reassign readStream */
            readStream = fLoadValue->create();

            isStreamNew = OFTrue;

            /* reset number of transferred bytes to zero */
            setTransferredBytes(0);
        }
        /* if we have a stream from which we can read */
        if (readStream)
        {
            /* check if the stream reported an error */
            errorFlag = readStream->status();
            /* if we encountered the end of the stream, set the error flag correspondingly */
            if (errorFlag.good() && readStream->eos())
                errorFlag = EC_EndOfStream;
            /* if we did not encounter the end of the stream and no error occurred so far, go ahead */
            else if (errorFlag.good())
            {
                /* if the object which holds this element's value does not yet exist, create it */
                if (!fValue)
                    fValue = newValueField(); /* also set errorFlag in case of error */

                /* if object could be created  (i.e. we have an object which can be used to capture this element's */
                /* value) we need to read a certain amount of bytes from the stream */
                if (fValue)
                {
                    /* determine how many bytes shall be read from the stream */
                    const Uint32 readLength = getLengthField() - getTransferredBytes();

                    /* read a corresponding amount of bytes from the stream, store the information in fValue */
                    /* increase the counter that counts how many bytes were actually read */
                    incTransferredBytes(OFstatic_cast(Uint32, readStream->read(&fValue[getTransferredBytes()], readLength)));

                    /* if we have read all the bytes which make up this element's value */
                    if (getLengthField() == getTransferredBytes())
                    {
                        /* call a function which performs certain operations on the information which was read */
                        postLoadValue();
                        errorFlag = EC_Normal;
                    }
                    /* else set the return value correspondingly */
                    else if (readStream->eos())
                    {
                        errorFlag = EC_InvalidStream; // premature end of stream
                        DCMDATA_ERROR("DcmElement: " << getTagName() << " " << getTag()
                            << " larger (" << getLengthField() << ") than remaining bytes ("
                            << getTransferredBytes() << ") in file, premature end of stream");
                    }
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


// ********************************


Uint8 *DcmElement::newValueField()
{
    Uint8 * value;
    /* if this element's length is odd */
    Uint32 lengthField = getLengthField();
    if (lengthField & 1)
    {
        if (lengthField == DCM_UndefinedLength)
        {
            /* Print an error message when private attribute states to have an odd length
             * equal to the maximum length, because we are not able then to make this value even (+1)
             * which would an overflow on some systems as well as being illegal in DICOM
             */
              DCMDATA_ERROR("DcmElement: " << getTagName() << " " << getTag()
                  << " has odd maximum length (" << DCM_UndefinedLength << ") and therefore is not loaded");
              errorFlag = EC_CorruptedData;
              return NULL;
        }
        /* create an array of Length+1 bytes */
#ifdef HAVE_STD__NOTHROW
        // we want to use a non-throwing new here if available.
        // If the allocation fails, we report an EC_MemoryExhausted error
        // back to the caller.
        value = new (std::nothrow) Uint8[lengthField + 1];    // protocol error: odd value length
#else
        /* make sure that the pointer is set to NULL in case of error */
        try
        {
            value = new Uint8[lengthField + 1];    // protocol error: odd value length
        }
        catch (STD_NAMESPACE bad_alloc const &)
        {
            value = NULL;
        }
#endif
        /* if creation was successful, set last byte to 0 (in order to initialize this byte) */
        /* (no value will be assigned to this byte later, since Length was odd) */
        if (value)
            value[lengthField] = 0;
        /* enforce old (pre DCMTK 3.5.2) behaviour ? */
        if (!dcmAcceptOddAttributeLength.get())
        {
            lengthField++;
            setLengthField(lengthField);           // make Length even
        }
    }
    /* if this element's length is even, create a corresponding array of Length bytes */
    else
#ifdef HAVE_STD__NOTHROW
        // we want to use a non-throwing new here if available.
        // If the allocation fails, we report an EC_MemoryExhausted error
        // back to the caller.
        value = new (std::nothrow) Uint8[lengthField];
#else
        /* make sure that the pointer is set to NULL in case of error */
        try
        {
            value = new Uint8[lengthField];
        }
        catch (STD_NAMESPACE bad_alloc const &)
        {
            value = NULL;
        }
#endif
    /* if creation was not successful set member error flag correspondingly */
    if (!value)
        errorFlag = EC_MemoryExhausted;
    /* return byte array */
    return value;
}


// ********************************


void DcmElement::postLoadValue()
{
    if (dcmEnableAutomaticInputDataCorrection.get())
    {
        // newValueField always allocates an even number of bytes
        // and sets the pad byte to zero, so we can safely increase Length here
        if (getLengthField() & 1)
            setLengthField(getLengthField() + 1);           // make Length even
    }
}


// ********************************


OFCondition DcmElement::changeValue(const void *value,
                                    const Uint32 position,
                                    const Uint32 num)
{
    errorFlag = EC_Normal;
    // check for invalid parameter values
    if (position % num != 0 || getLengthField() % num != 0 || position > getLengthField())
        errorFlag = EC_IllegalCall;
    else if (position == getLengthField())
    {
        // append value
        if (getLengthField() == 0)
        {
            errorFlag = putValue(value, num);
        } else {
            // load value (if not loaded yet)
            if (!fValue)
                errorFlag = loadValue();
            if (errorFlag.good())
            {
                Uint8 * newValue;
                // allocate new memory for value
#ifdef HAVE_STD__NOTHROW
                // we want to use a non-throwing new here if available.
                // If the allocation fails, we report an EC_MemoryExhausted error
                // back to the caller.
                newValue = new (std::nothrow) Uint8[getLengthField() + num];
#else
                /* make sure that the pointer is set to NULL in case of error */
                try
                {
                    newValue = new Uint8[getLengthField() + num];
                }
                catch (STD_NAMESPACE bad_alloc const &)
                {
                    newValue = NULL;
                }
#endif
                if (!newValue)
                    errorFlag = EC_MemoryExhausted;
                if (errorFlag.good())
                {
                    // swap to local byte order
                    swapIfNecessary(gLocalByteOrder, fByteOrder, fValue,
                                    getLengthField(), getTag().getVR().getValueWidth());
                    fByteOrder = gLocalByteOrder;
                    // copy old value in the beginning of new value
                    memcpy(newValue, fValue, size_t(getLengthField()));
                    // copy value passed as a parameter to the end
                    memcpy(&newValue[getLengthField()], OFstatic_cast(const Uint8 *, value), size_t(num));
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
                    // if created with the nothrow version it must also be deleted with
                    // the nothrow version else memory error.
                    operator delete[] (fValue, std::nothrow);
#else
                    delete[] fValue;
#endif
                    fValue = newValue;
                    setLengthField(getLengthField() + num);
                } else
                    errorFlag = EC_MemoryExhausted;
            }
        }
    } else {
        // load value (if not loaded yet)
        if (!fValue)
            errorFlag = loadValue();
        if (errorFlag.good())
        {
            // swap to local byte order
            swapIfNecessary(gLocalByteOrder, fByteOrder, fValue,
                            getLengthField(), getTag().getVR().getValueWidth());
            // copy value at given position
            memcpy(&fValue[position], OFstatic_cast(const Uint8 *, value), size_t(num));
            fByteOrder = gLocalByteOrder;
        }
    }
    return errorFlag;
}


// ********************************


OFCondition DcmElement::putOFStringArray(const OFString &val)
{
    /* sets the value of a complete (possibly multi-valued) string attribute */
    return putString(val.c_str(), OFstatic_cast(Uint32, val.length()));
}


OFCondition DcmElement::putString(const char * /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putString(const char * /*val*/,
                                  const Uint32 /*len*/)
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
    {
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
        // if created with the nothrow version it must also be deleted with
        // the nothrow version else memory error.
        operator delete[] (fValue, std::nothrow);
#else
        delete[] fValue;
#endif
    }
    fValue = NULL;

    if (fLoadValue)
        delete fLoadValue;
    fLoadValue = NULL;

    setLengthField(length);

    if (length != 0)
    {
        fValue = newValueField();

        // newValueField always allocates an even number of bytes
        // and sets the pad byte to zero, so we can safely increase Length here
        if (getLengthField() & 1)
            setLengthField(getLengthField() + 1);           // make Length even

        // copy length (which may be odd), not Length (which is always even)
        if (fValue)
            memcpy(fValue, newValue, size_t(length));
        else
            errorFlag = EC_MemoryExhausted;
    }
    fByteOrder = gLocalByteOrder;
    return errorFlag;
}


// ********************************


OFCondition DcmElement::createUint8Array(const Uint32 /*numBytes*/,
                                         Uint8 *& /*bytes*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::createUint16Array(const Uint32 /*numWords*/,
                                          Uint16 *& /*bytes*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmElement::createEmptyValue(const Uint32 length)
{
    errorFlag = EC_Normal;
    if (fValue)
    {
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
        // if created with the nothrow version it must also be deleted with
        // the nothrow version else memory error.
        operator delete[] (fValue, std::nothrow);
#else
        delete[] fValue;
#endif
    }
    fValue = NULL;
    if (fLoadValue)
        delete fLoadValue;
    fLoadValue = NULL;
    setLengthField(length);

    if (length != 0)
    {
        fValue = newValueField();
        // newValueField always allocates an even number of bytes
        // and sets the pad byte to zero, so we can safely increase Length here
        if (getLengthField() & 1)
            setLengthField(getLengthField() + 1);           // make Length even

        // initialize <length> bytes (which may be odd), not Length (which is always even)
        if (fValue)
            memset(fValue, 0, size_t(length));
        else
            errorFlag = EC_MemoryExhausted;
    }
    fByteOrder = gLocalByteOrder;
    return errorFlag;
}


// ********************************


OFCondition DcmElement::read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding /*glenc*/,
                             const Uint32 maxReadLength)
{
    /* if this element's transfer state shows ERW_notInitialized, this is an illegal call */
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, go ahead and create a DcmXfer */
        /* object based on the transfer syntax which was passed */
        DcmXfer inXfer(ixfer);

        /* determine the transfer syntax's byte ordering */
        if (getTag() == DCM_PixelData)
          fByteOrder = inXfer.getPixelDataByteOrder();
          else fByteOrder = inXfer.getByteOrder();

        /* check if the stream reported an error */
        errorFlag = inStream.status();
        /* if we encountered the end of the stream, set the error flag correspondingly */
        if (errorFlag.good() && inStream.eos())
        {
            errorFlag = EC_EndOfStream; // not handled as parsing error by caller
            /* Network can never report EOS at this point, so we are in a file
             * and no bytes are left for reading. This is only valid if
             * the announced length of the current value is 0 and we are in the
             * last element of the dataset (e.g. Pixel Data) otherwise it must be an error */
            if (getLengthField() > 0)
            {
                /* Return error code if we are are not ignoring parsing errors */
                if (!dcmIgnoreParsingErrors.get())
                    errorFlag = EC_StreamNotifyClient; // should we rather return EC_InvalidStream?
                /* In any case, make sure that calling the load value routine on this
                 * element will fail later. For that, create the stream factory that
                 * the load routine will use. Otherwise it would not realize
                 * that there is a problem */
                delete fLoadValue;
                fLoadValue = inStream.newFactory();
                /* Print an error message when too few bytes are available in the file in order to
                 * distinguish this problem from any other generic "InvalidStream" problem. */
                DCMDATA_ERROR("DcmElement: " << getTagName() << " " << getTag()
                    << " larger (" << getLengthField() << ") than remaining bytes in file");
            }
        }
        /* if we did not encounter the end of the stream and no error occurred so far, go ahead */
        else if (errorFlag.good())
        {
            /* if the transfer state is ERW_init, we need to prepare */
            /* the reading of this element's value from the stream */
            if (getTransferState() == ERW_init)
            {
                /* if the Length of this element's value is greater than the amount of bytes we */
                /* can read from the stream and if the stream has random access, we want to create */
                /* a DcmInputStreamFactory object that enables us to read this element's value later. */
                /* This new object will be stored (together with the position where we have to start */
                /* reading the value) in the member variable fLoadValue. */
                if (getLengthField() > maxReadLength)
                {
                    /* try to create a stream factory to read the value later */
                    delete fLoadValue;
                    fLoadValue = inStream.newFactory();

                    if (fLoadValue)
                    {
                        offile_off_t skipped = inStream.skip(getLengthField());
                        if (skipped < OFstatic_cast(offile_off_t, getLengthField()))
                        {
                            /* If desired, specific parser errors will be ignored */
                            if (dcmIgnoreParsingErrors.get())
                                errorFlag = EC_Normal;
                            else
                                errorFlag = EC_StreamNotifyClient;
                            /* Print an error message when too few bytes are available in the file in order to
                             * distinguish this problem from any other generic "InvalidStream" problem. */
                            DCMDATA_ERROR("DcmElement: " << getTagName() << " " << getTag()
                                << " larger (" << getLengthField() << ") than remaining bytes in file");
                        }
                    }
                }
                /* if there is already a value for this element, delete this value */
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
                // if created with the nothrow version it must also be deleted with
                // the nothrow version else memory error.
                operator delete[] (fValue, std::nothrow);
#else
                delete[] fValue;
#endif
                /* set the transfer state to ERW_inWork */
                setTransferState(ERW_inWork);
            }
            /* if the transfer state is ERW_inWork and we are not supposed */
            /* to read this element's value later, read the value now */
            if (getTransferState() == ERW_inWork && !fLoadValue)
                errorFlag = loadValue(&inStream);
            /* if the amount of transferred bytes equals the Length of this element */
            /* or the object which contains information to read the value of this */
            /* element later is existent, set the transfer state to ERW_ready */
            if (getTransferredBytes() == getLengthField() || fLoadValue)
                setTransferState(ERW_ready);
        }
    }

    /* return result value */
    return errorFlag;
}


// ********************************


void DcmElement::swapValueField(size_t valueWidth)
{
    if (getLengthField() != 0)
    {
        if (!fValue)
            errorFlag = loadValue();

        if (errorFlag.good())
            swapBytes(fValue, getLengthField(), valueWidth);
    }
}


// ********************************


void DcmElement::transferInit()
{
    DcmObject::transferInit();
    setTransferredBytes(0);
}


// ********************************

OFCondition DcmElement::write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache)
{
    DcmWriteCache wcache2;

    /* Create an object that represents this object's data type */
    DcmVR myvr(getVR());

    /* create an object that represents the transfer syntax */
    DcmXfer outXfer(oxfer);

    /* getValidEVR() will convert post 1993 VRs to "OB" if these are disabled */
    DcmEVR vr = myvr.getValidEVR();

    /* compute length of element value */
    const Uint32 elemLength = getLength(oxfer, enctype);

    /* Create an object that represents this object's "valid" data type */
    DcmVR myvalidvr(vr);

    if ((elemLength) > 0xffff && (! myvalidvr.usesExtendedLengthEncoding()) && outXfer.isExplicitVR())
    {
        /* special case: we are writing in explicit VR, the VR of this
         * element uses a 2-byte length encoding, but the element length is
         * too large for a 2-byte length field. We need to write this element
         * as VR=UN (or VR=OB if the generation of UN is disabled).
         * In this method, the variable "vr" is only used to determine the
         * output byte order, which is always the same for OB and UN.
         * Therefore, we do not need to distinguish between these two.
         */
        vr = EVR_UN;
    }

    /* In case the transfer state is not initialized, this is an illegal call */
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, we need to do something. First */
        /* of all, check the error state of the stream that was passed */
        /* only do something if the error state of the stream is ok */
        errorFlag = outStream.status();
        if (errorFlag.good())
        {
            E_ByteOrder outByteOrder;

            /* determine the transfer syntax's byte ordering for this element */
            if (getTag() == DCM_PixelData)
              outByteOrder = outXfer.getPixelDataByteOrder();
              else outByteOrder = outXfer.getByteOrder();

            /* UN and OB element content always needs to be written in little endian.
               We need to set this manually for the case that we are converting
               elements to UN or OB while writing because some post-1993 VRs are disabled */
            if ((vr == EVR_OB) || (vr == EVR_UN)) outByteOrder = EBO_LittleEndian;

            /* pointer to element value if value resides in memory or old-style
             * write behaviour is active (i.e. everything loaded into memory upon write)
             */
            Uint8 *value = NULL;
            OFBool accessPossible = OFFalse;

            /* check that we actually do have access to the element's value.
             * If the element is unaccessible (which would mean that the value resides
             * in file and access to the file fails), write an empty element with
             * zero length.
             */
            if (getLengthField() > 0)
            {
              if (valueLoaded())
              {
                /* get this element's value. Mind the byte ordering (little */
                /* or big endian) of the transfer syntax which shall be used */
                value = OFstatic_cast(Uint8 *, getValue(outByteOrder));
                if (value) accessPossible = OFTrue;
              }
              else
              {
                /* Use local cache object if needed. This may cause those bytes
                 * that are read but not written because the output stream stalls to
                 * be read again, and the file from being re-opened next time.
                 * Therefore, this case should be avoided.
                 */
                if (!wcache) wcache = &wcache2;

                /* initialize cache object. This is safe even if the object was already initialized */
                wcache->init(this, getLengthField(), getTransferredBytes(), outByteOrder);

                /* access first block of element content */
                errorFlag = wcache->fillBuffer(*this);

                /* check that everything worked and the buffer is non-empty now */
                accessPossible = errorFlag.good() && (! wcache->bufferIsEmpty());
              }
            }

            /* if this element's transfer state is ERW_init (i.e. it has not yet been written to */
            /* the stream) and if the outstream provides enough space for tag and length information */
            /* write tag and length information to it, do something */
            if (getTransferState() == ERW_init)
            {
                // Force a compression filter (if any) to process the input buffer, by calling outStream.write().
                // This ensures that we cannot get stuck if there are just a few bytes available in the buffer
                outStream.write(NULL, 0);

                /* first compare with DCM_TagInfoLength (12). If there is not enough space
                 * in the buffer, check if the buffer is still sufficient for the requirements
                 * of this element, which may need only 8 instead of 12 bytes.
                 */
                if ((outStream.avail() >= OFstatic_cast(offile_off_t, DCM_TagInfoLength)) ||
                    (outStream.avail() >= OFstatic_cast(offile_off_t, getTagAndLengthSize(oxfer))))
                {
                    /* if there is no value, Length (member variable) shall be set to 0 */
                    if (! accessPossible) setLengthField(0);

                    /* remember how many bytes have been written to the stream, currently none so far */
                    Uint32 writtenBytes = 0;

                    /* write tag and length information (and possibly also data type information) to the stream, */
                    /* mind the transfer syntax and remember the amount of bytes that have been written */
                    errorFlag = writeTagAndLength(outStream, oxfer, writtenBytes);

                    /* if the writing was successful, set this element's transfer */
                    /* state to ERW_inWork and the amount of transferred bytes to 0 */
                    if (errorFlag.good())
                    {
                        setTransferState(ERW_inWork);
                        setTransferredBytes(0);
                    }
                } else errorFlag = EC_StreamNotifyClient;
            }

            /* if there is a value that has to be written to the stream */
            /* and if this element's transfer state is ERW_inWork */
            if (accessPossible && getTransferState() == ERW_inWork)
            {
                Uint32 len = 0;
                if (valueLoaded())
                {
                    /* write as many bytes as possible to the stream starting at value[getTransferredBytes()] */
                    /* (note that the bytes value[0] to value[getTransferredBytes()-1] have already been */
                    /* written to the stream) */
                    len = OFstatic_cast(Uint32, outStream.write(&value[getTransferredBytes()], getLengthField() - getTransferredBytes()));

                    /* increase the amount of bytes which have been transferred correspondingly */
                    incTransferredBytes(len);

                    /* see if there is something fishy with the stream */
                    errorFlag = outStream.status();
                }
                else
                {
                    Uint32 buflen = 0;
                    OFBool done = getTransferredBytes() == getLengthField();
                    while (! done)
                    {
                      // re-fill buffer from file if empty
                      errorFlag = wcache->fillBuffer(*this);
                      buflen = wcache->contentLength();

                      if (errorFlag.good())
                      {
                        // write as many bytes from cache buffer to stream as possible
                        len = wcache->writeBuffer(outStream);

                        /* increase the amount of bytes which have been transferred correspondingly */
                        incTransferredBytes(len);

                        /* see if there is something fishy with the stream */
                        errorFlag = outStream.status();
                      }

                      // stop writing if something went wrong, we were unable to send all of the buffer content
                      // (which indicates that the output stream needs to be flushed, or everything was sent out.
                      done = errorFlag.bad() || (len < buflen) || (getTransferredBytes() == getLengthField());
                    }
                }

                /* if the amount of transferred bytes equals the length of the element's value, the */
                /* entire value has been written to the stream. Thus, this element's transfer state */
                /* has to be set to ERW_ready. If this is not the case but the error flag still shows */
                /* an ok value, there was no more space in the stream and a corresponding return value */
                /* has to be set. (Isn't the "else if" part superfluous?!?) */
                if (getLengthField() == getTransferredBytes()) setTransferState(ERW_ready);
                else if (errorFlag.good()) errorFlag = EC_StreamNotifyClient;

            }
        }
    }
    /* return result value */
    return errorFlag;
}


OFCondition DcmElement::writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype,
                                             DcmWriteCache *wcache)
{
    // for normal DICOM elements (everything except sequences), the data
    // stream used for digital signature creation or verification is
    // identical to the stream used for network communication or media
    // storage.
    return write(outStream, oxfer, enctype, wcache);
}


// ********************************


void DcmElement::writeXMLStartTag(STD_NAMESPACE ostream &out,
                                  const size_t flags,
                                  const char *attrText)
{
    OFString xmlString;
    DcmVR vr(getTag().getVR());
    DcmTag tag = getTag();
    const OFBool isPrivate = tag.isPrivate();

    /* write XML start tag for attribute */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        out << "<DicomAttribute";
        out << STD_NAMESPACE uppercase;
    } else
        out << "<element";

    /* write attribute tag */
    out << " tag=\"";
    out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
        << STD_NAMESPACE setw(4) << tag.getGTag();
    /* in Native DICOM Model, write "ggggeeee" (no comma, upper case!) */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* for private element numbers, zero out 2 first element digits */
        if (isPrivate)
        {
            out << STD_NAMESPACE setw(4) << (tag.getETag() & 0x00FF) << "\""
                << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
        }
        else  /* output full element number "eeee" */
        {
            out << STD_NAMESPACE setw(4) << tag.getETag() << "\""
                << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
        }
        out << STD_NAMESPACE nouppercase;
    }
    else  /* in DCMTK-specific format, write "gggg,eeee" */
    {
        out << "," << STD_NAMESPACE setw(4) << tag.getETag() << "\""
            << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
    }

    /* value representation = VR */
    out << " vr=\"" << vr.getValidVRName() << "\"";

    if (flags & DCMTypes::XF_useNativeModel)
    {
        if (isPrivate)
        {
            /* output the creator of this private tag (but not for the creator tag itself) */
            if (!tag.isPrivateReservation())
            {
                const char *creator = tag.getPrivateCreator();
                if (creator != NULL)
                {
                    out << " privateCreator=\"";
                    out << creator << "\"";
                } else {
                    DCMDATA_WARN("Cannot write private creator for group 0x"
                        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
                        << tag.getGTag() << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ')
                        << " to XML output: Not present in data set");
                }
            }
        } else {
            /* write attribute keyword if known (and the official name is used in the data dictionary) */
            const OFString tagName = getTagName();
            if ((tagName != DcmTag_ERROR_TagName) &&
                /* check for DCMTK-specific name prefixes used for old ACR NEMA and retired tags */
                (tagName.substr(0, 8) != "RETIRED_") && (tagName.substr(0, 9) != "ACR_NEMA_"))
            {
                out << " keyword=\"" << OFStandard::convertToMarkupString(tagName, xmlString) << "\"";
            }
        }
        /* close XML start tag */
        out << ">" << OFendl;
    } else {
        /* value multiplicity = 1..n */
        out << " vm=\"" << getVM() << "\"";
        /* value length in bytes = 0..max */
        out << " len=\"" << getLengthField() << "\"";
        /* tag name (if known and not suppressed) */
        if (!(flags & DCMTypes::XF_omitDataElementName))
            out << " name=\"" << OFStandard::convertToMarkupString(getTagName(), xmlString) << "\"";
        /* value loaded = no (or absent)*/
        if (!valueLoaded())
            out << " loaded=\"no\"";
        /* write additional attributes (if any) */
        if ((attrText != NULL) && (attrText[0] != '\0'))
            out << " " << attrText;
        /* close XML start tag */
        out << ">";
    }
}


void DcmElement::writeXMLEndTag(STD_NAMESPACE ostream &out,
                                const size_t flags)
{
    /* write standardized XML end tag for all element types */
    if (flags & DCMTypes::XF_useNativeModel)
        out << "</DicomAttribute>" << OFendl;
    else
        out << "</element>" << OFendl;
}


OFCondition DcmElement::writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags)
{
    /* do not output group length elements in Native DICOM Model
     * (as per PS 3.19 section A.1.1, introduced with Supplement 166) */
    if (!(flags & DCMTypes::XF_useNativeModel) || !getTag().isGroupLength())
    {
        /* write XML start tag */
        writeXMLStartTag(out, flags);
        OFString value;
        const OFBool convertNonASCII = (flags & DCMTypes::XF_convertNonASCII) > 0;
        if (flags & DCMTypes::XF_useNativeModel)
        {
            /* write element value (if non-empty) */
            if (!isEmpty())
            {
                const unsigned long vm = getVM();
                for (unsigned long valNo = 0; valNo < vm; valNo++)
                {
                    if (getOFString(value, valNo).good())
                    {
                        out << "<Value number=\"" << (valNo + 1) << "\">";
                        /* check whether conversion to XML markup string is required */
                        if (OFStandard::checkForMarkupConversion(value, convertNonASCII))
                            OFStandard::convertToMarkupStream(out, value, convertNonASCII);
                        else
                            out << value;
                        out << "</Value>" << OFendl;
                    }
                }
            }
        } else {
            /* write element value (only if loaded) */
            if (valueLoaded())
            {
                if (getOFStringArray(value).good())
                {
                    /* check whether conversion to XML markup string is required */
                    if (OFStandard::checkForMarkupConversion(value, convertNonASCII))
                        OFStandard::convertToMarkupStream(out, value, convertNonASCII);
                    else
                        out << value;
                }
            }
        }
        /* write XML end tag  */
        writeXMLEndTag(out, flags);
    }
    /* always report success */
    return EC_Normal;
}


// ********************************


void DcmElement::writeJsonOpener(STD_NAMESPACE ostream &out,
                                 DcmJsonFormat &format)
{
    DcmVR vr(getTag().getVR());
    DcmTag tag = getTag();
    /* increase indentation level */
    /* write attribute tag */
    out << ++format.indent() << "\""
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
        << STD_NAMESPACE setw(4) << STD_NAMESPACE uppercase << tag.getGTag();
    /* write "ggggeeee" (no comma, upper case!) */
    /* for private element numbers, zero out 2 first element digits */
    /* or output full element number "eeee" */
    out << STD_NAMESPACE setw(4) << STD_NAMESPACE uppercase << tag.getETag() << "\":"
        << format.space() << "{" << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
    out << STD_NAMESPACE nouppercase;
    /* increase indentation level */
    /* value representation = VR */
    out << format.newline() << ++format.indent() << "\"vr\":" << format.space() << "\""
        << vr.getValidVRName() << "\"";
}


void DcmElement::writeJsonCloser(STD_NAMESPACE ostream &out,
                                 DcmJsonFormat &format)
{
    /* output JSON ending and decrease indentation level */
    out << format.newline() << --format.indent() << "}";
    --format.indent();
}


OFCondition DcmElement::writeJson(STD_NAMESPACE ostream &out,
                                  DcmJsonFormat &format)
{
    /* always write JSON Opener */
    writeJsonOpener(out, format);
    /* write element value (if non-empty) */
    if (!isEmpty())
    {
        OFString value;
        if (format.asBulkDataURI(getTag(), value))
        {
            format.printBulkDataURIPrefix(out);
            DcmJsonFormat::printString(out, value);
        }
        else
        {
            OFCondition status = getOFString(value, 0L);
            if (status.bad())
                return status;
            format.printValuePrefix(out);
            DcmJsonFormat::printNumberDecimal(out, value);
            const unsigned long vm = getVM();
            for (unsigned long valNo = 1; valNo < vm; ++valNo)
            {
                status = getOFString(value, valNo);
                if (status.bad())
                    return status;
                format.printNextArrayElementPrefix(out);
                DcmJsonFormat::printNumberDecimal(out, value);
            }
            format.printValueSuffix(out);
        }
    }
    /* write JSON Closer  */
    writeJsonCloser(out, format);
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmElement::getPartialValue(void *targetBuffer,
                                        const Uint32 offset,
                                        Uint32 numBytes,
                                        DcmFileCache *cache,
                                        E_ByteOrder byteOrder)
{
  // check integrity of parameters passed to this method
  if (targetBuffer == NULL) return EC_IllegalCall;

  // if the user has only requested zero bytes, we immediately return
  if (numBytes == 0) return EC_Normal;

  // offset must always be less than attribute length (unless offset,
  // attribute length and numBytes are all zero, a case that was
  // handled above).
  if (offset >= getLengthField()) return EC_InvalidOffset;

  // check if the caller is trying to read past the end of the value field
  if (numBytes > (getLengthField() - offset)) return EC_TooManyBytesRequested;

  // check if the value is already in memory
  if (valueLoaded())
  {
    // the attribute value is already in memory.
    // change internal byte order of the attribute value to the desired byte order.
    // This should only happen once for multiple calls to this method since the
    // caller will hopefully always request the same byte order.
    char *value = OFstatic_cast(char *, getValue(byteOrder));
    if (value)
    {
      memcpy(targetBuffer, value + offset, numBytes);
    }
    else
    {
      // this should never happen because valueLoaded() returned true, but
      // we don't want to dereference a NULL pointer anyway
      return EC_IllegalCall;
    }
  }
  else
  {
    // the value is not in memory. We should directly read from file and
    // also consider byte order.

    // since the value is not in memory, fLoadValue should exist. Check anyway.
    if (! fLoadValue) return EC_IllegalCall;

    // make sure we have a file cache object
    DcmFileCache defaultcache; // automatic object, creation is cheap.
    if (cache == NULL) cache = &defaultcache;

    // the stream from which we will read the attribute value
    DcmInputStream *readStream = NULL;

    // check if we need to seek to a position in file earlier than
    // the one specified by the user in order to correctly swap according
    // to the VR.
    size_t valueWidth = getTag().getVR().getValueWidth();

    // we need to cast the target buffer to something we can increment bytewise
    char *targetBufferChar = OFreinterpret_cast(char *, targetBuffer);

    // the swap buffer should be large enough to keep one value of the current VR
    unsigned char swapBuffer[SWAPBUFFER_SIZE];
    if (valueWidth > SWAPBUFFER_SIZE) return EC_IllegalCall;

    // seekoffset is the number of bytes we need to skip from the beginning of the
    // value field to the point where we will start reading. This is always at the
    // start of a new value of a multi-valued attribute.
    Uint32 partialvalue = 0;
    const Uint32 partialoffset = OFstatic_cast(Uint32, offset % valueWidth);
    const offile_off_t seekoffset = offset - partialoffset;

    // check if cache already contains the stream we're looking for
    if (cache->isUser(this))
    {
      readStream = cache->getStream();

      // since we cannot seek back in the stream (only forward), check if the stream
      // is already past our needed start position
      if (readStream->tell() - cache->getOffset() > seekoffset)
      {
        readStream = NULL;
      }
    }

    // initialize the cache with new stream
    if (!readStream)
    {
      // create input stream object
      readStream = fLoadValue->create();

      // check that read stream is non-NULL
      if (readStream == NULL) return EC_InvalidStream;

      // check that stream status is OK
      if (readStream->status().bad())
      {
        OFCondition result = readStream->status();
        delete readStream;
        return result;
      }

      // readStream will be deleted when the cache is deleted
      cache->init(readStream, this);
    }

    // now skip bytes from our current position in file to where we
    // want to start reading.
    offile_off_t remainingBytesToSkip = seekoffset - (readStream->tell() - cache->getOffset());
    offile_off_t skipResult;

    while (remainingBytesToSkip)
    {
      skipResult = readStream->skip(remainingBytesToSkip);
      if (skipResult == 0) return EC_InvalidStream; // error while skipping
      remainingBytesToSkip -= skipResult;
    }

    // check if the first few bytes we want to read are "in the middle" of one value
    // of a multi-valued attribute. In that case we need to read the complete value,
    // swap it and then copy only the last bytes in desired byte order.
    if (partialoffset > 0)
    {
      // we possibly want to reset the stream to this point later
      readStream->mark();

      // compute the number of bytes we need to copy from the first attributes
      partialvalue = OFstatic_cast(Uint32, valueWidth - partialoffset);

      // we need to read a single data element into the swap buffer
      if (valueWidth != OFstatic_cast(size_t, readStream->read(swapBuffer, OFstatic_cast(offile_off_t, valueWidth))))
          return EC_InvalidStream;

      // swap to desired byte order. fByteOrder contains the byte order in file.
      swapIfNecessary(byteOrder, fByteOrder, swapBuffer, OFstatic_cast(Uint32, valueWidth), valueWidth);

      // copy to target buffer and adjust values
      if (partialvalue > numBytes)
      {
        memcpy(targetBufferChar, &swapBuffer[partialoffset], numBytes);
        targetBufferChar += numBytes;
        numBytes = 0;

        // Reset stream to position marked before, since we have not copied the complete value
        readStream->putback();
      }
      else
      {
        memcpy(targetBufferChar, &swapBuffer[partialoffset], partialvalue);
        targetBufferChar += partialvalue;
        numBytes -= partialvalue;
      }
    }

    // now read the main block of data directly into the target buffer
    partialvalue = OFstatic_cast(Uint32, numBytes % valueWidth);
    const Uint32 bytesToRead = numBytes - partialvalue;

    if (bytesToRead > 0)
    {
      // here we read the main block of data
     if (OFstatic_cast(offile_off_t, bytesToRead) != readStream->read(targetBufferChar, bytesToRead))
         return EC_InvalidStream;

      // swap to desired byte order. fByteOrder contains the byte order in file.
      swapIfNecessary(byteOrder, fByteOrder, targetBufferChar, bytesToRead, valueWidth);

      // adjust pointer to target buffer
      targetBufferChar += bytesToRead;
    }

    // check if the last few bytes we want to read are only a partial value.
    // In that case we need to read the complete value, swap it and then copy
    // only the first few bytes in desired byte order.
    if (partialvalue > 0)
    {
      OFBool appendDuplicateByte = OFFalse;
      size_t partialBytesToRead = valueWidth;

      // we want to reset the stream to this point later
      readStream->mark();

      if (readStream->tell() + valueWidth > getLengthField()) {
        // We are trying to read past the end of the value. We already made sure
        // above that the requested range fits completely into the element's
        // size, so this must mean that the length is not a multiple of the VR's
        // value width.
        // We allow this for OW and error out on all other VRs.
        if (getTag().getVR().getValidEVR() == EVR_OW)
        {
          DCMDATA_WARN("DcmElement: Trying to read past end of value, duplicating last byte");
          appendDuplicateByte = OFTrue;
          // This is 2 for OW, but we know that only 1 byte of data is available
          partialBytesToRead--;
        }
        else
        {
          // This would read the beginning of the next element from the stream,
          // possibly hitting the end of stream.
          DCMDATA_ERROR("DcmElement: Trying to read past end of value");
          return EC_InvalidStream;
        }
      }

      // we need to read a single data element into the swap buffer
      if (partialBytesToRead != OFstatic_cast(size_t, readStream->read(swapBuffer, OFstatic_cast(offile_off_t, partialBytesToRead))))
          return EC_InvalidStream;

      if (appendDuplicateByte)
        swapBuffer[partialBytesToRead] = swapBuffer[partialBytesToRead - 1];

      // swap to desired byte order. fByteOrder contains the byte order in file.
      swapIfNecessary(byteOrder, fByteOrder, swapBuffer, OFstatic_cast(Uint32, valueWidth), valueWidth);

      // copy to target buffer and adjust values
      memcpy(targetBufferChar, swapBuffer, partialvalue);

      // finally reset stream to position marked before
      readStream->putback();
    }
  }

  // done.
  return EC_Normal;
}


void DcmElement::compact()
{
  if (fLoadValue && fValue)
  {
    DCMDATA_DEBUG("DcmElement::compact() removed element value of " << getTag()
        << " with " << getTransferredBytes() << " bytes");
    delete[] fValue;
    fValue = NULL;
    setTransferredBytes(0);
  }
}


OFCondition DcmElement::createValueFromTempFile(DcmInputStreamFactory *factory,
                                                const Uint32 length,
                                                const E_ByteOrder byteOrder)
{
    if (factory && !(length & 1))
    {
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
        // if created with the nothrow version it must also be deleted with
        // the nothrow version else memory error.
        operator delete[] (fValue, std::nothrow);
#else
        delete[] fValue;
#endif
        fValue = 0;
        delete fLoadValue;
        fLoadValue = factory;
        fByteOrder = byteOrder;
        setLengthField(length);
        return EC_Normal;
    }
    else return EC_IllegalCall;
}


// the following macro makes the source code more readable and easier to maintain

#define GET_AND_CHECK_UINT16_VALUE(tag, variable)                                                                   \
    result = dataset->findAndGetUint16(tag, variable);                                                              \
    if (result == EC_TagNotFound)                                                                                   \
    {                                                                                                               \
        DCMDATA_WARN("DcmElement: Mandatory element " << DcmTag(tag).getTagName() << " " << tag << " is missing");  \
        result = EC_MissingAttribute;                                                                               \
    }                                                                                                               \
    else if ((result == EC_IllegalCall) || (result == EC_IllegalParameter))                                         \
    {                                                                                                               \
        DCMDATA_WARN("DcmElement: No value for mandatory element " << DcmTag(tag).getTagName() << " " << tag);      \
        result = EC_MissingValue;                                                                                   \
    }                                                                                                               \
    else if (result.bad())                                                                                          \
        DCMDATA_WARN("DcmElement: Cannot retrieve value of element " << DcmTag(tag).getTagName() << " " << tag << ": " << result.text());


OFCondition DcmElement::getUncompressedFrameSize(DcmItem *dataset,
                                                 Uint32 &frameSize,
                                                 OFBool pixelDataIsUncompressed) const
{
    OFCondition result = EC_IllegalParameter;
    if (dataset != NULL)
    {
        Uint16 rows = 0;
        Uint16 cols = 0;
        Uint16 samplesPerPixel = 0;
        Uint16 bitsAllocated = 0;
        Sint32 numberOfFrames = 1;
        OFString photometricInterpretation;

        /* retrieve number of frames from dataset (may be absent) */
        (void) dataset->findAndGetSint32(DCM_NumberOfFrames, numberOfFrames);
        if (numberOfFrames < 1) numberOfFrames = 1;

        /* retrieve further values from dataset (and check them for validity and plausibility) */
        GET_AND_CHECK_UINT16_VALUE(DCM_Columns, cols)
        else if (cols == 0)
            DCMDATA_WARN("DcmElement: Dubious value (" << cols << ") for element Columns " << DCM_Columns);
        if (result.good())
        {
            GET_AND_CHECK_UINT16_VALUE(DCM_Rows, rows)
            else if (rows == 0)
                DCMDATA_WARN("DcmElement: Dubious value (" << rows << ") for element Rows " << DCM_Rows);
        }
        if (result.good())
        {
            GET_AND_CHECK_UINT16_VALUE(DCM_SamplesPerPixel, samplesPerPixel)
            else /* result.good() */
            {
                /* also need to check value of PhotometricInterpretation */
                if (dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, photometricInterpretation).good())
                {
                    if (photometricInterpretation.empty())
                        DCMDATA_WARN("DcmElement: No value for mandatory element PhotometricInterpretation " << DCM_PhotometricInterpretation);
                    else {
                        const OFBool isMono =   (photometricInterpretation == "MONOCHROME1") ||
                                                (photometricInterpretation == "MONOCHROME2");
                        const OFBool isColor1 = (photometricInterpretation == "PALETTE COLOR");
                        const OFBool isColor3 = (photometricInterpretation == "RGB") ||
                                                (photometricInterpretation == "HSV" /* retired */) ||
                                                (photometricInterpretation == "YBR_FULL") ||
                                                (photometricInterpretation == "YBR_FULL_422") ||
                                                (photometricInterpretation == "YBR_PARTIAL_422" /* retired */) ||
                                                (photometricInterpretation == "YBR_PARTIAL_420") ||
                                                (photometricInterpretation == "YBR_ICT") ||
                                                (photometricInterpretation == "YBR_RCT");
                        const OFBool isColor4 = (photometricInterpretation == "ARGB" /* retired */) ||
                                                (photometricInterpretation == "CMYK" /* retired */);
                        if (((isMono || isColor1) && (samplesPerPixel != 1)) || (isColor3 && (samplesPerPixel != 3)) || (isColor4 && (samplesPerPixel != 4)))

                        {
                            DCMDATA_WARN("DcmElement: Invalid value (" << samplesPerPixel << ") for element SamplesPerPixel " << DCM_SamplesPerPixel
                                << " when PhotometricInterpretation " << DCM_PhotometricInterpretation << " is " << photometricInterpretation);
                            result = EC_InvalidValue;
                        }
                        else if (!isMono && !isColor1 && !isColor3 && !isColor4)
                            DCMDATA_WARN("DcmElement: Unsupported value (" << photometricInterpretation << ") for element PhotometricInterpretation " << DCM_PhotometricInterpretation);
                    }
                }
                if (result.good() && (samplesPerPixel != 1) && (samplesPerPixel != 3))
                    DCMDATA_WARN("DcmElement: Dubious value (" << samplesPerPixel << ") for element SamplesPerPixel " << DCM_SamplesPerPixel);
            }
        }
        if (result.good())
        {
            GET_AND_CHECK_UINT16_VALUE(DCM_BitsAllocated, bitsAllocated)
            /* see PS3.3 Table C.7-11c: "Bits Allocated (0028,0100) shall be either 1, or a multiple of 8." */
            else if ((bitsAllocated == 0) || ((bitsAllocated > 1) && (bitsAllocated % 8 != 0)))
                DCMDATA_WARN("DcmElement: Dubious value (" << bitsAllocated << ") for element BitsAllocated " << DCM_BitsAllocated);
        }
        /* if all checks were passed... */
        if (result.good())
        {
            if (pixelDataIsUncompressed && (photometricInterpretation == "YBR_FULL_422"))
            {
              /* YBR_FULL_422 can exist in uncompressed format, but in many cases
               * images claiming to be YBR_FULL_422 are in fact formerly compressed
               * images in YBR_FULL color model where the decoder has failed to update
               * the photometric interpretation. We can keep these apart by checking
               * the size of the pixel data and the number of frames.
               */
               Uint32 pixelLen = 0;
               DcmElement *pixData = NULL;
               result = dataset->findAndGetElement(DCM_PixelData, pixData);
               if (result.good() && pixData && ((pixelLen = pixData->getLength()) > 0))
               {
                  const Uint32 v1 = rows * cols * 3;
                  const Uint32 v2 = (bitsAllocated / 8) * v1;
                  const Uint32 v3 = ((bitsAllocated % 8) * v1 + 7) / 8;

                  if (pixelLen >= (v2 + v3) * numberOfFrames)
                  {
                     /* the size of the pixel data indicates that no subsampling is present. We assume YBR_FULL. */
                     DCMDATA_WARN("DcmElement: PhotometricInterpretation probably incorrect, assuming YBR_FULL instead of YBR_FULL_422");
                  }
                  else
                  {
                     /* the size of the pixel data indicates subsampling is present. We assume YBR_FULL_422,
                      * which means that the frame size can be computed by setting samplesPerPixel to 2.
                      */
                     samplesPerPixel = 2;
                  }
               }
               else
                   DCMDATA_WARN("DcmElement: failed to compute size of PixelData element");
            }

            /* compute frame size (TODO: check for 32-bit integer overflow?) */
            if ((bitsAllocated % 8) == 0)
            {
                const Uint16 bytesAllocated = bitsAllocated / 8;
                frameSize = bytesAllocated * rows * cols * samplesPerPixel;
            }
            else
            {
                /* need to split calculation in order to avoid integer overflow for large pixel data */
                const Uint32 v1 = rows * cols * samplesPerPixel;
                const Uint32 v2 = (bitsAllocated / 8) * v1;
                const Uint32 v3 = ((bitsAllocated % 8) * v1 + 7) / 8;
            //  # old code: frameSize = (bitsAllocated * rows * cols * samplesPerPixel + 7) / 8;
                frameSize = v2 + v3;
            }
        } else {
            /* in case of error, return a frame size of 0 */
            frameSize = 0;
        }
    }
    return result;
}


OFCondition DcmElement::getUncompressedFrame(DcmItem * /* dataset */ ,
                                             Uint32 /* frameNo */ ,
                                             Uint32& /* startFragment */ ,
                                             void * /* buffer */ ,
                                             Uint32 /* bufSize */ ,
                                             OFString& /* decompressedColorModel */ ,
                                             DcmFileCache * /* cache */ )
{
  return EC_IllegalCall;
}


OFCondition DcmElement::getDecompressedColorModel(DcmItem * /* dataset */,
                                                  OFString & /* decompressedColorModel */)
{
  return EC_IllegalCall;
}


// ********************************


int DcmElement::scanValue(const OFString &value,
                          const OFString &vr,
                          const size_t pos,
                          const size_t num)
{
  return scanValue(vr, value.data() + pos, num != OFString_npos ? num : value.size() - pos);
}


int DcmElement::scanValue(const OFString& vr,
                          const char* const value,
                          const size_t size)
{
  return vrscan::scan(vr, value, size);
}


unsigned long DcmElement::determineVM(const char *str,
                                      const size_t len)
{
    unsigned long vm = 0;
    // check for non-empty string
    if ((str != NULL) && (len > 0))
    {
        // count number of delimiters (plus 1)
        vm = 1;
        const char *p = str;
        for (size_t i = 0; i < len; i++)
        {
            if (*p++ == '\\')
                vm++;
        }
    }
    return vm;
}


size_t DcmElement::getValueFromString(const char *str,
                                      const size_t pos,
                                      const size_t len,
                                      OFString &val)
{
    size_t newPos = pos;
    // check for non-empty string or invalid start position
    if ((str != NULL) && (len > 0) && (pos < len))
    {
        // start at given position
        const char *p = str + pos;
        // search for next backslash (if any)
        while ((newPos++ < len) && (*p != '\\'))
            p++;
        // extract selected value from string
        val.assign(str + pos, newPos - pos - 1);
    } else
        val.clear();
    return newPos;
}


OFCondition DcmElement::checkVM(const unsigned long vmNum,
                                const OFString &vmStr)
{
  OFCondition result = EC_Normal;
  // only check non-empty values
  if (vmNum > 0)
  {
    if (vmStr == "1")
    {
      if (vmNum != 1) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "1-2")
    {
      if (vmNum > 2) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "1-3")
    {
      if (vmNum > 3) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "1-8")
    {
      if (vmNum > 8) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "1-99")
    {
      if (vmNum > 99) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "2")
    {
      if (vmNum != 2) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "2-4")
    {
      if ((vmNum < 2) || (vmNum > 4)) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "2-n")
    {
      if (vmNum < 2) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "2-2n")
    {
      if ((vmNum % 2) != 0) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "3")
    {
      if (vmNum != 3) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "3-n")
    {
      if (vmNum < 3) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "3-3n")
    {
      if ((vmNum % 3) != 0) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "4")
    {
      if (vmNum != 4) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "4-5")
    {
      if ((vmNum < 4) || (vmNum > 5)) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "4-4n")
    {
      if ((vmNum % 4) != 0) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "5")
    {
      if (vmNum != 5) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "5-n")
    {
      if (vmNum < 5) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "6")
    {
      if (vmNum != 6) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "6-n")
    {
      if (vmNum < 6) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "7")
    {
      if (vmNum != 7) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "7-7n")
    {
      if ((vmNum % 7) != 0) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "8")
    {
      if (vmNum != 8) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "9")
    {
      if (vmNum != 9) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "11")
    {
      if (vmNum != 11) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "16")
    {
      if (vmNum != 16) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "24")
    {
      if (vmNum != 24) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "32")
    {
      if (vmNum != 32) result = EC_ValueMultiplicityViolated;
    }
    else if (vmStr == "256")
    {
      if (vmNum != 256) result = EC_ValueMultiplicityViolated;
    }
    else if ((vmStr != "1-n") && (vmStr != "0-n"))
    {
      // given value of 'vmStr' not (yet) supported
      result = EC_IllegalParameter;
    }
  }
  return result;
}


OFBool DcmElement::isUniversalMatch(const OFBool normalize,
                                    const OFBool enableWildCardMatching)
{
  OFstatic_cast(void,enableWildCardMatching);
  return isEmpty(normalize);
}

OFBool DcmElement::matches(const DcmElement& candidate,
                           const OFBool enableWildCardMatching) const
{
  OFstatic_cast(void,candidate);
  OFstatic_cast(void,enableWildCardMatching);
  return OFFalse;
}


OFBool DcmElement::combinationMatches(const DcmElement& keySecond,
                                      const DcmElement& candidateFirst,
                                      const DcmElement& candidateSecond) const
{
  OFstatic_cast(void,keySecond);
  OFstatic_cast(void,candidateFirst);
  OFstatic_cast(void,candidateSecond);
  return OFFalse;
}
