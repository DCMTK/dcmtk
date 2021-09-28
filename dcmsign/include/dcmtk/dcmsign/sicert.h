/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
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

#ifndef SICERT_H
#define SICERT_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitypes.h"
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */

class DcmItem;
class SiAlgorithm;
class OFDateTime;
struct x509_st;
struct asn1_string_st;
typedef struct x509_st X509;
typedef struct asn1_string_st ASN1_STRING;
typedef struct asn1_string_st ASN1_GENERALIZEDTIME;

/** a class representing X.509 public key certificates.
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiCertificate
{
public:

  /// default constructor
  SiCertificate();

  /** constructor
   *  @param cert pointer to OpenSSL X509 object. Ownership is transferred to the SiCertificate instance
   */
  SiCertificate(X509 *cert);

  ///destructor
  virtual ~SiCertificate();

  /** loads an X.509 certificate from file.
   *  @param filename file name of X.509 certificate
   *  @param filetype file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return status code
   */
  OFCondition loadCertificate(const char *filename, int filetype);

  /** reads an X.509 certificate from an item of the Digital Signatures Sequence
   *  and checks the certificate type information in the item.
   *  @param item item of the DigitalSignatureSQ from which the certificate is read
   *  @return status code
   */
  OFCondition read(DcmItem& item);

  /** writes the current X.509 certificate into an item of the Digital Signatures Sequence
   *  and creates the certificate type information in the item.
   *  @param item item of the DigitalSignatureSQ to which the certificate is written
   *  @return dcmdata OFCondition status code
   */
  OFCondition write(DcmItem& item);

  /** returns the type of public key stored in this certificate
   */
  E_KeyType getKeyType();

  /** creates an SiAlgorithm object for the public key contained in this certificate.
   *  If no certificate loaded or operation fails, returns NULL.
   *  New SiAlgorithm object must be deleted by caller.
   *  @return pointer to new SiAlgorithm object
   */
  SiAlgorithm *createAlgorithmForPublicKey();

  /** returns the format version of the X.509 certificate.
   *  If no certificate is loaded, returns 0.
   *  @return X.509 certificate version
   */
  long getX509Version();

  /** returns the subject name (distinguished name) of the current certificate.
   *  If no certificate is loaded, returns an empty string.
   *  @param str subject name returned in this string.
   */
  void getCertSubjectName(OFString& str);

  /** returns the issuer name (distinguished name) of the current certificate.
   *  If no certificate is loaded, returns an empty string.
   *  @param str issuer name returned in this string.
   */
  void getCertIssuerName(OFString& str);

  /** returns the serial number of the X.509 certificate.
   *  If no certificate is loaded, returns -1.
   *  @return X.509 certificate serial number
   */
  long getCertSerialNo();

  /** returns the start of validity of the current certificate in human readable form.
   *  If no certificate is loaded, returns an empty string.
   *  @param str start of validity returned in this string.
   */
  void getCertValidityNotBefore(OFString& str);

  /** returns the end of validity of the current certificate in human readable form.
   *  If no certificate is loaded, returns an empty string.
   *  @param str end of validity returned in this string.
   */
  void getCertValidityNotAfter(OFString& str);

  /** returns the strength (number of bits) of the public key contained
   *  in the current certificate.
   *  If no certificate is loaded, returns 0.
   *  @return key strength in bits.
   */
  long getCertKeyBits();

  /** returns the name of the elliptic curve used in the certificate.
   *  @return empty string if the certificate is not of elliptic curve type,
   *    "unnamed curve" if the curve name has not been stored in the certificate,
   *    or the short name of the elliptic curve if available.
   */
  OFString getCertCurveName();

  /** checks if the length of the public key in the certificate is too short
   *  and must be considered weak. Currently, an RSA or DSA key with less than
   *  1024 bits and an ECDSA key with less than 256 bits are considered weak.
   *  @return OFTrue if key is weak, OFFalse otherwise.
   */
  OFBool isWeakKey();

  /** checks if the length of the public key in the certificate is too short
   *  and must be considered weak, and if so, prints a warning to the logger.
   */
  void checkForWeakKey();

  /** returns a pointer to the raw certificate structure or NULL if no
   *  certificate present. Should not be called by users of this library.
   */
  X509 *getRawCertificate();

  /** returns true if the certificate expires before the given date.
   *  @param date a string in the format YYMMDDHHMMSSZ or YYYYMMDDHHMMSSZ
   *    (where Z represents the letter 'Z', meaning time zone UTC+0)
   *  @return OFTrue if certificate expires before the given date,
   *    or if the given date is invalid; OFFalse otherwise.
   */
  OFBool isCertExpiredAt(OFString& date);

  /** returns true if the certificate is expired.
   *  @return OFTrue if certificate is expired, OFFalse otherwise
   */
  OFBool isCertExpiredNow() const;

  /** returns true if the certificate is not yet valid at the given date.
   *  @param date a string in the format YYMMDDHHMMSSZ or YYYYMMDDHHMMSSZ
   *    (where Z represents the letter 'Z', meaning time zone UTC+0)
   *  @return OFTrue if certificate is not yet valid at the given date,
   *    or if the given date is invalid; OFFalse otherwise.
   */
  OFBool isCertNotYetValidAt(OFString& date);

  /** returns true if the certificate is not yet valid.
   *  @return OFTrue if certificate is not yet valid, OFFalse otherwise
   */
  OFBool isCertNotYetValidNow() const;

  /** this helper function converts a datetime in ASN1_GENERALIZEDTIME
   *  format to OFDateTime.
   *  @param d datetime in ASN1_GENERALIZEDTIME format
   *  @param dt dt datetime stored in this parameter upon success
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition convertGeneralizedTime(const ASN1_GENERALIZEDTIME *d, OFDateTime& dt);

  /** this helper function converts a datetime in ASN1_TIME format
   *  (which is in fact an alias for ASN1_STRING) to OFDateTime.
   *  @param d datetime in ASN1_TIME format
   *  @param dt dt datetime stored in this parameter upon success
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition convertASN1Time(const ASN1_STRING *d, OFDateTime& dt);

private:

  /// private undefined copy constructor
  SiCertificate(SiCertificate& arg);

  /// private undefined copy assignment operator
  SiCertificate& operator=(SiCertificate& arg);

  /// OpenSSL X.509 structure
  X509* x509;

};

#endif
#endif
