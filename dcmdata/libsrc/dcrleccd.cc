/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: decoder codec class for RLE
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 10:42:44 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrleccd.h"

// dcmdata includes
#include "dcmtk/dcmdata/dcrlecp.h"   /* for class DcmRLECodecParameter */
#include "dcmtk/dcmdata/dcrledec.h"  /* for class DcmRLEDecoder */
#include "dcmtk/dcmdata/dcdatset.h"  /* for class DcmDataset */
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcpixseq.h"  /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcpxitem.h"  /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcvrpobw.h"  /* for class DcmPolymorphOBOW */
#include "dcmtk/dcmdata/dcswap.h"    /* for swapIfNecessary() */
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmGenerateUniqueIdentifer()*/


DcmRLECodecDecoder::DcmRLECodecDecoder()
: DcmCodec()
{
}


DcmRLECodecDecoder::~DcmRLECodecDecoder()
{
}


OFBool DcmRLECodecDecoder::canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const
{
  E_TransferSyntax myXfer = EXS_RLELossless;
  DcmXfer newRep(newRepType);
  if (newRep.isNotEncapsulated() && (oldRepType == myXfer)) return OFTrue; // decompress requested

  // we don't support re-coding for now.
  return OFFalse;
}


OFCondition DcmRLECodecDecoder::decode(
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * pixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    const DcmCodecParameter * cp,
    const DcmStack& objStack) const
{
  OFCondition result = EC_Normal;

  // assume we can cast the codec parameter to what we need
  const DcmRLECodecParameter *djcp = OFstatic_cast(const DcmRLECodecParameter *, cp);

  OFBool enableReverseByteOrder = djcp->getReverseDecompressionByteOrder();

  DcmStack localStack(objStack);
  (void)localStack.pop();             // pop pixel data element from stack
  DcmObject *dataset = localStack.pop(); // this is the item in which the pixel data is located
  if ((!dataset)||((dataset->ident()!= EVR_dataset) && (dataset->ident()!= EVR_item))) result = EC_InvalidTag;
  else
  {
    Uint16 imageSamplesPerPixel = 0;
    Uint16 imageRows = 0;
    Uint16 imageColumns = 0;
    Sint32 imageFrames = 1;
    Uint16 imageBitsAllocated = 0;
    Uint16 imageBytesAllocated = 0;
    Uint16 imagePlanarConfiguration = 0;
    Uint32 rleHeader[16];
    DcmItem *ditem = OFstatic_cast(DcmItem *, dataset);
    OFBool numberOfFramesPresent = OFFalse;

    if (result.good()) result = ditem->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel);
    if (result.good()) result = ditem->findAndGetUint16(DCM_Rows, imageRows);
    if (result.good()) result = ditem->findAndGetUint16(DCM_Columns, imageColumns);
    if (result.good()) result = ditem->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated);
    if (result.good())
    {
      imageBytesAllocated = OFstatic_cast(Uint16, imageBitsAllocated / 8);
      if ((imageBitsAllocated < 8)||(imageBitsAllocated % 8 != 0)) result = EC_CannotChangeRepresentation;
    }
    if (result.good() && (imageSamplesPerPixel > 1))
    {
      result = ditem->findAndGetUint16(DCM_PlanarConfiguration, imagePlanarConfiguration);
    }

    // number of frames is an optional attribute - we don't mind if it isn't present.
    if (result.good())
    {
      if (ditem->findAndGetSint32(DCM_NumberOfFrames, imageFrames).good()) numberOfFramesPresent = OFTrue;
    }

    if (imageFrames >= OFstatic_cast(Sint32, pixSeq->card()))
      imageFrames = pixSeq->card() - 1; // limit number of frames to number of pixel items - 1
    if (imageFrames < 1)
      imageFrames = 1; // default in case the number of frames attribute is absent or contains garbage

    if (result.good())
    {
      DcmPixelItem *pixItem = NULL;
      Uint8 * rleData = NULL;
      const size_t bytesPerStripe = imageColumns * imageRows;

      DcmRLEDecoder rledecoder(bytesPerStripe);
      if (rledecoder.fail()) result = EC_MemoryExhausted;  // RLE decoder failed to initialize
      else
      {
        Uint32 frameSize = imageBytesAllocated * imageRows * imageColumns * imageSamplesPerPixel;
        Uint32 totalSize = frameSize * imageFrames;
        if (totalSize & 1) totalSize++; // align on 16-bit word boundary
        Uint16 *imageData16 = NULL;
        Sint32 currentFrame = 0;
        Uint32 currentItem = 1; // ignore offset table
        Uint32 numberOfStripes = 0;
        Uint32 fragmentLength = 0;
        Uint32 i;

        result = uncompressedPixelData.createUint16Array(totalSize/sizeof(Uint16), imageData16);
        if (result.good())
        {
          Uint8 *imageData8 = OFreinterpret_cast(Uint8 *, imageData16);

          while ((currentFrame < imageFrames) && result.good())
          {
            DCMDATA_DEBUG("RLE decoder processes frame " << currentFrame);
            DCMDATA_DEBUG("RLE decoder processes pixel item " << currentItem);
            // get first pixel item of this frame
            result = pixSeq->getItem(pixItem, currentItem++);
            if (result.good())
            {
              fragmentLength = pixItem->getLength();
              result = pixItem->getUint8Array(rleData);
              if (result.good())
              {
                // we require that the RLE header must be completely
                // contained in the first fragment; otherwise bail out
                if (fragmentLength < 64) result = EC_CannotChangeRepresentation;
              }
            }

            if (result.good())
            {
              // copy RLE header to buffer and adjust byte order
              memcpy(rleHeader, rleData, 64);
              swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rleHeader, 16*sizeof(Uint32), sizeof(Uint32));

              // determine number of stripes.
              numberOfStripes = rleHeader[0];

              // check that number of stripes in RLE header matches our expectation
              if ((numberOfStripes < 1) || (numberOfStripes > 15) ||
                  (numberOfStripes != OFstatic_cast(Uint32, imageBytesAllocated) * imageSamplesPerPixel))
                  result = EC_CannotChangeRepresentation;
            }

            if (result.good())
            {
              // this variable keeps the number of bytes we have processed
              // for the current frame in earlier pixel fragments
              Uint32 fragmentOffset = 0;

              // this variable keeps the current position within the current fragment
              Uint32 byteOffset = 0;

              OFBool lastStripe = OFFalse;
              Uint32 inputBytes = 0;

              // pointers for buffer copy operations
              Uint8 *outputBuffer = NULL;
              Uint8 *pixelPointer = NULL;

              // byte offset for first sample in frame
              Uint32 sampleOffset = 0;

              // byte offset between samples
              Uint32 offsetBetweenSamples = 0;

              // temporary variables
              Uint32 sample = 0;
              Uint32 byte = 0;
              register Uint32 pixel = 0;

              // for each stripe in stripe set
              for (i=0; (i<numberOfStripes) && result.good(); ++i)
              {
                // reset RLE codec
                rledecoder.clear();

                // adjust start point for RLE stripe, ignoring trailing garbage from the last run
                byteOffset = rleHeader[i+1];
                if (byteOffset < fragmentOffset) result = EC_CannotChangeRepresentation;
                else
                {
                  byteOffset -= fragmentOffset; // now byteOffset is correct but may point to next fragment
                  while ((byteOffset > fragmentLength) && result.good())
                  {
                    DCMDATA_DEBUG("RLE decoder processes pixel item " << currentItem);
                    result = pixSeq->getItem(pixItem, currentItem++);
                    if (result.good())
                    {
                      byteOffset -= fragmentLength;
                      fragmentOffset += fragmentLength;
                      fragmentLength = pixItem->getLength();
                      result = pixItem->getUint8Array(rleData);
                    }
                  }
                }

                // byteOffset now points to the first byte of the new RLE stripe
                // check if the current stripe is the last one for this frame
                if (i+1 == numberOfStripes) lastStripe = OFTrue; else lastStripe = OFFalse;

                if (lastStripe)
                {
                  // the last stripe needs special handling because we cannot use the
                  // offset table to determine the number of bytes to feed to the codec
                  // if the RLE data is split in multiple fragments. We need to feed
                  // data fragment by fragment until the RLE codec has produced
                  // sufficient output.
                  while ((rledecoder.size() < bytesPerStripe) && result.good())
                  {
                    // feed complete remaining content of fragment to RLE codec and
                    // switch to next fragment
                    result = rledecoder.decompress(rleData + byteOffset, OFstatic_cast(size_t, fragmentLength - byteOffset));

                    // special handling for zero pad byte at the end of the RLE stream
                    // which results in an EC_StreamNotifyClient return code
                    // or trailing garbage data which results in EC_CorruptedData
                    if (rledecoder.size() == bytesPerStripe) result = EC_Normal;

                    // Check if we're already done. If yes, don't change fragment
                    if (result.good() || result == EC_StreamNotifyClient)
                    {
                      if (rledecoder.size() < bytesPerStripe)
                      {
                        DCMDATA_WARN("RLE decoder is finished but has produced insufficient data for this stripe, will continue with next pixel item");
                        DCMDATA_DEBUG("RLE decoder processes pixel item " << currentItem);
                        result = pixSeq->getItem(pixItem, currentItem++);
                        if (result.good())
                        {
                          byteOffset = 0;
                          fragmentOffset += fragmentLength;
                          fragmentLength = pixItem->getLength();
                          result = pixItem->getUint8Array(rleData);
                        }
                      }
                      else byteOffset = fragmentLength;
                    }
                  } /* while */
                }
                else
                {
                  // not the last stripe. We can use the offset table to determine
                  // the number of bytes to feed to the RLE codec.
                  inputBytes = rleHeader[i+2];
                  if (inputBytes < rleHeader[i+1]) result = EC_CannotChangeRepresentation;
                  else
                  {
                    inputBytes -= rleHeader[i+1]; // number of bytes to feed to codec
                    while ((inputBytes > (fragmentLength - byteOffset)) && result.good())
                    {
                      // feed complete remaining content of fragment to RLE codec and
                      // switch to next fragment
                      result = rledecoder.decompress(rleData + byteOffset, OFstatic_cast(size_t, fragmentLength - byteOffset));

                      if (result.good() || result == EC_StreamNotifyClient)
                      {
                        DCMDATA_DEBUG("RLE decoder processes pixel item " << currentItem);
                        result = pixSeq->getItem(pixItem, currentItem++);
                      }
                      if (result.good())
                      {
                        inputBytes -= fragmentLength - byteOffset;
                        byteOffset = 0;
                        fragmentOffset += fragmentLength;
                        fragmentLength = pixItem->getLength();
                        result = pixItem->getUint8Array(rleData);
                      }
                    } /* while */

                    // last fragment for this RLE stripe
                    result = rledecoder.decompress(rleData + byteOffset, OFstatic_cast(size_t, inputBytes));

                    // special handling for zero pad byte at the end of the RLE stream
                    // which results in an EC_StreamNotifyClient return code
                    // or trailing garbage data which results in EC_CorruptedData
                    if (rledecoder.size() == bytesPerStripe) result = EC_Normal;

                    byteOffset += inputBytes;
                  }
                }

                // make sure the RLE decoder has produced the right amount of data
                if (result.good() && (rledecoder.size() != bytesPerStripe))
                {
                    DCMDATA_ERROR("RLE decoder is finished but has produced insufficient data for this stripe");
                    result = EC_CannotChangeRepresentation;
                }

                // distribute decompressed bytes into output image array
                if (result.good())
                {
                  // which sample and byte are we currently compressing?
                  sample = i / imageBytesAllocated;
                  byte = i % imageBytesAllocated;

                  // raw buffer containing bytesPerStripe bytes of uncompressed data
                  outputBuffer = OFstatic_cast(Uint8 *, rledecoder.getOutputBuffer());

                  // compute byte offsets
                  if (imagePlanarConfiguration == 0)
                  {
                     sampleOffset = sample * imageBytesAllocated;
                     offsetBetweenSamples = imageSamplesPerPixel * imageBytesAllocated;
                  }
                  else
                  {
                     sampleOffset = sample * imageBytesAllocated * imageColumns * imageRows;
                     offsetBetweenSamples = imageBytesAllocated;
                  }

                  // initialize pointer to output data
                  if (enableReverseByteOrder)
                  {
                    // assume incorrect LSB to MSB order of RLE segments as produced by some tools
                    pixelPointer = imageData8 + sampleOffset + byte;
                  }
                  else
                  {
                    pixelPointer = imageData8 + sampleOffset + imageBytesAllocated - byte - 1;
                  }

                  // loop through all pixels of the frame
                  for (pixel = 0; pixel < bytesPerStripe; ++pixel)
                  {
                    *pixelPointer = *outputBuffer++;
                    pixelPointer += offsetBetweenSamples;
                  }
                }
              } /* for */
            }

            // advance by one frame
            if (result.good())
            {
              currentFrame++;
              imageData8 += frameSize;
            }

          } /* while still frames to process */

          // adjust byte order for uncompressed image to little endian
          swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, imageData16, totalSize, sizeof(Uint16));

          // Number of Frames might have changed in case the previous value was wrong
          if (result.good() && (numberOfFramesPresent || (imageFrames > 1)))
          {
            char numBuf[20];
            sprintf(numBuf, "%ld", OFstatic_cast(long, imageFrames));
            result = OFstatic_cast(DcmItem *, dataset)->putAndInsertString(DCM_NumberOfFrames, numBuf);
          }
        }
      }
    }

    // the following operations do not affect the Image Pixel Module
    // but other modules such as SOP Common.  We only perform these
    // changes if we're on the main level of the dataset,
    // which should always identify itself as dataset, not as item.
    if (dataset->ident() == EVR_dataset)
    {
        // create new SOP instance UID if codec parameters require so
        if (result.good() && djcp->getUIDCreation()) result =
          DcmCodec::newInstance(OFstatic_cast(DcmItem *, dataset), NULL, NULL, NULL);
    }
  }
  return result;
}


OFCondition DcmRLECodecDecoder::decodeFrame(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence * fromPixSeq,
    const DcmCodecParameter * cp,
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32& startFragment,
    void *buffer,
    Uint32 bufSize,
    OFString& decompressedColorModel) const
{
    OFCondition result = EC_Normal;

    // assume we can cast the codec parameter to what we need
    const DcmRLECodecParameter *djcp = OFstatic_cast(const DcmRLECodecParameter *, cp);

    OFBool enableReverseByteOrder = djcp->getReverseDecompressionByteOrder();

    if ((!dataset)||((dataset->ident()!= EVR_dataset) && (dataset->ident()!= EVR_item))) return EC_InvalidTag;

    Uint16 imageSamplesPerPixel = 0;
    Uint16 imageRows = 0;
    Uint16 imageColumns = 0;
    Sint32 imageFrames = 1;
    Uint16 imageBitsAllocated = 0;
    Uint16 imageBytesAllocated = 0;
    Uint16 imagePlanarConfiguration = 0;
    Uint32 rleHeader[16];
    OFString photometricInterpretation;
    DcmItem *ditem = OFstatic_cast(DcmItem *, dataset);

    if (result.good()) result = ditem->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel);
    if (result.good()) result = ditem->findAndGetUint16(DCM_Rows, imageRows);
    if (result.good()) result = ditem->findAndGetUint16(DCM_Columns, imageColumns);
    if (result.good()) result = ditem->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated);
    if (result.good()) result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation);
    if (result.good())
    {
        imageBytesAllocated = OFstatic_cast(Uint16, imageBitsAllocated / 8);
        if ((imageBitsAllocated < 8)||(imageBitsAllocated % 8 != 0)) return EC_CannotChangeRepresentation;
    }
    if (result.good() && (imageSamplesPerPixel > 1))
    {
        result = ditem->findAndGetUint16(DCM_PlanarConfiguration, imagePlanarConfiguration);
    }

    // number of frames is an optional attribute - we don't mind if it isn't present.
    if (result.good()) (void) ditem->findAndGetSint32(DCM_NumberOfFrames, imageFrames);
    if (imageFrames < 1) imageFrames = 1; // default in case this attribute contains garbage

    if (result.bad())
       return result;

    DcmPixelItem *pixItem = NULL;
    Uint8 * rleData = NULL;
    const size_t bytesPerStripe = imageColumns * imageRows;
    Uint32 numberOfStripes = 0;
    Uint32 fragmentLength = 0;
    Uint32 i;
    Uint32 frameSize = imageBytesAllocated * imageRows * imageColumns * imageSamplesPerPixel;

    if (frameSize > bufSize) return EC_IllegalCall;

    DcmRLEDecoder rledecoder(bytesPerStripe);
    if (rledecoder.fail()) return EC_MemoryExhausted;  // RLE decoder failed to initialize

    DCMDATA_DEBUG("RLE decoder processes frame " << frameNo);

    // determine the corresponding item (first fragment) for this frame
    Uint32 currentItem = startFragment;

    // if the user has provided this information, we trust him.
    // If the user has passed a zero, try to find out ourselves.
    if (currentItem == 0 && result.good())
    {
        result = determineStartFragment(frameNo, imageFrames, fromPixSeq, currentItem);
        if (result.bad())
            return result;
    }

    DCMDATA_DEBUG("RLE decoder processes pixel item " << currentItem);
    // now access and decompress the frame starting at the item we have identified
    result = fromPixSeq->getItem(pixItem, currentItem);
    if (result.bad())
       return result;

    fragmentLength = pixItem->getLength();
    result = pixItem->getUint8Array(rleData);
    if (result.bad())
       return result;

    // copy RLE header to buffer and adjust byte order
    memcpy(rleHeader, rleData, 64);
    swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rleHeader, 16*sizeof(Uint32), sizeof(Uint32));

    // determine number of stripes.
    numberOfStripes = rleHeader[0];

    // check that number of stripes in RLE header matches our expectation
    if ((numberOfStripes < 1) || (numberOfStripes > 15) || (numberOfStripes != OFstatic_cast(Uint32, imageBytesAllocated) * imageSamplesPerPixel))
        return EC_CannotChangeRepresentation;

    // this variable keeps the current position within the current fragment
    Uint32 byteOffset = 0;

    OFBool lastStripe = OFFalse;
    Uint32 inputBytes = 0;

    // pointers for buffer copy operations
    Uint8 *outputBuffer = NULL;
    Uint8 *pixelPointer = NULL;
    Uint16 *imageData16 = OFreinterpret_cast(Uint16 *, buffer);
    Uint8 *imageData8 = OFreinterpret_cast(Uint8 *, buffer);

    // byte offset for first sample in frame
    Uint32 sampleOffset = 0;

    // byte offset between samples
    Uint32 offsetBetweenSamples = 0;

    // temporary variables
    Uint32 sample = 0;
    Uint32 byte = 0;
    register Uint32 pixel = 0;
    size_t bytesToDecode;

    // for each stripe in stripe set
    for (i = 0; i < numberOfStripes; ++i)
    {
        // reset RLE codec
        rledecoder.clear();

        // adjust start point for RLE stripe
        byteOffset = rleHeader[i+1];

        // byteOffset now points to the first byte of the new RLE stripe
        // check if the current stripe is the last one for this frame
        if (i+1 == numberOfStripes) lastStripe = OFTrue; else lastStripe = OFFalse;

        if (lastStripe)
        {
            // the last stripe needs special handling because we cannot use the
            // offset table to determine the number of bytes to feed to the codec
            // if the RLE data is split in multiple fragments. We need to feed
            // data fragment by fragment until the RLE codec has produced
            // sufficient output.
            bytesToDecode = OFstatic_cast(size_t, fragmentLength - byteOffset);
        }
        else
        {
            // not the last stripe. We can use the offset table to determine
            // the number of bytes to feed to the RLE codec.
            inputBytes = rleHeader[i+2];
            if (inputBytes < rleHeader[i+1]) return EC_CannotChangeRepresentation;

            inputBytes -= rleHeader[i+1]; // number of bytes to feed to codec

            bytesToDecode = OFstatic_cast(size_t, inputBytes);
        }

        // last fragment for this RLE stripe
        result = rledecoder.decompress(rleData + byteOffset, bytesToDecode);

        // special handling for zero pad byte at the end of the RLE stream
        // which results in an EC_StreamNotifyClient return code
        // or trailing garbage data which results in EC_CorruptedData
        if (rledecoder.size() == bytesPerStripe) result = EC_Normal;

        byteOffset += inputBytes;

        // copy the decoded stuff over to the buffer here...
        // make sure the RLE decoder has produced the right amount of data
        if (lastStripe && (rledecoder.size() < bytesPerStripe))
        {
            // stream ended premature? report a warning and continue
            if (result == EC_StreamNotifyClient)
            {
                DCMDATA_WARN("RLE decoder is finished but has produced insufficient data for this stripe, filling remaining pixels");
                result = EC_Normal;
            }
        }
        else if (rledecoder.size() != bytesPerStripe)
        {
            DCMDATA_ERROR("RLE decoder is finished but has produced insufficient data for this stripe");
            return EC_CannotChangeRepresentation;
        }

        // distribute decompressed bytes into output image array
        // which sample and byte are we currently decompressing?
        sample = i / imageBytesAllocated;
        byte = i % imageBytesAllocated;

        // raw buffer containing bytesPerStripe bytes of uncompressed data
        outputBuffer = OFstatic_cast(Uint8 *, rledecoder.getOutputBuffer());

        // compute byte offsets
        if (imagePlanarConfiguration == 0)
        {
            sampleOffset = sample * imageBytesAllocated;
            offsetBetweenSamples = imageSamplesPerPixel * imageBytesAllocated;
        }
        else
        {
            sampleOffset = sample * imageBytesAllocated * imageColumns * imageRows;
            offsetBetweenSamples = imageBytesAllocated;
        }

        // initialize pointer to output data
        if (enableReverseByteOrder)
        {
            // assume incorrect LSB to MSB order of RLE segments as produced by some tools
            pixelPointer = imageData8 + sampleOffset + byte;
        }
        else
        {
            pixelPointer = imageData8 + sampleOffset + imageBytesAllocated - byte - 1;
        }

        // copy the pixel data that was decoded
        const size_t decoderSize = rledecoder.size();
        for (pixel = 0; pixel < decoderSize; ++pixel)
        {
            *pixelPointer = *outputBuffer++;
            pixelPointer += offsetBetweenSamples;
        }
        // and fill the remainder of the image with copies of the last decoded pixel
        const Uint8 lastPixelValue = *(outputBuffer - 1);
        for (pixel = decoderSize; pixel < bytesPerStripe; ++pixel)
        {
            *pixelPointer = lastPixelValue;
            pixelPointer += offsetBetweenSamples;
        }
    }

    /* remove used fragment from memory */
    pixItem->compact(); // there should only be one...

    if (result.good())
    {
      // compression was successful. Now update output parameters
      startFragment = currentItem + 1;
      decompressedColorModel = photometricInterpretation;
    }

    // adjust byte order for uncompressed image to little endian
    swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, imageData16, frameSize, sizeof(Uint16));

    return result;
}


OFCondition DcmRLECodecDecoder::encode(
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


OFCondition DcmRLECodecDecoder::encode(
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


OFCondition DcmRLECodecDecoder::determineDecompressedColorModel(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmItem *dataset,
    OFString &decompressedColorModel) const
{
    OFCondition result = EC_InvalidTag;
    if ((dataset != NULL ) && ((dataset->ident() == EVR_dataset) || (dataset->ident() == EVR_item)))
    {
        // retrieve color model from given dataset
        result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
    }
    return result;
}


/*
 * CVS/RCS Log
 * $Log: dcrleccd.cc,v $
 * Revision 1.18  2010-11-01 10:42:44  uli
 * Fixed some compiler warnings reported by gcc with additional flags.
 *
 * Revision 1.17  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.16  2010-10-01 10:21:05  uli
 * Fixed most compiler warnings from -Wall -Wextra -pedantic in dcmdata.
 *
 * Revision 1.15  2010-05-27 16:52:32  joergr
 * Re-added comment that was accidentally removed by the last commit.
 *
 * Revision 1.14  2010-05-21 14:02:48  joergr
 * Fixed issue with incorrectly encoded RLE images: Now, if the RLE decoder is
 * finished but has produced insufficient data, the remaining pixels of the
 * image are filled with the value of the last pixel. Applies to decodeFrame().
 * Added useful log messages on various levels to decode() and decodeFrame().
 *
 * Revision 1.13  2009-11-17 16:41:26  joergr
 * Added new method that allows for determining the color model of the
 * decompressed image.
 *
 * Revision 1.12  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.11  2009-08-10 11:27:00  meichel
 * Added working implementation of DcmRLECodecDecoder::decodeFrame().
 *
 * Revision 1.10  2009-08-10 09:38:06  meichel
 * All decompression codecs now replace NumberOfFrames if larger than one
 *   or present in the original image.
 *
 * Revision 1.9  2008-08-15 09:18:13  meichel
 * Decoder now gracefully handles the case of faulty images where value of
 *   NumberOfFrames is larger than the number of compressed fragments, if and only
 *   if there is just a single fragment per frame.
 *
 * Revision 1.8  2008-05-29 10:46:16  meichel
 * Implemented new method DcmPixelData::getUncompressedFrame
 *   that permits frame-wise access to compressed and uncompressed
 *   objects without ever loading the complete object into main memory.
 *   For this new method to work with compressed images, all classes derived from
 *   DcmCodec need to implement a new method decodeFrame(). For now, only
 *   dummy implementations returning an error code have been defined.
 *
 * Revision 1.7  2005/12/08 15:41:29  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2005/07/26 17:08:35  meichel
 * Added option to RLE decoder that allows to correctly decode images with
 *   incorrect byte order of byte segments (LSB instead of MSB).
 *
 * Revision 1.5  2004/08/24 14:54:20  meichel
 *  Updated compression helper methods. Image type is not set to SECONDARY
 *   any more, support for the purpose of reference code sequence added.
 *
 * Revision 1.4  2003/08/14 09:01:06  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.3  2003/03/21 13:08:04  meichel
 * Minor code purifications for warnings reported by MSVC in Level 4
 *
 * Revision 1.2  2002/07/18 12:15:39  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/06/06 14:52:40  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
