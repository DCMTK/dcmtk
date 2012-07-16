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

DcmRoleSelectionMap::~DcmRoleSelectionMap()
{
  OFListIterator(DcmKeyValuePair<DcmRoleSelectionList *> *) first = map_.begin();
  OFListIterator(DcmKeyValuePair<DcmRoleSelectionList *> *) last = map_.end();
  while (first != last)
  {
    delete (*first)->value();
    ++first;
  }
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

  OFString skey(key);
  DcmRoleSelectionList * const *value = OFconst_cast(DcmRoleSelectionList * const *, map_.lookup(skey));
  if (value == NULL)
  {
    DcmRoleSelectionList *newentry = new DcmRoleSelectionList();
    map_.add(skey, OFstatic_cast(DcmRoleSelectionList *, newentry));
    value = &newentry;
  }
  else
  {
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

OFBool DcmRoleSelectionMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;
  if (map_.lookup(OFString(key))) return OFTrue;
  return OFFalse;
}

OFCondition DcmRoleSelectionMap::checkConsistency(
  const char *key,
  const DcmPresentationContextMap& pclist,
  const char *pckey) const
{
  if ((!key)||(!pckey)) return EC_IllegalCall;

  DcmRoleSelectionList * const *entry = OFconst_cast(DcmRoleSelectionList * const *, map_.lookup(OFString(key)));
  if (!entry)
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
    DcmRoleSelectionList * const *value = OFconst_cast(DcmRoleSelectionList * const *, map_.lookup(OFString(key)));
    if (value) result = *value;
  }
  return result;
}
