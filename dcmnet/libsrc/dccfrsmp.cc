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
 *    class DcmRoleSelectionItem
 *    class DcmRoleSelectionMap
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dccfrsmp.h" /* for class DcmRoleSelectionMap */
#include "dcmtk/dcmnet/dccfpcmp.h" /* for class DcmPresentationContextMap */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */

DcmRoleSelectionItem::DcmRoleSelectionItem(
    T_ASC_SC_ROLE role,
    const DcmUIDHandler& abstractSyntax )
: role_(role)
, uid_(abstractSyntax)
{
}

DcmRoleSelectionItem::DcmRoleSelectionItem(const DcmRoleSelectionItem& arg)
: role_(arg.role_)
, uid_(arg.uid_)
{
}

DcmRoleSelectionItem::~DcmRoleSelectionItem()
{
}

DcmRoleSelectionItem& DcmRoleSelectionItem::operator=(const DcmRoleSelectionItem& arg)
{
  role_ = arg.role_;
  uid_ = arg.uid_;
  return *this;
}

/* ========================================================= */

DcmRoleSelectionMap::DcmRoleSelectionMap()
: map_()
{
}

void DcmRoleSelectionMap::clear()
{
  while (map_.size () != 0)
  {
    OFMap<OFString, DcmRoleSelectionList *>::iterator first = map_.begin();
    delete (*first).second;
    map_.erase(first);
  }
}

DcmRoleSelectionMap::~DcmRoleSelectionMap()
{
  clear();
}

/// Copy constructor, creates deep copy
DcmRoleSelectionMap::DcmRoleSelectionMap(const DcmRoleSelectionMap& arg)
{
  /* Copy all map entries */
  OFMap<OFString, DcmRoleSelectionList *>::const_iterator first = arg.map_.begin();
  OFMap<OFString, DcmRoleSelectionList *>::const_iterator last = arg.map_.end();
  while (first != last)
  {
    DcmRoleSelectionList* copy = new DcmRoleSelectionList( *(*first).second );
    map_.insert(OFPair<const OFString, DcmRoleSelectionList*>( (*first).first, copy ) );
    ++first;
  }
}

/// Copy assignment operator, creates deep copy
DcmRoleSelectionMap& DcmRoleSelectionMap::operator=(const DcmRoleSelectionMap& arg)
{
  if (this != &arg)
  {
    OFMap<OFString, DcmRoleSelectionList *>::const_iterator first = arg.map_.begin();
    OFMap<OFString, DcmRoleSelectionList *>::const_iterator last = arg.map_.end();
    while (first != last)
    {
      DcmRoleSelectionList* copy = new DcmRoleSelectionList( *(*first).second );
      map_.insert( OFPair<const OFString, DcmRoleSelectionList*>( (*first).first, copy ) );
      ++first;
    }
  }
  return *this;
}

OFCondition DcmRoleSelectionMap::add(
  const char *key,
  const char *abstractSyntaxUID,
  T_ASC_SC_ROLE role)
{
  if ((!key)||(!abstractSyntaxUID)) return EC_IllegalCall;

  // perform syntax check of UID
  DcmUIDHandler uid(abstractSyntaxUID);
  if (! uid.isValidUID())
  {
    OFString s("not a valid abstract syntax UID: ");
    s += abstractSyntaxUID;
    return makeOFCondition(OFM_dcmnet, 1026, OF_error, s.c_str());
  }

  DcmRoleSelectionList * const *value = NULL;
  OFString skey(key);
  OFMap<OFString, DcmRoleSelectionList*>::iterator it = map_.find(skey);

  if (it == map_.end())
  {
    DcmRoleSelectionList *newentry = new DcmRoleSelectionList();
    map_.insert(OFPair<OFString, DcmRoleSelectionList*>(skey, newentry));
    value = &newentry;
  }
  else
  {
    value = & ((*it).second);
    // check if abstract syntax is already in list
    OFListIterator(DcmRoleSelectionItem) first = (*value)->begin();
    OFListIterator(DcmRoleSelectionItem) last = (*value)->end();
    while (first != last)
    {
      if ((*first).matches(uid))
      {
        // error: role selection defined twice for one abstract syntax
        OFString s("role selection defined twice for abstract syntax UID: ");
        s += abstractSyntaxUID;
        return makeOFCondition(OFM_dcmnet, 1027, OF_error, s.c_str());
      }
      ++first;
    }
  }

  // insert values into list.
  (*value)->push_back(DcmRoleSelectionItem(role, uid));
  return EC_Normal;
}

OFCondition DcmRoleSelectionMap::addEmpty(
  const char *key)
{
  if (!key) return EC_IllegalCall;
  OFString skey(key);
  OFMap<OFString, DcmRoleSelectionList*>::iterator it = map_.find(skey);

  if (it == map_.end())
  {
    DcmRoleSelectionList *newentry = new DcmRoleSelectionList();
    map_.insert(OFPair<OFString, DcmRoleSelectionList*>(skey, newentry));
  }
  return EC_Normal;
}

OFBool DcmRoleSelectionMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;
  if (map_.find(OFString(key)) != map_.end()) return OFTrue;
  return OFFalse;
}

OFCondition DcmRoleSelectionMap::checkConsistency(
  const char *key,
  const DcmPresentationContextMap& pclist,
  const char *pckey) const
{
  if ((!key)||(!pckey)) return EC_IllegalCall;

  DcmRoleSelectionList * const *entry = NULL;
  OFMap<OFString, DcmRoleSelectionList*>::const_iterator it = map_.find(OFString(key));
  if (it == map_.end())
  {
    // error: key undefined
    OFString s("role selection key undefined: ");
    s += key;
    return makeOFCondition(OFM_dcmnet, 1036, OF_error, s.c_str());
  }

  if (! pclist.isKnownKey(pckey))
  {
    // error: key undefined
    OFString s("presentation context key undefined: ");
    s += pckey;
    return makeOFCondition(OFM_dcmnet, 1037, OF_error, s.c_str());
  }

  // continue with existing entry
  entry = & ((*it).second);
  if (entry == NULL)
  {
    // error: key undefined
    OFString s("presentation context NULL entry for key: ");
    s += pckey;
    return makeOFCondition(OFM_dcmnet, 1037, OF_error, s.c_str());
  }

  OFListIterator(DcmRoleSelectionItem) first = (*entry)->begin();
  OFListIterator(DcmRoleSelectionItem) last = (*entry)->end();
  while (first != last)
  {
    if (! pclist.isKnownAbstractSyntax(pckey, (*first).getAbstractSyntax()))
    {
      // error: abstract syntax inconsistency
      OFString s("abstract syntax defined in role selection list '");
      s += key;
      s += "' but not in presentation context list '";
      s += pckey;
      s += "': ";
      s += (*first).getAbstractSyntax().c_str();

      return makeOFCondition(OFM_dcmnet, 1038, OF_error, s.c_str());
    }
    ++first;
  }
  return EC_Normal;
}

const DcmRoleSelectionList *DcmRoleSelectionMap::getRoleSelectionList(const char *key) const
{
  const DcmRoleSelectionList *result = NULL;
  if (key)
  {
    OFMap<OFString, DcmRoleSelectionList*>::const_iterator it = map_.find(OFString(key));
    if (it != map_.end())
      result = (*it).second;
  }
  return result;
}
