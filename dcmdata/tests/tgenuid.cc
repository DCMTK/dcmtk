/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: test program for function dcmGenerateUniqueIdentifier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcuid.h"


OFTEST(dcmdata_generateUniqueIdentifier)
{
  // test generation of UIDs. We generate 10000 UIDs in quick
  // succession and check if they are pair-wise different.
  // This will also catch any application error during UID generation.

  char uid[65];
  OFString previousuid;

  // This variable name will be printed on the console if the test fails.
  // In fact a generated UID is not unique if this variable is false!
  OFBool generated_uid_is_not_unique;

  for (size_t i = 0; i < 10000; ++i)
  {
     // generate UID
     dcmGenerateUniqueIdentifier(uid);

     // compare with the previously generated UID, should be different.
     generated_uid_is_not_unique = (previousuid != uid);
     OFCHECK(generated_uid_is_not_unique);
     previousuid = uid;

     // if we have found a non-unique UID, the test has failed.
     // Break out of the loop.
     if (! generated_uid_is_not_unique) break;
  }
}
