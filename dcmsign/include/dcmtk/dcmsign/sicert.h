/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:24 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SICERT_H
#define SICERT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sitypes.h"

#ifdef WITH_OPENSSL

#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */

class DcmItem;
class SiAlgorithm;
struct x509_st;
typedef struct x509_st X509;

/** a class representing X.509 public key certificates.
 */
class SiCertificate
{    
public:

  /// default constructor
  SiCertificate();
  
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

  /** returns a pointer to the raw certificate structure or NULL if no 
   *  certificate present. Should not be called by users of this library.
   */
  X509 *getRawCertificate();

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

/*
 *  $Log: sicert.h,v $
 *  Revision 1.8  2010-10-14 13:17:24  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.7  2010-02-22 11:39:54  uli
 *  Remove some unneeded includes.
 *
 *  Revision 1.6  2005-12-08 16:04:33  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2003/07/04 13:28:13  meichel
 *  Replaced forward declarations for OFString with explicit includes,
 *    needed when compiling with HAVE_STD_STRING
 *
 *  Revision 1.4  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.3  2001/09/26 14:30:19  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:47  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:48:53  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

