/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmRoleSelectionItem
 *    class DcmRoleSelectionMap
 *
 */

#ifndef DCCFRSMP_H
#define DCCFRSMP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"   /* for class OFList<> */
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/ofstd/ofmap.h"    /* for class OFMap */
#include "dcmtk/dcmnet/dccfuidh.h" /* for class DcmUIDHandler */
#include "dcmtk/dcmnet/assoc.h"    /* for T_ASC_SC_ROLE */

class DcmPresentationContextMap;

/** this helper class is a role selection list entry.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmRoleSelectionItem
{
public:
  /** constructor
   *  @param role supported SCP/SCU role
   *  @param abstractSyntax abstract syntax
   */
  DcmRoleSelectionItem(
    T_ASC_SC_ROLE role,
    const DcmUIDHandler& abstractSyntax );

  /// copy constructor
  DcmRoleSelectionItem(const DcmRoleSelectionItem& arg);
 
  /// destructor
  ~DcmRoleSelectionItem();

  /// assignment operator
  DcmRoleSelectionItem& operator=(const DcmRoleSelectionItem& arg);

  /** checks if the given argument matches the abstract syntax UID
   *  maintained by this object
   *  @param arg argument to compare to
   *  @return true if equal, false otherwise
   */
  OFBool matches(const DcmUIDHandler& arg) const
  {
    return arg == uid_;
  }

  /** returns a const reference to the abstract syntax UID
   *  maintained by this object
   *  @return reference to abstract syntax UID
   */   
  const DcmUIDHandler& getAbstractSyntax() const
  {
    return uid_;
  }

  /** returns the role maintained by this object
   */
  T_ASC_SC_ROLE getRole() const
  {
    return role_;
  } 

  /** comparison operator.
   *  @param arg object to compare with
   *  @return true if equal
   */
  OFBool operator==(const DcmRoleSelectionItem& arg) const
  {
    return (role_ == arg.role_) && (uid_ == arg.uid_); 
  }

private:

  /// SCP/SCU role
  T_ASC_SC_ROLE role_;

  /// abstract syntax
  DcmUIDHandler uid_;
};


/** this helper class is a simple list of role selection list entries.
 *  Not intended for use by the end user.
 */
typedef OFList<DcmRoleSelectionItem> DcmRoleSelectionList;

/** this helper class maintains a map of role selection lists.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmRoleSelectionMap
{
public:
  /// constructor
  DcmRoleSelectionMap();

  /// destructor
  ~DcmRoleSelectionMap();

  /// Copy constructor, creates deep copy
  DcmRoleSelectionMap(const DcmRoleSelectionMap& arg);

  /// Copy assignment operator, creates deep copy
  DcmRoleSelectionMap& operator=(const DcmRoleSelectionMap& arg);

  /** Resets DcmRoleSelectionMap and frees any allocated memory
   */
  void clear();

  /** add new entry to list within map.
   *  If key is new, new list is created. Otherwise value
   *  is appended to existing list.
   *  @param key map key
   *  @param abstractSyntaxUID abstract syntax UID
   *  @param role SCP/SCU role
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition add(
    const char *key,
    const char *abstractSyntaxUID,
    T_ASC_SC_ROLE role);

  /** checks if the key is known
   *  @param key key name, must not be NULL
   *  @return true if key is known, false otherwise
   */
  OFBool isKnownKey(const char *key) const;

  /** checks if each abstract syntax in the role selection list
   *  identified by key is also contained in the presentation context
   *  list identified by pckey and maintained by pclist.
   *  @param key role selection key to check
   *  @param pclist map of presentation context lists
   *  @param pckey presentation context list key to check against
   *  @return EC_Normal if consistent, an error code otherwise
   */
  OFCondition checkConsistency(
    const char *key, 
    const DcmPresentationContextMap& pclist,
    const char *pckey) const;

  /** returns the list of role selection items identified by the given key
   *  @param key role selection list to search
   *  @return pointer to role selection list if found, NULL otherwise
   */
  const DcmRoleSelectionList *getRoleSelectionList(const char *key) const;

private:

  /// map of role selection lists
  OFMap<OFString, DcmRoleSelectionList *> map_;

};

#endif
