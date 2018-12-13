/*
 *
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Module: dcmtls
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTLSCiphersuiteHandler
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmtls/tlsciphr.h"
#include "dcmtk/dcmtls/tlsdefin.h"
#include "dcmtk/dcmtls/tlslayer.h"    /* for TLS_ERROR macro */

#ifdef WITH_OPENSSL

BEGIN_EXTERN_C
#include <openssl/ssl.h>
#include <openssl/tls1.h>
END_EXTERN_C

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
#define SSL_CTX_get_ciphers(ctx) (ctx)->cipher_list
#endif

/* POD struct for the list of supported ciphersuite
 */
struct DcmCipherSuiteList
{
  const char *TLSname;
  const char *openSSLName;
  DcmTLSCipherProtocolVersion protocolVersion;
  DcmTLSCipherKeyExchange keyExchange;
  DcmTLSCipherAuthentication authentication;
  DcmTLSCipherEncryption encryption;
  DcmTLSCipherMAC mac;
  size_t keySize;
  size_t effectiveKeySize;
};


/* This is a subset of the list of ciphersuites supported by OpenSSL 1.0.1 and newer.
 * This list only contains ciphersuites that offer an acceptable level of security,
 * plus the "historic" ciphersuites for older TLS profiles (Basic, AES, IHE ATNA NULL).
 *
 * IMPORTANT: This list must be sorted from the weakest to the strongest ciphersuite.
 *   - first sort by availability of forward secrecy (RSA < ECDH < ECDHE < DHE)
 *   - then sort by effective key size
 *   - then sort by hash key algorithm  (SHA-1 < SHA-256 < SHA-384); (CBC < AEAD)
 */
static const DcmCipherSuiteList globalCipherSuiteList[] =
{
    // -- first all ciphersuites without forward secrecy --

    // historic ciphersuites
    {"TLS_RSA_WITH_NULL_SHA",                         SSL3_TXT_RSA_NULL_SHA,                           TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_None,     TCM_SHA1,     0,   0},
    {"TLS_RSA_WITH_3DES_EDE_CBC_SHA",                 SSL3_TXT_RSA_DES_192_CBC3_SHA,                   TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_3DES,     TCM_SHA1,   168, 112},

    // 128 bit encryption with static RSA
    {"TLS_RSA_WITH_AES_128_CBC_SHA",                  TLS1_TXT_RSA_WITH_AES_128_SHA,                   TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA1,   128, 128},
    {"TLS_RSA_WITH_CAMELLIA_128_CBC_SHA",             TLS1_TXT_RSA_WITH_CAMELLIA_128_CBC_SHA,          TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_Camellia, TCM_SHA1,   128, 128},
    {"TLS_RSA_WITH_AES_128_CBC_SHA256",               TLS1_TXT_RSA_WITH_AES_128_SHA256,                TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA256, 128, 128},
    {"TLS_RSA_WITH_AES_128_GCM_SHA256",               TLS1_TXT_RSA_WITH_AES_128_GCM_SHA256,            TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AESGCM,   TCM_AEAD,   128, 128},

    // 128 bit encryption with ECDH
    {"TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA",           TLS1_TXT_ECDH_ECDSA_WITH_AES_128_CBC_SHA,        TPV_SSLv3,  TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA1,   128, 128},
    {"TLS_ECDH_RSA_WITH_AES_128_CBC_SHA",             TLS1_TXT_ECDH_RSA_WITH_AES_128_CBC_SHA,          TPV_SSLv3,  TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA1,   128, 128},
    {"TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256",        TLS1_TXT_ECDH_ECDSA_WITH_AES_128_SHA256,         TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA256, 128, 128},
    {"TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256",          TLS1_TXT_ECDH_RSA_WITH_AES_128_SHA256,           TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA256, 128, 128},
    {"TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256",        TLS1_TXT_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,     TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,   128, 128},
    {"TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256",          TLS1_TXT_ECDH_RSA_WITH_AES_128_GCM_SHA256,       TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,   128, 128},

    // 256 bit encryption with static RSA
    {"TLS_RSA_WITH_CAMELLIA_256_CBC_SHA",             TLS1_TXT_RSA_WITH_CAMELLIA_256_CBC_SHA,          TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_Camellia, TCM_SHA1,   256, 256},
    {"TLS_RSA_WITH_AES_256_CBC_SHA",                  TLS1_TXT_RSA_WITH_AES_256_SHA,                   TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA1,   256, 256},
    {"TLS_RSA_WITH_AES_256_CBC_SHA256",               TLS1_TXT_RSA_WITH_AES_256_SHA256,                TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA256, 256, 256},
    {"TLS_RSA_WITH_AES_256_GCM_SHA384",               TLS1_TXT_RSA_WITH_AES_256_GCM_SHA384,            TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AESGCM,   TCM_AEAD,   256, 256},

    // 256 bit encryption with static ECDH
    {"TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA",           TLS1_TXT_ECDH_ECDSA_WITH_AES_256_CBC_SHA,        TPV_SSLv3,  TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA1,   256, 256},
    {"TLS_ECDH_RSA_WITH_AES_256_CBC_SHA",             TLS1_TXT_ECDH_RSA_WITH_AES_256_CBC_SHA,          TPV_SSLv3,  TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA1,   256, 256},
    {"TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384",        TLS1_TXT_ECDH_ECDSA_WITH_AES_256_SHA384,         TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA384, 256, 256},
    {"TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384",          TLS1_TXT_ECDH_RSA_WITH_AES_256_SHA384,           TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA384, 256, 256},
    {"TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384",        TLS1_TXT_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,     TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,   256, 256},
    {"TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384",          TLS1_TXT_ECDH_RSA_WITH_AES_256_GCM_SHA384,       TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,   256, 256},

    // -- then all ciphersuites with forward secrecy --

    // 128 bit encryption with ECDHE
    {"TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA",          TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,       TPV_SSLv3,  TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA1,   128, 128},
    {"TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA",            TLS1_TXT_ECDHE_RSA_WITH_AES_128_CBC_SHA,         TPV_SSLv3,  TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA1,   128, 128},
    {"TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_SHA256,        TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA256, 128, 128},
    {"TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256",         TLS1_TXT_ECDHE_RSA_WITH_AES_128_SHA256,          TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA256, 128, 128},
    {"TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,    TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AESGCM,   TCM_AEAD,   128, 128},
    {"TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256",         TLS1_TXT_ECDHE_RSA_WITH_AES_128_GCM_SHA256,      TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AESGCM,   TCM_AEAD,   128, 128},

    // 128 bit encryption with DHE
    {"TLS_DHE_DSS_WITH_AES_128_CBC_SHA",              TLS1_TXT_DHE_DSS_WITH_AES_128_SHA,               TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA1,   128, 128},
    {"TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA",         TLS1_TXT_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_Camellia, TCM_SHA1,   128, 128},
    {"TLS_DHE_RSA_WITH_AES_128_CBC_SHA",              TLS1_TXT_DHE_RSA_WITH_AES_128_SHA,               TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA1,   128, 128},
    {"TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA",         TLS1_TXT_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_Camellia, TCM_SHA1,   128, 128},
    {"TLS_DHE_DSS_WITH_AES_128_CBC_SHA256",           TLS1_TXT_DHE_DSS_WITH_AES_128_SHA256,            TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA256, 128, 128},
    {"TLS_DHE_RSA_WITH_AES_128_CBC_SHA256",           TLS1_TXT_DHE_RSA_WITH_AES_128_SHA256,            TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA256, 128, 128},
    {"TLS_DHE_DSS_WITH_AES_128_GCM_SHA256",           TLS1_TXT_DHE_DSS_WITH_AES_128_GCM_SHA256,        TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AESGCM,   TCM_AEAD,   128, 128},
    {"TLS_DHE_RSA_WITH_AES_128_GCM_SHA256",           TLS1_TXT_DHE_RSA_WITH_AES_128_GCM_SHA256,        TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AESGCM,   TCM_AEAD,   128, 128},

    // 256 bit encryption with ECDHE
    {"TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA",          TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,       TPV_SSLv3,  TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA1,   256, 256},
    {"TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA",            TLS1_TXT_ECDHE_RSA_WITH_AES_256_CBC_SHA,         TPV_SSLv3,  TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA1,   256, 256},
    {"TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_SHA384,        TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA384, 256, 256},
    {"TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384",         TLS1_TXT_ECDHE_RSA_WITH_AES_256_SHA384,          TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA384, 256, 256},
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    /* OpenSSL 1.1.0 supports the ChaCha20-Poly1305 ciphersuites defined in RFC 7905 */
    {"TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256", TLS1_TXT_ECDHE_ECDSA_WITH_CHACHA20_POLY1305,     TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_ChaCha20, TCM_AEAD,   256, 256},
    {"TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256",   TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305,       TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_ChaCha20, TCM_AEAD,   256, 256},
#endif
    {"TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,    TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AESGCM,   TCM_AEAD,   256, 256},
    {"TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384",         TLS1_TXT_ECDHE_RSA_WITH_AES_256_GCM_SHA384,      TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AESGCM,   TCM_AEAD,   256, 256},

    // 256 bit encryption with DHE
    {"TLS_DHE_DSS_WITH_AES_256_CBC_SHA",              TLS1_TXT_DHE_DSS_WITH_AES_256_SHA,               TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA1,   256, 256},
    {"TLS_DHE_RSA_WITH_AES_256_CBC_SHA",              TLS1_TXT_DHE_RSA_WITH_AES_256_SHA,               TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA1,   256, 256},
    {"TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA",         TLS1_TXT_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_Camellia, TCM_SHA1,   256, 256},
    {"TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA",         TLS1_TXT_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_Camellia, TCM_SHA1,   256, 256},
    {"TLS_DHE_DSS_WITH_AES_256_CBC_SHA256",           TLS1_TXT_DHE_DSS_WITH_AES_256_SHA256,            TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA256, 256, 256},
    {"TLS_DHE_RSA_WITH_AES_256_CBC_SHA256",           TLS1_TXT_DHE_RSA_WITH_AES_256_SHA256,            TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA256, 256, 256},
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    /* OpenSSL 1.1.0 supports the ChaCha20-Poly1305 ciphersuites defined in RFC 7905 */
    {"TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256",     TLS1_TXT_DHE_RSA_WITH_CHACHA20_POLY1305,         TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_ChaCha20, TCM_AEAD,   256, 256},
#endif
    {"TLS_DHE_DSS_WITH_AES_256_GCM_SHA384",           TLS1_TXT_DHE_DSS_WITH_AES_256_GCM_SHA384,        TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AESGCM,   TCM_AEAD,   256, 256},
    {"TLS_DHE_RSA_WITH_AES_256_GCM_SHA384",           TLS1_TXT_DHE_RSA_WITH_AES_256_GCM_SHA384,        TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AESGCM,   TCM_AEAD,   256, 256}

};

#define GLOBAL_NUM_CIPHERSUITES (sizeof(globalCipherSuiteList)/sizeof(DcmCipherSuiteList))

const size_t DcmTLSCiphersuiteHandler::unknownCipherSuiteIndex = (size_t) -1;

DcmTLSCiphersuiteHandler::DcmTLSCiphersuiteHandler()
: ciphersuiteList()
, currentProfile(TSP_Profile_None)
, ciphersuiteSupported(new OFBool[GLOBAL_NUM_CIPHERSUITES])
{
  determineSupportedCiphers();
}

DcmTLSCiphersuiteHandler::~DcmTLSCiphersuiteHandler()
{
  delete[] ciphersuiteSupported;
}

void DcmTLSCiphersuiteHandler::determineSupportedCiphers()
{
  // initialize all ciphersuites to "unsupported"
  size_t numEntries = GLOBAL_NUM_CIPHERSUITES;
  for (size_t i = 0; i < numEntries; i++) ciphersuiteSupported[i] = OFFalse;

#if OPENSSL_VERSION_NUMBER < 0x10100000L
  SSL_CTX *ctx = SSL_CTX_new(SSLv23_method());
#else
  SSL_CTX *ctx = SSL_CTX_new(TLS_method());
#endif

  if (ctx)
  {
    // we don't want to support SSL2 and SSL3 and its ciphers
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);
    // other than that, enable all ciphers including those without encryption
    SSL_CTX_set_cipher_list(ctx, "ALL:eNULL");
    STACK_OF(SSL_CIPHER) *sk = SSL_CTX_get_ciphers(ctx);
    if (sk)
    {
      const SSL_CIPHER *c = NULL;
      const char *p = NULL;
      size_t idx = 0;
      long numCiphers = sk_SSL_CIPHER_num(sk);
      // for each ciphersuite supported by OpenSSL...
      for (long j = 0; j < numCiphers; j++)
      {
        c = sk_SSL_CIPHER_value(sk, j);
        p = SSL_CIPHER_get_name(c);
        if (p)
        {
          // ...check if we also support it in DCMTK
          idx = lookupCiphersuiteByOpenSSLName(p);
          // if so, set the "supported" flag to true
          if (idx < numEntries) ciphersuiteSupported[idx] = OFTrue;
        }
      }
    }
    SSL_CTX_free(ctx);
  }
}


DcmTransportLayerStatus DcmTLSCiphersuiteHandler::addRequiredCipherSuite(const char *name)
{
  if (NULL == name) return TCS_illegalCall;
  size_t idx = lookupCiphersuite(name);
  if (idx < GLOBAL_NUM_CIPHERSUITES) ciphersuiteList.push_back(idx);
  else
  {
     DCMTLS_FATAL("Ciphersuite '" << name << "' not supported by the OpenSSL library used to compile this application.");
     return TCS_tlsError;
  }
  return TCS_ok;
}


void DcmTLSCiphersuiteHandler::addOptional3DESCipherSuite()
{
  size_t idx = lookupCiphersuite("TLS_RSA_WITH_3DES_EDE_CBC_SHA");
  if (idx < GLOBAL_NUM_CIPHERSUITES) ciphersuiteList.push_back(idx);
  else
  {
    // we cannot activate the basic profile because OpenSSL has been compiled without 3DES support.
    DCMTLS_WARN("Ciphersuite 'TLS_RSA_WITH_3DES_EDE_CBC_SHA' not supported by the OpenSSL library used to compile this application, no backward compatibility with DICOM Basic TLS profile.");
  }
  return;
}


DcmTransportLayerStatus DcmTLSCiphersuiteHandler::setTLSProfile(DcmTLSSecurityProfile profile)
{
  currentProfile = profile;
  DcmTransportLayerStatus result = TCS_ok;

  switch (profile)
  {
    case TSP_Profile_None:
      ciphersuiteList.clear();
      break;
    case TSP_Profile_Basic:
      result = addRequiredCipherSuite("TLS_RSA_WITH_3DES_EDE_CBC_SHA");
      if (TCS_ok != result) return result;
      break;
    case TSP_Profile_AES:
      result = addRequiredCipherSuite("TLS_RSA_WITH_AES_128_CBC_SHA");
      if (TCS_ok != result) return result;
      addOptional3DESCipherSuite();
      break;
    case TSP_Profile_BCP195:
      // recommended ciphersuites as defined in the DICOM profile, plus backwards compatibility
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
      if (TCS_ok != result) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_128_GCM_SHA256");
      if (TCS_ok != result) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384");
      if (TCS_ok != result) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_256_GCM_SHA384");
      if (TCS_ok != result) return result;
      result = addRequiredCipherSuite("TLS_RSA_WITH_AES_128_CBC_SHA");
      if (TCS_ok != result) return result;
      addOptional3DESCipherSuite();
      break;
    case TSP_Profile_BCP195_ND:
      // required ciphersuites as defined in the DICOM profile
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
      if (TCS_ok != result) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_128_GCM_SHA256");
      if (TCS_ok != result) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384");
      if (TCS_ok != result) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_256_GCM_SHA384");
      if (TCS_ok != result) return result;
      break;
    case TSP_Profile_IHE_ATNA_Unencrypted:
      result = addRequiredCipherSuite("TLS_RSA_WITH_NULL_SHA");
      if (TCS_ok != result) return result;
      break;
    // we don't want a default case here
  }

  return TCS_ok;
}

void DcmTLSCiphersuiteHandler::clearTLSProfile()
{
  currentProfile = TSP_Profile_None;
  ciphersuiteList.clear();
}

DcmTransportLayerStatus DcmTLSCiphersuiteHandler::addCipherSuite(const char *suite)
{
  // first look up the index for the TLS ciphersuite name,
  // then look up the OpenSSL ciphersuite name based on this index.
  size_t idx = lookupCiphersuite(suite); // can handle NULL pointers
  if (idx < GLOBAL_NUM_CIPHERSUITES)
  {
    if (! ciphersuiteSupported[idx])
	{
      // user has selected a ciphersuite not supported by the OpenSSL version we are using
      DCMTLS_FATAL("Ciphersuite '" << suite << "' not supported by the OpenSSL library used to compile this application.");
      return TCS_tlsError;
    }

    size_t keySize = 0;
    switch (currentProfile)
    {
      case TSP_Profile_Basic:
      case TSP_Profile_AES:
      case TSP_Profile_BCP195:
      case TSP_Profile_BCP195_ND:
        // make sure that we don't add a NULL ciphersuite (i.e. a ciphersuite
        // without encryption) to one of the secure TLS profiles, because this
        // would jeopardize communication security. NULL ciphersuites are only
        // permitted in the "IHE ATNA unencrypted" profile, or when no profile is used.
        keySize = getCipherSuiteKeySize(idx);
        if (keySize == 0)
        {
          DCMTLS_FATAL("Unencrypted ciphersuite '" << suite << "' not permitted with security profile '" << lookupProfileName(currentProfile) << "'");
          return TCS_tlsError;
        }
        if (TKE_RSA == getCipherSuiteKeyExchange(idx))
        {
          DCMTLS_WARN("Ciphersuite '" << suite << "' uses RSA key transport. RFC 7525 recommends that such cipher suites should not be used.");
        }
		else
        {
          DCMTLS_WARN("Ciphersuite '" << suite << "' uses NO RSA key transport. RFC 7525 recommends that such cipher suites should not be used.");
        }
        break;

      case TSP_Profile_None:
      case TSP_Profile_IHE_ATNA_Unencrypted:
        // do nothing
        break;
    }

    ciphersuiteList.push_back(idx);
    return TCS_ok;
  }
  // invalid/unknown cipher suite name
  return TCS_illegalCall;
}

// Static helper function for the qsort() call in
// DcmTLSCiphersuiteHandler::getListOfCipherSuitesForOpenSSL().
static int cipherSuiteComparison(const void *lhs, const void *rhs)
{
  const size_t *lh = OFreinterpret_cast(const size_t *, lhs);
  const size_t *rh = OFreinterpret_cast(const size_t *, rhs);

  if (*lh < *rh) return 1;
  if (*lh > *rh) return -1;
  return 0;
}

void DcmTLSCiphersuiteHandler::getListOfCipherSuitesForOpenSSL(OFString& cslist, OFBool isServer) const
{
  cslist.clear();
  const char *c = NULL;
  OFVector<size_t> tempList = ciphersuiteList;

  if (isServer)
  {
    // When acting as a TLS server, we follow the recommendation of BCP 195
    // to select the strongest ciphersuite offered by the client. We do this by
    // ignoring the order of preference proposed by the client; instead we select the first
    // matching ciphersuite from our own (server-side) list of supported ciphersuites.
    // We sort this list with the strongest ciphersuite first. Since the global list
    // of ciphersuites (globalCipherSuiteList) is already sorted by strength (weakest
    // ciphersuite first), we only have to sort the ciphersuite IDs in decreasing order.
    qsort(&tempList[0], tempList.size(), sizeof(size_t), cipherSuiteComparison);
  }

  for (OFVector<size_t>::const_iterator it = tempList.begin(); it != tempList.end(); ++it)
  {
    c = getOpenSSLCipherSuiteName(*it);
    if (c)
    {
      if (! cslist.empty()) cslist += ":";
      cslist += c;
    }
  }
}

size_t DcmTLSCiphersuiteHandler::getNumberOfCipherSuites()
{
  return GLOBAL_NUM_CIPHERSUITES;
}

const char *DcmTLSCiphersuiteHandler::getTLSCipherSuiteName(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].TLSname;
  return NULL;
}

const char *DcmTLSCiphersuiteHandler::getOpenSSLCipherSuiteName(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].openSSLName;
  return NULL;
}

DcmTLSCipherProtocolVersion DcmTLSCiphersuiteHandler::getCipherSuiteProtocolVersion(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].protocolVersion;
  return TPV_TLSv12;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherKeyExchange DcmTLSCiphersuiteHandler::getCipherSuiteKeyExchange(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].keyExchange;
  return TKE_RSA;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherAuthentication DcmTLSCiphersuiteHandler::getCipherSuiteAuthentication(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].authentication;
  return TCA_RSA;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherEncryption DcmTLSCiphersuiteHandler::getCipherSuiteEncryption(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].encryption;
  return TCE_None;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherMAC DcmTLSCiphersuiteHandler::getCipherSuiteMAC(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].mac;
  return TCM_AEAD;
}

size_t DcmTLSCiphersuiteHandler::getCipherSuiteKeySize(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].keySize;
  return 0;
}

size_t DcmTLSCiphersuiteHandler::getCipherSuiteEffectiveKeySize(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].effectiveKeySize;
  return 0;
}

size_t DcmTLSCiphersuiteHandler::lookupCiphersuite(const char *tlsCipherSuiteName)
{
  if (tlsCipherSuiteName)
  {
    OFString aString(tlsCipherSuiteName);
    size_t numEntries = GLOBAL_NUM_CIPHERSUITES;
    for (size_t i = 0; i < numEntries; i++)
    {
      if (aString == globalCipherSuiteList[i].TLSname) return i;
    }
  }
  // ciphersuite not found
  return unknownCipherSuiteIndex;
}

size_t DcmTLSCiphersuiteHandler::lookupCiphersuiteByOpenSSLName(const char *opensslCipherSuiteName)
{
  if (opensslCipherSuiteName)
  {
    OFString aString(opensslCipherSuiteName);
    size_t numEntries = GLOBAL_NUM_CIPHERSUITES;
    for (size_t i = 0; i < numEntries; i++)
    {
      if (aString == globalCipherSuiteList[i].openSSLName) return i;
    }
  }
  // ciphersuite not found
  return unknownCipherSuiteIndex;
}

void DcmTLSCiphersuiteHandler::printSupportedCiphersuites(STD_NAMESPACE ostream& os) const
{
  size_t numEntries = GLOBAL_NUM_CIPHERSUITES;
  for (size_t i = 0; i < numEntries; i++)
  {
    if (ciphersuiteSupported[i])
       os << "  " << globalCipherSuiteList[i].TLSname << OFendl;
  }
}

const char *DcmTLSCiphersuiteHandler::lookupProfileName(DcmTLSSecurityProfile profile)
{
  switch (profile)
  {
    case TSP_Profile_Basic:
      return "Basic TLS Secure Transport Connection Profile";
      break;
    case TSP_Profile_AES:
      return "AES TLS Secure Transport Connection Profile";
      break;
    case TSP_Profile_BCP195:
      return "BCP 195 TLS Profile";
      break;
    case TSP_Profile_BCP195_ND:
      return "Non-downgrading BCP 195 TLS Profile";
      break;
    case TSP_Profile_None:
      return "None";
      break;
    case TSP_Profile_IHE_ATNA_Unencrypted:
      return "Authenticated unencrypted communication";
      break;
  }
  return "Unknown"; // should never happen
}

OFBool DcmTLSCiphersuiteHandler::cipher3DESsupported() const
{
  size_t idx = lookupCiphersuiteByOpenSSLName(SSL3_TXT_RSA_DES_192_CBC3_SHA);
  if (idx < GLOBAL_NUM_CIPHERSUITES) return ciphersuiteSupported[idx];

  // Default. Only happens if ciphersuite is not supported even by DCMTK anymore
  return OFFalse;
}

OFBool DcmTLSCiphersuiteHandler::cipherNULLsupported() const
{
  size_t idx = lookupCiphersuiteByOpenSSLName(SSL3_TXT_RSA_NULL_SHA);
  if (idx < GLOBAL_NUM_CIPHERSUITES) return ciphersuiteSupported[idx];

  // Default. Only happens if ciphersuite is not supported even by DCMTK anymore
  return OFFalse;
}

long DcmTLSCiphersuiteHandler::getTLSOptions() const
{
  long result = 0;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
  // When compiling with OpenSSL 1.1.0, SSL support is disabled in DcmTLSTransportLayer anyway.
  // For older OpenSSL versions we explicitly disable them here.
  result |= SSL_OP_NO_SSLv2;
  result |= SSL_OP_NO_SSLv3;
#endif

  // For the Non-downgrading BCP 195 TLS Profile,
  // we also disable TLS 1.0 and TLS 1.1
  if (currentProfile == TSP_Profile_BCP195_ND)
  {
    result |= SSL_OP_NO_TLSv1;
    result |= SSL_OP_NO_TLSv1_1;
  }

  return result;
}

#else  /* WITH_OPENSSL */

/* make sure that the object file is not completely empty if compiled
 * without OpenSSL because some linkers might fail otherwise.
 */
DCMTK_DCMTLS_EXPORT void tlsciphr_dummy_function()
{
  return;
}

#endif /* WITH_OPENSSL */
