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
 *    classes: SiRSA
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sirsa.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/siprivat.h"

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


OFCondition SiRSA::sign(
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
    return EC_Normal;    
}


OFCondition SiRSA::verify(
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
    return EC_Normal;
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

int sirsa_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sirsa.cc,v $
 *  Revision 1.7  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.6  2005-12-08 15:47:29  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2002/12/16 12:57:53  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.4  2001/09/26 14:30:26  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:50:55  meichel
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

