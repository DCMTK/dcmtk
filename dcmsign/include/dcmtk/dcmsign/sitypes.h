/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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

/*
** Logging
*/

extern DCMTK_DCMSIGN_EXPORT OFLogger DCM_dcmsignLogger;

#define DCMSIGN_TRACE(msg) OFLOG_TRACE(DCM_dcmsignLogger, msg)
#define DCMSIGN_DEBUG(msg) OFLOG_DEBUG(DCM_dcmsignLogger, msg)
#define DCMSIGN_INFO(msg)  OFLOG_INFO(DCM_dcmsignLogger, msg)
#define DCMSIGN_WARN(msg)  OFLOG_WARN(DCM_dcmsignLogger, msg)
#define DCMSIGN_ERROR(msg) OFLOG_ERROR(DCM_dcmsignLogger, msg)
#define DCMSIGN_FATAL(msg) OFLOG_FATAL(DCM_dcmsignLogger, msg)


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

/*
 * specific error conditions for module dcmsign
 */

/// object initialization failed
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_InitializationFailed;

/// an OpenSSL call has failed
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_OpenSSLFailure;

/// file cannot be read
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_CannotRead;

/// unable to use the selected transfer syntax for MAC computation
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_WrongTransferSyntax;

/// no more MAC ID numbers available
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_MacIDsExhausted;

/// certificate and private key do not match
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_CertificateDoesNotMatchPrivateKey;

/// MAC algorithm not allowed for the current security profile
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_MacDoesNotMatchProfile;

/// Signature algorithm not allowed for the current security profile
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_AlgorithmDoesNotMatchProfile;

/// Transfer syntax not allowed for the current security profile
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_TransferSyntaxDoesNotMatchProfile;

/** signature verification failed because the certificate is missing
 *  or cannot be read (e.g. unsupported format)
 *  @remark this constant is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoCertificate;

/// signature verification failed because the corresponding MAC parameters item could not be found or is incomplete
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoMAC;

/// signature verification failed because the corresponding signature item is incomplete
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoSignature;

/// signature verification failed because the MAC algorithm is not supported
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_UnsupportedMACAlgorithm;

/// signature verification failed because the signature is invalid (document corrupted)
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_Corrupted;

/// signature verification failed because the certificate was issued by an untrusted (unknown) CA
/// @remark this constant is only available if DCMTK is compiled with
/// OpenSSL support enabled.
extern DCMTK_DCMSIGN_EXPORT const OFConditionConst SI_EC_VerificationFailed_NoTrust;

#endif
#endif
