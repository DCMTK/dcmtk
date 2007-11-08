/*
 *
 *  Copyright (C) 2001-2007, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Implements conversion from image into DICOM Visible Light Photography IOD
 *
 *  Last Update:      $$
 *  Update Date:      $$
 *  Source File:      $$
 *  CVS/RCS Revision: $$
 *  Status:           $$
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/i2dlib/i2dplvlp.h"


I2DOutputPlugVLP::I2DOutputPlugVLP()
{
  if (m_debug)
    COUT << "I2DOutputPlugVLP: Output plugin for VLP initialized" << OFendl;
}


OFString I2DOutputPlugVLP::targetSOPClassUID() const
{
  return UID_VLPhotographicImageStorage;
}


OFCondition I2DOutputPlugVLP::convert(DcmDataset &dataset) const
{
  if (m_debug)
    COUT << "I2DOutputPlugVLP: Inserting VLP specific attributes" << OFendl;
  OFCondition cond;
  cond = dataset.putAndInsertOFStringArray(DCM_SOPClassUID, targetSOPClassUID());
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to insert SOP class into dataset");

  cond = dataset.putAndInsertOFStringArray(DCM_Modality, "XC");
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to insert Modality (XC) into dataset");

  return EC_Normal;
}


OFCondition I2DOutputPlugVLP::isValid(DcmDataset& dset) const
{
  if (m_debug)
    COUT << "I2DOutputPlugVLP: Checking VLP specific attributes for validity" << OFendl;
  // Acquisition Context Module
  OFString err; OFString dummy;
  if (!dset.tagExists(DCM_AcquisitionContextSequence))
    err += "Error: Acquisition Context Sequence (type 2) not present in dataset\n";

  // General Equipment Module
  if (!dset.tagExists(DCM_Manufacturer))
    err += "Error: Manufacturer (type 2) not present in dataset\n";

  // VL Image Module
  OFCondition cond = dset.findAndGetOFString(DCM_ImageType, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Image Type (type 1) not present or having empty value in dataset\n";
    dummy.clear();
  }

  if (!dset.tagExists(DCM_LossyImageCompression))
    err += "Error: Lossy Image Compression (type 2) not present in dataset\n";

  if (err.length() > 0)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, err.c_str());
  else
    return EC_Normal;
}


I2DOutputPlugVLP::~I2DOutputPlugVLP()
{
}


/*
 * CVS/RCS Log:
 * $Log: i2dplvlp.cc,v $
 * Revision 1.1  2007-11-08 15:55:17  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
