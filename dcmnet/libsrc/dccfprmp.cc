/*
 *
 *  Copyright (C) 2003-2015, OFFIS e.V.
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
 *    class DcmProfileEntry
 *    class DcmProfileMap
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dccfprmp.h" /* for class DcmProfileMap */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */


DcmProfileEntry::DcmProfileEntry(
    const OFString& presentationContextGroup,
    const OFString& roleSelectionGroup,
    const OFString& extendedNegotiationGroup)
: presentationContextGroup_(presentationContextGroup)
, roleSelectionGroup_(roleSelectionGroup)
, extendedNegotiationGroup_(extendedNegotiationGroup)
{
}

DcmProfileEntry::DcmProfileEntry(const DcmProfileEntry& arg)
: presentationContextGroup_(arg.presentationContextGroup_)
, roleSelectionGroup_(arg.roleSelectionGroup_)
, extendedNegotiationGroup_(arg.extendedNegotiationGroup_)
{
}

DcmProfileEntry& DcmProfileEntry::operator=(const DcmProfileEntry& arg)
{
  if (this != &arg)
  {
    presentationContextGroup_ = arg.presentationContextGroup_;
    roleSelectionGroup_ = arg.roleSelectionGroup_;
    extendedNegotiationGroup_ = arg.extendedNegotiationGroup_;
  }
  return *this;
}

DcmProfileEntry::~DcmProfileEntry()
{
}

const char *DcmProfileEntry::getPresentationContextKey() const
{
  return presentationContextGroup_.c_str();
}

const char *DcmProfileEntry::getRoleSelectionKey() const
{
  if (roleSelectionGroup_.size() == 0) return NULL; else return roleSelectionGroup_.c_str();
}

const char *DcmProfileEntry::getExtendedNegotiationKey() const
{
  if (extendedNegotiationGroup_.size() == 0) return NULL; else return extendedNegotiationGroup_.c_str();
}

/* ========================================================= */

DcmProfileMap::DcmProfileMap()
: map_()
{
}

DcmProfileMap::~DcmProfileMap()
{
  clear();
}

DcmProfileMap::DcmProfileMap(const DcmProfileMap& arg)
{
  /* Copy all map entries */
  OFMap<OFString, DcmProfileEntry *>::const_iterator first = arg.map_.begin();
  OFMap<OFString, DcmProfileEntry *>::const_iterator last = arg.map_.end();
  while (first != last)
  {
    DcmProfileEntry* copy = new DcmProfileEntry( *(*first).second );
    map_.insert( OFPair<const OFString, DcmProfileEntry*>( (*first).first, copy ) );
    ++first;
  }
}

DcmProfileMap& DcmProfileMap::operator=(const DcmProfileMap& arg)
{
  if (this != &arg)
  {
     /* Clear old and copy all map entries */
    this->clear();
    OFMap<OFString, DcmProfileEntry *>::const_iterator first = arg.map_.begin();
    OFMap<OFString, DcmProfileEntry *>::const_iterator last = arg.map_.end();
    while (first != last)
    {
      DcmProfileEntry* copy = new DcmProfileEntry( *(*first).second );
      map_.insert( OFPair<const OFString, DcmProfileEntry*>( (*first).first, copy ) ); // TODO: Does that work?
      ++first;
    }
  }
  return *this;
}

OFMap<OFString, DcmProfileEntry*>::const_iterator DcmProfileMap::begin()
{
  return map_.begin();
}

OFMap<OFString, DcmProfileEntry*>::const_iterator DcmProfileMap::end()
{
  return map_.end();
}

const DcmProfileEntry* DcmProfileMap::getProfile(const OFString& name)
{
  OFMap<OFString,DcmProfileEntry*>::const_iterator it = map_.find(name);
  if ( it != map_.end() )
  {
    return (*it).second;
  }
  return NULL;
}

void DcmProfileMap::clear()
{
  while (map_.size () != 0)
  {
    OFMap<OFString, DcmProfileEntry *>::iterator first = map_.begin();
    delete (*first).second;
    map_.erase(first);
  }
}

OFCondition DcmProfileMap::add(
    const char *key,
    const char *presentationContextKey,
    const char *roleSelectionKey,
    const char *extendedNegotiationKey)
{
  if ((!key)||(!presentationContextKey)) return EC_IllegalCall;

  OFString presKey(presentationContextKey);
  OFString roleKey;
  if (roleSelectionKey) roleKey = roleSelectionKey;
  OFString extnegKey;
  if (extendedNegotiationKey) extnegKey = extendedNegotiationKey;

  OFString skey(key);
  OFMap<OFString, DcmProfileEntry*>::iterator it = map_.find(skey);

  if (it == map_.end())
  {
    DcmProfileEntry *newentry = new DcmProfileEntry(presKey, roleKey, extnegKey);
    map_.insert(OFPair<OFString, DcmProfileEntry*>(skey, newentry));
  }
  else
  {
    // error: key already present
    OFString s("two profiles defined for key: ");
    s += presKey;
    return makeOFCondition(OFM_dcmnet, 1030, OF_error, s.c_str());
  }

  return EC_Normal;
}

OFBool DcmProfileMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;
  if (map_.find(OFString(key)) != map_.end()) return OFTrue;
  return OFFalse;
}


const char *DcmProfileMap::getPresentationContextKey(const char *key) const
{
  if (key)
  {
    OFMap<OFString, DcmProfileEntry*>::const_iterator it = map_.find(OFString(key));
    if (it != map_.end())
      return (*it).second->getPresentationContextKey();
  }
  return NULL;
}


const char *DcmProfileMap::getRoleSelectionKey(const char *key) const
{
  if (key)
  {
    OFMap<OFString, DcmProfileEntry*>::const_iterator it = map_.find(OFString(key));
    if (it != map_.end())
    {
      if ( (*it).second != NULL)
        return (*it).second->getRoleSelectionKey();
    }
  }
  return NULL;
}


const char *DcmProfileMap::getExtendedNegotiationKey(const char *key) const
{
  if (key)
  {
    OFMap<OFString, DcmProfileEntry*>::const_iterator it = map_.find(OFString(key));
    if (it != map_.end())
    {
      if ( (*it).second != NULL)
        return (*it).second->getExtendedNegotiationKey();
    }
  }
  return NULL;
}
