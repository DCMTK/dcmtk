/*
 *
 *  Copyright (C) 2003-2017, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dccfpcmp.h" /* for class DcmPresentationContextMap */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */


DcmPresentationContextItem::DcmPresentationContextItem(
  const DcmUIDHandler& abstractSyntax,
  const OFString& xferSyntaxGroup )
: uid_(abstractSyntax)
, xferSyntaxGroup_(xferSyntaxGroup)
{
}

DcmPresentationContextItem::DcmPresentationContextItem(const DcmPresentationContextItem& arg)
: uid_(arg.uid_)
, xferSyntaxGroup_(arg.xferSyntaxGroup_)
{
}

DcmPresentationContextItem::~DcmPresentationContextItem()
{
}

DcmPresentationContextItem& DcmPresentationContextItem::operator=(const DcmPresentationContextItem& arg)
{
  uid_ = arg.uid_;
  xferSyntaxGroup_ = arg.xferSyntaxGroup_;
  return *this;
}

/* ========================================================= */

DcmPresentationContextMap::DcmPresentationContextMap()
: map_()
{
}

DcmPresentationContextMap::~DcmPresentationContextMap()
{
  clear();
}

DcmPresentationContextMap::DcmPresentationContextMap(const DcmPresentationContextMap& arg)
{
  /* Copy all map entries */
  OFMap<OFString, DcmPresentationContextList *>::const_iterator first = arg.map_.begin();
  OFMap<OFString, DcmPresentationContextList *>::const_iterator last = arg.map_.end();
  while (first != last)
  {
    DcmPresentationContextList* copy = new DcmPresentationContextList( *(*first).second );
    map_.insert( OFPair<const OFString, DcmPresentationContextList*>( (*first).first, copy ) );
    ++first;
  }
}

DcmPresentationContextMap& DcmPresentationContextMap::operator=(const DcmPresentationContextMap& arg)
{
  if (this != &arg )
  {
    /* Clear and copy all map entries */
    this->clear();
    OFMap<OFString, DcmPresentationContextList *>::const_iterator first = arg.map_.begin();
    OFMap<OFString, DcmPresentationContextList *>::const_iterator last = arg.map_.end();
    while (first != last)
    {
      DcmPresentationContextList* copy = new DcmPresentationContextList( *(*first).second );
      map_.insert(OFPair<const OFString, DcmPresentationContextList*>( (*first).first, copy ) );
      ++first;
    }
  }
  return *this;
}

void DcmPresentationContextMap::clear()
{
  while (map_.size () != 0)
  {
    OFMap<OFString, DcmPresentationContextList *>::iterator first = map_.begin();
    delete (*first).second;
    map_.erase(first);
  }
}


OFMap<OFString, DcmPresentationContextList*>::const_iterator DcmPresentationContextMap::begin()
{
  return map_.begin();
}


OFMap<OFString, DcmPresentationContextList*>::const_iterator DcmPresentationContextMap::end()
{
  return map_.end();
}

const DcmPresentationContextList* DcmPresentationContextMap::getPresentationContextList(const OFString& pcName)
{
  OFMap<OFString,DcmPresentationContextList*>::const_iterator it = map_.find(pcName);
  if ( it == map_.end() )
  {
    return NULL;
  }
  else
    return (*it).second;
}

OFCondition DcmPresentationContextMap::add(
  const OFString& key,
  const OFString& abstractSyntaxUID,
  const OFString& transferSyntaxKey)
{
  if ((key.empty())||(abstractSyntaxUID.empty())||(transferSyntaxKey.empty())) return EC_IllegalCall;


  // perform syntax check of UID
  DcmUIDHandler uid(abstractSyntaxUID);
  if (! uid.isValidUID())
  {
    OFString s("not a valid abstract syntax UID: ");
    s += abstractSyntaxUID;
    return makeOFCondition(OFM_dcmnet, 1025, OF_error, s.c_str());
  }

  DcmPresentationContextList* value = NULL;
  OFString skey(key);
  OFMap<OFString, DcmPresentationContextList*>::iterator it = map_.find(skey);

  // create new value if not existing yet
  if (it == map_.end())
  {
    DcmPresentationContextList *newentry = new DcmPresentationContextList;
    map_.insert(OFPair<OFString, DcmPresentationContextList*>(skey, newentry));
    value = newentry;
  }
  else // use existing value
    value = ((*it).second);

  // make sure list does not get longer than 128 entries
  if (((value)->size()) > 127)
  {
    OFString s("presentation context list too long (> 128 entries): ");
    s += key;
    return makeOFCondition(OFM_dcmnet, 1032, OF_error, s.c_str());
  }

  // insert values into list.
  (value)->push_back(DcmPresentationContextItem(uid, transferSyntaxKey));
  return EC_Normal;
}

OFBool DcmPresentationContextMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;

  if (map_.find(OFString(key)) != map_.end()) return OFTrue;
  return OFFalse;
}

OFBool DcmPresentationContextMap::isKnownAbstractSyntax(
  const char*key,
  const DcmUIDHandler& abstractSyntax) const
{
  if (!key) return OFFalse;

  OFString skey(key);
  DcmPresentationContextList * const *value = NULL;
  OFMap<OFString, DcmPresentationContextList*>::const_iterator it = map_.find(OFString(skey));
  if ( it != map_.end() )
    value = & ((*it).second);

  if (value)
  {
    // check if abstract syntax is in list
    OFListIterator(DcmPresentationContextItem) first = (*value)->begin();
    OFListIterator(DcmPresentationContextItem) last = (*value)->end();
    while (first != last)
    {
      if ((*first).matches(abstractSyntax)) return OFTrue;
      ++first;
    }
  }

  return OFFalse;
}

const DcmPresentationContextList *DcmPresentationContextMap::getPresentationContextList(const char *key) const
{
  const DcmPresentationContextList *result = NULL;
  if (key)
  {
    OFMap<OFString, DcmPresentationContextList*>::const_iterator it = map_.find(OFString(key));
    if (it != map_.end())
      result = (*it).second;
  }
  return result;
}
