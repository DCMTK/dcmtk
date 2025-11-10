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
 *  Purpose: Implements conversion from image into DICOM Visible Light Photography IOD
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2dplvlp.h"
#include "dcmtk/dcmdata/dcdeftag.h"        /* for DCM_ defines */
#include "dcmtk/dcmdata/dcuid.h"           /* for UID_ defines */
#include "dcmtk/dcmdata/dcdatset.h"


I2DOutputPlugVLP::I2DOutputPlugVLP()
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugVLP: Output plugin for VLP initialized");
}


I2DOutputPlugVLP::~I2DOutputPlugVLP()
{
}


OFString I2DOutputPlugVLP::ident()
{
  return "Visible Light Photographic Image SOP Class";
}


void I2DOutputPlugVLP::supportedSOPClassUIDs(OFList<OFString>& suppSOPs)
{
  suppSOPs.push_back(UID_VLPhotographicImageStorage);
}


OFCondition I2DOutputPlugVLP::convert(DcmDataset &dataset) const
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugVLP: Inserting VLP specific attributes");
  OFCondition cond; Uint16 u16 = 0; OFString str;

  cond = dataset.findAndGetUint16(DCM_BitsAllocated, u16);
  if (cond.bad() || (u16 != 8))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Bits Allocated does not fit SOP class");

  cond = dataset.findAndGetUint16(DCM_BitsStored, u16);
  if (cond.bad() || (u16 != 8))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Bits Stored does not fit SOP class");

  cond = dataset.findAndGetUint16(DCM_HighBit, u16);
  if (cond.bad() || (u16 != 7))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: High Bit does not fit SOP class");

  cond = dataset.findAndGetUint16(DCM_PixelRepresentation, u16);
  if (cond.bad() || (u16 != 0))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Pixel Representation does not fit SOP class");

  cond = dataset.findAndGetOFStringArray(DCM_PhotometricInterpretation, str);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Photometric Interpretation not set for Pixel Data");

  if (str == "MONOCHROME2")
  {
    cond = dataset.findAndGetUint16(DCM_SamplesPerPixel, u16);
    if (cond.bad() || (u16 != 1))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Samples Per Pixel does not fit SOP class");
  }
  else if ((str == "YBR_FULL_422") || (str == "RGB"))
  {
    cond = dataset.findAndGetUint16(DCM_SamplesPerPixel, u16);
    if (cond.bad() || (u16 != 3))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Samples Per Pixel does not fit SOP class");

    cond = dataset.findAndGetUint16(DCM_PlanarConfiguration, u16);
    if (cond.bad() || (u16 != 0))
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Planar Configuration does not fit SOP class");
  }
  else
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugVLP: Photometric Interpretation does not fit SOP class");

  cond = dataset.putAndInsertOFStringArray(DCM_SOPClassUID, UID_VLPhotographicImageStorage);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to insert SOP class into dataset");

  cond = dataset.putAndInsertOFStringArray(DCM_Modality, "XC");
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to insert Modality (XC) into dataset");

  return EC_Normal;
}


OFString I2DOutputPlugVLP::isValid(DcmDataset& dataset) const
{
  OFString err;
  // Just return if checking was disabled
  if (!m_doAttribChecking)
    return err;

  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugVLP: Checking VLP specific attributes for validity");
  // Acquisition Context Module
  err = checkAndInventType2Attrib(DCM_AcquisitionContextSequence, &dataset);

  // General Equipment Module
  err += checkAndInventType2Attrib(DCM_Manufacturer, &dataset);

  // VL Image Module
  err += checkAndInventType1Attrib(DCM_ImageType, &dataset, "DERIVED\\SECONDARY");
  err += checkAndInventType2Attrib(DCM_LossyImageCompression, &dataset);

  return err;
}


OFBool I2DOutputPlugVLP::supportsMultiframe() const
{
  return OFFalse;
}


OFBool I2DOutputPlugVLP::colorModelPermitted(const OFString& photometricInterpretation, E_TransferSyntax outputTS) const
{
    if (photometricInterpretation == "MONOCHROME2") return OFTrue;

    // DICOM part 3, C.8.12.1.1.1:
    // Photometric Interpretation (0028,0004) shall be RGB for uncompressed or lossless compressed Transfer Syntaxes that do not have defined color space transformations,
    // YBR_ICT for irreversible JPEG 2000 Transfer Syntaxes,
    // YBR_RCT for reversible JPEG 2000 Transfer Syntaxes,
    // YBR_PARTIAL_420 for MPEG2, MPEG-4 AVC/H.264, HEVC/H.265 Transfer Syntaxes and
    // YBR_FULL_422 for JPEG lossy compressed Transfer Syntaxes.
    // Note that YBR_FULL is explicitly not permitted for RLE in this SOP class.

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


OFCondition I2DOutputPlugVLP::updateColorModel(OFString& photometricInterpretation, E_TransferSyntax outputTS) const
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


OFCondition I2DOutputPlugVLP::insertMultiFrameAttributes(
  DcmDataset* /* targetDataset */,
  size_t /* numberOfFrames */) const
{
  return EC_Normal;
}
