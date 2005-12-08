/*
 *
 *  Copyright (C) 1998-2005, OFFIS
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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiRIPEMD160
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:04:42 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SIRIPEMD_H
#define SIRIPEMD_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/simac.h"
#include "dcmtk/dcmsign/sitypes.h"

#ifdef WITH_OPENSSL

struct RIPEMD160state_st;
typedef struct RIPEMD160state_st RIPEMD160_CTX;

/**
 * a class implementing the hash function RIPEMD160
 */
class SiRIPEMD160 : public SiMAC 
{    
public:
  /// default constructor
  SiRIPEMD160();

  /// destructor
  virtual ~SiRIPEMD160();
  
  /** initializes the MAC algorithm.
   *  @return status code
   */
  virtual OFCondition initialize();
  
  /** feeds data into the MAC algorithm
   *  @param data pointer to raw data to be fed into the MAC, must not be NULL
   *  @param length number of bytes in raw data array
   *  @return status code
   */
  virtual OFCondition digest(const unsigned char *data, unsigned long length);
  
  /** finalizes the MAC and writes it to the given output array,
   *  which must be at least getSize() bytes large.
   *  After a call to finalize, the MAC algorithm must be initialized
   *  again, see initialize().
   *  @param result pointer to array of getSize() bytes into which the MAC is written
   *  @return status code
   */
  virtual OFCondition finalize(unsigned char *result);

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
  SiRIPEMD160(SiRIPEMD160& arg);

  /// private undefined copy assignment operator
  SiRIPEMD160& operator=(SiRIPEMD160& arg);

  /// OpenSSL RIPEMD 160 context
  RIPEMD160_CTX *ctx;    
};

#endif
#endif

/*
 *  $Log: siripemd.h,v $
 *  Revision 1.5  2005-12-08 16:04:42  meichel
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
 *  Revision 1.1  2000/11/07 16:48:57  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

