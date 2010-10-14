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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiRSA
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:25 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SIRSA_H
#define SIRSA_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sialgo.h"

#ifdef WITH_OPENSSL

#include "dcmtk/ofstd/oftypes.h"

class SiPrivateKey;
struct rsa_st;
typedef struct rsa_st RSA;

/** 
 *  This class implements the RSA public key crypto algorithms.
 *  
 */

class SiRSA : public SiAlgorithm
{   
public:

  /** constructor
   *  @param pointer to public RSA key
   */
  SiRSA(RSA *key);
  
  /// destructor
  virtual ~SiRSA();

  /** creates a signature.
   *  @param inputHash array of hash key bytes that are to be signed
   *  @param inputHashSize length of hash key array in bytes
   *  @param inputHashAlgorithm MAC algorithm used for creation of hash key.
   *    Required for creation of PKCS#1 RSA signature padding.
   *  @param outputSignature pointer to array of at least getSize() which must be allocated by caller.
   *  @param outputSignatureSize returns the number of bytes written to outputSignature.
   *  @return SI_EC_Normal if successful, errorcode otherwise.
   */
  virtual OFCondition sign(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    unsigned char *outputSignature,
    unsigned long &outputSignatureSize);

  /** verifies a signature.
   *  @param inputHash array of bytes containing hash key to be verified against signature
   *  @param inputHashSize length of hash key array in bytes
   *  @param inputHashAlgorithm MAC algorithm used for creation of hash key.
   *    Required for creation of PKCS#1 RSA signature padding.
   *  @param inputSignature array of bytes containing signature to be verified
   *  @param inputSignatureSize length of signature array in bytes
   *  @param verified returns whether the signature was successfully verified
   *  @return SI_EC_Normal if successful, errorcode otherwise.
   */
  virtual OFCondition verify(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    const unsigned char *inputSignature,
    unsigned long inputSignatureSize,
    OFBool &verified);

  /** returns the size of a block of encrypted/decrypted ciphertext in bytes.
   *  The result depends on the public key algorithm, key size and padding scheme.
   *  In general the input to decrypt() or encrypt() must be less than or equal
   *  to this block size.  The output of decrypt() or encrypt() is always equal
   *  to this block size.
   *  @return block size for this public key cryptosystem and key
   */
  virtual unsigned long getSize() const;

  /** returns the type of public key algorithm computed by this object
   *  @return type of public key algorithm
   */
  virtual E_KeyType keyType() const;    

private:

  /// private undefined copy constructor
  SiRSA(SiRSA& arg);

  /// private undefined copy assignment operator
  SiRSA& operator=(SiRSA& arg);

  /// RSA key used for signature/verification
  RSA *rsa;

};

#endif
#endif

/*
 *  $Log: sirsa.h,v $
 *  Revision 1.6  2010-10-14 13:17:25  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2005-12-08 16:04:43  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.4  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.3  2001/09/26 14:30:21  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:50  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:48:58  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

