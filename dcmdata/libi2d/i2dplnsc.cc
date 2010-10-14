/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:18:23 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2dplnsc.h"
#include "dcmtk/dcmdata/dcdeftag.h"        /* for DCM_ defines */
#include "dcmtk/dcmdata/dcuid.h"           /* for UID_ defines */
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"


I2DOutputPlugNewSC::I2DOutputPlugNewSC()
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugNewSC: Output plugin for new Multiframe Secondary Capture SOPs initialized");
}


OFString I2DOutputPlugNewSC::ident()
{
  return "New Secondary Capture Image SOP Classes";
}


void I2DOutputPlugNewSC::supportedSOPClassUIDs(OFList<OFString> suppSOPs)
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

  cond = insertMultiFrameAttribs(&dataset);
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


OFCondition I2DOutputPlugNewSC::insertMultiFrameAttribs(DcmDataset* targetDataset) const
{
  if (!targetDataset)
    return EC_IllegalParameter;

  // We only support 1 image Multi-frames so far
  return targetDataset->putAndInsertOFStringArray(DCM_NumberOfFrames, "1");
  // Frame Increment pointer is set later in the more specific SC Multi-frame Image context
}


OFCondition I2DOutputPlugNewSC::handle1BitSC(DcmDataset *dataset) const
{
  if (!dataset)
    return EC_IllegalParameter;

  OFCondition cond; Uint16 u16 = 0; OFString str;
  cond = dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, str);
  if (cond.bad() || (str != "MONOCHROME2"))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric interpretation does not fit SOP class");

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
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric interpretation not set for Pixel Data");
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
    cond = insertSCMultiFrameAttribs(dataset);
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
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric interpretation does not fit SOP class");

  return cond;
}


OFCondition I2DOutputPlugNewSC::handle16BitSC(DcmDataset *dataset) const
{
  if (!dataset)
    return EC_IllegalParameter;

  OFCondition cond; Uint16 u16 = 0; OFString str;
  cond = dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, str);
  if (cond.bad() || (str != "MONOCHROME2"))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DOutputPlugNewSC: Photometric interpretation does not fit SOP class");

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
    cond = insertSCMultiFrameAttribs(dataset);
  return cond;
}


OFCondition I2DOutputPlugNewSC::insertSCMultiFrameAttribs(DcmDataset *targetDataset) const
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


I2DOutputPlugNewSC::~I2DOutputPlugNewSC()
{
}


/*
 * CVS/RCS Log:
 * $Log: i2dplnsc.cc,v $
 * Revision 1.5  2010-10-14 13:18:23  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2009-11-04 09:58:08  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2009-09-30 08:05:26  uli
 * Stop including dctk.h in libi2d's header files.
 *
 * Revision 1.2  2008-01-16 16:32:31  onken
 * Fixed some empty or doubled log messages in libi2d files.
 *
 * Revision 1.1  2008-01-16 14:40:02  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 */
