/*
 *
 *  Copyright (C) 2002-2019, OFFIS e.V.
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
      imageFrames = OFstatic_cast(Sint32, pixSeq->card()) - 1; // limit number of frames to number of pixel items - 1
    if (imageFrames < 1)
      imageFrames = 1; // default in case the number of frames attribute is absent or contains garbage

    if (result.good())
    {
      DcmPixelItem *pixItem = NULL;
      Uint8 * rleData = NULL;
      const size_t bytesPerStripe = OFstatic_cast(size_t, imageColumns) * OFstatic_cast(size_t, imageRows);

      DcmRLEDecoder rledecoder(bytesPerStripe);
      if (rledecoder.fail()) result = EC_MemoryExhausted;  // RLE decoder failed to initialize
      else
      {
        const size_t frameSize = OFstatic_cast(size_t, imageBytesAllocated) * OFstatic_cast(size_t, imageRows)
            * OFstatic_cast(size_t, imageColumns) * OFstatic_cast(size_t, imageSamplesPerPixel);
        size_t totalSize = frameSize * imageFrames;
        if (totalSize & 1) totalSize++; // align on 16-bit word boundary
        Uint16 *imageData16 = NULL;
        Sint32 currentFrame = 0;
        Uint32 currentItem = 1; // ignore offset table
        Uint32 numberOfStripes = 0;
        Uint32 fragmentLength = 0;

        result = uncompressedPixelData.createUint16Array(OFstatic_cast(Uint32, totalSize/sizeof(Uint16)), imageData16);
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
              swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rleHeader, 16*OFstatic_cast(Uint32, sizeof(Uint32)), sizeof(Uint32));

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
              OFBool lastStripeOfColor = OFFalse;
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
              Uint32 pixel = 0;

              // for each stripe in stripe set
              for (Uint32 stripeIndex = 0; (stripeIndex < numberOfStripes) && result.good(); ++stripeIndex)
              {
                // reset RLE codec
                rledecoder.clear();

                // adjust start point for RLE stripe, ignoring trailing garbage from the last run
                byteOffset = rleHeader[stripeIndex + 1];
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

                // something went wrong; most likely the byte offset in the RLE header is incorrect.
                if (result.bad()) return EC_CannotChangeRepresentation;

                // byteOffset now points to the first byte of the new RLE stripe
                // check if the current stripe is the last one for this frame
                if (stripeIndex + 1 == numberOfStripes) lastStripe = OFTrue; else lastStripe = OFFalse;

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
                  inputBytes = rleHeader[stripeIndex+2];
                  if (inputBytes < rleHeader[stripeIndex + 1]) result = EC_CannotChangeRepresentation;
                  else
                  {
                    inputBytes -= rleHeader[stripeIndex + 1]; // number of bytes to feed to codec
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

                // copy the decoded stuff over to the buffer here...
                // make sure the RLE decoder has produced the right amount of data
                lastStripeOfColor = lastStripe || ((imagePlanarConfiguration == 1) && ((stripeIndex + 1) % imageBytesAllocated == 0));

                if (lastStripeOfColor && (rledecoder.size() < bytesPerStripe))
                {
                    // stripe ended prematurely? report a warning and continue
                    DCMDATA_WARN("RLE decoder is finished but has produced insufficient data for this stripe, filling remaining pixels");
                    result = EC_Normal;
                }
                else if (rledecoder.size() != bytesPerStripe)
                {
                    DCMDATA_ERROR("RLE decoder is finished but has produced insufficient data for this stripe");
                    result = EC_CannotChangeRepresentation;
                }

                // distribute decompressed bytes into output image array
                if (result.good())
                {
                  // which sample and byte are we currently compressing?
                  sample = stripeIndex / imageBytesAllocated;
                  byte = stripeIndex % imageBytesAllocated;

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
          swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, imageData16, OFstatic_cast(Uint32, totalSize), sizeof(Uint16));

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
    const size_t bytesPerStripe = OFstatic_cast(size_t, imageColumns) * OFstatic_cast(size_t, imageRows);
    Uint32 numberOfStripes = 0;
    Uint32 fragmentLength = 0;
    Uint32 frameSize = OFstatic_cast(Uint32, imageBytesAllocated) * OFstatic_cast(Uint32, imageRows)
                       * OFstatic_cast(Uint32, imageColumns) * OFstatic_cast(Uint32, imageSamplesPerPixel);

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
    swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rleHeader, OFstatic_cast(Uint32, 16*sizeof(Uint32)), sizeof(Uint32));

    // determine number of stripes.
    numberOfStripes = rleHeader[0];

    // check that number of stripes in RLE header matches our expectation
    if ((numberOfStripes < 1) || (numberOfStripes > 15) || (numberOfStripes != OFstatic_cast(Uint32, imageBytesAllocated) * imageSamplesPerPixel))
        return EC_CannotChangeRepresentation;

    // this variable keeps the current position within the current fragment
    Uint32 byteOffset = 0;

    OFBool lastStripe = OFFalse;
    OFBool lastStripeOfColor = OFFalse;
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
    Uint32 pixel = 0;
    size_t bytesToDecode;

    // for each stripe in stripe set
    for (Uint32 stripeIndex = 0; stripeIndex < numberOfStripes; ++stripeIndex)
    {
        // reset RLE codec
        rledecoder.clear();

        // adjust start point for RLE stripe
        byteOffset = rleHeader[stripeIndex + 1];

        // byteOffset now points to the first byte of the new RLE stripe
        // check if the current stripe is the last one for this frame
        if (stripeIndex + 1 == numberOfStripes) lastStripe = OFTrue; else lastStripe = OFFalse;

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
            inputBytes = rleHeader[stripeIndex+2];
            if (inputBytes < rleHeader[stripeIndex + 1]) return EC_CannotChangeRepresentation;

            inputBytes -= rleHeader[stripeIndex + 1]; // number of bytes to feed to codec

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
        lastStripeOfColor = lastStripe || ((imagePlanarConfiguration == 1) && ((stripeIndex + 1) % imageBytesAllocated == 0));
        if (lastStripeOfColor && (rledecoder.size() < bytesPerStripe))
        {
            // stripe ended prematurely? report a warning and continue
            DCMDATA_WARN("RLE decoder is finished but has produced insufficient data for this stripe, filling remaining pixels");
            result = EC_Normal;
        }
        else if (rledecoder.size() != bytesPerStripe)
        {
            DCMDATA_ERROR("RLE decoder is finished but has produced insufficient data for this stripe");
            return EC_CannotChangeRepresentation;
        }

        // distribute decompressed bytes into output image array
        // which sample and byte are we currently decompressing?
        sample = stripeIndex / imageBytesAllocated;
        byte = stripeIndex % imageBytesAllocated;

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
        for (pixel = OFstatic_cast(Uint32, decoderSize); pixel < bytesPerStripe; ++pixel)
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
    OFCondition result = EC_IllegalParameter;
    if (dataset != NULL )
    {
        if ((dataset->ident() == EVR_dataset) || (dataset->ident() == EVR_item))
        {
            // retrieve color model from given dataset
            result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
            if (result == EC_TagNotFound)
            {
                DCMDATA_WARN("DcmRLECodecDecoder: Mandatory element PhotometricInterpretation " << DCM_PhotometricInterpretation << " is missing");
                result = EC_MissingAttribute;
            }
            else if (result.bad())
            {
                DCMDATA_WARN("DcmRLECodecDecoder: Cannot retrieve value of element PhotometricInterpretation " << DCM_PhotometricInterpretation << ": " << result.text());
            }
            else if (decompressedColorModel.empty())
            {
                DCMDATA_WARN("DcmRLECodecDecoder: No value for mandatory element PhotometricInterpretation " << DCM_PhotometricInterpretation);
                result = EC_MissingValue;
            }
        } else
            result = EC_CorruptedData;
    }
    return result;
}
