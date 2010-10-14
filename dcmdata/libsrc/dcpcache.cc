/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: general purpose 32-bit CRC in C++
 *           Code is based on the CRC32 implementation (C)1986 Gary S. Brown
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:08 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcpcache.h"
#include "dcmtk/dcmdata/dcelem.h"      /* for DcmElement, DcmObject */

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


/*
 * CVS/RCS Log:
 * $Log: dcpcache.cc,v $
 * Revision 1.7  2010-10-14 13:14:08  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2006/07/24 15:43:52  meichel
 * Fixed memory leak in private tag lookup cache.
 *
 * Revision 1.5  2005/12/08 15:41:24  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2004/10/20 15:56:15  meichel
 * Changed private inheritance from OFList to class member,
 *   needed for compilation with HAVE_STL.
 *
 * Revision 1.3  2004/02/04 16:40:48  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.2  2003/06/12 18:22:23  joergr
 * Modified code to use const_iterators where appropriate (required for STL).
 *
 * Revision 1.1  2002/07/23 14:21:34  meichel
 * Added support for private tag data dictionaries to dcmdata
 *
 *
 */
