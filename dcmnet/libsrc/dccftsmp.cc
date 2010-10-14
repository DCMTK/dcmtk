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
 *    class DcmTransferSyntaxMap
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
#include "dcmtk/dcmnet/dccftsmp.h" /* for class DcmTransferSyntaxMap */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */

DcmTransferSyntaxMap::DcmTransferSyntaxMap()
: map_()
{
}

DcmTransferSyntaxMap::~DcmTransferSyntaxMap()
{
  OFListIterator(DcmKeyValuePair<DcmTransferSyntaxList *> *) first = map_.begin();
  OFListIterator(DcmKeyValuePair<DcmTransferSyntaxList *> *) last = map_.end();
  while (first != last)
  {
    delete (*first)->value();
    ++first;
  }  
}

OFCondition DcmTransferSyntaxMap::add(
  const char *key,
  const char *transferSyntaxUID)
{
  if ((!key)||(!transferSyntaxUID)) return EC_IllegalCall;

  // perform syntax check of UID
  DcmUIDHandler uid(transferSyntaxUID);
  if (! uid.isValidUID())
  {
    OFString s("not a valid transfer syntax UID: ");
    s += transferSyntaxUID;
    return makeOFCondition(OFM_dcmnet, 1024, OF_error, s.c_str());
  }

  OFString skey(key);
  DcmTransferSyntaxList * const *value = OFconst_cast(DcmTransferSyntaxList * const *, map_.lookup(skey));
  if (value == NULL)
  {
    DcmTransferSyntaxList *newentry = new DcmTransferSyntaxList();
    map_.add(skey, OFstatic_cast(DcmTransferSyntaxList *, newentry));
    value = &newentry;
  }

  // insert UID into list.
  (*value)->push_back(uid);
  return EC_Normal;  
}

OFBool DcmTransferSyntaxMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;
  if (map_.lookup(OFString(key))) return OFTrue;
  return OFFalse;
}

const DcmTransferSyntaxList *DcmTransferSyntaxMap::getTransferSyntaxList(const char *key) const
{
  const DcmTransferSyntaxList *result = NULL;
  if (key)
  {
    DcmTransferSyntaxList * const *value = OFconst_cast(DcmTransferSyntaxList * const *, map_.lookup(OFString(key)));
    if (value) result = *value;
  }
  return result;
}


/*
 * CVS/RCS Log
 * $Log: dccftsmp.cc,v $
 * Revision 1.5  2010-10-14 13:14:28  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2005/12/08 15:44:32  meichel
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
