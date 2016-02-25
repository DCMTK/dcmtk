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
 *  Purpose: Main interface class for managing Functional Groups
 *
 */

#ifndef FGINTERFACE_H
#define FGINTERFACE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmfg/fgdefine.h"
#include "dcmtk/dcmfg/fg.h"

// Forward declaration
class IODMultiframeDimensionModule;

/** Main interface class to access functional groups from DICOM Enhanced
 *  objects. Allows reading, modifying and writing functional groups back
 *  and forth from DICOM datasets.
 */
class DCMTK_DCMFG_EXPORT FGInterface
{

public:

  /// Type representing per-frame functional groups, i.e.\ a number of
  /// functional groups assigned to each frame
  typedef OFMap<Uint32, FunctionalGroups*> PerFrameGroups;

  /// Iterator type for iterating over functional groups
  typedef FunctionalGroups::iterator iterator;

  /// Const iterator type for iterating over functional groups
  typedef FunctionalGroups::const_iterator const_iterator;

  /** Constructor, constructs empty sets of per-frame and shared
   *  functional groups
   */
  FGInterface();

  /** Virtual destructor, frees memory
   */
  virtual ~FGInterface();

  /** Delete all functional groups (shared and per-frame)
   */
  virtual void clear();

  /** Checks the functional groups for consistency
   *  @return OFTrue, if check consistency is ok, error otherwise
   */
  virtual OFBool check();

  /** Returns number of frames. Computed by number of per-frame
   *  functional group items (i.e.\ the Number of Frames attribute
   *  is not taken into account).
   *  @return Number of frames
   */
  virtual size_t getNumberOfFrames();

  /** Read enhanced multi-frame information from DICOM item, usually
   *  DcmDataset, i.e.\ must contain Shared and Per-frame Functional Group
   *  Sequences
   *  @param  dataset The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& dataset);

  /** Write enhanced multi-frame information to DICOM item,
   *  usually DcmDataset, i.e.\ writes  Shared and Per-frame Functional Group
   *  Sequences
   *  @param  dataset The item to write to
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& dataset);

  /** Get specific functional group for a frame, no matter whether it is stored
   *  per frame or shared
   *  @param  frameNo The frame number the functional group should apply to
   *          (starts with 0)
   *  @param  fgType The type of functional group to look for
   *  @return The functional group if found, NULL otherwise
   */
  virtual FGBase* get(const Uint32 frameNo,
                      const DcmFGTypes::E_FGType fgType);

  // TODO Add get(..) version that takes the sequence tag (e.g.\ for unknown
  // functional groups

  /** Get specific functional group for a frame, no matter whether it is stored
   *  per frame or shared.
   *  @param  frameNo The frame number of group of interest (starts from 0)
   *  @param  fgType The type of functional group to look for
   *  @param  isPerFrame If OFTrue, the group found was found as per-frame,
   *          otherwise it is a shared functional group
   *  @return The functional group if found, error otherwise
   */
  virtual FGBase* get(const Uint32 frameNo,
                      const DcmFGTypes::E_FGType fgType,
                      OFBool& isPerFrame);

  /** Return all per-frame functional groups, e.g.\ to iterate over them
   *  @param  frameNo The frame number of the groups of interest (starts from 0)
   *  @return The per-frame functional groups for the given frame
   */
  const FunctionalGroups* getPerFrame(const Uint32 frameNo) const;

  /** Return all shared functional groups, e.g.\ to iterate over them
   *  @return The shared functional groups
   */
  const FunctionalGroups* getShared() const;

  /** Add functional group that should be shared for all frames. This will
   *  delete all per-frame groups of the same type if existing.
   *  @param  group   The group to be added (copy is performed)
   *  @return EC_Normal, if adding was successful, error otherwise
   */
  virtual OFCondition addShared(const FGBase& group);

  /** Add functional group for given frame. If there is already a shared
   *  functional group with identical values, the call returns without
   *  errors, too. If there is a shared group that differs, the shared group
   *  is converted to be "per-frame" for all frames and then the given group
   *  is inserted for the frame specified by the user.
   *  If a per-frame functional group of the same type already exists it is
   *  overwritten.
   *  @param  frameNo The frame number this group should be added for (starts
   *          from 0)
   *  @param  group The group to be added
   *  @return EC_Normal, if adding was successful, error otherwise
   */
  virtual OFCondition addPerFrame(const Uint32 frameNo,
                                  const FGBase& group);

  /** Deletes a shared functional group of the given type
   *  @param fgType The type of functional group to delete
   *  @return OFTrue if group existed and could be deleted, OFFalse (group did
   *          not exist) otherwise
   */
  virtual OFBool deleteShared(const DcmFGTypes::E_FGType fgType);

  /** Deletes per-frame functional group of the given type for a specific frame
   *  @param frameNo The frame number for the functional group of interest
   *  @param fgType The type of functional group to delete
   *  @return OFTrue if group existed and could be deleted, OFFalse (group did
   *          not exist) otherwise
   */
  virtual OFBool deletePerFrame(const Uint32 frameNo,
                                const DcmFGTypes::E_FGType fgType);

  /** Deletes per-frame functional group for all frames
   *  @param  fgType The type of functional group to delete
   *  @return Number of per-frame groups deleted (usually equal to number of
   *          images)
   */
  size_t deletePerFrame(const DcmFGTypes::E_FGType fgType);

protected:

  /** Get shared functional group based on its type
   *  @param  fgType The type of functional group
   *  @return The functional group or NULL if not existent
   */
  virtual FGBase* getShared(const DcmFGTypes::E_FGType fgType);

  /** Insert shared functional group
   *  @param  group The functional group to be inserted
   *  @param  replaceExisting If OFTrue, an existing shared functional group
   *          will be deleted, otherwise the old group is not overwritten
   *  @return EC_Normal if insertion worked, FG_EC_DoubledFG if group exists and
   *          is not overwritten, other error code for other cases
   */
  virtual OFCondition insertShared(FGBase* group,
                                   const OFBool replaceExisting = OFTrue);

  /** Get per-frame functional group
   *  @param  frameNo  The frame number of the group
   *  @param  fgType The type of the group
   *  @return The functional group or NULL if not existent
   */
  virtual FGBase* getPerFrame(const Uint32 frameNo,
                              const DcmFGTypes::E_FGType fgType);

  /** Insert per-frame functional group
   *  @param  frameNo The frame number the group should be added for
   *  @param  group The functional group to be inserted
   *  @param  replaceExisting If OFTrue, an existing per-frame functional group
   *          will be deleted, otherwise the old group is not overwritten
   *  @return EC_Normal if insertion worked, FG_EC_DoubledFG if group exists and
   *          is not overwritten, other error code for other cases
   */
  virtual OFCondition insertPerFrame(const Uint32 frameNo,
                                     FGBase* group,
                                     const OFBool replaceExisting = OFTrue);

  /** Get existing per-frame group or create it for the given frame. Note that
   *  the per-frame groups do not have to be created "in order", i.e.\ one could
   *  add groups in order 3,5,1 ,... .
   *  @param  frameNo The frame number to get/create per-frame groups for
   *  @return The functional groups if found/created, NULL in case of error
   */
  virtual FunctionalGroups* getOrCreatePerFrameGroups(const Uint32 frameNo);

  /** Read Shared Functional Group Sequence from given item
   *  @param  dataset The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition readSharedFG(DcmItem& dataset);

  /** Read Per-Frame Functional Group Sequence from given item
   *  @param  dataset The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition readPerFrameFG(DcmItem& dataset);

  /** Read single functional group into the item provided
   *  @param  fgItem The item to read from
   *  @param  groups The resulting group after reading
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition readSingleFG(DcmItem& fgItem,
                                   FunctionalGroups& groups);

  /** Write Shared Functional Group Sequence to given item
   *  @param  dataset The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition writeSharedFG(DcmItem& dataset);

  /** Write Per-Frame Functional Group Sequence to given item
   *  @param  dataset The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition writePerFrameFG(DcmItem& dataset);

  /** Convert a shared functional group to a per-frame one by copying the
   *  shared one into a per-frame one for each frame and deleting the shared one
   *  aftewrards.
   *  @param  fgType The type of functional group to convert
   *  @return EC_Normal if conversion worked out, FG_EC_NoSuchGroup if such a
   *          group does not exist and other error otherwise. In the last case
   *          the functional groups may be left in invalid state, but that
   *          should only happen for fatal errors like exhausted memory.
   */
  virtual OFCondition convertSharedToPerFrame(const DcmFGTypes::E_FGType fgType);

private:

  /// Shared functional groups
  FunctionalGroups m_shared;

  /// Link from frame number (map key) to the list of functional groups (value)
  /// relevant for the frame
  PerFrameGroups m_perFrame;
};

#endif // MODMULTIFRAMEFGH_H
