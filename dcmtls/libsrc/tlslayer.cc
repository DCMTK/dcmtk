/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:46 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <winbase.h>
#endif
#include <openssl/rand.h>
#include <openssl/err.h>
END_EXTERN_C

#include "dcmtk/dcmtls/tlslayer.h"
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmnet/dicom.h"

extern "C" int DcmTLSTransportLayer_certificateValidationCallback(int ok, X509_STORE_CTX *storeContext);

OFLogger DCM_dcmtlsGetLogger()
{
    // We don't just use a global variable, because constructors of globals are
    // executed in random order. This guarantees that the OFLogger is constructed
    // before first use.
    static OFLogger DCM_dcmtlsLogger = OFLog::getLogger("dcmtk.dcmtls");
    return DCM_dcmtlsLogger;
}

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
    {"TLS_RSA_EXPORT_WITH_RC4_40_MD5",          SSL3_TXT_RSA_RC4_40_MD5},
    {"TLS_RSA_WITH_RC4_128_MD5",                SSL3_TXT_RSA_RC4_128_MD5},
    {"TLS_RSA_WITH_RC4_128_SHA",                SSL3_TXT_RSA_RC4_128_SHA},
    {"TLS_RSA_EXPORT_WITH_RC2_CBC_40_MD5",      SSL3_TXT_RSA_RC2_40_MD5},
    {"TLS_RSA_WITH_IDEA_CBC_SHA",               SSL3_TXT_RSA_IDEA_128_SHA},
    {"TLS_RSA_EXPORT_WITH_DES40_CBC_SHA",       SSL3_TXT_RSA_DES_40_CBC_SHA},
    {"TLS_RSA_WITH_DES_CBC_SHA",                SSL3_TXT_RSA_DES_64_CBC_SHA},
    {"TLS_RSA_WITH_3DES_EDE_CBC_SHA",           SSL3_TXT_RSA_DES_192_CBC3_SHA},
    {"TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA",    SSL3_TXT_DH_DSS_DES_40_CBC_SHA},
    {"TLS_DH_DSS_WITH_DES_CBC_SHA",             SSL3_TXT_DH_DSS_DES_64_CBC_SHA},
    {"TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA",        SSL3_TXT_DH_DSS_DES_192_CBC3_SHA},
    {"TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA",    SSL3_TXT_DH_RSA_DES_40_CBC_SHA},
    {"TLS_DH_RSA_WITH_DES_CBC_SHA",             SSL3_TXT_DH_RSA_DES_64_CBC_SHA},
    {"TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA",        SSL3_TXT_DH_RSA_DES_192_CBC3_SHA},
    {"TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA",   SSL3_TXT_EDH_DSS_DES_40_CBC_SHA},
    {"TLS_DHE_DSS_WITH_DES_CBC_SHA",            SSL3_TXT_EDH_DSS_DES_64_CBC_SHA},
    {"TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA",       SSL3_TXT_EDH_DSS_DES_192_CBC3_SHA},
    {"TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA",   SSL3_TXT_EDH_RSA_DES_40_CBC_SHA},
    {"TLS_DHE_RSA_WITH_DES_CBC_SHA",            SSL3_TXT_EDH_RSA_DES_64_CBC_SHA},
    {"TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA",       SSL3_TXT_EDH_RSA_DES_192_CBC3_SHA},
    {"TLS_DH_anon_EXPORT_WITH_RC4_40_MD5",      SSL3_TXT_ADH_RC4_40_MD5},
    {"TLS_DH_anon_WITH_RC4_128_MD5",            SSL3_TXT_ADH_RC4_128_MD5},
    {"TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA",   SSL3_TXT_ADH_DES_40_CBC_SHA},
    {"TLS_DH_anon_WITH_DES_CBC_SHA",            SSL3_TXT_ADH_DES_64_CBC_SHA},
    {"TLS_DH_anon_WITH_3DES_EDE_CBC_SHA",       SSL3_TXT_ADH_DES_192_CBC_SHA},
    {"TLS_RSA_EXPORT1024_WITH_DES_CBC_SHA",     TLS1_TXT_RSA_EXPORT1024_WITH_DES_CBC_SHA},
    {"TLS_RSA_EXPORT1024_WITH_RC4_56_SHA",      TLS1_TXT_RSA_EXPORT1024_WITH_RC4_56_SHA},
    {"TLS_DHE_DSS_EXPORT1024_WITH_DES_CBC_SHA", TLS1_TXT_DHE_DSS_EXPORT1024_WITH_DES_CBC_SHA},
    {"TLS_DHE_DSS_EXPORT1024_WITH_RC4_56_SHA",  TLS1_TXT_DHE_DSS_EXPORT1024_WITH_RC4_56_SHA},
    {"TLS_DHE_DSS_WITH_RC4_128_SHA",            TLS1_TXT_DHE_DSS_WITH_RC4_128_SHA}

#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
    // cipersuites added in OpenSSL 0.9.7
    ,
    {"TLS_RSA_EXPORT_WITH_RC4_56_MD5",          TLS1_TXT_RSA_EXPORT1024_WITH_RC4_56_MD5},
    {"TLS_RSA_EXPORT_WITH_RC2_CBC_56_MD5",      TLS1_TXT_RSA_EXPORT1024_WITH_RC2_CBC_56_MD5},

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

DcmTLSTransportLayer::DcmTLSTransportLayer(int networkRole, const char *randFile)
: DcmTransportLayer(networkRole)
, transportLayerContext(NULL)
, canWriteRandseed(OFFalse)
, privateKeyPasswd()
{
   // the call to SSL_library_init was not needed in OpenSSL versions prior to 0.9.8,
   // but the API has been available at least since 0.9.5.
   SSL_library_init();
   SSL_load_error_strings();
   SSLeay_add_all_algorithms();
   seedPRNG(randFile);

   if (networkRole == DICOM_APPLICATION_ACCEPTOR)
   {
     transportLayerContext = SSL_CTX_new(TLSv1_server_method());
   } else if (networkRole == DICOM_APPLICATION_REQUESTOR) {
     transportLayerContext = SSL_CTX_new(TLSv1_client_method());
   } else {
     transportLayerContext = SSL_CTX_new(TLSv1_method());
   }

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
    if (!SSL_CTX_set_cipher_list(transportLayerContext, suites)) return TCS_tlsError;
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
    if (0 >= SSL_CTX_use_PrivateKey_file(transportLayerContext, fileName, fileType)) return TCS_tlsError;
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTransportLayerStatus DcmTLSTransportLayer::setCertificateFile(const char *fileName, int fileType)
{
  /* fileType should be SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM */
  if (transportLayerContext)
  {
    if (0 >= SSL_CTX_use_certificate_file(transportLayerContext, fileName, fileType)) return TCS_tlsError;
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

DcmTransportLayerStatus DcmTLSTransportLayer::addTrustedCertificateFile(const char *fileName, int fileType)
{
  /* fileType should be SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM */
  if (transportLayerContext)
  {
    X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(transportLayerContext->cert_store, X509_LOOKUP_file());
    if (x509_lookup == NULL) return TCS_tlsError;
    if (! X509_LOOKUP_load_file(x509_lookup, fileName, fileType)) return TCS_tlsError;
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTransportLayerStatus DcmTLSTransportLayer::addTrustedCertificateDir(const char *pathName, int fileType)
{
  /* fileType should be SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM */
  if (transportLayerContext)
  {
    X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(transportLayerContext->cert_store, X509_LOOKUP_hash_dir());
    if (x509_lookup == NULL) return TCS_tlsError;
    if (! X509_LOOKUP_add_dir(x509_lookup, pathName, fileType)) return TCS_tlsError;
  } else return TCS_illegalCall;
  return TCS_ok;
}

DcmTransportConnection *DcmTLSTransportLayer::createConnection(int openSocket, OFBool useSecureLayer)
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
    if (RAND_egd(randFile) <= 0)
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
      switch (EVP_PKEY_type(pubkey->type))
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
         << "  Subject      : " << certSubjectName << OFendl
         << "  Issued by    : " << certIssuerName << OFendl
         << "  Serial no.   : " << certSerialNumber << OFendl
         << "  Validity     : not before " << certValidNotBefore << ", not after " << certValidNotAfter << OFendl
         << "  Public key   : " << certPubKeyType << ", " << certPubKeyBits << " bits" << OFendl << OFStringStream_ends;
    OFSTRINGSTREAM_GETOFSTRING(out, ret)
    return ret;
  } else {
    return "No X.509 Certificate.";
  }
}

#else  /* WITH_OPENSSL */

/* make sure that the object file is not completely empty if compiled
 * without OpenSSL because some linkers might fail otherwise.
 */
void tlslayer_dummy_function()
{
  return;
}

#endif /* WITH_OPENSSL */

/*
 *  $Log: tlslayer.cc,v $
 *  Revision 1.19  2010-10-14 13:14:46  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.18  2010-08-05 08:38:11  uli
 *  Fixed some warnings from -Wold-style-cast.
 *
 *  Revision 1.17  2010-06-02 12:32:58  joergr
 *  Appended missing OFStringStream_ends to the end of output streams because
 *  this is required when OFOStringStream is mapped to ostrstream.
 *
 *  Revision 1.16  2010-02-22 11:39:55  uli
 *  Remove some unneeded includes.
 *
 *  Revision 1.15  2009-12-18 13:15:45  joergr
 *  Fixed issue with beta version of OpenSSL 1.0 (const declaration changed).
 *
 *  Revision 1.14  2009-11-18 12:11:19  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.13  2009-01-30 13:53:53  joergr
 *  Replaced checking of macro WINDOWS by _WIN32.
 *
 *  Revision 1.12  2006/08/15 16:02:56  meichel
 *  Updated the code in module dcmtls to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.11  2005/12/08 15:48:28  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2005/11/23 16:10:28  meichel
 *  Added support for AES ciphersuites in TLS module. All TLS-enabled
 *    tools now support the "AES TLS Secure Transport Connection Profile".
 *
 *  Revision 1.9  2003/12/18 17:16:05  meichel
 *  Added standard includes needed by Borland Builder
 *
 *  Revision 1.8  2001/06/05 10:32:55  joergr
 *  Replaced some #ifdef _WIN32 statements by #ifdef HAVE_WINDOWS_H or #ifdef
 *  __CYGWIN__ respectively to reflect the fact that the latest Cygwin/gcc
 *  version does not define _WIN32 any more.
 *
 *  Revision 1.7  2001/06/01 15:51:13  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/11/14 13:54:29  meichel
 *  Renamed callback functions to avoid linker name clashes
 *
 *  Revision 1.5  2000/11/08 11:21:13  meichel
 *  iFixed trailing garbage characters problem in extracting validity
 *    information from a X.509 certificate.
 *
 *  Revision 1.4  2000/10/19 08:12:29  meichel
 *  Fixed dcmtls module so that openssl headers are includes only
 *    if the symbol WITH_OPENSSL is defined.
 *
 *  Revision 1.3  2000/10/11 16:15:35  meichel
 *  Updated includes for Win32 environment
 *
 *  Revision 1.2  2000/10/10 12:13:35  meichel
 *  Added routines for printing certificates and connection parameters.
 *
 *  Revision 1.1  2000/08/10 14:50:29  meichel
 *  Added initial OpenSSL support.
 *
 *
 */
