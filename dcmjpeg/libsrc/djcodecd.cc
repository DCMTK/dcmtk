/*
 *
 *  Copyright (C) 2001-2016, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: Abstract base class for IJG JPEG decoder
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djcodecd.h"

// dcmdata includes
#include "dcmtk/dcmdata/dcdatset.h"  /* for class DcmDataset */
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcpixseq.h"  /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcpxitem.h"  /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcvrpobw.h"  /* for class DcmPolymorphOBOW */
#include "dcmtk/dcmdata/dcswap.h"    /* for swapIfNecessary() */
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmGenerateUniqueIdentifer()*/

// dcmjpeg includes
#include "dcmtk/dcmjpeg/djcparam.h"  /* for class DJCodecParameter */
#include "dcmtk/dcmjpeg/djdecabs.h"  /* for class DJDecoder */


DJCodecDecoder::DJCodecDecoder()
: DcmCodec()
{
}


DJCodecDecoder::~DJCodecDecoder()
{
}


OFBool DJCodecDecoder::canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const
{
  E_TransferSyntax myXfer = supportedTransferSyntax();
  DcmXfer newRep(newRepType);
  if (newRep.isNotEncapsulated() && (oldRepType == myXfer)) return OFTrue; // decompress requested

  // we don't support re-coding for now.
  return OFFalse;
}


OFCondition DJCodecDecoder::decode(
    const DcmRepresentationParameter * fromRepParam,
    DcmPixelSequence * pixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    const DcmCodecParameter * cp,
    const DcmStack& objStack) const
{
  OFCondition result = EC_Normal;
  // assume we can cast the codec parameter to what we need
  const DJCodecParameter *djcp = OFreinterpret_cast(const DJCodecParameter*, cp);

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
    Uint16 imageBitsStored = 0;
    Uint16 imageHighBit = 0;
    const char *sopClassUID = NULL;
    OFBool createPlanarConfiguration = OFFalse;
    OFBool createPlanarConfigurationInitialized = OFFalse;
    EP_Interpretation colorModel = EPI_Unknown;
    OFBool isSigned = OFFalse;
    Uint16 pixelRep = 0; // needed to decline color conversion of signed pixel data to RGB
    OFBool numberOfFramesPresent = OFFalse;

    if (result.good()) result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel);
    if (result.good()) result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_Rows, imageRows);
    if (result.good()) result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_Columns, imageColumns);
    if (result.good()) result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated);
    if (result.good()) result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_BitsStored, imageBitsStored);
    if (result.good()) result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_HighBit, imageHighBit);
    if (result.good()) result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_PixelRepresentation, pixelRep);
    isSigned = (pixelRep == 0) ? OFFalse : OFTrue;

    // number of frames is an optional attribute - we don't mind if it isn't present.
    if (result.good())
    {
      if (OFreinterpret_cast(DcmItem*, dataset)->findAndGetSint32(DCM_NumberOfFrames, imageFrames).good()) numberOfFramesPresent = OFTrue;
    }

    // we consider SOP Class UID as optional since we only need it to determine SOP Class specific
    // encoding rules for planar configuration.
    if (result.good()) (void) OFreinterpret_cast(DcmItem*, dataset)->findAndGetString(DCM_SOPClassUID, sopClassUID);

    EP_Interpretation dicomPI = DcmJpegHelper::getPhotometricInterpretation(OFreinterpret_cast(DcmItem*, dataset));

    OFBool isYBR = OFFalse;
    if ((dicomPI == EPI_YBR_Full)||(dicomPI == EPI_YBR_Full_422)||(dicomPI == EPI_YBR_Partial_422)) isYBR = OFTrue;

    if (imageFrames >= OFstatic_cast(Sint32, pixSeq->card()))
      imageFrames = OFstatic_cast(Sint32, pixSeq->card() - 1); // limit number of frames to number of pixel items - 1
    if (imageFrames < 1)
      imageFrames = 1; // default in case the number of frames attribute contains garbage

    if (result.good())
    {
      DcmPixelItem *pixItem = NULL;
      Uint8 * jpegData = NULL;
      result = pixSeq->getItem(pixItem, 1); // first item is offset table, use second item
      if (result.good())
      {
        Uint32 fragmentLength = pixItem->getLength();
        result = pixItem->getUint8Array(jpegData);
        if (result.good())
        {
          if (jpegData == NULL) result = EC_CorruptedData; // JPEG data stream is empty/absent
          else
          {
            Uint8 precision = scanJpegDataForBitDepth(jpegData, fragmentLength);
            if (precision == 0) result = EC_CannotChangeRepresentation; // something has gone wrong, bail out
            else
            {
              DJDecoder *jpeg = createDecoderInstance(fromRepParam, djcp, precision, isYBR);
              if (jpeg == NULL) result = EC_MemoryExhausted;
              else
              {
                size_t frameSize = ((precision > 8) ? sizeof(Uint16) : sizeof(Uint8)) * imageRows * imageColumns * imageSamplesPerPixel;
                size_t totalSize = frameSize * imageFrames;
                if (totalSize & 1) totalSize++; // align on 16-bit word boundary
                Uint16 *imageData16 = NULL;
                Sint32 currentFrame = 0;
                size_t currentItem = 1; // ignore offset table

                if (isYBR && (imageBitsStored < imageBitsAllocated)) // check for a special case that is currently not handled properly
                {
                  if (djcp->getDecompressionColorSpaceConversion() != EDC_never)
                  {
                    DCMJPEG_WARN("BitsStored < BitsAllocated for JPEG compressed image with YCbCr color model, color space conversion will probably not work properly");
                    DCMJPEG_DEBUG("workaround: use option --conv-never (for command line tools) or EDC_never (for the DJDecoderRegistration::registerCodecs() call)");
                  }
                }

                result = uncompressedPixelData.createUint16Array(OFstatic_cast(Uint32, totalSize / sizeof(Uint16)), imageData16);
                if (result.good())
                {
                  Uint8 *imageData8 = OFreinterpret_cast(Uint8*, imageData16);

                  while ((currentFrame < imageFrames)&&(result.good()))
                  {
                    result = jpeg->init();
                    if (result.good())
                    {
                      result = EJ_Suspension;
                      while (EJ_Suspension == result)
                      {
                        result = pixSeq->getItem(pixItem, OFstatic_cast(Uint32, currentItem++));
                        if (result.good())
                        {
                          fragmentLength = pixItem->getLength();
                          result = pixItem->getUint8Array(jpegData);
                          if (result.good())
                          {
                            result = jpeg->decode(jpegData, fragmentLength, imageData8, OFstatic_cast(Uint32, frameSize), isSigned);
                          }
                        }
                      }
                      if (result.good())
                      {
                        if (! createPlanarConfigurationInitialized)
                        {
                          // we need to know the decompressed photometric interpretation in order
                          // to determine the final planar configuration.  However, this is only
                          // known after the first call to jpeg->decode(), i.e. here.
                          colorModel = jpeg->getDecompressedColorModel();
                          if (colorModel == EPI_Unknown)
                          {
                            // derive color model from DICOM photometric interpretation
                            if ((dicomPI == EPI_YBR_Full_422)||(dicomPI == EPI_YBR_Partial_422)) colorModel = EPI_YBR_Full;
                            else colorModel = dicomPI;
                          }

                          switch (djcp->getPlanarConfiguration())
                          {
                            case EPC_default:
                              createPlanarConfiguration = requiresPlanarConfiguration(sopClassUID, colorModel);
                              break;
                            case EPC_colorByPixel:
                              createPlanarConfiguration = OFFalse;
                              break;
                            case EPC_colorByPlane:
                              createPlanarConfiguration = OFTrue;
                              break;
                          }
                          createPlanarConfigurationInitialized = OFTrue;
                        }

                        // convert planar configuration if necessary
                        if ((imageSamplesPerPixel == 3) && createPlanarConfiguration)
                        {
                          if (precision > 8)
                            result = createPlanarConfigurationWord(OFreinterpret_cast(Uint16*, imageData8), imageColumns, imageRows);
                            else result = createPlanarConfigurationByte(imageData8, imageColumns, imageRows);
                        }
                        currentFrame++;
                        imageData8 += frameSize;
                      }
                    }
                  }

                  if (result.good())
                  {
                    // decompression is complete, finally adjust byte order if necessary
                    if (jpeg->bytesPerSample() == 1) // we're writing bytes into words
                    {
                      result = swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, imageData16,
                        OFstatic_cast(Uint32, totalSize), sizeof(Uint16));
                    }
                  }

                  // adjust photometric interpretation depending on what conversion has taken place
                  if (result.good())
                  {
                    switch (colorModel)
                    {
                      case EPI_Monochrome2:
                        result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
                        if (result.good())
                        {
                          imageSamplesPerPixel = 1;
                          result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_SamplesPerPixel, imageSamplesPerPixel);
                        }
                        break;
                      case EPI_YBR_Full:
                        result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertString(DCM_PhotometricInterpretation, "YBR_FULL");
                        if (result.good())
                        {
                          imageSamplesPerPixel = 3;
                          result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_SamplesPerPixel, imageSamplesPerPixel);
                        }
                        break;
                      case EPI_RGB:
                        result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertString(DCM_PhotometricInterpretation, "RGB");
                        if (result.good())
                        {
                          imageSamplesPerPixel = 3;
                          result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_SamplesPerPixel, imageSamplesPerPixel);
                        }
                        break;
                      default:
                        /* leave photometric interpretation untouched unless it is YBR_FULL_422
                         * or YBR_PARTIAL_422. In this case, replace by YBR_FULL since decompression
                         * eliminates the subsampling.
                         */
                        if ((dicomPI == EPI_YBR_Full_422)||(dicomPI == EPI_YBR_Partial_422))
                        {
                          result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertString(DCM_PhotometricInterpretation, "YBR_FULL");
                        }
                        break;
                    }
                  }

                  // Bits Allocated is now either 8 or 16
                  if (result.good())
                  {
                    if (precision > 8) result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_BitsAllocated, 16);
                    else result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_BitsAllocated, 8);
                  }

                  // Planar Configuration depends on the createPlanarConfiguration flag
                  if ((result.good()) && (imageSamplesPerPixel > 1))
                  {
                    result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_PlanarConfiguration, (createPlanarConfiguration ? 1 : 0));
                  }

                  // Bits Stored cannot be larger than precision
                  if ((result.good()) && (imageBitsStored > precision))
                  {
                    result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_BitsStored, precision);
                  }

                  // High Bit cannot be larger than precision - 1
                  if ((result.good()) && (imageHighBit >= precision))
                  {
                    result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertUint16(DCM_HighBit, OFstatic_cast(Uint16, precision-1));
                  }

                  // Number of Frames might have changed in case the previous value was wrong
                  if (result.good() && (numberOfFramesPresent || (imageFrames > 1)))
                  {
                    char numBuf[20];
                    sprintf(numBuf, "%ld", OFstatic_cast(long, imageFrames));
                    result = OFreinterpret_cast(DcmItem*, dataset)->putAndInsertString(DCM_NumberOfFrames, numBuf);
                  }

                  // Pixel Representation could be signed if lossless JPEG. For now, we just believe what we get.
                }
                delete jpeg;
              }
            }
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
        if (result.good() && (djcp->getUIDCreation() == EUC_always))
          result = DcmCodec::newInstance(OFreinterpret_cast(DcmItem*, dataset), NULL, NULL, NULL);
    }

  }
  return result;
}


OFCondition DJCodecDecoder::decodeFrame(
    const DcmRepresentationParameter *fromParam,
    DcmPixelSequence *fromPixSeq,
    const DcmCodecParameter *cp,
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32& startFragment,
    void *buffer,
    Uint32 bufSize,
    OFString& decompressedColorModel) const
{

  OFCondition result = EC_Normal;
  // assume we can cast the codec parameter to what we need
  const DJCodecParameter *djcp = OFreinterpret_cast(const DJCodecParameter*, cp);

  if ((!dataset)||((dataset->ident()!= EVR_dataset) && (dataset->ident()!= EVR_item))) result = EC_InvalidTag;
  else
  {
    Uint16 imageSamplesPerPixel = 0;
    Uint16 imageRows = 0;
    Uint16 imageColumns = 0;
    Sint32 imageFrames = 1;
    Uint16 imageBitsAllocated = 0;
    Uint16 imageBitsStored = 0;
    Uint16 imageHighBit = 0;
    Uint16 planarConfig = 0;
    OFString photometricInterpretation;
    OFBool isSigned = OFFalse;
    Uint16 pixelRep = 0; // needed to decline color conversion of signed pixel data to RGB

    if (result.good()) result = dataset->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel);
    if (result.good()) result = dataset->findAndGetUint16(DCM_Rows, imageRows);
    if (result.good()) result = dataset->findAndGetUint16(DCM_Columns, imageColumns);
    if (result.good()) result = dataset->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated);
    if (result.good()) result = dataset->findAndGetUint16(DCM_BitsStored, imageBitsStored);
    if (result.good()) result = dataset->findAndGetUint16(DCM_HighBit, imageHighBit);
    if (result.good()) result = dataset->findAndGetUint16(DCM_PixelRepresentation, pixelRep);
    if (result.good()) result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation);
    if (imageSamplesPerPixel > 1)
    {
      if (result.good()) result = dataset->findAndGetUint16(DCM_PlanarConfiguration, planarConfig);
    }

    isSigned = (pixelRep == 0) ? OFFalse : OFTrue;

    // number of frames is an optional attribute - we don't mind if it isn't present.
    if (result.good()) dataset->findAndGetSint32(DCM_NumberOfFrames, imageFrames);

    EP_Interpretation dicomPI = DcmJpegHelper::getPhotometricInterpretation(dataset);

    OFBool isYBR = OFFalse;
    if ((dicomPI == EPI_YBR_Full)||(dicomPI == EPI_YBR_Full_422)||(dicomPI == EPI_YBR_Partial_422)) isYBR = OFTrue;

    if (imageFrames < 1) imageFrames = 1; // default in case this attribute contains garbage

    // determine the corresponding item (first fragment) for this frame
    Uint32 currentItem = startFragment;

    // if the user has provided this information, we trust him.
    // If the user has passed a zero, try to find out ourselves.
    if (currentItem == 0 && result.good())
    {
      result = determineStartFragment(frameNo, imageFrames, fromPixSeq, currentItem);
    }

    // book-keeping needed to clean-up memory the end of this routine
    Uint32 firstFragmentUsed = currentItem;
    Uint32 pastLastFragmentUsed  = firstFragmentUsed;

    // now access and decompress the frame starting at the item we have identified
    if (result.good())
    {
      DcmPixelItem *pixItem = NULL;
      Uint8 * jpegData = NULL;
      result = fromPixSeq->getItem(pixItem, currentItem);
      if (result.good())
      {
        size_t fragmentLength = pixItem->getLength();
        result = pixItem->getUint8Array(jpegData);
        if (result.good())
        {
          if (jpegData == NULL) result = EC_CorruptedData; // JPEG data stream is empty/absent
          else
          {
            Uint8 precision = scanJpegDataForBitDepth(jpegData, OFstatic_cast(Uint32, fragmentLength));
            if (precision == 0) result = EC_CannotChangeRepresentation; // something has gone wrong, bail out
            else
            {
              size_t frameSize = ((precision > 8) ? sizeof(Uint16) : sizeof(Uint8)) * imageRows * imageColumns * imageSamplesPerPixel;
              if (frameSize > bufSize) return EC_IllegalCall;

              DJDecoder *jpeg = createDecoderInstance(fromParam, djcp, precision, isYBR);
              if (jpeg == NULL) result = EC_MemoryExhausted;
              else
              {
                if (isYBR && (imageBitsStored < imageBitsAllocated)) // check for a special case that is currently not handled properly
                {
                  if (djcp->getDecompressionColorSpaceConversion() != EDC_never)
                  {
                    DCMJPEG_WARN("BitsStored < BitsAllocated for JPEG compressed image with YCbCr color model, color space conversion will probably not work properly");
                    DCMJPEG_DEBUG("workaround: use option --conv-never (for command line tools) or EDC_never (for the DJDecoderRegistration::registerCodecs() call)");
                  }
                }

                result = jpeg->init();
                if (result.good())
                {
                  result = EJ_Suspension;
                  while (EJ_Suspension == result)
                  {
                    result = fromPixSeq->getItem(pixItem, currentItem++);
                    if (result.good())
                    {
                      fragmentLength = pixItem->getLength();
                      result = pixItem->getUint8Array(jpegData);
                      if (result.good())
                      {
                        result = jpeg->decode(jpegData, OFstatic_cast(Uint32, fragmentLength), OFreinterpret_cast(Uint8*, buffer), OFstatic_cast(Uint32, frameSize), isSigned);
                        pastLastFragmentUsed = currentItem;
                      }
                    }
                  }
                  if (result.good())
                  {
                    // convert planar configuration to color by plane if necessary
                    if ((imageSamplesPerPixel == 3) && (planarConfig == 1))
                    {
                      if (precision > 8)
                        result = createPlanarConfigurationWord(OFreinterpret_cast(Uint16*, buffer), imageColumns, imageRows);
                      else result = createPlanarConfigurationByte(OFreinterpret_cast(Uint8*, buffer), imageColumns, imageRows);
                    }
                  }

                  if (result.good())
                  {
                    // decompression is complete, finally adjust byte order if necessary
                    if (jpeg->bytesPerSample() == 1) // we're writing bytes into words
                    {
                      result = swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, OFreinterpret_cast(Uint16*, buffer), OFstatic_cast(Uint32, frameSize), sizeof(Uint16));
                    }
                  }

                  if (result.good())
                  {
                    // compression was successful. Now update output parameters
                    startFragment = pastLastFragmentUsed;
                    decompressedColorModel = photometricInterpretation; // this is the default

                    // now see if we have to change the photometric interpretation
                    // because the decompression has changed something
                    switch (jpeg->getDecompressedColorModel())
                    {
                      case EPI_Monochrome2:
                        decompressedColorModel = "MONOCHROME2";
                        break;
                      case EPI_YBR_Full:
                        decompressedColorModel = "YBR_FULL";
                        break;
                      case EPI_RGB:
                        decompressedColorModel = "RGB";
                        break;
                      default:
                        if ((dicomPI == EPI_YBR_Full_422)||(dicomPI == EPI_YBR_Partial_422))
                        {
                          // decompression always eliminates subsampling
                          decompressedColorModel = "YBR_FULL";
                        }
                        break;
                    }
                  }

                  delete jpeg;

                  /* remove all used fragments from memory */
                  while (firstFragmentUsed < pastLastFragmentUsed)
                  {
                    fromPixSeq->getItem(pixItem, firstFragmentUsed++);
                    pixItem->compact();
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return result;
}


OFCondition DJCodecDecoder::encode(
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


OFCondition DJCodecDecoder::encode(
    const E_TransferSyntax /* fromRepType */,
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmRepresentationParameter * /* toRepParam */,
    DcmPixelSequence * & /* toPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmStack & /* objStack */) const
{
  // we don't support re-coding for now
  return EC_IllegalCall;
}


OFCondition DJCodecDecoder::determineDecompressedColorModel(
    const DcmRepresentationParameter *fromParam,
    DcmPixelSequence *fromPixSeq,
    const DcmCodecParameter *cp,
    DcmItem *dataset,
    OFString &decompressedColorModel) const
{
  OFCondition result = EC_CorruptedData;
  if ((dataset != NULL) && (fromPixSeq != NULL))
  {
    // the first frame always starts with the second fragment
    Uint32 startFragment = 1;
    Uint32 bufSize = 0;
    // determine size of uncompressed frame
    if ((fromPixSeq->getUncompressedFrameSize(dataset, bufSize).good()) && (bufSize > 0))
    {
      // allocate temporary buffer for a single frame
      Uint8 *buffer = new Uint8[bufSize];
      if (buffer != NULL)
      {
        DCMJPEG_DEBUG("decompressing first frame to determine the decompressed color model");
        // simple approach: decode first frame in order to determine the uncompressed color model
        result = decodeFrame(fromParam, fromPixSeq, cp, dataset, 0 /* frameNo */, startFragment,
          OFstatic_cast(void *, buffer), bufSize, decompressedColorModel);
      } else
        result = EC_MemoryExhausted;
      delete[] buffer;
    }
  }
  if (result.bad())
      DCMJPEG_ERROR("can't decompress first frame: " << result.text());
  return result;
}


Uint16 DJCodecDecoder::readUint16(const Uint8 *data)
{
  return OFstatic_cast(Uint16, (OFstatic_cast(Uint16, *data) << 8) | OFstatic_cast(Uint16, *(data+1)));
}


Uint8 DJCodecDecoder::scanJpegDataForBitDepth(
  const Uint8 *data,
  const Uint32 fragmentLength)
{
  // first, check whether there is any JPEG data at all
  if (data == NULL) return 0;
  Uint32 offset = 0;
  while(offset+4 < fragmentLength)
  {
    switch(readUint16(data+offset))
    {
      case 0xffc0: // SOF_0: JPEG baseline
        return data[offset+4];
        /* break; */
      case 0xffc1: // SOF_1: JPEG extended sequential DCT
        return data[offset+4];
        /* break; */
      case 0xffc2: // SOF_2: JPEG progressive DCT
        return data[offset+4];
        /* break; */
      case 0xffc3 : // SOF_3: JPEG lossless sequential
        return data[offset+4];
        /* break; */
      case 0xffc5: // SOF_5: differential (hierarchical) extended sequential, Huffman
        return data[offset+4];
        /* break; */
      case 0xffc6: // SOF_6: differential (hierarchical) progressive, Huffman
        return data[offset+4];
        /* break; */
      case 0xffc7: // SOF_7: differential (hierarchical) lossless, Huffman
        return data[offset+4];
        /* break; */
      case 0xffc8: // Reserved for JPEG extentions
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffc9: // SOF_9: extended sequential, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffca: // SOF_10: progressive, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffcb: // SOF_11: lossless, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffcd: // SOF_13: differential (hierarchical) extended sequential, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffce: // SOF_14: differential (hierarchical) progressive, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffcf: // SOF_15: differential (hierarchical) lossless, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffc4: // DHT
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffcc: // DAC
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffd0: // RST m
      case 0xffd1:
      case 0xffd2:
      case 0xffd3:
      case 0xffd4:
      case 0xffd5:
      case 0xffd6:
      case 0xffd7:
        offset +=2;
        break;
      case 0xffd8: // SOI
        offset +=2;
        break;
      case 0xffd9: // EOI
        offset +=2;
        break;
      case 0xffda: // SOS
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdb: // DQT
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdc: // DNL
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdd: // DRI
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffde: // DHP
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdf: // EXP
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffe0: // APPn
      case 0xffe1:
      case 0xffe2:
      case 0xffe3:
      case 0xffe4:
      case 0xffe5:
      case 0xffe6:
      case 0xffe7:
      case 0xffe8:
      case 0xffe9:
      case 0xffea:
      case 0xffeb:
      case 0xffec:
      case 0xffed:
      case 0xffee:
      case 0xffef:
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xfff0: // JPGn
      case 0xfff1:
      case 0xfff2:
      case 0xfff3:
      case 0xfff4:
      case 0xfff5:
      case 0xfff6:
      case 0xfff7:
      case 0xfff8:
      case 0xfff9:
      case 0xfffa:
      case 0xfffb:
      case 0xfffc:
      case 0xfffd:
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xfffe: // COM
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffff: // fill byte 0xff (skip one byte only)
        offset += 1;
        break;
      case 0xff01: // TEM
        break;
      default:
        if ((data[offset]==0xff) && (data[offset+1]>2) && (data[offset+1] <= 0xbf)) // RES reserved markers
        {
          offset += 2;
        }
        else
        {
          DCMJPEG_ERROR("found invalid marker in JPEG stream while scanning for bit depth: 0x"
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
            << STD_NAMESPACE setw(2) << OFstatic_cast(int, data[offset])
            << STD_NAMESPACE setw(2) << OFstatic_cast(int, data[offset+1])
            << STD_NAMESPACE dec << STD_NAMESPACE setfill(' '));
          return 0; // syntax error, stop parsing
        }
        break;
    }
  } // while
  return 0; // no SOF marker found
}


OFCondition DJCodecDecoder::createPlanarConfigurationByte(
  Uint8 *imageFrame,
  Uint16 columns,
  Uint16 rows)
{
  if (imageFrame == NULL) return EC_IllegalCall;

  size_t numPixels = columns * rows;
  if (numPixels == 0) return EC_IllegalCall;

  Uint8 *buf = new Uint8[3*numPixels + 3];
  if (buf)
  {
    memcpy(buf, imageFrame, 3*numPixels);
    Uint8 *s = buf;                        // source
    Uint8 *r = imageFrame;                 // red plane
    Uint8 *g = imageFrame + numPixels;     // green plane
    Uint8 *b = imageFrame + (2*numPixels); // blue plane
    for (size_t i=numPixels; i; i--)
    {
      *r++ = *s++;
      *g++ = *s++;
      *b++ = *s++;
    }
    delete[] buf;
  } else return EC_MemoryExhausted;
  return EC_Normal;
}

OFCondition DJCodecDecoder::createPlanarConfigurationWord(
  Uint16 *imageFrame,
  Uint16 columns,
  Uint16 rows)
{
  if (imageFrame == NULL) return EC_IllegalCall;

  size_t numPixels = columns * rows;
  if (numPixels == 0) return EC_IllegalCall;

  Uint16 *buf = new Uint16[3*numPixels + 3];
  if (buf)
  {
    memcpy(buf, imageFrame, 3*numPixels*sizeof(Uint16));
    Uint16 *s = buf;                        // source
    Uint16 *r = imageFrame;                 // red plane
    Uint16 *g = imageFrame + numPixels;     // green plane
    Uint16 *b = imageFrame + (2*numPixels); // blue plane
    for (size_t i=numPixels; i; i--)
    {
      *r++ = *s++;
      *g++ = *s++;
      *b++ = *s++;
    }
    delete[] buf;
  } else return EC_MemoryExhausted;
  return EC_Normal;
}

/* This method examines if a given image requires color-by-plane planar configuration
 * depending on SOP Class UID (DICOM IOD) and photometric interpretation.
 * All SOP classes defined in the 2001 edition of the DICOM standard or earlier
 * are handled correctly.
 */

OFBool DJCodecDecoder::requiresPlanarConfiguration(
  const char *sopClassUID,
  EP_Interpretation photometricInterpretation)
{
  if (sopClassUID)
  {
    OFString sopClass(sopClassUID);

    // Hardcopy Color Image always requires color-by-plane
    if (sopClass == UID_RETIRED_HardcopyColorImageStorage) return OFTrue;

    // The 1996 Ultrasound Image IODs require color-by-plane if color model is YBR_FULL.
    if (photometricInterpretation == EPI_YBR_Full)
    {
      if ((sopClass == UID_UltrasoundMultiframeImageStorage)
        ||(sopClass == UID_UltrasoundImageStorage)) return OFTrue;
    }

  }
  return OFFalse;
}
