/*
 *
 *  Copyright (C) 1994-2004, OFFIS
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
 *    class DcmRoleSelectionItem
 *    class DcmRoleSelectionMap
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2004-05-05 12:57:58 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dccfrsmp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dccfrsmp.h" /* for class DcmRoleSelectionMap */
#include "dccfpcmp.h" /* for class DcmPresentationContextMap */
#include "dcerror.h"  /* for EC_IllegalCall */

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
  DcmRoleSelectionList * const *value = map_.lookup(skey);
  if (value == NULL)
  {
    DcmRoleSelectionList *newentry = new DcmRoleSelectionList();
    map_.add(key, newentry);
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

  DcmRoleSelectionList * const *entry = map_.lookup(OFString(key));
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
    DcmRoleSelectionList * const *value = map_.lookup(OFString(key));
    if (value) result = *value;
  }
  return result;
}


/*
 * CVS/RCS Log
 * $Log: dccfrsmp.cc,v $
 * Revision 1.2  2004-05-05 12:57:58  meichel
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
