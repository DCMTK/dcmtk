/*
 *
 *  Copyright (C) 1994-2003, OFFIS
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmExtendedNegotiationItem
 *    class DcmExtendedNegotiationMap
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-10 14:30:15 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dccfenmp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dccfenmp.h" /* for class DcmExtendedNegotiationMap */
#include "dccfpcmp.h" /* for class DcmPresentationContextMap */
#include "dcerror.h"  /* for EC_IllegalCall */


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
    (void) memcpy(raw_, data, length_);
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
    (void) memcpy(raw_, arg.raw_, length_);
  }
}

DcmExtendedNegotiationItem::~DcmExtendedNegotiationItem()
{
  delete[] raw_;
}

/* ========================================================= */

DcmExtendedNegotiationMap::DcmExtendedNegotiationMap()
: map_()
{
}

DcmExtendedNegotiationMap::~DcmExtendedNegotiationMap()
{
  OFListIterator(DcmKeyValuePair<DcmExtendedNegotiationList *>) first = map_.begin();
  OFListIterator(DcmKeyValuePair<DcmExtendedNegotiationList *>) last = map_.end();
  while (first != last)
  {
    delete (*first).value();
    ++first;
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
  DcmExtendedNegotiationList * const *value = map_.lookup(skey);
  if (value == NULL)
  {
    DcmExtendedNegotiationList *newentry = new DcmExtendedNegotiationList();
    map_.add(key, newentry);
    value = &newentry;
  }
  else
  {
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
  if (map_.lookup(OFString(key))) return OFTrue;
  return OFFalse;
}

OFCondition DcmExtendedNegotiationMap::checkConsistency(
  const char *key, 
  const DcmPresentationContextMap& pclist,
  const char *pckey) const
{
  if ((!key)||(!pckey)) return EC_IllegalCall;

  DcmExtendedNegotiationList * const *entry = map_.lookup(OFString(key));
  if (!entry)
  {
    // error: key undefined
    OFString s("extended negotation key undefined: ");
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
    DcmExtendedNegotiationList * const *value = map_.lookup(OFString(key));
    if (value) result = *value;
  }
  return result;
}


/*
 * CVS/RCS Log
 * $Log: dccfenmp.cc,v $
 * Revision 1.1  2003-06-10 14:30:15  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
