/*
 *
 *  Copyright (C) 2011-2017, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: test program for class DcmUniqueIdentifier and dcmGenerateUniqueIdentifier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmdata_uniqueIdentifier_1)
{
  /* test UI value with 0-byte padding */
  OFString value;
  DcmUniqueIdentifier sopInstanceUID(DCM_SOPInstanceUID);
  dcmEnableAutomaticInputDataCorrection.set(OFTrue);
  OFCHECK(sopInstanceUID.putString("1.2.3.4\0", 8).good());
  OFCHECK(sopInstanceUID.checkValue("1").good());
  OFCHECK(sopInstanceUID.getOFString(value, 0).good());
  OFCHECK_EQUAL(value, "1.2.3.4");
  // switch automatic data correct off
  dcmEnableAutomaticInputDataCorrection.set(OFFalse);
  OFCHECK(sopInstanceUID.putString("1.2.3.4\\5.6.7.8").good());
  OFCHECK(sopInstanceUID.checkValue("2").good());
  OFCHECK(sopInstanceUID.putString("1.2.3.4\\5.6.7.8\0", 16).good());
  // the trailing 0-byte is still there, which leads to an error
  OFCHECK(sopInstanceUID.checkValue("2").bad());
  OFCHECK(sopInstanceUID.getOFString(value, 1 /*, normalize = OFTrue */).good());
  OFCHECK_EQUAL(value, "5.6.7.8");
  OFCHECK(sopInstanceUID.getOFString(value, 1, OFFalse /*normalize*/).good());
  OFCHECK_EQUAL(value, OFString("5.6.7.8\0", 8));
}


OFTEST(dcmdata_uniqueIdentifier_2)
{
  /* test UI value with space character padding */
  OFString value;
  DcmUniqueIdentifier sopInstanceUID(DCM_SOPInstanceUID);
  dcmEnableAutomaticInputDataCorrection.set(OFTrue);
  OFCHECK(sopInstanceUID.putString("1.2.3.4 ").good());
  OFCHECK(sopInstanceUID.checkValue("1").good());
  OFCHECK(sopInstanceUID.putString("1.2.3.4  ").good());
  OFCHECK(sopInstanceUID.checkValue("1").good());
  OFCHECK(sopInstanceUID.getOFString(value, 0).good());
  OFCHECK_EQUAL(value, "1.2.3.4");
  // we also accept leading and embedded space characters
  OFCHECK(sopInstanceUID.putString(" 1.2.3.4 ").good());
  OFCHECK(sopInstanceUID.checkValue("1").good());
  OFCHECK(sopInstanceUID.putString("1.2. 3.4").good());
  OFCHECK(sopInstanceUID.checkValue("1").good());
  OFCHECK(sopInstanceUID.getOFString(value, 0).good());
  OFCHECK_EQUAL(value, "1.2.3.4");
  // switch automatic data correct off
  dcmEnableAutomaticInputDataCorrection.set(OFFalse);
  OFCHECK(sopInstanceUID.putString("1.2.3.4 ").good());
  OFCHECK(sopInstanceUID.checkValue("1").bad());
  OFCHECK(sopInstanceUID.getOFString(value, 0).good());
  OFCHECK_EQUAL(value, "1.2.3.4 ");
  OFCHECK(sopInstanceUID.putString("1.2.3.4\\5.6.7.8 ").good());
  OFCHECK(sopInstanceUID.checkValue("2").bad());
  OFCHECK(sopInstanceUID.getOFString(value, 0).good());
  OFCHECK_EQUAL(value, "1.2.3.4");
  OFCHECK(sopInstanceUID.getOFString(value, 1).good());
  OFCHECK_EQUAL(value, "5.6.7.8 ");
}

OFTEST(dcmdata_uniqueIdentifier_3)
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
