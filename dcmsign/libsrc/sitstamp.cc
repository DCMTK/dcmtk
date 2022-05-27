/*
 *
 *  Copyright (C) 2019-2022, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitstamp.h"

#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmsign/simdmac.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/sicertvf.h"
#include "dcmtk/dcmsign/dcsignat.h"
#include "dcmtk/ofstd/ofdatime.h"

BEGIN_EXTERN_C
#include <openssl/ts.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/x509.h>
END_EXTERN_C

#ifndef HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTBEFORE
#define X509_get0_notBefore(x) X509_get_notBefore(x)
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTAFTER
#define X509_get0_notAfter(x) X509_get_notAfter(x)
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_STATUS
#define TS_STATUS_INFO_get0_status(x) (x)->status
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_TEXT
#define TS_STATUS_INFO_get0_text(x) (x)->text
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_FAILURE_INFO
#define TS_STATUS_INFO_get0_failure_info(x) (x)->failure_info
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTS_SET_CERTS
#define TS_VERIFY_CTS_set_certs(x,y) ((x)->certs = (y))
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_DATA
#define TS_VERIFY_CTX_set_data(x,y) ((x)->data = (y))
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_FLAGS
#define TS_VERIFY_CTX_set_flags(x,y) ((x)->flags = (y))
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_STORE
#define TS_VERIFY_CTX_set_store(x,y) ((x)->store = (y))
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_ASN1_STRING_GET0_DATA
#define ASN1_STRING_get0_data(x) ASN1_STRING_data((asn1_string_st*)x)
#endif

/// maximum length of the integer nonce, in bytes
#define NONCE_LENGTH 8

/// simple RAII container class for a raw buffer
class RAIIBuffer
{
public:
  /** constructor, allocates buffer
   *  @param len buffer size, must be > 0
   */
  RAIIBuffer(size_t len)
  : buf_(new unsigned char[len])
  , len_(len)
  {
  }

  /// destructor
  virtual ~RAIIBuffer()
  {
    delete[] buf_;
  }

  /// buffer
  unsigned char *buf_;

  /// buffer size
  size_t len_;
};


SiTimeStamp::SiTimeStamp()
: tsq_policy_()
, tsq_mac_(EMT_SHA256)
, tsq_use_nonce_(OFTrue)
, tsq_certificate_requested_(OFTrue)
, tsq_(NULL)
, tsr_(NULL)
, ts_(NULL)
, tsinfo_(NULL)
, errorCode_(0)
, errorString_("")
{
}


SiTimeStamp::~SiTimeStamp()
{
  TS_REQ_free(tsq_);
  TS_RESP_free(tsr_);
  PKCS7_free(ts_);
  TS_TST_INFO_free(tsinfo_);
}

void SiTimeStamp::setPolicyOID(const char *oid)
{
  if (oid) tsq_policy_ = oid; else tsq_policy_ = "";
}


void SiTimeStamp::setNonce(OFBool nonce)
{
  tsq_use_nonce_ = nonce;
}


void SiTimeStamp::setCertificateRequested(OFBool creq)
{
  tsq_certificate_requested_ = creq;
}

void SiTimeStamp::setMAC(E_MACType mac)
{
  tsq_mac_ = mac;
}


OFCondition SiTimeStamp::create_ts_query(
    const unsigned char *inputData,
    unsigned long inputDataSize)
{

  // validate input parameters
  if (inputData == NULL || inputDataSize == 0) return EC_IllegalCall;

  // delete a previous time stamp query, if present
  if (tsq_)
  {
    TS_REQ_free(tsq_);
    tsq_ = NULL;
  }

  // create message digest object
  SiMAC *mac = NULL;  // dcmsign MAC object
  const EVP_MD *evpmd = NULL; // OpenSSL MAC object
  switch (tsq_mac_)
  {
    case EMT_SHA1:
      mac = new SiMDMAC(EMT_SHA1);
      evpmd = EVP_sha1();
      break;
    case EMT_RIPEMD160:
      mac = new SiMDMAC(EMT_RIPEMD160);
      evpmd = EVP_ripemd160();
      break;
    case EMT_MD5:
      mac = new SiMDMAC(EMT_MD5);
      evpmd = EVP_md5();
      break;
    case EMT_SHA256:
      mac = new SiMDMAC(EMT_SHA256);
      evpmd = EVP_sha256();
      break;
    case EMT_SHA384:
      mac = new SiMDMAC(EMT_SHA384);
      evpmd = EVP_sha384();
      break;
    case EMT_SHA512:
      mac = new SiMDMAC(EMT_SHA512);
      evpmd = EVP_sha512();
      break;
    default:
      DCMSIGN_WARN("Unsupported MAC algorithm for timestamp selected.");
      return SI_EC_UnsupportedMAC;
      break;
  }
  if (mac == NULL || evpmd == NULL) return EC_MemoryExhausted;

  // compute message digest
  OFCondition result = EC_Normal;
  RAIIBuffer md(mac->getSize());
  result = mac->initialize();
  if (result.good()) result = mac->digest(inputData, inputDataSize);
  if (result.good()) result = mac->finalize(md.buf_);

  // delete message digest object
  delete mac;

  if (result.good())
  {
    // create time stamp request, algorithm and message imprint objects
    tsq_ = TS_REQ_new();
    X509_ALGOR *algo = X509_ALGOR_new();
    TS_MSG_IMPRINT *msg_imprint = TS_MSG_IMPRINT_new();
    if (tsq_ && algo && msg_imprint)
    {
      // set time stamp query version (always 1)
      if (!TS_REQ_set_version(tsq_, 1)) result = SI_EC_OpenSSLFailure;

      // set message digest type
      if (result.good())
      {
        if (NULL == (algo->algorithm = OBJ_nid2obj(EVP_MD_type(evpmd)))) result = SI_EC_OpenSSLFailure;
      }

      // create parameter object
      if (result.good())
      {
        if (NULL == (algo->parameter = ASN1_TYPE_new())) result = EC_MemoryExhausted;
      }

      // copy algorithm object into message imprint
      if (result.good())
      {
        algo->parameter->type = V_ASN1_NULL;
        if (!TS_MSG_IMPRINT_set_algo(msg_imprint, algo)) result = SI_EC_OpenSSLFailure;
      }

      // copy message digest into message imprint
      if (result.good())
      {
        if (!TS_MSG_IMPRINT_set_msg(msg_imprint, md.buf_, OFstatic_cast(int, md.len_))) result = SI_EC_OpenSSLFailure;
      }

      // copy message imprint into time stamp query
      if (result.good())
      {
        if (!TS_REQ_set_msg_imprint(tsq_, msg_imprint)) result = SI_EC_OpenSSLFailure;
      }

      // set certificate requested flag
      if (result.good())
      {
        if (!TS_REQ_set_cert_req(tsq_, (tsq_certificate_requested_ ? 1: 0))) result = SI_EC_OpenSSLFailure;
      }
    }
    else result = EC_MemoryExhausted;

    // delete algorithm object
    X509_ALGOR_free(algo);

    // delete message imprint object
    TS_MSG_IMPRINT_free(msg_imprint);

    // now we check the optional components of a time stamp query

    // policy OID
    if (result.good() && (tsq_policy_.length() > 0))
    {
      ASN1_OBJECT *policy_obj = OBJ_txt2obj(tsq_policy_.c_str(), 0);
      if (policy_obj)
      {
        // copy policy object into time stamp query
        if (!TS_REQ_set_policy_id(tsq_, policy_obj)) result = SI_EC_OpenSSLFailure;

        // delete policy object
        ASN1_OBJECT_free(policy_obj);
      }
      else
      {
        DCMSIGN_WARN("Cannot convert " << tsq_policy_ << " to OID.");
        result = SI_EC_InvalidOID;
      }
    }

    // nonce (pseudo-random integer used as protection against replay attacks)
    if (result.good() && tsq_use_nonce_)
    {
      // create buffer for random data
      unsigned char nonce_buf[NONCE_LENGTH];

      // fill buffer with random data
      if (RAND_bytes(nonce_buf, NONCE_LENGTH) <= 0) result = SI_EC_OpenSSLFailure;

      if (result.good())
      {
        // create nonce object
        ASN1_INTEGER *nonce_asn1 = ASN1_INTEGER_new();

        if (nonce_asn1)
        {
          // free memory buffer of nonce_asn1 (we will create a new one)
          OPENSSL_free(nonce_asn1->data);

          // Find the first non-zero byte in the buffer
          int i = 0;
          for (i = 0; i < NONCE_LENGTH && !nonce_buf[i]; ++i)
            continue;

          // create a new memory buffer for nonce_asn1
          nonce_asn1->length = NONCE_LENGTH - i;
          nonce_asn1->data = OFreinterpret_cast(unsigned char *, OPENSSL_malloc(nonce_asn1->length + 1));
          if (NULL != nonce_asn1->data)
          {
              // copy random data into nonce. The first byte is guaranteed to be nonzero,
              // which is necessary because otherwise the field might violate DER encoding.
              memcpy(nonce_asn1->data, nonce_buf + i, nonce_asn1->length);
          }
          else result = EC_MemoryExhausted;

          if (result.good())
          {
            // copy policy object into time stamp query
            if (!TS_REQ_set_nonce(tsq_, nonce_asn1)) result = SI_EC_OpenSSLFailure;
          }

          // delete nonce object
          ASN1_INTEGER_free(nonce_asn1);
        }
        else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

///helper structure for status bit arrays.
struct StatusMap
{
  int bit;
  const char *text;
};

/** status flags for timestamp response failures
 *  Source: OpenSSL, ts_rsp_print.c
 */
static StatusMap failure_map[] =
{
  {TS_INFO_BAD_ALG,
   "unrecognized or unsupported algorithm identifier"},
  {TS_INFO_BAD_REQUEST,
   "transaction not permitted or supported"},
  {TS_INFO_BAD_DATA_FORMAT,
   "the data submitted has the wrong format"},
  {TS_INFO_TIME_NOT_AVAILABLE,
   "the TSA's time source is not available"},
  {TS_INFO_UNACCEPTED_POLICY,
   "the requested TSA policy is not supported by the TSA"},
  {TS_INFO_UNACCEPTED_EXTENSION,
   "the requested extension is not supported by the TSA"},
  {TS_INFO_ADD_INFO_NOT_AVAILABLE,
   "the additional information requested could not be understood "
   "or is not available"},
  {TS_INFO_SYSTEM_FAILURE,
   "the request cannot be handled due to system failure"},
  {-1, NULL}
};

OFCondition SiTimeStamp::check_ts_response(
    TS_REQ *tsq,
    TS_RESP *tsr,
    DcmItem& ditem)
{
  if (tsr == NULL) return EC_IllegalCall;

  // retrieve status structure from timestamp response.
  // This function does not create a copy that we need to release later.
  TS_STATUS_INFO *status = TS_RESP_get_status_info(tsr);
  if (status == NULL)
  {
    DCMSIGN_ERROR("unable to access status structure of timestamp response message");
    return SI_EC_InvalidTSR;
  }

  // evaluate status structure from timestamp response
  OFCondition result = EC_Normal;
  long status_value = ASN1_INTEGER_get(TS_STATUS_INFO_get0_status(status));

  if (status_value != 0) // status 0 means "granted", i.e. success
  {
     if (status_value == 1) DCMSIGN_WARN("timestamp response status: granted with modifications");
     else
     {
       result = SI_EC_InvalidTSR;
       switch (status_value)
       {
         case 2:
           DCMSIGN_ERROR("timestamp response status: rejected");
           break;
         case 3:
           DCMSIGN_ERROR("timestamp response status: waiting");
           break;
         case 4:
           DCMSIGN_ERROR("timestamp response status: revocation warning");
           break;
         case 5:
           DCMSIGN_ERROR("timestamp response status: revoked");
           break;
         default:
           DCMSIGN_ERROR("timestamp response status: unknown error code " << status_value);
           break;
       }
     }

     // if we have additional status text (optional), let's print it to the logger
     const unsigned char *c = NULL;
     for (int i = 0; i < sk_ASN1_UTF8STRING_num(TS_STATUS_INFO_get0_text(status)); ++i)
     {
       c = ASN1_STRING_get0_data(sk_ASN1_UTF8STRING_value(TS_STATUS_INFO_get0_text(status), i));
       if (c) DCMSIGN_WARN("timestamp response status text: " << c);
     }

     // if we have additional failure info flags (optional), let's print them to the logger
     if (TS_STATUS_INFO_get0_failure_info(status) != NULL)
     {
      const StatusMap *fmap = failure_map;
      for (; fmap->bit >= 0; ++fmap)
      {
        if (ASN1_BIT_STRING_get_bit(TS_STATUS_INFO_get0_failure_info(status), fmap->bit))
        {
          DCMSIGN_WARN("timestamp response status text: " << fmap->text);
        }
      }
    }
  }

  TS_TST_INFO *ts_info = NULL;

  // access TSTInfo structure (i.e. the content of the signed timestamp)
  if (result.good())
  {
    ts_info = TS_RESP_get_tst_info(tsr);
    if (ts_info == NULL)
    {
      DCMSIGN_ERROR("timestamp response does not contain TSTInfo structure");
      result = SI_EC_InvalidTSR;
    }
  }

  // check version number of timestamp response
  if (result.good())
  {
    if (TS_TST_INFO_get_version(ts_info) != 1)
    {
      DCMSIGN_ERROR("unsupported timestamp response version number " << TS_TST_INFO_get_version(ts_info));
      result =  SI_EC_InvalidTSR;
    }
  }

  // access TSR message imprint (MAC algorithm and hash)
  TS_MSG_IMPRINT *ts_info_imprint = NULL;
  if (result.good())
  {
    ts_info_imprint = TS_TST_INFO_get_msg_imprint(ts_info);
    if (ts_info_imprint == NULL)
    {
      DCMSIGN_ERROR("timestamp response validation failed: message imprint missing in timestamp response");
      result = SI_EC_InvalidTSR;
    }
  }

  // access TSR MAC algorithm OID
  X509_ALGOR *ts_info_algo = NULL;
  if (result.good())
  {
    ts_info_algo = TS_MSG_IMPRINT_get_algo(ts_info_imprint);
    if (ts_info_algo == NULL)
    {
      DCMSIGN_ERROR("timestamp response validation failed: message digest algorithm missing in timestamp response");
      result = SI_EC_InvalidTSR;
    }
  }

  // bail out if the timestamp response had an error status
  if (result.bad()) return result;

  // check if the timestamp response matches the timestamp query
  if (tsq == NULL)
  {
    DCMSIGN_WARN("timestamp query not available, will not check consistency with timestamp response");
  }
  else
  {
    // at this point, tsq and ts_info are guaranteed to be valid pointers.

    // access message imprints (MAC algorithm and hash)
    TS_MSG_IMPRINT *tsq_imprint = TS_REQ_get_msg_imprint(tsq);
    if (tsq_imprint)
    {
      // compare message imprints
      X509_ALGOR *tsq_algo = TS_MSG_IMPRINT_get_algo(tsq_imprint);
      if (tsq_algo == NULL)
      {
        DCMSIGN_ERROR("timestamp response validation failed: message digest algorithm missing in timestamp query");
        result = SI_EC_InvalidTSR;
      }
      else
      {
        if (OBJ_cmp(tsq_algo->algorithm, ts_info_algo->algorithm))
        {
          DCMSIGN_ERROR("timestamp response validation failed: message digest algorithms are different in timestamp query and response");
          result = SI_EC_InvalidTSR;
        }
        else
        {
          unsigned int tsq_len = ASN1_STRING_length(TS_MSG_IMPRINT_get_msg(tsq_imprint));
          unsigned int ts_info_len = ASN1_STRING_length(TS_MSG_IMPRINT_get_msg(ts_info_imprint));
          if (tsq_len != ts_info_len)
          {
            DCMSIGN_ERROR("timestamp response validation failed: message digest lengths are different in timestamp query and response");
            result = SI_EC_InvalidTSR;
          }
          else
          {
            if (memcmp(ASN1_STRING_get0_data(TS_MSG_IMPRINT_get_msg(tsq_imprint)),
                ASN1_STRING_get0_data(TS_MSG_IMPRINT_get_msg(ts_info_imprint)), tsq_len) != 0)
            {
              DCMSIGN_ERROR("timestamp response validation failed: message digests do not match in timestamp query and response");
              result = SI_EC_InvalidTSR;
            }
          }
        }
      }
    }
    else
    {
      DCMSIGN_ERROR("timestamp response validation failed: message imprint missing in timestamp query");
      result = SI_EC_InvalidTSR;
    }
    if (result.bad()) return result;

    // check nonces for consistency
    const ASN1_INTEGER *tsq_nonce = TS_REQ_get_nonce(tsq);
    if (tsq_nonce) // the timestamp query may or may not contain a nonce
    {
       // query contains a nonce, therefore, the response MUST also contain a nonce
       // with the same value
      const ASN1_INTEGER *ts_info_nonce = TS_TST_INFO_get_nonce(ts_info);
      if (ts_info_nonce == NULL)
      {
        DCMSIGN_ERROR("timestamp response validation failed: nonce is missing in timestamp response");
        result = SI_EC_InvalidTSR;
      }
      else
      {
        // compare nonces
        if (ASN1_INTEGER_cmp(tsq_nonce, ts_info_nonce) != 0)
        {
          DCMSIGN_ERROR("timestamp response validation failed: nonces are not equal in timestamp query and response");
          result = SI_EC_InvalidTSR;
        }
      }
    }
    if (result.bad()) return result;

    // check policy OIDs for consistency
    ASN1_OBJECT *tsq_policy = TS_REQ_get_policy_id(tsq);
    if (tsq_policy) // the timestamp query may or may not contain a policy OID
    {
      const ASN1_OBJECT *ts_info_policy = TS_TST_INFO_get_policy_id(ts_info);
      if (ts_info_policy == NULL)
      {
        DCMSIGN_ERROR("timestamp response validation failed: policy OID is missing in timestamp response");
        result = SI_EC_InvalidTSR;
      }
      else
      {
        // compare policy OIDs
        if (OBJ_cmp(tsq_policy, ts_info_policy) != 0)
        {
          DCMSIGN_ERROR("timestamp response validation failed: policy OIDs are not equal in timestamp query and response");
          result = SI_EC_InvalidTSR;
        }
      }
    }
    if (result.bad()) return result;
  }

  // the timestamp response looks OK and is consistent with the timestamp query;
  // now let's check consistency with the DICOM signature.

  // get the signature from the DICOM dataset
  const Uint8 *signature = NULL;
  unsigned long sigLength = 0;
  result = ditem.findAndGetUint8Array(DCM_Signature, signature, &sigLength);
  if (result.bad() || sigLength < 4 || (signature == NULL))
  {
    DCMSIGN_ERROR("timestamp response validation failed: signature in DICOM dataset not found or invalid");
    result = SI_EC_InvalidTSR;
  }

  // determine type of signature
  E_KeyType signatureType = EKT_none;
  if (result.good())
  {
    SiCertificate cert;
    result = cert.read(ditem);
    if (result.good()) signatureType = cert.getKeyType();
    else
    {
      DCMSIGN_ERROR("timestamp response validation failed: certificate in DICOM dataset not found or invalid");
      result = SI_EC_InvalidTSR;
    }
  }

  // if the signature type is DSA or ECDSA, we need to account for a possible pad byte
  if (result.good())
  {
    if (signatureType == EKT_DSA || signatureType == EKT_EC)
    {
      DcmSignature::adjustASN1SequenceLength(signature, sigLength);
    }
  }

  // now determine the MAC algorithm we need to feed the signature to
  SiMAC *mac = NULL;
  if (result.good())
  {
#ifndef HAVE_OPENSSL_X509_ALGOR_GET0_CONST_PARAM
    ASN1_OBJECT *mac_oid = NULL;
    void *ppval = NULL;
#else
    const ASN1_OBJECT *mac_oid = NULL;
    const void *ppval = NULL;
#endif
    int pptype = 0;
    X509_ALGOR_get0(&mac_oid, &pptype, &ppval, ts_info_algo);
    if (mac_oid == NULL)
    {
      DCMSIGN_ERROR("timestamp response validation failed: cannot determine MAC algorithm in timestamp response");
      result = SI_EC_InvalidTSR;
    }
    else
    {
      int mac_nid = OBJ_obj2nid(mac_oid);
      const char *mac_name = OBJ_nid2ln(mac_nid);
      switch (mac_nid)
      {
        case NID_sha1:
          mac = new SiMDMAC(EMT_SHA1);
          break;
        case NID_ripemd160:
          mac = new SiMDMAC(EMT_RIPEMD160);
          break;
        case NID_md5:
          mac = new SiMDMAC(EMT_MD5);
          break;
        case NID_sha256:
          mac = new SiMDMAC(EMT_SHA256);
          break;
        case NID_sha384:
          mac = new SiMDMAC(EMT_SHA384);
          break;
        case NID_sha512:
          mac = new SiMDMAC(EMT_SHA512);
          break;
        default:
          DCMSIGN_ERROR("timestamp response validation failed: unsupported MAC algorithm " << ( mac_name ? mac_name : "(unknown)") << " in timestamp response");
          result = SI_EC_InvalidTSR;
          break;
      }
    }
  }

  // check that the lengths of both MACs are really the same
  if (result.good())
  {
    // mac is now guaranteed to point to a valid SiMAC object
    unsigned int ts_info_len = ASN1_STRING_length(TS_MSG_IMPRINT_get_msg(ts_info_imprint));
    if (mac->getSize() != ts_info_len)
    {
      DCMSIGN_ERROR("timestamp response validation failed: message digest length does not match in DICOM dataset and timestamp response");
      result = SI_EC_InvalidTSR;
    }
  }

  // compute MAC of digital signature and compare with the MAC in the TSR
  if (result.good())
  {
    RAIIBuffer md(mac->getSize());
    result = mac->initialize();
    if (result.good()) result = mac->digest(signature, sigLength);
    if (result.good()) result = mac->finalize(md.buf_);
    if (memcmp(md.buf_, ASN1_STRING_get0_data(TS_MSG_IMPRINT_get_msg(ts_info_imprint)), mac->getSize()) != 0)
    {
      DCMSIGN_ERROR("timestamp response validation failed: message digests do not match in DICOM dataset and timestamp response");
      result = SI_EC_InvalidTSR;
    }
  }

  // delete message digest object
  delete mac;

  // Done. If we have found not problem at this point, the TSR is successful and matches
  // both the TSQ (if present) and the DICOM dataset.
  return result;
}


OFCondition SiTimeStamp::load_ts_query(const char *fname)
{
  if (fname == NULL) return EC_InvalidFilename;
  TS_REQ_free(tsq_);
  tsq_ = NULL;

  OFCondition result = EC_Normal;
  BIO *in_bio = BIO_new_file(fname, "rb");
  if ((in_bio == NULL) || (NULL == (tsq_ = d2i_TS_REQ_bio(in_bio, NULL))))
  {
    DCMSIGN_ERROR("Unable to load timestamp query file '" << fname << "'");
    result = SI_EC_CannotRead;
  }
  BIO_free_all(in_bio);
  return result;
}


OFCondition SiTimeStamp::load_ts_response(const char *fname)
{
  if (fname == NULL) return EC_InvalidFilename;
  TS_RESP_free(tsr_);
  tsr_ = NULL;

  OFCondition result = EC_Normal;
  BIO *in_bio = BIO_new_file(fname, "rb");
  if ((in_bio == NULL) || (NULL == (tsr_ = d2i_TS_RESP_bio(in_bio, NULL))))
  {
    DCMSIGN_ERROR("Unable to load timestamp response file '" << fname << "'");
    result = SI_EC_CannotRead;
  }
  BIO_free_all(in_bio);
  return result;
}


OFCondition SiTimeStamp::write_ts_token(
    TS_RESP *tsr,
    DcmItem& ditem)
{
  if (tsr == NULL)
  {
    DCMSIGN_ERROR("Cannot insert timestamp response into DICOM dataset, not loaded");
    return EC_IllegalCall;
  }

  PKCS7 *ts_token = TS_RESP_get_token(tsr);
  if (ts_token == NULL)
  {
    DCMSIGN_ERROR("No timestamp token in the timestamp respone");
    return SI_EC_InvalidTSR;
  }

  // write timestamp token to buffer
  OFCondition result = EC_Normal;
  unsigned char *ts_token_buf = NULL;
  int len = i2d_PKCS7(ts_token, &ts_token_buf);

  if (len < 0)
  {
    DCMSIGN_ERROR("Error while serializing timestamp token");
    result = SI_EC_OpenSSLFailure;
  }

  if (result.good()) result = ditem.putAndInsertUint8Array(DCM_CertifiedTimestamp, ts_token_buf, len);
  if (result.good()) result = ditem.putAndInsertString(DCM_CertifiedTimestampType, "CMS_TSP");

  OPENSSL_free(ts_token_buf);
  return result;
}


OFCondition SiTimeStamp::read(DcmItem& item)
{
  OFString tstype;
  const Uint8 *tsbytes = NULL;
  unsigned long tslen = 0;

  // delete any prior timestamp ticket
  PKCS7_free(ts_);
  ts_ = NULL;
  TS_TST_INFO_free(tsinfo_);
  tsinfo_ = NULL;

  // check certified timestamp type
  OFCondition result = item.findAndGetOFString(DCM_CertifiedTimestampType, tstype);
  if (result.good())
  {
     if (tstype == "CMS_TSP")
     {
       // access certified timestamp
       result = item.findAndGetUint8Array(DCM_CertifiedTimestamp, tsbytes, &tslen);
       if (result.good())
       {
         // adjust for a possible pad byte
         DcmSignature::adjustASN1SequenceLength(tsbytes, tslen);
         const unsigned char *tsbytes_temp = tsbytes;
         ts_ = d2i_PKCS7(NULL, &tsbytes_temp, tslen);
         if (ts_ == NULL)
         {
           DCMSIGN_WARN("unable to read certified timestamp in dataset");
           result = SI_EC_InvalidTimestamp;
         }
         else
         {
           // PKCS7_to_TS_TST_INFO() is an "expensive" function that parses
           // the content of the signed timestamp ticket. Therefore, we cache
           // the result.
           tsinfo_ = PKCS7_to_TS_TST_INFO(ts_);
           if (tsinfo_ == NULL)
           {
             DCMSIGN_WARN("unable to read certified timestamp ticket in dataset");
             result = SI_EC_InvalidTimestamp;
           }
         }
       }
     }
     else
     {
       DCMSIGN_WARN("unknown certified timestamp type: " << tstype);
       result = SI_EC_UnknownTimestampType;
     }
  }
  return result;
}


OFBool SiTimeStamp::have_tsinfo() const
{
  return (tsinfo_ != NULL);
}

long SiTimeStamp::get_tsinfo_version() const
{
  if (tsinfo_)
    return TS_TST_INFO_get_version(tsinfo_);
    else return -1;
}


void SiTimeStamp::get_tsinfo_policy_oid(OFString& oid) const
{
#define TSINFO_BUFSIZE 200
  oid = "";
  if (tsinfo_)
  {
    char buf[TSINFO_BUFSIZE]; // The OpenSSL recommends a buffer of at least 80 characters
    buf[0]='\0'; // zero terminate buffer
    int len = OBJ_obj2txt(buf, TSINFO_BUFSIZE, TS_TST_INFO_get_policy_id(tsinfo_), 1);
    if (len > TSINFO_BUFSIZE)
    {
       DCMSIGN_WARN("timestamp policy OID truncated, length is " << len);
    }
    oid = buf;
  }
}


void SiTimeStamp::get_tsinfo_imprint_algorithm_name(OFString& mac) const
{
  mac = "";
  if (tsinfo_)
  {

    TS_MSG_IMPRINT *ts_info_imprint = TS_TST_INFO_get_msg_imprint(tsinfo_);
    if (ts_info_imprint == NULL)
    {
      DCMSIGN_WARN("timestamp imprint cannot be accessed");
    }
    else
    {
      X509_ALGOR *ts_info_algo = TS_MSG_IMPRINT_get_algo(ts_info_imprint);
      if (ts_info_algo == NULL)
      {
        DCMSIGN_WARN("timestamp imprint algorithm cannot be accessed");
      }
      else
      {
        char buf[TSINFO_BUFSIZE]; // The OpenSSL recommends a buffer of at least 80 characters
        buf[0]='\0'; // zero terminate buffer
        int len = OBJ_obj2txt(buf, TSINFO_BUFSIZE, ts_info_algo->algorithm, 0);
        if (len > TSINFO_BUFSIZE)
        {
           DCMSIGN_WARN("timestamp imprint algorithm name truncated, length is " << len);
        }
        mac = buf;
      }
    }
  }
}


void SiTimeStamp::get_tsinfo_serial_number(OFString& serial) const
{
  serial = "";
  if (tsinfo_)
  {
    const ASN1_INTEGER *asn1serial = TS_TST_INFO_get_serial(tsinfo_);
    if (asn1serial == NULL)
    {
      DCMSIGN_WARN("timestamp serial number cannot be accessed");
    }
    else
    {
      BIGNUM *bnserial = ASN1_INTEGER_to_BN(asn1serial, NULL); // creates new object
      if (bnserial == NULL)
      {
        DCMSIGN_WARN("timestamp serial number cannot be converted to BIGNUM");
      }
      else
      {
        BIO *bio = BIO_new(BIO_s_mem());
        if (bio)
        {
          char *bufptr = NULL;
          BN_print(bio, bnserial);
          BIO_write(bio,"\0",1); // add terminating zero
          BIO_get_mem_data(bio, (char *)(&bufptr));
          if (bufptr)
          {
            serial = "0x";
            serial += bufptr;
          }
          BIO_free(bio);
        }
        else
        {
          DCMSIGN_WARN("timestamp serial number cannot be printed");
        }
        BN_free(bnserial);
      }
    }
  }
}


void SiTimeStamp::get_tsinfo_nonce(OFString& nonce) const
{
  nonce = "";
  if (tsinfo_)
  {
    const ASN1_INTEGER *asn1nonce = TS_TST_INFO_get_nonce(tsinfo_);
    if (asn1nonce) // nonce is an optional field, may be absent
    {
      BIGNUM *bnnonce = ASN1_INTEGER_to_BN(asn1nonce, NULL); // creates new object
      if (bnnonce == NULL)
      {
        DCMSIGN_WARN("timestamp nonce cannot be converted to BIGNUM");
      }
      else
      {
        BIO *bio = BIO_new(BIO_s_mem());
        if (bio)
        {
          char *bufptr = NULL;
          BN_print(bio, bnnonce);
          BIO_write(bio,"\0",1); // add terminating zero
          BIO_get_mem_data(bio, (char *)(&bufptr));
          if (bufptr)
          {
            nonce = "0x";
            nonce += bufptr;
          }
          BIO_free(bio);
        }
        else
        {
          DCMSIGN_WARN("timestamp nonce cannot be printed");
        }
        BN_free(bnnonce);
      }
    }
  }
}


void SiTimeStamp::get_tsinfo_tsa_name(OFString& tsa) const
{
  tsa = "";
  if (tsinfo_)
  {
    GENERAL_NAME *tsaname = TS_TST_INFO_get_tsa(tsinfo_);
    if (tsaname) // tsa is an optional field, may be absent
    {
      BIO *bio = BIO_new(BIO_s_mem());
      if (bio)
      {
        char *bufptr = NULL;
        GENERAL_NAME_print(bio, tsaname);
        BIO_write(bio,"\0",1); // add terminating zero
        BIO_get_mem_data(bio, (char *)(&bufptr));
        if (bufptr)
        {
          tsa = bufptr;
        }
        BIO_free(bio);
      }
      else
      {
        DCMSIGN_WARN("timestamp tsa cannot be printed");
      }
    }
  }
}


void SiTimeStamp::get_tsinfo_accuracy(OFString& accuracy) const
{
  accuracy = "";
  if (tsinfo_)
  {
    TS_ACCURACY *acc = TS_TST_INFO_get_accuracy(tsinfo_);
    if (acc) // accuracy is an optional field, may be absent
    {
      char buf[20];
      long sec = ASN1_INTEGER_get(TS_ACCURACY_get_seconds(acc));
      long msec = ASN1_INTEGER_get(TS_ACCURACY_get_millis(acc));
      long usec = ASN1_INTEGER_get(TS_ACCURACY_get_micros(acc));

      if (sec >= 0)
      {
        OFStandard::snprintf(buf, 20, "%ld", sec);
        accuracy = buf;
        if (usec > 0)
        {
          OFStandard::snprintf(buf, 20, ".%03ld%03lds", msec, usec);
          accuracy += buf;
        }
        else
        {
          OFStandard::snprintf(buf, 20, ".%03lds", msec);
          accuracy += buf;
        }
      }
    }
  }
}


OFBool SiTimeStamp::get_tsinfo_ordering() const
{
  OFBool result = OFFalse; // the default
  if (tsinfo_)
  {
    result = TS_TST_INFO_get_ordering(tsinfo_) ? OFTrue : OFFalse;
  }
  return result;
}


void SiTimeStamp::get_tsinfo_timestamp(OFString& ts) const
{
  ts = "";
  if (tsinfo_)
  {
    const ASN1_GENERALIZEDTIME *gtime = TS_TST_INFO_get_time(tsinfo_);
    if (gtime == NULL)
    {
      DCMSIGN_WARN("timestamp date/time cannot be accessed");
    }
    else
    {
      // ASN1_GENERALIZEDTIME is just an alias for ASN1_STRING,
      // so we can use ASN1_STRING_get0_data() to access the raw string
      const unsigned char *c = ASN1_STRING_get0_data(gtime);
      if (c) ts = OFreinterpret_cast(const char *, c);
    }
  }
}

int SiTimeStamp::get_tsinfo_numextensions() const
{
  int result = 0;
  if (tsinfo_)
  {
    result = TS_TST_INFO_get_ext_count(tsinfo_);
  }
  return result;
}


void SiTimeStamp::get_tsinfo_extension(OFString& ext, int idx) const
{
  ext = "";
  if (tsinfo_)
  {
    int numextensions = TS_TST_INFO_get_ext_count(tsinfo_);
    if (idx >= numextensions)
    {
      DCMSIGN_WARN("timestamp extension " << idx << " does not exist");
    }
    else
    {
      X509_EXTENSION *x509ext = TS_TST_INFO_get_ext(tsinfo_, idx);
      if (x509ext == NULL)
      {
        DCMSIGN_WARN("timestamp extension " << idx << " cannot be accessed");
      }
      else
      {
        BIO *bio = BIO_new(BIO_s_mem());
        if (bio)
        {
          char *bufptr = NULL;
          X509V3_EXT_print(bio, x509ext, X509V3_EXT_PARSE_UNKNOWN, 0 /* indent */);
          BIO_write(bio,"\0",1); // add terminating zero
          BIO_get_mem_data(bio, (char *)(&bufptr));
          if (bufptr)
          {
            ext = bufptr;
          }
          BIO_free(bio);
        }
        else
        {
          DCMSIGN_WARN("timestamp extension " << idx << " cannot be printed");
        }
      }
    }
  }
}


OFCondition SiTimeStamp::verifyTSSignature(SiCertificateVerifier& cv)
{
  // check if we have a timestamp object
  if (ts_ == NULL) return EC_IllegalCall;

  X509 *signerCert = NULL;
  OFCondition result = EC_Normal;
  OFString aString;
  errorCode_ = 0;

  // unfortunately, TS_RESP_verify_signature() only returns the certificate of the timestamp signer
  // if the signature verification succeeds. If verification fails, e.g. because the timestamp
  // certificate has expired, we have no way of accessing it
  if (! TS_RESP_verify_signature(ts_, cv.getUntrustedCerts(), cv.getTrustedCertStore(), &signerCert))
  {
    result = SI_EC_TimestampSignatureVerificationFailed;
    errorCode_ = ERR_get_error();
  }

  if (result.good() && signerCert)
  {
    // compare timestamp against validity period of the TSA certificate
    // The timestamp should have been created while the certificate is valid.
    const ASN1_TIME *notBefore = X509_get0_notBefore(signerCert);
    const ASN1_TIME *notAfter = X509_get0_notAfter(signerCert);
    const ASN1_GENERALIZEDTIME *gtime = TS_TST_INFO_get_time(tsinfo_);

    OFDateTime dt_notBefore;
    OFDateTime dt_notAfter;
    OFDateTime dt_gtime;

    if ((notBefore == NULL)||(notAfter == NULL)||(gtime == NULL)||
      SiCertificate::convertASN1Time(notBefore, dt_notBefore).bad() ||
      SiCertificate::convertASN1Time(notAfter, dt_notAfter).bad() ||
      SiCertificate::convertGeneralizedTime(gtime, dt_gtime).bad())
    {
      errorCode_ = -1;
      errorString_ = "comparison of timestamp date/time with TSA certificate notBefore/notAfter attribute failed, value possibly malformed.";
      DCMSIGN_ERROR(errorString_);
      result = SI_EC_TimestampSignatureVerificationFailed;
    }
    else
    {
      if (dt_gtime < dt_notBefore)
      {
        errorCode_ = -1;
        errorString_ = "timestamp created before start of TSA certificate validity.";
        DCMSIGN_ERROR(errorString_);
        result = SI_EC_TimestampSignatureVerificationFailed;
      }
      else if (dt_gtime > dt_notAfter)
      {
        errorCode_ = -1;
        errorString_ = "timestamp created after expiry of TSA certificate.";
        DCMSIGN_ERROR(errorString_);
        result = SI_EC_TimestampSignatureVerificationFailed;
      }
    }
  }

  // wrap certificate into a SiCertificate instance that will also free
  // the memory when it goes out of scope.
  // SiCertificate gracefully handles the case where signerCert is NULL.
  SiCertificate cert(signerCert);

  if (signerCert && dcmsignLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
  {
    DCMSIGN_INFO("  Timestamp signature certificate: ");
    if ((signerCert == NULL)||(cert.getKeyType()==EKT_none))
      DCMSIGN_INFO("      none");
    else
    {
      DCMSIGN_INFO("      X.509v" << cert.getX509Version());
      cert.getCertSubjectName(aString);
      DCMSIGN_INFO("      Subject                 : " << aString);
      cert.getCertIssuerName(aString);
      DCMSIGN_INFO("      Issued by               : " << aString);
      DCMSIGN_INFO("      Serial no.              : " << cert.getCertSerialNo());
      cert.getCertValidityNotBefore(aString);
      DCMSIGN_INFO("      Validity                : not before " << aString);
      cert.getCertValidityNotAfter(aString);
      DCMSIGN_INFO("      Validity                : not after " << aString);
      OFString ecname;
      switch (cert.getKeyType())
      {
        case EKT_RSA:
          DCMSIGN_INFO("      Public key              : RSA, " << cert.getCertKeyBits() << " bits");
          break;
        case EKT_DSA:
          DCMSIGN_INFO("      Public key              : DSA, " << cert.getCertKeyBits() << " bits");
          break;
        case EKT_EC:
          ecname = cert.getCertCurveName();
          if (ecname.length() > 0)
          {
            DCMSIGN_INFO("      Public key              : EC, curve " << ecname << ", " << cert.getCertKeyBits() << " bits");
          }
          else
          {
            DCMSIGN_INFO("      Public key              : EC, " << cert.getCertKeyBits() << " bits");
          }
          break;
        case EKT_DH:
          DCMSIGN_INFO("      Public key              : DH, " << cert.getCertKeyBits() << " bits");
          break;
        default:
        case EKT_none: // should never happen
          DCMSIGN_INFO("      Public key              : unknown type");
          break;
      }
    }
  }
  return result;
}


OFCondition SiTimeStamp::verifyTSToken(
    SiCertificateVerifier& cv,
    DcmItem& ditem,
    SiCertificate& cert)
{
  // check if we have a timestamp object
  if (ts_ == NULL) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  errorCode_ = 0;

  // determine type of signature
  E_KeyType signatureType = cert.getKeyType();

  // create timestamp verification context
  TS_VERIFY_CTX *ctx = TS_VERIFY_CTX_new();

  if (ctx)
  {
    // these are the checks we want to perform
    TS_VERIFY_CTX_set_flags(ctx, TS_VFY_VERSION | TS_VFY_SIGNER | TS_VFY_DATA | TS_VFY_SIGNATURE);

    // get the signature from the DICOM dataset
    const Uint8 *signature = NULL;
    unsigned long sigLength = 0;
    result = ditem.findAndGetUint8Array(DCM_Signature, signature, &sigLength);
    if (result.bad() || sigLength < 4 || (signature == NULL))
    {
      DCMSIGN_ERROR("timestamp verification failed: signature in DICOM dataset not found or invalid");
      result = SI_EC_TimestampSignatureVerificationFailed;
    }

    // if the signature type is DSA or ECDSA, we need to account for a possible pad byte
    if (result.good())
    {
      if (signatureType == EKT_DSA || signatureType == EKT_EC)
      {
        DcmSignature::adjustASN1SequenceLength(signature, sigLength);
      }
    }

    if (result.good())
    {
      // In OpenSSL 1.0.1 and earlier, the first parameter was not const
      // We cast the const away, which should work with old and new versions
      BIO *bio = BIO_new_mem_buf(OFconst_cast(Uint8 *, signature), sigLength);
      if (bio)
      {
        // set the digital signature as the raw data against which
        // the imprint in the timestamp ticket should be checked
        TS_VERIFY_CTX_set_data(ctx, bio);

        // set certificate store and stack. Needed to verify that
        // the TSA field in the timestamp ticket matches the signer certificate
        TS_VERIFY_CTX_set_store(ctx, cv.getTrustedCertStore());
        TS_VERIFY_CTS_set_certs(ctx, cv.getUntrustedCerts());

        // run the verification function
        if (! TS_RESP_verify_token(ctx, ts_))
        {
          result = SI_EC_TimestampSignatureVerificationFailed;
          errorCode_ = ERR_get_error();
        }

        BIO_free(bio);
      }
      else
      {
        errorCode_ = -1;
        errorString_ = "timestamp verification failed: cannot create memory buffer";
        DCMSIGN_ERROR(errorString_);
        result = SI_EC_TimestampSignatureVerificationFailed;
      }
    }

    // prevent TS_VERIFY_CTX_free from double-deleting our objects
    TS_VERIFY_CTX_set_data(ctx, NULL);
    TS_VERIFY_CTX_set_store(ctx, NULL);
    TS_VERIFY_CTS_set_certs(ctx, NULL);

    TS_VERIFY_CTX_free(ctx);
  }

  if (result.good())
  {
    // compare timestamp against validity period of the certificate
    // for the DICOM signature. The timestamp should have been created while the
    // certificate is valid.
    X509 *rawcert = cert.getRawCertificate();
    const ASN1_TIME *notBefore = X509_get0_notBefore(rawcert);
    const ASN1_TIME *notAfter = X509_get0_notAfter(rawcert);
    const ASN1_GENERALIZEDTIME *gtime = TS_TST_INFO_get_time(tsinfo_);

    OFDateTime dt_notBefore;
    OFDateTime dt_notAfter;
    OFDateTime dt_gtime;

    if ((notBefore == NULL)||(notAfter == NULL)||(gtime == NULL)||
      SiCertificate::convertASN1Time(notBefore, dt_notBefore).bad() ||
      SiCertificate::convertASN1Time(notAfter, dt_notAfter).bad() ||
      SiCertificate::convertGeneralizedTime(gtime, dt_gtime).bad())
    {
      errorCode_ = -1;
      errorString_ = "comparison of timestamp date/time with signer certificate notBefore/notAfter attribute failed, value possibly malformed.";
      DCMSIGN_ERROR(errorString_);
      result = SI_EC_TimestampSignatureVerificationFailed;
    }
    else
    {
      if (dt_gtime < dt_notBefore)
      {
        errorCode_ = -1;
        errorString_ = "timestamp created before start of signer certificate validity.";
        DCMSIGN_ERROR(errorString_);
        result = SI_EC_TimestampSignatureVerificationFailed;
      }
      else if (dt_gtime > dt_notAfter)
      {
        errorCode_ = -1;
        errorString_ = "timestamp created after expiry of signer certificate.";
        DCMSIGN_ERROR(errorString_);
        result = SI_EC_TimestampSignatureVerificationFailed;
      }
    }
  }

  return result;
}


void SiTimeStamp::lastError(OFString& err) const
{
  if (errorCode_ < 0) err = errorString_;
  else
  {
    const char *c = ERR_reason_error_string(errorCode_);
    if (c) err = c; else err = "";
  }
}


#else /* WITH_OPENSSL */

int sitstamp_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
