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
 *  Purpose: Class for managing Plane Orientation (Patient) Functional Group
 *
 */

#ifndef FGPLANOR_H
#define FGPLANOR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrds.h"

/** Class representing the Plane Orientation (Patient) Functional Group Macro.
 *  It specifies the direction cosines of the first row and the first column
 *  with respect to the patient.
 */
class DCMTK_DCMFG_EXPORT FGPlaneOrientationPatient : public FGBase
{
public:

  /** Constructor, creates empty functional group
   */
  FGPlaneOrientationPatient();

  /** Destructor, frees memory
   */
  virtual ~FGPlaneOrientationPatient();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Convenience function to create a minimal functional group.
   *  @param  imageOrientationPatientRowX Row X Value
   *  @param  imageOrientationPatientRowY Row Y Value
   *  @param  imageOrientationPatientRowZ Row Z Value
   *  @param  imageOrientationPatientColX Column X Value
   *  @param  imageOrientationPatientColY Column Y Value
   *  @param  imageOrientationPatientColZ Column Z Value
   *  @return The functional group created with above parameters
   */
  static FGPlaneOrientationPatient* createMinimal(const OFString& imageOrientationPatientRowX,
                                                  const OFString& imageOrientationPatientRowY,
                                                  const OFString& imageOrientationPatientRowZ,
                                                  const OFString& imageOrientationPatientColX,
                                                  const OFString& imageOrientationPatientColY,
                                                  const OFString& imageOrientationPatientColZ);

  /** Get shared type of this functional group (can be both, shared and
   *  per-frame)
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clears all data
   */
  virtual void clearData();

  /** Check whether this group contains valid data
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from item, must contain the Plane Orientation
   *  Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Writes functional group to given item (will write Plane Orientation
   *  Sequence to it)
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

  /** Get Image Orientation Patient
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageOrientationPatient(OFString& value,
                                                 const signed long pos);

  /** Retrieve all values of Image Orientation Patient at the same time
   *  @param  rowX Row X Value
   *  @param  rowY Row Y Value
   *  @param  rowZ Row Z Value
   *  @param  colX Column X Value
   *  @param  colY Column Y Value
   *  @param  colZ Column Z Value
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getImageOrientationPatient(Float64& rowX,
                                                 Float64& rowY,
                                                 Float64& rowZ,
                                                 Float64& colX,
                                                 Float64& colY,
                                                 Float64& colZ);

  // --- set() functionality ---

   /** Set all values of Image Orientation Patient at once
    *  @param  rowX Row X Value
    *  @param  rowY Row Y Value
    *  @param  rowZ Row Z Value
    *  @param  colX Column X Value
    *  @param  colY Column Y Value
    *  @param  colZ Column Z Value
    *  @param  checkValue If OFTrue, values are checked for validity
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition setImageOrientationPatient(const OFString& rowX,
                                                 const OFString& rowY,
                                                 const OFString& rowZ,
                                                 const OFString& colX,
                                                 const OFString& colY,
                                                 const OFString& colZ,
                                                 const OFBool checkValue = OFTrue);

private:

  /* Content of Plane Orientation (Patient) Macro */

  /// Image Orientation (Patient)   (DS, VM 6, Required type 1C)
  DcmDecimalString m_ImageOrientationPatient;
};

#endif // FGPLANOR_H
