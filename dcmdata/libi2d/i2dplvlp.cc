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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:18:24 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

void I2DOutputPlugVLP::supportedSOPClassUIDs(OFList<OFString> suppSOPs)
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


/*
 * CVS/RCS Log:
 * $Log: i2dplvlp.cc,v $
 * Revision 1.5  2010-10-14 13:18:24  joergr
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
 * Revision 1.1  2008-01-16 14:42:00  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 * Revision 1.1  2007/11/08 15:55:17  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
