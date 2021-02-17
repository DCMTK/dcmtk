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
 *    consts, typedefs and enums for dcmsign
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitypes.h"
#include "dcmtk/dcmdata/dcerror.h" /* for OFM_dcmsign */

makeOFConditionConst(SI_EC_InitializationFailed,                       OFM_dcmsign,  1, OF_error, "object initialization failed");
makeOFConditionConst(SI_EC_OpenSSLFailure,                             OFM_dcmsign,  2, OF_error, "an OpenSSL call has failed");
makeOFConditionConst(SI_EC_CannotRead,                                 OFM_dcmsign,  3, OF_error, "file cannot be read");
makeOFConditionConst(SI_EC_WrongTransferSyntax,                        OFM_dcmsign,  4, OF_error, "unable to use the selected transfer syntax for MAC computation");
makeOFConditionConst(SI_EC_MacIDsExhausted,                            OFM_dcmsign,  5, OF_error, "no more MAC ID numbers available");
makeOFConditionConst(SI_EC_CertificateDoesNotMatchPrivateKey,          OFM_dcmsign,  6, OF_error, "certificate and private key do not match");
makeOFConditionConst(SI_EC_MacDoesNotMatchProfile,                     OFM_dcmsign,  7, OF_error, "MAC algorithm not allowed for the selected security profile");
makeOFConditionConst(SI_EC_AlgorithmDoesNotMatchProfile,               OFM_dcmsign,  8, OF_error, "signature algorithm not allowed for the selected security profile");
makeOFConditionConst(SI_EC_TransferSyntaxDoesNotMatchProfile,          OFM_dcmsign,  9, OF_error, "transfer syntax not allowed for the selected security profile");
makeOFConditionConst(SI_EC_VerificationFailed_NoCertificate,           OFM_dcmsign, 10, OF_error, "signature verification failed: certificate is missing or unreadable");
makeOFConditionConst(SI_EC_VerificationFailed_NoMAC,                   OFM_dcmsign, 11, OF_error, "signature verification failed: MAC parameters not be found or incomplete");
makeOFConditionConst(SI_EC_VerificationFailed_NoSignature,             OFM_dcmsign, 12, OF_error, "signature verification failed: signature item incomplete");
makeOFConditionConst(SI_EC_VerificationFailed_UnsupportedMACAlgorithm, OFM_dcmsign, 13, OF_error, "signature verification failed: MAC algorithm not supported");
makeOFConditionConst(SI_EC_VerificationFailed_Corrupted,               OFM_dcmsign, 14, OF_error, "signature verification failed: signature is invalid (document corrupted)");
makeOFConditionConst(SI_EC_VerificationFailed_NoTrust,                 OFM_dcmsign, 16, OF_error, "signature verification failed: certificate issued by unknown CA");
makeOFConditionConst(SI_EC_DatasetDoesNotMatchProfile,                 OFM_dcmsign, 17, OF_error, "dataset not suitable for the selected security profile");
makeOFConditionConst(SI_EC_UnsupportedMAC,                             OFM_dcmsign, 18, OF_error, "unsupported MAC algorithm selected");
makeOFConditionConst(SI_EC_InvalidOID,                                 OFM_dcmsign, 19, OF_error, "invalid object identifier (OID) string");
makeOFConditionConst(SI_EC_CannotWriteTSQ,                             OFM_dcmsign, 20, OF_error, "unable to write time stamp query file");
makeOFConditionConst(SI_EC_InvalidTSR,                                 OFM_dcmsign, 21, OF_error, "verification of timestamp response message failed");
makeOFConditionConst(SI_EC_VerificationFailed_NoDataElementsSigned,    OFM_dcmsign, 22, OF_error, "signature verification failed: DataElementsSigned missing or incorrect");
makeOFConditionConst(SI_EC_DataElementsSignedDoesNotMatchProfile,      OFM_dcmsign, 23, OF_error, "list of data elements signed does not match profile requirements");
makeOFConditionConst(SI_EC_ItemLocationNotFound,                       OFM_dcmsign, 24, OF_error, "desired signature location not found");
makeOFConditionConst(SI_EC_UnknownTimestampType,                       OFM_dcmsign, 25, OF_error, "unknown certified timestamp type");
makeOFConditionConst(SI_EC_InvalidTimestamp,                           OFM_dcmsign, 26, OF_error, "certified timestamp in dataset cannot be read");
makeOFConditionConst(SI_EC_InvalidFiletype,                            OFM_dcmsign, 27, OF_error, "filetype is unknown (neither PEM nor DER)");
makeOFConditionConst(SI_EC_TimestampSignatureVerificationFailed,       OFM_dcmsign, 28, OF_error, "signature verification of the certified timestamp failed");
makeOFConditionConst(SI_EC_VerificationFailed_CertExpiredAtSignature,  OFM_dcmsign, 29, OF_error, "signature verification failed: certificate was expired at signature creation date");
makeOFConditionConst(SI_EC_VerificationFailed_CertNotYetValidAtSig,    OFM_dcmsign, 30, OF_error, "signature verification failed: certificate was not yet valid at signature creation date");
makeOFConditionConst(SI_EC_AttributeNotSignable,                       OFM_dcmsign, 31, OF_error, "list of attributes to be signed contains attribute that is not signable");
makeOFConditionConst(SI_EC_VerificationFailed_AttributeNotSignable,    OFM_dcmsign, 32, OF_error, "signature verification failed: signature contains attribute that is not signable");
makeOFConditionConst(SI_EC_DatasetEmpty,                               OFM_dcmsign, 33, OF_error, "selected dataset or item is empty, nothing to sign");
makeOFConditionConst(SI_EC_RequiredAttributeMissing,                   OFM_dcmsign, 34, OF_error, "cannot create signature for current signature profile: required attributes missing");
makeOFConditionConst(SI_EC_EllipticCurveNotSupported,                  OFM_dcmsign, 35, OF_error, "cannot handle ECDSA signatures because OpenSSL was compiled without elliptic curve support");

OFLogger dcmsignLogger = OFLog::getLogger("dcmtk.dcmsign");

#else /* WITH_OPENSSL */

int sitypes_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
