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
 *  Purpose: Base class for converter from image file to DICOM
 *
 */

#ifndef I2DOUTPL_H
#define I2DOUTPL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmdata/libi2d/i2define.h"


extern DCMTK_I2D_EXPORT OFLogger DCM_dcmdataLibi2dLogger;

#define DCMDATA_LIBI2D_TRACE(msg) OFLOG_TRACE(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_DEBUG(msg) OFLOG_DEBUG(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_INFO(msg)  OFLOG_INFO(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_WARN(msg)  OFLOG_WARN(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_ERROR(msg) OFLOG_ERROR(DCM_dcmdataLibi2dLogger, msg)
#define DCMDATA_LIBI2D_FATAL(msg) OFLOG_FATAL(DCM_dcmdataLibi2dLogger, msg)


class DCMTK_I2D_EXPORT I2DOutputPlug
{

public:

  /** Constructor, initializes member variables
   *  @return none
   */
  I2DOutputPlug() : m_doAttribChecking(OFTrue), m_inventMissingType2Attribs(OFTrue),
                    m_inventMissingType1Attribs(OFTrue)
  {};

  /** Virtual function that returns a short name of the plugin.
   *  @return The name of the plugin
   */
  virtual OFString ident() =0;

  /** Virtual function that returns the Storage SOP class UID, the plugin writes.
   *  @param suppSOPs - [out] List containing supported output SOP classes
   *  @return String containing the Storage SOP class UID
   */
  virtual void supportedSOPClassUIDs(OFList<OFString>& suppSOPs) =0;

  /** Outputs SOP class specific information into dataset
   * @param dataset - [in/out] Dataset to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition convert(DcmDataset &dataset) const =0;

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return Error string if error occurs, empty string otherwise
   */
  virtual OFString isValid(DcmDataset& dataset) const = 0;

  /** Destructor
   *  @return none
   */
  virtual ~I2DOutputPlug() {};

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
                                   OFBool inventMissingType1 = OFTrue)
  {
    m_doAttribChecking = doChecks;
    m_inventMissingType2Attribs = insertMissingType2;
    m_inventMissingType1Attribs = inventMissingType1;
  };

protected:

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
                                             const OFString& defaultValue ="") const
  {
    OFBool exists = targetDset->tagExists(key);
    if (!exists && !m_inventMissingType1Attribs)
    {
      OFString err = "I2DOutputPlug: Missing type 1 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
      return err;
    }
    DcmElement *elem;
    OFCondition cond = targetDset->findAndGetElement(key, elem);
    if (cond.bad() || !elem || (elem->getLength() == 0))
    {
      if (!m_inventMissingType1Attribs)
      {
        OFString err;
        err += "I2DOutputPlug: Empty value for type 1 attribute: ";
        err += DcmTag(key).getTagName();
        err += "\n";
        return err;
      }
      //holds element to insert in item
      elem = NULL;
      DcmTag tag(key); OFBool wasError = OFFalse;
      //if dicom element could be created, insert in to item and modify to value
      if ( newDicomElement(elem, tag).good())
      {
          if (targetDset->insert(elem, OFTrue).good())
          {
            if (elem->putString(defaultValue.c_str()).good())
            {
              DCMDATA_LIBI2D_DEBUG("I2DOutputPlug: Inserting missing type 1 attribute: " << tag.getTagName() << " with value " << defaultValue);
            } else wasError = OFTrue;
          } else wasError = OFTrue;
      } else wasError = OFTrue;
      if (wasError)
      {
        OFString err = "Unable to insert type 1 attribute ";
        err += tag.getTagName(); err += " with value "; err += defaultValue; err += "\n";
        return err;
      }
    }
    return "";
  };


  /** Checks whether a given tag exists in a dataset (can be empty)
   *  If not, the tag is inserted (if enabled) with empty value.
   *  @param key - [in] The tag to be checked/inserted
   *  @param targetDset - [in/out] The dataset to search (and insert) in
   *  @return Error string, which is empty if no error occurs.
   */
  virtual OFString checkAndInventType2Attrib(const DcmTagKey& key,
                                             DcmDataset* targetDset,
                                             const OFString& defaultValue ="") const
  {
    OFString err;
    OFBool exists = targetDset->tagExists(key);
    if (!exists)
    {
      if (m_inventMissingType2Attribs)
      {
        //holds element to insert in item
        DcmElement *elem = NULL;
        DcmTag tag(key); OFBool wasError = OFFalse;
        //if dicom element could be created, insert in to item and modify to value
        if ( newDicomElement(elem, tag).good())
        {
          if (targetDset->insert(elem, OFTrue).good())
          {
            OFCondition result;
            if (!defaultValue.empty()) // only insert value if not empty(e. g. empty type 2 sequences)
            {
              result = elem->putString(defaultValue.c_str());
            }
            if (result.good())
            {
              DCMDATA_LIBI2D_DEBUG("I2DOutputPlug: Inserting missing type 2 attribute: " << tag.getTagName() << " with value " << (defaultValue.empty() ? "<empty>" : defaultValue));
            } else wasError = OFTrue;
          } else wasError = OFTrue;
        } else wasError = OFTrue;
        if (wasError)
        {
          err += "Unable to insert type 2 attribute "; err += tag.getTagName(); err += " with value "; err += defaultValue; err += "\n";
        }        
      }
      else
      {
        err = "Image2Dcm: Missing type 2 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
        return err;
      }
    }
    return err;
  };

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
