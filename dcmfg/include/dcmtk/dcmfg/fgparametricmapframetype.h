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
 *  Purpose: Class for managing the Parametric Map Frame Type Functional Group
 *
 */

#ifndef FGPARAMETRICMAPFRAMETYPE_H
#define FGPARAMETRICMAPFRAMETYPE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmdata/dcvrcs.h"

/** Class representing the Parametric Map Frame Type Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGParametricMapFrameType : public FGBase
{
public:
  /** Constructor, creates empty Parametric Map Frame Type Functional Group
   */
  FGParametricMapFrameType();

  /** Destructor, frees memory
   */
  virtual ~FGParametricMapFrameType();

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

  /** Read functional group from given item, i.e.\ read Parametric Map Frame Type Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Write functional group to given item, i.e.\ write Parametric Map Frame Type Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  /** Get FrameType
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getFrameType(OFString &value,
                                   const signed long pos = 0) const;

  /** Set FrameType
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (4) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setFrameType(const OFString &value,
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

  /* Content of Parametric Map Frame Type Functional Group Macro */

  /// FrameType (CS, VM 4, Required type 1)
  DcmCodeString m_FrameType;
};

#endif // FGPARAMETRICMAPFRAMETYPE_H
