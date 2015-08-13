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
 *  Purpose: Class managing the Ultrasound Frame of Reference Module
 *
 */

#ifndef MODUSFOR_H
#define MODUSFOR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofoption.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Frame of Reference Module:
 *
 *  Volume Frame of Reference UID: (UI, 1, 1)
 *  Ultrasound Acquisition Geometry: (CS, 1, 1)
 *  Apex Position: (FD, 3, 1C)
 *  Volume to Transducer Relationship: (CS, 1, 1C)
 *  Volume to Transducer Mapping Matrix: (FD, 16, 1)
 *  Patient Frame of Reference Source: (CS, 1, 1C)
 *  Table Frame of Reference UID: (UI, 1, 1C)
 *  Volume to Table Mapping Matrix: (FD, 16, 1C)
 *
 */
class DCMTK_DCMIOD_EXPORT IODUSFoRModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODUSFoRModule(OFshared_ptr<DcmItem> item,
                 OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODUSFoRModule();

  /** Destructor
   */
  virtual ~IODUSFoRModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module
   *  @return Name of the module ("UltrasoundFrameOfReferenceModule")
   */
  virtual OFString getName() const;

  /** Get Volume Frame of Reference UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumeFrameOfReferenceUID(OFString& value,
                                                   const signed long pos = 0) const;

  /** Get Position Reference Indicator
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getUltrasoundAcquisitionGeometry(OFString& value,
                                                       const signed long pos = 0) const;

  /** Get Apex Position
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getApexPosition(Float64& value,
                                      const unsigned long pos = 0) const;

  /** Get Apex Position
   *  @param  value Reference to variable in which the value should be stored
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getApexPosition(OFVector<Float64>& value);

  /** Get Volume to Transducer Relationship
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumetoTransducerRelationship(OFString& value,
                                                        const signed long pos = 0);

  /** Get Volume to Table Mapping Matrix
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumetoTransducerMappingMatrix(Float64& value,
                                                         const signed long pos = 0) const;

  /** Get Volume to Table Mapping Matrix
   *  @param  value Reference to variable in which the value should be stored
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumetoTransducerMappingMatrix(OFVector<Float64>& value);

  /** Get Patient Frame of Reference Source
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPatientFrameofReferenceSource(OFString& value,
                                                       const signed long pos = 0);

  /** Get Table Frame of Reference UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getTableFrameofReferenceUID(OFString& value,
                                                  const signed long pos = 0);

  /** Get Volume to Table Mapping Matrix
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumetoTableMappingMatrix(Float64& value,
                                                    const signed long pos = 0) const;

  /** Get Volume to Table Mapping Matrix
   *  @param  value Reference to variable in which the value should be stored
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumetoTableMappingMatrix(OFVector<Float64>& value);

  /** Set Volume Frame Of Reference UID
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setVolumeFrameOfReferenceUID(const OFString &value,
                                                   const OFBool checkValue = OFTrue);

  /** Set Ultrasound Acquisition Geometry
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setUltrasoundAcquisitionGeometry(const OFString &value,
                                                       const OFBool checkValue = OFTrue);

  /** Set Apex Position
   *  @param  xValue xValue to be set
   *  @param  yValue yValue to be set
   *  @param  zValue zValue to be set
   *  @param  checkValue Does nothing at the moment
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setApexPosition(const Float64& xValue,
                                      const Float64& yValue,
                                      const Float64& zValue,
                                      const OFBool checkValue = OFTrue);

  /** Set Volume to Transducer Relationship
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setVolumetoTransducerRelationship(const OFString &value,
                                                        const OFBool checkValue = OFTrue);

  /** Set Volume to Transducer Mapping Matrix
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (FD) and VM (16) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setVolumetoTransducerMappingMatrix(const OFVector<Float64>& value,
                                                         const OFBool checkValue = OFTrue);

  /** Set Patient Frame of Reference Source
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientFrameOfReferenceSource(const OFString &value,
                                                       const OFBool checkValue = OFTrue);

  /** Set Table Frame of Reference UID
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setTableFrameofReferenceUID(const OFString &value,
                                                  const OFBool checkValue = OFTrue);

  /** Set Volume to Table Mapping Matrix
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (FD) and VM (16) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setVolumeToTableMappingMatrix(const OFVector<Float64>& value,
                                                    const OFBool checkValue = OFTrue);

private:

  /// Module name "UltrasoundFrameOfReferenceModule"
  static const OFString m_ModuleName;

};

#endif // MODUSFOR_H
