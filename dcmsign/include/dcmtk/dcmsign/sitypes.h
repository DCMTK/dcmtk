/*
 *
 *  Copyright (C) 1998-2020, OFFIS e.V.
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
 */

#ifndef SITYPES_H
#define SITYPES_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmsign/sidefine.h"
#include "dcmtk/dcmsign/sitypes.h"

/*
** Logging
*/

extern DCMTK_DCMSIGN_EXPORT OFLogger dcmsignLogger;

#define DCMSIGN_TRACE(msg) OFLOG_TRACE(dcmsignLogger, msg)
#define DCMSIGN_DEBUG(msg) OFLOG_DEBUG(dcmsignLogger, msg)
#define DCMSIGN_INFO(msg)  OFLOG_INFO(dcmsignLogger, msg)
#define DCMSIGN_WARN(msg)  OFLOG_WARN(dcmsignLogger, msg)
#define DCMSIGN_ERROR(msg) OFLOG_ERROR(dcmsignLogger, msg)
#define DCMSIGN_FATAL(msg) OFLOG_FATAL(dcmsignLogger, msg)


// DICOM defined terms for MAC algorithms, certificate and timestamp types
#define SI_DEFTERMS_RIPEMD160 "RIPEMD160"
#define SI_DEFTERMS_SHA1      "SHA1"
#define SI_DEFTERMS_MD5       "MD5"
#define SI_DEFTERMS_X509CERT  "X509_1993_SIG"
#define SI_DEFTERMS_CMS_TS    "CMS_TS"
#define SI_DEFTERMS_SHA256    "SHA256"
#define SI_DEFTERMS_SHA384    "SHA384"
#define SI_DEFTERMS_SHA512    "SHA512"

// include this file in doxygen documentation

/** @file sitypes.h
 *  @brief type definitions and constants for the dcmsign module
 */


/** type of key for public key cryptosystem
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum E_KeyType
{
  /// RSA key
  EKT_RSA,

  /// DSA key
  EKT_DSA,

  /// DH key
  EKT_DH,

  /// EC key
  EKT_EC,

  /// no key present
  EKT_none
};


/** type of MAC algorithm
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum E_MACType
{
  /// SHA-1
  EMT_SHA1,

  /// RIPEMD160
  EMT_RIPEMD160,

  /// MD5
  EMT_MD5,

  /// SHA-256
  EMT_SHA256,

  /// SHA-384
  EMT_SHA384,

  /// SHA-512
  EMT_SHA512
};

/** signature verification policy
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum E_SignatureVerificationPolicy
{
  /// verify signatures if present, pass otherwise
  ESVP_verifyIfPresent,

  /// fail if no signature is present at all but do not check any signature profile
  ESVP_requireSignature,

  /// fail if no valid creator RSA signature is present on the main dataset level
  ESVP_requireCreatorRSASignature,

  /// fail if no valid authorization RSA signature is present on the main dataset level
  ESVP_requireAuthorizationRSASignature,

  /// fail if no valid SR RSA signature is present on the main dataset level
  ESVP_requireSRRSASignature
};

/** verification policy for certified timestamps attached to signatures
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum E_TimestampVerificationPolicy
{
  /// verify timestamp if present, pass otherwise
  ETVP_verifyTSIfPresent,

  /// ignore certified timestamp even if present
  ETVP_ignoreTS,

  /// fail if signature does not contain a certified timestamp
  ETVP_requireTS,

};


/*
 * specific error conditions for module dcmsign
 */

/// object initialization failed
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_InitializationFailed;

/// an OpenSSL call has failed
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_OpenSSLFailure;

/// file cannot be read
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_CannotRead;

/// unable to use the selected transfer syntax for MAC computation
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_WrongTransferSyntax;

/// no more MAC ID numbers available
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_MacIDsExhausted;

/// certificate and private key do not match
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_CertificateDoesNotMatchPrivateKey;

/// MAC algorithm not allowed for the current security profile
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_MacDoesNotMatchProfile;

/// Signature algorithm not allowed for the current security profile
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_AlgorithmDoesNotMatchProfile;

/// Transfer syntax not allowed for the current security profile
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_TransferSyntaxDoesNotMatchProfile;

/// Dataset is not suitable for the current security profile (e.g. wrong SOP class)
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_DatasetDoesNotMatchProfile;

/// signature verification failed because the certificate is missing or cannot be read (e.g. unsupported format)
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoCertificate;

/// signature verification failed because the corresponding MAC parameters item could not be found or is incomplete
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoMAC;

/// signature verification failed because the corresponding signature item is incomplete
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoSignature;

/// signature verification failed because the MAC algorithm is not supported
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_UnsupportedMACAlgorithm;

/// signature verification failed because the signature is invalid (document corrupted)
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_Corrupted;

/// signature verification failed because the certificate was issued by an untrusted (unknown) CA
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoTrust;

/// unsupported MAC algorithm specified
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_UnsupportedMAC;

/// invalid object identifier (OID) string
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_InvalidOID;

/// unable to write time stamp query file
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_CannotWriteTSQ;

/// verification of timestamp response message failed
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_InvalidTSR;

/// signature verification failed because DataElementsSigned is missing or incorrect
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoDataElementsSigned;

/// List of data elements signed does not match the profile requirements
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_DataElementsSignedDoesNotMatchProfile;

/// desired signature location item not found
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_ItemLocationNotFound;

/// unknown certified timestamp type
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_UnknownTimestampType;

/// certified timestamp in dataset cannot be read
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_InvalidTimestamp;

/// filetype is unknown (neither PEM nor DER)
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_InvalidFiletype;

/// signature verification of the certified timestamp failed
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_TimestampSignatureVerificationFailed;

/// signature verification failed because the certificate was already expired at the signature create date
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_CertExpiredAtSignature;

/// signature verification failed because the certificate was not yet valid at signature creation date
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_CertNotYetValidAtSig;

/// list of attributes to be signed contains attribute that is not signable
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_AttributeNotSignable;

/// signature verification failed because the signature contains an attribute that is not signable
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_AttributeNotSignable;

/// selected dataset or item is empty, nothing to sign
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_DatasetEmpty;

/// cannot create signature for current signature profile: required attributes missing
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_RequiredAttributeMissing;

/// cannot handle ECDSA signatures because OpenSSL was compiled without elliptic curve support
/// @remark this constant is only available if DCMTK is compiled with OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_EllipticCurveNotSupported;

#endif
#endif
