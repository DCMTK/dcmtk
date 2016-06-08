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
 *  Purpose: Class for managing the General Image Module
 *
 */

#ifndef MODGENERALIMAGE_H
#define MODGENERALIMAGE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"


/** Class representing the General Image Module:
  *
  * Instance Number: (IS, 1, 2)
  * Patient Orientation: (CS, 2, 2)
  * Content Date:    (DA, 1, 2C)
  * Content Time:    (TM, 1, 2C)
  * Image Type:      (CS, 1, 2-n)
  * Acquisition Number (IS, 1, 3)
  * Acquisition Date (DA, 1, 3)
  * Acquisition Time (TM, 1, 3)
  * Acquisition Date Time (DT, 1, 3)
  * Image Comments   (LT, 1, 3)
  * Burned In Annotation (CS, 1, 3)
  * Recognizable Visual Features (CS, 1, 3)
  * Lossy Image Compression (CS, 1, 3)
  * Lossy Image Compression Ratio (DS, 1, 3)
  * Lossy Image Compression Method (CS, 1-n, 3)
  * Presentation LUT Shape (CS, 1, 3)
  * Irradiation Event UID (CS, 1-n, 3)
  *
  */
class DCMTK_DCMIOD_EXPORT IODGeneralImageModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODGeneralImageModule(OFshared_ptr<DcmItem> item,
                        OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODGeneralImageModule();

  /** Destructor
   */
  virtual ~IODGeneralImageModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module
   *  @return Name of the module ("GeneralImageModule")
   */
  virtual OFString getName() const;

  /** Get Instance Number
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getInstanceNumber(OFString &value,
                                        const signed long pos = 0);

  /** Get Patient Orientation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return  EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPatientOrientation(OFString &value,
                                            const signed long pos = 0);

  /** Get Content Date
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getContentDate(OFString &value,
                                     const signed long pos = 0);

  /** Get Content Time
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getContentTime(OFString &value,
                                     const signed long pos = 0);

  /** Get Image Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageType(OFString &value,
                                   const signed long pos = 0);

  /** Get Acquisition Number
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getAcquisitionNumber(OFString &value,
                                           const signed long pos = 0);

  /** Get Acquisition Date
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getAcquisitionDate(OFString &value,
                                         const signed long pos = 0);

  /** Get Acquisition Time
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getAcquisitionTime(OFString &value,
                                         const signed long pos = 0);

  /** Get Acquisition Date Time
   *  @param  value  Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getAcquisitionDateTime(OFString &value,
                                             const signed long pos = 0);

  /** Get Image Comments
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageComments(OFString &value,
                                       const signed long pos = 0);

  /** Get Burned In Annotation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getBurnedInAnnotation(OFString &value,
                                            const signed long pos = 0);

  /** Get Recognizable Visual Features
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos  Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRecognizableVisualFeatures(OFString &value,
                                                    const signed long pos = 0);

  /** Get Lossy Image Compression
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompression(OFString &value,
                                               const signed long pos = 0);

  /** Get Lossy Image Compression
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompressionRatio(OFString &value,
                                                    const signed long pos = 0);

  /** Get Lossy Image Compression Method
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompressionMethod(OFString &value,
                                                    const signed long pos = 0);

  /** Get Presentation LUT Shape
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPresentationLUTShape(OFString &value,
                                              const signed long pos = 0);

  /** Get Irridation Event UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos  Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getIrradiationEventUID(OFString &value,
                                             const signed long pos = 0);

  /** Set Instance Number
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setInstanceNumber(const OFString &value,
                                        const OFBool checkValue = OFTrue);

  /** Set Patient Orientation
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (2) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPatientOrientation(const OFString &value,
                                            const OFBool checkValue = OFTrue);

  /** Set Content Date
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setContentDate(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Set Content Time
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setContentTime(const OFString &value,
                                     const OFBool checkValue = OFTrue);


  /** Set Image Type
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (2-n) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setImageType(const OFString &value,
                                   const OFBool checkValue = OFTrue);

  /** Set Acquisition Number
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAcquisitionNumber(const OFString &value,
                                           const OFBool checkValue = OFTrue);

  /** Set Acquisition Date
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAcquisitionDate(const OFString &value,
                                         const OFBool checkValue = OFTrue);

  /** Set Acquisition Time
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAcquisitionTime(const OFString &value,
                                         const OFBool checkValue = OFTrue);

  /** Set Acquisition Date Time
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (DT) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAcquisitionDateTime(const OFString &value,
                                             const OFBool checkValue = OFTrue);

  /** Set Image Comments
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (LT) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setImageComments(const OFString &value,
                                       const OFBool checkValue = OFTrue);

  /** Set Burned In Annotation
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBurnedInAnnotation(const OFString &value,
                                            const OFBool checkValue = OFTrue);

  /** Set Recognizable Visual Features
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecognizableVisualFeatures(const OFString &value,
                                                    const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompression(const OFString &value,
                                               const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression Ratio
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompressionRatio(const OFString &value,
                                                    const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression method
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1-n) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompressionMethod(const OFString &value,
                                                     const OFBool checkValue = OFTrue);

  /** Set Presentation LUT Shape
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPresentationLUTShape(const OFString &value,
                                              const OFBool checkValue = OFTrue);

  /** Set Irradiation Event UID
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1-n) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setIrradiationEventUID(const OFString &value,
                                             const OFBool checkValue = OFTrue);

private:

  /// The module's name ("GeneralIamgeModule")
  static const OFString m_ModuleName;
};

#endif // MODGENERALIMAGE_H
