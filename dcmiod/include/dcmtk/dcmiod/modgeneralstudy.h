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
 *  Purpose: Class for managing the General Study Module
 *
 */

#ifndef MODGENERALSTUDY_H
#define MODGENERALSTUDY_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodmacro.h"

/** Class representing the General Study Module:
  * Notation is "Attribute Name: (VR, VM, Type)"
  *
  * Study Instance UID: (UI, 1, 1)
  * Study Date: (DA, 1, 2)
  * Study Time: (TM, 1, 2)
  * Referring Physician's Name: (PN, 1, 2)
  * Study ID: (SH, 1, 2)
  * Accession Number: (SH, 1, 2)
  * Study Description: (LO, 1, 3)
  * Issuer of Accession Number Sequence: (SQ, 3, 1)
  * Procedure Code Sequence: (SQ, 1-n, 3)
  * Reason for Performed Procedure Code Sequence: (SQ, 1-n, 3)
  */
class DCMTK_DCMIOD_EXPORT IODGeneralStudyModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODGeneralStudyModule(OFshared_ptr<DcmItem> item,
                        OFshared_ptr<IODRules> rules);
  /** Constructor
   */
  IODGeneralStudyModule();

  /** Destructor
   */
  virtual ~IODGeneralStudyModule();

  /** Set missing values by inventing "default values". Automatically
   *  called during write() by IODComponent.
   */
  virtual void inventMissing();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module
   *  @return Name of the module ("GeneralStudyModule")
   */
  virtual OFString getName() const;

  /** Clear (removes) all attributes handled by the modules of this component.
   *  Rules are not reset.
   */
  virtual void clearData();

  /** Read data into this module from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data in this module is cleared first (default: OFTrue)
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write this module to given item
   *  @param  destination The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Make sure that the module contains a Study Instance UID, i.e.\ a new one
   *  is created if empty. An invalid UID is corrected if desired.
   *  @param  correctInvalid Correct invalid UID if OFTrue, otherwise do nothing
   */
  virtual void ensureInstanceUID(const OFBool correctInvalid = OFFalse);

 /** Get Study Instance UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getStudyInstanceUID(OFString &value,
                                          const signed long pos = 0) const;

   /** Get Study Date
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos   Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getStudyDate(OFString &value,
                                    const signed long pos = 0) const;

   /** Get Study Time
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos  Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getStudyTime(OFString &value,
                                    const signed long pos = 0) const;

   /** Get Referring Physician's Name
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getReferringPhysicianName(OFString &value,
                                                const signed long pos = 0) const;

   /** Get Study ID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getStudyID(OFString &value,
                                 const signed long pos = 0) const;

   /** Get Accession Number
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getAccessionNumber(OFString &value,
                                         const signed long pos = 0) const;

   /** Get Study Description
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getStudyDescription(OFString &value,
                                          const signed long pos = 0) const;

  /** Get reference to content of Issuer of Accession Number Sequence
   *  @return Reference to single item of Issuer of Accession Number Sequence
   */
  virtual HL7HierarchicDesignatorMacro& getIssuerOfAccesionNumber();

  /** Get reference to content of Procedure Code Sequence
   *  @return Reference to items of Procedure Code Sequence
   */
  virtual OFVector<CodeSequenceMacro*>& getProcedureCodeSequence();

  /** Get reference to content of Reason for Performed Procedure Code Sequence
   *  @return Reference to items of Procedure Code Sequence
   */
  virtual OFVector<CodeSequenceMacro*>& getReasonForPerformedProcedureCodeSequence();

  /** Set Study Instance UID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setStudyInstanceUID(const OFString &value,
                                          const OFBool checkValue = OFTrue);

  /** Set Study Date
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setStudyDate(const OFString &value,
                                    const OFBool checkValue = OFTrue);

  /** set study time
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setStudyTime(const OFString &value,
                                    const OFBool checkValue = OFTrue);
  /** Set Referring Physician's Name
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setReferringPhysicianName(const OFString &value,
                                                const OFBool checkValue = OFTrue);

  /** Set Study ID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (SH) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setStudyID(const OFString &value,
                                  const OFBool checkValue = OFTrue);

  /** Set Accession Number
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (SH) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAccessionNumber(const OFString &value,
                                          const OFBool checkValue = OFTrue);

  /** Set Study Description
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setStudyDescription(const OFString &value,
                                          const OFBool checkValue = OFTrue);

private:

  /// Name of this module ("GeneralStudyModule")
  static const OFString m_ModuleName;

  /// Issuer of Accession Number Sequence
  HL7HierarchicDesignatorMacro m_IssuerOfAccessionNumberSequence;

  /// Procedure Code Sequence
  OFVector<CodeSequenceMacro*> m_ProcedureCodeSequence;

  /// Reason for Performed Procedure Code Sequence
  OFVector<CodeSequenceMacro*> m_ReasonForPerformedProcedureCodeSequence;

};

#endif // MODGENERALSTUDY_H
