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
 *  Purpose: Base class(es) for functional groups
 *
 */

#ifndef FGBASE_H
#define FGBASE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgtypes.h"

/** Abstract base class for deriving specific functional groups
 */
class DCMTK_DCMFG_EXPORT FGBase
{

public:

  /** Constructor, creates new functional group of given type.
   *  @param  fgType The type of functional group to create
   */
  FGBase(const DcmFGTypes::E_FGType fgType);

  /** Check whether functional group has valid and complete data
   *  @return EC_Normal, if check is ok, error otherwise
   */
  virtual OFCondition check() const = 0;

  /** Read functional group from given item. Old data is overwritten.
   *  @param  item The item to read from. This must contain the sequence
   *          element that uniquely identifies the functional group.
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item) = 0;

  /** Write functional group to given item. If the functional group already
   *  exists it is overwritten.
   *  @param  item The item to write to. The method will write the sequence
   *          specific for the functional group into the item
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item) = 0;

  /** Find out whether functional group is potentially only shared, only
   *  per-frame or can be both
   *  @return The functional group "shared type"
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const =0;

  /** Return the type of this functional group
   *  @return The functional group's type
   */
  virtual DcmFGTypes::E_FGType getType() const;

  /** Clear any data in the group
   */
  virtual void clearData() =0;

  /** Virtual destructor
   */
  virtual ~FGBase();

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type, i.e.\ the elements within both
   *  functional groups (this and rhs parameter) are compared by value!
   *  Both objects (this and rhs) need to have the same type (e.g.\ both
   *  FGDerivationImage) to be comparable. This function is used in order
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
   *          but the rhs component is shorter.  Also returned if this type and
   *          rhs type (DcmFGTypes::E_FGType) do not match.
   *          1 if either the value of the first component that does not match
   *          is greater in the rhs object, or all compared components match
   *          but the rhs component is longer.
   */
  virtual int compare(const FGBase& rhs) const = 0;

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const = 0;

protected:

  /** Get the item containing the payload of a functional group sequence,
   *  identified by the functional group's sequence key provided and the desired
   *  item number.
   *  @param  source The item to read the sequence from, e.g.\ item of the
   *          Shared Functional Group Sequence
   *  @param  seqKey The identifying key of the functional group's sequence
   *  @param  itemNum The item number to get from that sequence (usually,
   *          a functional group has exactly a single item, i.e.\ the parameter
   *          would be set to 0
   *  @param  result The item if found, otherwise NULL
   *  @return EC_Normal, if specified item could be retrieved, error otherwise
   */
  virtual OFCondition getItemFromFGSequence(DcmItem& source,
                                            const DcmTagKey& seqKey,
                                            const unsigned long itemNum,
                                            DcmItem*& result);

  /** Create functional group sequence specified by given sequence tag key
   *  @param  destination The item to put the sequence into
   *  @param  seqKey The functional group's tag key
   *  @param  numItems The number of items to create within sequence (minus 1).
   *          Usually, functional group only contain a single item,
   *          i.e.\ numItems would be set to 0
   *  @param  firstItem Reference to the first item the method created
   *  @return EC_Normal if creation was successful, error otherwise
   */
  virtual OFCondition createNewFGSequence(DcmItem& destination,
                                          const DcmTagKey& seqKey,
                                          const unsigned long numItems,
                                          DcmItem*& firstItem);

private:

  /// Private default constructor, shall not be used
  FGBase();

  /// The type of the functional group
  DcmFGTypes::E_FGType m_fgType;

};


/** Class representing an "unknown" functional group, e.g.\ a private one
 *  specified by a vendor or one that is not explicitly known yet to the
 *  dcmfg library.
 *  The main purpose is to have a container for such functional groups that
 *  allows to read them into memory, access them using a low level API, and
 *  to be able to write store them back.
 */
class DCMTK_DCMFG_EXPORT FGUnknown : public FGBase
{

public:

  /** Creates unknown (to the dcmfg class library) functional group
   *  @param seqStartTag The tag that uniquely identifies this functional group
   *  @param sharedType Defines whether this group is potentially per-frame,
   *         shared or can be both. Default is "unknown".
   */
  FGUnknown(const DcmTagKey& seqStartTag,
            const DcmFGTypes::E_FGSharedType sharedType = DcmFGTypes::EFGS_UNKNOWN);

  /** Returns type of this functional group (always "EFG_UNKNOWN")
   *  @return Always returns DcmFGTypes::EFG_UNKNOWN
   */
  virtual DcmFGTypes::E_FGType getType() const {return DcmFGTypes::EFG_UNKNOWN;}

  /** Returns whether this group is potentially per-frame, shared or can be both
   *  @return The functional group's "shared type"
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return m_sharedType;}

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Check whether this functional group contains valid data
   *  @returns EC_Normal if functional group is valid. For now, always returns
   *           EC_Normal
   */
  virtual OFCondition check() const;

  /** Clear data within this group
   */
  virtual void clearData();

  /** Read this group into memory
   *  @param  item The item to read from
   */
  virtual OFCondition read(DcmItem& item);

  /** Write this group to given item
   *  @param  item The item to write to
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

  /** Virtual destructor, frees memory
   */
  virtual ~FGUnknown();

private:

  /** Private default constructor, shall not be used
   */
  FGUnknown();

  /// The tag that uniquely identifies this functional group
  DcmTagKey m_seqStartTag;

  /// The data hold by this item, i.e.\ the sequence making up the functional
  /// group
  DcmSequenceOfItems* m_fgSequence;

  /// Denotes whether this group is potentially per-frame, shared or can be both
  DcmFGTypes::E_FGSharedType m_sharedType;

};

#endif // FGBASE_H

