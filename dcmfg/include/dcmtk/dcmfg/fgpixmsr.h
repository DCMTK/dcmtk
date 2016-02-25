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
 *  Purpose: Class for managing Pixel Measures Functional Group
 *
 */

#ifndef FGPIXMSR_H
#define FGPIXMSR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing the Pixel Measures Functional Group Macro
 */
class DCMTK_DCMFG_EXPORT FGPixelMeasures : public FGBase
{
public:

  /** Constructor, creates empty Pixel Measures Functional Group
   */
  FGPixelMeasures();

  /** Virtual destructor, frees memory
   */
  virtual ~FGPixelMeasures();

  /** Returns a deep copy of this object
   *  @return Deep copy of this object
   */
  virtual FGBase *clone() const;


  /** Returns shared type of this group (can be shared or per-frame)
   *  @return Always returns DcmFGTypes::EFGS_BOTH;
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clear all data
   */
  virtual void clearData();

  /** Check whether data in functional group is valid
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from given item
   *  @param  item The item to read from, must contain Pixel Measures Sequence
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Write functional group to given item
   *  @param  item The item to write Pixel Measures Sequence to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  // --- get() functionality ---

  /** Get Pixel Spacing
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPixelSpacing(Float64& value,
                                      const signed long pos = 0);

  /** Get Slice Thickness
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSliceThickness(Float64& value,
                                        const signed long pos = 0);

  /** Get Spacing Between Slices
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSpacingBetweenSlices(Float64& value,
                                              const signed long pos = 0);

  // --- set() functionality ---

  /** Set Pixel Spacing
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (2) if
   *          enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPixelSpacing(const OFString& value,
                                      const OFBool checkValue = OFTrue);

  /** Set Slice Thickness
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if
   *          enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSliceThickness(const OFString& value,
                                        const OFBool checkValue = OFTrue);

  /** Set Spacing between Slices
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if
   *          enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSpacingBetweenSlices(const OFString& value,
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

  /* Content of Pixel Measures Macro */

  /// Pixel Spacing   (DS, VM 2, Required type 1C)
  DcmDecimalString m_PixelSpacing;

  /// Slice Thickness (DS, VM 1, Required type 1C)
  DcmDecimalString m_SliceThickness;

  /// Spacing Between Slices (DS, VM 1, Required type 3)
  DcmDecimalString m_SpacingBetweenSlices;
};

#endif // FGPIXMSRS_H
