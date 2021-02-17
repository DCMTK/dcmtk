/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *    classes: SiBaseRSAProfile
 *
 */

#ifndef SIBRSAPR_H
#define SIBRSAPR_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisprof.h"   /* for SiSecurityProfile */

/** Base RSA Digital Signature Profile
 *  @remark This class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiBaseRSAProfile: public SiSecurityProfile
{
public:

  /// default constructor
  SiBaseRSAProfile() { }

  /// destructor
  virtual ~SiBaseRSAProfile() { }

  /** checks whether the given MAC type can be used with this security profile.
   *  @param macType MAC type to be checked
   *  @return true if MAC type is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableMACType(E_MACType macType) const;

  /** checks whether the given public/private key algorithm can be used with this security profile.
   *  @param keyType public key algorithm type to be checked
   *  @return true if public key algorithm is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableAlgorithmType(E_KeyType keyType) const;

  /** checks whether the given transfer syntax can be used with this security profile
   *  @param xfer transfer syntax to be checked
   *  @return true if transfer syntax is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableTransferSyntax(E_TransferSyntax xfer) const;

  /** checks whether an attribute with the given tag is required to be signed
   *  for the current security profile if the attribute is present in the dataset
   *  @param key tag key to be checked
   *  @return true if required, false otherwise.
   */
  virtual OFBool attributeRequiredIfPresent(const DcmTagKey& key) const;

  /** checks whether all attributes that are required unconditionally
   *  to be signed in this profile are included in the given tagList.
   *  @param taglist attribute tag list
   *  @return true if requirements for profile are fulfilled, false otherwise.
   */
  virtual OFBool checkRequiredAttributeList(DcmAttributeTag& tagList) const;

  /** checks whether an attribute with the given tag must not be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if attribute must not be signed, false otherwise.
   */
  virtual OFBool attributeForbidden(const DcmTagKey& key) const;

  /** some digital signature profiles specify conditions under which certain
   *  attributes must be included into the signature.
   *  This method allows the signature profile to inspect the dataset in order
   *  to determine whether or not the conditions are met.
   *  This method should be called before DcmSignature::createSignature() is executed.
   *  @param item the dataset or item to which the signature will be added
   *  @return status code
   */
  virtual OFCondition inspectSignatureDataset(DcmItem &item);

  /** returns true if this signature profile only applies to main dataset level
   *  @return OFTrue if this signature profile only applies to main dataset level, OFFalse otherwise
   */
  virtual OFBool mainDatasetRequired() const;

};

#endif
#endif
