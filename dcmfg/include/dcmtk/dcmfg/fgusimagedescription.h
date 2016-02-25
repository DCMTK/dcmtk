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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing US Image Description Functional Group
 *
 */

#ifndef FGUSIMAGEDESCRIPTION_H
#define FGUSIMAGEDESCRIPTION_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing the US Image Description Functional Group
 */
class DCMTK_DCMFG_EXPORT FGUSImageDescription: public FGBase
{
public:

  /** Constructor, creates empty functional group
   */
  FGUSImageDescription();

  /** Destructor, frees memory
   */
  virtual ~FGUSImageDescription();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Clears all data
   */
  virtual void clearData();

  /** Check whether data in this functional group is valid
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from item, must contain the US Image Description
   *  Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem &item);

  /** Write functional group to item, will write to US Image Description
   *  Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  /** Get shared type of this functional group (can be both, shared and
   *  per-frame)
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  // --- get() functionality ---

  /** Get Frame Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getFrameType(OFString& value,
                                   const signed long pos = 0);

  /** Get Volumetric Properties
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumetricProperties(OFString& value,
                                              const signed long pos = 0);

  /** Get Volume Based Calculation Technique
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getVolumeBasedCalculationTechnique(OFString& value,
                                                         const signed long pos = 0);

  // --- set() functionality ---

  /** Set Frame Type. Frame Type contains up to four values. Value 3 and 4 are
   *  optional and can be left empty if desired. The values in their order
   *  of occurrence are:
   *  1) Pixel Data Characteristics: Either ORIGINAL or DERIVED
   *  2) Patient Examination Characteristics: Fixed to "PRIMARY", thus cannot be
   *  influenced through this function.
   *  3) Image Flavor: Defined Terms listed in the standard
   *  4) Derived Pixel Contrast: Defined Terms listed in the standard
   *  @param  pixelDataChar Value 1 of Frame Type
   *  @param  imageFlavor Value 3 of Frame Type
   *  @param  derivedPixelContrast Value 4 of Frame Type
   *  @param  checkValue If OFTrue, the value is checked for conformance.
   *  @return EC_Normal if setting was successful, error otherwise.
   */
  virtual OFCondition setFrameType(const DcmFGTypes::E_FGUSFrameType pixelDataChar,
                                   const OFString& imageFlavor = "",
                                   const OFString& derivedPixelContrast = "",
                                   const OFBool checkValue = OFTrue);

  /** Set Volumetric Properties
   *  @param  value The value to set. Permitted values:
   *          "VOLUME", "SAMPLED", "DISTORTED", "MIXED"
   *  @param  checkValue If OFTrue, the value is checked for conformance
   *  @return EC_Normal if setting was successful, error otherwise.
   */
  virtual OFCondition setVolumetricProperties(const OFString& value,
                                              const OFBool checkValue = OFTrue);

  /** Set Volume Based Calculation Technique
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setVolumeBasedCalculationTechnique(const OFString& value,
                                                         const OFBool checkValue = OFTrue);

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type, i.e.\ the elements within both
   *  functional groups (this and rhs parameter) are compared by value!
   *  Both objects (this and rhs) need to have the same type (i.e.\ both
   *  FGUnknown) to be comparable. This function is used in order
   *  to decide whether a functional group already exists, or is new. This
   *  is used in particular to find out whether a given functional group
   *  can be shared (i.e.\ the same information already exists as shared
   *  functional group) or is different from the same shared group. In that
   *  case the shared functional group must be distributed into per-frame
   *  functional groups, instead. The exact implementation for implementing
   *  the comparison is not relevant. However, it must be a comparison
   *  by value.
   *  @param  rhs the right hand side of the comparison
   *  @return 0 if the object values are equal.
   *          -1 if either the value of the first component that does not match
   *          is lower in the this object, or all compared components match
   *          but this component is shorter. Also returned if this type and
   *          rhs type (DcmFGTypes::E_FGType) do not match.
   *          1 if either the value of the first component that does not match
   *          is greater in this object, or all compared components match
   *          but this component is longer.
   */
  virtual int compare(const FGBase& rhs) const;

private:

  /* Content of US Image Description Macro */

  /// Frame Type (CS, VM 4, Required type 1)
  DcmCodeString m_FrameType;

  /// Volumetric Properties (CS 1, VM 1, Required Type 1)
  DcmCodeString m_VolumetricProperties;

  /// Volume Based Calculation Technique (CS, VM 1, Required Type 1)
  DcmCodeString m_VolumeBasedCalculationTechnique;
};

#endif // FGUSIMAGEDESCRIPTION_H

