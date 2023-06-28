/*
 *
 *  Copyright (C) 2018-2023, OFFIS e.V.
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
#include "dcmtk/dcmtls/tlscond.h"
#include "dcmtk/dcmtls/tlslayer.h"    /* for TLS_ERROR macro */
#include "tlsfmacr.h"                 /* for OpenSSL feature macros */

#ifdef WITH_OPENSSL

BEGIN_EXTERN_C
#include <openssl/ssl.h>
#include <openssl/tls1.h>
END_EXTERN_C

#ifndef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CIPHERS
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
  DcmTLSCipherMode mode;
  size_t keySize;
  size_t effectiveKeySize;
};


/* This is a subset of the list of ciphersuites supported by OpenSSL 1.0.1 and newer.
 * This list only contains ciphersuites that offer an acceptable level of security,
 * plus the "historic" ciphersuites for older TLS profiles (Basic, AES, IHE ATNA NULL).
 *
 * TLS 1.3 ciphersuites are not included since OpenSSL uses a completely different
 * API to negotiate these.
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
    {"TLS_RSA_WITH_NULL_SHA",                         SSL3_TXT_RSA_NULL_SHA,                           TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_None,     TCM_SHA1,    TKM_NA,     0,   0},
    {"TLS_RSA_WITH_3DES_EDE_CBC_SHA",                 SSL3_TXT_RSA_DES_192_CBC3_SHA,                   TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_3DES,     TCM_SHA1,    TKM_CBC,  168, 112},

    // 128 bit encryption with static RSA
    {"TLS_RSA_WITH_AES_128_CBC_SHA",                  TLS1_TXT_RSA_WITH_AES_128_SHA,                   TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_RSA_WITH_CAMELLIA_128_CBC_SHA",             TLS1_TXT_RSA_WITH_CAMELLIA_128_CBC_SHA,          TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_Camellia, TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_RSA_WITH_AES_128_CBC_SHA256",               TLS1_TXT_RSA_WITH_AES_128_SHA256,                TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA256,  TKM_CBC,  128, 128},
    {"TLS_RSA_WITH_AES_128_GCM_SHA256",               TLS1_TXT_RSA_WITH_AES_128_GCM_SHA256,            TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  128, 128},

    // 128 bit encryption with ECDH
    {"TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA",           TLS1_TXT_ECDH_ECDSA_WITH_AES_128_CBC_SHA,        TPV_SSLv3,  TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_ECDH_RSA_WITH_AES_128_CBC_SHA",             TLS1_TXT_ECDH_RSA_WITH_AES_128_CBC_SHA,          TPV_SSLv3,  TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256",        TLS1_TXT_ECDH_ECDSA_WITH_AES_128_SHA256,         TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA256,  TKM_CBC,  128, 128},
    {"TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256",          TLS1_TXT_ECDH_RSA_WITH_AES_128_SHA256,           TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA256,  TKM_CBC,  128, 128},
    {"TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256",        TLS1_TXT_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,     TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  128, 128},
    {"TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256",          TLS1_TXT_ECDH_RSA_WITH_AES_128_GCM_SHA256,       TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  128, 128},

    // 256 bit encryption with static RSA
    {"TLS_RSA_WITH_CAMELLIA_256_CBC_SHA",             TLS1_TXT_RSA_WITH_CAMELLIA_256_CBC_SHA,          TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_Camellia, TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_RSA_WITH_AES_256_CBC_SHA",                  TLS1_TXT_RSA_WITH_AES_256_SHA,                   TPV_SSLv3,  TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_RSA_WITH_AES_256_CBC_SHA256",               TLS1_TXT_RSA_WITH_AES_256_SHA256,                TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AES,      TCM_SHA256,  TKM_CBC,  256, 256},
    {"TLS_RSA_WITH_AES_256_GCM_SHA384",               TLS1_TXT_RSA_WITH_AES_256_GCM_SHA384,            TPV_TLSv12, TKE_RSA,        TCA_RSA,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  256, 256},

    // 256 bit encryption with static ECDH
    {"TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA",           TLS1_TXT_ECDH_ECDSA_WITH_AES_256_CBC_SHA,        TPV_SSLv3,  TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_ECDH_RSA_WITH_AES_256_CBC_SHA",             TLS1_TXT_ECDH_RSA_WITH_AES_256_CBC_SHA,          TPV_SSLv3,  TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384",        TLS1_TXT_ECDH_ECDSA_WITH_AES_256_SHA384,         TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AES,      TCM_SHA384,  TKM_CBC,  256, 256},
    {"TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384",          TLS1_TXT_ECDH_RSA_WITH_AES_256_SHA384,           TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AES,      TCM_SHA384,  TKM_CBC,  256, 256},
    {"TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384",        TLS1_TXT_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,     TPV_TLSv12, TKE_ECDH_ECDSA, TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  256, 256},
    {"TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384",          TLS1_TXT_ECDH_RSA_WITH_AES_256_GCM_SHA384,       TPV_TLSv12, TKE_ECDH_RSA,   TCA_ECDH,   TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  256, 256},

    // -- then all ciphersuites with forward secrecy --

    // 128 bit encryption with DHE
    {"TLS_DHE_DSS_WITH_AES_128_CBC_SHA",              TLS1_TXT_DHE_DSS_WITH_AES_128_SHA,               TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA",         TLS1_TXT_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_Camellia, TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_DHE_RSA_WITH_AES_128_CBC_SHA",              TLS1_TXT_DHE_RSA_WITH_AES_128_SHA,               TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA",         TLS1_TXT_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_Camellia, TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_DHE_DSS_WITH_AES_128_CBC_SHA256",           TLS1_TXT_DHE_DSS_WITH_AES_128_SHA256,            TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA256,  TKM_CBC,  128, 128},
    {"TLS_DHE_RSA_WITH_AES_128_CBC_SHA256",           TLS1_TXT_DHE_RSA_WITH_AES_128_SHA256,            TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA256,  TKM_CBC,  128, 128},
    {"TLS_DHE_DSS_WITH_AES_128_GCM_SHA256",           TLS1_TXT_DHE_DSS_WITH_AES_128_GCM_SHA256,        TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  128, 128},
    {"TLS_DHE_RSA_WITH_AES_128_GCM_SHA256",           TLS1_TXT_DHE_RSA_WITH_AES_128_GCM_SHA256,        TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  128, 128},

    // 128 bit encryption with ECDHE
    {"TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA",          TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,       TPV_SSLv3,  TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA",            TLS1_TXT_ECDHE_RSA_WITH_AES_128_CBC_SHA,         TPV_SSLv3,  TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA1,    TKM_CBC,  128, 128},
    {"TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_SHA256,        TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA256,  TKM_CBC,  128, 128},
    {"TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256",         TLS1_TXT_ECDHE_RSA_WITH_AES_128_SHA256,          TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA256,  TKM_CBC,  128, 128},
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
     /* Apparently we support the CAMELLIA ciphersuites in Galois/counter mode defined in RFC 6367 */
    {"TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256",  TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA384, TPV_TLSv12, TKE_ECDH,     TCA_ECDSA,  TCE_Camellia, TCM_SHA256,  TKM_GCM,  128, 128},
    {"TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256",    TLS1_TXT_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA384,   TPV_TLSv12, TKE_ECDH,     TCA_RSA,    TCE_Camellia, TCM_SHA256,  TKM_GCM,  128, 128},
#endif
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8
    /* OpenSSL 1.1.1 supports the AES-CCM Elliptic Curve Cryptography (ECC) ciphersuites defined in RFC 7251 */
    {"TLS_ECDHE_ECDSA_WITH_AES_128_CCM",              TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_CCM,           TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_CBC_MAC, TKM_CCM,  128, 128},
    {"TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8",            TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_CCM_8,         TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_CBC_MAC, TKM_CCM,  128, 128},
#endif
    {"TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,    TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  128, 128},
    {"TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256",         TLS1_TXT_ECDHE_RSA_WITH_AES_128_GCM_SHA256,      TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  128, 128},

    // 256 bit encryption with DHE
    {"TLS_DHE_DSS_WITH_AES_256_CBC_SHA",              TLS1_TXT_DHE_DSS_WITH_AES_256_SHA,               TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_DHE_RSA_WITH_AES_256_CBC_SHA",              TLS1_TXT_DHE_RSA_WITH_AES_256_SHA,               TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA",         TLS1_TXT_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_DSS,    TCE_Camellia, TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA",         TLS1_TXT_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA,      TPV_SSLv3,  TKE_DH,         TCA_RSA,    TCE_Camellia, TCM_SHA1,    TKM_CBC,  256, 256},
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305
    /* OpenSSL 1.1.0 supports the ChaCha20-Poly1305 ciphersuites defined in RFC 7905 */
    {"TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256",     TLS1_TXT_DHE_RSA_WITH_CHACHA20_POLY1305,         TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_ChaCha20, TCM_AEAD,    TKM_NA,   256, 256},
#endif
    {"TLS_DHE_DSS_WITH_AES_256_CBC_SHA256",           TLS1_TXT_DHE_DSS_WITH_AES_256_SHA256,            TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AES,      TCM_SHA256,  TKM_CBC,  256, 256},
    {"TLS_DHE_RSA_WITH_AES_256_CBC_SHA256",           TLS1_TXT_DHE_RSA_WITH_AES_256_SHA256,            TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AES,      TCM_SHA256,  TKM_CBC,  256, 256},
    {"TLS_DHE_DSS_WITH_AES_256_GCM_SHA384",           TLS1_TXT_DHE_DSS_WITH_AES_256_GCM_SHA384,        TPV_TLSv12, TKE_DH,         TCA_DSS,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  256, 256},
    {"TLS_DHE_RSA_WITH_AES_256_GCM_SHA384",           TLS1_TXT_DHE_RSA_WITH_AES_256_GCM_SHA384,        TPV_TLSv12, TKE_DH,         TCA_RSA,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  256, 256},

    // 256 bit encryption with ECDHE
    {"TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA",          TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,       TPV_SSLv3,  TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA",            TLS1_TXT_ECDHE_RSA_WITH_AES_256_CBC_SHA,         TPV_SSLv3,  TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA1,    TKM_CBC,  256, 256},
    {"TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_SHA384,        TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_SHA384,  TKM_CBC,  256, 256},
    {"TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384",         TLS1_TXT_ECDHE_RSA_WITH_AES_256_SHA384,          TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AES,      TCM_SHA384,  TKM_CBC,  256, 256},

#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
     /* Apparently we support the CAMELLIA ciphersuites in Galois/counter mode defined in RFC 6367 */
    {"TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384",  TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384, TPV_TLSv12, TKE_ECDH,     TCA_ECDSA,  TCE_Camellia, TCM_SHA384,  TKM_GCM,  256, 256},
    {"TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384",    TLS1_TXT_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384,   TPV_TLSv12, TKE_ECDH,     TCA_RSA,    TCE_Camellia, TCM_SHA384,  TKM_GCM,  256, 256},
#endif
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305
    /* OpenSSL 1.1.0 supports the ChaCha20-Poly1305 ciphersuites defined in RFC 7905 */
    {"TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256", TLS1_TXT_ECDHE_ECDSA_WITH_CHACHA20_POLY1305,     TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_ChaCha20, TCM_AEAD,    TKM_NA,   256, 256},
    {"TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256",   TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305,       TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_ChaCha20, TCM_AEAD,    TKM_NA,   256, 256},
#endif
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8
    /* OpenSSL 1.1.1 supports the AES-CCM Elliptic Curve Cryptography (ECC) ciphersuites defined in RFC 7251 */
    {"TLS_ECDHE_ECDSA_WITH_AES_256_CCM",              TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM,           TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_CBC_MAC, TKM_CCM,  256, 256},
    {"TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8",            TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8,         TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AES,      TCM_CBC_MAC, TKM_CCM,  256, 256},
#endif
    {"TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384",       TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,    TPV_TLSv12, TKE_ECDH,       TCA_ECDSA,  TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  256, 256},
    {"TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384",         TLS1_TXT_ECDHE_RSA_WITH_AES_256_GCM_SHA384,      TPV_TLSv12, TKE_ECDH,       TCA_RSA,    TCE_AESGCM,   TCM_AEAD,    TKM_GCM,  256, 256}
};

#define GLOBAL_NUM_CIPHERSUITES (sizeof(globalCipherSuiteList)/sizeof(DcmCipherSuiteList))

/* This is the list of TLS 1.3 ciphersuites supported by OpenSSL 1.1.1 and newer.
 *
 * IMPORTANT: This list must be sorted from the weakest to the strongest ciphersuite.
 *   - first sort by effective key size
 *   - then sort by hash key algorithm  (SHA-256 < SHA-384)
 */

#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
static const DcmCipherSuiteList globalTLS13CipherSuiteList[] =
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_128_CCM_8_SHA256
    {"TLS_AES_128_CCM_SHA256",                        TLS1_3_RFC_AES_128_CCM_SHA256,                   TPV_TLSv13, TKE_TLSv13,     TCA_TLSv13, TCE_AES,      TCM_SHA256,  TKM_CCM,  128, 128},
    {"TLS_AES_128_CCM_8_SHA256",                      TLS1_3_RFC_AES_128_CCM_8_SHA256,                 TPV_TLSv13, TKE_TLSv13,     TCA_TLSv13, TCE_AES,      TCM_SHA256,  TKM_CCM,  128, 128},
#endif
    {"TLS_AES_128_GCM_SHA256",                        TLS1_3_RFC_AES_128_GCM_SHA256,                   TPV_TLSv13, TKE_TLSv13,     TCA_TLSv13, TCE_AES,      TCM_SHA256,  TKM_GCM,  128, 128},
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_CHACHA20_POLY1305_SHA256
    {"TLS_CHACHA20_POLY1305_SHA256",                  TLS1_3_RFC_CHACHA20_POLY1305_SHA256,             TPV_TLSv13, TKE_TLSv13,     TCA_TLSv13, TCE_ChaCha20, TCM_SHA256,  TKM_NA,   256, 256},
#endif
    {"TLS_AES_256_GCM_SHA384",                        TLS1_3_RFC_AES_256_GCM_SHA384,                   TPV_TLSv13, TKE_TLSv13,     TCA_TLSv13, TCE_AES,      TCM_SHA384,  TKM_GCM,  256, 256},
};

#define GLOBAL_NUM_TLS13_CIPHERSUITES (sizeof(globalCipherSuiteList)/sizeof(DcmCipherSuiteList))

#endif


const size_t DcmTLSCiphersuiteHandler::unknownCipherSuiteIndex = (size_t) -1;

DcmTLSCiphersuiteHandler::DcmTLSCiphersuiteHandler()
: ciphersuiteList()
, tls13ciphersuiteList()
, currentProfile(TSP_Profile_None)
, tls13_enabled(OFTrue)
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

#ifndef HAVE_OPENSSL_PROTOTYPE_TLS_METHOD
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


OFCondition DcmTLSCiphersuiteHandler::addRequiredCipherSuite(const char *name)
{
  if (NULL == name) return EC_IllegalCall;
  size_t idx = lookupCiphersuite(name);
  if (idx < GLOBAL_NUM_CIPHERSUITES) ciphersuiteList.push_back(idx);
  else
  {
     DCMTLS_FATAL("Ciphersuite '" << name << "' not supported by the OpenSSL library used to compile this application.");
     return DCMTLS_EC_UnknownCiphersuite(name);
  }
  return EC_Normal;
}


OFCondition DcmTLSCiphersuiteHandler::addRequiredTLS13CipherSuite(const char *name)
{
  if (NULL == name) return EC_IllegalCall;
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  size_t idx = lookupTLS13Ciphersuite(name);
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) tls13ciphersuiteList.push_back(idx);
  else
  {
     DCMTLS_FATAL("Ciphersuite '" << name << "' not supported by the OpenSSL library used to compile this application.");
     return DCMTLS_EC_UnknownCiphersuite(name);
  }
  return EC_Normal;
#else
  DCMTLS_FATAL("TLS 1.3, and therefore, ciphersuite '" << name << "' not supported by the OpenSSL library used to compile this application.");
  return DCMTLS_EC_UnknownCiphersuite(name);
#endif
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


OFCondition DcmTLSCiphersuiteHandler::setTLSProfile(DcmTLSSecurityProfile profile)
{
  currentProfile = profile;
  OFCondition result = EC_Normal;

  switch (profile)
  {
    case TSP_Profile_None:
      tls13_enabled = OFTrue;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      break;
    case TSP_Profile_Basic:
      tls13_enabled = OFFalse;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      result = addRequiredCipherSuite("TLS_RSA_WITH_3DES_EDE_CBC_SHA");
      if (result.bad()) return result;
      break;
    case TSP_Profile_AES:
      tls13_enabled = OFFalse;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      result = addRequiredCipherSuite("TLS_RSA_WITH_AES_128_CBC_SHA");
      if (result.bad()) return result;
      addOptional3DESCipherSuite();
      break;
    case TSP_Profile_BCP195:
      tls13_enabled = OFTrue;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      // recommended ciphersuites as defined in the DICOM profile, plus backwards compatibility
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_RSA_WITH_AES_128_CBC_SHA");
      if (result.bad()) return result;
      addOptional3DESCipherSuite();
      break;
    case TSP_Profile_BCP195_ND:
      tls13_enabled = OFTrue;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      // required ciphersuites as defined in the DICOM profile
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      break;
    case TSP_Profile_BCP195_Extended:
      tls13_enabled = OFFalse;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      // required ciphersuites as defined in the DICOM profile
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_DHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      break;
    case TSP_Profile_BCP_195_RFC_8996:
      tls13_enabled = OFTrue;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      // recommended ciphersuites as defined in RFC 9325
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      break;
    case TSP_Profile_BCP_195_RFC_8996_Modified:
#ifdef DCMTK_Modified_BCP195_RFC8996_TLS_Profile_Supported
      tls13_enabled = OFTrue;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      // required TLS 1.0-1.2 ciphersuites as defined in the DICOM profile
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256");
      if (result.bad()) return result;
#endif
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_128_CCM");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256");
      if (result.bad()) return result;
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384");
      if (result.bad()) return result;
#endif
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_256_CCM");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredCipherSuite("TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384");
      if (result.bad()) return result;

      // required TLS 1.3 ciphersuites as defined in the DICOM profile
      result = addRequiredTLS13CipherSuite("TLS_AES_256_GCM_SHA384");
      if (result.bad()) return result;
      result = addRequiredTLS13CipherSuite("TLS_CHACHA20_POLY1305_SHA256");
      if (result.bad()) return result;
      result = addRequiredTLS13CipherSuite("TLS_AES_128_GCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredTLS13CipherSuite("TLS_AES_128_CCM_SHA256");
      if (result.bad()) return result;
      result = addRequiredTLS13CipherSuite("TLS_AES_128_CCM_8_SHA256");
      if (result.bad()) return result;
#else
      return DCMTLS_EC_TLSProfileNotSupported;
#endif
      break;

    case TSP_Profile_IHE_ATNA_Unencrypted:
      tls13_enabled = OFFalse;
      ciphersuiteList.clear();
      tls13ciphersuiteList.clear();
      result = addRequiredCipherSuite("TLS_RSA_WITH_NULL_SHA");
      if (result.bad()) return result;
      break;
    // we don't want a default case here
  }

  return EC_Normal;
}

void DcmTLSCiphersuiteHandler::clearTLSProfile()
{
  tls13_enabled = OFTrue;
  currentProfile = TSP_Profile_None;
  ciphersuiteList.clear();
  tls13ciphersuiteList.clear();
}

OFCondition DcmTLSCiphersuiteHandler::addCipherSuite(const char *suite)
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
      return DCMTLS_EC_UnknownCiphersuite(suite);
    }

    size_t keySize = 0;
    size_t effectiveSize = 0;
    OFString suiteName;
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
          return DCMTLS_EC_CiphersuiteNotAllowed;
        }
        // static RSA should not be used
        if (TKE_RSA == getCipherSuiteKeyExchange(idx))
        {
          DCMTLS_WARN("Ciphersuite '" << suite << "' uses RSA key transport. RFC 7525 recommends that such cipher suites should not be used.");
        }
        break;

      case TSP_Profile_BCP_195_RFC_8996:
        // make sure that we don't add a NULL ciphersuite (i.e. a ciphersuite without encryption)
        keySize = getCipherSuiteKeySize(idx);
        if (keySize == 0)
        {
          DCMTLS_FATAL("Unencrypted ciphersuite '" << suite << "' not permitted with security profile '" << lookupProfileName(currentProfile) << "'");
          return DCMTLS_EC_CiphersuiteNotAllowed;
        }
        // SHA-1 MUST NOT be used. This also applies to MD5, but we don't support any MD5 ciphersuites anyway
        if (TCM_SHA1 == getCipherSuiteMAC(idx))
        {
          DCMTLS_FATAL("Ciphersuite '" << suite << "' using SHA-1 MAC not permitted with security profile '" << lookupProfileName(currentProfile) << "'");
          return DCMTLS_EC_CiphersuiteNotAllowed;
        }
        // print a warning if the TLS_RSA_WITH_AES_128_CBC_SHA ciphersuite is added
        suiteName = suite;
        if (suiteName == "TLS_RSA_WITH_AES_128_CBC_SHA")
        {
          DCMTLS_WARN("RFC 9325 recommends that ciphersuite 'TLS_RSA_WITH_AES_128_CBC_SHA' should not be used.");
        }
        else
        {
          // print a warning if a static RSA ciphersuite is added
          if (TKE_RSA == getCipherSuiteKeyExchange(idx))
          {
            DCMTLS_WARN("Ciphersuite '" << suite << "' uses RSA key transport. RFC 9325 recommends that such cipher suites should not be used.");
          }
          // print a warning if a DHE ciphersuite is added
          if (TKE_DH == getCipherSuiteKeyExchange(idx))
          {
            DCMTLS_WARN("Ciphersuite '" << suite << "' uses ephemeral finite-field Diffie-Hellman key agreement. RFC 9325 recommends that such cipher suites should not be used.");
          }
        }
        // print a warning if a block cipher in CBC mode is added
        if (TKM_CBC == getCipherSuiteMode(idx))
        {
            DCMTLS_WARN("Ciphersuite '" << suite << "' uses CBC mode of operation. RFC 9325 recommends that such cipher suites should not be used, unless both sides support the encrypt_then_mac extension (we do).");
        }
        // print a warning if cipher with less than 128 bits of effective key length is added.
        // Actually this should never happen because 3DES is the only cipher suite with less than 128 bits supported in DCMTK,
        // and that one is already blocked by the test for SHA-1. Nevertheless, make sure we remain on the safe side.
        effectiveSize = getCipherSuiteEffectiveKeySize(idx);
        if (effectiveSize < 128)
        {
            DCMTLS_WARN("Ciphersuite '" << suite << "' only offers ' << effectiveSize << ' bits of effective key length. RFC 9325 recommends that cipher suites offering less than 128 bits should not be used.");
        }
        break;

      case TSP_Profile_BCP195_Extended:
      case TSP_Profile_BCP_195_RFC_8996_Modified:
        DCMTLS_FATAL("Additional ciphersuites not permitted with security profile '" << lookupProfileName(currentProfile) << "'");
        return DCMTLS_EC_CiphersuiteNotAllowed;
        break;

      case TSP_Profile_None:
      case TSP_Profile_IHE_ATNA_Unencrypted:
        // do nothing
        break;
    }

    ciphersuiteList.push_back(idx);
    return EC_Normal;
  }
  // invalid/unknown cipher suite name
  return DCMTLS_EC_UnknownCiphersuite( suite ? suite : "(NULL)" );
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

void DcmTLSCiphersuiteHandler::getListOfTLS13CipherSuitesForOpenSSL(OFString& cslist, OFBool isServer) const
{
  cslist.clear();
  const char *c = NULL;
  OFVector<size_t> tempList = tls13ciphersuiteList;

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
    c = getOpenSSLTLS13CipherSuiteName(*it);
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

size_t DcmTLSCiphersuiteHandler::getNumberOfTLS13CipherSuites()
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  return GLOBAL_NUM_TLS13_CIPHERSUITES;
#else
  return 0;
#endif
}

const char *DcmTLSCiphersuiteHandler::getTLSCipherSuiteName(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].TLSname;
  return NULL;
}

const char *DcmTLSCiphersuiteHandler::getTLS13CipherSuiteName(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].TLSname;
#endif
  return NULL;
}

const char *DcmTLSCiphersuiteHandler::getOpenSSLCipherSuiteName(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].openSSLName;
  return NULL;
}

const char *DcmTLSCiphersuiteHandler::getOpenSSLTLS13CipherSuiteName(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].openSSLName;
#endif
  return NULL;
}

DcmTLSCipherProtocolVersion DcmTLSCiphersuiteHandler::getCipherSuiteProtocolVersion(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].protocolVersion;
  return TPV_TLSv12;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherProtocolVersion DcmTLSCiphersuiteHandler::getTLS13CipherSuiteProtocolVersion(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].protocolVersion;
#endif
  return TPV_TLSv12;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherKeyExchange DcmTLSCiphersuiteHandler::getCipherSuiteKeyExchange(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].keyExchange;
  return TKE_RSA;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherKeyExchange DcmTLSCiphersuiteHandler::getTLS13CipherSuiteKeyExchange(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].keyExchange;
#endif
  return TKE_RSA;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherAuthentication DcmTLSCiphersuiteHandler::getCipherSuiteAuthentication(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].authentication;
  return TCA_RSA;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherAuthentication DcmTLSCiphersuiteHandler::getTLS13CipherSuiteAuthentication(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].authentication;
#endif
  return TCA_RSA;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherEncryption DcmTLSCiphersuiteHandler::getCipherSuiteEncryption(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].encryption;
  return TCE_None;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherEncryption DcmTLSCiphersuiteHandler::getTLS13CipherSuiteEncryption(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].encryption;
#endif
  return TCE_None;  // invalid index, return a (rather arbitrary) default
}

DcmTLSCipherMAC DcmTLSCiphersuiteHandler::getCipherSuiteMAC(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].mac;
  return TCM_AEAD;
}

DcmTLSCipherMAC DcmTLSCiphersuiteHandler::getTLS13CipherSuiteMAC(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].mac;
#endif
  return TCM_AEAD;
}

DcmTLSCipherMode DcmTLSCiphersuiteHandler::getCipherSuiteMode(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].mode;
  return TKM_NA;
}

DcmTLSCipherMode DcmTLSCiphersuiteHandler::getTLS13CipherSuiteMode(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].mode;
#endif
  return TKM_NA;
}

size_t DcmTLSCiphersuiteHandler::getCipherSuiteKeySize(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].keySize;
  return 0;
}

size_t DcmTLSCiphersuiteHandler::getTLS13CipherSuiteKeySize(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].keySize;
#endif
  return 0;
}

size_t DcmTLSCiphersuiteHandler::getCipherSuiteEffectiveKeySize(size_t idx)
{
  if (idx < GLOBAL_NUM_CIPHERSUITES) return globalCipherSuiteList[idx].effectiveKeySize;
  return 0;
}

size_t DcmTLSCiphersuiteHandler::getTLS13CipherSuiteEffectiveKeySize(size_t idx)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (idx < GLOBAL_NUM_TLS13_CIPHERSUITES) return globalTLS13CipherSuiteList[idx].effectiveKeySize;
#endif
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

size_t DcmTLSCiphersuiteHandler::lookupTLS13Ciphersuite(const char *tlsCipherSuiteName)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (tlsCipherSuiteName)
  {
    OFString aString(tlsCipherSuiteName);
    size_t numEntries = GLOBAL_NUM_TLS13_CIPHERSUITES;
    for (size_t i = 0; i < numEntries; i++)
    {
      if (aString == globalTLS13CipherSuiteList[i].TLSname) return i;
    }
  }
#endif

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

size_t DcmTLSCiphersuiteHandler::lookupTLS13CiphersuiteByOpenSSLName(const char *opensslCipherSuiteName)
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  if (opensslCipherSuiteName)
  {
    OFString aString(opensslCipherSuiteName);
    size_t numEntries = GLOBAL_NUM_TLS13_CIPHERSUITES;
    for (size_t i = 0; i < numEntries; i++)
    {
      if (aString == globalTLS13CipherSuiteList[i].openSSLName) return i;
    }
  }
#endif

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

void DcmTLSCiphersuiteHandler::printSupportedTLS13Ciphersuites(STD_NAMESPACE ostream& os) const
{
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
  size_t numEntries = GLOBAL_NUM_TLS13_CIPHERSUITES;
  for (size_t i = 0; i < numEntries; i++)
  {
    if (ciphersuiteSupported[i])
       os << "  " << globalTLS13CipherSuiteList[i].TLSname << OFendl;
  }
#endif
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
    case TSP_Profile_BCP195_Extended:
      return "Extended BCP 195 TLS Profile";
      break;
    case TSP_Profile_None:
      return "None";
      break;
    case TSP_Profile_BCP_195_RFC_8996:
      return "BCP 195 RFC 8996 TLS Profile";
      break;
    case TSP_Profile_BCP_195_RFC_8996_Modified:
      return "Modified BCP 195 RFC 8996 TLS Profile";
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

OFBool DcmTLSCiphersuiteHandler::isTLS13Enabled() const
{
  return tls13_enabled;
}

long DcmTLSCiphersuiteHandler::getTLSOptions() const
{
  long result = 0;
#ifndef HAVE_OPENSSL_PROTOTYPE_TLS_METHOD
  // When compiling with OpenSSL 1.1.0, SSL support is disabled in DcmTLSTransportLayer anyway.
  // For older OpenSSL versions we explicitly disable them here.
  result |= SSL_OP_NO_SSLv2;
  result |= SSL_OP_NO_SSLv3;
#endif

  // For the Non-downgrading and Extended BCP 195 TLS Profile,
  // we also disable TLS 1.0 and TLS 1.1
  if ((currentProfile == TSP_Profile_BCP195_ND) || (currentProfile == TSP_Profile_BCP195_Extended))
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
