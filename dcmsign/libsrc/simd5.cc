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
 *  Update Date:      $Date: 2001-09-26 14:30:25 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "simd5.h"
#include "dcerror.h"

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

OFCondition SiMD5::initialize()
{
  MD5_Init(ctx);
  return EC_Normal;
}

OFCondition SiMD5::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return EC_IllegalCall;
  MD5_Update(ctx, data, length);
  return EC_Normal;
}

OFCondition SiMD5::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return EC_IllegalCall;
  MD5_Final(result, ctx);
  return EC_Normal;  
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
 *  Revision 1.3  2001-09-26 14:30:25  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:54  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:05  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

