/*
 *
 *  Copyright (C) 2001-2021, OFFIS e.V.
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
 *  Purpose: Implements conversion from image into DICOM SC IOD
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2dplsc.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"
#include "dcmtk/dcmdata/dcdeftag.h"        /* for DCM_ defines */
#include "dcmtk/dcmdata/dcuid.h"           /* for UID_ defines */
#include "dcmtk/dcmdata/dcdatset.h"


I2DOutputPlugSC::I2DOutputPlugSC()
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugSC: Output plugin for Secondary Capture initialized");
}


I2DOutputPlugSC::~I2DOutputPlugSC()
{
}


OFString I2DOutputPlugSC::ident()
{
  return "Secondary Capture Image SOP Class";
}


void I2DOutputPlugSC::supportedSOPClassUIDs(OFList<OFString>& suppSOPs)
{
  suppSOPs.push_back(UID_SecondaryCaptureImageStorage);
}


OFCondition I2DOutputPlugSC::convert(DcmDataset &dataset) const
{
  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugSC: Inserting SC specific attributes");
  OFCondition cond;
  cond = dataset.putAndInsertOFStringArray(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);

  return EC_Normal;
}


OFString I2DOutputPlugSC::isValid(DcmDataset& dataset) const
{
  OFString err;
  // Just return if checking was disabled
  if (!m_doAttribChecking)
    return err;

  DCMDATA_LIBI2D_DEBUG("I2DOutputPlugSC: Checking SC specific attributes");
  err += checkAndInventType1Attrib(DCM_ConversionType, &dataset, "WSD"); // WSD="Workstation"

  return err;
}


OFBool I2DOutputPlugSC::supportsMultiframe() const
{
  return OFFalse;
}


OFCondition I2DOutputPlugSC::insertMultiFrameAttributes(
  DcmDataset* /* targetDataset */,
  size_t /* numberOfFrames */) const
{
  return EC_Normal;
}
