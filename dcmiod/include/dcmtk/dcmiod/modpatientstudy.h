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
 *  Purpose: Class for managing the Patient Study Module
 *
 */

#ifndef MODPATIENTSTUDY_H
#define MODPATIENTSTUDY_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofoption.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Patient Study Module:
  *
  * Admitting Diagnoses Description: (LO, 1-n, 3)
  * Patient's Age: (AS, 1, 3)
  * Patient's Size: (DS, 1, 3)
  * Patient's Weight: (DS,1, 3)
  */
class DCMTK_DCMIOD_EXPORT IODPatientStudyModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODPatientStudyModule(OFshared_ptr<DcmItem> item,
                        OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODPatientStudyModule();

  /** Destructor
   */
  virtual ~IODPatientStudyModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("PatientStudyModule")
   *  @return Name of the module ("PatientStudyModule")
   */
  virtual OFString getName() const;

   /** Get Admitting Diagnoses Description
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getAdmittingDiagnosesDescription(OFString& value,
                                                       const signed long pos = 0) const;

  /** Get Patient's Age (string as defined by VR "AS", see DICOM part 5)
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPatientAge(OFString &value,
                                    const signed long pos = 0) const;

  /** Get Patient's Weight (in kilograms)
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPatientWeight(Float64 &value,
                                       const unsigned long pos = 0) const;

  /** Get Patient's Size (in meters)
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPatientSize(Float64 &value,
                                     const unsigned long pos = 0) const;

  /** Set Admitting Diagnoses Description
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1-n)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAdmittingDiagnosesDescription(const OFString &value,
                                                       const OFBool checkValue = OFTrue);

  /** Set Patient's Age (formatted as defined by VR "AS", see part 5 of the standard)
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (AS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientAge(const OFString &value,
                                    const OFBool checkValue = OFTrue);

  /** Set Patient's Size (in meters)
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1)
   *         if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientSize(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Set Patient's Weight (in kilograms)
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientWeight(const OFString &value,
                                       const OFBool checkValue = OFTrue);

private:

  /// The name of this module ("PatientStudyModule")
  static const OFString m_ModuleName;
};

#endif // MODPATIENT_H
