/*
 *
 *  Copyright (C) 1998-2024, OFFIS e.V.
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
 *    classes: DcmTLSTransportLayer
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmtls/tlslayer.h"
#include "dcmtk/dcmtls/tlsdefin.h"
#include "dcmtk/dcmtls/tlscond.h"
#include "dcmtk/ofstd/ofdiag.h"      /* for DCMTK_DIAGNOSTIC macros */

#ifdef WITH_OPENSSL

BEGIN_EXTERN_C
#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#endif
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/x509_vfy.h>
END_EXTERN_C

#ifndef X509_V_ERR_UNSPECIFIED
#define X509_V_ERR_UNSPECIFIED 1
#endif

#include "dcmtk/dcmtls/tlslayer.h"
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/ofstd/ofrand.h"

#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET0_PARAM
#define DCMTK_SSL_CTX_get0_param SSL_CTX_get0_param
#else
#define DCMTK_SSL_CTX_get0_param(A) (A)->param;
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_X509_GET_SIGNATURE_NID
#define X509_get_signature_nid(x509) OBJ_obj2nid((x509)->sig_alg->algorithm)
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CERT_STORE
#define SSL_CTX_get_cert_store(ctx) (ctx)->cert_store
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_BASE_ID
#define EVP_PKEY_base_id(key) EVP_PKEY_type((key)->type)
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_DH_BITS
#define DH_bits(dh) BN_num_bits((dh)->p)
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_X509_STORE_GET0_PARAM
#define X509_STORE_get0_param(A) (A)->param;
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_X509_STORE_CTX_GET0_CERT
#define X509_STORE_CTX_get0_cert(ctx) ((ctx)->cert)
#endif

int DcmTLSTransportLayer::contextStoreIndex = -1;

extern "C" int DcmTLSTransportLayer_certificateValidationCallback(int ok, X509_STORE_CTX *storeContext);

OFLogger DCM_dcmtlsLogger = OFLog::getLogger("dcmtk.dcmtls");

/*  This static sets a hard-coded set of Diffie-Hellman parameters
 *  with 2048 bits key size that is used for ephemeral Diffie-Hellmane
 *  (DHE_) ciphersuites unless the user replaces the parameter set
 *  by calling DcmTLSTransportLayer::setTempDHParameters().
 *  Using a hard-coded DH parameter set is safe because the DH key exchange
 *  does not require these parameters to be secret.
 *  We use the 2048 bit Diffie-Hellman MODP group defined in RFC 7919.
 */
OFBool DcmTLSTransportLayer::setBuiltInDHParameters()
{
  static char dh2048_p[] =
    "-----BEGIN DH PARAMETERS-----\n"
    "MIIBCAKCAQEA//////////+t+FRYortKmq/cViAnPTzx2LnFg84tNpWp4TZBFGQz\n"
    "+8yTnc4kmz75fS/jY2MMddj2gbICrsRhetPfHtXV/WVhJDP1H18GbtCFY2VVPe0a\n"
    "87VXE15/V8k1mE8McODmi3fipona8+/och3xWKE2rec1MKzKT0g6eXq8CrGCsyT7\n"
    "YdEIqUuyyOP7uWrat2DX9GgdT0Kj3jlN9K5W7edjcrsZCwenyO4KbXCeAvzhzffi\n"
    "7MA0BM0oNC9hkXL+nOmFg/+OTxIy7vKBg8P+OxtMb61zO7X8vC7CIAXFjvGDfRaD\n"
    "ssbzSibBsu/6iGtCOGEoXJf//////////wIBAg==\n"
    "-----END DH PARAMETERS-----\n";

  if (transportLayerContext==NULL) return OFFalse;
  BIO *bio = BIO_new_mem_buf(dh2048_p, sizeof(dh2048_p));
  if (bio)
  {
#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET0_TMP_DH_PKEY
    EVP_PKEY *dhparams = PEM_read_bio_Parameters(bio,NULL);
    BIO_free(bio);
    if (dhparams)
    {
      SSL_CTX_set0_tmp_dh_pkey(transportLayerContext, dhparams); // transfers ownership of "dhparams" to transportLayerContext
      return OFTrue;
    }
#else
    DH *dhparams = PEM_read_bio_DHparams(bio,NULL,NULL,NULL);
    BIO_free(bio);
    if (dhparams)
    {
      SSL_CTX_set_tmp_dh(transportLayerContext,dhparams);
      DH_free(dhparams); /* Safe because of reference counts in OpenSSL */
      return OFTrue;
    }
#endif
  }

  return OFFalse;
}

int DcmTLSTransportLayer_certificateValidationCallback(int ok, X509_STORE_CTX *storeContext)
{
  // this callback is called whenever OpenSSL has validated a X.509 certificate.
  // The result of OpenSSL's pre-validation is passed as parameter "ok".
  if (ok) // prevalidation has passed, perform additional checks
  {
    // this call will return a pointer to the certificate in the chain
    // that is currently being checked, starting with the root CA certificate,
    // and working upwards from there.
    X509 *cert = X509_STORE_CTX_get_current_cert(storeContext);
    if (cert)
    {
      int rsabits = 0;
      int ecdsabits = 0;
      EVP_PKEY *pubkey = X509_get_pubkey(cert); // creates a copy of the public key
      if (pubkey)
      {
        if (EVP_PKEY_base_id(pubkey) == EVP_PKEY_RSA)
        {
          rsabits = EVP_PKEY_bits(pubkey); // RSA public key size, in bits
        }
        if (EVP_PKEY_base_id(pubkey) == EVP_PKEY_EC)
        {
          ecdsabits = EVP_PKEY_bits(pubkey); // ECDSA public key size, in bits
        }
        EVP_PKEY_free(pubkey);
      }

      // get a pointer to the SSL structure of the current connection
      SSL *ssl = OFreinterpret_cast(SSL *, X509_STORE_CTX_get_ex_data(storeContext, SSL_get_ex_data_X509_STORE_CTX_idx()));
      if (ssl)
      {
        DcmTLSTransportLayer *tlayer = OFreinterpret_cast(DcmTLSTransportLayer *, SSL_get_ex_data(ssl, DcmTLSTransportLayer::contextStoreIndex));
        if (tlayer)
        {
          // check if the hash key used in the peer certificate is on our "blacklist" of weak hash key algorithms
          const char *hash = tlayer->checkHashKeyIsTooInSecure(cert);
          if (hash)
          {
            DCMTLS_ERROR("Weak certificate hash key: peer provided certificate with '" << hash << "'. Refusing TLS connection.");
            return 0;
          }

          // check if the hash key used in the peer certificate is on our "whitelist" of strong hash key algorithms
          hash = tlayer->checkHashKeyIsSecure(cert);
          DcmTLSSecurityProfile profile = tlayer->getTLSProfile();
          switch (profile)
          {
             case TSP_Profile_BCP_195_RFC_8996_Modified:
               if ((rsabits > 0) && (rsabits < 2048))
               {
                 DCMTLS_ERROR("RSA key length too short: the DICOM TLS profile requires at least 2048 bits, but peer provided RSA certificate with "  << rsabits << " bits. Refusing TLS connection.");
                 return 0;
               }
               if ((ecdsabits > 0) && (ecdsabits < 256))
               {
                 DCMTLS_ERROR("ECDSA key length too short: the DICOM TLS profile requires at least 256 bits, but peer provided ECDSA certificate with "  << ecdsabits << " bits. Refusing TLS connection.");
                 return 0;
               }
               if (hash)
               {
                 DCMTLS_ERROR("Weak certificate hash key: the DICOM TLS profile requires SHA-256 (or better) for certificates, but peer provided certificate with '" << hash << "'. Refusing TLS connection.");
                 return 0;
               }
               break;

             case TSP_Profile_BCP_195_RFC_8996:
               if ((rsabits > 0) && (rsabits < 2048))
               {
                 if (! SSL_is_server(ssl))
                 {
                   DCMTLS_ERROR("Server RSA key length too short: RFC 9325 requires at least 2048 bits, but server provided RSA certificate with "  << rsabits << " bits. Refusing TLS connection.");
                   return 0;
                 }
               }
               /* fallthrough */

             default:
               if ((rsabits > 0) && (rsabits < 2048))
               {
                 DCMTLS_WARN("RSA key length too short: RFC 9325 recommends at least 2048 bits, but peer provided RSA certificate with "  << rsabits << " bits.");
               }
               if ((ecdsabits > 0) && (ecdsabits < 224))
               {
                 DCMTLS_WARN("ECDSA key length too short: RFC 9325 recommends at least 224 bits, but peer provided ECDSA certificate with "  << ecdsabits << " bits.");
               }
               if (hash)
               {
                 DCMTLS_WARN("Weak certificate hash key: RFC 9325 recommends SHA-256 (or better, but peer provided certificate with '" << hash << "'.");
               }
               break;
           }
        }
      }
    }
  }
  return ok;
}

// The 'dicom' protocol identifier for DICOM in network format
// (string length, followed by a sequence of characters, no terminating zero,
// as defined in https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml#alpn-protocol-ids

static const unsigned char alpn_dicom_protocol[] = {
     5, 0x64, 0x69, 0x63, 0x6f, 0x6d
};

static const unsigned char alpn_dicom_protocol_len = OFstatic_cast(unsigned char, sizeof(alpn_dicom_protocol));

extern "C" int DcmTLSTransportLayer_ALPNCallback(SSL *ssl, const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg);

int DcmTLSTransportLayer_ALPNCallback(SSL * /*ssl*/, const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void * /*arg*/ )
{
  // check if the list of protocols proposed by the client contains DICOM
  unsigned char *npout = NULL;
  if (OPENSSL_NPN_NEGOTIATED == SSL_select_next_proto(&npout, outlen, alpn_dicom_protocol, alpn_dicom_protocol_len, in, inlen))
  {
    // client has proposed the DICOM protocol. We accept this.
    *out = npout;
    return SSL_TLSEXT_ERR_OK;
  }

  // client has proposed a protocol other than DICOM. Reject this.
  DCMTLS_ERROR("TLS ALPN negotiation failure: Client has proposed protocol(s) other than 'dicom'");
  return SSL_TLSEXT_ERR_ALERT_FATAL;
}

extern "C" int DcmTLSTransportLayer_SNICallback(SSL *s, int *al, void *arg);

int DcmTLSTransportLayer_SNICallback(SSL *s, int * /* al */, void *arg)
{
  const char *called_name = SSL_get_servername(s, TLSEXT_NAMETYPE_host_name);
  if (called_name)
  {
    DcmTLSTransportLayer *tlayer = OFreinterpret_cast(DcmTLSTransportLayer *, arg);
    if (! tlayer->checkServerSNI(called_name))
    {
      DCMTLS_ERROR("SNI failure: client requested server '" << called_name << "', my name is '" << tlayer->getServerSNI() << "'.");
      return SSL_TLSEXT_ERR_ALERT_FATAL;
    }
  }
  return SSL_TLSEXT_ERR_OK;
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
  int passwordSize = OFstatic_cast(int, password->length());
  if (passwordSize > size) passwordSize = size;
  strncpy(buf, password->c_str(), passwordSize);
  return passwordSize;
}


// The TLS Supported Elliptic Curves extension (RFC 4492) is only supported in OpenSSL 1.0.2 and newer.
// When compiling with OpenSSL 1.0.1, we are not using computeEllipticCurveList().
#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_CURVES

/** determine the list of elliptic curves supported by the OpenSSL library
 *  for use with the TLS elliptic curve extension.
 *  @param ecvector a list of supported elliptic curves that have 256 or
 *     more bits is added to this vector upon return.
 */
static void computeEllipticCurveList(OFVector<int>& ecvector)
{
  // RFC 9325: Curves of less than 224 bits MUST NOT be used.
  // Actually we only enable curves with at least 256 bits in DCMTK,
  // following NIST and BSI recommendations.
  const int eclist[] = {
    // The list of elliptic curves actually supported by OpenSSL 1.0.2
    // seems to be undocumented. See implementation of tls1_ec_nid2curve_id()
    // for a list of supported NIDs. Here are all elliptic curves
    // supported by OpenSSL 1.0.2 that have 256 or more bits.
    //
    // Compiled versions of OpenSSL may further reduce this list.
    // For example, OpenSSL on RHEL 7.6 only supports four of these curves.
    // We therefore simply test each curve and only retain those that are
    // accepted by SSL_CTX_set1_curves().

    NID_X9_62_prime256v1,  NID_secp256k1,         NID_secp384r1,
    NID_secp521r1,         NID_sect283k1,         NID_sect283r1,
    NID_sect409k1,         NID_sect409r1,         NID_sect571k1,
    NID_sect571r1,         NID_brainpoolP256r1,   NID_brainpoolP384r1,
    NID_brainpoolP512r1
  };

  // create  a SSL context object
#ifndef HAVE_OPENSSL_PROTOTYPE_TLS_METHOD
   SSL_CTX *ctx = SSL_CTX_new(SSLv23_method());
   if (ctx) SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);
#else
   SSL_CTX *ctx = SSL_CTX_new(TLS_method());
#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_SECURITY_LEVEL
   if (ctx) SSL_CTX_set_security_level(ctx, 0);
#endif
#endif
  if (ctx)
  {
    size_t numentries = sizeof(eclist) / sizeof(int);
    ecvector.reserve(numentries);
    for (size_t i = 0; i < numentries; ++i)
    {
      // try to set the given elliptic curve
      if (SSL_CTX_set1_curves(ctx, &eclist[i], 1))
      {
        // if successful, add to the list of supported elliptic curves
        ecvector.push_back(eclist[i]);
      }
    }
    // delete the SSL context object
    SSL_CTX_free(ctx);
  }
}

#endif /* HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_CURVES */


DcmTLSTransportLayer::DcmTLSTransportLayer()
: DcmTransportLayer()
, transportLayerContext(NULL)
, canWriteRandseed(OFFalse)
, privateKeyPasswd()
, role(NET_ACCEPTORREQUESTOR)
, clientSNI(NULL)
, serverSNI(NULL)
, certificateTypeIsDSA(OFFalse)
{
}

// Depending on the OpenSSL version used, SSL_CTX_set_tmp_ecdh() will
// cause this warning to be issued. In any case, this can safely be ignored.
#include DCMTK_DIAGNOSTIC_IGNORE_CONST_EXPRESSION_WARNING

DcmTLSTransportLayer::DcmTLSTransportLayer(T_ASC_NetworkRole networkRole, const char *randFile, OFBool initOpenSSL)
: DcmTransportLayer()
, transportLayerContext(NULL)
, canWriteRandseed(OFFalse)
, privateKeyPasswd()
, role(networkRole)
, clientSNI(NULL)
, serverSNI(NULL)
, certificateTypeIsDSA(OFFalse)
{
   if (initOpenSSL) initializeOpenSSL();
   if (randFile) seedPRNG(randFile);

#ifndef HAVE_OPENSSL_PROTOTYPE_TLS_METHOD
   // on versions of OpenSSL older than 1.1.0, we use the
   // SSLv23 methods and not the TLSv1 methods because the latter
   // only accept TLS 1.0 and prevent the negotiation of newer
   // versions of TLS.
   // We use SSL_CTX_set_options() to disable SSLv2 and SSLv3.
   switch (networkRole)
   {
     case NET_ACCEPTOR:
       transportLayerContext = SSL_CTX_new(SSLv23_server_method());
       break;
     case NET_REQUESTOR:
       transportLayerContext = SSL_CTX_new(SSLv23_client_method());
       break;
     case NET_ACCEPTORREQUESTOR:
       transportLayerContext = SSL_CTX_new(SSLv23_method());
       break;
   }
   if (transportLayerContext) SSL_CTX_set_options(transportLayerContext, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);
#else
   // starting with OpenSSL 1.1.0, a new TLS_method() is offered
   // that automatically selects the highest version of the TLS
   // protocol supported by client and server.
   // The previous TLSv1_methods are now deprecated and generate
   // a warning.
   switch (networkRole)
   {
     case NET_ACCEPTOR:
       transportLayerContext = SSL_CTX_new(TLS_server_method());
       break;
     case NET_REQUESTOR:
       transportLayerContext = SSL_CTX_new(TLS_client_method());
       break;
     case NET_ACCEPTORREQUESTOR:
       transportLayerContext = SSL_CTX_new(TLS_method());
       break;
   }

#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_SECURITY_LEVEL
   // starting with OpenSSL 1.1.0, we explicitly need to set the security level to 0
   // if we want to support any of the NULL ciphersuites. Since we manage the list
   // of supported ciphersuites ourselves and prevent a mix of NULL and non-NULL
   // ciphersuites, this is safe.
   if (transportLayerContext) SSL_CTX_set_security_level(transportLayerContext, 0);
#endif
#endif /* HAVE_OPENSSL_PROTOTYPE_TLS_METHOD */

   if (transportLayerContext == NULL)
   {
      const char *result = ERR_reason_error_string(ERR_get_error());
      if (result == NULL) result = "unknown error in SSL_CTX_new()";
      DCMTLS_ERROR("unable to create TLS transport layer: " << result);
   }
   else
   {
     // create default set of DH parameters
     if (!setBuiltInDHParameters())
       DCMTLS_ERROR("unable to create Diffie-Hellman parameters.");

     // set a random 32-bit number as TLS session ID
     OFRandom rnd;
     Uint32 session_id = rnd.getRND32();
     if (0 == SSL_CTX_set_session_id_context(transportLayerContext, OFreinterpret_cast(const unsigned char *, &session_id), sizeof(session_id)))
     {
       DCMTLS_ERROR("unable to set TLS session ID context.");
     }

     // disable session caching (and, thus, session re-use)
     SSL_CTX_set_session_cache_mode(transportLayerContext, SSL_SESS_CACHE_OFF);

     // create Elliptic Curve DH parameters
#ifndef OPENSSL_NO_ECDH
#ifndef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_ECDH_AUTO
     // we create ECDH parameters for the NIST P-256 (secp256r1) curve
     // as recommended by BCP 195.
     EC_KEY  *ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
     if (ecdh)
     {
       SSL_CTX_set_tmp_ecdh(transportLayerContext, ecdh);
       EC_KEY_free(ecdh); /* Safe because of reference counts */
     }
     else DCMTLS_ERROR("unable to create Elliptic-Curve Diffie-Hellman parameters.");
#else
    // OpenSSL 1.0.2 and newer have this function, which causes
    // the server to automatically select the most appropriate shared curve for each client.
    if (0 == SSL_CTX_set_ecdh_auto(transportLayerContext, 1))
    {
      DCMTLS_ERROR("unable to create Elliptic-Curve Diffie-Hellman parameters.");
    }
#endif /* HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_ECDH_AUTO */
#endif /* OPENSSL_NO_ECDH */

    // set default certificate verification strategy
    setCertificateVerification(DCV_requireCertificate);

#if HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_SIGALGS
    // The TLS 1.2 Signature Algorithms extension is only supported in OpenSSL 1.0.2 and newer.
    if (networkRole != NET_ACCEPTOR)
    {
      // BCP 195: Clients SHOULD indicate to servers that they request SHA-256,
      // by using the "Signature Algorithms" extension defined in TLS 1.2.
      // We implement this by requesting SHA-256 OR BETTER, i.e. we also indicate
      // support for SHA-384 and SHA-512.

      // RFC 9325 has strengthened this requirement to a MUST condition:
      // Clients MUST indicate to servers that they request SHA-256...

      const int slist[] = {NID_sha256, EVP_PKEY_RSA,     NID_sha384, EVP_PKEY_RSA,     NID_sha512, EVP_PKEY_RSA,
#ifdef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_RSA_PSS
                           // Connections between a client and a server that both use OpenSSL 1.1.1
                           // will fail unless RSA-PSS is also offered as a signature algorithm.
                           NID_sha256, EVP_PKEY_RSA_PSS, NID_sha384, EVP_PKEY_RSA_PSS, NID_sha512, EVP_PKEY_RSA_PSS,
#endif
                           NID_sha256, EVP_PKEY_DSA,     NID_sha384, EVP_PKEY_DSA,     NID_sha512, EVP_PKEY_DSA,
                           NID_sha256, EVP_PKEY_EC,      NID_sha384, EVP_PKEY_EC,      NID_sha512, EVP_PKEY_EC};

      if (0 == SSL_CTX_set1_sigalgs(transportLayerContext, slist, sizeof(slist)/sizeof(int)))
      {
        DCMTLS_ERROR("unable to configure the TLS 1.2 Signature Algorithms extension.");
      }
    }

#endif /* HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_SIGALGS */

#if HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_CURVES
    // The TLS Supported Elliptic Curves extension (RFC 4492) is only supported in OpenSSL 1.0.2 and newer.

    // BCP 195: Both clients and servers SHOULD include the "Supported Elliptic Curves" extension.
    // For interoperability, clients and servers SHOULD support the NIST P-256 (secp256r1) curve
    // (in OpenSSL this curve is called "prime256v1").

    OFVector<int> ecvector;
    computeEllipticCurveList(ecvector);
    if (ecvector.size() > 0) // only try to add the EC extension if we actually do support at least one curve
    {
      if (0 == SSL_CTX_set1_curves(transportLayerContext, &ecvector[0], OFstatic_cast(int, ecvector.size())))
      {
        DCMTLS_ERROR("unable to configure the TLS Supported Elliptic Curves extension.");
      }
    }
#endif /* HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_CURVES */

    if (networkRole != NET_ACCEPTOR)
    {
      if (0 != SSL_CTX_set_alpn_protos(transportLayerContext, alpn_dicom_protocol, alpn_dicom_protocol_len))
      {
        DCMTLS_ERROR("unable to configure the TLS Application-Layer Protocol Negotiation extension.");
      }
    }

    if (networkRole != NET_REQUESTOR)
    {
      SSL_CTX_set_alpn_select_cb(transportLayerContext, DcmTLSTransportLayer_ALPNCallback, NULL);
    }

    // activate the callback for incoming connections using SNI
    if (networkRole != NET_REQUESTOR)
    {
      SSL_CTX_set_tlsext_servername_callback(transportLayerContext, DcmTLSTransportLayer_SNICallback);
      SSL_CTX_set_tlsext_servername_arg(transportLayerContext, this);
    }

    if (NET_REQUESTOR != networkRole)
    {
      // BCP 195: Servers MUST prefer TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 over weaker cipher suites whenever it is proposed, even if it is not the first proposal.
      // BCP 195: Servers SHOULD prefer stronger cipher suites unless there are compelling reasons to choose otherwise
      // BCP 195: Implementations MUST support and prefer to negotiate cipher suites offering forward secrecy
      // This all requires that when acting as a server we select the ciphersuites by our order of preference,
      // which implements all three recommendations by sorting the list of supported ciphersuites appropriately.
      if (0 == SSL_CTX_set_options(transportLayerContext, SSL_OP_CIPHER_SERVER_PREFERENCE))
      {
        DCMTLS_ERROR("unable to configure the TLS layer to select ciphersuites by server preference.");
      }
    }
  } /* transportLayerContext != NULL */
}

// move constructor
DcmTLSTransportLayer::DcmTLSTransportLayer(OFrvalue_ref(DcmTLSTransportLayer) rhs)
: DcmTransportLayer(OFrvalue_ref_upcast(DcmTransportLayer, rhs))
, transportLayerContext(rhs.transportLayerContext)
, canWriteRandseed(OFmove(OFrvalue_access(rhs).canWriteRandseed))
, privateKeyPasswd(OFmove(OFrvalue_access(rhs).privateKeyPasswd))
{
  OFrvalue_access(rhs).transportLayerContext = NULL;
}

// move assignment
DcmTLSTransportLayer& DcmTLSTransportLayer::operator=(OFrvalue_ref(DcmTLSTransportLayer) rhs)
{
  if (this != &rhs)
  {
    clear();
    DcmTransportLayer::operator=(OFrvalue_ref_upcast(DcmTransportLayer, rhs));
    transportLayerContext = rhs.transportLayerContext;
    canWriteRandseed = OFmove(OFrvalue_access(rhs).canWriteRandseed);
    privateKeyPasswd = OFmove(OFrvalue_access(rhs).privateKeyPasswd);
    OFrvalue_access(rhs).transportLayerContext = NULL;
  }
  return *this;
}

void DcmTLSTransportLayer::clear()
{
  if (transportLayerContext)
  {
    SSL_CTX_free(transportLayerContext);
    transportLayerContext = NULL;
    canWriteRandseed = OFFalse;
    privateKeyPasswd.clear();
  }
}

DcmTLSTransportLayer::operator OFBool() const
{
  return !!transportLayerContext;
}

OFBool DcmTLSTransportLayer::operator!() const
{
  return !transportLayerContext;
}

OFBool DcmTLSTransportLayer::checkServerSNI(const char *s) const
{
  if (s && serverSNI)
  {
    OFString requestedSNI(s);

    // if a server name is set and a server name is requested, only succeed if these two match
    return (requestedSNI == serverSNI);
  }

  // if either name is NULL, we succeed
  return OFTrue;
}

const char *DcmTLSTransportLayer::getServerSNI() const
{
  if (serverSNI) return serverSNI;
  return "";
}

OFBool DcmTLSTransportLayer::setTempDHParameters(const char *filename)
{
  if ((filename==NULL)||(transportLayerContext==NULL)) return OFFalse;
  BIO *bio = BIO_new_file(filename,"r");
  if (bio)
  {
#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET0_TMP_DH_PKEY
    EVP_PKEY *dh = PEM_read_bio_Parameters(bio,NULL);
    BIO_free(bio);
    if (dh)
    {
      // check BCP 195 recommendation: With a key exchange based on modular
      // exponential (MODP) Diffie-Hellman groups ("DHE" cipher suites),
      // DH key lengths of at least 2048 bits are RECOMMENDED.
      if (EVP_PKEY_bits(dh) < 2048)
      {
          DCMTLS_WARN("Key length of Diffie-Hellman parameter file too short: RFC 9325 recommends at least 2048 bits, but the key in file '"
          << filename << "' is only " << EVP_PKEY_bits(dh) << " bits.");
          if (ciphersuites.getTLSProfile() == TSP_Profile_BCP195_Extended)
          {
              // Extended BCP 195 profile: Reject DH parameter set, because it has less than 2048 bits
              // This will cause the default DH parameter set (which is large enough) to be used
              EVP_PKEY_free(dh);
              return OFFalse;
          }
      }

      SSL_CTX_set0_tmp_dh_pkey(transportLayerContext, dh); // transfers ownership of "dh" to transportLayerContext
      return OFTrue;
    }
#else
    DH *dh = PEM_read_bio_DHparams(bio,NULL,NULL,NULL);
    BIO_free(bio);
    if (dh)
    {
      // check BCP 195 recommendation: With a key exchange based on modular
      // exponential (MODP) Diffie-Hellman groups ("DHE" cipher suites),
      // DH key lengths of at least 2048 bits are RECOMMENDED.
      if (DH_bits(dh) < 2048)
      {
          DCMTLS_WARN("Key length of Diffie-Hellman parameter file too short: RFC 9325 recommends at least 2048 bits, but the key in file '"
          << filename << "' is only " << DH_bits(dh) << " bits.");
          if (ciphersuites.getTLSProfile() == TSP_Profile_BCP195_Extended)
          {
              // Extended BCP 195 profile: Reject DH parameter set, because it has less than 2048 bits
              // This will cause the default DH parameter set (which is large enough) to be used
              DH_free(dh);
              return OFFalse;
          }
      }
      SSL_CTX_set_tmp_dh(transportLayerContext,dh);
      DH_free(dh); /* Safe because of reference counts in OpenSSL */
      return OFTrue;
    }
#endif
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

OFCondition DcmTLSTransportLayer::activateCipherSuites()
{
  OFString cslist;
  OFString tls13cslist;
  ciphersuites.getListOfCipherSuitesForOpenSSL(cslist, (role != NET_REQUESTOR));
  ciphersuites.getListOfTLS13CipherSuitesForOpenSSL(tls13cslist, (role != NET_REQUESTOR));

  if (transportLayerContext)
  {
    if (!SSL_CTX_set_cipher_list(transportLayerContext, cslist.c_str()))
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }

#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
    // we have TLS 1.3 support, so let's set the list of TLS 1.3 ciphersuites if we have any.
    // If that list is empty, the default list remains active
    if (tls13cslist.length() > 0)
    {
      if (!SSL_CTX_set_ciphersuites(transportLayerContext, tls13cslist.c_str()))
      {
        return convertOpenSSLError(ERR_get_error(), OFTrue);
      }
    }
#endif

    SSL_CTX_set_options(transportLayerContext, ciphersuites.getTLSOptions());

#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_MAX_PROTO_VERSION
    // when compiling with OpenSSL 1.1.1 or newer, set the maximum supported
    // TLS protocol version to TLS 1.2 if required.
    // This applies to some historic TLS profiles, which would otherwise
    // show unexpected behaviour, but also in the case that a DSA certificate
    // is in use, since TLS 1.3 does not support DSA certificates.
    // If we don't disable TLS 1.3 in this case, we may receive a TLS
    // negotiation failure with a "no suitable signature algorithm" error
    // under certain circumstances (e.g. if SNI is enabled).
    if ((! ciphersuites.isTLS13Enabled()) || certificateTypeIsDSA)
    {
      SSL_CTX_set_max_proto_version(transportLayerContext, TLS1_2_VERSION);
    }
#endif
  } else return EC_IllegalCall;

  return EC_Normal;
}

OFCondition DcmTLSTransportLayer::setCipherSuites(const char *suites)
{
  if (transportLayerContext && suites)
  {
    if (!SSL_CTX_set_cipher_list(transportLayerContext, suites))
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }
  } else return EC_IllegalCall;
  return EC_Normal;
}

DcmTLSTransportLayer::~DcmTLSTransportLayer()
{
  clear();
}

OFCondition DcmTLSTransportLayer::setPrivateKeyFile(const char *fileName, DcmKeyFileFormat fileType)
{
  if (transportLayerContext)
  {
    if (0 >= SSL_CTX_use_PrivateKey_file(transportLayerContext, fileName, lookupOpenSSLCertificateFormat(fileType)))
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }
  } else return EC_IllegalCall;
  return EC_Normal;
}


OFCondition DcmTLSTransportLayer::setCertificateFile(const char *fileName, DcmKeyFileFormat fileType, DcmTLSSecurityProfile profile)
{
  if (transportLayerContext)
  {
    // we load the first certificate from the file and check the key length
    // and hash key against RFC 9325 recommendations.
    int result = 0;
    int rsabits = 0;
    int ecdsabits = 0;
    int certificateType = 0;
    OFBool enforceRSALengthServer = OFFalse;
    OFBool enforceRSALengthClient = OFFalse;
    OFBool enforceECDSALength = OFFalse;
    OFBool enforceHashLength = OFFalse;
    OFBool refuseDSAcert = OFFalse;

    switch (profile)
    {
      case TSP_Profile_BCP_195_RFC_8996_Modified:
        enforceRSALengthClient = OFTrue; // refuse certificate if we are a client and the RSA key is shorter than 2048 bits
        enforceECDSALength = OFTrue; // refuse certificate if the ECDSA key is shorter than 256 bits
        enforceHashLength = OFTrue; // refuse certificate if hash is not SHA-256 or better
        /* fallthrough */
      case TSP_Profile_BCP_195_RFC_8996:
        enforceRSALengthServer = OFTrue; // refuse certificate if we are a server and the RSA key is shorter than 2048 bits
        refuseDSAcert = OFTrue; // refuse DSA certificates because they prevent the use of TLS 1.3
        break;
      default:
        break;
    }

    X509 *certificate = loadCertificateFile(fileName, fileType);
    if (certificate)
    {
      // reset certificate type flag
      certificateTypeIsDSA = OFFalse;

      EVP_PKEY *pubkey = X509_get_pubkey(certificate); // creates a copy of the public key
      if (pubkey)
      {
        certificateType = EVP_PKEY_base_id(pubkey);
        if (certificateType == EVP_PKEY_RSA)
        {
          rsabits = EVP_PKEY_bits(pubkey); // RSA public key size, in bits
        }
        if (certificateType == EVP_PKEY_EC)
        {
          ecdsabits = EVP_PKEY_bits(pubkey); // ECDSA public key size, in bits
        }
        EVP_PKEY_free(pubkey);
      }

      if ((rsabits > 0) && (rsabits < 2048))
      {
        if (enforceRSALengthServer && (role != NET_REQUESTOR))
        {
          DCMTLS_FATAL("Key length of RSA public key too short: RFC 9325 requires at least 2048 bits for server RSA keys, but the key in certificate file '"
            << fileName << "' is only " << rsabits << " bits.");
          return DCMTLS_EC_FailedToLoadCertificate(fileName);
        }
        if (enforceRSALengthClient)
        {
          DCMTLS_FATAL("Key length of RSA public key too short: TLS profile requires at least 2048 bits for RSA keys, but the key in certificate file '"
            << fileName << "' is only " << rsabits << " bits.");
          return DCMTLS_EC_FailedToLoadCertificate(fileName);
        }
        DCMTLS_WARN("Key length of RSA public key too short: RFC 9325 recommends at least 2048 bits for RSA keys, but the key in certificate file '"
          << fileName << "' is only " << rsabits << " bits.");
      }

      if ((ecdsabits > 0) && (ecdsabits < 256))
      {

        if (enforceECDSALength)
        {
          DCMTLS_FATAL("Key length of ECDSA public key too short: TLS profile requires at least 256 bits for ECDSA keys, but the key in certificate file '"
            << fileName << "' is only " << ecdsabits << " bits.");
          return DCMTLS_EC_FailedToLoadCertificate(fileName);
        }
        if (ecdsabits < 224)
        {
          DCMTLS_WARN("Key length of ECDSA public key too short: RFC 9325 recommends at least 224 bits for ECDSA keys, but the key in certificate file '"
            << fileName << "' is only " << ecdsabits << " bits.");
        }
      }

      if (certificateType == EVP_PKEY_DSA)
      {
        if (refuseDSAcert)
        {
          DCMTLS_FATAL("DSA certificate '" << fileName << "' not permitted in the selected TLS profile because it prevents the use of TLS 1.3.");
          return DCMTLS_EC_FailedToLoadCertificate(fileName);
        }
        else
        {
          DCMTLS_WARN("Use of DSA certificate not recommended because it prevents the use of TLS 1.3, which does not support this certificate type.");
          certificateTypeIsDSA = OFTrue;
        }
      }

      // check if the hash key used in the peer certificate is on our "blacklist" of weak hash key algorithms
      const char *hash = checkHashKeyIsTooInSecure(certificate);
      if (hash)
      {
        DCMTLS_ERROR("Weak certificate hash key: certificate file '" << fileName << "' uses '" << hash << "'.");
        return DCMTLS_EC_FailedToLoadCertificate(fileName);
      }

      // check if the hash key used in the peer certificate is on our "whitelist" of strong hash key algorithms
      hash = checkHashKeyIsSecure(certificate);
      if (hash)
      {
        if (enforceHashLength)
        {
          DCMTLS_FATAL("Weak certificate hash key: TLS profile requires SHA-256 (or better) for certificates, but certificate file '"
            << fileName << "' uses '" << hash << "'.");
          return DCMTLS_EC_FailedToLoadCertificate(fileName);
        }
        else
        {
          DCMTLS_WARN("Possibly weak certificate hash key: RFC 9325 recommends the use of SHA-256 (or better) for certificates, but certificate file '"
            << fileName << "' uses '" << hash << "'.");
        }
      }

      if (fileType == DCF_Filetype_PEM)
      {
        // This will load the file again, this time processing multiple certificates
        // that might be present, establishing a full certificate chain.
        // This function only works with PEM files.
        result = SSL_CTX_use_certificate_chain_file(transportLayerContext, fileName);
      }
      else
      {
        // copy certificate into the SSL context
        result = SSL_CTX_use_certificate(transportLayerContext, certificate);
      }
      X509_free(certificate);

    } else result = -1;

    if (result <= 0)
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }
  } else return DCMTLS_EC_FailedToLoadCertificate(fileName);
  return EC_Normal;
}

OFBool DcmTLSTransportLayer::checkPrivateKeyMatchesCertificate()
{
  if (transportLayerContext)
  {
    if (SSL_CTX_check_private_key(transportLayerContext)) return OFTrue;
  }
  return OFFalse;
}

OFCondition DcmTLSTransportLayer::addVerificationFlags(unsigned long flags)
{
  X509_VERIFY_PARAM* const parameter = DCMTK_SSL_CTX_get0_param(transportLayerContext);
  return parameter && X509_VERIFY_PARAM_set_flags(parameter,flags) ? EC_Normal : DCMTLS_EC_FailedToSetVerificationMode;
}

OFCondition DcmTLSTransportLayer::setCRLverification(DcmTLSCRLVerification crlmode)
{
  X509_VERIFY_PARAM* const parameter = DCMTK_SSL_CTX_get0_param(transportLayerContext);
  if (parameter)
  {
    unsigned long flags = X509_VERIFY_PARAM_get_flags(parameter);
    switch (crlmode)
    {
      case TCR_noCRL:
        flags &= ~X509_V_FLAG_CRL_CHECK;
        flags &= ~X509_V_FLAG_CRL_CHECK_ALL;
        break;
      case TCR_checkLeafCRL:
        flags |= X509_V_FLAG_CRL_CHECK;
        flags &= ~X509_V_FLAG_CRL_CHECK_ALL;
        break;
      case TCR_checkAllCRL:
        flags |= X509_V_FLAG_CRL_CHECK;
        flags |= X509_V_FLAG_CRL_CHECK_ALL;
        break;
    }
    return X509_VERIFY_PARAM_set_flags(parameter,flags) ? EC_Normal : DCMTLS_EC_FailedToSetVerificationMode;
  }
  return EC_IllegalCall;
}

OFCondition DcmTLSTransportLayer::addTrustedCertificateFile(const char *fileName, DcmKeyFileFormat fileType)
{
  if (transportLayerContext)
  {
    X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(SSL_CTX_get_cert_store(transportLayerContext), X509_LOOKUP_file());
    if (x509_lookup == NULL)
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }
    if (! X509_LOOKUP_load_file(x509_lookup, fileName, lookupOpenSSLCertificateFormat(fileType)))
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }
  } else return EC_IllegalCall;
  return EC_Normal;
}

OFCondition DcmTLSTransportLayer::addCertificateRevocationList(const char *fileName, DcmKeyFileFormat fileType)
{
  // OpenSSL uses the same X509_LOOKUP_load_file() function for both certificates and CRLs
  return addTrustedCertificateFile(fileName, fileType);
}

OFCondition DcmTLSTransportLayer::addTrustedCertificateDir(const char *pathName, DcmKeyFileFormat fileType)
{
  if (transportLayerContext)
  {
    X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(SSL_CTX_get_cert_store(transportLayerContext), X509_LOOKUP_hash_dir());
    if (x509_lookup == NULL)
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }
    if (! X509_LOOKUP_add_dir(x509_lookup, pathName, lookupOpenSSLCertificateFormat(fileType)))
    {
      return convertOpenSSLError(ERR_get_error(), OFTrue);
    }
  } else return EC_IllegalCall;
  return EC_Normal;
}

OFCondition DcmTLSTransportLayer::addTrustedClientCertificateFile(const char *fileName)
{
  if (transportLayerContext)
  {
    STACK_OF(X509_NAME) *caNames = sk_X509_NAME_dup(SSL_CTX_get_client_CA_list(transportLayerContext));
    if (caNames == NULL) caNames = sk_X509_NAME_new_null();
    STACK_OF(X509_NAME) *newCaNames = SSL_load_client_CA_file(fileName);
    for (int i = 0; i < sk_X509_NAME_num(newCaNames); ++i)
    {
      X509_NAME *newCaName = sk_X509_NAME_value(newCaNames,i);
      if (sk_X509_NAME_find(caNames,newCaName) == -1)
      {
        sk_X509_NAME_push(caNames,X509_NAME_dup(newCaName));
      }
    }
    sk_X509_NAME_pop_free(newCaNames,X509_NAME_free);
    SSL_CTX_set_client_CA_list(transportLayerContext,caNames);
  } else return EC_IllegalCall;
  return EC_Normal;
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
        int s = OFstatic_cast(int, openSocket);
        if (openSocket != OFstatic_cast(DcmNativeSocketType, s))
        {
          // On Win64, the native type for sockets there is an unsigned 64-bit integer,
          // and OpenSSL uses a signed 32-bit int file descriptor.
          // This should be fixed in OpenSSL, there is nothing we can do here
          // except to check whether the type conversion truncates the value and,
          // in this case, issue an error message.
          DCMTLS_ERROR("Conversion of 64-bit socket type to int in OpenSSL API causes loss of information.");
        }
        SSL_set_fd(newConnection, s);

        if (clientSNI && (role != NET_ACCEPTOR))
        {
          if (0 == SSL_set_tlsext_host_name(newConnection, clientSNI))
          {
            DCMTLS_WARN("Unable to set the server name for the TLS SNI extension, ignoring.");
          }
        }

        // store a pointer to this DcmTLSTransportLayer instance in the SSL object,
        // for use by the certificate verification callback
        SSL_set_ex_data(newConnection, contextStoreIndex, this);

        return new DcmTLSConnection(openSocket, newConnection);
      }
    }
    return NULL;
  }
  else return DcmTransportLayer::createConnection(openSocket, useSecureLayer);
}

void DcmTLSTransportLayer::seedPRNG(const char *randFile)
{
#ifdef _WIN32
  RAND_screen();
#endif
  if (randFile)
  {
#ifdef HAVE_OPENSSL_PROTOTYPE_RAND_EGD
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
    out << "Peer X.509v" << certVersion << " Certificate" << OFendl
        << "  Subject     : " << certSubjectName << OFendl
        << "  Issued by   : " << certIssuerName << OFendl
        << "  Serial no.  : " << certSerialNumber << OFendl
        << "  Validity    : not before " << certValidNotBefore << ", not after " << certValidNotAfter << OFendl
        << "  Public key  : " << certPubKeyType << ", " << certPubKeyBits << " bits" << OFStringStream_ends;
    OFSTRINGSTREAM_GETOFSTRING(out, ret)
    return ret;
  } else {
    return "Peer did not provide a certificate or certificate verification is disabled.";
  }
}

OFCondition DcmTLSTransportLayer::setTLSProfile(DcmTLSSecurityProfile profile)
{
  return ciphersuites.setTLSProfile(profile);
}

DcmTLSSecurityProfile DcmTLSTransportLayer::getTLSProfile() const
{
  return ciphersuites.getTLSProfile();
}

void DcmTLSTransportLayer::clearTLSProfile()
{
  ciphersuites.clearTLSProfile();
}

OFCondition DcmTLSTransportLayer::addCipherSuite(const char *suite)
{
  return ciphersuites.addCipherSuite(suite);
}

DcmTLSTransportLayer::native_handle_type DcmTLSTransportLayer::getNativeHandle()
{
  return transportLayerContext;
}

int DcmTLSTransportLayer::lookupOpenSSLCertificateFormat(DcmKeyFileFormat fileType)
{
  int result = -1;
  switch (fileType)
  {
    case DCF_Filetype_PEM:
      result = SSL_FILETYPE_PEM;
      break;
    case DCF_Filetype_ASN1:
      result = SSL_FILETYPE_ASN1;
      break;
  }
  return result;
}


void DcmTLSTransportLayer::printSupportedCiphersuites(STD_NAMESPACE ostream& os) const
{
  ciphersuites.printSupportedCiphersuites(os);
}

void DcmTLSTransportLayer::getListOfCipherSuitesForOpenSSL(OFString& cslist) const
{
  ciphersuites.getListOfCipherSuitesForOpenSSL(cslist, (role != NET_REQUESTOR));
}

int DcmTLSTransportLayer::getRSAKeySize(X509 *certificate)
{
  int result = 0; // default for non-RSA keys
  if (certificate)
  {
    EVP_PKEY *pubkey = X509_get_pubkey(certificate); // creates a copy of the public key
    if (pubkey)
    {
      if (EVP_PKEY_base_id(pubkey) == EVP_PKEY_RSA)
      {
        result = EVP_PKEY_bits(pubkey); // RSA public key size, in bits
      }
      EVP_PKEY_free(pubkey);
    }
  }
  return result;
}


const char *DcmTLSTransportLayer::checkHashKeyIsTooInSecure(X509 *certificate)
{
  if (certificate)
  {
      // this method implements a blacklist of hash key algorithms
      // that we never accept, because they are too insecure
      int nid = X509_get_signature_nid(certificate);
      switch (nid)
      {
        case NID_md2WithRSAEncryption: // MD2
        case NID_md2:
          return "MD2";
        case NID_md4WithRSAEncryption: // MD4
        case NID_md4:
          return "MD4";
        case NID_md5WithRSAEncryption: // MD5
        case NID_md5:
        case NID_md5WithRSA:
          return "MD5";
        default:
         break;
      }
  }
  return NULL;
}


const char *DcmTLSTransportLayer::checkHashKeyIsSecure(X509 *certificate)
{
  if (certificate)
  {
      int nid = X509_get_signature_nid(certificate);
      switch (nid)
      {
          // RSA with SHA-2
          case NID_sha256WithRSAEncryption:
          case NID_sha384WithRSAEncryption:
          case NID_sha512WithRSAEncryption:
#ifdef HAVE_OPENSSL_PROTOTYPE_NID_SHA512_256WITHRSAENCRYPTION
          // we have support for SHA-512_256
          case NID_sha512_256WithRSAEncryption:
#endif

#ifdef HAVE_OPENSSL_PROTOTYPE_NID_ECDSA_WITH_SHA3_256
          // We have SHA-3 support. Accept RSA with SHA-3
          case NID_RSA_SHA3_256:
          case NID_RSA_SHA3_384:
          case NID_RSA_SHA3_512:
#endif

          // ECDSA with SHA-2
          case NID_ecdsa_with_SHA256:
          case NID_ecdsa_with_SHA384:
          case NID_ecdsa_with_SHA512:

#ifdef HAVE_OPENSSL_PROTOTYPE_NID_ECDSA_WITH_SHA3_256
          // We have SHA-3 support. Accept ECDSA with SHA-3
          case NID_ecdsa_with_SHA3_256:
          case NID_ecdsa_with_SHA3_384:
          case NID_ecdsa_with_SHA3_512:
#endif

          // DSA with SHA-2
          case NID_dsa_with_SHA256:

#ifdef HAVE_OPENSSL_PROTOTYPE_NID_DSA_WITH_SHA512
          // We have support for DSA with SHA-384 and SHA-512
          case NID_dsa_with_SHA384:
          case NID_dsa_with_SHA512:
#endif

#ifdef HAVE_OPENSSL_PROTOTYPE_NID_ECDSA_WITH_SHA3_256
          // We have SHA-3 support. Accept DSA with SHA-3
          case NID_dsa_with_SHA3_256:
          case NID_dsa_with_SHA3_384:
          case NID_dsa_with_SHA3_512:
#endif

          return NULL; // hash key is on the "whitelist"
        default:
          return OBJ_nid2sn(nid); // hash key is not on our "whitelist", return the name
    }
  }
  return NULL; // default: everything is OK
}

X509 *DcmTLSTransportLayer::loadCertificateFile(const char *fileName, DcmKeyFileFormat fileType)
{
  X509 *result = NULL;
  BIO *in=BIO_new_file(fileName, "rb");
  if (in)
  {
    if (fileType == DCF_Filetype_ASN1)
    {
      result=d2i_X509_bio(in,NULL);
    }
    else if (fileType == DCF_Filetype_PEM)
    {
      result=PEM_read_bio_X509(in, NULL, NULL, NULL);
    }
    BIO_free(in);
  }
  return result;
}

OFCondition DcmTLSTransportLayer::verifyClientCertificate(const char *fileName, DcmKeyFileFormat fileType)
{
  OFCondition result = EC_IllegalCall;
  if (transportLayerContext && fileName)
  {
    X509_STORE *trustStore = SSL_CTX_get_cert_store(transportLayerContext);
    if (trustStore)
    {

      // for some reason, the SSL context and the X509_STORE within that
      // context have different X509_VERIFY_PARAM parameter sets, in particular
      // they have different verification flags. We copy the flags from the
      // SSL context to the X509_STORE and restore the original value
      // after certificate verification.
      X509_VERIFY_PARAM *vparam_ssl = DCMTK_SSL_CTX_get0_param(transportLayerContext);
      X509_VERIFY_PARAM *vparam_store = X509_STORE_get0_param(trustStore);
      unsigned long ssl_vparam_flags = 0;
      unsigned long store_vparam_flags = 0;
      if (vparam_ssl) ssl_vparam_flags = X509_VERIFY_PARAM_get_flags(vparam_ssl);
      if (vparam_store)
      {
        store_vparam_flags = X509_VERIFY_PARAM_get_flags(vparam_store);
        X509_VERIFY_PARAM_set_flags(vparam_store, ssl_vparam_flags);
      }

      X509_STORE_CTX *storeCtx = X509_STORE_CTX_new();
      if (storeCtx)
      {
        // we have a trust store and a context object for certificate verification.
        // Now let's load the client certificate chain
        X509 *clientCert = NULL;
        STACK_OF(X509) *chain = sk_X509_new(NULL);
        BIO *in=BIO_new_file(fileName, "rb");
        if (in)
        {
          if (fileType == DCF_Filetype_ASN1)
          {
            clientCert = d2i_X509_bio(in,NULL);
            if (clientCert == NULL)
            {
              result = DCMTLS_EC_FailedToLoadCertificate(fileName);
              DCMTLS_ERROR("Not a DER certificate file: '" << fileName << "'");
            }
          }
          else if (fileType == DCF_Filetype_PEM)
          {
            clientCert = PEM_read_bio_X509(in, NULL, NULL, NULL);
            if (clientCert == NULL)
            {
              result = DCMTLS_EC_FailedToLoadCertificate(fileName);
              DCMTLS_ERROR("Not a PEM certificate file: '" << fileName << "'");
            }
            // in a PEM file, a certificate chain may follow after the client certificate.
            X509 *chainCert = NULL;
            while (NULL != (chainCert = PEM_read_bio_X509(in, NULL, NULL, NULL)))
            {
              sk_X509_push(chain, chainCert);
            }
          }
          BIO_free(in);
        }
        else
        {
          result = DCMTLS_EC_FailedToLoadCertificate(fileName);
          DCMTLS_ERROR("Cannot open certificate file '" << fileName << "'");
        }
        if (clientCert)
        {
          if (X509_STORE_CTX_init(storeCtx, trustStore, clientCert, chain))
          {
            if (X509_verify_cert(storeCtx))
            {
              result = EC_Normal;
            }
            else
            {
              result = convertOpenSSLX509VerificationError(X509_STORE_CTX_get_error(storeCtx), OFTrue);
            }
          }
          else
          {
            result = DCMTLS_EC_CertStoreCtxInitFailed;
            DCMTLS_ERROR("certificate store context initialization failed");
          }
          X509_free(clientCert);
        }

        X509_STORE_CTX_free(storeCtx);
        sk_X509_pop_free(chain, X509_free);
      }

      // restore original value of X509 store flags
      if (vparam_store)
      {
        X509_VERIFY_PARAM_set_flags(vparam_store, store_vparam_flags);
      }

    }
  }
  return result;
}

OFCondition DcmTLSTransportLayer::isRootCertificate(const char *fileName, DcmKeyFileFormat fileType)
{
  OFCondition result = EC_IllegalCall;
  if (fileName)
  {
    X509_STORE *trustStore = X509_STORE_new();
    X509_STORE_CTX *storeCtx = X509_STORE_CTX_new();
    if (trustStore && storeCtx)
    {
      // we have a trust store and a context object for certificate verification.
      // Now let's load the client certificate
      X509 *clientCert = loadCertificateFile(fileName, fileType);
      if (clientCert == NULL)
      {
        result = DCMTLS_EC_FailedToLoadCertificate(fileName);
        DCMTLS_ERROR("Cannot read certificate file '" << fileName << "'");
      }
      else
      {
        if (X509_STORE_add_cert(trustStore, clientCert))
        {
          if (X509_STORE_CTX_init(storeCtx, trustStore, clientCert, NULL))
          {
            if (X509_verify_cert(storeCtx)) result = EC_Normal;
              else result = convertOpenSSLX509VerificationError(X509_STORE_CTX_get_error(storeCtx), OFFalse);
          } else result = DCMTLS_EC_CertStoreCtxInitFailed;
        } else result = DCMTLS_EC_FailedToLoadCertificate(fileName);;
      }
      X509_free(clientCert);
    }
    if (storeCtx) X509_STORE_CTX_free(storeCtx);
    if (trustStore) X509_STORE_free(trustStore);
  }
  return result;
}

OFCondition DcmTLSTransportLayer::convertOpenSSLError(unsigned long errorCode, OFBool logAsError)
{
    if (errorCode == 0) return EC_Normal;

    const char *err = ERR_reason_error_string(errorCode);
    if (err == NULL) err = "OpenSSL error";

    // we generate special error codes for SSL errors
    if (ERR_LIB_SSL == ERR_GET_LIB(errorCode))
    {

      OFOStringStream os;
      os << "TLS error: " << err;

      OFCondition cond;
      OFSTRINGSTREAM_GETSTR( os, c )
      if (logAsError) DCMTLS_ERROR(c);
      cond = makeOFCondition(OFM_dcmtls, DCMTLS_EC_SSL_Offset + ERR_GET_REASON(errorCode), OF_error,  c);
      OFSTRINGSTREAM_FREESTR( c )

      return cond;
    }
    else
    {
      if (logAsError) DCMTLS_ERROR("OpenSSL error " << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(8) << errorCode << ": " << err);

      // we return a generic OpenSSL error for all other OpenSSL sublibraries
      return DCMTLS_EC_GenericOpenSSLError(errorCode);
    }
}

OFCondition DcmTLSTransportLayer::convertOpenSSLX509VerificationError(int errorCode, OFBool logAsError)
{
    if (errorCode == 0) return EC_Normal;

    // check if this is a known error code, map to "unspecified error" otherwise and print a warning
    if (errorCode > DCMTLS_EC_X509Verify_Max)
    {
      DCMTLS_WARN("Unsupported OpenSSL X.509 verification error code " << errorCode << "; mapped to DCMTLS_EC_X509VerifyUnspecified.");
      errorCode = X509_V_ERR_UNSPECIFIED;
    }

    // retrieve error string
    const char *err = X509_verify_cert_error_string(errorCode);
    if (err == NULL) err = "unspecified error.";

    if (logAsError) DCMTLS_ERROR("certificate verification failed: " << err);

    return makeOFCondition(OFM_dcmtls, OFstatic_cast(Uint16, DCMTLS_EC_X509Verify_Offset + errorCode), OF_error,  err);
}

void DcmTLSTransportLayer::initializeOpenSSL()
{
  // initialize OpenSSL library
#if defined(HAVE_OPENSSL_PROTOTYPE_OPENSSL_INIT_CRYPTO) && defined(OPENSSL_INIT_ATFORK)
  (void) OPENSSL_init_crypto(OPENSSL_INIT_ATFORK, NULL);
#endif
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();

  // generate a globally unique index that can be used to store application
  // specific data in the SSL structure. This only needs to be done once.
  contextStoreIndex = SSL_get_ex_new_index(0, &contextStoreIndex, NULL, NULL, NULL);
}

const char *DcmTLSTransportLayer::getOpenSSLVersionName()
{
  return OPENSSL_VERSION_TEXT;
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
