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
 *  Purpose: Class managing various segmentation-typical types
 *
 */

#ifndef SEGTYPES_H
#define SEGTYPES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmseg/segdef.h"

/*
 * *  Logging
 */

extern OFString c;
extern DCMTK_DCMSEG_EXPORT OFLogger DCM_dcmsegLogger;

#define DCMSEG_TRACE(msg) OFLOG_TRACE(DCM_dcmsegLogger, msg)
#define DCMSEG_DEBUG(msg) OFLOG_DEBUG(DCM_dcmsegLogger, msg)
#define DCMSEG_INFO(msg)  OFLOG_INFO(DCM_dcmsegLogger, msg)
#define DCMSEG_WARN(msg)  OFLOG_WARN(DCM_dcmsegLogger, msg)
#define DCMSEG_ERROR(msg) OFLOG_ERROR(DCM_dcmsegLogger, msg)
#define DCMSEG_FATAL(msg) OFLOG_FATAL(DCM_dcmsegLogger, msg)

/*-----------------------*
 *  constant definitions  *
 *-----------------------*/

/** DICOM assigns unique number to each Segment within the Segmentation object
 *  using an unsigned 16 Bit Integer within "Segment Number". Thus a maximum number
 *  of 65536 segments can be stored.
 */
const Uint32 DCM_SEG_MAX_SEGMENTS = 65536;

/** @name Specific error conditions for module dcmseg
 *  These error codes can be used in addition to the general purpose
 *  codes defined in module dcmdata.
 */
//@{

/// error: specified functional group is already existing
extern DCMTK_DCMSEG_EXPORT   const OFConditionConst     SG_EC_MaxSegmentsReached;
/// error: specified segment does not exist
extern DCMTK_DCMSEG_EXPORT   const OFConditionConst     SG_EC_NoSuchSegment;
/// error: unknown segmentation type
extern DCMTK_DCMSEG_EXPORT   const OFConditionConst     SG_EC_UnknownSegmentationType;
/// error: invalid value
extern DCMTK_DCMSEG_EXPORT   const OFConditionConst     SG_EC_InvalidValue;
/// error: not enough data
extern DCMTK_DCMSEG_EXPORT   const OFConditionConst     SG_EC_NotEnoughData;

/** General purpose class hiding global functions, constants and types in the
 *  segmentation context from the global namespace.
 */
class DCMTK_DCMSEG_EXPORT DcmSegTypes
{

  public:

   /** Segmentation object types
     */
    enum E_SegmentationType
    {
      /// Unknown (e.g.\ not initialized)
      ST_UNKNOWN,
      /// Binary segmentation with 1 bit depth denoting whether a pixel
      /// belongs to a segmentation or not.
      ST_BINARY,
      /// Fractional segmentation where fraction specifies how much of voxel
      /// is occupied by the segment
      ST_FRACTIONAL
    };

    /** Segment Algorithm Type
     */
    enum E_SegmentAlgoType
    {
      /// Unknown (e.g.\ not initialized)
      SAT_UNKNOWN,
      /// Calculated segment
      SAT_AUTOMATIC,
      /// Calculated segment with user interaction
      SAT_SEMIAUTOMATIC,
      /// Manual segment creation
      SAT_MANUAL
    };


    /** Segmentation object Fractional Type
     */
    enum E_SegmentationFractionalType
    {
      /// Unknown (e.g.\ not initialized)
      SFT_UNKNOWN,
      /// For fractional segmentations, a value defines probability that pixel belongs to segment
      SFT_PROBABILITY,
      /// For fractional segmentations, a value defines how much of the pixel is covered by the segment
      SFT_OCCUPANCY
    };


    // -- helper functions --

    /** Return string representation of algorithm type
     *  @param  algo The algorithm type
     *  @return The algorithm type as a string
     */
    static OFString algoType2OFString(E_SegmentAlgoType algo );

    /** Return enum representation of algorithm type string as found in
     *  segmentation objects
     *  @param  algoType The algorithm type as a string
     *  @return The algorithm type as enum value
     */
    static E_SegmentAlgoType OFString2AlgoType(const OFString& algoType);

    /** Return enum representation of segmentation type string as found in
     *  segmentation objects
     *  @param  value The segmentation type as a string
     *  @return The segmentation type as enum value
     */
    static DcmSegTypes::E_SegmentationType OFString2Segtype( const OFString& value );

    /** Return string representation from segmentation enum type
     *  @param  value The segmentation type as enum value
     *  @return The segmentation type as a string
     */
    static OFString segtype2OFString( const DcmSegTypes::E_SegmentationType& value );

    /** Return enum representation of fractional type string as found in
     *  segmentation objects
     *  @param  value The fractional type as a string
     *  @return The fractional type as enum value
     */
    static DcmSegTypes::E_SegmentationFractionalType OFString2FractionalType( const OFString& value );
};

/** Class representing the Segmented Property Type Code and Segmented
 *  Property Type Code Modifier Code Sequence
 */
class DCMTK_DCMSEG_EXPORT SegmentedPropertyTypeCodeItem
{

public:

  // Easy access to containing class
  friend class SegmentDescriptionMacro;

  /** Constructor
   */
  SegmentedPropertyTypeCodeItem();

  /** Virtual destructor, frees memory
   */
  virtual ~SegmentedPropertyTypeCodeItem();

  /** Clear all data
   */
  virtual void clearData();

  /** Check whether class has valid data
   *  @param  quiet If OFTrue, check will not produce any warnings or errors
   *          on the logger. Default is OFFalse.
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Read data from given item
   *  @param  item The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared before reading
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item,
                           const OFBool clearOldData = OFTrue);

  /** Write data to given item
   *  @param item  The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

protected:

  /** Check whether the modifier codes are ok
   *  @param  quiet If OFTrue, no warnings or errors will be printed to the
   *          loggers. Default is OFFalse.
   *  @return EC_Normal if data is ok, error otherwise
   */
  OFCondition checkModifiers(const OFBool quiet = OFFalse);

private:

  /// Segmented Property Type Code (SQ,1,1)
  /// Baseline CID 7151
  CodeSequenceMacro m_SegmentedPropertyTypeCode;

   /// Segmented Property Type Modifier Code (SQ, VM 1-n, Type 3)
  OFVector<CodeSequenceMacro*> m_SegmentedPropertyTypeModifierCode;
};

/** Class representing the Segment Description Macro
 */
class DCMTK_DCMSEG_EXPORT SegmentDescriptionMacro
{

public:

  /** Constructor
   */
  SegmentDescriptionMacro();

  /** Virtual destructor, frees memory
   */
  virtual ~SegmentDescriptionMacro();

  /** Clear all data
   */
  virtual void clearData();

  /** Read class data from given item
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Writes the data from this class to given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

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

  /** Get Segment Algorithm Type
   *  @return The algorithm type
   */
  virtual DcmSegTypes::E_SegmentAlgoType getSegmentAlgorithmType();

  /** Get reference to General Anatomy Code
   *  @return Reference to General Anatomy Code
   */
  virtual GeneralAnatomyMacro& getGeneralAnatomyCode();

  /** Get Reference to Segmented Property Category Code
   *  @return Reference to Segmented Property Category Code
   */
  virtual CodeSequenceMacro& getSegmentedPropertyCategoryCode();

  /** Get Reference to Segmented Property Type Code
   *  @return Reference to Segmented Property Type Code
   */
  virtual CodeSequenceMacro& getSegmentedPropertyTypeCode();

  /** Get Reference to Segmented Property Type Modifier Codes
   *  @return Reference to Segmented Property Type Modifier Codes
   */
  virtual OFVector<CodeSequenceMacro*>& getSegmentedPropertyTypeModifier();

  /** Set Segment Label
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSegmentLabel(const OFString& value,
                                      const OFBool checkValue = OFTrue);

  /** Set Segment Description
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSegmentDescription(const OFString& value,
                                            const OFBool checkValue = OFTrue);

  /** Set Segment Algorithm Type
   *  @param  value Value to be set
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSegmentAlgorithmType(const DcmSegTypes::E_SegmentAlgoType value);

protected:

  /** Read Segment Algorithm Type from given item
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition readSegmentAlgorithmType(DcmItem& item);

  /** Write Segment Algorithm Type to given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition writeSegmentAlgorithmType(DcmItem& item);


private:

  // Segment Number: (US, VM 1, Type 1): Computed by position in vector of DcmSegmentation
  // DcmUnsignedShort m_SegmentNumber;
  /// Segment Number: (LO, VM 1, Type 1)
  DcmLongString m_SegmentLabel;

  /// Segment Description: (ST, 1, Type 3)
  DcmShortText m_SegmentDescription;

  /// Segment Algorithm Type: (CS, 1, Type 1)
  DcmSegTypes::E_SegmentAlgoType m_SegmentAlgorithmType;

  /// General Anatomy Mandatory Macro
  GeneralAnatomyMacro m_GeneralAnatomyCode;

  /// Segment Property Category Code (SQ, 1, 1)
  /// Baseline CID 7150
  CodeSequenceMacro m_SegmentedPropertyCategoryCode;

  /// Segmented Property Type Code
  SegmentedPropertyTypeCodeItem m_SegmentedPropertyType;

};


#endif // SEGTYPES_H
