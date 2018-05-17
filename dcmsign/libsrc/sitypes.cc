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

OFLogger DCM_dcmsignLogger = OFLog::getLogger("dcmtk.dcmsign");

#else /* WITH_OPENSSL */

int sitypes_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
