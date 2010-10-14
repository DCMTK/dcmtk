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
 *    consts, typedefs and enums for dcmsign
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:25 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SITYPES_H
#define SITYPES_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcond.h"

// DICOM defined terms for MAC algorithms, certificate and timestamp types
#define SI_DEFTERMS_RIPEMD160 "RIPEMD160"
#define SI_DEFTERMS_SHA1      "SHA1"
#define SI_DEFTERMS_MD5       "MD5"
#define SI_DEFTERMS_X509CERT  "X509_1993_SIG"
#define SI_DEFTERMS_CMS_TS    "CMS_TS"


/** type of key for public key cryptosystem
 */
enum E_KeyType
{
  /// RSA key
  EKT_RSA,

  /// DSA key
  EKT_DSA,

  /// DH key
  EKT_DH,
  
  /// no key present
  EKT_none
};


/** type of MAC algorithm
 */
enum E_MACType
{
  /// SHA-1
  EMT_SHA1,
  
  /// RIPEMD160
  EMT_RIPEMD160,
  
  /// MD5
  EMT_MD5
};


/*
 * specific error conditions for module dcmsign
 */
 
/// object initialization failed
extern const OFCondition SI_EC_InitializationFailed;

/// an OpenSSL call has failed
extern const OFCondition SI_EC_OpenSSLFailure;

/// file cannot be read
extern const OFCondition SI_EC_CannotRead;

/// unable to use the selected transfer syntax for MAC computation
extern const OFCondition SI_EC_WrongTransferSyntax;

/// no more MAC ID numbers available
extern const OFCondition SI_EC_MacIDsExhausted;

/// certificate and private key do not match
extern const OFCondition SI_EC_CertificateDoesNotMatchPrivateKey;

/// MAC algorithm not allowed for the current security profile
extern const OFCondition SI_EC_MacDoesNotMatchProfile;

/// Signature algorithm not allowed for the current security profile
extern const OFCondition SI_EC_AlgorithmDoesNotMatchProfile;

/// Transfer syntax not allowed for the current security profile
extern const OFCondition SI_EC_TransferSyntaxDoesNotMatchProfile;

/** signature verification failed because the certificate is missing
 *  or cannot be read (e.g. unsupported format)
 */
extern const OFCondition SI_EC_VerificationFailed_NoCertificate;

/// signature verification failed because the corresponding MAC parameters item could not be found or is incomplete
extern const OFCondition SI_EC_VerificationFailed_NoMAC;

/// signature verification failed because the corresponding signature item is incomplete
extern const OFCondition SI_EC_VerificationFailed_NoSignature;

/// signature verification failed because the MAC algorithm is not supported
extern const OFCondition SI_EC_VerificationFailed_UnsupportedMACAlgorithm;

/// signature verification failed because the signature is invalid (document corrupted)
extern const OFCondition SI_EC_VerificationFailed_Corrupted;

/// signature verification failed because the certificate was issued by an untrusted (unknown) CA
extern const OFCondition SI_EC_VerificationFailed_NoTrust;

#endif
#endif

/*
 *  $Log: sitypes.h,v $
 *  Revision 1.7  2010-10-14 13:17:25  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.6  2005-12-08 16:04:47  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2001/11/16 15:50:51  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.4  2001/09/26 14:30:23  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:50:51  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2001/01/25 15:11:44  meichel
 *  Added class SiCertificateVerifier in dcmsign which allows to check
 *    whether a certificate from a digital signature is trusted, i.e. issued
 *    by a known CA and not contained in a CRL.
 *
 *  Revision 1.1  2000/11/07 16:49:00  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

