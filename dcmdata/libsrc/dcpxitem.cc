/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmPixelItem
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcwcache.h"    /* for class DcmWriteCache */


// ********************************


DcmPixelItem::DcmPixelItem(const DcmTag &tag,
                           const Uint32 len)
  : DcmOtherByteOtherWord(tag, len)
{
    setTagVR(EVR_pixelItem);
}


DcmPixelItem::DcmPixelItem(const DcmPixelItem &old)
  : DcmOtherByteOtherWord(old)
{
}


DcmPixelItem &DcmPixelItem::operator=(const DcmPixelItem &obj)
{
  DcmOtherByteOtherWord::operator=(obj);
  return *this;
}


OFCondition DcmPixelItem::copyFrom(const DcmObject &rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmPixelItem &, rhs);
  }
  return EC_Normal;
}


DcmPixelItem::~DcmPixelItem()
{
}


// ********************************


OFBool DcmPixelItem::isNested() const
{
    OFBool nested = OFFalse;
    if (getParent() != NULL)
    {
        // check for surrounding structure of a pixel sequence
        if ((getParent()->ident() == EVR_pixelSQ))
            nested = OFTrue;
    }
    return nested;
}


DcmItem *DcmPixelItem::getParentItem()
{
    DcmItem *parentItem = NULL;
    if (getParent() != NULL)
    {
        // make sure that the direct parent has the correct type
        if (getParent()->ident() == EVR_pixelSQ)
        {
            DcmObject *parent = getParent()->getParent();
            if (parent != NULL)
            {
                // make sure that it is really a class derived from DcmItem
                switch (parent->ident())
                {
                    case EVR_metainfo:
                    case EVR_dataset:
                    case EVR_item:
                    case EVR_dirRecord:
                        parentItem = OFreinterpret_cast(DcmItem *, parent);
                        break;
                    default:
                        DCMDATA_DEBUG("DcmPixelItem::getParentItem() Parent object has wrong class identifier: "
                            << OFstatic_cast(int, parent->ident())
                            << " (" << DcmVR(parent->ident()).getVRName() << ")");
                        break;
                }
            }
        } else {
            DCMDATA_DEBUG("DcmPixelItem::getParentItem() Direct parent object is not a pixel sequence");
        }
    }
    return parentItem;
}


// ********************************


Uint32 DcmPixelItem::calcElementLength(const E_TransferSyntax xfer,
                                       const E_EncodingType enctype)
{
    /* silence unused arguments warnings */
    OFstatic_cast(void, xfer);
    OFstatic_cast(void, enctype);
    /* get length of the pixel data */
    Uint32 valueLength = getLengthField();
    /* make sure the value did not overflow, clamp it otherwise. */
    if (OFStandard::check32BitAddOverflow(valueLength, 8))
      return OFnumeric_limits<Uint32>::max();
    return valueLength + 8;
}


// ********************************


OFCondition DcmPixelItem::writeTagAndLength(DcmOutputStream &outStream,
                                            const E_TransferSyntax oxfer,
                                            Uint32 &writtenBytes) const
{
    OFCondition l_error = outStream.status();
    if (l_error.good())
    {
        /* write tag information */
        l_error = writeTag(outStream, getTag(), oxfer);
        writtenBytes = 4;
        /* prepare to write the value field */
        Uint32 valueLength = getLengthField();
        DcmXfer outXfer(oxfer);
        /* check byte-ordering */
        const E_ByteOrder oByteOrder = outXfer.getByteOrder();
        if (oByteOrder == EBO_unknown)
        {
            return EC_IllegalCall;
        }
        swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
        // availability of four bytes space in output buffer
        // has been checked by caller.
        writtenBytes += OFstatic_cast(Uint32, outStream.write(&valueLength, 4));
    } else
        writtenBytes = 0;
    return l_error;
}


void DcmPixelItem::print(STD_NAMESPACE ostream &out,
                         const size_t flags,
                         const int level,
                         const char *pixelFileName,
                         size_t *pixelCounter)
{
    /* call inherited method */
    printPixel(out, flags, level, pixelFileName, pixelCounter);
}


OFCondition DcmPixelItem::createOffsetTable(const DcmOffsetList &offsetList)
{
    OFCondition result = EC_Normal;

    size_t numEntries = offsetList.size();
    if (numEntries > 0)
    {
        Uint32 current = 0;
        Uint32 *array = new Uint32[numEntries];
        if (array)
        {
            DCMDATA_DEBUG("DcmPixelItem: creating offset table with " << numEntries << " entries");
            OFListConstIterator(Uint32) first = offsetList.begin();
            OFListConstIterator(Uint32) last = offsetList.end();
            unsigned long idx = 0;
            OFBool overflow = OFFalse;
            while ((first != last) && result.good())
            {
                // check for 32-bit unsigned integer overflow (during previous iteration) and report on this
                if (overflow)
                {
                    DCMDATA_WARN("DcmPixelItem: offset value exceeds maximum (32-bit unsigned integer) for frame #"
                        << (idx + 1) << ", cannot create offset table");
                    result = EC_InvalidBasicOffsetTable;
                }
                // check for odd offset values, should never happen at this point (if list was filled by an encoder)
                else if (current & 1)
                {
                    DCMDATA_WARN("DcmPixelItem: odd offset value (" << current << ") for frame #"
                        << (idx + 1) << ", cannot create offset table");
                    result = EC_InvalidBasicOffsetTable;
                } else {
                    // value "current" is proven to be valid
                    array[idx++] = current;
                    // check for 32-bit unsigned integer overflow (but report only during next iteration)
                    overflow = !OFStandard::safeAdd(current, *first, current);
                    ++first;
                }
            }
            if (result.good())
            {
                result = swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, array, OFstatic_cast(Uint32, numEntries * sizeof(Uint32)), sizeof(Uint32));
                if (result.good())
                    result = putUint8Array(OFreinterpret_cast(Uint8 *, array), OFstatic_cast(unsigned long, numEntries * sizeof(Uint32)));
            }
            delete[] array;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DcmPixelItem::writeXML(STD_NAMESPACE ostream &out,
                                   const size_t flags)
{
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* in Native DICOM Model, there is no concept of a "pixel item" */
        return makeOFCondition(OFM_dcmdata, EC_CODE_CannotConvertToXML, OF_error,
            "Cannot convert Pixel Item to Native DICOM Model");
    } else {
        /* XML start tag for "item" */
        out << "<pixel-item";
        /* value length in bytes = 0..max */
        out << " len=\"" << getLengthField() << "\"";
        /* value loaded = no (or absent)*/
        if (!valueLoaded())
            out << " loaded=\"no\"";
        /* pixel item contains binary data */
        if (!(flags & DCMTypes::XF_writeBinaryData))
            out << " binary=\"hidden\"";
        else if (flags & DCMTypes::XF_encodeBase64)
            out << " binary=\"base64\"";
        else
            out << " binary=\"yes\"";
        out << ">";
        /* write element value (if loaded) */
        if (valueLoaded() && (flags & DCMTypes::XF_writeBinaryData))
        {
            /* encode binary data as Base64 */
            if (flags & DCMTypes::XF_encodeBase64)
            {
                /* pixel items always contain 8 bit data, therefore, byte swapping not required */
                OFStandard::encodeBase64(out, OFstatic_cast(Uint8 *, getValue()), OFstatic_cast(size_t, getLengthField()));
            } else {
                /* get and check 8 bit data */
                Uint8 *byteValues = NULL;
                if (getUint8Array(byteValues).good() && (byteValues != NULL))
                {
                    const unsigned long count = getLengthField();
                    out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0');
                    /* print byte values in hex mode */
                    out << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(byteValues++));
                    for (unsigned long i = 1; i < count; i++)
                        out << "\\" << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(byteValues++));
                    /* reset i/o manipulators */
                    out << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
                }
            }
        }
        /* XML end tag for "item" */
        out << "</pixel-item>" << OFendl;
        /* always report success */
        return EC_Normal;
    }
}


OFCondition DcmPixelItem::writeSignatureFormat(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
  if (dcmEnableOldSignatureFormat.get())
  {
      /* Old signature format as created by DCMTK releases previous to 3.5.4.
       * This is non-conformant because it includes the item length in pixel items.
       */
      return DcmOtherByteOtherWord::writeSignatureFormat(outStream, oxfer, enctype, wcache);
  }
  else
  {
      DcmWriteCache wcache2;

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
              /* create an object that represents the transfer syntax */
              DcmXfer outXfer(oxfer);

              /* pointer to element value if value resides in memory or old-style
               * write behaviour is active (i.e. everything loaded into memory upon write
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
                  value = OFstatic_cast(Uint8 *, getValue(outXfer.getByteOrder()));
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
                  wcache->init(this, getLengthField(), getTransferredBytes(), outXfer.getByteOrder());

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
                  /* first compare with DCM_TagInfoLength (12). If there is not enough space
                   * in the buffer, check if the buffer is still sufficient for the requirements
                   * of this element, which may need only 8 instead of 12 bytes.
                   */
                  if (outStream.avail() >= 4)
                  {
                      /* if there is no value, Length (member variable) shall be set to 0 */
                      if (! accessPossible) setLengthField(0);

                      /* write tag and length information (and possibly also data type information) to the stream, */
                      /* mind the transfer syntax and remember the amount of bytes that have been written */
                      errorFlag = writeTag(outStream, getTag(), oxfer);

                      /* if the writing was successful, set this element's transfer */
                      /* state to ERW_inWork and the amount of transferred bytes to 0 */
                      if (errorFlag.good())
                      {
                          setTransferState(ERW_inWork);
                          setTransferredBytes(0);
                      }
                  } else
                      errorFlag = EC_StreamNotifyClient;
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

                      /* increase the amount of bytes which have been transfered correspondingly */
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

                          /* increase the amount of bytes which have been transfered correspondingly */
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
  }

  /* return result value */
  return errorFlag;
}
