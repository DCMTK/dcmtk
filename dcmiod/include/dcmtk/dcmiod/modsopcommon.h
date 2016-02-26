/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the SOP Common Module
 *
 */

#ifndef MODSOPCOMMON_H
#define MODSOPCOMMON_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmiod/modbase.h"


/** Class representing the SOP Common Module. Supported Attributes:
  *
  * SOP Class UID: (UI, 1, 1)
  * SOP Instance UID: (UI, 1, 1)
  * Specific Character Set: (CS, 1-n, 1C)
  * Instance Creation Date: (DA, 1, 3)
  * Instance Creation Time: (TM, 1, 3)
  * Instance Creator UID: (UI, 1, 3)
  * Timezone Offset from UTC: (SH, 1, 3)
  *
  */
class DCMTK_DCMIOD_EXPORT IODSOPCommonModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODSOPCommonModule(OFshared_ptr<DcmItem> item,
                     OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODSOPCommonModule();

  /** Destructor
   */
  virtual ~IODSOPCommonModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Set missing values by inventing "default values". Automatically
   *  called during write() by IODComponent.
   */
  virtual void inventMissing();

  /** Get name of module
   *  @return Name of the module ("SOPCommonModule")
   */
  virtual OFString getName() const;

  /** Make sure that the module contains a SOP Instance UID, i.e.\ a new one
   *  is created if empty. An invalid UID is corrected if desired.
   *  @param  correctInvalid Correct invalid UID if OFTrue, otherwise do nothing
   */
  virtual void ensureInstanceUID(const OFBool correctInvalid = OFFalse);

   /** Get SOP Class UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSOPClassUID(OFString &value,
                                      const signed long pos = 0) const;

   /** Get SOP Instance UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSOPInstanceUID(OFString &value,
                                        const signed long pos = 0) const;

   /** Get Instance Creator UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getInstanceCreatorUID(OFString &value,
                                            const signed long pos = 0) const;

   /** Get Timeho e Offset from UTC
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getTimezoneOffsetFromUTC(OFString &value,
                                               const signed long pos = 0) const;

   /** Get Specific Character Set
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSpecificCharacterSet(OFString &value,
                                              const signed long pos = 0) const;

   /** Get Instance Creation Date
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getInstanceCreationDate(OFString &value,
                                              const signed long pos = 0) const;

   /** Get Instance Creation Time
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getInstanceCreationTime(OFString &value,
                                              const signed long pos = 0) const;

  /** Set SOP Instance UID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSOPInstanceUID(const OFString &value,
                                        const OFBool checkValue = OFTrue);

  /** Set SOP Class UID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (UI) and
   *           VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSOPClassUID(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /**  Set Specific Character Set. The internal enumerated value is set accordingly.
   *   Please note that code extensions techniques are not supported. Therefore, only
   *   a single value can be passed.
   *   @param  value Value to be set (single value only) or "" for no value
   *   @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *   @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSpecificCharacterSet(const OFString &value,
                                              const OFBool checkValue = OFTrue);

  /** Set Instance Creation Date
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setInstanceCreationDate(const OFString &value,
                                              const OFBool checkValue = OFTrue);
  /** Set Instance Creation Date
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setInstanceCreationTime(const OFString &value,
                                              const OFBool checkValue = OFTrue);

  /**  Set Instance Creator UID
   *   @param  value Value to be set (single value only) or "" for no value
   *   @param  checkValue Check 'value' for conformance with VR (UID) and VM (1)
   *           if enabled
   *   @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setInstanceCreatorUID(const OFString &value,
                                            const OFBool checkValue = OFTrue);

  /**  Set Timezone Offset From UTC
   *   @param  value Value to be set (single value only) or "" for no value.
   *   Encoded as an ASCII string in the format “&ZZXX”. The components of this
   *   string, from left to right, are & = “+” or “-”, and ZZ = Hours and
   *   XX = Minutes of offset.
   *   @param  checkValue Check 'value' for conformance with VR (SH) and VM (1) if enabled
   *   @return EC_Normal if successful, an error code otherwise
   */  virtual OFCondition setTimeZoneOffsetFromUTC(const OFString &value,
                                                    const OFBool checkValue = OFTrue) const;

private:

  /// Name of this module ("SOPCommonModule")
  static const OFString m_ModuleName;

};

#endif // MODSOPCOMMON_H
