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
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2008-01-11 14:19:28 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/i2dlib/i2dplsc.h"


I2DOutputPlugSC::I2DOutputPlugSC()
{
  if (m_debug)
    printMessage(m_logStream, "I2DOutputPlugSC: Output plugin for Secondary Capture initialized");
}

OFString I2DOutputPlugSC::ident()
{
  return "Secondary Capture Image SOP Class";
}

void I2DOutputPlugSC::supportedSOPClassUIDs(OFList<OFString> suppSOPs)
{
  suppSOPs.push_back(UID_SecondaryCaptureImageStorage);
}


OFCondition I2DOutputPlugSC::convert(DcmDataset &dataset) const
{
  if (m_debug)
    printMessage(m_logStream, "I2DOutputPlugSC: Inserting SC specific attributes");
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

  if (m_debug)
    printMessage(m_logStream, "I2DOutputPlugSC: Checking SC specific attributes");
  err += checkAndInventType1Attrib(DCM_ConversionType, &dataset, "WSD"); // WSD="Workstation"

  return err;
}


I2DOutputPlugSC::~I2DOutputPlugSC()
{
}


/*
 * CVS/RCS Log:
 * $Log: i2dplsc.cc,v $
 * Revision 1.3  2008-01-11 14:19:28  onken
 * *** empty log message ***
 *
 * Revision 1.2  2007/11/08 18:08:58  onken
 * *** empty log message ***
 *
 * Revision 1.1  2007/11/08 15:55:17  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
