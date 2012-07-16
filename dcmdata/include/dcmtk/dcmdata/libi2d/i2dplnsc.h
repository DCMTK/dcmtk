/*
 *
 *  Copyright (C) 2001-2011, OFFIS e.V.
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
 *  Purpose: Class for conversion of image file into new DICOM SC Image IODs
 *
 */

#ifndef I2DPLNSC_H
#define I2DPLNSC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"

class DCMTK_I2D_EXPORT I2DOutputPlugNewSC : public I2DOutputPlug
{

public:

  /** Constructor, initializes member variables with standard values
   *  @return none
   */
  I2DOutputPlugNewSC();

  /** Virtual function that returns a short name of the plugin.
   *  @return The name of the plugin
   */
  virtual OFString ident();

  /** Overwrites function from base class. Returns the Storage SOP class
   *  written by this plugin
   *  @param suppSOPs - [out] List of UIDs representing the supported SOP 
   *                    classes supported by this plugin.
   *  @return none
   */
  virtual void supportedSOPClassUIDs(OFList<OFString>& suppSOPs);

  /** Outputs SOP class specific information into dataset
   * @param dataset - [in/out] Dataset to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition convert(DcmDataset &dataset) const;

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return Error string if error occurs, empty string otherwise
   */
  virtual OFString isValid(DcmDataset& dataset) const;

  /** Virtual Destructor, clean up memory
   *  @return none
   */
  virtual ~I2DOutputPlugNewSC();

protected:

  /** Inserts attributes for Multi-frame Module
   *  @param targetDataset - [in/out] The dataset to write to
   *  @return EC_Normal if insertion was successfull, error code otherwise
   */
  virtual OFCondition insertMultiFrameAttribs(DcmDataset* targetDataset) const;

  /** Inserts attributes Rescale Slope/Intercept/Type, which have to be
   *  written (1C) if color model is MONOCHROME2 and BitsStored > 1.
   *  @param targetDataset - [out] The dataset to write to
   *  @return EC_Normal if insertion was successfull, error code otherwise
   */
  virtual OFCondition insertSCMultiFrameAttribs(DcmDataset *targetDataset) const;

  /** Checks whether Image Pixel module attributes conform to the
   *  specification of a a 1 bit Secondary Capture object.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal, if everything is fine, error code otherwise
   */
  virtual OFCondition handle1BitSC(DcmDataset *dataset) const;

  /** Checks whether Image Pixel module attributes conform to the
   *  specification of a a 8 bit Secondary Capture object.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal, if everything is fine, error code otherwise
   */
  virtual OFCondition handle8BitSC(DcmDataset *dataset) const;

  /** Checks whether Image Pixel module attributes conform to the
   *  specification of a a 16 bit Secondary Capture object.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal, if everything is fine, error code otherwise
   */
  virtual OFCondition handle16BitSC(DcmDataset *dataset) const;


};

#endif // I2DPLNSC_H
