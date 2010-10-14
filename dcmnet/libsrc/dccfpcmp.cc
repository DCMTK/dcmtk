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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:28 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*
 * CVS/RCS Log
 * $Log: dccfpcmp.cc,v $
 * Revision 1.5  2010-10-14 13:14:28  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2005/12/08 15:44:29  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2004/05/06 16:36:30  joergr
 * Added typecasts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.2  2004/05/05 12:57:58  meichel
 * Simplified template class DcmSimpleMap<T>, needed for Sun CC 2.0.1
 *
 * Revision 1.1  2003/06/10 14:30:15  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
