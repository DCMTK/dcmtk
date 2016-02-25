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
 *  Purpose: Class for managing Plane Position (Patient) Functional Group
 *
 */

#ifndef FGPLANPO_H
#define FGPLANPO_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrds.h"

/** Class representing the Plane Position (Patient) Functional Group containing
 *  the x, y, and z coordinates of the upper left hand corner (center of the
 *  first voxel transmitted) of the frame.
 */
class DCMTK_DCMFG_EXPORT FGPlanePosPatient : public FGBase
{
public:

  /** Constructor, creates empty functional group
   */
  FGPlanePosPatient();

  /** Destructor, frees memory
   */
  virtual ~FGPlanePosPatient();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Convenience function to create minimal functional group
   *  @param  imagePositionPatientX The X coordinate of the upper left hand
   *          corner of the the frame in mm.
   *  @param  imagePositionPatientY The Y coordinate of the upper left hand
   *          corner of the the frame in mm.
   *  @param  imagePositionPatientZ The Z coordinate of the upper left hand
   *          corner of the the frame in mm.
   */
  static FGPlanePosPatient* createMinimal(const OFString& imagePositionPatientX,
                                          const OFString& imagePositionPatientY,
                                          const OFString& imagePositionPatientZ);

  /** Returns that this functional group can be shared or per-frame (both)
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clears all data
   */
  virtual void clearData();

  /** Check whether data in functional group is valid
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from item, must contain the Plane Position Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Write functional group to item, will write the Plane Position Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

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

  // --- get() functionality ---

  /** Get Image Position Patient
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImagePositionPatient(OFString& value,
                                              const signed long pos);

  /** Retrieve all values of Image Orientation Patient at the same time
   *  @param  coordinateX X coordinate of upper left hand corner voxel in mm
   *  @param  coordinateY Y coordinate of upper left hand corner voxel in mm
   *  @param  coordinateZ Z coordinate of upper left hand corner voxel in mm
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImagePositionPatient(Float64& coordinateX,
                                              Float64& coordinateY,
                                              Float64& coordinateZ);

  // --- set() functionality ---

  /** Set Image Position Patient. The x, y, and z coordinates of the upper left
   *  hand corner (center of the first voxel transmitted) of the frame, in mm.
   *  @param  coordinateX X coordinate of upper left hand corner voxel in mm
   *  @param  coordinateY Y coordinate of upper left hand corner voxel in mm
   *  @param  coordinateZ Z coordinate of upper left hand corner voxel in mm
   *  @param  checkValue Check coordinates for validity, including VR (DS)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setImagePositionPatient(const OFString& coordinateX,
                                              const OFString& coordinateY,
                                              const OFString& coordinateZ,
                                              const OFBool checkValue = OFTrue);

private:

  /* Content of Plane Position Patient Macro */

  /// Image Position (Patient)   (DS, VM 3, Required type 1C)
  DcmDecimalString m_ImagePositionPatient;
};

#endif // FGPLANPO_H
