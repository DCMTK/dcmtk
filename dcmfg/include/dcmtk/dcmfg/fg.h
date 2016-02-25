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
 *  Purpose: Class for managing Functional Groups from a DICOM object
 *
 */

#ifndef FG_H
#define FG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcsequen.h"


// forward declaration
class FGBase;

/** Class representing a list  of functional groups where each type of group
 *  can only be once in the list.
 */
class DCMTK_DCMFG_EXPORT FunctionalGroups
{

public:

  /// Iterator type for iterating functional groups
  typedef OFMap<DcmFGTypes::E_FGType, FGBase*>::iterator iterator;

  /// Const iterator type for iterating functional groups
  typedef OFMap<DcmFGTypes::E_FGType, FGBase*>::const_iterator const_iterator;

  /** Constructor, creates empty set of functional groups
   */
  FunctionalGroups();

  /** Virtual destructor, cleans up set of functional groups and
   *  frees the related memory
   */
  virtual ~FunctionalGroups();

  /** Cleans up set of functional groups and frees the related memory
   */
  virtual void clear();

  /** Find a functional group by its type
   *  @param  fgType The type of the functional group
   *  @return The functional group, if found, NULL otherwise
   */
  virtual FGBase* find(const DcmFGTypes::E_FGType fgType);

  /** Iterator pointing to first functional group in the set
   *  @return First functional group in set
   */
  virtual FunctionalGroups::iterator begin();

  /** Iterator pointing behind last functional group in the set
   *  @return Iterator pointing behind last functional group in the set
   */
  virtual FunctionalGroups::iterator end();

  /** Const iterator pointing to first functional group in the set
   *  @return First functional group in set
   */
  virtual FunctionalGroups::const_iterator begin() const;

  /** Const iterator pointing behind last functional group in the set
   *  @return Iterator pointing behind last functional group in the set
   */
  virtual FunctionalGroups::const_iterator end() const;

  /** Insert new functional group; ownership is taken over from caller if
   *  function call is successful.
   *  @param  group The group to insert
   *  @param  replaceOld If OFTrue, then the existing functional group of the
   *          same type (if existing) is deleted and replaced by the given one.
   *  @return EC_Normal, if insertion was successful, error otherwise. In the
   *          latter case, the caller keeps the ownership of the given object.
   */
  virtual OFCondition insert(FGBase* group,
                             const OFBool replaceOld);

  /** Remove functional group specified by its type. The memory is not
   *  freed by the call but must freed by the caller using the returned pointer.
   *  @param  fgType The type of the group to remove
   *  @return The functional group removed, or NULL, if not found
   */
  virtual FGBase* remove(const DcmFGTypes::E_FGType fgType);

private:

  /// Map holding the functional groups, one of each type at most
  OFMap<DcmFGTypes::E_FGType, FGBase*> m_groups;

};

#endif // FG_H
