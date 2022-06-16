/*
 *
 *  Copyright (C) 2011-2022, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class DcmUniqueIdentifier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmdata_uniqueIdentifier_1)
{
  /* test UI value with 0-byte padding */
  OFString value;
  DcmUniqueIdentifier sopInstanceUID(DCM_SOPInstanceUID);
  dcmEnableAutomaticInputDataCorrection.set(OFTrue);
  OFCHECK(sopInstanceUID.putString("1.2.3.4\0", 8).good());
  OFCHECK_EQUAL(sopInstanceUID.getNumberOfValues(), 1);
  OFCHECK(sopInstanceUID.checkValue("1").good());
  OFCHECK(sopInstanceUID.getOFString(value, 0).good());
  OFCHECK_EQUAL(value, "1.2.3.4");
  // switch automatic data correct off
  dcmEnableAutomaticInputDataCorrection.set(OFFalse);
  OFCHECK(sopInstanceUID.putString("1.2.3.4\\5.6.7.8").good());
  OFCHECK_EQUAL(sopInstanceUID.getNumberOfValues(), 2);
  OFCHECK(sopInstanceUID.checkValue("2").good());
  OFCHECK(sopInstanceUID.putString("1.2.3.4\\5.6.7.8\0", 16).good());
  // the trailing 0-byte is still there, but this is not an error
  OFCHECK(sopInstanceUID.checkValue("2").good());
  OFCHECK(sopInstanceUID.getOFString(value, 1 /*, normalize = OFTrue */).good());
  OFCHECK_EQUAL(value, "5.6.7.8");
  OFCHECK(sopInstanceUID.getOFString(value, 1, OFFalse /*normalize*/).good());
  OFCHECK_EQUAL(value, OFString("5.6.7.8\0", 8));
  // check UI value with two trailing 0-bytes, which is not allowed
  OFCHECK(sopInstanceUID.putString("7.8.9.10\0\0", 10).good());
  OFCHECK(sopInstanceUID.checkValue("1").bad());
  OFCHECK(sopInstanceUID.getOFString(value, 0 /*, normalize = OFTrue */).good());
  OFCHECK_EQUAL(value, "7.8.9.10");
  OFCHECK(sopInstanceUID.getOFString(value, 0, OFFalse /*normalize*/).good());
  OFCHECK_EQUAL(value, OFString("7.8.9.10\0\0", 10));
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
