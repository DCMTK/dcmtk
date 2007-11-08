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
 *  Purpose: Base class for converter from image file to DICOM
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

#ifndef I2DOUTPL_H
#define I2DOUTPL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

class I2DOutputPlug
{

public:

  /** Constructor, initializes member variables
   *  @param none
   *  @return none
   */
  I2DOutputPlug() {m_verbose = OFFalse; m_debug = OFFalse; };

  /** Virtual function that returns the Storage SOP class UID, the plugin writes.
   *  @param none
   *  @return String containing the Storage SOP class UID
   */
  virtual OFString targetSOPClassUID() const =0;

  /** Outputs SOP class specific information into dataset
   * @param dset - [in/out] Dataset to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition convert(DcmDataset &dataset) const = 0;

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition isValid(DcmDataset& dataset) const = 0;

  /** Destructor
   *  @param none
   *  @return none
   */
  virtual ~I2DOutputPlug() {};

  /** Sets the verbose mode
   *  @param verboseMode - [in] New status for verbose mode
   *  @return none
   */
  void setVerboseMode(const OFBool& verboseMode) { m_verbose = verboseMode; };

  /** Sets the debug mode
   *  @param debugMode - [in] New status for debug mode
   *  @return none
   */
  void setDebugMode(const OFBool& debugMode) { m_debug = debugMode; };

protected:

  /// verbose mode status
  OFBool m_verbose;

  /// debug mode status
  OFBool m_debug;

};

#endif // #ifndef I2DOUTPL_H

/*
 * CVS/RCS Log:
 * $Log: i2doutpl.h,v $
 * Revision 1.1  2007-11-08 15:58:55  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */

