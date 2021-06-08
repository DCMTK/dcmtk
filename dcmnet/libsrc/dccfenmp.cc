/*
 *
 *  Copyright (C) 2003-2021, OFFIS e.V.
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
 *    class DcmExtendedNegotiationItem
 *    class DcmExtendedNegotiationMap
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dccfenmp.h" /* for class DcmExtendedNegotiationMap */
#include "dcmtk/dcmnet/dccfpcmp.h" /* for class DcmPresentationContextMap */
#include "dcmtk/dcmdata/dcerror.h" /* for EC_IllegalCall */
#include "dcmtk/ofstd/ofdefine.h"  /* for memcmp() */

DcmExtendedNegotiationItem::DcmExtendedNegotiationItem(
  const DcmUIDHandler& abstractSyntax,
  const unsigned char *data,
  Uint32 length)
: raw_(NULL)
, length_(0)
, uid_(abstractSyntax)
{
  if (length && data)
  {
    length_ = length;
    raw_ = new unsigned char[length_];
    (void) memcpy(raw_, data, OFstatic_cast(size_t, length_));
  }
}

DcmExtendedNegotiationItem::DcmExtendedNegotiationItem(const DcmExtendedNegotiationItem& arg)
: raw_(NULL)
, length_(arg.length_)
, uid_(arg.uid_)
{
  if (length_)
  {
    raw_ = new unsigned char[length_];
    (void) memcpy(raw_, arg.raw_, OFstatic_cast(size_t, length_));
  }
}

DcmExtendedNegotiationItem::~DcmExtendedNegotiationItem()
{
  delete[] raw_;
  raw_ = NULL;
  length_ = 0;
}

DcmExtendedNegotiationItem& DcmExtendedNegotiationItem::operator=(const DcmExtendedNegotiationItem& arg)
{
  if (raw_)
    delete[] raw_;
  if (arg.length_ && arg.raw_)
  {
    length_ = arg.length_;
    raw_ = new unsigned char[length_];
    (void) memcpy(raw_, arg.raw_, OFstatic_cast(size_t, length_));
  }
  else
  {
    raw_ = NULL;
    length_ = 0;
  }
  return *this;
}

OFBool DcmExtendedNegotiationItem::operator==(const DcmExtendedNegotiationItem& arg) const
{
  return (uid_ == arg.uid_)
      && (length_ == arg.length_)
      && ((length_ == 0) || (memcmp(raw_, arg.raw_, OFstatic_cast(size_t, length_)) == 0));
}

/* ========================================================= */

DcmExtendedNegotiationMap::DcmExtendedNegotiationMap()
: map_()
{
}

DcmExtendedNegotiationMap::DcmExtendedNegotiationMap(const DcmExtendedNegotiationMap& arg)
{
  /* Copy all map entries */
  OFMap<OFString, DcmExtendedNegotiationList *>::const_iterator first = arg.map_.begin();
  OFMap<OFString, DcmExtendedNegotiationList *>::const_iterator last = arg.map_.end();
  while (first != last)
  {
    DcmExtendedNegotiationList* copy = new DcmExtendedNegotiationList( *(*first).second );
    map_.insert( OFPair<const OFString, DcmExtendedNegotiationList*>( (*first).first, copy ) );
    ++first;
  }
}

DcmExtendedNegotiationMap& DcmExtendedNegotiationMap::operator=(const DcmExtendedNegotiationMap& arg)
{
  if (this != &arg)
  {
    this->clear();
    /* Clear old and copy all map entries */
    OFMap<OFString, DcmExtendedNegotiationList *>::const_iterator first = arg.map_.begin();
    OFMap<OFString, DcmExtendedNegotiationList *>::const_iterator last = arg.map_.end();
    while (first != last)
    {
      DcmExtendedNegotiationList* copy = new DcmExtendedNegotiationList( *(*first).second );
      map_.insert( OFPair<const OFString, DcmExtendedNegotiationList*>( (*first).first, copy ) );
      ++first;
    }
  }
  return *this;
}


DcmExtendedNegotiationMap::~DcmExtendedNegotiationMap()
{
  clear();
}

void DcmExtendedNegotiationMap::clear()
{
  while (map_.size () != 0)
  {
    OFMap<OFString, DcmExtendedNegotiationList *>::iterator first = map_.begin();
    delete (*first).second;
    map_.erase(first);
  }
}

OFCondition DcmExtendedNegotiationMap::add(
  const char *key,
  const char *abstractSyntaxUID,
  const unsigned char *rawData,
  Uint32 length)
{
  if ((!key)||(!abstractSyntaxUID)||(!rawData)||(!length)) return EC_IllegalCall;

  // perform syntax check of UID
  DcmUIDHandler uid(abstractSyntaxUID);
  if (! uid.isValidUID())
  {
    OFString s("not a valid abstract syntax UID: ");
    s += abstractSyntaxUID;
    return makeOFCondition(OFM_dcmnet, 1028, OF_error, s.c_str());
  }

  OFString skey(key);
  OFMap<OFString, DcmExtendedNegotiationList*>::iterator it = map_.find(skey);

  DcmExtendedNegotiationList * const *value = NULL;
  DcmExtendedNegotiationList *newentry = NULL;
  if (it == map_.end())
  {
    newentry = new DcmExtendedNegotiationList();
    map_.insert(OFPair<OFString, DcmExtendedNegotiationList*>(skey, newentry));
    value = &newentry;
  }
  else
  {
    value = & ((*it).second);
    // check if abstract syntax is already in list
    OFListIterator(DcmExtendedNegotiationItem) first = (*value)->begin();
    OFListIterator(DcmExtendedNegotiationItem) last = (*value)->end();
    while (first != last)
    {
      if ((*first).matches(uid))
      {
        // error: extended negotiation defined twice for one abstract syntax
        OFString s("extended negotiation defined twice for abstract syntax UID: ");
        s += abstractSyntaxUID;
        return makeOFCondition(OFM_dcmnet, 1029, OF_error, s.c_str());
      }
      ++first;
    }
  }

  // insert values into list.
  (*value)->push_back(DcmExtendedNegotiationItem(uid, rawData, length));
  return EC_Normal;
}

OFBool DcmExtendedNegotiationMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;
  if (map_.find(OFString(key)) != map_.end()) return OFTrue;
  return OFFalse;
}

OFCondition DcmExtendedNegotiationMap::checkConsistency(
  const char *key,
  const DcmPresentationContextMap& pclist,
  const char *pckey) const
{
  if ((!key)||(!pckey)) return EC_IllegalCall;

  // DcmExtendedNegotiationList * const *entry = OFconst_cast(DcmExtendedNegotiationList * const *, map_.lookup(OFString(key)));
  DcmExtendedNegotiationList * const *entry = NULL;
  OFMap<OFString, DcmExtendedNegotiationList*>::const_iterator it = map_.find(OFString(key));

  if (it == map_.end())
  {
    // error: key undefined
    OFString s("extended negotiation key undefined: ");
    s += key;
    return makeOFCondition(OFM_dcmnet, 1039, OF_error, s.c_str());
  }

  if (! pclist.isKnownKey(pckey))
  {
    // error: key undefined
    OFString s("presentation context key undefined: ");
    s += pckey;
    return makeOFCondition(OFM_dcmnet, 1040, OF_error, s.c_str());
  }

  // continue with entry found
  entry = &(*it).second;
  if (entry == NULL)
  {
    // error: key undefined
    OFString s("presentation context NULL entry for key: ");
    s += pckey;
    return makeOFCondition(OFM_dcmnet, 1040, OF_error, s.c_str());
  }

  OFListIterator(DcmExtendedNegotiationItem) first = (*entry)->begin();
  OFListIterator(DcmExtendedNegotiationItem) last = (*entry)->end();
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

      return makeOFCondition(OFM_dcmnet, 1041, OF_error, s.c_str());
    }
    ++first;
  }
  return EC_Normal;
}

const DcmExtendedNegotiationList *DcmExtendedNegotiationMap::getExtendedNegotiationList(const char *key) const
{
  const DcmExtendedNegotiationList *result = NULL;
  if (key)
  {
    OFMap<OFString, DcmExtendedNegotiationList*>::const_iterator it = map_.find(OFString(key));
    if (it != map_.end())
      result = (*it).second;
  }
  return result;
}
