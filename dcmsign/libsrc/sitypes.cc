/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *    consts, typedefs and enums for dcmsign
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-01-25 15:11:48 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#include "sitypes.h"

const char *siErrorConditionToString(SI_E_Condition cond) 
{
  switch (cond)
  {
    case SI_EC_Normal:
      return "no error";
      /* break; */
    case SI_EC_InitializationFailed:
      return "object initialization failed";
      /* break; */
    case SI_EC_IllegalCall:
      return "invalid parameter passed";
      /* break; */
    case SI_EC_OpenSSLFailure:
      return "an OpenSSL call has failed";
      /* break; */
    case SI_EC_CannotRead:
      return "file cannot be read";
      /* break; */
    case SI_EC_MemoryExhausted:
      return "unable to allocate memory";
      /* break; */
    case SI_EC_DcmDataFailure:
      return "a dcmdata call has failed";
      /* break; */
    case SI_EC_WrongTransferSyntax:
      return "unable to use the selected transfer syntax for MAC computation";
      /* break; */
    case SI_EC_MacIDsExhausted:
      return "no more MAC ID numbers available";
      /* break; */
    case SI_EC_CertificateDoesNotMatchPrivateKey:
      return "certificate and private key do not match";
      /* break; */
    case SI_EC_MacDoesNotMatchProfile:
      return "MAC algorithm not allowed for the selected security profile";
      /* break; */
    case SI_EC_AlgorithmDoesNotMatchProfile:
      return "signature algorithm not allowed for the selected security profile";
      /* break; */
    case SI_EC_TransferSyntaxDoesNotMatchProfile:
      return "transfer syntax not allowed for the selected security profile";
      /* break; */
    case SI_EC_VerificationFailed_NoCertificate:
      return "signature verification failed: certificate is missing or unreadable";
      /* break; */
    case SI_EC_VerificationFailed_NoMAC:
      return "signature verification failed: MAC parameters not be found or incomplete";
      /* break; */
    case SI_EC_VerificationFailed_NoSignature:
      return "signature verification failed: signature item incomplete";
      /* break; */
    case SI_EC_VerificationFailed_UnsupportedMACAlgorithm:
      return "signature verification failed: the MAC algorithm not supported";
      /* break; */
    case SI_EC_VerificationFailed_Corrupted:
      return "signature verification failed: signature is invalid (document corrupted)";
      /* break; */
    case SI_EC_VerificationFailed_NoTrust:
      return "signature verification failed: certificate issued by unknown CA";
      /* break; */
  }
  return "unknown dcmsign error";
}

#else /* WITH_OPENSSL */

const int sitypes_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sitypes.cc,v $
 *  Revision 1.2  2001-01-25 15:11:48  meichel
 *  Added class SiCertificateVerifier in dcmsign which allows to check
 *    whether a certificate from a digital signature is trusted, i.e. issued
 *    by a known CA and not contained in a CRL.
 *
 *  Revision 1.1  2000/11/07 16:49:09  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

