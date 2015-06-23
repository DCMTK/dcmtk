/*
 *
 *  Copyright (C) 1998-2015, OFFIS e.V.
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

#ifndef TLSLAYER_H
#define TLSLAYER_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcmlayer.h"    /* for DcmTransportLayer */
#include "dcmtk/ofstd/ofstream.h"    /* for ostream */
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmtls/tlsdefin.h"

#ifdef WITH_OPENSSL
BEGIN_EXTERN_C
#include <openssl/ssl.h>
END_EXTERN_C

extern DCMTK_DCMTLS_EXPORT OFLogger DCM_dcmtlsLogger;

#define DCMTLS_TRACE(msg) OFLOG_TRACE(DCM_dcmtlsLogger, msg)
#define DCMTLS_DEBUG(msg) OFLOG_DEBUG(DCM_dcmtlsLogger, msg)
#define DCMTLS_INFO(msg)  OFLOG_INFO(DCM_dcmtlsLogger, msg)
#define DCMTLS_WARN(msg)  OFLOG_WARN(DCM_dcmtlsLogger, msg)
#define DCMTLS_ERROR(msg) OFLOG_ERROR(DCM_dcmtlsLogger, msg)
#define DCMTLS_FATAL(msg) OFLOG_FATAL(DCM_dcmtlsLogger, msg)

/** this enum describes how to handle X.509 certificates on a TLS based
 *  secure transport connection. They can be ignored, validated if present
 *  or validated and demanded.
 */
enum DcmCertificateVerification
{
  /** check peer certificate, fail if no certificate is present
   */
  DCV_requireCertificate,

  /** check peer certificate if present, succeed if no certificate is present
   */
  DCV_checkCertificate,

  /** do not check peer certificate
   */
  DCV_ignoreCertificate
};


/** factory class which creates secure TLS transport layer connections
 *  and maintains the parameters common to all TLS transport connections
 *  in one application (e.g. the pool of trusted certificates, the key
 *  and certificate to be used for authentication and the list of
 *  ciphersuite to be used for association negotiation.
 */

class DCMTK_DCMTLS_EXPORT DcmTLSTransportLayer: public DcmTransportLayer
{
public:

  /** a type alias for the type of the underlying OpenSSL context handle to be
   *  used in conjunction with the getNativeHandle() member function.
   */
  typedef SSL_CTX* native_handle_type;

  /** constructor.
   *  @param networkRole network role to be used by the application, influences
   *    the choice of the secure transport layer code.
   *  @param randFile path to file used to feed the random generator
   *  @param initializeOpenSSL Determines if OpenSSL library should be initialized.
   *    Some setups (e.g. multi-threaded environments) may be interested in using
   *    more than one TLS transport layer at a time and thus must make sure the
   *    library is only initialized once.
   */
  DcmTLSTransportLayer(int networkRole, const char *randFile, OFBool initializeOpenSSL = OFTrue);

  /// destructor
  virtual ~DcmTLSTransportLayer();

  /** factory method that returns a new transport connection for the
   *  given socket.  Depending on the second parameter, either a transparent
   *  or a secure connection is established.  If the object cannot be created
   *  (e. g. because no secure layer is available), returns NULL.
   *  @param openSocket TCP/IP socket to be used for the transport connection.
   *    the connection must already be establised on socket level. If a non-null
   *    pointer is returned, the new connection object takes over control of the socket.
   *  @param useSecureLayer if true, a secure layer is used. If false, a
   *    transparent layer is used.
   *  @return pointer to new connection object if successful, NULL otherwise.
   */
  virtual DcmTransportConnection *createConnection(int openSocket, OFBool useSecureLayer);

  /** loads the private key used for authentication of this application from a file.
   *  @param fileName path to the private key file
   *  @param fileType, must be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1
   *  @return TCS_ok if successful, an error code otherwise
   */
  DcmTransportLayerStatus setPrivateKeyFile(const char *fileName, int fileType);

  /** loads the certificate (public key) used for authentication of this application from a file.
   *  @param fileName path to the certificate file
   *  @param fileType, must be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1
   *  @return TCS_ok if successful, an error code otherwise
   */
  DcmTransportLayerStatus setCertificateFile(const char *fileName, int fileType);

  /** checks if the private key and the certificate set using setPrivateKeyFile()
   *  and setCertificateFile() match, i.e. if they establish a private/public key pair.
   *  @return OFTrue if private key and certificate match, OFFalse otherwise.
   */
  OFBool checkPrivateKeyMatchesCertificate();

  /** loads a certificate from a file and adds it to the pool of trusted certificates.
   *  @param fileName path to the certificate file
   *  @param fileType, must be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1
   *  @return TCS_ok if successful, an error code otherwise
   */
  DcmTransportLayerStatus addTrustedCertificateFile(const char *fileName, int fileType);

  /** loads all files as certificates from the specified directory and adds them
   *  to the pool of trusted certificates.
   *  @param fileName path to the directory containing certificate files
   *  @param fileType, must be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1
   *  @return TCS_ok if successful, an error code otherwise
   */
  DcmTransportLayerStatus addTrustedCertificateDir(const char *pathName, int fileType);

  /** loads certificates from a file and adds them to the pool of trusted client
   *  certificates.
   *  @param fileName path to the certificate file
   *  @return TCS_ok if successful, an error code otherwise
   */
  DcmTransportLayerStatus addTrustedClientCertificateFile(const char *fileName);

  /** appends the given verification flags to the existing ones in this OpenSSL context
   *  (using binary or).
   *  @warning Documentation for the underlying OpenSSL functions is not available,
   *    therefore, these semantics were guessed based on looking at the OpenSSL source
   *    code!
   *  @param flags the verification flags to append, e. g. X509_V_FLAG_CRL_CHECK.
   *  @return TCS_ok if the flags were appended to the existing ones, TCS_unspecifiedError
   *    if OpenSSL returns an (unspecified, since the documentation is missing) error.
   */
  DcmTransportLayerStatus addVerificationFlags(unsigned long flags);

  /** sets the list of ciphersuites to negotiate.
   *  @param suites string containing the list of ciphersuites.
   *    The list must be in OpenSSL syntax (use findOpenSSLCipherSuiteName to convert
   *    from RFC 2246 ciphersuite names to OpenSSL names), with ciphersuites separated
   *    by ':' characters.
   *  @return TCS_ok if successful, an error code otherwise
   */
  DcmTransportLayerStatus setCipherSuites(const char *suites);

  /** returns the number of known ciphersuites.
   *  @return number of known ciphersuites
   */
  static unsigned long getNumberOfCipherSuites();

  /** returns a ciphersuite name in RFC 2246 (TLS) form
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return ciphersuite name
   */
  static const char *getTLSCipherSuiteName(unsigned long idx);

  /** returns a ciphersuite name in OpenSSL form
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return ciphersuite name
   */
  static const char *getOpenSSLCipherSuiteName(unsigned long idx);

  /** finds the OpenSSL ciphersuite name for a given RFC 2246 ciphersuite name.
   *  @param tlsCipherSuiteName ciphersuite name in RFC 2246 form
   *  @returns ciphersuite name in OpenSSL form, NULL if unknown.
   */
  static const char *findOpenSSLCipherSuiteName(const char *tlsCipherSuiteName);

  /** checks if enough entropy data is available to write back a modified
   *  random seed file.
   *  @return OFTrue if random seed file can be written, OFFalse otherwise.
   */
  OFBool canWriteRandomSeed() { return canWriteRandseed; }

  /** writes a modified random seed to file.
   *  @param randFile path of file to write
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool writeRandomSeed(const char *randFile);

  /** adds the contents of a file to the seed for the cryptographic
   *  pseudo-random number generator. The file should contain real
   *  random entropy data gathered from keystrokes, system events,
   *  /dev/random (on Linux) or something similar.
   *  If the TLS layer object is not initialized with sufficient
   *  random data, negotiation of TLS connections may fail.
   *  @param randFile path of the file containing random data
   */
  void seedPRNG(const char *randFile);

  /** modifies the PRNG by adding random data from the given buffer
   *  to the PRNG state.
   *  @param buf pointer to buffer containing random data
   *  @bufSize number of bytes in buffer
   */
  void addPRNGseed(void *buf, size_t bufSize);

  /** defines how peer certificates should be treated when
   *  negotiating a TLS connection.
   *  @param vtype certificate verification mode
   */
  void setCertificateVerification(DcmCertificateVerification vtype);

  /** sets the password string to be used when loading an
   *  encrypted private key file.
   *  Must be called prior to setPrivateKeyFile() in order to be effective.
   *  @param thePasswd password string, may be "" or NULL in which case an empty
   *    password is assumed.
   */
  void setPrivateKeyPasswd(const char *thePasswd);

  /** sets the password string to be used when loading an
   *  encrypted private key file to be read from the console stdin.
   */
  void setPrivateKeyPasswdFromConsole();

  /** loads a set of Diffie-Hellman parameters from file.
   *  These parameters are required for DH, DHE or DSS ciphersuites.
   *  @param filename path to the DH parameter file
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool setTempDHParameters(const char *filename);

  /** gets the most important attributes of the given X.509 certificate.
   *  @param peerCertificate X.509 certificate, may be NULL
   *  @return a string describing the certificate
   */
  static OFString dumpX509Certificate(X509 *peerCertificate);

  /** provides access to the underlying OpenSSL context handle for implementing
   *  custom functionality not accessible by the existing member functions of
   *  DcmTLSTransportLayer.
   *  @return the underlying OpenSSL context handle.
   *  @details
   *  <h4>Usage Example</h4>
   *  @code{.cpp}
   *  DcmTLSTransportLayer tLayer(DICOM_APPLICATION_REQUESTOR, "random.dat");
   *  ...
   *  DcmTLSTransportLayer::native_handle_type native = tlayer.getNativeHandle();
   *  X509_VERIFY_PARAM* param = SSL_CTX_get0_param(native);
   *
   *  // Enable automatic hostname checks
   *  X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
   *  X509_VERIFY_PARAM_set1_host(param, "www.example.com", 0);
   *
   *  // Configure a non-zero callback if desired
   *  SSL_CTX_set_verify(native, SSL_VERIFY_PEER, 0);
   *  ...
   *  @endcode
   */
  native_handle_type getNativeHandle();

private:

  /// private undefined copy constructor
  DcmTLSTransportLayer(const DcmTLSTransportLayer&);

  /// private undefined assignment operator
  DcmTLSTransportLayer& operator=(const DcmTLSTransportLayer&);

  /// OpenSSL context data, needed only once per application
  SSL_CTX *transportLayerContext;

  /// true if there is enough random data to write a new random seed file
  OFBool canWriteRandseed;

  /// contains the password for the private key if set on command line
  OFString privateKeyPasswd;

};

#endif /* WITH_OPENSSL */

#endif
