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
 *    class DcmExtendedNegotiationItem
 *    class DcmExtendedNegotiationMap
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:28 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

DcmExtendedNegotiationMap::~DcmExtendedNegotiationMap()
{
  OFListIterator(DcmKeyValuePair<DcmExtendedNegotiationList *> *) first = map_.begin();
  OFListIterator(DcmKeyValuePair<DcmExtendedNegotiationList *> *) last = map_.end();
  while (first != last)
  {
    delete (*first)->value();
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
  DcmExtendedNegotiationList * const *value = OFconst_cast(DcmExtendedNegotiationList * const *, map_.lookup(skey));
  if (value == NULL)
  {
    DcmExtendedNegotiationList *newentry = new DcmExtendedNegotiationList();
    map_.add(skey, OFstatic_cast(DcmExtendedNegotiationList *, newentry));
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

  DcmExtendedNegotiationList * const *entry = OFconst_cast(DcmExtendedNegotiationList * const *, map_.lookup(OFString(key)));
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
    DcmExtendedNegotiationList * const *value = OFconst_cast(DcmExtendedNegotiationList * const *, map_.lookup(OFString(key)));
    if (value) result = *value;
  }
  return result;
}


/*
 * CVS/RCS Log
 * $Log: dccfenmp.cc,v $
 * Revision 1.8  2010-10-14 13:14:28  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2009-09-28 13:28:15  joergr
 * Moved general purpose definition file from module dcmdata to ofstd, and
 * added new defines in order to make the usage easier.
 *
 * Revision 1.6  2005/12/08 15:44:28  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2004/05/06 16:36:30  joergr
 * Added typecasts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.4  2004/05/05 12:57:58  meichel
 * Simplified template class DcmSimpleMap<T>, needed for Sun CC 2.0.1
 *
 * Revision 1.3  2004/04/14 11:59:50  joergr
 * Added explicit type cast to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.2  2003/06/18 08:16:17  meichel
 * Added comparison operators to keep MSVC5 compiler happy
 *
 * Revision 1.1  2003/06/10 14:30:15  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
