/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiSecurityProfile
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisprof.h"
#include "dcmtk/dcmdata/dcitem.h"    /* for DcmItem */
#include "dcmtk/dcmdata/dcvrat.h"    /* for DcmAttributeTag */
#include "dcmtk/dcmsign/simac.h"     /* for SiMAC */
#include "dcmtk/dcmsign/sialgo.h"    /* for SiAlgorithm */

OFBool SiSecurityProfile::isAllowableMAC(const SiMAC& mac) const
{
  return isAllowableMACType(mac.macType());
}


OFBool SiSecurityProfile::isAllowableAlgorithm(const SiAlgorithm& algo) const
{
  return isAllowableAlgorithmType(algo.keyType());
}


OFCondition SiSecurityProfile::updateAttributeList(DcmItem &item, DcmAttributeTag& tagList)
{
  OFCondition result = EC_Normal;
  unsigned long card = item.card();
  if (card == 0)
  {
    // nothing to sign
    tagList.clear();
    return result;
  }
  unsigned long maxArray = 2*card;
  Uint16 *array = new Uint16[maxArray];
  if (array == NULL) return EC_MemoryExhausted;
  unsigned long i=0;  
  for (i=0; i<maxArray; i++) array[i]=0;
  DcmElement *elem = NULL;

  // for all elements in the dataset, check if we want to have them in the list  
  for (i=0; i<card; i++)
  {
    elem = item.getElement(i);
    const DcmTagKey& key = elem->getTag();
    if (key.isSignableTag())
    {
      if ((attributeRequired(key))||((containsTag(tagList, key))&&(! attributeForbidden(key))))
      {
      	array[2*i] = key.getGroup();
      	array[2*i+1] = key.getElement();
      }
    }
  }

  // pack array
  unsigned long j=0;
  i = 0;
  while (i < maxArray)
  {
    if (array[i] > 0)
    {
      array [j++] = array[i++];
      array [j++] = array[i++];
    } else i += 2;
  }
  
  // now copy nonzero entries from array to tagList
  tagList.clear();
  if (j > 0)
  {
    result = tagList.putUint16Array(array, j>>1);
  }
  return result;
}


OFBool SiSecurityProfile::checkAttributeList(DcmItem &item, DcmAttributeTag& tagList)
{
  DcmElement *elem = NULL;
  unsigned long card = item.card();
  for (unsigned long i=0; i<card; i++)
  {
    elem = item.getElement(i);
    const DcmTagKey& key = elem->getTag();
    if (key.isSignableTag())
    {
      if (containsTag(tagList, key))
      {
        if (attributeForbidden(key)) return OFFalse; // attribute is signed but forbidden
      } else {
        if (attributeRequired(key)) return OFFalse;  // attribute is required but unsigned
      }
    } else {
      if (containsTag(tagList, key)) return OFFalse; // unsignable tag contained in list
    }
  }
  return OFTrue;
}


OFBool SiSecurityProfile::containsTag(DcmAttributeTag& tagList, const DcmTagKey& key)
{
  unsigned long vm = tagList.getVM();
  DcmTagKey current;
  for (unsigned long i=0; i<vm; ++i)
  {
    if ((tagList.getTagVal(current, i)).good() && (key == current)) return OFTrue;
  }
  return OFFalse;
}


#else /* WITH_OPENSSL */

int sisprof_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
