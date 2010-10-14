/*
 *
 *  Copyright (C) 2007-2010, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm, Marco Eichelberg, Uli Schlachter
 *
 *  Purpose: codec classes for JPEG-LS decoders.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:24 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djcodecd.h"

#include "dcmtk/ofstd/ofstream.h"    /* for ofstream */
#include "dcmtk/ofstd/ofcast.h"      /* for casts */
#include "dcmtk/ofstd/offile.h"      /* for class OFFile */
#include "dcmtk/ofstd/ofstd.h"       /* for class OFStandard */
#include "dcmtk/dcmdata/dcdatset.h"  /* for class DcmDataset */
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcpixseq.h"  /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcpxitem.h"  /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcvrpobw.h"  /* for class DcmPolymorphOBOW */
#include "dcmtk/dcmdata/dcswap.h"    /* for swapIfNecessary() */
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmGenerateUniqueIdentifer()*/
#include "dcmtk/dcmjpls/djcparam.h"  /* for class DJLSCodecParameter */
#include "djerror.h"                 /* for private class DJLSError */

// JPEG-LS library (CharLS) includes
#include "intrface.h"

E_TransferSyntax DJLSLosslessDecoder::supportedTransferSyntax() const
{
  return EXS_JPEGLSLossless;
}

E_TransferSyntax DJLSNearLosslessDecoder::supportedTransferSyntax() const
{
  return EXS_JPEGLSLossy;
}

// --------------------------------------------------------------------------

DJLSDecoderBase::DJLSDecoderBase()
: DcmCodec()
{
}


DJLSDecoderBase::~DJLSDecoderBase()
{
}


OFBool DJLSDecoderBase::canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const
{
  // this codec only handles conversion from JPEG-LS to uncompressed.

  DcmXfer newRep(newRepType);
  if (newRep.isNotEncapsulated() &&
     ((oldRepType == EXS_JPEGLSLossless)||(oldRepType == EXS_JPEGLSLossy)))
     return OFTrue;

  return OFFalse;
}


OFCondition DJLSDecoderBase::decode(
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * pixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    const DcmCodecParameter * cp,
    const DcmStack& objStack) const
{
  // retrieve pointer to dataset from parameter stack
  DcmStack localStack(objStack);
  (void)localStack.pop();  // pop pixel data element from stack
  DcmObject *dobject = localStack.pop(); // this is the item in which the pixel data is located
  if ((!dobject)||((dobject->ident()!= EVR_dataset) && (dobject->ident()!= EVR_item))) return EC_InvalidTag;
  DcmItem *dataset = OFstatic_cast(DcmItem *, dobject);
  OFBool numberOfFramesPresent = OFFalse;

  // determine properties of uncompressed dataset
  Uint16 imageSamplesPerPixel = 0;
  if (dataset->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel).bad()) return EC_TagNotFound;
  // we only handle one or three samples per pixel
  if ((imageSamplesPerPixel != 3) && (imageSamplesPerPixel != 1)) return EC_InvalidTag;

  Uint16 imageRows = 0;
  if (dataset->findAndGetUint16(DCM_Rows, imageRows).bad()) return EC_TagNotFound;
  if (imageRows < 1) return EC_InvalidTag;

  Uint16 imageColumns = 0;
  if (dataset->findAndGetUint16(DCM_Columns, imageColumns).bad()) return EC_TagNotFound;
  if (imageColumns < 1) return EC_InvalidTag;

  // number of frames is an optional attribute - we don't mind if it isn't present.
  Sint32 imageFrames = 0;
  if (dataset->findAndGetSint32(DCM_NumberOfFrames, imageFrames).good()) numberOfFramesPresent = OFTrue;

  if (imageFrames >= OFstatic_cast(Sint32, pixSeq->card()))
    imageFrames = pixSeq->card() - 1; // limit number of frames to number of pixel items - 1
  if (imageFrames < 1)
    imageFrames = 1; // default in case the number of frames attribute is absent or contains garbage

  Uint16 imageBitsStored = 0;
  if (dataset->findAndGetUint16(DCM_BitsStored, imageBitsStored).bad()) return EC_TagNotFound;

  Uint16 imageBitsAllocated = 0;
  if (dataset->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated).bad()) return EC_TagNotFound;

  Uint16 imageHighBit = 0;
  if (dataset->findAndGetUint16(DCM_HighBit, imageHighBit).bad()) return EC_TagNotFound;

  //we only support up to 16 bits per sample
  if ((imageBitsStored < 1) || (imageBitsStored > 16)) return EC_JLSUnsupportedBitDepth;

  // determine the number of bytes per sample (bits allocated) for the de-compressed object.
  Uint16 bytesPerSample = 1;
  if (imageBitsStored > 8) bytesPerSample = 2;
  else if (imageBitsAllocated > 8) bytesPerSample = 2;

  // compute size of uncompressed frame, in bytes
  Uint32 frameSize = bytesPerSample * imageRows * imageColumns * imageSamplesPerPixel;

  // compute size of pixel data attribute, in bytes
  Uint32 totalSize = frameSize * imageFrames;
  if (totalSize & 1) totalSize++; // align on 16-bit word boundary

  // assume we can cast the codec parameter to what we need
  const DJLSCodecParameter *djcp = OFreinterpret_cast(const DJLSCodecParameter *, cp);

  // determine planar configuration for uncompressed data
  OFString imageSopClass;
  OFString imagePhotometricInterpretation;
  dataset->findAndGetOFString(DCM_SOPClassUID, imageSopClass);
  dataset->findAndGetOFString(DCM_PhotometricInterpretation, imagePhotometricInterpretation);

  // allocate space for uncompressed pixel data element
  Uint16 *pixeldata16 = NULL;
  OFCondition result = uncompressedPixelData.createUint16Array(totalSize/sizeof(Uint16), pixeldata16);
  if (result.bad()) return result;

  Uint8 *pixeldata8 = OFreinterpret_cast(Uint8 *, pixeldata16);
  Sint32 currentFrame = 0;
  Uint32 currentItem = 1; // item 0 contains the offset table
  OFBool done = OFFalse;

  while (result.good() && !done)
  {
      DCMJPLS_INFO("Current Frame Number: " << currentFrame);

      result = decodeFrame(pixSeq, djcp, dataset, currentFrame, currentItem, pixeldata8, frameSize,
          imageFrames, imageColumns, imageRows, imageSamplesPerPixel, bytesPerSample);

      if (result.good())
      {
        // increment frame number, check if we're finished
        if (++currentFrame == imageFrames) done = OFTrue;
        pixeldata8 += frameSize;
      }
  }

  // Number of Frames might have changed in case the previous value was wrong
  if (result.good() && (numberOfFramesPresent || (imageFrames > 1)))
  {
    char numBuf[20];
    sprintf(numBuf, "%ld", OFstatic_cast(long, imageFrames));
    result = ((DcmItem *)dataset)->putAndInsertString(DCM_NumberOfFrames, numBuf);
  }

  if (result.good())
  {
    // the following operations do not affect the Image Pixel Module
    // but other modules such as SOP Common.  We only perform these
    // changes if we're on the main level of the dataset,
    // which should always identify itself as dataset, not as item.
    if ((dataset->ident() == EVR_dataset) && (djcp->getUIDCreation() == EJLSUC_always))
    {
        // create new SOP instance UID
        result = DcmCodec::newInstance((DcmItem *)dataset, NULL, NULL, NULL);
    }
  }

  return result;
}


OFCondition DJLSDecoderBase::decodeFrame(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence *fromPixSeq,
    const DcmCodecParameter *cp,
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32& currentItem,
    void * buffer,
    Uint32 bufSize,
    OFString& decompressedColorModel) const
{
  OFCondition result = EC_Normal;

  // assume we can cast the codec parameter to what we need
  const DJLSCodecParameter *djcp = OFreinterpret_cast(const DJLSCodecParameter *, cp);

  // determine properties of uncompressed dataset
  Uint16 imageSamplesPerPixel = 0;
  if (dataset->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel).bad()) return EC_TagNotFound;
  // we only handle one or three samples per pixel
  if ((imageSamplesPerPixel != 3) && (imageSamplesPerPixel != 1)) return EC_InvalidTag;

  Uint16 imageRows = 0;
  if (dataset->findAndGetUint16(DCM_Rows, imageRows).bad()) return EC_TagNotFound;
  if (imageRows < 1) return EC_InvalidTag;

  Uint16 imageColumns = 0;
  if (dataset->findAndGetUint16(DCM_Columns, imageColumns).bad()) return EC_TagNotFound;
  if (imageColumns < 1) return EC_InvalidTag;

  Uint16 imageBitsStored = 0;
  if (dataset->findAndGetUint16(DCM_BitsStored, imageBitsStored).bad()) return EC_TagNotFound;

  Uint16 imageBitsAllocated = 0;
  if (dataset->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated).bad()) return EC_TagNotFound;

  //we only support up to 16 bits per sample
  if ((imageBitsStored < 1) || (imageBitsStored > 16)) return EC_JLSUnsupportedBitDepth;

  // determine the number of bytes per sample (bits allocated) for the de-compressed object.
  Uint16 bytesPerSample = 1;
  if (imageBitsStored > 8) bytesPerSample = 2;
  else if (imageBitsAllocated > 8) bytesPerSample = 2;

  // number of frames is an optional attribute - we don't mind if it isn't present.
  Sint32 imageFrames = 0;
  dataset->findAndGetSint32(DCM_NumberOfFrames, imageFrames).good();

  if (imageFrames >= OFstatic_cast(Sint32, fromPixSeq->card()))
    imageFrames = fromPixSeq->card() - 1; // limit number of frames to number of pixel items - 1
  if (imageFrames < 1)
    imageFrames = 1; // default in case the number of frames attribute is absent or contains garbage

  // if the user has provided this information, we trust him.
  // If the user has passed a zero, try to find out ourselves.
  if (currentItem == 0)
  {
    result = determineStartFragment(frameNo, imageFrames, fromPixSeq, currentItem);
  }

  if (result.good())
  {
    // We got all the data we need from the dataset, let's start decoding
    DCMJPLS_DEBUG("Starting to decode frame " << frameNo << " with fragment " << currentItem);
    result = decodeFrame(fromPixSeq, djcp, dataset, frameNo, currentItem, buffer, bufSize,
        imageFrames, imageColumns, imageRows, imageSamplesPerPixel, bytesPerSample);
  }

  if (result.good())
  {
    // retrieve color model from given dataset
    result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
  }

  return result;
}

OFCondition DJLSDecoderBase::decodeFrame(
    DcmPixelSequence * fromPixSeq,
    const DJLSCodecParameter *cp,
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32& currentItem,
    void * buffer,
    Uint32 bufSize,
    Sint32 imageFrames,
    Uint16 imageColumns,
    Uint16 imageRows,
    Uint16 imageSamplesPerPixel,
    Uint16 bytesPerSample)
{
  DcmPixelItem *pixItem = NULL;
  Uint8 * jlsData = NULL;
  Uint8 * jlsFragmentData = NULL;
  Uint32 fragmentLength = 0;
  size_t compressedSize = 0;
  Uint32 fragmentsForThisFrame = 0;
  OFCondition result = EC_Normal;
  OFBool ignoreOffsetTable = cp->ignoreOffsetTable();

  // compute the number of JPEG-LS fragments we need in order to decode the next frame
  fragmentsForThisFrame = computeNumberOfFragments(imageFrames, frameNo, currentItem, ignoreOffsetTable, fromPixSeq);
  if (fragmentsForThisFrame == 0) result = EC_JLSCannotComputeNumberOfFragments;

  // determine planar configuration for uncompressed data
  OFString imageSopClass;
  OFString imagePhotometricInterpretation;
  dataset->findAndGetOFString(DCM_SOPClassUID, imageSopClass);
  dataset->findAndGetOFString(DCM_PhotometricInterpretation, imagePhotometricInterpretation);
  Uint16 imagePlanarConfiguration = 0; // 0 is color-by-pixel, 1 is color-by-plane

  if (imageSamplesPerPixel > 1)
  {
    switch (cp->getPlanarConfiguration())
    {
      case EJLSPC_restore:
        // get planar configuration from dataset
        imagePlanarConfiguration = 2; // invalid value
        dataset->findAndGetUint16(DCM_PlanarConfiguration, imagePlanarConfiguration);
        // determine auto default if not found or invalid
        if (imagePlanarConfiguration > 1)
          imagePlanarConfiguration = determinePlanarConfiguration(imageSopClass, imagePhotometricInterpretation);
        break;
      case EJLSPC_auto:
        imagePlanarConfiguration = determinePlanarConfiguration(imageSopClass, imagePhotometricInterpretation);
        break;
      case EJLSPC_colorByPixel:
        imagePlanarConfiguration = 0;
        break;
      case EJLSPC_colorByPlane:
        imagePlanarConfiguration = 1;
        break;
    }
  }

  // get the size of all the fragments
  if (result.good())
  {
    // Don't modify the original values for now
    Uint32 fragmentsForThisFrame2 = fragmentsForThisFrame;
    Uint32 currentItem2 = currentItem;

    while (result.good() && fragmentsForThisFrame2--)
    {
      result = fromPixSeq->getItem(pixItem, currentItem2++);
      if (result.good() && pixItem)
      {
        fragmentLength = pixItem->getLength();
        if (result.good())
          compressedSize += fragmentLength;
      }
    } /* while */
  }

  // get the compressed data
  if (result.good())
  {
    Uint32 offset = 0;
    jlsData = new Uint8[compressedSize];

    while (result.good() && fragmentsForThisFrame--)
    {
      result = fromPixSeq->getItem(pixItem, currentItem++);
      if (result.good() && pixItem)
      {
        fragmentLength = pixItem->getLength();
        result = pixItem->getUint8Array(jlsFragmentData);
        if (result.good() && jlsFragmentData)
        {
          memcpy(&jlsData[offset], jlsFragmentData, fragmentLength);
          offset += fragmentLength;
        }
      }
    } /* while */
  }

  if (result.good())
  {
    JlsParameters params;
    JLS_ERROR err;

    err = JpegLsReadHeader(jlsData, compressedSize, &params);
    result = DJLSError::convert(err);

    if (result.good())
    {
      if (params.width != imageColumns) result = EC_JLSImageDataMismatch;
      else if (params.height != imageRows) result = EC_JLSImageDataMismatch;
      else if (params.components != imageSamplesPerPixel) result = EC_JLSImageDataMismatch;
      else if ((bytesPerSample == 1) && (params.bitspersample > 8)) result = EC_JLSImageDataMismatch;
      else if ((bytesPerSample == 2) && (params.bitspersample <= 8)) result = EC_JLSImageDataMismatch;
    }

    if (!result.good())
    {
      delete[] jlsData;
    }
    else
    {
      err = JpegLsDecode(buffer, bufSize, jlsData, compressedSize, &params);
      result = DJLSError::convert(err);
      delete[] jlsData;

      if (result.good() && imageSamplesPerPixel == 3)
      {
        if (imagePlanarConfiguration == 1 && params.ilv != ILV_NONE)
        {
          // The dataset says this should be planarConfiguration == 1, but
          // it isn't -> convert it.
          DCMJPLS_WARN("different planar configuration in JPEG stream, converting to \"1\"");
          if (bytesPerSample == 1)
            result = createPlanarConfiguration1Byte(OFreinterpret_cast(Uint8*, buffer), imageColumns, imageRows);
          else
            result = createPlanarConfiguration1Word(OFreinterpret_cast(Uint16*, buffer), imageColumns, imageRows);
        }
        else if (imagePlanarConfiguration == 0 && params.ilv != ILV_SAMPLE && params.ilv != ILV_LINE)
        {
          // The dataset says this should be planarConfiguration == 0, but
          // it isn't -> convert it.
          DCMJPLS_WARN("different planar configuration in JPEG stream, converting to \"0\"");
          if (bytesPerSample == 1)
            result = createPlanarConfiguration0Byte(OFreinterpret_cast(Uint8*, buffer), imageColumns, imageRows);
          else
            result = createPlanarConfiguration0Word(OFreinterpret_cast(Uint16*, buffer), imageColumns, imageRows);
        }
      }

      if (result.good())
      {
          // decompression is complete, finally adjust byte order if necessary
          if (bytesPerSample == 1) // we're writing bytes into words
          {
              result = swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, buffer,
                      bufSize, sizeof(Uint16));
          }
      }
    }
  }

  return result;
}


OFCondition DJLSDecoderBase::encode(
    const Uint16 * /* pixelData */,
    const Uint32 /* length */,
    const DcmRepresentationParameter * /* toRepParam */,
    DcmPixelSequence * & /* pixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmStack & /* objStack */) const
{
  // we are a decoder only
  return EC_IllegalCall;
}


OFCondition DJLSDecoderBase::encode(
    const E_TransferSyntax /* fromRepType */,
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmRepresentationParameter * /* toRepParam */,
    DcmPixelSequence * & /* toPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmStack & /* objStack */) const
{
  // we don't support re-coding for now.
  return EC_IllegalCall;
}


OFCondition DJLSDecoderBase::determineDecompressedColorModel(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmItem * dataset,
    OFString & decompressedColorModel) const
{
  OFCondition result = EC_IllegalParameter;
  if (dataset != NULL)
  {
    // retrieve color model from given dataset
    result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
  }
  return result;
}


Uint16 DJLSDecoderBase::determinePlanarConfiguration(
  const OFString& sopClassUID,
  const OFString& photometricInterpretation)
{
  // Hardcopy Color Image always requires color-by-plane
  if (sopClassUID == UID_RETIRED_HardcopyColorImageStorage) return 1;

  // The 1996 Ultrasound Image IODs require color-by-plane if color model is YBR_FULL.
  if (photometricInterpretation == "YBR_FULL")
  {
    if ((sopClassUID == UID_UltrasoundMultiframeImageStorage)
       ||(sopClassUID == UID_UltrasoundImageStorage)) return 1;
  }

  // default for all other cases
  return 0;
}

Uint32 DJLSDecoderBase::computeNumberOfFragments(
  Sint32 numberOfFrames,
  Uint32 currentFrame,
  Uint32 startItem,
  OFBool ignoreOffsetTable,
  DcmPixelSequence * pixSeq)
{

  unsigned long numItems = pixSeq->card();
  DcmPixelItem *pixItem = NULL;

  // We first check the simple cases, that is, a single-frame image,
  // the last frame of a multi-frame image and the standard case where we do have
  // a single fragment per frame.
  if ((numberOfFrames <= 1) || (currentFrame + 1 == OFstatic_cast(Uint32, numberOfFrames)))
  {
    // single-frame image or last frame. All remaining fragments belong to this frame
    return (numItems - startItem);
  }
  if (OFstatic_cast(Uint32, numberOfFrames + 1) == numItems)
  {
    // multi-frame image with one fragment per frame
    return 1;
  }

  OFCondition result = EC_Normal;
  if (! ignoreOffsetTable)
  {
    // We do have a multi-frame image with multiple fragments per frame, and we are
    // not working on the last frame. Let's check the offset table if present.
    result = pixSeq->getItem(pixItem, 0);
    if (result.good() && pixItem)
    {
      Uint32 offsetTableLength = pixItem->getLength();
      if (offsetTableLength == (OFstatic_cast(Uint32, numberOfFrames) * 4))
      {
        // offset table is non-empty and contains one entry per frame
        Uint8 *offsetData = NULL;
        result = pixItem->getUint8Array(offsetData);
        if (result.good() && offsetData)
        {
          // now we can access the offset table
          Uint32 *offsetData32 = OFreinterpret_cast(Uint32 *, offsetData);

          // extract the offset for the NEXT frame. This offset is guaranteed to exist
          // because the "last frame/single frame" case is handled above.
          Uint32 offset = offsetData32[currentFrame+1];

          // convert to local endian byte order (always little endian in file)
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, &offset, sizeof(Uint32), sizeof(Uint32));

          // determine index of start fragment for next frame
          Uint32 byteCount = 0;
          Uint32 fragmentIndex = 1;
          while ((byteCount < offset) && (fragmentIndex < numItems))
          {
             pixItem = NULL;
             result = pixSeq->getItem(pixItem, fragmentIndex++);
             if (result.good() && pixItem)
             {
               byteCount += pixItem->getLength() + 8; // add 8 bytes for item tag and length
               if ((byteCount == offset) && (fragmentIndex > startItem))
               {
                 // bingo, we have found the offset for the next frame
                 return fragmentIndex - startItem;
               }
             }
             else break; /* something went wrong, break out of while loop */
          } /* while */
        }
      }
    }
  }

  // So we have a multi-frame image with multiple fragments per frame and the
  // offset table is empty or wrong. Our last chance is to peek into the JPEG-LS
  // bistream and identify the start of the next frame.
  Uint32 nextItem = startItem;
  Uint8 *fragmentData = NULL;
  while (++nextItem < numItems)
  {
    pixItem = NULL;
    result = pixSeq->getItem(pixItem, nextItem);
    if (result.good() && pixItem)
    {
        fragmentData = NULL;
        result = pixItem->getUint8Array(fragmentData);
        if (result.good() && fragmentData && (pixItem->getLength() > 3))
        {
          if (isJPEGLSStartOfImage(fragmentData))
          {
            // found a JPEG-LS SOI marker. Assume that this is the start of the next frame.
            return (nextItem - startItem);
          }
        }
        else break; /* something went wrong, break out of while loop */
    }
    else break; /* something went wrong, break out of while loop */
  }

  // We're bust. No way to determine the number of fragments per frame.
  return 0;
}


OFBool DJLSDecoderBase::isJPEGLSStartOfImage(Uint8 *fragmentData)
{
  // A valid JPEG-LS bitstream will always start with an SOI marker FFD8, followed
  // by either an SOF55 (FFF7), COM (FFFE) or APPn (FFE0-FFEF) marker.
  if ((*fragmentData++) != 0xFF) return OFFalse;
  if ((*fragmentData++) != 0xD8) return OFFalse;
  if ((*fragmentData++) != 0xFF) return OFFalse;
  if ((*fragmentData == 0xF7)||(*fragmentData == 0xFE)||((*fragmentData & 0xF0) == 0xE0))
  {
    return OFTrue;
  }
  return OFFalse;
}


OFCondition DJLSDecoderBase::createPlanarConfiguration1Byte(
  Uint8 *imageFrame,
  Uint16 columns,
  Uint16 rows)
{
  if (imageFrame == NULL) return EC_IllegalCall;

  unsigned long numPixels = columns * rows;
  if (numPixels == 0) return EC_IllegalCall;

  Uint8 *buf = new Uint8[3*numPixels + 3];
  if (buf)
  {
    memcpy(buf, imageFrame, (size_t)(3*numPixels));
    register Uint8 *s = buf;                        // source
    register Uint8 *r = imageFrame;                 // red plane
    register Uint8 *g = imageFrame + numPixels;     // green plane
    register Uint8 *b = imageFrame + (2*numPixels); // blue plane
    for (register unsigned long i=numPixels; i; i--)
    {
      *r++ = *s++;
      *g++ = *s++;
      *b++ = *s++;
    }
    delete[] buf;
  } else return EC_MemoryExhausted;
  return EC_Normal;
}


OFCondition DJLSDecoderBase::createPlanarConfiguration1Word(
  Uint16 *imageFrame,
  Uint16 columns,
  Uint16 rows)
{
  if (imageFrame == NULL) return EC_IllegalCall;

  unsigned long numPixels = columns * rows;
  if (numPixels == 0) return EC_IllegalCall;

  Uint16 *buf = new Uint16[3*numPixels + 3];
  if (buf)
  {
    memcpy(buf, imageFrame, (size_t)(3*numPixels*sizeof(Uint16)));
    register Uint16 *s = buf;                        // source
    register Uint16 *r = imageFrame;                 // red plane
    register Uint16 *g = imageFrame + numPixels;     // green plane
    register Uint16 *b = imageFrame + (2*numPixels); // blue plane
    for (register unsigned long i=numPixels; i; i--)
    {
      *r++ = *s++;
      *g++ = *s++;
      *b++ = *s++;
    }
    delete[] buf;
  } else return EC_MemoryExhausted;
  return EC_Normal;
}

OFCondition DJLSDecoderBase::createPlanarConfiguration0Byte(
  Uint8 *imageFrame,
  Uint16 columns,
  Uint16 rows)
{
  if (imageFrame == NULL) return EC_IllegalCall;

  unsigned long numPixels = columns * rows;
  if (numPixels == 0) return EC_IllegalCall;

  Uint8 *buf = new Uint8[3*numPixels + 3];
  if (buf)
  {
    memcpy(buf, imageFrame, (size_t)(3*numPixels));
    register Uint8 *t = imageFrame;          // target
    register Uint8 *r = buf;                 // red plane
    register Uint8 *g = buf + numPixels;     // green plane
    register Uint8 *b = buf + (2*numPixels); // blue plane
    for (register unsigned long i=numPixels; i; i--)
    {
      *t++ = *r++;
      *t++ = *g++;
      *t++ = *b++;
    }
    delete[] buf;
  } else return EC_MemoryExhausted;
  return EC_Normal;
}


OFCondition DJLSDecoderBase::createPlanarConfiguration0Word(
  Uint16 *imageFrame,
  Uint16 columns,
  Uint16 rows)
{
  if (imageFrame == NULL) return EC_IllegalCall;

  unsigned long numPixels = columns * rows;
  if (numPixels == 0) return EC_IllegalCall;

  Uint16 *buf = new Uint16[3*numPixels + 3];
  if (buf)
  {
    memcpy(buf, imageFrame, (size_t)(3*numPixels*sizeof(Uint16)));
    register Uint16 *t = imageFrame;          // target
    register Uint16 *r = buf;                 // red plane
    register Uint16 *g = buf + numPixels;     // green plane
    register Uint16 *b = buf + (2*numPixels); // blue plane
    for (register unsigned long i=numPixels; i; i--)
    {
      *t++ = *r++;
      *t++ = *g++;
      *t++ = *b++;
    }
    delete[] buf;
  } else return EC_MemoryExhausted;
  return EC_Normal;
}

/*
 * CVS/RCS Log:
 * $Log: djcodecd.cc,v $
 * Revision 1.15  2010-10-14 13:14:24  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2010-10-12 12:32:22  uli
 * Avoid redundant findAndGet*() calls.
 *
 * Revision 1.13  2010-10-12 10:17:32  uli
 * Added working implementation of DJLSDecoderBase::decodeFrame().
 *
 * Revision 1.12  2010-10-05 10:15:19  uli
 * Fixed all remaining warnings from -Wall -Wextra -pedantic.
 *
 * Revision 1.11  2010-10-05 08:25:41  uli
 * Update dcmjpls to newest CharLS snapshot.
 *
 * Revision 1.10  2010-09-24 13:40:18  joergr
 * Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 * resulting name changes are mainly caused by the fact that the corresponding
 * SOP Class is now retired.
 *
 * Revision 1.9  2010-08-19 08:04:29  uli
 * Removed unused variables.
 *
 * Revision 1.8  2010-01-19 15:19:06  uli
 * Made file names fit into 8.3 format.
 *
 * Revision 1.7  2009-11-25 13:56:33  joergr
 * Removed unused implementation of method determineDecompressedColorModel().
 *
 * Revision 1.6  2009-11-17 16:56:35  joergr
 * Added new method that allows for determining the color model of the
 * decompressed image.
 *
 * Revision 1.5  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.4  2009-09-08 11:19:45  meichel
 * Fixed bug affecting decompression of images with 8 bits/sample on big
 *   endian machines
 *
 * Revision 1.3  2009-08-10 09:38:06  meichel
 * All decompression codecs now replace NumberOfFrames if larger than one
 *   or present in the original image.
 *
 * Revision 1.2  2009-07-31 09:05:43  meichel
 * Added more detailed error messages, minor code clean-up
 *
 * Revision 1.1  2009-07-29 14:46:47  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.8  2008-08-15 09:19:57  meichel
 * Decoder now gracefully handles the case of faulty images where value of
 *   NumberOfFrames is larger than the number of compressed fragments, if and only
 *   if there is just a single fragment per frame.
 *
 * Revision 1.7  2008-05-29 10:54:05  meichel
 * Implemented new method DcmPixelData::getUncompressedFrame
 *   that permits frame-wise access to compressed and uncompressed
 *   objects without ever loading the complete object into main memory.
 *   For this new method to work with compressed images, all classes derived from
 *   DcmCodec need to implement a new method decodeFrame(). For now, only
 *   dummy implementations returning an error code have been defined.
 *
 * Revision 1.6  2007/06/20 12:37:23  meichel
 * Fixed include files and dependencies
 *
 * Revision 1.5  2007/06/15 14:35:45  meichel
 * Renamed CMake project and include directory from dcmjpgls to dcmjpls
 *
 * Revision 1.4  2007/06/15 10:39:15  meichel
 * Completed implementation of decoder, which now correctly processes all
 *   of the NEMA JPEG-LS sample images, including fragmented frames.
 *
 * Revision 1.3  2007/06/14 12:36:14  meichel
 * Further code clean-up. Updated doxygen comments.
 *
 * Revision 1.2  2007/06/13 16:41:07  meichel
 * Code clean-up and removal of dead code
 *
 *
 */
