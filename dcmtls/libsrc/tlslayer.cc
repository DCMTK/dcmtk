/*
 *
 *  Copyright (C) 2000-2017, OFFIS e.V.
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
 *  Module: dcmnet
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTLSTransportLayer
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmtls/tlsdefin.h"

#ifdef WITH_OPENSSL

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#endif
#include <openssl/rand.h>
#include <openssl/err.h>
END_EXTERN_C

#include "dcmtk/dcmtls/tlslayer.h"
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmnet/dicom.h"

#ifdef HAVE_SSL_CTX_GET0_PARAM
#define DCMTK_SSL_CTX_get0_param SSL_CTX_get0_param
#else
#define DCMTK_SSL_CTX_get0_param(A) A->param;
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#define SSL_CTX_get_cert_store(ctx) ctx->cert_store
#define EVP_PKEY_base_id(key) EVP_PKEY_type(key->type)
#endif

extern "C" int DcmTLSTransportLayer_certificateValidationCallback(int ok, X509_STORE_CTX *storeContext);

OFLogger DCM_dcmtlsLogger = OFLog::getLogger("dcmtk.dcmtls");

int DcmTLSTransportLayer_certificateValidationCallback(int ok, X509_STORE_CTX * /* storeContext */)
{
  // this callback is called whenever OpenSSL has validated a X.509 certificate.
  // we could for example print it:
  // DcmTLSTransportLayer::printX509Certificate(cout, storeContext->cert);
  return ok;
}

/* buf     : buffer to write password into
 * size    : length of buffer in bytes
 * rwflag  : nonzero if the password will be used as a new password, i.e. user should be asked to repeat the password
 * userdata: arbitrary pointer that can be set with SSL_CTX_set_default_passwd_cb_userdata()
 * returns : number of bytes written to password buffer, -1 upon error
 */
extern "C" int DcmTLSTransportLayer_passwordCallback(char *buf, int size, int rwflag, void *userdata);

int DcmTLSTransportLayer_passwordCallback(char *buf, int size, int /* rwflag */, void *userdata)
{
  if (userdata == NULL) return -1;
  OFString *password = OFreinterpret_cast(OFString *, userdata);
  int passwordSize = password->length();
  if (passwordSize > size) passwordSize = size;
  strncpy(buf, password->c_str(), passwordSize);
  return passwordSize;
}


struct DcmCipherSuiteList
{
  const char *TLSname;
  const char *openSSLName;
};

static const DcmCipherSuiteList cipherSuiteList[] =
{
    {"TLS_RSA_WITH_NULL_MD5",                   SSL3_TXT_RSA_NULL_MD5},
    {"TLS_RSA_WITH_NULL_SHA",                   SSL3_TXT_RSA_NULL_SHA},
    {"TLS_RSA_WITH_RC4_128_MD5",                SSL3_TXT_RSA_RC4_128_MD5},
    {"TLS_RSA_WITH_RC4_128_SHA",                SSL3_TXT_RSA_RC4_128_SHA},
    {"TLS_RSA_WITH_IDEA_CBC_SHA",               SSL3_TXT_RSA_IDEA_128_SHA},
    {"TLS_RSA_WITH_DES_CBC_SHA",                SSL3_TXT_RSA_DES_64_CBC_SHA},
    {"TLS_RSA_WITH_3DES_EDE_CBC_SHA",           SSL3_TXT_RSA_DES_192_CBC3_SHA},
    {"TLS_DH_DSS_WITH_DES_CBC_SHA",             SSL3_TXT_DH_DSS_DES_64_CBC_SHA},
    {"TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA",        SSL3_TXT_DH_DSS_DES_192_CBC3_SHA},
    {"TLS_DH_RSA_WITH_DES_CBC_SHA",             SSL3_TXT_DH_RSA_DES_64_CBC_SHA},
    {"TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA",        SSL3_TXT_DH_RSA_DES_192_CBC3_SHA},
    {"TLS_DHE_DSS_WITH_DES_CBC_SHA",            SSL3_TXT_EDH_DSS_DES_64_CBC_SHA},
    {"TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA",       SSL3_TXT_EDH_DSS_DES_192_CBC3_SHA},
    {"TLS_DHE_RSA_WITH_DES_CBC_SHA",            SSL3_TXT_EDH_RSA_DES_64_CBC_SHA},
    {"TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA",       SSL3_TXT_EDH_RSA_DES_192_CBC3_SHA},
    {"TLS_DH_anon_WITH_RC4_128_MD5",            SSL3_TXT_ADH_RC4_128_MD5},
    {"TLS_DH_anon_WITH_DES_CBC_SHA",            SSL3_TXT_ADH_DES_64_CBC_SHA},
    {"TLS_DH_anon_WITH_3DES_EDE_CBC_SHA",       SSL3_TXT_ADH_DES_192_CBC_SHA},
    {"TLS_DHE_DSS_WITH_RC4_128_SHA",            TLS1_TXT_DHE_DSS_WITH_RC4_128_SHA}

#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
    ,
    /* AES ciphersuites from RFC3268 */
    {"TLS_RSA_WITH_AES_128_CBC_SHA",            TLS1_TXT_RSA_WITH_AES_128_SHA},
    {"TLS_DH_DSS_WITH_AES_128_CBC_SHA",         TLS1_TXT_DH_DSS_WITH_AES_128_SHA},
    {"TLS_DH_RSA_WITH_AES_128_CBC_SHA",         TLS1_TXT_DH_RSA_WITH_AES_128_SHA},
    {"TLS_DHE_DSS_WITH_AES_128_CBC_SHA",        TLS1_TXT_DHE_DSS_WITH_AES_128_SHA},
    {"TLS_DHE_RSA_WITH_AES_128_CBC_SHA",        TLS1_TXT_DHE_RSA_WITH_AES_128_SHA},
    {"TLS_DH_anon_WITH_AES_128_CBC_SHA",        TLS1_TXT_ADH_WITH_AES_128_SHA},
    {"TLS_RSA_WITH_AES_256_CBC_SHA",            TLS1_TXT_RSA_WITH_AES_256_SHA},
    {"TLS_DH_DSS_WITH_AES_256_CBC_SHA",         TLS1_TXT_DH_DSS_WITH_AES_256_SHA},
    {"TLS_DH_RSA_WITH_AES_256_CBC_SHA",         TLS1_TXT_DH_RSA_WITH_AES_256_SHA},
    {"TLS_DHE_DSS_WITH_AES_256_CBC_SHA",        TLS1_TXT_DHE_DSS_WITH_AES_256_SHA},
    {"TLS_DHE_RSA_WITH_AES_256_CBC_SHA",        TLS1_TXT_DHE_RSA_WITH_AES_256_SHA},
    {"TLS_DH_anon_WITH_AES_256_CBC_SHA",        TLS1_TXT_ADH_WITH_AES_256_SHA}
#endif

};

unsigned long DcmTLSTransportLayer::getNumberOfCipherSuites()
{
  return sizeof(cipherSuiteList)/sizeof(DcmCipherSuiteList);
}

const char *DcmTLSTransportLayer::getTLSCipherSuiteName(unsigned long idx)
{
  if (idx < sizeof(cipherSuiteList)/sizeof(DcmCipherSuiteList)) return cipherSuiteList[idx].TLSname;
  return NULL;
}

const char *DcmTLSTransportLayer::getOpenSSLCipherSuiteName(unsigned long idx)
{
  if (idx < sizeof(cipherSuiteList)/sizeof(DcmCipherSuiteList)) return cipherSuiteList[idx].openSSLName;
  return NULL;
}

const char *DcmTLSTransportLayer::findOpenSSLCipherSuiteName(const char *tlsCipherSuiteName)
{
  if (tlsCipherSuiteName == NULL) return NULL;
  OFString aString(tlsCipherSuiteName);
  unsigned long numEntries = sizeof(cipherSuiteList)/sizeof(DcmCipherSuiteList);
  for (unsigned long i = 0; i < numEntries; i++)
  {
    if (aString == cipherSuiteList[i].TLSname) return cipherSuiteList[i].openSSLName;
  }
  return NULL;
}

DcmTLSTransportLayer::DcmTLSTransportLayer(int networkRole, const char *randFile, OFBool initializeOpenSSL)
: DcmTransportLayer(networkRole)
, transportLayerContext(NULL)
, canWriteRandseed(OFFalse)
, privateKeyPasswd()
{
   if (initializeOpenSSL)
   {
     // the call to SSL_library_init was not needed in OpenSSL versions prior to 0.9.8,
     // but the API has been available at least since 0.9.5.
     SSL_library_init();
     SSL_load_error_strings();
     OpenSSL_add_all_algorithms();
     seedPRNG(randFile);
   }

#if OPENSSL_VERSION_NUMBER < 0x10100000L
   if (networkRole == DICOM_APPLICATION_ACCEPTOR)
   {
     transportLayerContext = SSL_CTX_new(TLSv1_server_method());
   } else if (networkRole == DICOM_APPLICATION_REQUESTOR) {
     transportLayerContext = SSL_CTX_new(TLSv1_client_method());
   } else {
     transportLayerContext = SSL_CTX_new(TLSv1_method());
   }
#else
   // starting with OpenSSL 1.1.0, a new TLS_method() is offered
   // that automatically selects the highest version of the TLS
   // protocol supported by client and server.
   // The previous TLSv1_methods are now deprecated and generate
   // a warning.
   if (networkRole == DICOM_APPLICATION_ACCEPTOR)
   {
     transportLayerContext = SSL_CTX_new(TLS_server_method());
   } else if (networkRole == DICOM_APPLICATION_REQUESTOR) {
     transportLayerContext = SSL_CTX_new(TLS_client_method());
   } else {
     transportLayerContext = SSL_CTX_new(TLS_method());
   }
#endif

#ifdef DEBUG
   if (transportLayerContext == NULL)
   {
      const char *result = ERR_reason_error_string(ERR_peek_error());
      if (result == NULL) result = "unknown error in SSL_CTX_new()";
      DCMTLS_ERROR("unable to create TLS transport layer: " << result);
   }
#endif

   setCertificateVerification(DCV_requireCertificate); /* default */
}

OFBool DcmTLSTransportLayer::setTempDHParameters(const char *filename)
{
  if ((filename==NULL)||(transportLayerContext==NULL)) return OFFalse;
  DH *dh = NULL;
  BIO *bio = BIO_new_file(filename,"r");
  if (bio)
  {
    dh = PEM_read_bio_DHparams(bio,NULL,NULL,NULL);
    BIO_free(bio);
    if (dh)
    {
      SSL_CTX_set_tmp_dh(transportLayerContext,dh);
      DH_free(dh);
      return OFTrue;
    }
  }
  return OFFalse;
}

void DcmTLSTransportLayer::setPrivateKeyPasswd(const char *thePasswd)
{
  if (thePasswd) privateKeyPasswd = thePasswd;
  else privateKeyPasswd.clear();

  if (transportLayerContext)
  {
    /* register callback that replaces console input */
    SSL_CTX_set_default_passwd_cb(transportLayerContext, DcmTLSTransportLayer_passwordCallback);
    SSL_CTX_set_default_passwd_cb_userdata(transportLayerContext, &privateKeyPasswd);
  }
  return;
}

void DcmTLSTransportLayer::setPrivateKeyPasswdFromConsole()
{
  privateKeyPasswd.clear();
  if (transportLayerContext)
  {
    /* deregister callback that replaces console input */
    SSL_CTX_set_default_passwd_cb(transportLayerContext, NULL);
    SSL_CTX_set_default_passwd_cb_userdata(transportLayerContext, NULL);
  }
  return;
}

void DcmTLSTransportLayer::setCertificateVerification(DcmCertificateVerification verificationType)
{
  if (transportLayerContext)
  {
    int vmode = 0;
    switch (verificationType)
    {
      case DCV_requireCertificate:
        vmode =  SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
        break;
      case DCV_checkCertificate:
        vmode =  SSL_VERIFY_PEER;
        break;
      case DCV_ignoreCertificate:
        break;
    }
    SSL_CTX_set_verify(transportLayerContext, vmode, DcmTLSTransportLayer_certificateValidationCallback);
  }
  return;
}


DcmTransportLayerStatus DcmTLSTransportLayer::setCipherSuites(const char *suites)
{
  if (transportLayerContext && suites)
  {
    if (!SSL_CTX_set_cipher_list(transportLayerContext, suites))
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMTLS_ERROR("OpenSSL error: " << err);
      return TCS_tlsError;
    }
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTLSTransportLayer::~DcmTLSTransportLayer()
{
  if (transportLayerContext) SSL_CTX_free(transportLayerContext);
}

DcmTransportLayerStatus DcmTLSTransportLayer::setPrivateKeyFile(const char *fileName, int fileType)
{
  /* fileType should be SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM */
  if (transportLayerContext)
  {
    if (0 >= SSL_CTX_use_PrivateKey_file(transportLayerContext, fileName, fileType))
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMTLS_ERROR("OpenSSL error: " << err);
      return TCS_tlsError;
    }
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTransportLayerStatus DcmTLSTransportLayer::setCertificateFile(const char *fileName, int fileType)
{
  /* fileType should be SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM */
  if (transportLayerContext)
  {
    if (0 >= SSL_CTX_use_certificate_file(transportLayerContext, fileName, fileType))
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMTLS_ERROR("OpenSSL error: " << err);
      return TCS_tlsError;
    }
  } else return TCS_illegalCall;
  return TCS_ok;
}

OFBool DcmTLSTransportLayer::checkPrivateKeyMatchesCertificate()
{
  if (transportLayerContext)
  {
    if (SSL_CTX_check_private_key(transportLayerContext)) return OFTrue;
  }
  return OFFalse;
}

DcmTransportLayerStatus DcmTLSTransportLayer::addVerificationFlags(unsigned long flags)
{
  X509_VERIFY_PARAM* const parameter = DCMTK_SSL_CTX_get0_param(transportLayerContext);
  return parameter && X509_VERIFY_PARAM_set_flags(parameter,flags) ? TCS_ok : TCS_unspecifiedError;
}

DcmTransportLayerStatus DcmTLSTransportLayer::addTrustedCertificateFile(const char *fileName, int fileType)
{
  /* fileType should be SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM */
  if (transportLayerContext)
  {
    X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(SSL_CTX_get_cert_store(transportLayerContext), X509_LOOKUP_file());
    if (x509_lookup == NULL)
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMTLS_ERROR("OpenSSL error: " << err);
      return TCS_tlsError;
    }
    if (! X509_LOOKUP_load_file(x509_lookup, fileName, fileType))
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMTLS_ERROR("OpenSSL error: " << err);
      return TCS_tlsError;
    }
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTransportLayerStatus DcmTLSTransportLayer::addTrustedCertificateDir(const char *pathName, int fileType)
{
  /* fileType should be SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM */
  if (transportLayerContext)
  {
    X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(SSL_CTX_get_cert_store(transportLayerContext), X509_LOOKUP_hash_dir());
    if (x509_lookup == NULL)
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMTLS_ERROR("OpenSSL error: " << err);
      return TCS_tlsError;
    }
    if (! X509_LOOKUP_add_dir(x509_lookup, pathName, fileType))
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMTLS_ERROR("OpenSSL error: " << err);
      return TCS_tlsError;
    }
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTransportLayerStatus DcmTLSTransportLayer::addTrustedClientCertificateFile(const char *fileName)
{
  if (transportLayerContext)
  {
    STACK_OF(X509_NAME) *caNames = sk_X509_NAME_dup(SSL_CTX_get_client_CA_list(transportLayerContext));
    if (caNames == NULL)
      caNames = sk_X509_NAME_new_null();
    STACK_OF(X509_NAME) *newCaNames = SSL_load_client_CA_file(fileName);
    for (int i = 0; i < sk_X509_NAME_num(newCaNames); ++i)
    {
      X509_NAME *newCaName = sk_X509_NAME_value(newCaNames,i);
      if (sk_X509_NAME_find(caNames,newCaName) == -1)
        sk_X509_NAME_push(caNames,X509_NAME_dup(newCaName));
    }
    sk_X509_NAME_pop_free(newCaNames,X509_NAME_free);
    SSL_CTX_set_client_CA_list(transportLayerContext,caNames);
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTransportConnection *DcmTLSTransportLayer::createConnection(DcmNativeSocketType openSocket, OFBool useSecureLayer)
{
  if (useSecureLayer)
  {
    if (transportLayerContext)
    {
      SSL *newConnection = SSL_new(transportLayerContext);
      if (newConnection)
      {
        SSL_set_fd(newConnection, openSocket);
        return new DcmTLSConnection(openSocket, newConnection);
      }
    }
    return NULL;
  }
  else return new DcmTCPConnection(openSocket);
}

void DcmTLSTransportLayer::seedPRNG(const char *randFile)
{
#ifdef _WIN32
  RAND_screen();
#endif
  if (randFile)
  {
#ifdef HAVE_RAND_EGD
    if (RAND_egd(randFile) <= 0)
#endif
    {
      RAND_load_file(randFile ,-1);
    }
  }
  if (RAND_status()) canWriteRandseed = OFTrue;
  else
  {
    /* warn user */
    DCMTLS_WARN("PRNG for TLS not seeded with sufficient random data.");
  }
}

void DcmTLSTransportLayer::addPRNGseed(void *buf, size_t bufSize)
{
  RAND_seed(buf,OFstatic_cast(int, bufSize));
}

OFBool DcmTLSTransportLayer::writeRandomSeed(const char *randFile)
{
  if (canWriteRandseed && randFile)
  {
    if (RAND_write_file(randFile)) return OFTrue;
  }
  return OFFalse;
}

OFString DcmTLSTransportLayer::dumpX509Certificate(X509 *peerCertificate)
{
  if (peerCertificate)
  {
    long certVersion = 0;                     /* certificate type */
    long certSerialNumber = -1;               /* certificate serial number */
    OFString certValidNotBefore;              /* certificate validity - not before */
    OFString certValidNotAfter;               /* certificate validity - not after */
    char certSubjectName[1024];               /* certificate subject name (DN) */
    char certIssuerName[1024];                /* certificate issuer name (DN) */
    const char *certPubKeyType = "unknown";   /* certificate public key type */
    int certPubKeyBits = 0;                   /* certificate number of bits in public key */
    certSubjectName[0]= '\0';
    certIssuerName[0]= '\0';
    certVersion = X509_get_version(peerCertificate) +1;
    certSerialNumber = ASN1_INTEGER_get(X509_get_serialNumber(peerCertificate));
    BIO *certValidNotBeforeBIO = BIO_new(BIO_s_mem());
    char *bufptr = NULL;
    if (certValidNotBeforeBIO)
    {
      ASN1_UTCTIME_print(certValidNotBeforeBIO, X509_get_notBefore(peerCertificate));
      BIO_write(certValidNotBeforeBIO,"\0",1);
      BIO_get_mem_data(certValidNotBeforeBIO, OFreinterpret_cast(char *, &bufptr));
      if (bufptr) certValidNotBefore = bufptr;
      BIO_free(certValidNotBeforeBIO);
    }
    bufptr = NULL;
    BIO *certValidNotAfterBIO  = BIO_new(BIO_s_mem());
    if (certValidNotAfterBIO)
    {
      ASN1_UTCTIME_print(certValidNotAfterBIO, X509_get_notAfter(peerCertificate));
      BIO_write(certValidNotAfterBIO,"\0",1);
      BIO_get_mem_data(certValidNotAfterBIO, OFreinterpret_cast(char *, &bufptr));
      if (bufptr) certValidNotAfter = bufptr;
      BIO_free(certValidNotAfterBIO);
    }
    X509_NAME_oneline(X509_get_subject_name(peerCertificate), certSubjectName, 1024);
    X509_NAME_oneline(X509_get_issuer_name(peerCertificate), certIssuerName, 1024);
    EVP_PKEY *pubkey = X509_get_pubkey(peerCertificate); // creates copy of public key
    if (pubkey)
    {
      switch (EVP_PKEY_base_id(pubkey))
      {
        case EVP_PKEY_RSA:
          certPubKeyType = "RSA";
          break;
        case EVP_PKEY_DSA:
          certPubKeyType = "DSA";
          break;
        case EVP_PKEY_DH:
          certPubKeyType = "DH";
          break;
        default:
          /* nothing */
          break;
      }
      certPubKeyBits = EVP_PKEY_bits(pubkey);
      EVP_PKEY_free(pubkey);
    }
    OFOStringStream out;
    out << "X.509v" << certVersion << " Certificate" << OFendl
        << "  Subject     : " << certSubjectName << OFendl
        << "  Issued by   : " << certIssuerName << OFendl
        << "  Serial no.  : " << certSerialNumber << OFendl
        << "  Validity    : not before " << certValidNotBefore << ", not after " << certValidNotAfter << OFendl
        << "  Public key  : " << certPubKeyType << ", " << certPubKeyBits << " bits" << OFStringStream_ends;
    OFSTRINGSTREAM_GETOFSTRING(out, ret)
    return ret;
  } else {
    return "No X.509 Certificate.";
  }
}

DcmTLSTransportLayer::native_handle_type DcmTLSTransportLayer::getNativeHandle()
{
  return transportLayerContext;
}

#else  /* WITH_OPENSSL */

/* make sure that the object file is not completely empty if compiled
 * without OpenSSL because some linkers might fail otherwise.
 */
DCMTK_DCMTLS_EXPORT void tlslayer_dummy_function()
{
  return;
}

#endif /* WITH_OPENSSL */
