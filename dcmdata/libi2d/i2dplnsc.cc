/*
 *
 *  Copyright (C) 2001-2025, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: Implements conversion from image into new DICOM SC IODs
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2dplnsc.h"
#include "dcmtk/dcmdata/dcdeftag.h"        /* for DCM_ defines */
#include "dcmtk/dcmdata/dcuid.h"           /* for UID_ defines */
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"
#include "dcmtk/dcmdata/dcdatset.h"


I2DOutputPlugNewSC::I2DOutputPlugNewSC()
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugNewSC: Output plugin for new Multiframe Secondary Capture SOPs initialized");
}


I2DOutputPlugNewSC::~I2DOutputPlugNewSC()
{
}


OFString I2DOutputPlugNewSC::ident()
{
  return "New Secondary Capture Image SOP Classes";
}


void I2DOutputPlugNewSC::supportedSOPClassUIDs(OFList<OFString>& suppSOPs)
{
  suppSOPs.push_back(UID_MultiframeSingleBitSecondaryCaptureImageStorage);
  suppSOPs.push_back(UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage);
  suppSOPs.push_back(UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage);
  suppSOPs.push_back(UID_MultiframeTrueColorSecondaryCaptureImageStorage);
}


OFCondition I2DOutputPlugNewSC::convert(DcmDataset &dataset) const
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugNewSC: Inserting new Multiframe SC specific attributes");

  // Find out which of the new SC SOP Class UIDs to use
  OFString sopUID, photInt; Uint16 bitsAllocated;
  OFCondition cond = dataset.findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Unable to determine correct SOP class due to missing Image Pixel module information");
  cond = dataset.findAndGetOFStringArray(DCM_PhotometricInterpretation, photInt);

  if (bitsAllocated == 1)
    cond = handle1BitSC(&dataset);
  else if (bitsAllocated == 8)
    cond = handle8BitSC(&dataset);
  else if (bitsAllocated == 16)
    cond = handle16BitSC(&dataset);
  else
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Bits Allocated needs a value of 1, 8 or 16 for conversion");

  return cond;
}


OFString I2DOutputPlugNewSC::isValid(DcmDataset& dataset) const
{
  OFString err;
  // Just return if checking was disabled
  if (!m_doAttribChecking)
    return err;

  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugNewSC: Checking new Multiframe SC specific attributes");
  // WSD="Workstation", found in most Secondary Capture files
  err += checkAndInventType1Attrib(DCM_ConversionType, &dataset, "WSD");
  // As a default we do not expect the image to have burnt in annotations
  err += checkAndInventType1Attrib(DCM_BurnedInAnnotation, &dataset, "NO");

  return err;
}


OFCondition I2DOutputPlugNewSC::handle1BitSC(DcmDataset *dataset) const
{
  if (!dataset)
    return EC_IllegalParameter;

  OFCondition cond; Uint16 u16 = 0; OFString str;
  cond = dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, str);
  if (cond.bad() || (str != "MONOCHROME2"))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric Interpretation does not fit SOP class");

  cond = dataset->findAndGetUint16(DCM_SamplesPerPixel, u16);
  if (cond.bad() || (u16 != 1))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Samples Per Pixel does not fit SOP class");

  cond = dataset->findAndGetUint16(DCM_BitsStored, u16);
  if (cond.bad() || (u16 != 1))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Bits Stored does not fit SOP class");

  cond = dataset->findAndGetUint16(DCM_HighBit, u16);
  if (cond.bad() || (u16 != 0))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: High Bit does not fit SOP class");

  cond = dataset->findAndGetUint16(DCM_PixelRepresentation, u16);
  if (cond.bad() || (u16 != 0))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Pixel Representation does not fit SOP class");

  cond = dataset->putAndInsertOFStringArray(DCM_SOPClassUID, UID_MultiframeSingleBitSecondaryCaptureImageStorage);
  return cond;
}


OFCondition I2DOutputPlugNewSC::handle8BitSC(DcmDataset *dataset) const
{
  if (!dataset)
    return EC_IllegalParameter;

  OFCondition cond; Uint16 u16 = 0; OFString str;
  cond = dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, str);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric Interpretation not set for Pixel Data");
  if (str == "MONOCHROME2") // Mult-Frame Grayscale Byte SC Image
  {
    cond = dataset->findAndGetUint16(DCM_SamplesPerPixel, u16);
    if (cond.bad() || (u16 != 1))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Samples Per Pixel does not fit SOP class");

    cond = dataset->findAndGetUint16(DCM_BitsStored, u16);
    if (cond.bad() || (u16 != 8))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Bits Stored does not fit SOP class");

    cond = dataset->findAndGetUint16(DCM_HighBit, u16);
    if (cond.bad() || (u16 != 7))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: High Bit does not fit SOP class");

    cond = dataset->findAndGetUint16(DCM_PixelRepresentation, u16);
    if (cond.bad() || (u16 != 0))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Pixel Representation does not fit SOP class");

    // For MONOCHROME2 and Bits Stored > 1, rescale slope/intercept/type have to be inserted
    cond = insertMonochromeAttribs(dataset);

    // Insert SOP Class UID
    if (cond.good())
      cond = dataset->putAndInsertOFStringArray(DCM_SOPClassUID, UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage);
  }
  else if ((str == "YBR_FULL_422") || (str == "RGB")) // Mult-Frame True Color Byte SC Image
  {
    cond = dataset->findAndGetUint16(DCM_SamplesPerPixel, u16);
    if (cond.bad() || (u16 != 3))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Samples Per Pixel does not fit SOP class");

    cond = dataset->findAndGetUint16(DCM_BitsStored, u16);
    if (cond.bad() || (u16 != 8))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Bits Stored does not fit SOP class");

    cond = dataset->findAndGetUint16(DCM_HighBit, u16);
    if (cond.bad() || (u16 != 7))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: High Bit does not fit SOP class");

    cond = dataset->findAndGetUint16(DCM_PixelRepresentation, u16);
    if (cond.bad() || (u16 != 0))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Pixel Representation does not fit SOP class");

    if (str == "RGB") // For RGB, Planar Configuration must be present and set to 0
    {
      cond = dataset->findAndGetUint16(DCM_PlanarConfiguration, u16);
      if (cond.bad() || (u16 != 0))
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Planar Configuration does not fit SOP class");
    }
    // Insert SOP Class UID
    if (cond.good())
      cond = dataset->putAndInsertOFStringArray(DCM_SOPClassUID, UID_MultiframeTrueColorSecondaryCaptureImageStorage);
  }
  else
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric Interpretation does not fit SOP class");

  return cond;
}


OFCondition I2DOutputPlugNewSC::handle16BitSC(DcmDataset *dataset) const
{
  if (!dataset)
    return EC_IllegalParameter;

  OFCondition cond; Uint16 u16 = 0; OFString str;
  cond = dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, str);
  if (cond.bad() || (str != "MONOCHROME2"))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric Interpretation does not fit SOP class");

  cond = dataset->findAndGetUint16(DCM_SamplesPerPixel, u16);
  if (cond.bad() || (u16 != 1))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Samples Per Pixel does not fit SOP class");

  Uint16 bitsStored;
  cond = dataset->findAndGetUint16(DCM_BitsStored, bitsStored);
  if (cond.bad() || (bitsStored >= 16) || (bitsStored <9) )
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Bits Stored does not fit SOP class");

  cond = dataset->findAndGetUint16(DCM_HighBit, u16);
  if (cond.bad() || (u16 != bitsStored -1))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: High Bit does not fit SOP class");

  cond = dataset->findAndGetUint16(DCM_PixelRepresentation, u16);
  if (cond.bad() || (u16 != 0))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Pixel Representation does not fit SOP class");

  // Insert SOP Class UID
  cond = dataset->putAndInsertOFStringArray(DCM_SOPClassUID, UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage);

  // For MONOCHROME2 and Bits Stored > 1, rescale slope/intercept/type have to be inserted
  if (cond.good())
    cond = insertMonochromeAttribs(dataset);

  return cond;
}


OFCondition I2DOutputPlugNewSC::insertMonochromeAttribs(DcmDataset *targetDataset) const
{
  OFCondition cond;
  // Rescale Intercept, set to 0 (constant enumerated value)
  cond = targetDataset->putAndInsertOFStringArray(DCM_RescaleIntercept, "0");
  // Rescale Slope, set to 1 (constant enumerated value)
  if (cond.good())
    cond = targetDataset->putAndInsertOFStringArray(DCM_RescaleSlope, "1");
  // Rescale type, set to unspecified "US" (constant enumerated value)
  if (cond.good())
    cond = targetDataset->putAndInsertOFStringArray(DCM_RescaleType, "US");
  // Rescale type, set to unspecified "IDENTITY" (constant enumerated value)
  if (cond.good())
    cond = targetDataset->putAndInsertOFStringArray(DCM_PresentationLUTShape, "IDENTITY");
  else
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Unable to insert attributes for Rescale Slope/Intercept/Type");
  return EC_Normal;
}


OFBool I2DOutputPlugNewSC::supportsMultiframe() const
{
  return OFTrue;
}


OFBool I2DOutputPlugNewSC::colorModelPermitted(const OFString& photometricInterpretation, E_TransferSyntax outputTS) const
{
    if (photometricInterpretation == "MONOCHROME2") return OFTrue;

    // DICOM part 3, section A.8.5.4:
    // Photometric Interpretation (0028,0004) shall be RGB for uncompressed or lossless compressed Transfer Syntaxes that do not have defined color space transformations,
    // YBR_ICT for irreversible JPEG 2000 Transfer Syntaxes,
    // YBR_RCT for reversible JPEG 2000 Transfer Syntaxes,
    // YBR_PARTIAL_420 for MPEG2, MPEG-4 AVC/H.264, HEVC/H.265 Transfer Syntaxes and
    // YBR_FULL_422 for JPEG lossy compressed Transfer Syntaxes and
    // YBR_FULL or RGB for RLE Transfer Syntaxes
    switch(outputTS)
    {
        case EXS_JPEG2000:
        case EXS_JPEG2000Multicomponent:
        case EXS_HighThroughputJPEG2000:
            return (photometricInterpretation == "YBR_ICT");
            /* break; */
        case EXS_JPEG2000LosslessOnly:
        case EXS_JPEG2000MulticomponentLosslessOnly:
        case EXS_HighThroughputJPEG2000LosslessOnly:
        case EXS_HighThroughputJPEG2000withRPCLOptionsLosslessOnly:
            return (photometricInterpretation == "YBR_RCT");
            /* break; */
        case EXS_MPEG2MainProfileAtMainLevel:
        case EXS_FragmentableMPEG2MainProfileMainLevel:
        case EXS_MPEG2MainProfileAtHighLevel:
        case EXS_FragmentableMPEG2MainProfileHighLevel:
        case EXS_MPEG4HighProfileLevel4_1:
        case EXS_FragmentableMPEG4HighProfileLevel4_1:
        case EXS_MPEG4BDcompatibleHighProfileLevel4_1:
        case EXS_FragmentableMPEG4BDcompatibleHighProfileLevel4_1:
        case EXS_MPEG4HighProfileLevel4_2_For2DVideo:
        case EXS_FragmentableMPEG4HighProfileLevel4_2_For2DVideo:
        case EXS_MPEG4HighProfileLevel4_2_For3DVideo:
        case EXS_FragmentableMPEG4HighProfileLevel4_2_For3DVideo:
        case EXS_MPEG4StereoHighProfileLevel4_2:
        case EXS_FragmentableMPEG4StereoHighProfileLevel4_2:
        case EXS_HEVCMainProfileLevel5_1:
        case EXS_HEVCMain10ProfileLevel5_1:
            return (photometricInterpretation == "YBR_PARTIAL_420");
            /* break; */
        case EXS_JPEGProcess1:
        case EXS_JPEGProcess2_4:
        case EXS_JPEGProcess3_5:
        case EXS_JPEGProcess6_8:
        case EXS_JPEGProcess7_9:
        case EXS_JPEGProcess10_12:
        case EXS_JPEGProcess11_13:
        case EXS_JPEGProcess16_18:
        case EXS_JPEGProcess17_19:
        case EXS_JPEGProcess20_22:
        case EXS_JPEGProcess21_23:
        case EXS_JPEGProcess24_26:
        case EXS_JPEGProcess25_27:
            return (photometricInterpretation == "YBR_FULL_422");
            /* break; */
        case EXS_RLELossless:
            return ((photometricInterpretation == "YBR_FULL") || (photometricInterpretation == "RGB"));
            /* break; */
        case EXS_JPEGXLLossless:
        case EXS_JPEGXLJPEGRecompression:
        case EXS_JPEGXL:
            // DICOM part 3 does not (yet) specify any requirements for this IOD and JPEG-XL.
            return OFTrue;
            /* break; */
        default:
            return (photometricInterpretation == "RGB");
            /* break; */
    }
}


OFCondition I2DOutputPlugNewSC::updateColorModel(OFString& photometricInterpretation, E_TransferSyntax outputTS) const
{
    if (photometricInterpretation == "MONOCHROME2") return EC_Normal;
    switch(outputTS)
    {
        case EXS_JPEG2000:
        case EXS_JPEG2000Multicomponent:
        case EXS_HighThroughputJPEG2000:
            if (photometricInterpretation == "YBR_ICT") return EC_Normal; else return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported color model");
            /* break; */
        case EXS_JPEG2000LosslessOnly:
        case EXS_JPEG2000MulticomponentLosslessOnly:
        case EXS_HighThroughputJPEG2000LosslessOnly:
        case EXS_HighThroughputJPEG2000withRPCLOptionsLosslessOnly:
            if (photometricInterpretation == "YBR_RCT") return EC_Normal; else return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported color model");
            /* break; */
        case EXS_MPEG2MainProfileAtMainLevel:
        case EXS_FragmentableMPEG2MainProfileMainLevel:
        case EXS_MPEG2MainProfileAtHighLevel:
        case EXS_FragmentableMPEG2MainProfileHighLevel:
        case EXS_MPEG4HighProfileLevel4_1:
        case EXS_FragmentableMPEG4HighProfileLevel4_1:
        case EXS_MPEG4BDcompatibleHighProfileLevel4_1:
        case EXS_FragmentableMPEG4BDcompatibleHighProfileLevel4_1:
        case EXS_MPEG4HighProfileLevel4_2_For2DVideo:
        case EXS_FragmentableMPEG4HighProfileLevel4_2_For2DVideo:
        case EXS_MPEG4HighProfileLevel4_2_For3DVideo:
        case EXS_FragmentableMPEG4HighProfileLevel4_2_For3DVideo:
        case EXS_MPEG4StereoHighProfileLevel4_2:
        case EXS_FragmentableMPEG4StereoHighProfileLevel4_2:
        case EXS_HEVCMainProfileLevel5_1:
        case EXS_HEVCMain10ProfileLevel5_1:
            if (photometricInterpretation == "YBR_PARTIAL_420") return EC_Normal; else return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported color model");
            /* break; */
        case EXS_JPEGProcess1:
        case EXS_JPEGProcess2_4:
        case EXS_JPEGProcess3_5:
        case EXS_JPEGProcess6_8:
        case EXS_JPEGProcess7_9:
        case EXS_JPEGProcess10_12:
        case EXS_JPEGProcess11_13:
        case EXS_JPEGProcess16_18:
        case EXS_JPEGProcess17_19:
        case EXS_JPEGProcess20_22:
        case EXS_JPEGProcess21_23:
        case EXS_JPEGProcess24_26:
        case EXS_JPEGProcess25_27:
            // silently replace "YBR_FULL" by "YBR_FULL_422". Most JPEG codecs won't mind.
            if (photometricInterpretation == "YBR_FULL")
            {
                photometricInterpretation = "YBR_FULL_422";
                return EC_Normal;
            }
            else if (photometricInterpretation == "YBR_FULL_422") return EC_Normal;
            else return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported color model");
            /* break; */
        case EXS_RLELossless:
            if ((photometricInterpretation == "YBR_FULL") || (photometricInterpretation == "RGB")) return EC_Normal; else return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported color model");
           /* break; */
        case EXS_JPEGXLLossless:
        case EXS_JPEGXLJPEGRecompression:
        case EXS_JPEGXL:
            // DICOM part 3 does not (yet) specify any requirements for this IOD and JPEG-XL.
            return EC_Normal;
            /* break; */
        default:
            if (photometricInterpretation == "RGB") return EC_Normal; else return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported color model");
            /* break; */
    }
}


OFCondition I2DOutputPlugNewSC::insertMultiFrameAttributes(
  DcmDataset* targetDataset,
  size_t numberOfFrames) const
{
  if ((!targetDataset) || (numberOfFrames == 0))
    return EC_IllegalParameter;

  // Note: Instead of FrameTime and FrameTimeVector, Multiframe Secondary Capture
  // also supports (0018,2001) PageNumberVector as an attribute to which the FrameIncrementPointer
  // may point. This might be useful for single-bit images, which are most likely scanned documents.
  // Currently we always generate a FrameTime.

  char numFrames[30];
  OFStandard::snprintf(numFrames, 30, "%lu", numberOfFrames);
  OFCondition cond = targetDataset->putAndInsertOFStringArray(DCM_NumberOfFrames, numFrames);

  // Secondary Capture permits FrameTime and FrameIncrementPointer only when multiple frames are present.
  if (numberOfFrames > 1)
  {
    char frameTime[30];
    size_t fTime = (numberOfFrames > 1) ? DCMTK_I2D_Default_Frame_Time : 0;
    OFStandard::snprintf(frameTime, 30, "%lu", fTime);
    if (cond.good()) cond = targetDataset->putAndInsertOFStringArray(DCM_FrameTime, frameTime);
    if (cond.good()) cond = targetDataset->putAndInsertTagKey(DCM_FrameIncrementPointer, DCM_FrameTime);
  }
  return cond;
}
