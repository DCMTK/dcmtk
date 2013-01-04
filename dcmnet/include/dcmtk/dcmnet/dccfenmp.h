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
 *    class DcmExtendedNegotiationItem
 *    class DcmExtendedNegotiationMap
 *
 */

#ifndef DCCFENMP_H
#define DCCFENMP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"    /* for class OFList<> */
#include "dcmtk/ofstd/ofcond.h"    /* for class OFCondition */
#include "dcmtk/ofstd/ofmap.h"     /* for class OFMap */
#include "dcmtk/dcmnet/dccfuidh.h" /* for class DcmUIDHandler */

class DcmPresentationContextMap;

/** this helper class is a extended negotiation list entry.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmExtendedNegotiationItem
{
public:
  /** constructor. Raw data is copied into this object.
   *  @param abstractSyntax abstract syntax
   *  @param data pointer to raw data, must not be NULL
   *  @param length length of data block pointed to by data, in bytes
   */
  DcmExtendedNegotiationItem(
    const DcmUIDHandler& abstractSyntax,
    const unsigned char *data,
    Uint32 length);

  /// copy constructor
  DcmExtendedNegotiationItem(const DcmExtendedNegotiationItem& arg);
 
  /// destructor
  ~DcmExtendedNegotiationItem();

  /// assignment operator
  DcmExtendedNegotiationItem& operator=(const DcmExtendedNegotiationItem& arg);

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

  /** returns the abstract syntax UID
   *  maintained by this object as a C string
   *  @return reference to abstract syntax UID
   */   
  const char *getAbstractSyntaxC() const
  {
    return uid_.c_str();
  }

  /** returns the length of the raw data block in bytes
   */
  Uint32 getLength() const
  {
    return length_;
  }

  /** returns a pointer to the raw data block
   */
  const unsigned char *getRaw() const
  {
    return raw_;
  }

  /** comparison operator.
   *  @param arg object to compare with
   *  @return true if equal
   */
  OFBool operator==(const DcmExtendedNegotiationItem& arg) const;

private:

  /// pointer to raw data block
  unsigned char *raw_;

  /// length of raw data block, in bytes
  Uint32 length_;

  /// abstract syntax UID
  DcmUIDHandler uid_;
};


/** this helper class is a simple list of extended negotiation list entries.
 *  Not intended for use by the end user.
 */
typedef OFList<DcmExtendedNegotiationItem> DcmExtendedNegotiationList;

/** this helper class maintains a map of extended negotiation lists.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmExtendedNegotiationMap
{
public:
  /// constructor
  DcmExtendedNegotiationMap();

  /// destructor
  ~DcmExtendedNegotiationMap();

  /// Copy constructor, performs deep copy
  DcmExtendedNegotiationMap(const DcmExtendedNegotiationMap& arg);

  /// Copy assignment operator, performs deep copy
  DcmExtendedNegotiationMap& operator=(const DcmExtendedNegotiationMap& arg);

  /** Resets DcmExtendedNegotiationMap and frees any allocated memory
   */
  void clear();

  /** add new entry to list within map.
   *  If key is new, new list is created. Otherwise value
   *  is appended to existing list.
   *  @param key map key
   *  @param abstractSyntaxUID abstract syntax UID
   *  @param rawData pointer to raw data, must not be NULL
   *  @param length length of data block pointed to by data, in bytes
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition add(
    const char *key,
    const char *abstractSyntaxUID,
    const unsigned char *rawData,
    Uint32 length);

  /** checks if the key is known
   *  @param key key name, must not be NULL
   *  @return true if key is known, false otherwise
   */
  OFBool isKnownKey(const char *key) const;

  /** checks if each abstract syntax in the extended negotiation list
   *  identified by key is also contained in the presentation context
   *  list identified by pckey and maintained by pclist.
   *  @param key extended negotiation key to check
   *  @param pclist map of presentation context lists
   *  @param pckey presentation context list key to check against
   *  @return EC_Normal if consistent, an error code otherwise
   */
  OFCondition checkConsistency(
    const char *key, 
    const DcmPresentationContextMap& pclist,
    const char *pckey) const;

  /** returns the list of extended negotiation items identified by the given key
   *  @param key extended negotiation list to search
   *  @return pointer to extended negotiation list if found, NULL otherwise
   */
  const DcmExtendedNegotiationList *getExtendedNegotiationList(const char *key) const;

private:

  /// map of extended negotiation lists
  OFMap<OFString, DcmExtendedNegotiationList *> map_;

};


#endif
