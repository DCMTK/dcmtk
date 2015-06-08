/*
 *
 *  Copyright (C) 1994-2015, OFFIS e.V.
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
 *    class DcmPresentationContextItem
 *    class DcmPresentationContextMap
 *
 */

#ifndef DCCFPCMP_H
#define DCCFPCMP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"   /* for class OFList<> */
#include "dcmtk/ofstd/ofstring.h" /* for class OFString */
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/ofstd/ofmap.h"    /* for class OFMap */
#include "dcmtk/dcmnet/dccfuidh.h" /* for class DcmUIDHandler */


/** this helper class is a presentation context list entry.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmPresentationContextItem
{
public:
  /** constructor
   *  @param abstractSyntax abstract syntax
   *  @param xferSyntaxGroup symbolic key for transfer syntax list
   */
  DcmPresentationContextItem(
    const DcmUIDHandler& abstractSyntax,
    const OFString& xferSyntaxGroup );

  /// copy constructor
  DcmPresentationContextItem(const DcmPresentationContextItem& arg);
 
  /// destructor
  ~DcmPresentationContextItem();

  /// assignment operator
  DcmPresentationContextItem& operator=(const DcmPresentationContextItem& arg);

  /** checks if the given argument matches the abstract syntax UID
   *  maintained by this object
   *  @param arg argument to compare to
   *  @return true if equal, false otherwise
   */
  OFBool matches(const DcmUIDHandler& arg) const
  {
    return arg == uid_;
  }

  /** returns the abstract syntax UID
   *  @return abstract syntax UID, never NULL
   */
  const char *getAbstractSyntax() const
  {
    return uid_.c_str();
  }

  /** returns the key for the list of transfer syntaxes maintained
   *  by this object
   *  @return transfer syntax key
   */
  const char *getTransferSyntaxKey() const
  {
    return xferSyntaxGroup_.c_str();
  }

  /** comparison operator.
   *  @param arg object to compare with
   *  @return true if equal
   */
  OFBool operator==(const DcmPresentationContextItem& arg) const
  {
    return (uid_ == arg.uid_) && (xferSyntaxGroup_ == arg.xferSyntaxGroup_); 
  }

private:

  /// abstract syntax UID
  DcmUIDHandler uid_;

  /// key of the transfer syntax group
  OFString xferSyntaxGroup_;
};


/** this helper class is a simple list of presentation context list entries.
 *  Not intended for use by the end user.
 */
typedef OFList<DcmPresentationContextItem> DcmPresentationContextList;


/** this helper class maintains a map of presentation context lists.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmPresentationContextMap
{
public:
  /// constructor
  DcmPresentationContextMap();

  /// destructor
  ~DcmPresentationContextMap();

  /** Resets DcmPresentationContextMap and frees any allocated memory.
   */
  void clear();

  /// Copy constructor, creates deep copy
  DcmPresentationContextMap(const DcmPresentationContextMap& arg);

  /// Copy assignment operator, creates deep copy
  DcmPresentationContextMap& operator=(const DcmPresentationContextMap& arg);

  /** const iterator pointing to start of presentation context map
   *  @return iterator to start of presentation context map
   */
  OFMap<OFString, DcmPresentationContextList*>::const_iterator begin();

  /** const iterator pointing to end of presentation context map (behind last entry)
   *  @return iterator to end of presentation context map
   */
  OFMap<OFString, DcmPresentationContextList*>::const_iterator end();

  /** get list of presentation contexts given the corresponding symbolic name
   *  @param pcName the symbolic name of the pc list
   *  @return the requested presentation context list (or NULL, if not existing)
   */
  const DcmPresentationContextList* getPresentationContextList(const OFString& pcName);

  /** add new entry to list within map.
   *  If key is new, new list is created. Otherwise value
   *  is appended to existing list.
   *  @param key map key
   *  @param abstractSyntaxUID abstract syntax UID
   *  @param transferSyntaxKey symbolic name of transfer syntax UID list, not checked here.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition add(
    const char *key,
    const char *abstractSyntaxUID,
    const char *transferSyntaxKey);

  /** checks if the key is known
   *  @param key key name, must not be NULL
   *  @return true if key is known, false otherwise
   */
  OFBool isKnownKey(const char *key) const;

  /** checks if the given abstract syntax is contained in the list
   *  identified by the given key.
   *  @param key presentation context list to search
   *  @param abstractSyntax abstract syntax UID to search
   *  @return true if abstract syntax is in list, false otherwise
   */
  OFBool isKnownAbstractSyntax(const char *key, const DcmUIDHandler& abstractSyntax) const;

  /** returns the list of presentation contexts identified by the given key
   *  @param key presentation context list to search
   *  @return pointer to presentation context list if found, NULL otherwise
   */
  const DcmPresentationContextList *getPresentationContextList(const char *key) const;

private:

  /// map of presentation context lists
  OFMap<OFString, DcmPresentationContextList *> map_;

};

#endif
