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
 *  Purpose: Class for managing the Enhanced US Image Module
 *
 */

#ifndef MODENHUSIMAGE_H
#define MODENHUSIMAGE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodmacro.h"

/** Class representing the Enhanced US Image Module:
 *
 *  Image Type: (CS, 2 as defined in part 3, 1)
 *  Samples Per Pixel: (US, 1, 1)
 *  Photometric Interpretation (CS, 1, 1)
 *  Rows: (US, 1, 1)
 *  Columns: (US, 1, 1)
 *  Bits Allocated: (US, 1, 1)
 *  Bits Stored: (US, 1, 1)
 *  High Bit: (US, 1, 1)
 *  Pixel Representation: (US, 1, 1)
 *  Dimension Organization Type (CS, 1, 1)
 *  Acquisition DateTime (DT, 1, 1)
 *  Acquisition Duration (FD, 1, 1)
 *  Pixel Spacing (DS, 2, 1)
 *  Position Measuring Device Used (CS, 1, 1C)
 *  Lossy Image Compression: (CS,  1, 1)
 *  Lossy Image Compression Ratio: (DS, 1-n, 1C)
 *  Lossy Image Compression Method: (CS, 1-n, 1C)
 *  Presentation LUT Shape: (CS, 1, 1)
 *  Rescale Slope: (DS, 1, 1)
 *  Rescale Intercept: (DS, 1, 1)
 *  Mandatory View And Slice Progression Macro (type 1)
 *  Burned In Annotation: (CS, 1, 1)
 *  Recognizable Visual Features: (CS, 1, 3)
 *  Transducer Scan Pattern Code Sequence: (SQ, 1, 1)
 *  Transducer Geometry Code Sequence: (SQ, 1, 1)
 *  Transducer Beam Steering Code Sequence: (SQ, 1, 1)
 *  Transducer Application Code Sequence: (SQ, 1, 1)
 *  Processing Function: (LO, 1, 3)
 *  Mechanical Index: (DS, 1, 1)
 *  Bone Thermal Index: (DS, 1, 1)
 *  Cranial Thermal Index: (DS, 1, 1)
 *  Soft Tissue Thermal Index: (DS, 1, 1)
 *  Depth(s) of Focus: (FD, 1-n, 1)
 *  Depth(s) of Scan Field: (IS, 1, 1)
 */
class DCMTK_DCMIOD_EXPORT IODEnhUSImageModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set where this classes rules are added to. If NULL, the
   *          class creates an empty rule set.
   */
  IODEnhUSImageModule(OFshared_ptr<DcmItem> item,
                      OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODEnhUSImageModule();

  /** Destructor
   */
  virtual ~IODEnhUSImageModule();

  /** Resets rules to their original values.
   */
  virtual void resetRules();

  /** Get name of module
   *  @return Name of the module ("EnhancedUSImageModule")
   */
  virtual OFString getName() const;

  /** Read attributes from given item into this class. Overwrites
   *  fucntion of base class IODComponent.
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
   *         old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write attributes from this class into given item. Overwrites
   *  function of base class IODComponent.
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Get Image Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageType(OFString &value,
                                   const signed long pos = 0);

  /** Get Samples Per Pixel
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
  */
  virtual OFCondition getSamplesPerPixel(Uint16& value,
                                         const unsigned long pos = 0);

  /** Get Photometric Interpretation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPhotometricInterpretation(OFString& value,
                                                   const signed long pos = 0);

  /** Get Rows
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRows(Uint16& value,
                              const unsigned long pos = 0);

  /** Get Columns
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getColumns(Uint16& value,
                                  const unsigned long pos = 0);
   /** Get Bits Allocated
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getBitsAllocated(Uint16& value,
                                       const unsigned long pos = 0);

   /** Get Bits Stored
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getBitsStored(Uint16& value,
                                  const unsigned long pos = 0);

  /** Get High Bit
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getHighBit(Uint16& value,
                                 const unsigned long pos = 0);

  /** Get Pixel Representation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPixelRepresentation(Uint16& value,
                                             const unsigned long pos = 0);

  /** Get Dimension Organization Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getDimensionOrganizationType(OFString& value,
                                                   const signed long pos = 0);

  /** Get Acquisition DateTime
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getAcquisitionDateTime(OFString& value,
                                             const signed long pos = 0);

  /** Get Acquisition Duration
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getAcquisitionDuration(Float64& value,
                                             const unsigned long pos = 0);

  /** Get Pixel Spacing
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPixelSpacing(Float64& value,
                                      const unsigned long pos = 0);

  /** Get Position Measuring Device
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPositionMeasuringDevice(OFString& value,
                                                 const signed long pos = 0);

  /** Get Lossy Image Compression
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompression(OFString& value,
                                               const signed long pos = 0);

  /** Get Lossy Image Compression Ratio
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompressionRatio(Float64& value,
                                                    const unsigned long pos = 0);

  /** Get Lossy Image Compression Method
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLossyImageCompressionMethod(OFString& value,
                                                     const signed long pos = 0);

  /** Get Presentation LUT Shape
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPresentationLUTShape(OFString& value,
                                              const signed long pos = 0);
  /** Get Rescale Slope
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRescaleSlope(Float64& value,
                                      const unsigned long pos = 0);
  /** Get Rescale Intercept
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRescaleIntercept(OFString& value,
                                          const signed long pos = 0);

  /** Get Burned-In Annotation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getBurnedInAnnotation(OFString& value,
                                            const signed long pos = 0);
  /** Get Recognizable Visual Features
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRecognizableVisibleFeatures(OFString& value,
                                                     const signed long pos = 0);
  /** Get Mandatory View And Slice Progression Direction
   *  @return Reference to Mandatory View And Slice Progression Direction
   */
  virtual MandatoryViewAndSliceProgressionDirectionMacro& getMandatoryViewAndSliceProgressionDirection();

  /** Get Anatomy
   *  @return Reference to Anatomy
   */
  virtual GeneralAnatomyMacro& getAnatomy();

  /** Get Transducer Scan Pattern
   *  @return Reference to Transducer Scan Pattern
   */
  virtual CodeSequenceMacro& getTransducerScanPattern();

  /** Get Transducer Geometry Code
   *  @return Reference to Transducer Geometry
   */
  virtual CodeSequenceMacro& getTransducerGeometry();

  /** Get Transducer Beam Steering
   *  @return Reference to Transducer Beam Steering
   */
  virtual OFVector<CodeSequenceMacro*>& getTransducerBeamSteering();

  /** Get Transducer Application
   *  @return Reference to Transducer Application
   */
  virtual CodeSequenceMacro& getTransducerApplication();

  /** Get Processing Function
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getProcessingFunction(OFString& value,
                                            const signed long pos = 0);
  /** Get Mechanical Index
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getMechanicalIndex(Float64& value,
                                         const unsigned long pos = 0);

  /** Get Bone Thermal Index
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getBoneThermalIndex(Float64& value,
                                          const unsigned long pos = 0);

  /** Get Cranial Thermal Index
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getCranialThermalIndex(Float64& value,
                                             const unsigned long pos = 0);

  /** Get Soft Tissue Thermal Index
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all values
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSoftTissueThermalIndex(Float64& value,
                                                const unsigned long pos = 0);

  /** Get Depth(s) of Focus
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getDepthsOfFocus(Float64& value,
                                       const unsigned long pos = 0);

  /** Get Depth(s) of Scan Field
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getDepthsOfScanField(Float64& value,
                                           const unsigned long pos = 0);

  /** Set Image Type. Image Type contains up to four values. Value 3 and 4 are
   *  optional and can be left empty if desired. The values in their order
   *  of occurrence are:
   *  1) Pixel Data Characteristics: Either ORIGINAL or DERIVED
   *  2) Patient Examination Characteristics: Fixed to "PRIMARY", thus cannot be
   *  influenced through this function.
   *  3) Image Flavor: Defined Terms listed in the standard
   *  4) Derived Pixel Contrast: Defined Terms listed in the standard
   *  @param  pixelDataChar Value 1 of Image Type
   *  @param  imageFlavor Value 3 of Image Type
   *  @param  derivedPixelContrast Value 4 of Image Type
   *  @param  checkValue If OFTrue, the value is checked for conformance.
   *  @return EC_Normal if setting was successful, error otherwise.
   */
  virtual OFCondition setImageType(const DcmIODTypes::IOD_ENHUSIMAGETYPE pixelDataChar,
                                   const OFString& imageFlavor = "",
                                   const OFString& derivedPixelContrast = "",
                                   const OFBool checkValue = OFTrue);

  /** Set Rows
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRows(const Uint16 value,
                              const OFBool checkValue = OFTrue);

  /** Set Columns
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setColumns(const Uint16 value,
                                 const OFBool checkValue = OFTrue);
  /** Set Bits Allocated
   *  @param  value Value to be set, permitted values: 8 or 16
   *  @param  checkValue Check 'value' for conformance if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBitsAllocated(const Uint16 value,
                                       const OFBool checkValue = OFTrue);

  /** Set Bits Stored
   *  @param  value Value to be set, permitted values: 8 or 16
   *  @param  checkValue Check 'value' for conformance if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBitsStored(const Uint16 value,
                                    const OFBool checkValue = OFTrue);

  /** Set High Bit
   *  @param  value Value to be set, permitted values: 7 or 15
   *  @param  checkValue Check 'value' for conformance if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setHighBit(const Uint16 value,
                                 const OFBool checkValue = OFTrue);

  /** Set Dimension Organization Type
   *  @param  value Value to be set, permitted values: "3D" or "3D_TEMPORAL"
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (CS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setDimensionOrganizationType(const OFString& value,
                                                   const OFBool checkValue = OFTrue);

  /** Set Acquisition DateTime
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (DT) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAcquisitionDateTime(const OFString& value,
                                             const OFBool checkValue = OFTrue);

  /** Set Acquisition Duration
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled,
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setAcquisitionDuration(const Float64 value,
                                             const OFBool checkValue = OFTrue);

  /** Set Pixel Spacing
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (DS) and VM (2)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPixelSpacing(const OFString& value,
                                      const OFBool checkValue = OFTrue);

  /** Set Position Measuring Device
   *  @param  value Value to be set, permitted values: "RIGID" or "FREEHAND"
   *  @param  checkValue Check 'value' for conformance if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPositionMeasuringDevice(const OFString& value,
                                                 const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression
   *  @param  value Value to be set, permitted values: "00" (not lossy compressed) or "01" (lossy compressed)
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (CS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompression(const OFString& value,
                                               const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression Ratio
   *  @param  value Value to be set, including VR (DS) and VM (1-n)
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (DS)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompressionRatio(const OFString& value,
                                                    const OFBool checkValue = OFTrue);

  /** Set Lossy Image Compression Method
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (CS)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLossyImageCompressionMethod(const OFString& value,
                                                     const OFBool checkValue = OFTrue);

  /** Set Recognizable Visual Features
   *  @param  value Value to be set, permitted values: "YES" or "NO"
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (CS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecognizableVisibleFeatures(const OFString& value,
                                                     const OFBool checkValue = OFTrue);

  /** Set Processing Funcion
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (LO) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setProcessingFunction(const OFString& value,
                                            const OFBool checkValue = OFTrue);

  /** Set Mechanical Index
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (DS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setMechanicalIndex(const OFString& value,
                                         const OFBool checkValue = OFTrue);

  /** Set Bone Thermal Index
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (DS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBoneThermalIndex(const OFString& value,
                                          const OFBool checkValue = OFTrue);

  /** Set Cranial Thermal Index
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (DS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setCranialThermalIndex(const OFString& value,
                                             const OFBool checkValue = OFTrue);

  /** Set Soft Tissue Thermal Index
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (DS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSoftTissueThermalIndex(const OFString& value,
                                                const OFBool checkValue = OFTrue);

  /** Set Depth(s) of Focus
   *  @param  value Value to be set
   *  @param  pos Index of the value to set (0..vm-1)
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency with
   *          other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setDepthsOfFocus(const Float64 value,
                                       const unsigned long pos,
                                       const OFBool checkValue = OFTrue);

  /** Set Depth(s) of Focus (convenience function)
   *  @param  values Value(s) to be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency with
   *          other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setDepthsOfFocus(const OFVector<Float64>& values,
                                       const OFBool checkValue = OFTrue);

  /** Set Depth(s) of Scan Field
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled, including VR (IS) and VM (1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setDepthsOfScanField(const OFString& value,
                                           const OFBool checkValue = OFTrue);


private:

  /// This module's name ("EnhancedUSImageModule")
  static const OFString m_ModuleName;

  /// Mandatory View and Slice Progression Direction
  MandatoryViewAndSliceProgressionDirectionMacro m_MandatoryViewAndSliceProgressionDirection;

  /// General Anatomy Mandatory Macro
  GeneralAnatomyMacro m_Anatomy;

  /// Item of Transducer Scan Pattern Code Sequence
  CodeSequenceMacro m_TransducerScanPattern;

  /// Item of Transducer Geometry Code Sequence
  CodeSequenceMacro m_TransducerGeometry;

  /// Item of Transducer Beam Steering Code Sequence
  OFVector<CodeSequenceMacro*> m_TransducerBeamSteering;

  /// Item of Transducer Application Code Sequence
  CodeSequenceMacro m_TransducerApplication;
};

#endif // MODENHUSIMAGE_H
