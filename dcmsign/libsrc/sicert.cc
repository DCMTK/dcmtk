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

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/sipkey.h"
#include "dcmtk/dcmdata/dcstack.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/ofstd/ofdatime.h"

BEGIN_EXTERN_C
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
END_EXTERN_C

#ifndef HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTBEFORE
#define X509_get0_notBefore(x) X509_get_notBefore(x)
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTAFTER
#define X509_get0_notAfter(x) X509_get_notAfter(x)
#endif

#ifndef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_ID
#define EVP_PKEY_id(key) key->type
#endif


SiCertificate::SiCertificate()
: x509(NULL)
{
}

SiCertificate::SiCertificate(X509 *cert)
: x509(cert)
{
}

SiCertificate::~SiCertificate()
{
  if (x509) X509_free(x509);
}

E_KeyType SiCertificate::getKeyType()
{
  E_KeyType result = EKT_none;
  if (x509)
  {
    EVP_PKEY *pkey = X509_extract_key(x509);
    if (pkey)
    {
      switch(EVP_PKEY_type(EVP_PKEY_id(pkey)))
      {
        case EVP_PKEY_RSA:
          result = EKT_RSA;
          break;
        case EVP_PKEY_DSA:
          result = EKT_DSA;
          break;
        case EVP_PKEY_DH:
          result = EKT_DH;
          break;
        case EVP_PKEY_EC:
          result = EKT_EC;
          break;
        default:
          /* nothing */
          break;
      }
      EVP_PKEY_free(pkey);
    }
  }
  return result;
}

SiAlgorithm *SiCertificate::createAlgorithmForPublicKey()
{
  SiAlgorithm *result = NULL;
  if (x509)
  {
    EVP_PKEY *pkey = X509_extract_key(x509);
    if (pkey) result = new SiPKEY(pkey, OFTrue);
  }
  return result;
}

OFCondition SiCertificate::loadCertificate(const char *filename, int filetype)
{
  OFCondition result = SI_EC_CannotRead;
  if (x509) X509_free(x509);
  x509 = NULL;
  if (filename)
  {
    BIO *in = BIO_new(BIO_s_file());
    if (in)
    {
      if (BIO_read_filename(in, filename) > 0)
      {
        if (filetype == X509_FILETYPE_ASN1)
        {
          x509 = d2i_X509_bio(in, NULL);
          if (x509) result = EC_Normal;
        } else {
          x509 = PEM_read_bio_X509(in, NULL, NULL, NULL);
          if (x509) result = EC_Normal;
        }
      }
      BIO_free(in);
    }
  }
  return result;
}


OFCondition SiCertificate::read(DcmItem& item)
{
  OFCondition result = EC_Normal;
  if (x509) X509_free(x509);
  x509 = NULL;
  OFString aString;
  DcmStack stack;
  result = item.search(DCM_CertificateType, stack, ESM_fromHere, OFFalse);
  if (result.good())
  {
    result = ((DcmElement *)(stack.top()))->getOFString(aString, 0);
    if (result.good())
    {
      if (aString == SI_DEFTERMS_X509CERT)
      {
        stack.clear();
        result = item.search(DCM_CertificateOfSigner, stack, ESM_fromHere, OFFalse);
        if (result.good())
        {
          DcmElement *cert = (DcmElement *)stack.top();
          Uint8 *data = NULL;
          result = cert->getUint8Array(data);
          if (result.good())
          {
            if (data)
            {
              const Uint8 *cdata = data;
              x509 = d2i_X509(NULL, &cdata, cert->getLength());
              if (x509 == NULL)
              {
                DCMSIGN_WARN("Unable to parse X.509 certificate.");
                result = SI_EC_VerificationFailed_NoCertificate;
              }
            }
            else
            {
              DCMSIGN_WARN("Empty certificate of signer.");
              result = SI_EC_VerificationFailed_NoCertificate;
            }
          }
        }
        else
        {
          DCMSIGN_WARN("Certificate missing in dataset.");
          result = SI_EC_VerificationFailed_NoCertificate;
        }
      } else {
        DCMSIGN_WARN("Encountered unsupported certificate type '" << aString << "' (expected '" << SI_DEFTERMS_X509CERT << "').");
        result = SI_EC_VerificationFailed_NoCertificate;
      }
    }
  }
  else
  {
    DCMSIGN_WARN("Certificate type missing in dataset.");
    result = SI_EC_VerificationFailed_NoCertificate;
  }
  return result;
}

OFCondition SiCertificate::write(DcmItem& item)
{
  if (x509 == NULL) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  DcmElement *elem = new DcmCodeString(DCM_CertificateType);
  if (elem)
  {
    result = elem->putString(SI_DEFTERMS_X509CERT);
    if (result.good()) item.insert(elem, OFTrue); else delete elem;
  } else result = EC_MemoryExhausted;
  if (result.good())
  {
    elem = new DcmOtherByteOtherWord(DCM_CertificateOfSigner);
    if (elem)
    {
      int certLength = i2d_X509(x509, NULL);
      unsigned char *data = new unsigned char[certLength];
      if (data)
      {
        unsigned char *data2 = data;
        i2d_X509(x509, &data2); // data2 now points to the last element of the byte array
        result = elem->putUint8Array((Uint8 *)data, certLength);
        delete[] data;
        if (result.good()) item.insert(elem, OFTrue); else delete elem;
      } else {
        delete elem;
        result = EC_MemoryExhausted;
      }
    } else result = EC_MemoryExhausted;
  }
  return result;
}

X509 *SiCertificate::getRawCertificate()
{
  return x509;
}

long SiCertificate::getX509Version()
{
  if (x509 == NULL) return 0;
  return X509_get_version(x509)+1;
}

void SiCertificate::getCertSubjectName(OFString& str)
{
  if (x509)
  {
    char certSubjectName[2048];  /* certificate subject name (DN) */
    certSubjectName[0]= '\0';
    X509_NAME_oneline(X509_get_subject_name(x509), certSubjectName, 2048);
    str = certSubjectName;
  } else str.clear();
}

void SiCertificate::getCertIssuerName(OFString& str)
{
  if (x509)
  {
    char certIssuerName[2048];  /* certificate issuer name (DN) */
    certIssuerName[0]= '\0';
    X509_NAME_oneline(X509_get_issuer_name(x509), certIssuerName, 2048);
    str = certIssuerName;
  } else str.clear();
}

long SiCertificate::getCertSerialNo()
{
  if (x509 == NULL) return -1;
  return ASN1_INTEGER_get(X509_get_serialNumber(x509));
}

void SiCertificate::getCertValidityNotBefore(OFString& str)
{
  str.clear();
  if (x509)
  {
    char *bufptr = NULL;
    BIO *certValidNotBeforeBIO = BIO_new(BIO_s_mem());
    if (certValidNotBeforeBIO)
    {
      ASN1_UTCTIME_print(certValidNotBeforeBIO, X509_get0_notBefore(x509));
      BIO_write(certValidNotBeforeBIO,"\0",1);
      BIO_get_mem_data(certValidNotBeforeBIO, (char *)(&bufptr));
      if (bufptr) str = bufptr;
      BIO_free(certValidNotBeforeBIO);
    }
  }
}

void SiCertificate::getCertValidityNotAfter(OFString& str)
{
  str.clear();
  if (x509)
  {
    char *bufptr = NULL;
    BIO *certValidNotAfterBIO = BIO_new(BIO_s_mem());
    if (certValidNotAfterBIO)
    {
      ASN1_UTCTIME_print(certValidNotAfterBIO, X509_get0_notAfter(x509));
      BIO_write(certValidNotAfterBIO,"\0",1);
      BIO_get_mem_data(certValidNotAfterBIO, (char *)(&bufptr));
      if (bufptr) str = bufptr;
      BIO_free(certValidNotAfterBIO);
    }
  }
}

OFBool SiCertificate::isCertExpiredAt(OFString& date)
{
  OFBool result = OFTrue;
  if (x509)
  {
    const ASN1_TIME *certexpiry = X509_get0_notAfter(x509);
    OFDateTime dt_certexpiry;
    if (certexpiry && convertASN1Time(certexpiry, dt_certexpiry).good())
    {
      ASN1_TIME *sigdate = ASN1_TIME_new(); // ASN1_TIME is a typedef for ASN1_STRING
      if (sigdate)
      {
        OFDateTime dt_sigdate;
        if (ASN1_TIME_set_string(sigdate, date.c_str()) && convertASN1Time(sigdate, dt_sigdate).good())
        {
          // check if signature date is before certificate expiry
          if (dt_sigdate < dt_certexpiry) result = OFFalse;
        }
        ASN1_TIME_free(sigdate);
      }
    }
  }
  return result;
}

OFBool SiCertificate::isCertNotYetValidAt(OFString& date)
{
  OFBool result = OFTrue;
  if (x509)
  {
    const ASN1_TIME *certstart = X509_get0_notBefore(x509);
    OFDateTime dt_certstart;
    if (certstart && convertASN1Time(certstart, dt_certstart).good())
    {
      ASN1_TIME *sigdate = ASN1_TIME_new(); // ASN1_TIME is a typedef for ASN1_STRING
      if (sigdate)
      {
        OFDateTime dt_sigdate;
        if (ASN1_TIME_set_string(sigdate, date.c_str()) && convertASN1Time(sigdate, dt_sigdate).good())
        {
          // check if signature date is after certificate validity start
          if (dt_sigdate > dt_certstart) result = OFFalse;
        }
        ASN1_TIME_free(sigdate);
      }
    }
  }
  return result;
}


OFBool SiCertificate::isCertExpiredNow() const
{
  OFBool result = OFTrue;
  if (x509)
  {
    const ASN1_TIME *certexpiry = X509_get0_notAfter(x509);
    if (certexpiry)
    {
      // X509_cmp_current_time() will return -1 when certexpiry is in the past
      // and zero if there is an error. In both cases we treat the certificate as expired.
      if (X509_cmp_current_time(certexpiry) > 0) result = OFFalse;
    }
  }
  return result;
}


OFBool SiCertificate::isCertNotYetValidNow() const
{
  OFBool result = OFTrue;
  if (x509)
  {
    const ASN1_TIME *certstart = X509_get0_notBefore(x509);
    if (certstart)
    {
      // X509_cmp_current_time() will return 1 when certstart is in the future
      // and zero if there is an error. In both cases we treat the certificate as invalid.
      if (X509_cmp_current_time(certstart) < 0) result = OFFalse;
    }
  }
  return result;
}


long SiCertificate::getCertKeyBits()
{
  long certPubKeyBits = 0;                   /* certificate number of bits in public key */
  if (x509)
  {
    EVP_PKEY *pubkey = X509_get_pubkey(x509); // creates copy of public key
    if (pubkey)
    {
      certPubKeyBits = EVP_PKEY_bits(pubkey);
      EVP_PKEY_free(pubkey);
    }
  }
  return certPubKeyBits;
}

OFString SiCertificate::getCertCurveName()
{
  OFString result;
#ifndef OPENSSL_NO_EC
  if (x509)
  {
    EVP_PKEY *pkey = X509_extract_key(x509);
    if (pkey && EVP_PKEY_type(EVP_PKEY_id(pkey)) == EVP_PKEY_EC)
    {
      // we have an elliptic curve.
#ifdef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET_GROUP_NAME
      /* code for OpenSSL 3.0 and newer */
      char groupname[100];
      groupname[0] = '\0';
      EVP_PKEY_get_group_name(pkey, groupname, 100,  NULL);
      result = groupname;
#else
      /* code for older OpenSSL versions */
#ifndef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET0_EC_KEY
      EC_KEY *eckey = EVP_PKEY_get1_EC_KEY(pkey);
#else
      const EC_KEY *eckey = EVP_PKEY_get0_EC_KEY(pkey);
#endif
      if (eckey)
      {
        // access EC group within EC key
        const EC_GROUP *ecgroup = EC_KEY_get0_group(eckey);
        if (ecgroup)
        {
          // check if we have a named curve
          int nid = EC_GROUP_get_curve_name(ecgroup);
          if (nid > 0)
          {
            result = OBJ_nid2sn(nid);
          }
          else result = "unnamed curve";
        }
#ifndef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET0_EC_KEY
        EC_KEY_free(eckey);
#endif
      }
#endif /* HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET_GROUP_NAME */

      EVP_PKEY_free(pkey);
    }
  }
#endif
  return result;
}

OFBool SiCertificate::isWeakKey()
{
  OFBool result = OFTrue;
  long bits = getCertKeyBits();
  switch (getKeyType())
  {
    case EKT_RSA:
    case EKT_DSA:
    case EKT_DH:
      if (bits >= 1024) result = OFFalse;
      break;
    case EKT_EC:
      if (bits >= 256) result = OFFalse;
      break;
    case EKT_none: // should never happen
      break;
  }
  return result;
}

void SiCertificate::checkForWeakKey()
{
  if (isWeakKey())
  {
    switch (getKeyType())
    {
      case EKT_RSA:
        DCMSIGN_WARN("Certificate contains a weak key: RSA, " << getCertKeyBits() << " bits");
        break;
      case EKT_DSA:
        DCMSIGN_WARN("Certificate contains a weak key: DSA, " << getCertKeyBits() << " bits");
        break;
      case EKT_EC:
        DCMSIGN_WARN("Certificate contains a weak key: EC, " << getCertKeyBits() << " bits");
        break;
      case EKT_DH:
        DCMSIGN_WARN("Certificate contains a weak key: DH, " << getCertKeyBits() << " bits");
        break;
      case EKT_none: // should never happen
        DCMSIGN_WARN("Certificate contains a weak key: unknown type");
        break;
    }
  }
}

OFCondition SiCertificate::convertGeneralizedTime(const ASN1_GENERALIZEDTIME *d, OFDateTime& dt)
{
    // This method is derived from OpenSSL's asn1_generalizedtime_to_tm(),
    // which was introduced in OpenSSL 1.0.2. Since we still support OpenSSL 1.0.1,
    // we cannot use that function.

    static const int minval[9] = { 0, 0, 1, 1, 0, 0, 0, 0, 0 };
    static const int maxval[9] = { 99, 99, 12, 31, 23, 59, 59, 12, 59 };

    unsigned int dt_year = 0;
    unsigned int dt_month = 0;
    unsigned int dt_day = 0;
    unsigned int dt_hour = 0;
    unsigned int dt_minute = 0;
    double dt_second = 0.0;
    double dt_timeZone = 0.0;
    char *a;
    int n, i, l, o;

    if (d == NULL) return EC_IllegalCall;
    if (d->type != V_ASN1_GENERALIZEDTIME) return EC_IllegalCall;
    l = d->length;
    a = (char *)d->data;
    o = 0;
    /*
     * GENERALIZEDTIME is similar to UTCTIME except the year is represented
     * as YYYY. This stuff treats everything as a two digit field so make
     * first two fields 00 to 99
     */
    if (l < 13) return EC_IllegalCall;
    for (i = 0; i < 7; i++) {
        if ((i == 6) && ((a[o] == 'Z') || (a[o] == '+') || (a[o] == '-'))) {
            i++;
            dt_second = 0;
            break;
        }
        if ((a[o] < '0') || (a[o] > '9')) return EC_IllegalCall;
        n = a[o] - '0';
        if (++o > l) return EC_IllegalCall;

        if ((a[o] < '0') || (a[o] > '9')) return EC_IllegalCall;
        n = (n * 10) + a[o] - '0';
        if (++o > l) return EC_IllegalCall;

        if ((n < minval[i]) || (n > maxval[i])) return EC_IllegalCall;
        switch (i)
       {
          case 0:
            dt_year = n * 100;
            break;
          case 1:
            dt_year += n;
            break;
          case 2:
            dt_month = n;
            break;
          case 3:
            dt_day = n;
            break;
          case 4:
            dt_hour = n;
            break;
          case 5:
            dt_minute = n;
            break;
          case 6:
            dt_second = n;
            break;
        }
    }
    /*
     * Optional fractional seconds: decimal point followed by one or more
     * digits.
     */
    if (a[o] == '.') {
        if (++o > l) return EC_IllegalCall;
        i = o;
        while ((a[o] >= '0') && (a[o] <= '9') && (o <= l)) o++;
        /* Must have at least one digit after decimal point */
        if (i == o) return EC_IllegalCall;
    }

    if (a[o] == 'Z')
        o++;
    else if ((a[o] == '+') || (a[o] == '-')) {
        int offsign = a[o] == '-' ? 1 : -1, offset = 0;
        o++;
        if (o + 4 > l)
            return EC_IllegalCall;
        for (i = 7; i < 9; i++) {
            if ((a[o] < '0') || (a[o] > '9')) return EC_IllegalCall;
            n = a[o] - '0';
            o++;
            if ((a[o] < '0') || (a[o] > '9')) return EC_IllegalCall;
            n = (n * 10) + a[o] - '0';
            if ((n < minval[i]) || (n > maxval[i])) return EC_IllegalCall;
            if (i == 7)
                offset = n * 3600;
            else if (i == 8)
                offset += n * 60;
            o++;
        }
        if (offset) dt_timeZone = offset / 3600.0 * offsign;
    } else if (a[o]) {
        /* Missing time zone information. */
        return EC_IllegalCall;
    }
    if (! dt.setDateTime(dt_year, dt_month, dt_day, dt_hour, dt_minute, dt_second, dt_timeZone)) return EC_IllegalCall;

    return ((o == l) ? EC_Normal : EC_IllegalCall);
}


OFCondition SiCertificate::convertASN1Time(const ASN1_TIME *d, OFDateTime& dt)
{
  if (d)
  {
    // Before OpenSSL 1.1.0, the first parameter of ASN1_TIME_to_generalizedtime()
    // was not declared const. We cast the const away, which should work with old and new versions.
    ASN1_GENERALIZEDTIME *gm = ASN1_TIME_to_generalizedtime(OFconst_cast(ASN1_TIME *, d), NULL);
    if (gm)
    {
      OFCondition result = convertGeneralizedTime(gm, dt);
      ASN1_GENERALIZEDTIME_free(gm);
      return result;
    }
  }
  return EC_IllegalCall;
}


#else /* WITH_OPENSSL */

int sicert_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
