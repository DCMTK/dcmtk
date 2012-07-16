/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
  OFListIterator(DcmKeyValuePair<DcmProfileEntry *> *) first = map_.begin();
  OFListIterator(DcmKeyValuePair<DcmProfileEntry *> *) last = map_.end();
  while (first != last)
  {
    delete (*first)->value();
    ++first;
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
  DcmProfileEntry * const *value = OFconst_cast(DcmProfileEntry * const *, map_.lookup(skey));
  if (value == NULL)
  {
    DcmProfileEntry *newentry = new DcmProfileEntry(presKey, roleKey, extnegKey);
    map_.add(skey, OFstatic_cast(DcmProfileEntry *, newentry));
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
  if (map_.lookup(OFString(key))) return OFTrue;
  return OFFalse;
}


const char *DcmProfileMap::getPresentationContextKey(const char *key) const
{
  if (key)
  {
    DcmProfileEntry * const *entry = OFconst_cast(DcmProfileEntry * const *, map_.lookup(OFString(key)));
    if (entry)
    {
      return (*entry)->getPresentationContextKey();
    }
  }
  return NULL;
}


const char *DcmProfileMap::getRoleSelectionKey(const char *key) const
{
  if (key)
  {
    DcmProfileEntry * const *entry = OFconst_cast(DcmProfileEntry * const *, map_.lookup(OFString(key)));
    if (entry)
    {
      return (*entry)->getRoleSelectionKey();
    }
  }
  return NULL;
}


const char *DcmProfileMap::getExtendedNegotiationKey(const char *key) const
{
  if (key)
  {
    DcmProfileEntry * const *entry = OFconst_cast(DcmProfileEntry * const *, map_.lookup(OFString(key)));
    if (entry)
    {
      return (*entry)->getExtendedNegotiationKey();
    }
  }
  return NULL;
}
