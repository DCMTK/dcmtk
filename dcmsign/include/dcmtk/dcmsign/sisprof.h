/*
 *
 *  Copyright (C) 1998-2011, OFFIS e.V.
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

#ifndef SISPROF_H
#define SISPROF_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sitypes.h"  /* for E_KeyType */

#ifdef WITH_OPENSSL

#include "dcmtk/dcmdata/dcxfer.h"   /* for E_TransferSyntax */

class SiAlgorithm;
class DcmItem;
class DcmAttributeTag;
class SiMAC;
class DcmTagKey;

/** abstract base class for all security profiles.
 */
class DCMTK_DCMSIGN_EXPORT SiSecurityProfile
{
public:

  /// default constructor
  SiSecurityProfile() { }

  /// destructor
  virtual ~SiSecurityProfile() { }
  
  /** checks whether the given MAC type can be used with this security profile.
   *  @param macType MAC type to be checked
   *  @return true if MAC type is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableMACType(E_MACType macType) const = 0;

  /** checks whether the given MAC object can be used with this security profile.
   *  @param macType object to be checked
   *  @return true if object is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableMAC(const SiMAC& mac) const;

  /** checks whether the given public/private key algorithm can be used with this security profile.
   *  @param keyType public key algorithm type to be checked
   *  @return true if public key algorithm is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableAlgorithmType(E_KeyType keyType) const = 0;
  
  /** checks whether the given public/private key object can be used with this security profile.
   *  @param algo object to be checked
   *  @return true if object is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableAlgorithm(const SiAlgorithm& algo) const;

  /** checks whether the given transfer syntax can be used with this security profile
   *  @param xfer transfer syntax to be checked
   *  @return true if transfer syntax is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableTransferSyntax(E_TransferSyntax xfer) const = 0;
  
  /** checks whether an attribute with the given tag is required to be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if required, false otherwise.
   */
  virtual OFBool attributeRequired(const DcmTagKey& key) const = 0;

  /** checks whether an attribute with the given tag must not be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if attribute must not be signed, false otherwise.
   */
  virtual OFBool attributeForbidden(const DcmTagKey& key) const = 0;
  
  /** updates the given list of attribute tags according to the
   *  requirements of the current security profile. For all elements present in the
   *  dataset, the attribute tag is inserted or removed from the list if required by the profile.
   *  @param item dataset to be handled
   *  @param tagList attribute tag list to be updated
   *  @return status code
   */
  virtual OFCondition updateAttributeList(DcmItem &item, DcmAttributeTag& tagList);

  /** checks whether the given list of attribute tags fulfils the requirements
   *  of the current security profile for the given dataset. 
   *  @param item dataset to be checked
   *  @param tagList attribute tag list. 
   *  @return true if minimum requirements for profile are fulfilled, false otherwise.
   */ 
  virtual OFBool checkAttributeList(DcmItem &item, DcmAttributeTag& tagList);

  /** checks if the given tag key is contained in the given list.
   *  @param tagList list of tag keys
   *  @param key tag key
   *  @return true if tag key is present in list, false otherwise.
   */
  static OFBool containsTag(DcmAttributeTag& tagList, const DcmTagKey& key);

private:

  /// private undefined copy constructor
  SiSecurityProfile(SiSecurityProfile& arg);

  /// private undefined copy assignment operator
  SiSecurityProfile& operator=(SiSecurityProfile& arg);

};

#endif
#endif
