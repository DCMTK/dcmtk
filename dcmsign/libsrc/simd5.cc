/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Update Date:      $Date: 2000-11-07 16:49:05 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "simd5.h"

BEGIN_EXTERN_C
#include <stdlib.h> /* for NULL */
#include <openssl/md5.h>
END_EXTERN_C

 
SiMD5::SiMD5()
: ctx(new MD5_CTX())
{
  initialize();
}

SiMD5::~SiMD5()
{
  delete ctx;
}

unsigned long SiMD5::getSize() const
{
  return MD5_DIGEST_LENGTH;
}

SI_E_Condition SiMD5::initialize()
{
  MD5_Init(ctx);
  return SI_EC_Normal;
}

SI_E_Condition SiMD5::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return SI_EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return SI_EC_IllegalCall;
  MD5_Update(ctx, data, length);
  return SI_EC_Normal;
}

SI_E_Condition SiMD5::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return SI_EC_IllegalCall;
  MD5_Final(result, ctx);
  return SI_EC_Normal;  
}

E_MACType SiMD5::macType() const
{
  return EMT_MD5;
}

const char *SiMD5::getDefinedTerm() const
{
  return SI_DEFTERMS_MD5;
}


#else /* WITH_OPENSSL */

const int simd5_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: simd5.cc,v $
 *  Revision 1.1  2000-11-07 16:49:05  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

