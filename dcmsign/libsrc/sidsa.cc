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
 *    classes: SiDSA
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:53 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sidsa.h"
#include "sicert.h"
#include "siprivat.h"

BEGIN_EXTERN_C
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/dsa.h>
END_EXTERN_C

SiDSA::SiDSA(DSA *key)
: dsa(key)
{
}


SiDSA::~SiDSA()
{
  if (dsa) DSA_free(dsa);
}


SI_E_Condition SiDSA::sign(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    unsigned char *outputSignature,
    unsigned long &outputSignatureSize)
{
    if (dsa==NULL) return SI_EC_InitializationFailed;    
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
    int error = DSA_sign(openSSLmac, inputHash, (unsigned int)inputHashSize, outputSignature, &sigLen, dsa);
    outputSignatureSize = sigLen;    
    if (error < 0) return SI_EC_OpenSSLFailure;
    return SI_EC_Normal;    
}


SI_E_Condition SiDSA::verify(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    const unsigned char *inputSignature,
    unsigned long inputSignatureSize,
    OFBool &verified)
{
    verified = OFFalse;
    if (dsa==NULL) return SI_EC_InitializationFailed;
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

    // we have to cast away const on inputSignature yet because of OpenSSL limitations
    int error = DSA_verify(openSSLmac, inputHash, (unsigned int)inputHashSize, (unsigned char *)inputSignature, (unsigned int)inputSignatureSize, dsa);
    if (error < 0) return SI_EC_OpenSSLFailure; else if (error > 0) verified = OFTrue;
    return SI_EC_Normal;
}


unsigned long SiDSA::getSize() const
{   
  if (dsa == NULL) return 0;
  return DSA_size(dsa);
}


E_KeyType SiDSA::keyType() const
{
  return EKT_DSA;
}

#else /* WITH_OPENSSL */

const int sidsa_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sidsa.cc,v $
 *  Revision 1.3  2001-06-01 15:50:53  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/07 18:07:08  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.1  2000/11/07 16:49:04  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

