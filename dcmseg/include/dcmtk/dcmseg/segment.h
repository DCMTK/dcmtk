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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing a Segment from the Segment Ident. Sequence
 *
 */

#ifndef SEGMENT_H
#define SEGMENT_H

#include "dcmtk/config/osconfig.h"  // include OS configuration first
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmseg/segtypes.h"
#include "dcmtk/dcmseg/segdoc.h"

/** Class representing a segment from the Segment Identification Sequence,
 *  as used within the Segmentation Image Module. It includes the Segment
 *  Description Macro.
 */

class DCMTK_DCMSEG_EXPORT DcmSegment
{

public:

  // -------------- constructors/destructors --------------------

  // constructor is protected

  /** Destructor, frees memory
   */
  virtual ~DcmSegment();

  /** Clears all data
   */
  virtual void clearData();

  // -------------- creation --------------------

  /** Factory method to create a Segment that expects the minimal parameters
   *  required.
   *  @param  segment Pointer to the resulting segment if creation was
   *          successful; memory is allocated by the function
   *  @param  segmentLabel Free text label for the segment
   *  @param  segmentedPropertyCategory The segmented property category.
   *          Baseline CID 7150 “Segmentation Property Categories” should be
   *          used.
   *  @param  segmentedPropertyType The segmented property type. Baseline CID
   *          7151 “Segmentation Property Types”
   *  @param  algoType The algorithm type used for segmenting this segment
   *  @param  algoName Algorithm name (required if algoType is not MANUAL)
   *  @return EC_Normal if creation was successful, error otherwise
   */
  static OFCondition create(DcmSegment*& segment,
                            const OFString& segmentLabel,
                            const CodeSequenceMacro& segmentedPropertyCategory,
                            const CodeSequenceMacro& segmentedPropertyType,
                            const DcmSegTypes::E_SegmentAlgoType algoType,
                            const OFString& algoName = "");

  // ---------------- writing --------------------

  /** Write segment to given item which is usually contained within
   *  within the Segment Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if successful, error otherwise
   */
  OFCondition write(DcmItem& item);

  // ---------------- reading --------------------

  /** Read segment from given item
   *  @param  item The item to read from (must be item as found in the
   *          Segment Identification Sequence)
   *  @param  clearOldData If true, old data is deleted first
   *  @return  EC_Normal if reading was successful, error otherwise
   */
  OFCondition read(DcmItem& item,
                   const OFBool clearOldData = OFTrue);

  // ---------------- access --------------------

  /** Get Segment Number
   *  @return The Segment number
   */
  virtual unsigned int getSegmentNumber();

  /** Get Segment Label
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSegmentLabel(OFString& value,
                                      const signed long pos = 0);

  /** Get Segment Description
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSegmentDescription(OFString& value,
                                            const signed long pos = 0);

  /** Get the Segment Algorithm Type
   *  @return The Algorithm Type
   */
  virtual DcmSegTypes::E_SegmentAlgoType getSegmentAlgorithmType();

  /** Get the Segment Algorithm Name
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSegmentAlgorithmName(OFString& value,
                                              const signed long pos = 0);

  /** Get General Anatomy Code
   *  @return Reference to the General Anatomy Macro, may be unset
   */
  virtual GeneralAnatomyMacro& getGeneralAnatomyCode();

  /** Get Segmented Property Category Code
   *  @return Reference to the Segmented Property Category Code, may be unset
   */
  virtual CodeSequenceMacro& getSegmentedPropertyCategoryCode();

  /** Get Segmented Property Type  Code
   *  @return Reference to the Segmented Property Type Code, may be unset
   */
  virtual CodeSequenceMacro& getSegmentedPropertyTypeCode();

  /** Get Segmented Property Type Modifier Code
   *  @return Reference to the Segmented Property Type Modifier Code, may
   *  be unset
   */
  virtual OFVector<CodeSequenceMacro*>& getSegmentedPropertyTypeModifierCode();

  /** Get Segment Surface Generation Algorithm Identification
   *  @return Reference to the Segment Surface Generation Algorithm
   *  Identification, may be unset
   */
  virtual AlgorithmIdentificationMacro& getSegmentSurfaceGenerationAlgorithmIdentification();

  /** Get Recommended Display Grayscale Value
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */  virtual OFCondition getRecommendedDisplayGrayscaleValue(Uint16& value,
                                                               const unsigned long pos = 0);

  /** Returns Recommended Display CIELab Value
   *  @param  L The L component
   *  @param  a The a* component
   *  @param  b The b* component
   *  @return EC_Normal if values could be returned
   */
  virtual OFCondition getRecommendedDisplayCIELabValue(Uint16& L,
                                                       Uint16& a,
                                                       Uint16& b);

  /** Get Tracking ID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getTrackingID(OFString& value,
                                    const signed long pos = 0);

  /** Get Tracking UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getTrackingUID(OFString& value,
                                     const signed long pos = 0);


  // -------------- setters --------------------

  /** Set Segment Label
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSegmentLabel(const OFString& value,
                                      const OFBool checkValue = OFTrue);

  /** Set Segment Label
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (ST) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSegmentDescription(const OFString& value,
                                            const OFBool checkValue = OFTrue);

  /** Set Segment Algorithm
   *  @param  algoType Algorithm type used to find segment
   *  @param  algoName Name of the algorithm used (required if algorithm type
   *          is not MANUAL
   *  @param  checkValue If OFTrue, input is checked for validity
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setSegmentAlgorithm(const DcmSegTypes::E_SegmentAlgoType algoType,
                                          const OFString& algoName,
                                          const OFBool checkValue = OFTrue);

  /** Set Segment Surface Generation Algorithm Identification
   *  @param  value The algorithm identification
   *  @param  checkValue If OFTrue, value undergoes some validity checks
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setSegmentSurfaceGenerationAlgorithmIdentification(const AlgorithmIdentificationMacro& value,
                                                                         const OFBool checkValue = OFTrue);

  /** Set Recommended Display Grayscale Value
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for validity if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecommendedDisplayGrayscaleValue(const Uint16 value,
                                                          const OFBool checkValue = OFTrue);

  /** Set Recommended Display CIELab Value
   *  @param  L L component
   *  @param  a a component
   *  @param  b b component
   *  @param  checkValue Check 'value' for validity if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecommendedDisplayCIELabValue(const Uint16 L,
                                                       const Uint16 a,
                                                       const Uint16 b,
                                                       const OFBool checkValue = OFTrue);

  /** Set Tracking ID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setTrackingID(const OFString& value,
                                    const OFBool checkValue = OFTrue);

  /** Set Tracking UID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setTrackingUID(const OFString& value,
                                     const OFBool checkValue = OFTrue);

  // The utility class must access the protected default constructor
  friend class DcmIODUtil;

protected:

  /** Protected default constructor
   */
  DcmSegment();

  /** Initialize IOD rules for this component
   */
  virtual void initIODRules();

  /** Set reference to the Segmentation object that this segment belongs to.
   *  This is used to find the Segment Number this Segment has within the
   *  Segmentation.
   *  @param  doc Pointer to the Segmentation object
   */
  void referenceSegmentationDoc(DcmSegmentation* doc);


private:

  /// The segmentation document where this segment is located in
  DcmSegmentation* m_SegmentationDoc;

  /// Segment Description Macro
  SegmentDescriptionMacro m_SegmentDescription;

  /// Segment Algorithm Name: (LO, 1, 1C)
  DcmLongString m_SegmentAlgorithmName;

  /// Segment Surface Generation Algorithm Identification (SQ, 1, 3),
  /// Baseline Context ID is 7162 (Surface Processing Algorithm Families)
  AlgorithmIdentificationMacro m_SegmentSurfaceGenerationAlgorithmIdentification;

  /// Recommended Display Grayscale Value (US, 1, 3)
  DcmUnsignedShort m_RecommendedDisplayGrayscaleValue;

  /// Recommended Display CIELab Value (US, 3, 3)
  DcmUnsignedShort m_RecommendedDisplayCIELabValue;

  /// Tracking ID (UT, 1, 1C)
  DcmUnlimitedText m_TrackingID;

  /// Tracking UID (UI, 1, 1C)
  DcmUniqueIdentifier m_TrackingUID;

  /// Rules for data elements within this IOD
  IODRules m_Rules;
};

#endif // SEGMENT_H
