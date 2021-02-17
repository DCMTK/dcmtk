/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiTimeStamp
 *
 */

#ifndef SITSTAMP_H
#define SITSTAMP_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitypes.h"
#include "dcmtk/ofstd/ofstring.h"

// forward declarations
class DcmItem;
class SiCertificateVerifier;
class SiCertificate;
struct TS_req_st;
struct TS_resp_st;
struct TS_tst_info_st;
typedef struct TS_req_st TS_REQ;
typedef struct TS_resp_st TS_RESP;
typedef struct TS_tst_info_st TS_TST_INFO;
typedef struct pkcs7_st PKCS7;

/** Base class for a timestamp client.
 *  Instances of derived classes are able to request timestamps from a timestamp service.
 *  This class implements the code needed to create a timestamp request and to insert
 *  a timestamp reply into a DICOM dataset, but not the protocol for actually
 *  interacting with a timestamp authority.
 *  @remark this class is only available if DCMTK is compiled with OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiTimeStamp
{
public:

  /// default constructor
  SiTimeStamp();

  /// destructor
  virtual ~SiTimeStamp();

  /** takes a block of raw data and requests a time stamp for this raw data.
   *  @param inputData pointer to raw data
   *  @param inputDataSize length of raw data block in bytes
   *  @return status code
   */
  virtual OFCondition stamp(
    const unsigned char *inputData,
    unsigned long inputDataSize) = 0;

  /** reads the current timestamp from an item of the Digital Signatures Sequence
   *  and creates a timestamp ticket object.
   *  @param item item of the DigitalSignatureSQ from which the timestamp is read
   *  @return status code
   */
  virtual OFCondition read(DcmItem& item);

  /** writes the current timestamp into an item of the Digital Signatures Sequence
   *  and creates the timestamp type information in the item.
   *  @param item item of the DigitalSignatureSQ to which the timestamp is written
   *  @return status code
   */
  virtual OFCondition write(DcmItem& item) = 0;

  /** set the requested policy OID to be included into the time stamp query
   *  @param oid policy OID, NULL for no policy (which is the default).
   */
  virtual void setPolicyOID(const char *oid);

  /** sets the flag controlling whether or not a nonce is included
   *  into the timestamp query. Since a nonce is a protection against replay
   *  attack, normally it should be included, and this is also the default.
   *  @param nonce OFTrue if nonce should be included, OFFalse otherwise
   */
  virtual void setNonce(OFBool nonce);

  /** sets the flag controlling whether the timestamp authority will be
   *  requested to include its certificate into the timestamp reply.
   *  A timestamp reply with certificate is easier to verify, as less
   *  external key material is required, but somewhat larger.
   *  Default is OFTrue;
   *  @param creq OFTrue if certificate should be requested, OFFalse otherwise
   */
  virtual void setCertificateRequested(OFBool creq);

  /** sets the message authentication code to be used for creating
   *  the hash value in the timestamp query. Default is SHA256.
   *  @param creq OFTrue if certificate should be requested, OFFalse otherwise
   */
  virtual void setMAC(E_MACType mac);

  // methods that return the various attributes of a timestamp ticket

  /** checks if this object contains a timestamp ticket info object
   *  @return OFTrue if timestamp ticket info available, OFFalse otherwise
   */
  virtual OFBool have_tsinfo() const;

  /** returns the timestamp ticket info version number
   *  @return tsinfo version number if available, -1 otherwise
   */
  virtual long get_tsinfo_version() const;

  /** returns the timestamp ticket info policy OID
   *  @param oid upon return, contains the policy OID if available, an empty string otherwise
   */
  virtual void get_tsinfo_policy_oid(OFString& oid) const;

  /** returns the timestamp ticket info MAC algorithm name
   *  @param mac upon return, contains the MAC algorithm name if available, an empty string otherwise
   */
  virtual void get_tsinfo_imprint_algorithm_name(OFString& mac) const;

  /** returns the timestamp ticket info serial number
   *  @param serial upon return, contains the serial number if available, an empty string otherwise
   */
  virtual void get_tsinfo_serial_number(OFString& serial) const;

  /** returns the timestamp ticket info nonce, if present
   *  @param nonce upon return, contains the nonce if available, an empty string otherwise
   */
  virtual void get_tsinfo_nonce(OFString& nonce) const;

  /** returns the timestamp ticket info timestamp authority (TSA) name, if present
   *  @param tsa upon return, contains the TSA name if available, an empty string otherwise
   */
  virtual void get_tsinfo_tsa_name(OFString& tsa) const;

  /** returns the timestamp ticket info ordering flag
   *  @return OFTrue if timestamp ticket is present and contains an ordering flag with value "true", OFFalse otherwise
   */
  virtual OFBool get_tsinfo_ordering() const;

  /** returns the timestamp ticket info accuracy, if present
   *  @param accuracy upon return, contains the accuracy if available, an empty string otherwise
   */
  virtual void get_tsinfo_accuracy(OFString& accuracy) const;

  /** returns the timestamp date/time information, if present
   *  @param ts upon return, contains the timestamp date/time information, an empty string otherwise
   */
  virtual void get_tsinfo_timestamp(OFString& ts) const;

  /** returns the number of extensions in the timestamp ticket
   *  @return number of extensions in the timestamp ticket
   */
  virtual int get_tsinfo_numextensions() const;

  /** returns one timestamp extension, if present
   *  @param ext upon return, contains the timestamp extension if present, an empty string otherwise
   *  @param idx number of the extension, 0 <= idx < get_tsinfo_numextensions().
   */
  virtual void get_tsinfo_extension(OFString& ext, int idx) const;

  /** this method performs the following functions:
   *  - it checks if there is one and only one signer.
   *  - it identifies the signer certificate (which may be embedded in the timestamp, or loaded in the SiCertificateVerifier store)
   *  - it checks the extended key usage and key usage fields of the signer certificate
   *  - it verifies the certificate path
   *  - it checks if the certificate path meets the requirements of the
   *    SigningCertificate ESS signed attribute.
   *  - it verifies the signature value (against the imprint in the timestamp token
   *  - it prints the contents of the signer certificate to the logger
   *  @param cv container for the certificates used during verification
   *  @return EC_Normal upon success, an error code otherwise.
   */
  virtual OFCondition verifyTSSignature(SiCertificateVerifier& cv);

  /** Verify the timestamp token by checking that its imprint is
   *  indeed a hash of the DICOM signature, that the version number
   *  is as expected and that the TSA name, if provided in the timestamp
   *  ticket structure, matches the name of the TSA given in the TSA
   *  certificate.
   *  @param cv container for the certificates used during verification
   *  @param ditem item of the Digital Signatures Sequence containing the timestamp
   *  @param cert certificate of the signer of the DICOM signature, used to check
   *    the date/time of the timestamp against the validity period of the certificate
   *  @return EC_Normal upon success, an error code otherwise.
   */
  virtual OFCondition verifyTSToken(
    SiCertificateVerifier& cv,
    DcmItem& ditem,
    SiCertificate& cert);

  /** returns an error string containing a textual description of the result
   *  of the last call to verifyTSSignature() or verifyTSToken()
   *  if that call returned SI_EC_TimestampSignatureVerificationFailed.
   *  @param err text string returned in this parameter
   */
  void lastError(OFString& err) const;

protected:

  /** takes a block of raw data, computes a message digest and creates
   *  a time stamp query object.
   *  @param inputData pointer to raw data
   *  @param inputDataSize length of raw data block in bytes
   *  @return status code
   */
  virtual OFCondition create_ts_query(
    const unsigned char *inputData,
    unsigned long inputDataSize);

  /** return pointer to timestamp query object, may be NULL.
   *  @return pointer to timestamp query object, may be NULL.
   */
  virtual TS_REQ *getTSQ() { return tsq_; }

  /** return pointer to timestamp response object, may be NULL.
   *  @return pointer to timestamp response object, may be NULL.
   */
  virtual TS_RESP *getTSR() { return tsr_; }

  /** return pointer to timestamp ticket object, may be NULL.
   *  @return pointer to timestamp ticket object, may be NULL.
   */
  virtual PKCS7 *getTS() { return ts_; }

  /** return pointer to timestamp ticket info object, may be NULL.
   *  @return pointer to timestamp ticket info object, may be NULL.
   */
  virtual TS_TST_INFO *getTSInfo() { return tsinfo_; }

  /** load timestamp query from file
   *  @param fname filename, must not be NULL
   *  @return status code
   */
  virtual OFCondition load_ts_query(const char *fname);

  /** load timestamp response from file
   *  @param fname filename, must not be NULL
   *  @return status code
   */
  virtual OFCondition load_ts_response(const char *fname);

  /** check consistency between timestamp query (if available), timestamp
   *  response and DICOM digital signature.
   *  @param tsq pointer to timestamp query, may be NULL
   *  @param tsr pointer to timestamp response, must not be NULL
   *  @param ditem item of the DigitalSignaturesSequence to which this timestamp belongs
   *  @return status code
   */
  static OFCondition check_ts_response(
    TS_REQ *tsq,
    TS_RESP *tsr,
    DcmItem& ditem);

  /** insert timestamp token into DICOM dataset
   *  The timestamp response must have been checked prior to this method call.
   *  @param tsr pointer to timestamp response, must not be NULL
   *  @param ditem item of the DigitalSignaturesSequence to which this timestamp is written
   *  @return status code
   */
  static OFCondition write_ts_token(
    TS_RESP *tsr,
    DcmItem& ditem);

private:

  /// time stamping policy OID to be included in the time stamp query. Default is empty.
  OFString tsq_policy_;

  /// MAC algorithm for creating the hash key to be timestamped. Default is SHA-256.
  E_MACType tsq_mac_;

  /// Use a pseudo-random nonce in the time stamp query. Default is OFTrue.
  OFBool tsq_use_nonce_;

  /// Request the TSA certificate to be embedded into the time stamp reply. Default is OFTrue.
  OFBool tsq_certificate_requested_;

  /// pointer to time stamp request object, may be NULL
  TS_REQ *tsq_;

  /// pointer to time stamp response object, may be NULL
  TS_RESP *tsr_;

  /// pointer to time stamp ticket object, may be NULL
  PKCS7 *ts_;

  /// pointer to time stamp ticket info object (extracted from the timestamp ticket), may be NULL
  TS_TST_INFO *tsinfo_;

  /// OpenSSL X.509 certificate verification error code for the last operation
  long errorCode_;

  /// error string for the last operation, valid if errorCode_ < 0
  const char *errorString_;

};

#endif
#endif
