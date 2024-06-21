/*
 *
 *  Copyright (C) 2001-2022, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Class for conversion of image file into Ophthalmic Photography 8 Bit Image IOD
 *
 */

#ifndef I2DPLOP8_H
#define I2DPLOP8_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"

/** img2dcm output plugin for Ophthalmic Photography Storage SOP Class */
class DCMTK_I2D_EXPORT I2DOutputPlugOphthalmicPhotography : public I2DOutputPlug
{

public:

  /** Constructor, initializes member variables with standard values
   */
  I2DOutputPlugOphthalmicPhotography();

  /** Virtual Destructor, clean up memory
   */
  virtual ~I2DOutputPlugOphthalmicPhotography();

  /** Virtual function that returns a short name of the plugin.
   *  @return The name of the plugin
   */
  virtual OFString ident();

  /** Overwrites function from base class. Returns the Storage SOP class
   *  written by this plugin
   *  @param suppSOPs - [out] List of UIDs representing the supported SOP
   *                    classes supported by this plugin.
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

  /** check if the output format supported by this plugin can write
   *  multi-frame images.
   *  @return true if multiframe is supported, false otherwise
   */
  virtual OFBool supportsMultiframe() const;

  /** Add multiframe specific attributes
   *  @param targetDataset pointer to DICOM dataset, must not be NULL
   *  @param numberOfFrames number of frames in this dataset
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition insertMultiFrameAttributes(
    DcmDataset* targetDataset,
    size_t numberOfFrames) const;

private:

  /** Handle 8-bit image dataset. Used internally in convert() method.
   *  @param dataset pointer to DICOM dataset, must not be NULL
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition handle8BitImage(DcmDataset *dataset) const;

  /** Handle 16-bit image dataset. Used internally in convert() method.
   *  @param dataset pointer to DICOM dataset, must not be NULL
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition handle16BitImage(DcmDataset *dataset) const;

};

#endif // I2DPLOP8_H
