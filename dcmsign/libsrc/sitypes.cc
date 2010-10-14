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
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitypes.h"
#include "dcmtk/dcmdata/dcerror.h" /* for OFM_dcmsign */

const OFConditionConst SI_ECC_InitializationFailed(                       OFM_dcmsign,  1, OF_error, "object initialization failed");
const OFConditionConst SI_ECC_OpenSSLFailure(                             OFM_dcmsign,  2, OF_error, "an OpenSSL call has failed");
const OFConditionConst SI_ECC_CannotRead(                                 OFM_dcmsign,  3, OF_error, "file cannot be read");
const OFConditionConst SI_ECC_WrongTransferSyntax(                        OFM_dcmsign,  4, OF_error, "unable to use the selected transfer syntax for MAC computation");
const OFConditionConst SI_ECC_MacIDsExhausted(                            OFM_dcmsign,  5, OF_error, "no more MAC ID numbers available");
const OFConditionConst SI_ECC_CertificateDoesNotMatchPrivateKey(          OFM_dcmsign,  6, OF_error, "certificate and private key do not match");
const OFConditionConst SI_ECC_MacDoesNotMatchProfile(                     OFM_dcmsign,  7, OF_error, "MAC algorithm not allowed for the selected security profile");
const OFConditionConst SI_ECC_AlgorithmDoesNotMatchProfile(               OFM_dcmsign,  8, OF_error, "signature algorithm not allowed for the selected security profile");
const OFConditionConst SI_ECC_TransferSyntaxDoesNotMatchProfile(          OFM_dcmsign,  9, OF_error, "transfer syntax not allowed for the selected security profile");
const OFConditionConst SI_ECC_VerificationFailed_NoCertificate(           OFM_dcmsign, 10, OF_error, "signature verification failed: certificate is missing or unreadable");
const OFConditionConst SI_ECC_VerificationFailed_NoMAC(                   OFM_dcmsign, 11, OF_error, "signature verification failed: MAC parameters not be found or incomplete");
const OFConditionConst SI_ECC_VerificationFailed_NoSignature(             OFM_dcmsign, 12, OF_error, "signature verification failed: signature item incomplete");
const OFConditionConst SI_ECC_VerificationFailed_UnsupportedMACAlgorithm( OFM_dcmsign, 13, OF_error, "signature verification failed: the MAC algorithm not supported");
const OFConditionConst SI_ECC_VerificationFailed_Corrupted(               OFM_dcmsign, 14, OF_error, "signature verification failed: signature is invalid (document corrupted)");
const OFConditionConst SI_ECC_VerificationFailed_NoTrust(                 OFM_dcmsign, 16, OF_error, "signature verification failed: certificate issued by unknown CA");

const OFCondition SI_EC_InitializationFailed(                       SI_ECC_InitializationFailed);
const OFCondition SI_EC_OpenSSLFailure(                             SI_ECC_OpenSSLFailure);
const OFCondition SI_EC_CannotRead(                                 SI_ECC_CannotRead);
const OFCondition SI_EC_WrongTransferSyntax(                        SI_ECC_WrongTransferSyntax);
const OFCondition SI_EC_MacIDsExhausted(                            SI_ECC_MacIDsExhausted);
const OFCondition SI_EC_CertificateDoesNotMatchPrivateKey(          SI_ECC_CertificateDoesNotMatchPrivateKey);
const OFCondition SI_EC_MacDoesNotMatchProfile(                     SI_ECC_MacDoesNotMatchProfile);
const OFCondition SI_EC_AlgorithmDoesNotMatchProfile(               SI_ECC_AlgorithmDoesNotMatchProfile);
const OFCondition SI_EC_TransferSyntaxDoesNotMatchProfile(          SI_ECC_TransferSyntaxDoesNotMatchProfile);
const OFCondition SI_EC_VerificationFailed_NoCertificate(           SI_ECC_VerificationFailed_NoCertificate);
const OFCondition SI_EC_VerificationFailed_NoMAC(                   SI_ECC_VerificationFailed_NoMAC);
const OFCondition SI_EC_VerificationFailed_NoSignature(             SI_ECC_VerificationFailed_NoSignature);
const OFCondition SI_EC_VerificationFailed_UnsupportedMACAlgorithm( SI_ECC_VerificationFailed_UnsupportedMACAlgorithm);
const OFCondition SI_EC_VerificationFailed_Corrupted(               SI_ECC_VerificationFailed_Corrupted);
const OFCondition SI_EC_VerificationFailed_NoTrust(                 SI_ECC_VerificationFailed_NoTrust);



#else /* WITH_OPENSSL */

int sitypes_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sitypes.cc,v $
 *  Revision 1.7  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.6  2005-12-08 15:47:32  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2002/12/16 12:57:54  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.4  2001/09/26 14:30:27  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:50:56  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2001/01/25 15:11:48  meichel
 *  Added class SiCertificateVerifier in dcmsign which allows to check
 *    whether a certificate from a digital signature is trusted, i.e. issued
 *    by a known CA and not contained in a CRL.
 *
 *  Revision 1.1  2000/11/07 16:49:09  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

