/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *  Module: dcmsign
 *
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiSecurityProfile
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:56 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sisprof.h"
#include "dcitem.h"    /* for DcmItem */
#include "dcvrat.h"    /* for DcmAttributeTag */
#include "simac.h"     /* for SiMAC */
#include "sialgo.h"    /* for SiAlgorithm */

OFBool SiSecurityProfile::isAllowableMAC(const SiMAC& mac) const
{
  return isAllowableMACType(mac.macType());
}


OFBool SiSecurityProfile::isAllowableAlgorithm(const SiAlgorithm& algo) const
{
  return isAllowableAlgorithmType(algo.keyType());
}


SI_E_Condition SiSecurityProfile::updateAttributeList(DcmItem &item, DcmAttributeTag& tagList)
{
  SI_E_Condition result = SI_EC_Normal;
  unsigned long card = item.card();
  if (card == 0)
  {
    // nothing to sign
    tagList.clear();
    return result;
  }
  unsigned long maxArray = 2*card;
  Uint16 *array = new Uint16[maxArray];
  if (array == NULL) return SI_EC_MemoryExhausted;
  unsigned long i=0;  
  for (i=0; i<maxArray; i++) array[i]=0;
  DcmElement *elem = NULL;

  // for all elements in the dataset, check if we want to have them in the list  
  for (i=0; i<card; i++)
  {
    elem = item.getElement(i);
    const DcmTagKey& key = elem->getTag();
    if (key.isSignable())
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
    if (EC_Normal != tagList.putUint16Array(array, j>>1)) result = SI_EC_DcmDataFailure;
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
    if (key.isSignable())
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


OFBool SiSecurityProfile::checkAttributeList(DcmItem &item)
{
  DcmElement *elem = NULL;
  unsigned long card = item.card();
  for (unsigned long i=0; i<card; i++)
  {
    elem = item.getElement(i);
    if ((elem->getTag().isSignable())&&(attributeForbidden(elem->getTag()))) return OFFalse;
  }
  return OFTrue;
}  


OFBool SiSecurityProfile::containsTag(DcmAttributeTag& tagList, const DcmTagKey& key)
{
  unsigned long vm = tagList.getVM();
  DcmTagKey current;
  for (unsigned long i=0; i<vm; ++i)
  {
    if ((EC_Normal == tagList.getTagVal(current, i)) && (key == current)) return OFTrue;
  }
  return OFFalse;
}

OFBool SiSecurityProfile::isUniversalAttributeList(DcmItem &item, DcmAttributeTag& tagList)
{
  DcmElement *elem = NULL;
  unsigned long card = item.card();
  for (unsigned long i=0; i<card; i++)
  {
    elem = item.getElement(i);
    const DcmTagKey& key = elem->getTag();
    if ((key.isSignable()) && (!containsTag(tagList, key))) return OFFalse;
  }
  return OFTrue;
}

#else /* WITH_OPENSSL */

const int sisprof_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sisprof.cc,v $
 *  Revision 1.2  2001-06-01 15:50:56  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:08  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

