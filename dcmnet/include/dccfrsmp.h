/*
 *
 *  Copyright (C) 1994-2003, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmRoleSelectionItem
 *    class DcmRoleSelectionMap
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-10 14:27:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dccfrsmp.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCCFRSMP_H
#define DCCFRSMP_H

#include "osconfig.h"
#include "oflist.h"   /* for class OFList<> */
#include "ofcond.h"   /* for class OFCondition */
#include "dcmsmap.h"  /* for class DcmSimpleMap<> */
#include "dccfuidh.h" /* for class DcmUIDHandler */
#include "assoc.h"    /* for T_ASC_SC_ROLE */

class DcmPresentationContextMap;

/** this helper class is a role selection list entry.
 *  Not intended for use by the end user.
 */
class DcmRoleSelectionItem
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

private:

  /// private undefined copy assignment operator
  DcmRoleSelectionItem& operator=(const DcmRoleSelectionItem& arg);

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
class DcmRoleSelectionMap
{
public:
  /// constructor
  DcmRoleSelectionMap();

  /// destructor
  ~DcmRoleSelectionMap();

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
  /// private undefined copy constructor
  DcmRoleSelectionMap(const DcmRoleSelectionMap& arg);

  /// private undefined copy assignment operator
  DcmRoleSelectionMap& operator=(const DcmRoleSelectionMap& arg);

  /// map of role selection lists
  DcmSimpleMap<DcmRoleSelectionList *> map_;

};

#endif

/*
 * CVS/RCS Log
 * $Log: dccfrsmp.h,v $
 * Revision 1.1  2003-06-10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
