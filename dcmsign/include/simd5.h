/*
 *
 *  Copyright (C) 1998-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiMD5
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:49 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SIMD5_H
#define SIMD5_H

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "simac.h"
#include "sitypes.h"

struct MD5state_st;
typedef struct MD5state_st MD5_CTX;

/**
 * a class implementing the hash function MD5
 */
class SiMD5 : public SiMAC 
{    
public:
  /// default constructor
  SiMD5();

  /// destructor
  virtual ~SiMD5();
  
  /** initializes the MAC algorithm.
   *  @return status code
   */
  virtual SI_E_Condition initialize();
  
  /** feeds data into the MAC algorithm
   *  @param data pointer to raw data to be fed into the MAC, must not be NULL
   *  @param length number of bytes in raw data array
   *  @return status code
   */
  virtual SI_E_Condition digest(const unsigned char *data, unsigned long length);
  
  /** finalizes the MAC and writes it to the given output array,
   *  which must be at least getSize() bytes large.
   *  After a call to finalize, the MAC algorithm must be initialized
   *  again, see initialize().
   *  @param result pointer to array of getSize() bytes into which the MAC is written
   *  @return status code
   */
  virtual SI_E_Condition finalize(unsigned char *result);

  /** returns the size of a MAC in bytes.
   *  @return block size for this MAC algorithm
   */
  virtual unsigned long getSize() const;

  /** returns the type of MAC algorithm computed by this object
   *  @return type of MAC algorithm
   */
  virtual E_MACType macType() const;    

  /** returns the DICOM identifier for this MAC algorithm
   *  @return DICOM defined term for algorithm
   */
  virtual const char *getDefinedTerm() const;

private:

  /// private undefined copy constructor
  SiMD5(SiMD5& arg);

  /// private undefined copy assignment operator
  SiMD5& operator=(SiMD5& arg);

  /// OpenSSL MD5 context
  MD5_CTX *ctx;    
};

#endif
#endif

/*
 *  $Log: simd5.h,v $
 *  Revision 1.2  2001-06-01 15:50:49  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:48:56  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

