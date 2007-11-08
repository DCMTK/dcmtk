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
 *  Purpose: Implements conversion from image into DICOM SC IOD
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
#include "dcmtk/dcmdata/i2dlib/i2dplsc.h"


I2DOutputPlugSC::I2DOutputPlugSC()
{
  if (m_debug)
    COUT << "I2DOutputPlugSC: Output plugin for Secondary Capture (deprecated) initialized" << OFendl;
}


OFString I2DOutputPlugSC::targetSOPClassUID() const
{
  return UID_SecondaryCaptureImageStorage;
}

OFCondition I2DOutputPlugSC::convert(DcmDataset &dataset) const
{
  if (m_debug)
    COUT << "I2DOutputPlugSC: Inserting SC specific attributes" << OFendl;
  OFCondition cond;
  cond = dataset.putAndInsertOFStringArray(DCM_SOPClassUID, targetSOPClassUID());

  return EC_Normal;
}

OFCondition I2DOutputPlugSC::isValid(DcmDataset& dset) const
{
  if (m_debug)
    COUT << "I2DOutputPlugSC: Checking SC specific attributes" << OFendl;
  OFString dummy; OFString err;
  OFCondition cond = dset.findAndGetOFString(DCM_ConversionType, dummy);
  if (cond.bad() || (dummy.length() == 0))
    err += "Error: Conversion Type (type 1) not present or having empty value in dataset\n";

  if (err.length() != 0)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, err.c_str());

  return EC_Normal;
}


I2DOutputPlugSC::~I2DOutputPlugSC()
{
}


/*
 * CVS/RCS Log:
 * $Log: i2dplsc.cc,v $
 * Revision 1.1  2007-11-08 15:55:17  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
