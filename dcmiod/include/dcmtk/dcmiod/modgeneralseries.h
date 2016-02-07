/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Purpose: Class for managing the General Series Module
 *
 */

#ifndef MODGENERALSERIES_H
#define MODGENERALSERIES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/dcmiod/iodmacro.h"

/** Class representing the General Series Module:
 *
 *  Modality: (CS, 1, 1)
 *  Series Instance Number: (UI, 1, 1)
 *  Series Number: (IS, 1, 2)
 *  Laterality: (CS, 1, 2C)
 *  Series Date: (DA, 1, 3)
 *  Series Time: (TM, 1, 3)
 *  Performing Physician's Name: (PN, 1, 3)
 *  Protocol Name: (LO, 1, 3)
 *  Series Description: (LO, 1, 3)
 *  Operators' Name: (PN, 1-n, 3)
 *  Body Part Examined: (CS, 1, 3)
 *  Patient Position: (CS, 1, 2C)
 */
class DCMTK_DCMIOD_EXPORT IODGeneralSeriesModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODGeneralSeriesModule(OFshared_ptr<DcmItem> item,
                         OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODGeneralSeriesModule();

  /** Destructor
   */
  virtual ~IODGeneralSeriesModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Set missing values by inventing "default values". Automatically
   *  called during write() by IODComponent.
   */
  virtual void inventMissing();

  /** Get name of module
   *  @return Name of the module ("GeneralSeriesModule")
   */
  virtual OFString getName() const;

  /** Make sure that the module contains a Series Instance UID, i.e.\ a new one
   *  is created if empty. An invalid UID is corrected if desired.
   *  @param  correctInvalid Correct invalid UID if OFTrue, otherwise do nothing
   */
  virtual void ensureInstanceUID(const OFBool correctInvalid = OFFalse);

  /** Read attributes from given item into this class
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
   *         old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

   /** Get Modality
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getModality(OFString &value,
                                  const signed long pos = 0) const;

  /** Get series instance UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSeriesInstanceUID(OFString &value,
                                            const signed long pos = 0) const;

   /** Get Series Number
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSeriesNumber(OFString &value,
                                      const signed long pos = 0) const;

   /** Get Laterality
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getLaterality(OFString &value,
                                      const signed long pos = 0) const;

   /** Get Series Date
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSeriesDate(OFString &value,
                                    const signed long pos = 0) const;

   /** Get Series Time
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSeriesTime(OFString &value,
                                    const signed long pos = 0) const;

   /** Get Performing Physician's Name
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getPerformingPhysicianName(OFString &value,
                                                 const signed long pos = 0) const;

   /** Get Protocol Name
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getProtocolName(OFString &value,
                                      const signed long pos = 0) const;

   /** Get Series Description
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos  Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSeriesDescription(OFString &value,
                                           const signed long pos = 0) const;

   /** Get Operators' Name
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getOperatorsName(OFString &value,
                                       const signed long pos = 0) const;

   /** Get Body Part Examined
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getBodyPartExamined(OFString &value,
                                          const signed long pos = 0) const;

   /** Get Patient Position
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getPatientPosition(OFString &value,
                                         const signed long pos = 0) const;

  /** Get reference to Referenced Performed Procedure Step
   *  @return Reference to PPS
   */
  virtual SOPInstanceReferenceMacro& getReferencedPPS();

  /** Set Modality
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setModality(const OFString &value,
                                  const OFBool checkValue = OFTrue);

  /** Set Series Instance UID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSeriesInstanceUID(const OFString &value,
                                           const OFBool checkValue = OFTrue);

  /** Set Series Number
   *  @param  value Value to be set (single value only).  If an empty string is,
   *          passed, the value "1" is set when displaying or writing the document
   *          since the corresponding DICOM attribute is mandatory.
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSeriesNumber(const OFString &value,
                                      const OFBool checkValue = OFTrue);

  /** Set Laterality
   *  @param  value Value to be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLaterality(const DcmIODTypes::IOD_LATERALITY value,
                                    const OFBool checkValue = OFTrue);

  /** Set Series Date
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSeriesDate(const OFString &value,
                                    const OFBool checkValue = OFTrue);

  /** Set Series Time
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSeriesTime(const OFString &value,
                                    const OFBool checkValue = OFTrue);

  /** Set Performing Physician Name
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPerformingPhysicianName(const OFString &value,
                                                 const OFBool checkValue = OFTrue);

  /** Set Protocol Name
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setProtocolName(const OFString &value,
                                      const OFBool checkValue = OFTrue);

  /** Set Series Description
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSeriesDescription(const OFString &value,
                                            const OFBool checkValue = OFTrue);

  /** Set Operators' Name
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1-n)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setOperatorsName(const OFString &value,
                                       const OFBool checkValue = OFTrue);

  /** Set Body Part Examined
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBodyPartExamined(const OFString &value,
                                          const OFBool checkValue = OFTrue);

  /** Set Patient Position
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientPosition(const OFString &value,
                                        const OFBool checkValue = OFTrue);

private:

  /// The name of this module ("GeneralSeriesModule")
  static const OFString m_ModuleName;

  /// Referenced Performed Procedure Step Sequence
  SOPInstanceReferenceMacro m_ReferencedPPS;

};

#endif // MODGENERALSERIES_H
