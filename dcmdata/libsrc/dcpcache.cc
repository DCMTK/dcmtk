/*
 *
 *  Copyright (C) 2002-2023, OFFIS e.V.
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


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcpcache.h"
#include "dcmtk/dcmdata/dcelem.h"     /* for DcmElement, DcmObject */

DcmPrivateTagCacheEntry::DcmPrivateTagCacheEntry(const DcmTagKey& tk, const char *pc)
: tagKey(tk)
, privateCreator(pc)
{
}


DcmPrivateTagCacheEntry::~DcmPrivateTagCacheEntry()
{
}


const char *DcmPrivateTagCacheEntry::getPrivateCreator() const
{
  return privateCreator.c_str();
}


OFBool DcmPrivateTagCacheEntry::isPrivateCreatorFor(const DcmTagKey& tk) const
{
  return (tagKey.getGroup() == tk.getGroup()) &&
         ((tagKey.getElement() << 8) == (tk.getElement() & 0xff00));
}

/* ======================================================================= */

DcmPrivateTagCache::DcmPrivateTagCache()
: list_()
{
}


DcmPrivateTagCache::~DcmPrivateTagCache()
{
  clear();
}


void DcmPrivateTagCache::clear()
{
  OFListIterator(DcmPrivateTagCacheEntry *) first = list_.begin();
  OFListIterator(DcmPrivateTagCacheEntry *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}


const char *DcmPrivateTagCache::findPrivateCreator(const DcmTagKey& tk) const
{
  OFListConstIterator(DcmPrivateTagCacheEntry *) first = list_.begin();
  OFListConstIterator(DcmPrivateTagCacheEntry *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isPrivateCreatorFor(tk)) return (*first)->getPrivateCreator();
    ++first;
  }
  return NULL;
}


void DcmPrivateTagCache::updateCache(DcmObject *dobj)
{
  if (dobj)
  {
    const DcmTag& tag = dobj->getTag();
    if (dobj->isLeaf() && (tag.getGTag() & 1) && (tag.getETag() <= 0xff) && (tag.getETag() >= 0x10))
    {
      // dobj is DcmElement containing private creator
      char *c = NULL;
      if ((OFstatic_cast(DcmElement *, dobj)->getString(c)).good() && c)
      {
        list_.push_back(new DcmPrivateTagCacheEntry(tag, c));
      }
    }
  }
}
