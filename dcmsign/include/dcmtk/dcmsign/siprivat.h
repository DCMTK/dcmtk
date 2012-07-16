/*
 *
 *  Copyright (C) 1998-2011, OFFIS e.V.
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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiPrivateKey
 *
 */

#ifndef SIPRIVAT_H
#define SIPRIVAT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sitypes.h"

#ifdef WITH_OPENSSL

#include "dcmtk/ofstd/ofstring.h"

class SiAlgorithm;
class SiCertificate;
struct evp_pkey_st;
typedef struct evp_pkey_st EVP_PKEY;


/** a class representing a private key.
 */
class DCMTK_DCMSIGN_EXPORT SiPrivateKey
{     
public:
  /// default constructor
  SiPrivateKey();
  
  ///destructor
  virtual ~SiPrivateKey();

  /** sets the password string to be used when loading an
   *  encrypted private key file in PEM format (ASN.1/DER encoded files are never encrypted).
   *  Must be called prior to loadPrivateKey() in order to be effective.
   *  @param thePasswd password string, may be "" or NULL in which case an empty
   *    password is assumed.
   */
  void setPrivateKeyPasswd(const char *thePasswd);

  /** sets the password string to be used when loading an
   *  encrypted private key file to be read from the console stdin.
   */
  void setPrivateKeyPasswdFromConsole();

  /** loads a private key from file. If the private key is in encrypted PEM
   *  format, the password is either read from console (default) or taken
   *  from an internal setting created with setPrivateKeyPasswd().
   *  @param filename file name of key
   *  @param filetype file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return status code
   */
  OFCondition loadPrivateKey(const char *filename, int filetype);

  /** returns the type of public key stored in this certificate
   */
  E_KeyType getKeyType() const;
  
  /** creates an SiAlgorithm object for the private key contained in this certificate.
   *  If no key is loaded or operation fails, returns NULL.
   *  New SiAlgorithm object must be deleted by caller.
   *  @return pointer to new SiAlgorithm object
   */
  SiAlgorithm *createAlgorithmForPrivateKey();

  /** checks if the private key and the certificate set using setPrivateKeyFile()
   *  and setCertificateFile() match, i.e. if they establish a private/public key pair.
   *  @return OFTrue if private key and certificate match, OFFalse otherwise.
   */  
  OFBool matchesCertificate(SiCertificate& cert);

  /** provides access to the raw private key in OpenSSL format. Use with care!
   *  @return raw private key in OpenSSL format
   */
  EVP_PKEY *getRawPrivateKey();

private:

  /// private undefined copy constructor
  SiPrivateKey(SiPrivateKey& arg);

  /// private undefined copy assignment operator
  SiPrivateKey& operator=(SiPrivateKey& arg);

  /// contains the password for the private key if set on command line
  OFString privateKeyPasswd;

  /// true if the privateKeyPasswd contains the password, false otherwise.
  OFBool usePrivateKeyPassword;

  /// the private key managed by this object, may be NULL if not loaded yet
  EVP_PKEY* pkey;

};

#endif
#endif
