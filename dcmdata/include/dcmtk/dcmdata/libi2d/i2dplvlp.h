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
 *  Purpose: Class for conversion of image file into DICOM SC Image Storage
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:46 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef I2DPLVLP_H
#define I2DPLVLP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"

class I2DOutputPlugVLP : public I2DOutputPlug
{

public:

  /** Constructor, initializes member variables with standard values
   *  @return none
   */
  I2DOutputPlugVLP();

  /** Virtual function that returns a short name of the plugin.
   *  @return The name of the plugin
   */
  virtual OFString ident();

  /** Overwrites function from base class. Returns the Storage SOP class
   *  written by this plugin
   *  @param suppSOPs - [out] List of UIDs representing SOP classes supported by
   *                    this plugin
   *  @return A string holding the Storage SOP class written by this plugin
   */
  virtual void supportedSOPClassUIDs(OFList<OFString> suppSOPs);

  /** Outputs SOP class specific information into dataset
   * @param dataset - [in/out] Dataset to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition convert(DcmDataset &dataset) const;

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return Error string if error occurs, empty string else
   */
  virtual OFString isValid(DcmDataset& dataset) const;

  /** Virtual Destructor, clean up memory
   *  @return none
   */
  virtual ~I2DOutputPlugVLP();

};

#endif // I2DPLVLP_H

/*
 * CVS/RCS Log:
 * $Log: i2dplvlp.h,v $
 * Revision 1.5  2010-10-14 13:15:46  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2009-11-04 09:58:08  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2009-09-30 08:05:25  uli
 * Stop including dctk.h in libi2d's header files.
 *
 * Revision 1.2  2009-01-16 09:51:55  onken
 * Completed doxygen documentation for libi2d.
 *
 * Revision 1.1  2008-01-16 15:13:17  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 * Revision 1.2  2008-01-11 14:17:53  onken
 * Added various options to i2dlib. Changed logging to use a configurable
 * logstream. Added output plugin for the new Multiframe Secondary Capture SOP
 * Classes. Added mode for JPEG plugin to copy exsiting APPn markers (except
 * JFIF). Changed img2dcm default behaviour to invent type1/type2 attributes (no
 * need for templates any more). Added some bug fixes.
 *
 * Revision 1.1  2007/11/08 15:58:56  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
