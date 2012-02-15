/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-02-15 14:50:43 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
makeOFConditionConst(SI_EC_VerificationFailed_UnsupportedMACAlgorithm, OFM_dcmsign, 13, OF_error, "signature verification failed: the MAC algorithm not supported");
makeOFConditionConst(SI_EC_VerificationFailed_Corrupted,               OFM_dcmsign, 14, OF_error, "signature verification failed: signature is invalid (document corrupted)");
makeOFConditionConst(SI_EC_VerificationFailed_NoTrust,                 OFM_dcmsign, 16, OF_error, "signature verification failed: certificate issued by unknown CA");


#else /* WITH_OPENSSL */

int sitypes_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sitypes.cc,v $
 *  Revision 1.8  2012-02-15 14:50:43  uli
 *  Removed dependency on static initialization order from OFCondition.
 *  All static condition objects are now created via makeOFConditionConst()
 *  in a way that doesn't need a constructor to run. This should only break
 *  code which defines its own condition objects, all other changes are
 *  backwards compatible.
 *
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

