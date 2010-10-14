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
 *    classes: SiSHA1
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisha1.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <openssl/sha.h>
END_EXTERN_C

 
SiSHA1::SiSHA1()
: ctx(new SHA_CTX())
{
  initialize();
}

SiSHA1::~SiSHA1()
{
  delete ctx;
}

unsigned long SiSHA1::getSize() const
{
  return SHA_DIGEST_LENGTH;
}

OFCondition SiSHA1::initialize()
{
  SHA1_Init(ctx);
  return EC_Normal;
}

OFCondition SiSHA1::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA1_Update(ctx, data, length);
  return EC_Normal;
}

OFCondition SiSHA1::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA1_Final(result, ctx);
  return EC_Normal;  
}

E_MACType SiSHA1::macType() const
{
  return EMT_SHA1;
}

const char *SiSHA1::getDefinedTerm() const
{
  return SI_DEFTERMS_SHA1;
}

#else /* WITH_OPENSSL */

int sisha1_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sisha1.cc,v $
 *  Revision 1.7  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.6  2005-12-08 15:47:30  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2002/12/16 12:57:53  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.4  2002/11/27 14:53:42  meichel
 *  Adapted module dcmsign to use of new header file ofstdinc.h
 *
 *  Revision 1.3  2001/09/26 14:30:26  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:55  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:08  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

