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
 *  Purpose: Class for managing Plane Orientation (Volume) Functional Group
 *
 */

#ifndef FGPLANORVOL_H
#define FGPLANORVOL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing the Plane Orientation (Volume) Functional Group Macro.
 *  It specifies the direction cosines of the first row and the first column of
 *  the frame with respect to the Volume Frame of Reference.
 */
class DCMTK_DCMFG_EXPORT FGPlaneOrientationVolume : public FGBase
{
public:

  /** Constructor, creates empty Plane Orientation (Volume) Functional Group
   */
  FGPlaneOrientationVolume();

  /** Destructor, frees memory
   */
  virtual ~FGPlaneOrientationVolume();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Get shared type of this functional group (can be both, per-frame and
   *  shared)
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clears all data
   */
  virtual void clearData();

  /** Check whether functional group contains valid data
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from given item, i.e.\ read Plane Orientation
   *  (Volume) Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Write functional group to given item, i.e.\ write Plane Orientation
   *  (Volume) Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  // --- get() functionality ---

  /** Get Image Orientation Volume
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageOrientationVolume(Float64& value,
                                                const unsigned long pos = 0);

  /** Retrieve all values of Image Orientation Volume at the same time
   *  @param  rowX Row X Value
   *  @param  rowY Row Y Value
   *  @param  rowZ Row Z Value
   *  @param  colX Column X Value
   *  @param  colY Column Y Value
   *  @param  colZ Column Z Value
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageOrientationVolume(Float64& rowX,
                                                Float64& rowY,
                                                Float64& rowZ,
                                                Float64& colX,
                                                Float64& colY,
                                                Float64& colZ);

  // --- set() functionality ---

   /** Set all values of Image Orientation Volume at once
    *  @param  rowX Row X Value
    *  @param  rowY Row Y Value
    *  @param  rowZ Row Z Value
    *  @param  colX Column X Value
    *  @param  colY Column Y Value
    *  @param  colZ Column Z Value
    *  @param  checkValue If OFTrue, values are checked for validity (not
    *          implemented yet)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition setImageOrientationVolume(const Float64& rowX,
                                                const Float64& rowY,
                                                const Float64& rowZ,
                                                const Float64& colX,
                                                const Float64& colY,
                                                const Float64& colZ,
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

  /* Content of Plane Orientation (Volume) Macro */

  /// Image Orientation (Volume) (FD, VM 6, Required type 1)
  DcmFloatingPointDouble m_ImageOrientationVolume;
};

#endif // FGPLANORVOL_H
