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
 *  Purpose: Class for managing Plane Position (Volume) Functional Group
 *
 */

#ifndef FGPLANEPOSVOL_H
#define FGPLANEPOSVOL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing the Plane Position (Volume) Functional Group that
 *  identifies the position of the plane of this frame.
 */
class DCMTK_DCMFG_EXPORT FGPlanePositionVolume : public FGBase
{
public:

  /** Constructor, creates empty functional group
   */
  FGPlanePositionVolume();

  /** Virtual destructor, frees memory
   */
  virtual ~FGPlanePositionVolume();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Returns shared type of this functional group (can be shared or per-frame,
   *  i.e.\ both)
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clears all data
   */
  virtual void clearData();

  /** Check whether data of this functional group is valid
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from item, must contain the Plane Position (Volume)
   *  Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Writes functional group to item, i.e.\ writes the Plane Position (Volume)
   *  Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  // --- get() functionality ---

  /** Get Image Position Volume
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImagePositionVolume(Float64& value,
                                             const unsigned long pos = 0);

  /** Retrieve all values of Image Position Volume at the same time
   *  @param  valueX X coordinate of upper left hand corner voxel in mm
   *  @param  valueY Y coordinate of upper left hand corner voxel in mm
   *  @param  valueZ Z coordinate of upper left hand corner voxel in mm
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImagePositionVolume(Float64& valueX,
                                             Float64& valueY,
                                             Float64& valueZ);

  // --- set() functionality ---

  /** Set Image Position Volume, i.e.\ the x/y/z coordinates of the upper left
   *  hand corner (center of the first voxel transmitted) of the frame, in mm.
   *  @param  value X,Y or Z coordinate of upper left hand corner voxel in mm
   *  @param  pos pos 0 is x, pos 1 is Y, pos 2 addresses Z coordinate
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setImagePositionVolume(const Float64& value,
                                             const unsigned long pos = 0,
                                             const OFBool checkValue = OFTrue);

  /** Set Image Position Volume, i.e.\ the x/y/z coordinates of the upper left
   *  hand corner (center of the first voxel transmitted) of the frame, in mm.
   *  @param  valueX X coordinate of upper left hand corner voxel in mm
   *  @param  valueY coordinate of upper left hand corner voxel in mm
   *  @param  valueZ coordinate of upper left hand corner voxel in mm
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setImagePositionVolume(const Float64& valueX,
                                             const Float64& valueY,
                                             const Float64& valueZ,
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

  /* Content of Plane Position (Volume) Macro */

  /// Image Position (Volume) (FD, VM 3, Required type 1)
  DcmFloatingPointDouble m_ImagePositionVolume;
};

#endif // FGPLANEPOSVOL_H
