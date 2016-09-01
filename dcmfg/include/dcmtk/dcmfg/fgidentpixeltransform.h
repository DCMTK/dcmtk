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
 *  Module:  dcmfg
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Class for managing the Identity Pixel Value Transformation Functional Group
 *
 */

#ifndef FGIDENTPIXELTRANSFORM_H
#define FGIDENTPIXELTRANSFORM_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcvrlo.h"


/** Class representing the Identity Pixel Value Transformation Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGIdentityPixelValueTransformation : public FGBase
{
public:

  /** Constructor, creates Identity Pixel Value Transformation Functional Group.
   *  All values (Rescale Slope, Intercept and Type) are set to their only valid
   *  enumerated value listed in the standard, i.e. slope = 1, intercept = 0 and
   *  type = "US".
   */
  FGIdentityPixelValueTransformation();

  /** Destructor, frees memory
   */
  virtual ~FGIdentityPixelValueTransformation();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Get shared type of this functional group (can be both, per-frame and
   *  shared)
   *  @return Always returns EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clears all data
   */
  virtual void clearData();

  /** Check whether functional group contains valid data
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from given item, i.e.\ read Identity Pixel Value
   *  Transformation Sequence. Clears existing data before reading.
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Write functional group to given item, i.e.\ write Identity Pixel Value Transformation Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  /** Get RescaleIntercept
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRescaleIntercept(OFString &value,
                                          const signed long pos = 0) const;

  /** Get RescaleSlope
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRescaleSlope(OFString &value,
                                      const signed long pos = 0) const;

  /** Get RescaleType
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRescaleType(OFString &value,
                                     const signed long pos = 0) const;

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

protected:

  /** Set RescaleIntercept. Hidden from user since value must always be equal
   *  to 1 (automatically set on write).
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRescaleIntercept(const OFString &value,
                                          const OFBool checkValue = OFTrue);

  /** Set RescaleSlope. Hidden from user since value must always be equal
   *  to 1 (automatically set on write).
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRescaleSlope(const OFString &value,
                                      const OFBool checkValue = OFTrue);

  /** Set RescaleType. Hidden from user since value must always be equal
   *  to "US" for "unspecified" (automatically set on write).
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRescaleType(const OFString &value,
                                     const OFBool checkValue = OFTrue);

private:

  /* Content of Identity Pixel Value Transformation Functional Group Macro */

  /// RescaleIntercept (DS, VM 1, Required type 1)
  DcmDecimalString m_RescaleIntercept;

  /// RescaleSlope (DS, VM 1, Required type 1)
  DcmDecimalString m_RescaleSlope;

  /// RescaleType (LO, VM 1, Required type 1)
  DcmLongString m_RescaleType;
};

#endif // FGIDENTPIXELTRANSFORM_H
