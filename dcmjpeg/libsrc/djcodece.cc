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
 *  Purpose: abstract codec class for JPEG encoders.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djcodece.h"

// ofstd includes
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstd.h"

// dcmdata includes
#include "dcmtk/dcmdata/dcdatset.h"   /* for class DcmDataset */
#include "dcmtk/dcmdata/dcdeftag.h"   /* for tag constants */
#include "dcmtk/dcmdata/dcovlay.h"    /* for class DcmOverlayData */
#include "dcmtk/dcmdata/dcpixseq.h"   /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcpxitem.h"   /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmGenerateUniqueIdentifer()*/
#include "dcmtk/dcmdata/dcvrcs.h"     /* for class DcmCodeString */
#include "dcmtk/dcmdata/dcvrds.h"     /* for class DcmDecimalString */
#include "dcmtk/dcmdata/dcvrlt.h"     /* for class DcmLongText */
#include "dcmtk/dcmdata/dcvrst.h"     /* for class DcmShortText */
#include "dcmtk/dcmdata/dcvrus.h"     /* for class DcmUnsignedShort */
#include "dcmtk/dcmdata/dcswap.h"     /* for swapIfNecessary */

// dcmjpeg includes
#include "dcmtk/dcmjpeg/djcparam.h"   /* for class DJCodecParameter */
#include "dcmtk/dcmjpeg/djencabs.h"   /* for class DJEncoder */

// dcmimgle includes
#include "dcmtk/dcmimgle/dcmimage.h"  /* for class DicomImage */

#define INCLUDE_CMATH
#include "dcmtk/ofstd/ofstdinc.h"


DJCodecEncoder::DJCodecEncoder()
: DcmCodec()
{
}


DJCodecEncoder::~DJCodecEncoder()
{
}


OFBool DJCodecEncoder::canChangeCoding(
  const E_TransferSyntax oldRepType,
  const E_TransferSyntax newRepType) const
{
  E_TransferSyntax myXfer = supportedTransferSyntax();
  DcmXfer newRep(newRepType);
  DcmXfer oldRep(oldRepType);
  if (oldRep.isNotEncapsulated() && (newRepType == myXfer)) return OFTrue; // compress requested

  // we don't support re-coding for now
  return OFFalse;
}


OFCondition DJCodecEncoder::decode(
  const DcmRepresentationParameter * /* fromRepParam */,
  DcmPixelSequence * /* pixSeq */,
  DcmPolymorphOBOW& /* uncompressedPixelData */,
  const DcmCodecParameter * /* cp */,
  const DcmStack& /* objStack */) const
{
  // we are an encoder only
  return EC_IllegalCall;
}


OFCondition DJCodecEncoder::decodeFrame(
  const DcmRepresentationParameter * /* fromParam */ ,
  DcmPixelSequence * /* fromPixSeq */ ,
  const DcmCodecParameter * /* cp */ ,
  DcmItem * /* dataset */ ,
  Uint32 /* frameNo */ ,
  Uint32& /* startFragment */ ,
  void * /* buffer */ ,
  Uint32 /* bufSize */ ,
  OFString& /* decompressedColorModel */ ) const
{
  // we are an encoder only
  return EC_IllegalCall;
}


OFCondition DJCodecEncoder::encode(
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


OFCondition DJCodecEncoder::encode(
  const Uint16 * /* pixelData */,
  const Uint32 /* length */,
  const DcmRepresentationParameter * toRepParam,
  DcmPixelSequence * & pixSeq,
  const DcmCodecParameter *cp,
  DcmStack & objStack) const
{
  OFCondition result = EC_Normal;
  // assume we can cast the codec parameter to what we need
  const DJCodecParameter *djcp = OFreinterpret_cast(const DJCodecParameter*, cp);

  // if true lossless mode is enabled, and we're supposed to do lossless compression,
  // call the "true lossless encoding"-engine
  if (isLosslessProcess() && (djcp->getTrueLosslessMode()))
    return encodeTrueLossless(toRepParam, pixSeq, cp, objStack);

  DcmStack localStack(objStack);
  (void)localStack.pop();                // pop pixel data element from stack
  DcmObject *dataset = localStack.pop(); // this is the item in which the pixel data is located

  if ((!dataset)||((dataset->ident()!= EVR_dataset) && (dataset->ident()!= EVR_item))) result = EC_InvalidTag;
  else
  {
    double compressionRatio = 0.0;
    EP_Interpretation photometricInterpretation = DcmJpegHelper::getPhotometricInterpretation(OFreinterpret_cast(DcmItem*, dataset));
    switch (photometricInterpretation)
    {
      case EPI_Monochrome1:
      case EPI_Monochrome2:
        // monochrome image
        result = encodeMonochromeImage(OFreinterpret_cast(DcmItem*, dataset), toRepParam, pixSeq, djcp, compressionRatio);
        break;
      case EPI_PaletteColor:
      case EPI_RGB:
      case EPI_HSV:
      case EPI_ARGB:
      case EPI_CMYK:
      case EPI_YBR_Partial_422:
        // color image except YCbCr which receives special treatment
        result = encodeColorImage(OFFalse, OFreinterpret_cast(DcmItem*, dataset), toRepParam, pixSeq, djcp, compressionRatio);
        break;
      case EPI_YBR_Full:
      case EPI_YBR_Full_422:
        // YCbCr color image
        result = encodeColorImage(OFTrue, OFreinterpret_cast(DcmItem*, dataset), toRepParam, pixSeq, djcp, compressionRatio);
        break;
      case EPI_Unknown:
      case EPI_Missing:
        // unknown color model - bail out
        result = EJ_UnsupportedPhotometricInterpretation;
        break;
    }

    // the following operations do not affect the Image Pixel Module
    // but other modules such as SOP Common.  We only perform these
    // changes if we're on the main level of the dataset,
    // which should always identify itself as dataset, not as item.
    if (dataset->ident() == EVR_dataset)
    {

      // update image type
      if (result.good()) result = DcmCodec::updateImageType(OFreinterpret_cast(DcmItem*, dataset));

      // determine compressed bit depth passed to JPEG codec
      Uint16 compressedBits = OFstatic_cast(Uint16, djcp->getForcedBitDepth());
      if (result.good())
      {
        if (compressedBits == 0)
        {
          result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_BitsStored, compressedBits);
        }
      }

      // update derivation description
      if (result.good()) result = updateDerivationDescription(OFreinterpret_cast(DcmItem*, dataset), toRepParam,
        djcp, OFstatic_cast(Uint8, compressedBits), compressionRatio);

      if (result.good())
      {
        // lossy process - create new UID unless mode is EUC_never and we're not converting to Secondary Capture
        // (pseudo-lossless mode may also result in lossy compression, so treat it the same way)
        if (djcp->getConvertToSC() || (djcp->getUIDCreation() != EUC_never))
          result = DcmCodec::newInstance(OFreinterpret_cast(DcmItem*, dataset), "DCM", "121320", "Uncompressed predecessor");

        // update lossy compression ratio
        if (result.good()) result = updateLossyCompressionRatio(OFreinterpret_cast(DcmItem*, dataset), compressionRatio);
      }

      // convert to Secondary Capture if requested by user.
      // This method creates a new SOP class UID, so it should be executed
      // after the call to newInstance() which creates a Source Image Sequence.
      if (result.good() && djcp->getConvertToSC()) result = DcmCodec::convertToSecondaryCapture(OFreinterpret_cast(DcmItem*, dataset));
    }
  }
  return result;
}


OFCondition DJCodecEncoder::determineDecompressedColorModel(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmItem * /* dataset */,
    OFString & /* decompressedColorModel */) const
{
    return EC_IllegalCall;
}


OFCondition DJCodecEncoder::encodeColorImage(
  OFBool YBRmode,
  DcmItem *dataset,
  const DcmRepresentationParameter * toRepParam,
  DcmPixelSequence * & pixSeq,
  const DJCodecParameter *cp,
  double& compressionRatio) const
{
  OFCondition result = EC_Normal;
  DcmOffsetList offsetList;
  DcmPixelSequence *pixelSequence = NULL;
  DcmPixelItem *offsetTable = NULL;
  unsigned short bitsPerSample = 0;
  compressionRatio = 0.0; // initialize if something goes wrong
  size_t compressedSize = 0;
  double uncompressedSize = 0.0;
  Uint16 compressedBits = OFstatic_cast(Uint16, cp->getForcedBitDepth());

  // initialize settings with defaults for RGB mode
  OFBool monochromeMode = OFFalse;
  size_t flags = 0; // flags for initialization of DicomImage
  EP_Interpretation interpr = EPI_RGB;
  Uint16 samplesPerPixel = 3;
  const char *photometricInterpretation = "RGB";
  if ((cp->getCompressionColorSpaceConversion() == ECC_lossyYCbCr) && (!isLosslessProcess()))
  {
    if (cp->getWriteYBR422()) photometricInterpretation = "YBR_FULL_422";
    else photometricInterpretation = "YBR_FULL";
  }

  // check mode and adjust settings
  if (cp->getCompressionColorSpaceConversion() == ECC_monochrome)
  {
    monochromeMode = OFTrue;
    flags = 0;
    interpr = EPI_Monochrome2;
    samplesPerPixel = 1;
    photometricInterpretation = "MONOCHROME2";
  }
  else if (YBRmode)
  {
    monochromeMode = OFFalse;
    flags = CIF_KeepYCbCrColorModel; // keep YCbCr color model
    interpr = EPI_YBR_Full;
    if (cp->getWriteYBR422()) photometricInterpretation = "YBR_FULL_422";
    else photometricInterpretation = "YBR_FULL";
  }

  // integrate DicomImage flags transported by DJCodecParameter into "flags"-variable
  if (cp->getAcceptWrongPaletteTags())
    flags |= CIF_WrongPaletteAttributeTags;
  if (cp->getAcrNemaCompatibility())
    flags |= CIF_AcrNemaCompatibility;

  // create dcmimage object. Will fail if dcmimage has not been activated in main().
  // transfer syntax can be any uncompressed one.
  DicomImage *dimage = NULL;
  if (monochromeMode)
  {
    DicomImage colorimage(dataset, EXS_LittleEndianImplicit, flags); // read all frames
    if (colorimage.getStatus() == EIS_Normal) dimage = colorimage.createMonochromeImage();
    if (dimage) dimage->setNoVoiTransformation();
  }
  else
  {
    dimage = new DicomImage(dataset, EXS_LittleEndianImplicit, flags); // read all frames
  }

  if (dimage == NULL) result = EC_MemoryExhausted;
  else if (dimage->getStatus() != EIS_Normal) result = EC_IllegalCall; // should return dimage->getStatus()

  // don't render overlays
  if (result.good())
  {
    dimage->hideAllOverlays();
  }

  // create initial pixel sequence
  if (result.good())
  {
    pixelSequence = new DcmPixelSequence(DCM_PixelSequenceTag);
    if (pixelSequence == NULL) result = EC_MemoryExhausted;
    else
    {
      // create empty offset table
      offsetTable = new DcmPixelItem(DCM_PixelItemTag);
      if (offsetTable == NULL) result = EC_MemoryExhausted;
      else pixelSequence->insert(offsetTable);
    }
  }

  // select bit depth
  if (result.good())
  {
    if (compressedBits == 0)
    {
      result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_BitsStored, compressedBits);
    }
  }

  // create codec instance
  if (result.good())
  {
    DJEncoder *jpeg = createEncoderInstance(toRepParam, cp, OFstatic_cast(Uint8, compressedBits));
    if (jpeg)
    {
      // render and compress each frame
      bitsPerSample = jpeg->bitsPerSample();
      size_t frameCount = dimage->getFrameCount();
      unsigned short bytesPerSample = jpeg->bytesPerSample();
      unsigned short columns = OFstatic_cast(unsigned short, dimage->getWidth());
      unsigned short rows = OFstatic_cast(unsigned short, dimage->getHeight());
      Uint8 *jpegData = NULL;
      Uint32 jpegLen  = 0;
      const void *frame = NULL;

      // compute original image size in bytes, ignoring any padding bits.
      uncompressedSize = OFstatic_cast(double, columns * rows * dimage->getDepth() * frameCount * samplesPerPixel) / 8.0;
      for (size_t i=0; (i<frameCount) && (result.good()); i++)
      {
        frame = dimage->getOutputData(bitsPerSample, i, 0);
        if (frame == NULL) result = EC_MemoryExhausted;
        else
        {
          // compress frame
          jpegData = NULL;
          if (bytesPerSample == 1)
          {
            result = jpeg->encode(columns, rows, interpr, samplesPerPixel, OFreinterpret_cast(Uint8*, OFconst_cast(void*, frame)), jpegData, jpegLen);
          } else {
            result = jpeg->encode(columns, rows, interpr, samplesPerPixel, OFreinterpret_cast(Uint16*, OFconst_cast(void*, frame)), jpegData, jpegLen);
          }

          // store frame
          if (result.good())
          {
            result = pixelSequence->storeCompressedFrame(offsetList, jpegData, jpegLen, cp->getFragmentSize());
          }

          // delete block of JPEG data
          delete[] jpegData;
          compressedSize += jpegLen;
        }
      }
      delete jpeg;
    } else result = EC_MemoryExhausted;
  }

  // store pixel sequence if everything was successful
  if (result.good()) pixSeq = pixelSequence;
  else
  {
    delete pixelSequence;
    pixSeq = NULL;
  }

  if ((result.good()) && (cp->getCreateOffsetTable()))
  {
    // create offset table
    result = offsetTable->createOffsetTable(offsetList);
  }

  if (result.good())
  {
    // adapt attributes in image pixel module
    if (result.good()) result = dataset->putAndInsertUint16(DCM_SamplesPerPixel, samplesPerPixel);
    if (result.good()) result = dataset->putAndInsertString(DCM_PhotometricInterpretation, photometricInterpretation);
    if (result.good())
    {
      if (bitsPerSample > 8)
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 16);
      else
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 8);
    }
    if (result.good()) result = dataset->putAndInsertUint16(DCM_BitsStored, bitsPerSample);
    if (result.good()) result = dataset->putAndInsertUint16(DCM_HighBit, OFstatic_cast(Uint16, bitsPerSample-1));
    if (result.good()) result = dataset->putAndInsertUint16(DCM_PixelRepresentation, 0);
    if (result.good())
    {
      if (monochromeMode) delete dataset->remove(DCM_PlanarConfiguration);
      else result = dataset->putAndInsertUint16(DCM_PlanarConfiguration, 0);
    }
    delete dataset->remove(DCM_SmallestImagePixelValue);
    delete dataset->remove(DCM_LargestImagePixelValue);
    delete dataset->remove(DCM_RedPaletteColorLookupTableDescriptor);
    delete dataset->remove(DCM_GreenPaletteColorLookupTableDescriptor);
    delete dataset->remove(DCM_BluePaletteColorLookupTableDescriptor);
    delete dataset->remove(DCM_RedPaletteColorLookupTableData);
    delete dataset->remove(DCM_GreenPaletteColorLookupTableData);
    delete dataset->remove(DCM_BluePaletteColorLookupTableData);
    delete dataset->remove(DCM_PixelPaddingValue);
    delete dataset->remove(DCM_PixelPaddingRangeLimit);
    delete dataset->remove(DCM_SmallestPixelValueInSeries);
    delete dataset->remove(DCM_LargestPixelValueInSeries);
    delete dataset->remove(DCM_PaletteColorLookupTableUID);
    delete dataset->remove(DCM_SegmentedRedPaletteColorLookupTableData);
    delete dataset->remove(DCM_SegmentedGreenPaletteColorLookupTableData);
    delete dataset->remove(DCM_SegmentedBluePaletteColorLookupTableData);
  }
  if (compressedSize > 0) compressionRatio = OFstatic_cast(double, uncompressedSize) / OFstatic_cast(double, compressedSize);

  delete dimage;
  return result;
}


OFCondition DJCodecEncoder::encodeTrueLossless(
  const DcmRepresentationParameter * toRepParam,
  DcmPixelSequence * & pixSeq,
  const DcmCodecParameter *cp,
  DcmStack & objStack) const
{
  OFCondition result = EC_Normal;
  // assume we can cast the codec parameter to what we need
  DJCodecParameter *djcp = OFreinterpret_cast(DJCodecParameter*, OFconst_cast(DcmCodecParameter*, cp));
  // get dataset from stack
  DcmStack localStack(objStack);
  (void)localStack.pop();
  DcmObject *dataset = localStack.pop();

  // check whether dataset was on top of the stack
  if ((!dataset)||((dataset->ident()!= EVR_dataset) && (dataset->ident()!= EVR_item)))
    return EC_InvalidTag;
  else
  {
    DcmItem *datsetItem = OFreinterpret_cast(DcmItem*, dataset);
    double compressionRatio = 0.0;
    const Uint16* pixelData;
    size_t length = 0;
    Uint16 bitsAllocated = 0;
    Uint16 bitsStored = 0;
    Uint16 bytesAllocated = 0;
    Uint16 samplesPerPixel = 0;
    Uint16 planarConfiguration = 0;
    Uint16 columns = 0;
    Uint16 rows = 0;
    Sint32 numberOfFrames = 1;
    EP_Interpretation interpr = EPI_Unknown;
    Uint8 *jpegData = NULL;
    Uint32 jpegLen  = 0;
    OFBool byteSwapped = OFFalse;      // true if we have byte-swapped the original pixel data
    OFBool planConfSwitched = OFFalse; // true if planar configuration was toggled
    DcmOffsetList offsetList;
    OFString photometricInterpretation;
    DcmElement *dummyElem;

    // get relevant attributes for encoding from dataset
    result = datsetItem->findAndGetUint16(DCM_BitsStored, bitsStored);
    if (result.good()) result = datsetItem->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
    if (result.good()) result = datsetItem->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
    if (result.good()) result = datsetItem->findAndGetUint16(DCM_Columns, columns);
    if (result.good()) result = datsetItem->findAndGetUint16(DCM_Rows, rows);
    if (result.good()) result = datsetItem->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation);
    if (result.good()) result = datsetItem->findAndGetUint16Array(DCM_PixelData, pixelData, NULL, OFFalse);
    if (result.good()) result = datsetItem->findAndGetElement(DCM_PixelData, dummyElem);
    if (result.good()) length = dummyElem->getLength();
    if (result.good())
    {
      result = datsetItem->findAndGetSint32(DCM_NumberOfFrames, numberOfFrames);
      if (result.bad() || numberOfFrames < 1) numberOfFrames = 1;
      result = EC_Normal;
    }
    if (result.bad())
    {
      DCMJPEG_ERROR("True lossless encoder: Unable to get relevant attributes from dataset");
      return result;
    }

    // check, whether bit depth is supported
    if (bitsAllocated == 8)
      bytesAllocated = 1;
    else if (bitsAllocated == 16)
      bytesAllocated = 2;
    else
    {
      DCMJPEG_ERROR("True lossless encoder: Only 8 or 16 bits allocated supported");
      return EC_IllegalParameter;
    }

    // make sure that all the descriptive attributes have sensible values
    if ((columns < 1)||(rows < 1)||(samplesPerPixel < 1))
    {
      DCMJPEG_ERROR("True lossless encoder: Invalid attribute values in pixel module");
      return EC_CannotChangeRepresentation;
    }

    /* Set and check photometric interpretation (up to now: EPI_RGB)
     * Only photometric interpretations, that are explicitly "supported" by the
     * IJG lib are set. For all others "unknown" is set. Some are even rejected here.
     */
    if (photometricInterpretation == "MONOCHROME1")
      interpr = EPI_Monochrome1;
    else if (photometricInterpretation == "MONOCHROME2")
      interpr = EPI_Monochrome2;
    else if (photometricInterpretation == "YBR_FULL")
      interpr = EPI_YBR_Full;
    // some photometric interpretations are not supported:
    else if ( (photometricInterpretation == "YBR_FULL_422")    ||
              (photometricInterpretation == "YBR_PARTIAL_422") ||
              (photometricInterpretation == "YBR_PARTIAL_420") ||
              (photometricInterpretation == "YBR_ICT")         ||
              (photometricInterpretation == "YBR_RCT") )
    {
      DCMJPEG_ERROR("True lossless encoder: Photometric interpretation not supported: " << photometricInterpretation);
      return EC_IllegalParameter;
    }
    else    // Palette, HSV, ARGB, CMYK
      interpr = EPI_Unknown;

    // IJG libs need "color by pixel", transform if required
    if (result.good() && (samplesPerPixel > 1) )
    {
      result = datsetItem->findAndGetUint16(DCM_PlanarConfiguration, planarConfiguration);
      if ( result.good() && (planarConfiguration == 1) )
      {
        if (bytesAllocated == 1)
          result = togglePlanarConfiguration8(OFreinterpret_cast(Uint8*, OFconst_cast(Uint16*, pixelData)), length, samplesPerPixel, OFstatic_cast(Uint16, 1) /* switch to "by pixel"*/);
        else
          result = togglePlanarConfiguration16(OFconst_cast(Uint16*, pixelData), length/2 /*16 bit*/, samplesPerPixel, OFstatic_cast(Uint16, 1) /* switch to "by pixel"*/);
        planConfSwitched = OFTrue;
      }
    }
    if (result.bad())
    {
        DCMJPEG_ERROR("True lossless encoder: Unable to change Planar Configuration from 'by plane' to 'by pixel' for encoding");
        return result;
    }

    // check whether enough raw data is available for encoding
    if (bytesAllocated * samplesPerPixel * columns * rows * OFstatic_cast(size_t,numberOfFrames) > length)
    {
      DCMJPEG_ERROR("True lossless encoder: Cannot change representation, not enough data");
      return EC_CannotChangeRepresentation;
    }

    // byte swap pixel data to little endian if bits allocated is 8
    if ((gLocalByteOrder == EBO_BigEndian) && (bitsAllocated == 8))
    {
      result = swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, OFconst_cast(Uint16*, pixelData), OFstatic_cast(Uint32, length), sizeof(Uint16));
      if ( result.bad() )
      {
        DCMJPEG_ERROR("True lossless encoder: Unable to swap bytes to respect local byte ordering");
        return EC_CannotChangeRepresentation;
      }
      byteSwapped = OFTrue;
    }

    // create initial pixel sequence with empty offset table
    DcmPixelSequence *pixelSequence = NULL;
    DcmPixelItem *offsetTable = NULL;
    if (result.good())
    {
      pixelSequence = new DcmPixelSequence(DCM_PixelSequenceTag);
      if (pixelSequence == NULL) result = EC_MemoryExhausted;
      else
      {
        // create empty offset table
        offsetTable = new DcmPixelItem(DCM_PixelItemTag);
        if (offsetTable == NULL) result = EC_MemoryExhausted;
        else pixelSequence->insert(offsetTable);
      }
    }

    // prepare some variables for encoding
    size_t frameCount = OFstatic_cast(size_t, numberOfFrames);
    size_t frameSize = columns * rows * samplesPerPixel * bytesAllocated;
    const Uint8 *framePointer = OFreinterpret_cast(const Uint8 *, pixelData);
    size_t compressedSize = 0;

    // create encoder corresponding to bit depth (8 or 16 bit)
    DJEncoder *jpeg = createEncoderInstance(toRepParam, djcp, OFstatic_cast(Uint8, bitsAllocated));
    if (jpeg)
    {
      // main loop for compression: compress each frame
      for (unsigned int i=0; i<frameCount && result.good(); i++)
      {
        if (bitsAllocated == 8)
        {
          jpeg->encode(columns, rows, interpr, samplesPerPixel, OFconst_cast(Uint8*, framePointer), jpegData, jpegLen);
        }
        else if (bitsAllocated == 16)
        {
          jpeg->encode(columns, rows, interpr, samplesPerPixel, OFreinterpret_cast(Uint16*, OFconst_cast(Uint8*, framePointer)), jpegData, jpegLen);
        }
        // update variables
        compressedSize+=jpegLen;
        framePointer+=frameSize;
        if (jpegLen == 0)
        {
          DCMJPEG_ERROR("True lossless encoder: Error encoding frame");
          result = EC_CannotChangeRepresentation;
        }
        else
        {
          result = pixelSequence->storeCompressedFrame(offsetList, jpegData, jpegLen, djcp->getFragmentSize());
        }
        // free memory
        delete[] jpegData;
      }
    }
    else
    {
      DCMJPEG_ERROR("True lossless encoder: Cannot allocate encoder instance");
      result = EC_IllegalCall;
    }
    if (result.good())
    {
      compressionRatio = OFstatic_cast(double, bytesAllocated * samplesPerPixel * columns * rows * numberOfFrames) / OFstatic_cast(double, compressedSize);
      pixSeq = pixelSequence;
    }
    else
      delete pixelSequence;
    delete jpeg; // encoder no longer in use

    if ((result.good()) && (djcp->getCreateOffsetTable()))
    {
      // create offset table
      result = offsetTable->createOffsetTable(offsetList);
    }

    // the following operations do not affect the Image Pixel Module
    // but other modules such as SOP Common.  We only perform these
    // changes if we're on the main level of the datsetItem,
    // which should always identify itself as datsetItem, not as item.

    // update derivation description reflecting the JPEG compression applied
    result = updateDerivationDescription(datsetItem, toRepParam, djcp, OFstatic_cast(Uint8, bitsAllocated), compressionRatio);

    if ( (datsetItem->ident() == EVR_dataset) && result.good() )
    {
      // convert to Secondary Capture if requested by user.
      // This method creates a new SOP class UID, so it should be executed
      // after the call to newInstance() which creates a Source Image Sequence.
      if ( djcp->getConvertToSC() || (djcp->getUIDCreation() == EUC_always) )
      {
        if (djcp->getConvertToSC())
        {
          result = DcmCodec::convertToSecondaryCapture(datsetItem);
        }
        // update image type (set to DERIVED)
        if (result.good())
          result = DcmCodec::updateImageType(datsetItem);
        if (result.good())
          result = DcmCodec::newInstance(OFreinterpret_cast(DcmItem*, datsetItem), "DCM", "121320", "Uncompressed predecessor");
      }
    }
    // switch _original_ pixel data back to "color by plane", if required
    if (planConfSwitched)
    {
      if (bytesAllocated == 1)
        result = togglePlanarConfiguration8(OFreinterpret_cast(Uint8*, OFconst_cast(Uint16*, pixelData)), length, samplesPerPixel, OFstatic_cast(Uint16, 0) /*switch to "by plane"*/);
      else
        result = togglePlanarConfiguration16(OFconst_cast(Uint16*, pixelData), length/2, samplesPerPixel, OFstatic_cast(Uint16, 0) /*switch to "by plane"*/);
      if (result.good())
      {
        // update Planar Configuration in dataset
        result = updatePlanarConfiguration(datsetItem, 0 /* update to "by pixel" */);
      }
      else
      {
        DCMJPEG_ERROR("True lossless encoder: Cannot switch back to original planar configuration of the pixel data");
        result = EC_CannotChangeRepresentation;
      }
    }
    // byte swap pixel data back to local endian if necessary
    if (byteSwapped)
      swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, OFconst_cast(Uint16*, pixelData), OFstatic_cast(Uint32, length), sizeof(Uint16));
  }
  return result;
}


void DJCodecEncoder::appendCompressionRatio(
  OFString& arg,
  double ratio)
{
  char buf[64];
  OFStandard::ftoa(buf, sizeof(buf), ratio, OFStandard::ftoa_uppercase, 0, 5);
  arg += buf;
}


OFCondition DJCodecEncoder::updateLossyCompressionRatio(
  DcmItem *dataset,
  double ratio) const
{
  if (dataset == NULL) return EC_IllegalCall;

  // set Lossy Image Compression to "01" (see DICOM part 3, C.7.6.1.1.5)
  OFCondition result = dataset->putAndInsertString(DCM_LossyImageCompression, "01");
  if (result.bad()) return result;

  // set Lossy Image Compression Ratio
  OFString s;
  const char *oldRatio = NULL;
  if ((dataset->findAndGetString(DCM_LossyImageCompressionRatio, oldRatio)).good() && oldRatio)
  {
    s = oldRatio;
    s += "\\";
  }
  appendCompressionRatio(s, ratio);

  result = dataset->putAndInsertString(DCM_LossyImageCompressionRatio, s.c_str());
  if (result.bad()) return result;

  // count VM of lossy image compression ratio
  size_t i;
  size_t s_vm = 0;
  size_t s_sz = s.size();
  for (i = 0; i < s_sz; ++i)
    if (s[i] == '\\') ++s_vm;

  // set Lossy Image Compression Method
  const char *oldMethod = NULL;
  OFString m;
  if ((dataset->findAndGetString(DCM_LossyImageCompressionMethod, oldMethod)).good() && oldMethod)
  {
    m = oldMethod;
    m += "\\";
  }

  // count VM of lossy image compression method
  size_t m_vm = 0;
  size_t m_sz = m.size();
  for (i = 0; i < m_sz; ++i)
    if (m[i] == '\\') ++m_vm;

  // make sure that VM of Compression Method is not smaller than  VM of Compression Ratio
  while (m_vm++ < s_vm) m += "\\";

  m += "ISO_10918_1";
  return dataset->putAndInsertString(DCM_LossyImageCompressionMethod, m.c_str());
}


OFCondition DJCodecEncoder::updateDerivationDescription(
  DcmItem *dataset,
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter *cp,
  Uint8 bitsPerSample,
  double ratio) const
{
  OFString derivationDescription;

  // create new Derivation Description
  createDerivationDescription(toRepParam, cp, bitsPerSample, ratio, derivationDescription);

  // append old Derivation Description, if any
  const char *oldDerivation = NULL;
  if ((dataset->findAndGetString(DCM_DerivationDescription, oldDerivation)).good() && oldDerivation)
  {
    derivationDescription += " [";
    derivationDescription += oldDerivation;
    derivationDescription += "]";
    if (derivationDescription.length() > 1024)
    {
      // ST is limited to 1024 characters, cut off tail
      derivationDescription.erase(1020);
      derivationDescription += "...]";
    }
  }

  OFCondition result = dataset->putAndInsertString(DCM_DerivationDescription, derivationDescription.c_str());
  if (result.good())
  {
    // assume we can cast the codec parameter to what we need
    DJCodecParameter *djcp = OFconst_cast(DJCodecParameter*, cp);

    if (djcp->getTrueLosslessMode())
      result = DcmCodec::insertCodeSequence(dataset, DCM_DerivationCodeSequence, "DCM", "121327", "Full fidelity image");
    else // pseudo-lossless mode may also result in lossy compression
      result = DcmCodec::insertCodeSequence(dataset, DCM_DerivationCodeSequence, "DCM", "113040", "Lossy Compression");
  }
  return result;
}


OFCondition DJCodecEncoder::adjustOverlays(
  DcmItem *dataset,
  DicomImage& image) const
{
  if (dataset == NULL) return EC_IllegalCall;

  unsigned int overlayCount = image.getOverlayCount();
  if (overlayCount > 0)
  {
    Uint16 group = 0;
    DcmStack stack;
    size_t bytesAllocated = 0;
    Uint8 *buffer = NULL;
    unsigned int width = 0;
    unsigned int height = 0;
    long unsigned int frames = 0;
    DcmElement *elem = NULL;
    OFCondition result = EC_Normal;

    // adjust overlays (prior to grayscale compression)
    for (unsigned int i=0; i < overlayCount; i++)
    {
      // check if current overlay is embedded in pixel data
      group = OFstatic_cast(Uint16, image.getOverlayGroupNumber(i));
      stack.clear();
      if ((dataset->search(DcmTagKey(group, 0x3000), stack, ESM_fromHere, OFFalse)).bad())
      {
        // separate Overlay Data not found. Assume overlay is embedded.
        bytesAllocated = image.create6xxx3000OverlayData(buffer, i, width, height, frames);
        if (bytesAllocated > 0)
        {
          elem = new DcmOverlayData(DcmTagKey(group, 0x3000)); // DCM_OverlayData
          if (elem)
          {
            result = elem->putUint8Array(buffer, OFstatic_cast(Uint32, bytesAllocated));
            delete[] buffer;
            if (result.good())
            {
              dataset->insert(elem, OFTrue /*replaceOld*/);
              // DCM_OverlayBitsAllocated
              result = dataset->putAndInsertUint16(DcmTagKey(group, 0x0100), 1);
              // DCM_OverlayBitPosition
              if (result.good()) result = dataset->putAndInsertUint16(DcmTagKey(group, 0x0102), 0);
            }
            else
            {
              delete elem;
              return result;
            }
          }
          else
          {
            delete[] buffer;
            return EC_MemoryExhausted;
          }
        }
        else return EC_IllegalCall;
      }
    }
  }
  return EC_Normal;
}


OFCondition DJCodecEncoder::encodeMonochromeImage(
  DcmItem *dataset,
  const DcmRepresentationParameter * toRepParam,
  DcmPixelSequence * & pixSeq,
  const DJCodecParameter *cp,
  double& compressionRatio) const
{
  OFCondition result = EC_Normal;
  DcmOffsetList offsetList;
  DcmPixelSequence *pixelSequence = NULL;
  DcmPixelItem *offsetTable = NULL;
  unsigned short bitsPerSample = 0;
  compressionRatio = 0.0; // initialize if something goes wrong
  size_t compressedSize = 0;
  double uncompressedSize = 0.0;
  size_t flags = 0; // flags for initialization of DicomImage

  // variables needed if VOI mode is 0
  double minRange = 0.0;
  double maxRange = 0.0;
  double minUsed  = 0.0;
  double maxUsed  = 0.0;
  double rescaleSlope = 1.0;
  double rescaleIntercept = 0.0;
  double voiFactor = 1.0;
  double voiOffset = 0.0;
  double windowCenter = 0.0;
  double windowWidth = 0.0;
  OFBool deleteVOILUT = OFFalse;
  OFBool mode_XA = OFFalse; // true if the current SOP class uses the X-Ray Image Module
  OFBool mode_CT = OFFalse; // true if the current SOP class uses the CT Image Module

  // Modes of operation (if VOI mode is 0)
  OFBool mode_usePixelValues = cp->getUsePixelValues();
  OFBool mode_useModalityRescale = cp->getUseModalityRescale();

  //create flags for DicomImage corresponding to DJCodecParameter options
  if (cp->getAcceptWrongPaletteTags())
    flags |= CIF_WrongPaletteAttributeTags;
  if (cp->getAcrNemaCompatibility())
    flags |= CIF_AcrNemaCompatibility;

  // create DicomImage object. Will fail if dcmimage has not been activated in main().
  // transfer syntax can be any uncompressed one.
  DicomImage dimage(dataset, EXS_LittleEndianImplicit, flags); // read all frames
  if (dimage.getStatus() != EIS_Normal) result = EC_IllegalCall; // should return dimage.getStatus()

  // don't render overlays
  dimage.hideAllOverlays();

  // actual pixel depth of source image which can be different from Bits Stored
  // e. g. when Modality LUT (if enabled) shifts pixel values to a smaller
  // range or if the pixel values itself do not make use of the "Bits Stored"
  // full range available.
  int pixelDepth = dimage.getDepth();

  // create overlay data for embedded overlays
  if (result.good()) result = adjustOverlays(dataset, dimage);

  // VOI transformations should only be applied on the dataset level, not
  // in nested items such as the Icon Image Sequence where we don't expect
  // a VOI window or LUT to be present
  size_t windowType = 0;
  if (dataset->ident() == EVR_dataset)
  {
    windowType = cp->getWindowType();
  }

  // set VOI transformation
  if (result.good())
  {
    switch (windowType)
    {
      case 0: // no VOI transformation
        {
          // disable correction of polarity or any other presentation LUT transformation
          dimage.setPresentationLutShape(ESP_Identity);

          // disable VOI transformation
          dimage.setNoVoiTransformation();

          // look up SOP Class UID, if any
          const char *classUID = NULL;
          dataset->findAndGetString(DCM_SOPClassUID, classUID);

          // SOP Class specifics.
          if (classUID && ! cp->getConvertToSC())
          {
            // these three SOP classes use the X-Ray Image Module in which the meaning
            // of the Modality LUT transformation is "inversed" and, therefore,
            // needs special handling.  This is not an issue if we're converting to
            // secondary capture anyway.
            if ((0 == strcmp(classUID, UID_XRayAngiographicImageStorage)) ||
                (0 == strcmp(classUID, UID_XRayRadiofluoroscopicImageStorage)) ||
                (0 == strcmp(classUID, UID_RETIRED_XRayAngiographicBiPlaneImageStorage)))
            {
              mode_XA = OFTrue;
              mode_useModalityRescale = OFFalse; // inverse definition of Modality LUT Module
            }

            // CT is also a special case because the Modality LUT is required here
            // to convert to Hounsfield units. Again, this is not an issue if we're
            // converting to SC anyway.
            if (0 == strcmp(classUID, UID_CTImageStorage))
            {
              mode_CT = OFTrue;
              mode_useModalityRescale = OFTrue; // required for Hounsfield units
            }
          }

          // query image range and extreme values
          if (result.good())
          {
            // technically possible min/max values
            if (! dimage.getMinMaxValues(minRange, maxRange, 1)) result = EC_IllegalCall;
            if (maxRange <= minRange) result = EC_IllegalCall;
          }

          if (result.good())
          {
            // actually present min/max values in pixel data
            if (! dimage.getMinMaxValues(minUsed, maxUsed, 0)) result = EC_IllegalCall;
            if (maxUsed < minUsed) result = EC_IllegalCall;
          }
        }
        break;
      case 1: // use the n-th VOI window from the image file
        {
          size_t windowParameter = cp->getWindowParameter();
          if ((windowParameter < 1) || (windowParameter > dimage.getWindowCount())) result = EC_IllegalCall;
          if (!dimage.setWindow(windowParameter - 1)) result = EC_IllegalCall;
        }
        break;
      case 2: // use the n-th VOI look up table from the image file
        {
          size_t windowParameter = cp->getWindowParameter();
          if ((windowParameter < 1) || (windowParameter > dimage.getVoiLutCount())) result = EC_IllegalCall;
          if (!dimage.setVoiLut(windowParameter - 1)) result = EC_IllegalCall;
        }
        break;
      case 3: // Compute VOI window using min-max algorithm
        if (!dimage.setMinMaxWindow(0)) result = EC_IllegalCall;
        break;
      case 4: // Compute VOI window using Histogram algorithm, ignoring n percent
        {
          size_t windowParameter = cp->getWindowParameter();
          if (!dimage.setHistogramWindow(OFstatic_cast(double, windowParameter)/100.0)) result = EC_IllegalCall;
        }
        break;
      case 5: // Compute VOI window using center r and width s
        {
          double winCenter=0.0, winWidth=0.0;
          cp->getVOIWindow(winCenter, winWidth);
          if (!dimage.setWindow(winCenter, winWidth)) result = EC_IllegalCall;
        }
        break;
      case 6: // Compute VOI window using min-max algorithm ignoring extremes
        if (!dimage.setMinMaxWindow(1)) result = EC_IllegalCall;
        break;
      case 7: // Compute region of interest VOI window
        {
         size_t left_pos=0, top_pos=0, width=0, height=0;
         cp->getROI(left_pos, top_pos, width, height);
          if (!dimage.setRoiWindow(left_pos, top_pos, width, height)) result = EC_IllegalCall;
        }
        break;
      default: // includes case 0, which must not occur here
        result = EC_IllegalCall;
        break;
    }
  }

  // create initial pixel sequence
  if (result.good())
  {
    pixelSequence = new DcmPixelSequence(DCM_PixelSequenceTag);
    if (pixelSequence == NULL) result = EC_MemoryExhausted;
    else
    {
      // create empty offset table
      offsetTable = new DcmPixelItem(DCM_PixelItemTag);
      if (offsetTable == NULL) result = EC_MemoryExhausted;
      else pixelSequence->insert(offsetTable);
    }
  }

  // select bit depth
  Uint16 compressedBits = OFstatic_cast(Uint16, cp->getForcedBitDepth());
  if (result.good())
  {
    if (compressedBits == 0)
    {
      result = OFreinterpret_cast(DcmItem*, dataset)->findAndGetUint16(DCM_BitsStored, compressedBits);
    }
  }

  // create codec instance
  if (result.good())
  {
    DJEncoder *jpeg = createEncoderInstance(toRepParam, cp, OFstatic_cast(Uint8, compressedBits));
    if (jpeg)
    {
      bitsPerSample = jpeg->bitsPerSample();

      if (windowType == 0)
      {
        // perform image computations
        if (mode_usePixelValues)
        {
          double z = minUsed;
          if (z >= 0) z = floor(z); else z = ceil(z-1023.0);
          double rangeUsed = maxUsed - minUsed + 1; // range that must be covered by window

          // compute optimum window width
          if (pixelDepth < bitsPerSample) windowWidth  = 1 << pixelDepth;
             else windowWidth  = 1 << bitsPerSample;
          while (windowWidth < rangeUsed) windowWidth *= 2;

          windowCenter = windowWidth * 0.5;
          if ((z + windowWidth) > maxUsed) windowCenter += z; else windowCenter += minUsed;
          dimage.setWindow(windowCenter, windowWidth);
        }

        // perform image computations
        if (mode_useModalityRescale)
        {
          // Use Rescale Slope/Intercept to map the decompressed image data
          // back to the original value range, keeping all VOI transformations valid.
          if (mode_usePixelValues)
          {
            rescaleSlope = (windowWidth-1)/((1<<bitsPerSample)-1);
            rescaleIntercept = windowCenter - (windowWidth * 0.5);
          }
          else
          {
            rescaleSlope = (maxRange - minRange)/((1<<bitsPerSample)-1);
            rescaleIntercept = minRange;
          }
        }
        else
        {
          // Don't use Rescale Slope/Intercept (remove or encode as identity transformation).
          // Modify window centers/widths encoded in image, remove VOI LUTs.
          deleteVOILUT = OFTrue;
          if (mode_usePixelValues)
          {
            voiFactor = ((1<<bitsPerSample)-1)/(windowWidth-1); // 1/rescaleSlope
            voiOffset = (windowWidth * 0.5) - windowCenter; // - rescaleIntercept
          }
          else
          {
            voiFactor = ((1<<bitsPerSample)-1)/(maxRange - minRange); // 1/rescaleSlope
            voiOffset = -minRange; // - rescaleIntercept
          }
        }
      }

      // render and compress each frame
      size_t frameCount = dimage.getFrameCount();
      unsigned short bytesPerSample = jpeg->bytesPerSample();
      unsigned short columns = OFstatic_cast(unsigned short, dimage.getWidth());
      unsigned short rows = OFstatic_cast(unsigned short, dimage.getHeight());
      Uint8 *jpegData = NULL;
      Uint32 jpegLen  = 0;
      const void *frame = NULL;

      // compute original image size in bytes, ignoring any padding bits.
      Uint16 samplesPerPixel = 0;
      if ((dataset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel)).bad()) samplesPerPixel = 1;
      uncompressedSize = OFstatic_cast(double, columns * rows * pixelDepth * frameCount * samplesPerPixel) / 8.0;
      for (size_t i=0; (i<frameCount) && (result.good()); i++)
      {
        frame = dimage.getOutputData(bitsPerSample, i, 0);
        if (frame == NULL) result = EC_MemoryExhausted;
        else
        {
          // compress frame
          jpegData = NULL;
          if (bytesPerSample == 1)
          {
            result = jpeg->encode(columns, rows, EPI_Monochrome2, 1, OFreinterpret_cast(Uint8*, OFconst_cast(void*, frame)), jpegData, jpegLen);
          } else {
            result = jpeg->encode(columns, rows, EPI_Monochrome2, 1, OFreinterpret_cast(Uint16*, OFconst_cast(void*, frame)), jpegData, jpegLen);
          }

          // store frame
          if (result.good())
          {
            result = pixelSequence->storeCompressedFrame(offsetList, jpegData, jpegLen, cp->getFragmentSize());
          }

          // delete block of JPEG data
          delete[] jpegData;
          compressedSize += jpegLen;
        }
      }
      delete jpeg;
    } else result = EC_MemoryExhausted;
  }

  // store pixel sequence if everything went well.
  if (result.good()) pixSeq = pixelSequence;
  else
  {
    delete pixelSequence;
    pixSeq = NULL;
  }

  if ((result.good()) && (cp->getCreateOffsetTable()))
  {
    // create offset table
    result = offsetTable->createOffsetTable(offsetList);
  }

  if (result.good())
  {
    // adapt attributes in image pixel module
    if (result.good()) result = dataset->putAndInsertUint16(DCM_SamplesPerPixel, 1);
    if (result.good() && (windowType != 0)) result = dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
    if (result.good())
    {
      if (bitsPerSample > 8)
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 16);
      else
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 8);
    }
    if (result.good()) result = dataset->putAndInsertUint16(DCM_BitsStored, bitsPerSample);
    if (result.good()) result = dataset->putAndInsertUint16(DCM_HighBit, OFstatic_cast(Uint16, bitsPerSample-1));
    if (result.good()) result = dataset->putAndInsertUint16(DCM_PixelRepresentation, 0);
    delete dataset->remove(DCM_PlanarConfiguration);
    delete dataset->remove(DCM_SmallestImagePixelValue);
    delete dataset->remove(DCM_LargestImagePixelValue);
    delete dataset->remove(DCM_RedPaletteColorLookupTableDescriptor);
    delete dataset->remove(DCM_GreenPaletteColorLookupTableDescriptor);
    delete dataset->remove(DCM_BluePaletteColorLookupTableDescriptor);
    delete dataset->remove(DCM_RedPaletteColorLookupTableData);
    delete dataset->remove(DCM_GreenPaletteColorLookupTableData);
    delete dataset->remove(DCM_BluePaletteColorLookupTableData);
    delete dataset->remove(DCM_PixelPaddingValue);
    delete dataset->remove(DCM_SmallestPixelValueInSeries);
    delete dataset->remove(DCM_LargestPixelValueInSeries);
    delete dataset->remove(DCM_PaletteColorLookupTableUID);
    delete dataset->remove(DCM_SegmentedRedPaletteColorLookupTableData);
    delete dataset->remove(DCM_SegmentedGreenPaletteColorLookupTableData);
    delete dataset->remove(DCM_SegmentedBluePaletteColorLookupTableData);

    // check if either Modality LUT Sequence or Rescale Slope are present
    DcmStack stack;
    OFBool foundModalityLUT = OFFalse;
    if ((dataset->search(DCM_ModalityLUTSequence, stack, ESM_fromHere, OFFalse)).good()) foundModalityLUT = OFTrue;
    if (! foundModalityLUT)
    {
      stack.clear();
      if ((dataset->search(DCM_RescaleSlope, stack, ESM_fromHere, OFFalse)).good()) foundModalityLUT = OFTrue;
    }

    // delete old Modality transformation
    delete dataset->remove(DCM_ModalityLUTSequence);
    delete dataset->remove(DCM_RescaleIntercept);
    delete dataset->remove(DCM_RescaleSlope);
    delete dataset->remove(DCM_RescaleType);

    // update Modality LUT Module and Pixel Intensity Relationship
    if (windowType == 0)
    {
      if (mode_XA) // XA needs special handling
      {
        // XA Mode: set Pixel Intensity Relationship to "DISP", no Modality LUT
        if (result.good()) result = dataset->putAndInsertString(DCM_PixelIntensityRelationship, "DISP");
      }
      /* else if we had a modality LUT before, a LUT is inserted again.
         or if specific rescale slope/intercept has been computed, use that in image
       */
      else if (foundModalityLUT || rescaleSlope != 1.0 || rescaleIntercept != 0.0)
      {
        char buf[64];
        OFStandard::ftoa(buf, sizeof(buf), rescaleIntercept, OFStandard::ftoa_uppercase, 0, 6);
        if (result.good()) result = dataset->putAndInsertString(DCM_RescaleIntercept, buf);
        OFStandard::ftoa(buf, sizeof(buf), rescaleSlope, OFStandard::ftoa_uppercase, 0, 6);
        if (result.good()) result = dataset->putAndInsertString(DCM_RescaleSlope, buf);
        if (result.good())
        {
          if (mode_CT) result = dataset->putAndInsertString(DCM_RescaleType, "HU"); // Hounsfield units
          else result =         dataset->putAndInsertString(DCM_RescaleType, "US"); // unspecified
        }
      }
    }
    else
    {
      // if we had found a Modality LUT Transformation, create a identity LUT transformation
      if (foundModalityLUT)
      {
        if (result.good()) result = dataset->putAndInsertString(DCM_RescaleIntercept, "0");
        if (result.good()) result = dataset->putAndInsertString(DCM_RescaleSlope, "1");
        if (result.good()) result = dataset->putAndInsertString(DCM_RescaleType, "US"); // unspecified
      }

      // Adjust Pixel Intensity Relationship (needed for XA). If present, set value to "DISP".
      stack.clear();
      if ((dataset->search(DCM_PixelIntensityRelationship, stack, ESM_fromHere, OFFalse)).good())
      {
        if (result.good()) result = dataset->putAndInsertString(DCM_PixelIntensityRelationship, "DISP");
      }
    }

    // Adjust VOI LUT and Presentation LUT transformation
    if (windowType == 0)
    {
      if (deleteVOILUT) delete dataset->remove(DCM_VOILUTSequence);

      // Adjust window center/width
      if (result.good()) result = correctVOIWindows(dataset, voiOffset, voiFactor);

      // Don't modify Presentation LUT transformations (if any)
    }
    else
    {
      delete dataset->remove(DCM_VOILUTSequence);
      delete dataset->remove(DCM_WindowCenter);
      delete dataset->remove(DCM_WindowWidth);
      delete dataset->remove(DCM_WindowCenterWidthExplanation);

      // Adjust Presentation LUT Transformation.
      stack.clear();
      OFBool foundPresentationLUT = OFFalse;
      if ((dataset->search(DCM_PresentationLUTSequence, stack, ESM_fromHere, OFFalse)).good()) foundPresentationLUT = OFTrue;
      if (! foundPresentationLUT)
      {
        stack.clear();
        if ((dataset->search(DCM_PresentationLUTShape, stack, ESM_fromHere, OFFalse)).good()) foundPresentationLUT = OFTrue;
      }

      // delete old Presentation LUT transformation
      delete dataset->remove(DCM_PresentationLUTSequence);
      delete dataset->remove(DCM_PresentationLUTShape);

      // if we had found a Presentation LUT Transformation, create a new identity transformation
      if (foundPresentationLUT)
      {
        if (result.good()) result = dataset->putAndInsertString(DCM_PresentationLUTShape, "IDENTITY");
      }
    }
  }

  // determine compression ratio
  if (compressedSize > 0) compressionRatio = OFstatic_cast(double, uncompressedSize) / OFstatic_cast(double, compressedSize);
  return result;
}


OFCondition DJCodecEncoder::correctVOIWindows(
  DcmItem *dataset,
  double voiOffset,
  double voiFactor)
{
  if (voiOffset == 0.0 && voiFactor == 1.0) return EC_Normal;

  OFCondition result = EC_Normal;
  DcmElement *center = NULL;
  DcmElement *width = NULL;
  DcmElement *explanation = NULL;

  DcmStack stack;
  if ((dataset->search(DCM_WindowCenter, stack, ESM_fromHere, OFFalse)).good())
  {
    center = OFreinterpret_cast(DcmElement*, stack.top());
  }
  stack.clear();
  if ((dataset->search(DCM_WindowWidth, stack, ESM_fromHere, OFFalse)).good())
  {
    width = OFreinterpret_cast(DcmElement*, stack.top());
  }
  stack.clear();
  if ((dataset->search(DCM_WindowCenterWidthExplanation, stack, ESM_fromHere, OFFalse)).good())
  {
    explanation = OFreinterpret_cast(DcmElement*, stack.top());
  }

  OFString newCenter;
  OFString newWidth;
  OFString newExplanation;
  Float64 currentCenter = 0.0;
  Float64 currentWidth = 0.0;
  OFString currentExplanation;
  double tempCenter = 0.0;
  double tempWidth = 0.0;
  char buf[64];

  if (center && width)
  {
    size_t numWindows = center->getVM();
    // iterate over all defined VOI windows
    for (size_t i=0; i<numWindows; i++)
    {
      if (((center->getFloat64(currentCenter,OFstatic_cast(Uint32, i))).good()) && ((width->getFloat64(currentWidth,OFstatic_cast(Uint32, i))).good()))
      {
        // found one pair of values, adapt them to value range shifted pixel data
        tempCenter = (currentCenter+voiOffset)*voiFactor;
        tempWidth = currentWidth * voiFactor;
        // add this window to the attribute values that are later replacing old windows
        OFStandard::ftoa(buf, sizeof(buf), tempCenter, OFStandard::ftoa_uppercase, 0, 6);
        if (!newCenter.empty()) newCenter += "\\";
        newCenter += buf;
        OFStandard::ftoa(buf, sizeof(buf), tempWidth, OFStandard::ftoa_uppercase, 0, 6);
        if (!newWidth.empty()) newWidth += "\\";
        newWidth += buf;

        if (!newExplanation.empty()) newExplanation += "\\";
        if (explanation && ((explanation->getOFString(currentExplanation,OFstatic_cast(Uint32, i))).good()))
          newExplanation += currentExplanation;
          else
          {
            newExplanation += "center=";
            OFStandard::ftoa(buf, sizeof(buf), tempCenter, OFStandard::ftoa_uppercase, 0, 6);
            newExplanation += buf;
            newExplanation += "/width=";
            OFStandard::ftoa(buf, sizeof(buf), tempWidth, OFStandard::ftoa_uppercase, 0, 6);
            newExplanation += buf;
          }
      }
    }
  }

  // remove old windows
  delete dataset->remove(DCM_WindowCenter);
  delete dataset->remove(DCM_WindowWidth);
  delete dataset->remove(DCM_WindowCenterWidthExplanation);

  // and insert newly computed ones if necessary
  if (!newCenter.empty())
  {
    if (result.good()) result = dataset->putAndInsertString(DCM_WindowCenter, newCenter.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_WindowWidth, newWidth.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_WindowCenterWidthExplanation, newExplanation.c_str());
  }

  return result;
}


OFCondition DJCodecEncoder::togglePlanarConfiguration8(
  Uint8 *pixelData,
  const size_t numValues,
  const Uint16 samplesPerPixel,
  const Uint16 oldPlanarConfig)
{
  if (pixelData == NULL)
    return EC_IllegalParameter;
  // allocate target buffer
  Uint8* px8 = new Uint8[numValues];
  if (!px8)
    return EC_MemoryExhausted;
  size_t numPixels = numValues / samplesPerPixel;
  if (oldPlanarConfig == 1)   // change from "by plane" to "by pixel"
  {
    for (size_t n=0; n < numPixels; n++)
    {
        for (Uint16 s=0; s < samplesPerPixel; s++)
          px8[n*samplesPerPixel+s]   = pixelData[n+numPixels*s];
    }
  }
  else  //change from "by pixel" to "by plane"
  {
    for (size_t n=0; n < numPixels; n++)
    {
        for (Uint16 s=0; s < samplesPerPixel; s++)
          px8[n+numPixels*s]   = pixelData[n*samplesPerPixel+s];
    }
  }
  // copy filled buffer to pixel data and free memory
  memcpy(pixelData, px8, OFstatic_cast(size_t, numValues));
  delete[] px8;
  return EC_Normal;
}


OFCondition DJCodecEncoder::togglePlanarConfiguration16(
  Uint16 *pixelData,
  const size_t numValues, //number of 16-bit components
  const Uint16 samplesPerPixel,
  const Uint16 oldPlanarConfig)
{
  if (pixelData == NULL)
    return EC_IllegalParameter;
  // allocate target buffer
  Uint16* px16 = new Uint16[numValues];
  if (!px16)
    return EC_MemoryExhausted;
  size_t numPixels = numValues / samplesPerPixel;
  if (oldPlanarConfig == 1)   // change from "by plane" to "by pixel"
  {
    for (size_t n=0; n < numPixels; n++)
    {
        for (Uint16 s=0; s < samplesPerPixel; s++)
          px16[n*samplesPerPixel+s]   = pixelData[n+numPixels*s];
    }
  }
  else  //change from "by pixel" to "by plane"
  {
    for (size_t n=0; n < numPixels; n++)
    {
        for (Uint16 s=0; s < samplesPerPixel; s++)
          px16[n+numPixels*s]   = pixelData[n*samplesPerPixel+s];
    }
  }
  // copy filled buffer to pixel data and free memory
  memcpy(pixelData, px16, OFstatic_cast(size_t, numValues*2));
  delete[] px16;
  return EC_Normal;
}


OFCondition DJCodecEncoder::updatePlanarConfiguration(
  DcmItem *item,
  const Uint16 newPlanConf) const
{
  if ( (item == NULL) || (newPlanConf) > 1)
    return EC_IllegalParameter;
  return item->putAndInsertUint16(DCM_PlanarConfiguration, newPlanConf);
}
