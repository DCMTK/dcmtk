/*
 *
 *  Copyright (C) 2018-2021, OFFIS e.V.
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

#ifndef TLSCIPHR_H
#define TLSCIPHR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#include "dcmtk/ofstd/ofcond.h"       /* for class OFCondition */
#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */
#include "dcmtk/ofstd/ofstream.h"     /* for class ostream */
#include "dcmtk/ofstd/ofvector.h"     /* for class OFVector */
#include "dcmtk/dcmtls/tlsdefin.h"    /* for DCMTK_DCMTLS_EXPORT */

// include this file in doxygen documentation

/** @file tlsciphr.h
 *  @brief type definitions and classes for TLS ciphersuites
 */

/** This enum describes DICOM TLS Security Profiles, which are the basis
 *  for the selection of supported ciphersuites when using DICOM over TLS.
 *  @remark This enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum DcmTLSSecurityProfile
{
  /// No security profile, start with an empty list of ciphersuites
  TSP_Profile_None,

  /** DICOM Basic TLS Secure Transport Connection Profile (retired),
    * using the ciphersuite TLS_RSA_WITH_3DES_EDE_CBC_SHA and TLS 1.0 or newer.
    */
  TSP_Profile_Basic,

  /** DICOM AES TLS Secure Transport Connection Profile (retired),
    * using the ciphersuites TLS_RSA_WITH_AES_128_CBC_SHA and TLS_RSA_WITH_3DES_EDE_CBC_SHA,
    * and TLS 1.0 or newer.
    */
  TSP_Profile_AES,

  /** DICOM BCP 195 TLS Profile, based on RFC 7525.
   *  This profile tries to negotiate TLS 1.2 or newer, but may fall back to previous
   *  protocol versions up to TLS 1.0. It supports four TLS 1.2 specific ciphersuites
   *  (TLS_DHE_RSA_WITH_AES_128_GCM_SHA256, TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
   *  TLS_DHE_RSA_WITH_AES_256_GCM_SHA384, TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384)
   *  but also the ciphersuites of the AES and Basic Profile, thus providing
   *  backward compatibility with older implementations while offering much
   *  better security when used with implementations also supporting one of the
   *  BCP 195 profiles.
   */
  TSP_Profile_BCP195,

  /** DICOM Non-downgrading BCP 195 TLS Profile, based on RFC 7525.
   *  This profile only negotiates TLS 1.2 or newer, and will not fall back to
   *  previous TLS versions. It supports four ciphersuites:
   *  TLS_DHE_RSA_WITH_AES_128_GCM_SHA256, TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
   *  TLS_DHE_RSA_WITH_AES_256_GCM_SHA384, TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384.
   *  It does not provide backwards compatibility with the older Basic and AES profiles,
   *  and thus guarantees the higher security level of BCP 195.
   */
  TSP_Profile_BCP195_ND,

  /** DICOM Extended BCP 195 TLS Profile, based on RFC 7525.
   *  This profile only negotiates TLS 1.2, and will not fall back to
   *  previous TLS versions. It does NOT support TLS 1.3.
   *  It supports the same set of ciphersuites as TSP_Profile_BCP195_ND,
   *  plus TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 and TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256.
   *  The other ciphersuites suggested by the profile (see DICOM Part 15, section B.11)
   *  are not supported in OpenSSL 1.0.1 to 1.1.0.
   *  This profile requires DHE keys of at least 2048 bits and ECDHE keys of at least 256 bits.
   *  It does not provide backwards compatibility with the older Basic and AES profiles,
   *  and thus guarantees the higher security level of BCP 195.
   */
  TSP_Profile_BCP195_Extended,

  /** IHE ATNA Profile for Unencrypted In-house Communication (retired).
   * This profile uses the ciphersuite SSL_RSA_WITH_NULL_SHA and TLS 1.0 or newer.
   * This ciphersuite offers peer authentication and integrity of communication,
   * but no encryption (i.e. no confidentiality), and is thus only suitable for use
   * over a secure infrastructure. This was used in older versions of the
   * IHE Audit Trail and Node Authentication (ATNA) integration profile,
   * but has been retired.
   */
  TSP_Profile_IHE_ATNA_Unencrypted

};


/** This enum describes the minimum SSL/TLS protocol version required
 *  for a certain ciphersuite.
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum DcmTLSCipherProtocolVersion
{
  /// Secure Socket Layer (SSL) version 3 or newer
  TPV_SSLv3,
  /// Transport Layer Security (TLS) version 1.2 or newer
  TPV_TLSv12
};


/** This enum describes the key exchange protocol used in a certain
 *  TLS ciphersuite.
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum DcmTLSCipherKeyExchange
{
  /// Static RSA key exchange
  TKE_RSA,

  /// Elliptic Curve Diffie-Hellman
  TKE_ECDH,

  /// Fixed ECDH with ECDSA-signed certificates
  TKE_ECDH_ECDSA,

  /// Fixed ECDH with RSA signatures
  TKE_ECDH_RSA,

  /// Diffie-Hellman key exchange
  TKE_DH

};


/** This enum describes the authentication mechanism
 *  used in a certain TLS ciphersuite.
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum DcmTLSCipherAuthentication
{
  /// Digital Signature Standard
  TCA_DSS,

  /// Elliptic Curve Diffie-Hellman
  TCA_ECDH,

  /// Elliptic Curve Digital Signature Algorithm
  TCA_ECDSA,

  /// RSA
  TCA_RSA
};


/** This enum describes the symmetric encryption algorithm
 *  (bulk cipher) used in a certain TLS ciphersuite.
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum DcmTLSCipherEncryption
{
  /// No encryption
  TCE_None,

  /// Triple DES in Cipher Block Chaining (CBC) mode
  TCE_3DES,

  /// Advanced Encryption Standard (AES) in Cipher Block Chaining (CBC) mode
  TCE_AES,

  /// Advanced Encryption Standard (AES) in Galois/Counter Mode (GCM)
  TCE_AESGCM,

  /// Camellia (RFC 3713) in Cipher Block Chaining (CBC) mode
  TCE_Camellia,

  /// ChaCha20-Poly1305 in Galois/Counter Mode (GCM)
  TCE_ChaCha20
};


/** This enum describes the message authentication code (MAC)
 *  used in a certain TLS ciphersuite.
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum DcmTLSCipherMAC
{
  /// Secure Hash Algorithm SHA-1
  TCM_SHA1,

  /// Secure Hash Algorithm SHA-2 in 256 bit mode
  TCM_SHA256,

  /// Secure Hash Algorithm SHA-2 in 384 bit mode
  TCM_SHA384,

  /// Authenticated Encryption with Associated Data (AEAD)
  TCM_AEAD

};


/** This enum describes the verification approach
 *  for certificate revocation lists (CRL)
 *  @remark this enum is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
enum DcmTLSCRLVerification
{
  /// Certificates are not checked against a CRL
  TCR_noCRL,

  /// Check chain leaf certificate against its CRL.
  /// An error occurs if a suitable CRL cannot be found.
  TCR_checkLeafCRL,

  /// Check the entire certificate chain against their CRLs.
  /// An error occurs if any required CRL cannot be found.
  TCR_checkAllCRL

};


/** This helper class manages the list of TLS ciphersuites supported by DCMTK,
 *  translates DcmTLSSecurityProfile enums into the corresponding sets of
 *  ciphersuites, and permits translation between the official TLS ciphersuite
 *  names (as used in DCMTK) and the corresponding internal OpenSSL name.
 *  @remark This class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMTLS_EXPORT DcmTLSCiphersuiteHandler
{
public:

  /** constructor. The constructor assumes that the OpenSSL library
   *  has already been initialized. This should be ensured prior to creating
   *  any DcmTLSCiphersuiteHandler instance by calling
   *  DcmTLSTransportLayer::initializeOpenSSL(). This only needs to be done once.
   */
  DcmTLSCiphersuiteHandler();

  /// destructor
  virtual ~DcmTLSCiphersuiteHandler();

  /** replace the current list of ciphersuites by the list of ciphersuites
   *  for the given profile.
   *  @param profile TLS Security Profile
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition setTLSProfile(DcmTLSSecurityProfile profile);

  /** return the currently selected TLS profile
   *  @return currently selected TLS profile
   */
  DcmTLSSecurityProfile getTLSProfile() const
  {
      return currentProfile;
  }

  /** clear the current list of ciphersuites. Equivalent to
   *  calling setTLSProfile(TSP_Profile_None).
   */
  void clearTLSProfile();

  /** adds a ciphersuite to the list of ciphersuites for TLS negotiation.
   *  It is the responsibility of the user to ensure that the added ciphersuite
   *  does not break the rules of the selected profile. Use with care!
   *  @param suite TLS ciphersuite name, in the official TLS name form.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition addCipherSuite(const char *suite);

  /** returns a string in OpenSSL syntax that contains the currently defined
   *  list of TLS ciphersuites.
   *  @param cslist The list of ciphersuites in OpenSSL syntax is written to this string.
   *  @param isServer true if the list of cipher suites is intended for
   *    a TLS server. In this case, the list of ciphersuites will be reordered
   *    from strongest to weakest, as recommended by BCP 195.
   */
  void getListOfCipherSuitesForOpenSSL(OFString& cslist, OFBool isServer) const;

  /** returns the set of flags that need to be activated in OpenSSL
   *  depending on the selected TLS profile.
   *  @return flags for openssl
   */
  long getTLSOptions() const;

  /** checks if the 3DES ciphersuite TLS_RSA_WITH_3DES_EDE_CBC_SHA
   *  is supported by the underlying OpenSSL library (which newer versions
   *  only do if compiled with "weak ciphers" enabled).
   *  @return true if we support 3DES, false otherwise
   */
  OFBool cipher3DESsupported() const;

  /** checks if the unencrypted ciphersuite TLS_RSA_WITH_NULL_SHA
   *  is supported by the underlying OpenSSL library (which newer versions
   *  only do if compiled with "weak ciphers" enabled).
   *  @return true if we support unencrypted TLS, false otherwise
   */
  OFBool cipherNULLsupported() const;

  /** checks if TLS 1.3 is enabled (permitted) for the currently selected
   *  TLS security profile. Note that this does not imply that the underlying
   *  OpenSSL library version actually supports TLS 1.3. That is checked elsewhere.
   *  @return true if we support TLS 1.3, false otherwise
   */
  OFBool isTLS13Enabled() const;

  /** print a list of supported ciphersuites to the given output stream
   *  @param os output stream
   */
  void printSupportedCiphersuites(STD_NAMESPACE ostream& os) const;

  /** returns the number of known ciphersuites.
   *  @return number of known ciphersuites
   */
  static size_t getNumberOfCipherSuites();

  /** looks up the index of the given ciphersuite by name
   *  @param tlsCipherSuiteName ciphersuite name in RFC 2246 form
   *  @returns index into list of ciphersuites, DcmTLSCiphersuiteHandler::unknownCipherSuiteIndex if ciphersuite unknown
   */
  static size_t lookupCiphersuite(const char *tlsCipherSuiteName);

  /** looks up the index of the given ciphersuite by OpenSSL name
   *  @param opensslCipherSuiteName ciphersuite name in the form used by OpenSSL
   *  @returns index into list of ciphersuites, DcmTLSCiphersuiteHandler::unknownCipherSuiteIndex if ciphersuite unknown
   */
  static size_t lookupCiphersuiteByOpenSSLName(const char *opensslCipherSuiteName);

  /** returns a ciphersuite name in RFC 2246 (TLS) form
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return ciphersuite name
   */
  static const char *getTLSCipherSuiteName(size_t idx);

  /** returns a ciphersuite name in OpenSSL form
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return ciphersuite name
   */
  static const char *getOpenSSLCipherSuiteName(size_t idx);

  /** returns the minimum SSL/TLS version required for the ciphersuite with the given index
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return minimum SSL/TLS version required
   */
  static DcmTLSCipherProtocolVersion getCipherSuiteProtocolVersion(size_t idx);

  /** returns the key exchange protocol used by the ciphersuite with the given index
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return key exchange protocol
   */
  static DcmTLSCipherKeyExchange getCipherSuiteKeyExchange(size_t idx);

  /** returns the authentication algorithm used by the ciphersuite with the given index
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return authentication algorithm
   */
  static DcmTLSCipherAuthentication getCipherSuiteAuthentication(size_t idx);

  /** returns the encryption algorithm used by  the ciphersuite with the given index
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return minimum SSL/TLS version required
   */
  static DcmTLSCipherEncryption getCipherSuiteEncryption(size_t idx);

  /** returns the message authentication code (MAC) algorithm used by  the ciphersuite with the given index
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return message authentication code (MAC) algorithm
   */
  static DcmTLSCipherMAC getCipherSuiteMAC(size_t idx);

  /** returns the symmetric key size used by the ciphersuite with the given index
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return symmetric key size, in bits
   */
  static size_t getCipherSuiteKeySize(size_t idx);

  /** returns the effective symmetric key size (i.e. security level) of the ciphersuite with the given index.
   *  BCP 195 (2015) recommends that no ciphersuites with an effective key size of less than 112 bits should be used anymore with TLS.
   *  @param idx index, must be < getNumberOfCipherSuites()
   *  @return effective symmetric key size, in bits
   */
  static size_t getCipherSuiteEffectiveKeySize(size_t idx);

  /** look up the name of the given security profile
   *  @param profile the given security profile
   *  @return name of security profile, never NULL.
   */
  static const char *lookupProfileName(DcmTLSSecurityProfile profile);

  /// constant returned by findOpenSSLCipherSuiteName() if ciphersuite name is unknown
  static const size_t unknownCipherSuiteIndex;

private:

  /// private undefined copy constructor
  DcmTLSCiphersuiteHandler(const DcmTLSCiphersuiteHandler&);

  /// private undefined assignment operator
  DcmTLSCiphersuiteHandler& operator=(const DcmTLSCiphersuiteHandler&);

  /** determine the set of ciphersuites that are supported both by DCMTK
   *  and the OpenSSL library we are currently using
   */
  void determineSupportedCiphers();

  /** add ciphersuite by name, print error if unsupported
   *  @param name ciphersuite name in RFC 2246 form
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition addRequiredCipherSuite(const char *name);

  /** add 3DES ciphersuite, print warning if unsupported
   */
  void addOptional3DESCipherSuite();

  /// current list of ciphersuites
  OFVector<size_t> ciphersuiteList;

  /// currently selected DICOM TLS security profile
  DcmTLSSecurityProfile currentProfile;

  /// indicator whether TLS 1.3 is enabled or disabled for the current profile
  OFBool tls13_enabled;

  /** an array of booleans indicating which ciphersuites known to DCMTK are
   *  actually supported by the OpenSSL library we are using.
   */
  OFBool *ciphersuiteSupported;
};

#endif /* WITH_OPENSSL */
#endif /* TLSCIPHR_H */
