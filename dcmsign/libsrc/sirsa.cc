/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *    classes: SiRSA
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:55 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sirsa.h"
#include "sicert.h"
#include "siprivat.h"

BEGIN_EXTERN_C
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>  /* for RSA */
END_EXTERN_C

SiRSA::SiRSA(RSA *key)
: rsa(key)
{
}

SiRSA::~SiRSA()
{
  if (rsa) RSA_free(rsa);
}


SI_E_Condition SiRSA::sign(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    unsigned char *outputSignature,
    unsigned long &outputSignatureSize)
{
    if (rsa==NULL) return SI_EC_InitializationFailed;    
    int openSSLmac = 0;
    switch (inputHashAlgorithm)
    {
      case EMT_SHA1:
        openSSLmac = NID_sha1;
        break;
      case EMT_RIPEMD160:
        openSSLmac = NID_ripemd160;
        break;
      case EMT_MD5:
        openSSLmac = NID_md5;
        break;
    }
    unsigned int sigLen = 0;
    // we have to cast away const on inputHash yet because of OpenSSL limitations
    int error = RSA_sign(openSSLmac, (unsigned char *)inputHash, (unsigned int)inputHashSize, outputSignature, &sigLen, rsa);
    outputSignatureSize = sigLen;    
    if (error < 0) return SI_EC_OpenSSLFailure;
    return SI_EC_Normal;    
}


SI_E_Condition SiRSA::verify(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    const unsigned char *inputSignature,
    unsigned long inputSignatureSize,
    OFBool &verified)
{
    verified = OFFalse;
    if (rsa==NULL) return SI_EC_InitializationFailed;    

    int openSSLmac = 0;
    switch (inputHashAlgorithm)
    {
      case EMT_SHA1:
        openSSLmac = NID_sha1;
        break;
      case EMT_RIPEMD160:
        openSSLmac = NID_ripemd160;
        break;
      case EMT_MD5:
        openSSLmac = NID_md5;
        break;
    }
    // we have to cast away const on inputHash yet because of OpenSSL limitations
    // we have to cast away const on inputSignature yet because of OpenSSL limitations
    int error = RSA_verify(openSSLmac, (unsigned char *)inputHash, (unsigned int)inputHashSize, (unsigned char *)inputSignature, (unsigned int)inputSignatureSize, rsa);
    if (error < 0) return SI_EC_OpenSSLFailure; else if (error > 0) verified = OFTrue;
    return SI_EC_Normal;
}

unsigned long SiRSA::getSize() const
{   
  if (rsa == NULL) return 0;
  return RSA_size(rsa);
}


E_KeyType SiRSA::keyType() const
{
  return EKT_RSA;
}

#else /* WITH_OPENSSL */

const int sirsa_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sirsa.cc,v $
 *  Revision 1.3  2001-06-01 15:50:55  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/07 18:07:09  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.1  2000/11/07 16:49:07  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

