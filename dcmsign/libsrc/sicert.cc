/*
 *
 *  Copyright (C) 1998-2016, OFFIS e.V.
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
 *  Author: Norbert Loxen
 *
 *  Purpose:
 *    classes: SiCertificate
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/sirsa.h"   /* for class SiRSA */
#include "dcmtk/dcmsign/sidsa.h"   /* for class SiDSA */
#include "dcmtk/dcmdata/dcstack.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcdeftag.h"

BEGIN_EXTERN_C
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
END_EXTERN_C

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#define EVP_PKEY_id(key) key->type
#endif

SiCertificate::SiCertificate()
: x509(NULL)
{
}

SiCertificate::~SiCertificate()
{
  if (x509) X509_free(x509);	
}

E_KeyType SiCertificate::getKeyType()
{
  E_KeyType result = EKT_none;
  if (x509)
  {
    EVP_PKEY *pkey = X509_extract_key(x509);
    if (pkey)
    {
      switch(EVP_PKEY_id(pkey))
      {
        case EVP_PKEY_RSA:
          result = EKT_RSA;
          break;
        case EVP_PKEY_DSA:
          result = EKT_DSA;
          break;
        case EVP_PKEY_DH:
          result = EKT_DH;
          break;
        default:
          /* nothing */
          break;
      }
      EVP_PKEY_free(pkey);
    }    
  }
  return result;
}

SiAlgorithm *SiCertificate::createAlgorithmForPublicKey()
{
  if (x509)
  {
    EVP_PKEY *pkey = X509_extract_key(x509);
    if (pkey)
    {
      switch(EVP_PKEY_id(pkey))
      {
        case EVP_PKEY_RSA:
          return new SiRSA(EVP_PKEY_get1_RSA(pkey));
          /* break; */
        case EVP_PKEY_DSA:
          return new SiDSA(EVP_PKEY_get1_DSA(pkey));
          /* break; */
        case EVP_PKEY_DH:
        default:
          /* nothing */
          break;
      }
      EVP_PKEY_free(pkey);
    }    
  }
  return NULL;
}

OFCondition SiCertificate::loadCertificate(const char *filename, int filetype)
{
  OFCondition result = SI_EC_CannotRead;  
  if (x509) X509_free(x509);	
  x509 = NULL;
  if (filename)
  {
    BIO *in = BIO_new(BIO_s_file());
    if (in)
    {
      if (BIO_read_filename(in, filename) > 0)
      {
        if (filetype == X509_FILETYPE_ASN1)
        {
          x509 = d2i_X509_bio(in, NULL);
          if (x509) result = EC_Normal;
        } else {
          x509 = PEM_read_bio_X509(in, NULL, NULL, NULL);
          if (x509) result = EC_Normal;
        }
      }
      BIO_free(in);
    }
  }
  return result;
}


OFCondition SiCertificate::read(DcmItem& item)
{
  OFCondition result = EC_Normal;
  OFString aString;
  DcmStack stack;
  result = item.search(DCM_CertificateType, stack, ESM_fromHere, OFFalse);
  if (result.good())
  {
    result = ((DcmElement *)(stack.top()))->getOFString(aString, 0);
    if (result.good())
    {
      if (aString == SI_DEFTERMS_X509CERT)
      {
      	stack.clear();
      	result = item.search(DCM_CertificateOfSigner, stack, ESM_fromHere, OFFalse);
        if (result.good())
        {
          DcmElement *cert = (DcmElement *)stack.top();
          Uint8 *data = NULL;
          result = cert->getUint8Array(data);
          if (result.good())
          {
            if (data)
            {
#if OPENSSL_VERSION_NUMBER >= 0x00908000L
              // incompatible API change in OpenSSL 0.9.8
              const Uint8 *cdata = data;
              x509 = d2i_X509(NULL, &cdata, cert->getLength());
#else
              x509 = d2i_X509(NULL, &data, cert->getLength());
#endif
              if (x509 == NULL) result = EC_IllegalCall;              
            } else result = EC_IllegalCall;            
          }      
        } 
      } else result = EC_IllegalCall;
    }
  }
  return result;
}

OFCondition SiCertificate::write(DcmItem& item)
{
  if (x509 == NULL) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  DcmElement *elem = new DcmCodeString(DCM_CertificateType);
  if (elem)
  {
    result = elem->putString(SI_DEFTERMS_X509CERT);
    if (result.good()) item.insert(elem, OFTrue); else delete elem;
  } else result = EC_MemoryExhausted;
  if (result.good())
  {
    elem = new DcmOtherByteOtherWord(DCM_CertificateOfSigner);
    if (elem)
    {
      int certLength = i2d_X509(x509, NULL);
      unsigned char *data = new unsigned char[certLength];
      if (data)
      {
        unsigned char *data2 = data;
        i2d_X509(x509, &data2); // data2 now points to the last element of the byte array
        result = elem->putUint8Array((Uint8 *)data, certLength);
        delete[] data;
        if (result.good()) item.insert(elem, OFTrue); else delete elem;
      } else {
        delete elem;
        result = EC_MemoryExhausted;
      }
    } else result = EC_MemoryExhausted;
  }
  return result;
}

X509 *SiCertificate::getRawCertificate()
{
  return x509;
}

long SiCertificate::getX509Version()
{
  if (x509 == NULL) return 0;
  return X509_get_version(x509)+1;
}

void SiCertificate::getCertSubjectName(OFString& str)
{
  if (x509)
  {
    char certSubjectName[2048];  /* certificate subject name (DN) */
    certSubjectName[0]= '\0';
    X509_NAME_oneline(X509_get_subject_name(x509), certSubjectName, 2048);
    str = certSubjectName;
  } else str.clear();
}

void SiCertificate::getCertIssuerName(OFString& str)
{
  if (x509)
  {
    char certIssuerName[2048];  /* certificate issuer name (DN) */
    certIssuerName[0]= '\0';
    X509_NAME_oneline(X509_get_issuer_name(x509), certIssuerName, 2048);
    str = certIssuerName;
  } else str.clear();
}

long SiCertificate::getCertSerialNo()
{
  if (x509 == NULL) return -1;
  return ASN1_INTEGER_get(X509_get_serialNumber(x509));
}

void SiCertificate::getCertValidityNotBefore(OFString& str)
{
  str.clear();
  if (x509)
  {
    char *bufptr = NULL;
    BIO *certValidNotBeforeBIO = BIO_new(BIO_s_mem());
    if (certValidNotBeforeBIO)
    {
      ASN1_UTCTIME_print(certValidNotBeforeBIO, X509_get_notBefore(x509));
      BIO_write(certValidNotBeforeBIO,"\0",1);
      BIO_get_mem_data(certValidNotBeforeBIO, (char *)(&bufptr));
      if (bufptr) str = bufptr;
      BIO_free(certValidNotBeforeBIO);
    }
  }
}

void SiCertificate::getCertValidityNotAfter(OFString& str)
{
  str.clear();
  if (x509)
  {
    char *bufptr = NULL;
    BIO *certValidNotAfterBIO = BIO_new(BIO_s_mem());
    if (certValidNotAfterBIO)
    {
      ASN1_UTCTIME_print(certValidNotAfterBIO, X509_get_notAfter(x509));
      BIO_write(certValidNotAfterBIO,"\0",1);
      BIO_get_mem_data(certValidNotAfterBIO, (char *)(&bufptr));
      if (bufptr) str = bufptr;
      BIO_free(certValidNotAfterBIO);
    }
  }
}

long SiCertificate::getCertKeyBits()
{
  long certPubKeyBits = 0;                   /* certificate number of bits in public key */
  if (x509)
  {
    EVP_PKEY *pubkey = X509_get_pubkey(x509); // creates copy of public key
    if (pubkey)
    {
      certPubKeyBits = EVP_PKEY_bits(pubkey);
      EVP_PKEY_free(pubkey);
    }
  }
  return certPubKeyBits;
}

#else /* WITH_OPENSSL */

int sicert_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
