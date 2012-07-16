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
 *  Purpose: Implements conversion from image into DICOM Visible Light Photography IOD
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2dplvlp.h"
#include "dcmtk/dcmdata/dcdeftag.h"        /* for DCM_ defines */
#include "dcmtk/dcmdata/dcuid.h"           /* for UID_ defines */


I2DOutputPlugVLP::I2DOutputPlugVLP()
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugVLP: Output plugin for VLP initialized");
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
  OFCondition cond;
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


I2DOutputPlugVLP::~I2DOutputPlugVLP()
{
}
