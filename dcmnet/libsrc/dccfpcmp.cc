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
  OFListIterator(DcmKeyValuePair<DcmPresentationContextList *> *) first = map_.begin();
  OFListIterator(DcmKeyValuePair<DcmPresentationContextList *> *) last = map_.end();
  while (first != last)
  {
    delete (*first)->value();
    ++first;
  }  
}

OFCondition DcmPresentationContextMap::add(
  const char *key,
  const char *abstractSyntaxUID,
  const char *transferSyntaxKey)
{
  if ((!key)||(!abstractSyntaxUID)||(!transferSyntaxKey)) return EC_IllegalCall;


  // perform syntax check of UID
  DcmUIDHandler uid(abstractSyntaxUID);
  if (! uid.isValidUID())
  {
    OFString s("not a valid abstract syntax UID: ");
    s += abstractSyntaxUID;
    return makeOFCondition(OFM_dcmnet, 1025, OF_error, s.c_str());
  }

  OFString skey(key);
  DcmPresentationContextList * const *value = OFconst_cast(DcmPresentationContextList * const *, map_.lookup(skey));
  if (value == NULL)
  {
    DcmPresentationContextList *newentry = new DcmPresentationContextList();
    map_.add(skey, OFstatic_cast(DcmPresentationContextList *, newentry));
    value = &newentry;
  }

  // make sure list does not get longer than 128 entries
  if (((*value)->size()) > 127)
  {
    OFString s("presentation context list too long (> 128 entries): ");
    s += key;
    return makeOFCondition(OFM_dcmnet, 1032, OF_error, s.c_str());
  }

  // insert values into list.
  (*value)->push_back(DcmPresentationContextItem(uid, OFString(transferSyntaxKey)));
  return EC_Normal;  
}

OFBool DcmPresentationContextMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;
  if (map_.lookup(OFString(key))) return OFTrue;
  return OFFalse;
}

OFBool DcmPresentationContextMap::isKnownAbstractSyntax(
  const char*key, 
  const DcmUIDHandler& abstractSyntax) const
{
  if (!key) return OFFalse;

  OFString skey(key);
  DcmPresentationContextList * const *value = OFconst_cast(DcmPresentationContextList * const *, map_.lookup(skey));
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
    DcmPresentationContextList * const *value = OFconst_cast(DcmPresentationContextList * const *, map_.lookup(OFString(key)));
    if (value) result = *value;
  }
  return result;
}
