/*
 *
 *  Copyright (C) 1997-2001, OFFIS
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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: abstract codec class for JPEG encoders.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-12-04 10:42:12 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djcodece.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djcodece.h"

// ofstd includes
#include "oflist.h"
#include "ofstd.h"

// dcmdata includes
#include "dcdatset.h"  /* for class DcmDataset */
#include "dcdeftag.h"  /* for tag constants */
#include "dcovlay.h"   /* for class DcmOverlayData */
#include "dcpixseq.h"  /* for class DcmPixelSequence */
#include "dcpxitem.h"  /* for class DcmPixelItem */
#include "dcuid.h"     /* for dcmGenerateUniqueIdentifer()*/
#include "dcvrcs.h"    /* for class DcmCodeString */
#include "dcvrds.h"    /* for class DcmDecimalString */
#include "dcvrlt.h"    /* for class DcmLongText */
#include "dcvrst.h"    /* for class DcmShortText */
#include "dcvrus.h"    /* for class DcmUnsignedShort */
#include "dcswap.h"    /* for swapIfNecessary */

// dcmjpeg includes
#include "djcparam.h"  /* for class DJCodecParameter */
#include "djencabs.h"  /* for class DJEncoder */

// dcmimgle includes
#include "dcmimage.h"  /* for class DicomImage */

#define INCLUDE_CMATH
#include "ofstdinc.h"


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

  // we don't support re-coding for now.
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

OFCondition DJCodecEncoder::encode(
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
  const DJCodecParameter *djcp = (const DJCodecParameter *)cp;
  DcmStack localStack(objStack);
  (void)localStack.pop();             // pop pixel data element from stack
  DcmObject *dataset = localStack.pop(); // this is the item in which the pixel data is located

  if ((!dataset)||((dataset->ident()!= EVR_dataset) && (dataset->ident()!= EVR_item))) result = EC_InvalidTag;
  else
  {
    double compressionRatio = 0.0;
    EP_Interpretation photometricInterpretation = DcmJpegHelper::getPhotometricInterpretation((DcmItem *)dataset);
    switch (photometricInterpretation)
    {
      case EPI_Monochrome1:
      case EPI_Monochrome2:
        // monochrome image
        result = encodeMonochromeImage((DcmItem *)dataset, toRepParam, pixSeq, djcp, compressionRatio);
        break;
      case EPI_PaletteColor:
      case EPI_RGB:
      case EPI_HSV:
      case EPI_ARGB:
      case EPI_CMYK:
      case EPI_YBR_Partial_422:
        // color image except YCbCr which receives special treatment
        result = encodeColorImage(OFFalse, (DcmItem *)dataset, toRepParam, pixSeq, djcp, compressionRatio);
        break;
      case EPI_YBR_Full:
      case EPI_YBR_Full_422:
        // YCbCr color image
        result = encodeColorImage(OFTrue, (DcmItem *)dataset, toRepParam, pixSeq, djcp, compressionRatio);
        break;
      case EPI_Unknown:
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
      if (result.good()) result = DcmCodec::updateImageType((DcmItem *)dataset);
      
      // determine compressed bit depth passed to JPEG codec
      Uint16 compressedBits = djcp->getForcedBitDepth();
      if (result.good())
      {
        if (compressedBits == 0)
        {
          result = ((DcmItem *)dataset)->findAndGetUint16(DCM_BitsStored, compressedBits);
        }  
      }
    
      // update derivation description
      if (result.good()) result = updateDerivationDescription((DcmItem *)dataset, toRepParam, 
        djcp, (Uint8)compressedBits, compressionRatio);
      
      if (result.good())
      {
        if (isLosslessProcess())
        {
          // lossless process - create new UID if mode is EUC_always or if we're converting to Secondary Capture
          if (djcp->getConvertToSC() || (djcp->getUIDCreation() == EUC_always)) result = DcmCodec::newInstance((DcmItem *)dataset);
        }
        else
        {
          // lossy process - create new UID unless mode is EUC_never and we're not converting to Secondary Capture
          if (djcp->getConvertToSC() || (djcp->getUIDCreation() != EUC_never)) result = DcmCodec::newInstance((DcmItem *)dataset);
        
          // update lossy compression ratio
          if (result.good()) result = updateLossyCompressionRatio((DcmItem *)dataset, compressionRatio);
        }
      }

      // convert to Secondary Capture if requested by user.  
      // This method creates a new SOP class UID, so it should be executed
      // after the call to newInstance() which creates a Source Image Sequence.
      if (result.good() && djcp->getConvertToSC()) result = DcmCodec::convertToSecondaryCapture((DcmItem *)dataset);
    }
  }

  return result;
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
  unsigned long compressedSize = 0;
  double uncompressedSize = 0.0;
  Uint16 compressedBits = cp->getForcedBitDepth();
  
  // initialize settings with defaults for RGB mode
  OFBool monochromeMode = OFFalse;
  unsigned long flags = 0;
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
    pixelSequence = new DcmPixelSequence(DcmTag(DCM_PixelData,EVR_OB));
    if (pixelSequence == NULL) result = EC_MemoryExhausted;
    else
    {
      // create empty offset table
      offsetTable = new DcmPixelItem(DcmTag(DCM_Item,EVR_OB));
      if (offsetTable == NULL) result = EC_MemoryExhausted;
      else pixelSequence->insert(offsetTable);
    }
  }

  // select bit depth
  if (result.good())
  {
    if (compressedBits == 0)
    {
      result = ((DcmItem *)dataset)->findAndGetUint16(DCM_BitsStored, compressedBits);
    }  
  }

  // create codec instance
  if (result.good())
  {
    DJEncoder *jpeg = createEncoderInstance(toRepParam, cp, (Uint8) compressedBits);
    if (jpeg)
    {
      // render and compress each frame
      bitsPerSample = jpeg->bitsPerSample();
      unsigned long frameCount = dimage->getFrameCount();
      unsigned short bytesPerSample = jpeg->bytesPerSample();      
      unsigned short columns = (unsigned short) dimage->getWidth();
      unsigned short rows = (unsigned short) dimage->getHeight();
      Uint8 *jpegData = NULL;
      Uint32 jpegLen  = 0;      
      const void *frame = NULL;
      
      // compute original image size in bytes, ignoring any padding bits. 
      uncompressedSize = columns * rows * dimage->getDepth() * frameCount * samplesPerPixel / 8.0;
      for (unsigned long i=0; (i<frameCount) && (result.good()); i++)
      {
        frame = dimage->getOutputData(bitsPerSample, i, 0);
        if (frame == NULL) result = EC_MemoryExhausted;
        else
        {
          // compress frame
          jpegData = NULL;
          if (bytesPerSample == 1)
          {
            result = jpeg->encode(columns, rows, interpr, samplesPerPixel, (Uint8 *)frame, jpegData, jpegLen);
          } else {
            result = jpeg->encode(columns, rows, interpr, samplesPerPixel, (Uint16 *)frame, jpegData, jpegLen);
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
    if (result.good()) result = dataset->putAndInsertUint16(DCM_HighBit, bitsPerSample-1);
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
    delete dataset->remove(DCM_SmallestPixelValueInSeries);
    delete dataset->remove(DCM_LargestPixelValueInSeries);
    delete dataset->remove(DCM_PaletteColorLookupTableUID);
    delete dataset->remove(DCM_SegmentedRedPaletteColorLookupTableData);
    delete dataset->remove(DCM_SegmentedGreenPaletteColorLookupTableData);
    delete dataset->remove(DCM_SegmentedBluePaletteColorLookupTableData);
  }
  if (compressedSize > 0) compressionRatio = uncompressedSize / compressedSize;

  delete dimage;
  return result;
}

void DJCodecEncoder::appendCompressionRatio(OFString& arg, double ratio)
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

  return dataset->putAndInsertString(DCM_LossyImageCompressionRatio, s.c_str());
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

  return dataset->putAndInsertString(DCM_DerivationDescription, derivationDescription.c_str());
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
    unsigned long bytesAllocated = 0;
    Uint8 *buffer = NULL;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned long frames = 0;
    DcmElement *elem = NULL;
    OFCondition result = EC_Normal;
    
    // adjust overlays (prior to grayscale compression)
    for (unsigned int i=0; i < overlayCount; i++)
    {
      // check if current overlay is embedded in pixel data
      group = (Uint16) image.getOverlayGroupNumber(i);
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
            result = elem->putUint8Array(buffer, bytesAllocated);
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
  unsigned long compressedSize = 0;
  double uncompressedSize = 0.0;

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

  // create dcmimage object. Will fail if dcmimage has not been activated in main().
  // transfer syntax can be any uncompressed one.
  DicomImage dimage(dataset, EXS_LittleEndianImplicit, 0); // read all frames
  if (dimage.getStatus() != EIS_Normal) result = EC_IllegalCall; // should return dimage.getStatus()

  // don't render overlays
  dimage.hideAllOverlays();

  // original pixel depth of source image
  int pixelDepth = dimage.getDepth();

  // create overlay data for embedded overlays
  if (result.good()) result = adjustOverlays(dataset, dimage);

  // set VOI transformation
  if (result.good())
  {
    switch (cp->getWindowType())
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
            // of the Modality LUT transformation is "inversersed" and, therefore,
            // needs special handling.  This is not an issue if we're converting to
            // secondary capture anyway.
            if ((0 == strcmp(classUID, UID_XRayAngiographicImageStorage)) ||
                (0 == strcmp(classUID, UID_XRayFluoroscopyImageStorage)) ||
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
            if (! dimage.getMinMaxValues(minRange, maxRange, 1)) result = EC_IllegalCall;
            if (maxRange <= minRange) result = EC_IllegalCall;
          }    
          
          if (result.good()) 
          {
            if (! dimage.getMinMaxValues(minUsed, maxUsed, 0)) result = EC_IllegalCall;
            if (maxUsed < minUsed) result = EC_IllegalCall;
          }              
        }      
        break;
      case 1: // use the n-th VOI window from the image file
        {
          unsigned long windowParameter = cp->getWindowParameter();
          if ((windowParameter < 1) || (windowParameter > dimage.getWindowCount())) result = EC_IllegalCall;
          if (!dimage.setWindow(windowParameter - 1)) result = EC_IllegalCall;
        }      
        break;
      case 2: // use the n-th VOI look up table from the image file
        {
          unsigned long windowParameter = cp->getWindowParameter();
          if ((windowParameter < 1) || (windowParameter > dimage.getVoiLutCount())) result = EC_IllegalCall;
          if (!dimage.setVoiLut(windowParameter - 1)) result = EC_IllegalCall;
        }      
        break;
      case 3: // Compute VOI window using min-max algorithm
        if (!dimage.setMinMaxWindow(0)) result = EC_IllegalCall;
        break;
      case 4: // Compute VOI window using Histogram algorithm, ignoring n percent
        {
          unsigned long windowParameter = cp->getWindowParameter();       
          if (!dimage.setHistogramWindow(((double)windowParameter)/100.0)) result = EC_IllegalCall;
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
         unsigned long left=0, top=0, width=0, height=0;
         cp->getROI(left, top, width, height);    
          if (!dimage.setRoiWindow(left, top, width, height)) result = EC_IllegalCall;
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
    pixelSequence = new DcmPixelSequence(DcmTag(DCM_PixelData,EVR_OB));
    if (pixelSequence == NULL) result = EC_MemoryExhausted;
    else
    {
      // create empty offset table
      offsetTable = new DcmPixelItem(DcmTag(DCM_Item,EVR_OB));
      if (offsetTable == NULL) result = EC_MemoryExhausted;
      else pixelSequence->insert(offsetTable);
    }
  }

  // select bit depth
  Uint16 compressedBits = cp->getForcedBitDepth();
  if (result.good())
  {
    if (compressedBits == 0)
    {
      result = ((DcmItem *)dataset)->findAndGetUint16(DCM_BitsStored, compressedBits);
    }  
  }

  // create codec instance
  if (result.good())
  {
    DJEncoder *jpeg = createEncoderInstance(toRepParam, cp, (Uint8) compressedBits);
    if (jpeg)
    {
      bitsPerSample = jpeg->bitsPerSample();

      if (cp->getWindowType() == 0)
      {
        // perform image computations
        if (mode_usePixelValues)
        {
          double z = minUsed;
          if (z >= 0) z = floor(z); else z = ceil(z-1023.0);
          double rangeUsed = maxUsed - minUsed + 1; // range that must be covered by window

          // compute optimum window width
          if ((unsigned long)pixelDepth < (unsigned long)bitsPerSample) windowWidth  = 1 << pixelDepth;
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
      unsigned long frameCount = dimage.getFrameCount();
      unsigned short bytesPerSample = jpeg->bytesPerSample();      
      unsigned short columns = (unsigned short) dimage.getWidth();
      unsigned short rows = (unsigned short) dimage.getHeight();
      Uint8 *jpegData = NULL;
      Uint32 jpegLen  = 0;      
      const void *frame = NULL;
      
      // compute original image size in bytes, ignoring any padding bits. 
      Uint16 samplesPerPixel = 0;
      if ((dataset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel)).bad()) samplesPerPixel = 1;
      uncompressedSize = columns * rows * pixelDepth * frameCount * samplesPerPixel / 8.0;

      for (unsigned long i=0; (i<frameCount) && (result.good()); i++)
      {
        frame = dimage.getOutputData(bitsPerSample, i, 0);
        if (frame == NULL) result = EC_MemoryExhausted;
        else
        {
          // compress frame
          jpegData = NULL;
          if (bytesPerSample == 1)
          {
            result = jpeg->encode(columns, rows, EPI_Monochrome2, 1, (Uint8 *)frame, jpegData, jpegLen);
          } else {
            result = jpeg->encode(columns, rows, EPI_Monochrome2, 1, (Uint16 *)frame, jpegData, jpegLen);
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
    if (result.good() && (cp->getWindowType() != 0)) result = dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
    if (result.good())
    {
      if (bitsPerSample > 8)
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 16);
      else
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 8);
    }
    if (result.good()) result = dataset->putAndInsertUint16(DCM_BitsStored, bitsPerSample);
    if (result.good()) result = dataset->putAndInsertUint16(DCM_HighBit, bitsPerSample-1);
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
    if (cp->getWindowType() == 0)
    {
      if (mode_XA)
      {
        // XA Mode: set Pixel Intensity Relationship to "DISP", no Modality LUT
        if (result.good()) result = dataset->putAndInsertString(DCM_PixelIntensityRelationship, "DISP");
      }
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
      // if we had found a Modality LUT Transformation, create a new identity transformation
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
    if (cp->getWindowType() == 0)
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
  if (compressedSize > 0) compressionRatio = uncompressedSize / compressedSize;
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
    center = (DcmElement *)stack.top();
  }
  stack.clear();
  if ((dataset->search(DCM_WindowWidth, stack, ESM_fromHere, OFFalse)).good())
  {
    width = (DcmElement *)stack.top();
  }
  stack.clear();
  if ((dataset->search(DCM_WindowCenterWidthExplanation, stack, ESM_fromHere, OFFalse)).good())
  {
    explanation = (DcmElement *)stack.top();
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
    unsigned long numWindows = center->getVM();
    for (unsigned long i=0; i<numWindows; i++)
    {
      if (((center->getFloat64(currentCenter,i)).good()) && ((width->getFloat64(currentWidth,i)).good()))
      {
        // found one pair of values
        tempCenter = (currentCenter+voiOffset)*voiFactor;
        tempWidth = currentWidth * voiFactor;
        OFStandard::ftoa(buf, sizeof(buf), tempCenter, OFStandard::ftoa_uppercase, 0, 6);
        if (newCenter.length() > 0) newCenter += "\\";
        newCenter += buf;
        OFStandard::ftoa(buf, sizeof(buf), tempWidth, OFStandard::ftoa_uppercase, 0, 6);
        if (newWidth.length() > 0) newWidth += "\\";
        newWidth += buf;

        if (newExplanation.length() > 0) newExplanation += "\\";
        if (explanation && ((explanation->getOFString(currentExplanation,i)).good()))
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

  delete dataset->remove(DCM_WindowCenter);
  delete dataset->remove(DCM_WindowWidth);
  delete dataset->remove(DCM_WindowCenterWidthExplanation);  

  if (newCenter.length() > 0)
  {
    if (result.good()) result = dataset->putAndInsertString(DCM_WindowCenter, newCenter.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_WindowWidth, newWidth.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_WindowCenterWidthExplanation, newExplanation.c_str());
  }  

  return result;
}


/*
 * CVS/RCS Log
 * $Log: djcodece.cc,v $
 * Revision 1.10  2002-12-04 10:42:12  meichel
 * Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *   double to string conversions that are supposed to be locale independent
 *
 * Revision 1.9  2002/11/27 15:39:59  meichel
 * Adapted module dcmjpeg to use of new header file ofstdinc.h
 *
 * Revision 1.8  2002/07/08 16:13:19  meichel
 * Fixed dcmjpeg encoder: non Image Pixel module attributes are now created on
 *   dataset level only, not inside sequence items (such as the Icon Image
 *   Sequence)
 *
 * Revision 1.7  2002/05/24 14:59:51  meichel
 * Moved helper methods that are useful for different compression techniques
 *   from module dcmjpeg to module dcmdata
 *
 * Revision 1.6  2002/01/25 13:44:53  meichel
 * Fixed minor bug in code that converts a DICOM object to secondary capture
 *
 * Revision 1.5  2002/01/08 10:30:13  joergr
 * Corrected spelling of function dcmGenerateUniqueIdentifier().
 * Changed prefix of UIDs created for series and studies (now using constants
 * SITE_SERIES_UID_ROOT and SITE_STUDY_UID_ROOT which are supposed to be used
 * in these cases).
 *
 * Revision 1.4  2001/12/20 10:41:50  meichel
 * Fixed warnings reported by Sun CC 2.0.1
 *
 * Revision 1.3  2001/11/28 13:48:16  joergr
 * Check return value of DcmItem::insert() statements where appropriate to
 * avoid memory leaks when insert procedure fails.
 *
 * Revision 1.2  2001/11/14 09:30:47  meichel
 * Minor modifications for Visual C++
 *
 * Revision 1.1  2001/11/13 15:58:24  meichel
 * Initial release of module dcmjpeg
 *
 *
 */

