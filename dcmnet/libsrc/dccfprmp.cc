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
 *    class DcmProfileEntry
 *    class DcmProfileMap
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-10 14:30:15 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dccfprmp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dccfprmp.h" /* for class DcmProfileMap */
#include "dcerror.h"  /* for EC_IllegalCall */


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
  OFListIterator(DcmKeyValuePair<DcmProfileEntry *>) first = map_.begin();
  OFListIterator(DcmKeyValuePair<DcmProfileEntry *>) last = map_.end();
  while (first != last)
  {
    delete (*first).value();
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
  DcmProfileEntry * const *value = map_.lookup(skey);
  if (value == NULL)
  {
    DcmProfileEntry *newentry = new DcmProfileEntry(presKey, roleKey, extnegKey);
    map_.add(key, newentry);
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
    DcmProfileEntry * const *entry = map_.lookup(OFString(key));
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
    DcmProfileEntry * const *entry = map_.lookup(OFString(key));
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
    DcmProfileEntry * const *entry = map_.lookup(OFString(key));
    if (entry)
    {
      return (*entry)->getExtendedNegotiationKey();
    }
  }
  return NULL;
}


/*
 * CVS/RCS Log
 * $Log: dccfprmp.cc,v $
 * Revision 1.1  2003-06-10 14:30:15  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
