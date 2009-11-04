/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmPrivateTagCache
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-04 09:58:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/dcmtk/dcmdata/dcpcache.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DcmPrivateTagCacheEntry
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
class DcmPrivateTagCache
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

/*
 * CVS/RCS Log:
 * $Log: dcpcache.h,v $
 * Revision 1.4  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2005-12-08 16:28:29  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2004/10/20 15:56:13  meichel
 * Changed private inheritance from OFList to class member,
 *   needed for compilation with HAVE_STL.
 *
 * Revision 1.1  2002/07/23 14:21:27  meichel
 * Added support for private tag data dictionaries to dcmdata
 *
 *
 */
