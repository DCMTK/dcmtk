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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiRIPEMD160
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 14:30:26 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "siripemd.h"
#include "dcerror.h"

BEGIN_EXTERN_C
#include <stdlib.h> /* for NULL */
#include <openssl/ripemd.h>
END_EXTERN_C

SiRIPEMD160::SiRIPEMD160()
: ctx(new RIPEMD160_CTX())
{
  initialize();
}

SiRIPEMD160::~SiRIPEMD160()
{
  delete ctx;
}

unsigned long SiRIPEMD160::getSize() const
{
  return RIPEMD160_DIGEST_LENGTH;
}

OFCondition SiRIPEMD160::initialize()
{
  RIPEMD160_Init(ctx);
  return EC_Normal;
}

OFCondition SiRIPEMD160::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return EC_IllegalCall;
  RIPEMD160_Update(ctx, data, length);
  return EC_Normal;
}

OFCondition SiRIPEMD160::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return EC_IllegalCall;
  RIPEMD160_Final(result, ctx);
  return EC_Normal;  
}

E_MACType SiRIPEMD160::macType() const
{
  return EMT_RIPEMD160;
}

const char *SiRIPEMD160::getDefinedTerm() const
{
  return SI_DEFTERMS_RIPEMD160;
}

#else /* WITH_OPENSSL */

const int siripemd_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: siripemd.cc,v $
 *  Revision 1.3  2001-09-26 14:30:26  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:55  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:07  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

