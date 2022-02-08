/*
 *
 *  Copyright (C) 2001-2022, OFFIS e.V.
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


OFCondition I2DOutputPlugVLP::insertMultiFrameAttributes(
  DcmDataset* /* targetDataset */,
  size_t /* numberOfFrames */) const
{
  return EC_Normal;
}
