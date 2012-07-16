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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmPrivateTagCache
 *
 */

#ifndef DCPCACHE_H
#define DCPCACHE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"     /* for OFBool */
#include "dcmtk/ofstd/oflist.h"      /* for OFList */
#include "dcmtk/ofstd/ofstring.h"    /* for OFString */
#include "dcmtk/dcmdata/dctagkey.h"    /* for DcmTagKey */

class DcmObject;

/** class handling one entry of the Private Tag Cache List
 */
class DCMTK_DCMDATA_EXPORT DcmPrivateTagCacheEntry
{
public:
  /** constructor
   *  @param tk tag key for private creator element
   *  @param pc private creator name, must not be NULL or empty string
   */
  DcmPrivateTagCacheEntry(const DcmTagKey& tk, const char *pc);

  /// destructor
  virtual ~DcmPrivateTagCacheEntry();

  /** returns the private creator name
   */
  const char *getPrivateCreator() const;

  /** checks if this element is the private creator for the element
   *  with the given tag key
   *  @param tk tag key to check
   *  @return OFTrue if this element contains the matching private creator,
   *    OFFalse otherwise.
   */
  OFBool isPrivateCreatorFor(const DcmTagKey& tk) const;

private:

  /// private undefined copy constructor
  DcmPrivateTagCacheEntry(const DcmPrivateTagCacheEntry&);
  
  /// private undefined copy assignment operator
  DcmPrivateTagCacheEntry& operator=(const DcmPrivateTagCacheEntry&);

  /// the tag key of the private creator element
  DcmTagKey tagKey;

  /// the private creator name
  OFString privateCreator;
};


/** this class implements a cache of Private Creator elements
 *  and corresponding reserved tag numbers.
 */
class DCMTK_DCMDATA_EXPORT DcmPrivateTagCache
{
public:
  /// default constructor
  DcmPrivateTagCache();

  /// destructor
  virtual ~DcmPrivateTagCache();

  /// resets the cache to default-constructed state
  void clear();

  /** looks up the private creator name for the given private tag
   *  @param tk private tag to check
   *  @return private creator name if found, NULL otherwise.
   */
  const char *findPrivateCreator(const DcmTagKey& tk) const;

  /** updates the private creator cache with the given object.
   *  If the object points to a private creator element,
   *  the tag key and creator code are added to the cache.
   *  Otherwise, the cache remains unmodified.
   */
  void updateCache(DcmObject *dobj);

private:

  /// private undefined copy constructor
  DcmPrivateTagCache(const DcmPrivateTagCache&);
  
  /// private undefined copy assignment operator
  DcmPrivateTagCache& operator=(const DcmPrivateTagCache&);

  /// the list
  OFList<DcmPrivateTagCacheEntry *> list_;
};

#endif
