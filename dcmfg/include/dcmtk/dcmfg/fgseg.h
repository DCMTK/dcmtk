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
 *  Purpose: Class for managing Segmentation Functional Group
 *
 */

#ifndef FGSEG_H
#define FGSEG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing the Segmentation Functional Group Macro
 */
class DCMTK_DCMFG_EXPORT FGSegmentation : public FGBase
{

public:

  /** Constructor, creates empty functional group
   */
  FGSegmentation();

  /** Destructor, frees memory
   */
  virtual ~FGSegmentation();

  /** Returns a deep copy of this object
   *  @return Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Returns that this functional group can be used shared or per-frame,
   *  i.e.\ both
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clear all data
   */
  virtual void clearData();

  /** Check whether data in this functional group is valid
   *  @return EC_Normal if data is valid, error otherwise
   */
  OFCondition check() const;

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type, i.e.\ the elements within both
   *  functional groups (this and rhs parameter) are compared by value!
   *  Both objects (this and rhs) need to have the same type (i.e.\ both
   *  FGSegmentation) to be comparable. This function is used in order
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
   *          -1 if either the value of the  first component that does not match
   *          is lower in the rhs object, or all compared components match
   *          but the rhs component is shorter. Also returned if this type and
   *          rhs type (DcmFGTypes::E_FGType) do not match.
   *          1 if either the value of the first component that does not match
   *          is greater in the rhs object, or all compared components match
   *          but the rhs component is longer.
   */
  virtual int compare(const FGBase& rhs) const;

  /** Get Referenced Segment Number
   *  @param  value  Reference to variable in which the value should be stored
   *  @param  pos Index value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getReferencedSegmentNumber(Uint16 &value,
                                                 const unsigned long pos = 0);

  /** Set Referenced Segment Number
   *  @param  segmentNumber Value to be set
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setReferencedSegmentNumber(const Uint16 &segmentNumber);

  /** Read functional group from item, must contain the Segment Identification
   *  Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Write functional group to item, will write Segment Identification
   *  Sequence
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

private:

  /// Referenced Segment Number: (US, 1-n, 1)
  /// Represents single value within the single permitted item of the
  /// Segment Identification Sequence being the data structure the
  /// Segmentation FG is made of.
  DcmUnsignedShort m_ReferencedSegmentNumber;
};

#endif // FGSEG_H
