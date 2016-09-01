/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Module: dcmpmap
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the Parametric Map Image Module
 *
 */

#ifndef DPMMODPARAMETRICMAPIMAGE_H
#define DPMMODPARAMETRICMAPIMAGE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmpmap/dpmdef.h"

/** Class representing the Parametric Map Image Module:
 *
 *  ImageType: (CS, 2-n, 1)
 *  SamplesPerPixel: (US, 1, 1)
 *  PhotometricInterpretation: (CS, 1, 1)
 *  BitsAllocated: (US, 1, 1)
 *  BitsStored: (US, 1, 1C)
 *  HighBit: (US, 1, 1C)
 *  PresentationLUTShape: (CS, 1, 1)
 *  LossyImageCompression: (CS, 1, 1)
 *  LossyImageCompressionRatio: (DS, 1-n, 1C)
 *  LossyImageCompressionMethod: (CS, 1-n, 1C)
 *  BurnedInAnnotation: (CS, 1, 1)
 *  RecognizableVisualFeatures: (CS, 1, 1)
 *  ContentQualification: (CS, 1, 1)
 */
class DCMTK_DCMPMAP_EXPORT DPMParametricMapImageModule : public IODModule
{
public:
  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  DPMParametricMapImageModule(OFshared_ptr<DcmItem> item,
                              OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  DPMParametricMapImageModule();

  /** Destructor
   */
  virtual ~DPMParametricMapImageModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("DPMParametricMapImageModule")
   *  @return Name of the module ("DPMParametricMapImageModule")
   */
  virtual OFString getName() const;

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

  /** Get Image Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageType(OFString &value,
                                   const signed long pos = 0) const;

  /** Get Samples Per Pixel
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSamplesPerPixel(Uint16 &value,
                                         const signed long pos = 0) const;

  /** Get Photometric Interpretation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPhotometricInterpretation(OFString &value,
                                                   const signed long pos = 0) const;

  /** Get Bits Allocated
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getBitsAllocated(Uint16 &value,
                                       const signed long pos = 0) const;

  /** Get Bits Stored
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getBitsStored(Uint16 &value,
                                    const signed long pos = 0) const;

  /** Get High Bit
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getHighBit(Uint16 &value,
                                 const signed long pos = 0) const;

  /** Get Presentation LUT Shape
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPresentationLUTShape(OFString &value,
                                              const signed long pos = 0) const;

  /** Get Lossy Image Compression
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompression(OFString &value,
                                               const signed long pos = 0) const;

  /** Get Lossy Image Compression Ratio
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompressionRatio(OFString &value,
                                                    const signed long pos = 0) const;

  /** Get Lossy Image Compression Method
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompressionMethod(OFString &value,
                                                     const signed long pos = 0) const;

  /** Get Burned In Annotation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getBurnedInAnnotation(OFString &value,
                                            const signed long pos = 0) const;

  /** Get Recognizable Visual Features
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRecognizableVisualFeatures(OFString &value,
                                                    const signed long pos = 0) const;

  /** Get Content Qualification
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getContentQualification(OFString &value,
                                              const signed long pos = 0) const;

  /** Set Image Type
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (2-n) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setImageType(const OFString &value,
                                   const OFBool checkValue = OFTrue);

  /** Set Samples Per Pixel
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (US) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSamplesPerPixel(const Uint16 &value,
                                         const OFBool checkValue = OFTrue);

  /** Set Photometric Interpretation
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPhotometricInterpretation(const OFString &value,
                                                   const OFBool checkValue = OFTrue);

  /** Set Bits Allocated
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (US) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBitsAllocated(const Uint16 &value,
                                       const OFBool checkValue = OFTrue);

  /** Set Bits Stored
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (US) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBitsStored(const Uint16 &value,
                                    const OFBool checkValue = OFTrue);

  /** Set High Bit
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (US) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setHighBit(const Uint16 &value,
                                 const OFBool checkValue = OFTrue);

  /** Set Presentation LUT Shape
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPresentationLUTShape(const OFString &value,
                                              const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompression(const OFString &value,
                                               const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression Ratio
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1-n) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompressionRatio(const OFString &value,
                                                    const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression Method
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1-n) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompressionMethod(const OFString &value,
                                                     const OFBool checkValue = OFTrue);

  /** Set Burned In Annotation
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBurnedInAnnotation(const OFString &value,
                                            const OFBool checkValue = OFTrue);

  /** Set Recognizable Visual Features
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecognizableVisualFeatures(const OFString &value,
                                                    const OFBool checkValue = OFTrue);

  /** Set ContentQualification
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setContentQualification(const OFString &value,
                                              const OFBool checkValue = OFTrue);

private:
  /// The name of this module ("DPMParametricMapImageModule")
  static const OFString m_ModuleName;
};

#endif // DPMMODPARAMETRICMAPIMAGE_H
