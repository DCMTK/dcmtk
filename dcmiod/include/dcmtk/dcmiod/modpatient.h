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
 *  Purpose: Class for managing the Patient Module
 *
 */

#ifndef MODPATIENT_H
#define MODPATIENT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Patient Module:
  *
  * Patient's Name: (PN, 1, 2)
  * Patient ID: (LO, 1, 2)
  * Patient's Birth Date: (DA, 1, 2)
  * Patient's Sex: (CS, 1, 2)
  */
class DCMTK_DCMIOD_EXPORT IODPatientModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODPatientModule(OFshared_ptr<DcmItem> item,
                   OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODPatientModule();

  /** Destructor
   */
  virtual ~IODPatientModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("PatientModule")
   *  @return Name of the module ("PatientModule")
   */
  virtual OFString getName() const;

   /** Get Patient's Name
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getPatientName(OFString &value,
                                     const signed long pos = 0) const;

  /** Get Patient ID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPatientID(OFString &value,
                                    const signed long pos = 0) const;

   /** Get Patient's Birth Date
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getPatientBirthDate(OFString &value,
                                          const signed long pos = 0) const;

   /** Get Patient's Sex
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getPatientSex(OFString &value,
                                    const signed long pos = 0) const;


  /** Set Patient's Name
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientName(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Set Patient ID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientID(const OFString &value,
                                   const OFBool checkValue = OFTrue);

  /** Set Patient's Birth Date
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientBirthDate(const OFString &value,
                                          const OFBool checkValue = OFTrue);

  /** Set Patient's Sex
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientSex(const OFString &value,
                                    const OFBool checkValue = OFTrue);

private:

  /// The name of this module ("PatientModule")
  static const OFString m_ModuleName;
};

#endif // MODPATIENT_H
