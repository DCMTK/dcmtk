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
 *  Purpose: Base class for converter from image file to DICOM
 *
 */

#ifndef I2DOUTPL_H
#define I2DOUTPL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/libi2d/i2define.h"

extern DCMTK_I2D_EXPORT OFLogger DCM_dcmdataLibi2dLogger;

#define DCMDATA_LIBI2D_TRACE(msg) OFLOG_TRACE(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_DEBUG(msg) OFLOG_DEBUG(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_INFO(msg)  OFLOG_INFO(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_WARN(msg)  OFLOG_WARN(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_ERROR(msg) OFLOG_ERROR(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_FATAL(msg) OFLOG_FATAL(DCM_dcmdataLibi2dLogger, msg)

/// default value for the FrameTime attribute when creating multiframe images
#define DCMTK_I2D_Default_Frame_Time 1000

class DcmDataset;
class DcmTagKey;

class DCMTK_I2D_EXPORT I2DOutputPlug
{

public:

  /** Constructor, initializes member variables
   */
  I2DOutputPlug();

  /** Destructor
   */
  virtual ~I2DOutputPlug();

  /** Virtual function that returns a short name of the plugin.
   *  @return The name of the plugin
   */
  virtual OFString ident() = 0;

  /** Virtual function that returns the Storage SOP class UID, the plugin writes.
   *  @param suppSOPs - [out] List containing supported output SOP classes
   *  @return String containing the Storage SOP class UID
   */
  virtual void supportedSOPClassUIDs(OFList<OFString>& suppSOPs) = 0;

  /** Outputs SOP class specific information into dataset
   * @param dataset - [in/out] Dataset to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition convert(DcmDataset &dataset) const = 0;

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return Error string if error occurs, empty string otherwise
   */
  virtual OFString isValid(DcmDataset& dataset) const = 0;

  /** check if the output format supported by this plugin can write
   *  multi-frame images.
   *  @return true if multiframe is supported, false otherwise
   */
  virtual OFBool supportsMultiframe() const = 0;

  /** Add multiframe specific attributes
   *  @param datset pointer to DICOM dataset, must not be NULL
   *  @param numberOfFrames number of frames in this dataset
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition insertMultiFrameAttributes(
    DcmDataset* targetDataset,
    size_t numberOfFrames) const = 0;

  /** Enable/Disable basic validity checks for output dataset
   *  @param doChecks - [in] OFTrue enables checking, OFFalse turns it off.
   *  @param insertMissingType2 - [in] If true (default), missing type 2
   *         attributes are inserted automatically
   *  @param inventMissingType1 - [in] If true (default), missing type 1
   *         attributes are inserted automatically with a predefined
   *         value (if possible). An existing empty type 1 attribute is
   *         assigned a value, too.
   *  @return none
   */
  virtual void setValidityChecking(OFBool doChecks,
                                   OFBool insertMissingType2 = OFTrue,
                                   OFBool inventMissingType1 = OFTrue);

protected:

  /** Checks whether a given tag exists in a dataset and is non-empty,
   *  otherwise returns an error string.
   *  @param key - [in] The tag to be checked/inserted
   *  @param targetDset - [in/out] The dataset to search (and insert) in
   *  @return Error string, which is empty if no error occurs.
   */
  virtual OFString checkType1Attrib(const DcmTagKey& key,
                                    DcmDataset* targetDset) const;

  /** Checks whether a given tag exists in a dataset and provides a non-empty
   *  value. If not, the tag is inserted (if enabled) and a default value is
   *  inserted.
   *  @param key - [in] The tag to be checked/inserted
   *  @param targetDset - [in/out] The dataset to search (and insert) in
   *  @param defaultValue - [in] The default value to set
   *  @return Error string, which is empty if no error occurs.
   */
  virtual OFString checkAndInventType1Attrib(const DcmTagKey& key,
                                             DcmDataset* targetDset,
                                             const OFString& defaultValue ="") const;

  /** Checks whether a given code sequence exists in a dataset and contains
   *  an item with the required attributes. If not, the sequence is inserted
   *  (if enabled) and default values are inserted.
   *  @param key - [in] The code sequence tag to be checked/inserted
   *  @param targetDset - [in/out] The dataset to search (and insert) in
   *  @param defaultValue - [in] The default value to set
   *  @return Error string, which is empty if no error occurs.
   */
  virtual OFString checkAndInventType1CodeSQ(
    const DcmTagKey& key,
    DcmDataset* targetDset,
    const OFString& codeValue,
    const OFString& codeMeaning,
    const OFString& codingSchemeDesignator) const;

  /** Checks whether a given tag exists in a dataset (can be empty)
   *  If not, the tag is inserted (if enabled) with empty value.
   *  @param key - [in] The tag to be checked/inserted
   *  @param targetDset - [in/out] The dataset to search (and insert) in
   *  @return Error string, which is empty if no error occurs.
   */
  virtual OFString checkAndInventType2Attrib(const DcmTagKey& key,
                                             DcmDataset* targetDset,
                                             const OFString& defaultValue ="") const;

  /// if enabled, some simple attribute checking is performed
  /// default: enabled (OFTrue)
  OFBool m_doAttribChecking;

  /// if enabled, missing type 2 attributes in the dataset are added automatically.
  /// default: enabled (OFTrue)
  OFBool m_inventMissingType2Attribs;

  /// if enbled, missing type 1 attributes are inserted and filled with a
  /// predefined value. Default: disabled (OFFalse)
  OFBool m_inventMissingType1Attribs;

};

#endif // #ifndef I2DOUTPL_H
