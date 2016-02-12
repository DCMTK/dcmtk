/*
 *
 *  Copyright (C) 2009-2016, OFFIS e.V.
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
 *  Purpose: test program for checkStringValue() methods
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/vrscan.h"
#include "dcmtk/ofstd/ofbmanip.h"


#define CHECK_GOOD(number, check) do { \
  if (!(check).good()) \
    OFCHECK_FAIL(number ": " #check); \
} while (0);

#define CHECK_BAD(number, check) do { \
  if (!(check).bad()) \
    OFCHECK_FAIL(number ": " #check); \
} while (0);

OFTEST(dcmdata_checkStringValue)
{
  /* test "Application Entity" */
  CHECK_BAD ( "AE-01", DcmApplicationEntity::checkStringValue(" ", "1") )
  CHECK_GOOD( "AE-02", DcmApplicationEntity::checkStringValue("A\\") )
  CHECK_GOOD( "AE-03", DcmApplicationEntity::checkStringValue(" AETITLE", "1") )
  CHECK_GOOD( "AE-04", DcmApplicationEntity::checkStringValue("AE-Title  ", "1") )
  CHECK_GOOD( "AE-05", DcmApplicationEntity::checkStringValue(" AE Title ", "1") )
  CHECK_GOOD( "AE-06", DcmApplicationEntity::checkStringValue("A \\ B", "2") )
  CHECK_BAD ( "AE-07", DcmApplicationEntity::checkStringValue("J\366rg", "1") )
  CHECK_BAD ( "AE-08", DcmApplicationEntity::checkStringValue("1234567890-ABCDE ") )
  CHECK_GOOD( "AE-09", DcmApplicationEntity::checkStringValue("\\ AE \\", "3-3n") )

  /* test "Age String" */
  CHECK_GOOD( "AS-01", DcmAgeString::checkStringValue("001Y", "1") )
  CHECK_GOOD( "AS-02", DcmAgeString::checkStringValue("099Y", "1") )
  CHECK_BAD ( "AS-03", DcmAgeString::checkStringValue("099Z", "1") )
  CHECK_GOOD( "AS-04", DcmAgeString::checkStringValue("199D\\000W\\500M", "2-n") )
  CHECK_BAD ( "AS-05", DcmAgeString::checkStringValue("Y010", "1") )
  CHECK_BAD ( "AS-06", DcmAgeString::checkStringValue("10Y", "1") )
  CHECK_BAD ( "AS-07", DcmAgeString::checkStringValue("010 Y", "1") )
  CHECK_BAD ( "AS-08", DcmAgeString::checkStringValue(" 010Y", "1") )
  CHECK_BAD ( "AS-09", DcmAgeString::checkStringValue("010Y ", "1") )
  CHECK_BAD ( "AS-10", DcmAgeString::checkStringValue("0109Y", "1") )

  /* test "Code String" */
  CHECK_GOOD( "CS-01", DcmCodeString::checkStringValue("TEST 1234567890\\TEST", "2-2n") )
  CHECK_BAD ( "CS-02", DcmCodeString::checkStringValue("TEST 1234567890\\TEST\\", "2-2n") )
  CHECK_GOOD( "CS-03", DcmCodeString::checkStringValue("", "2-2n") )
  CHECK_GOOD( "CS-04", DcmCodeString::checkStringValue("\\TEST 1234567890", "2") )
  CHECK_GOOD( "CS-05", DcmCodeString::checkStringValue("\\ ", "2") )
  CHECK_BAD ( "CS-06", DcmCodeString::checkStringValue("\\TEST 1234567890 A", "2") )
  CHECK_BAD ( "CS-07", DcmCodeString::checkStringValue("J\366rg", "1-2") )
  CHECK_BAD ( "CS-08", DcmCodeString::checkStringValue("Joerg", "1-2") )
  CHECK_GOOD( "CS-09", DcmCodeString::checkStringValue("MARCO \\ JOERG", "1-2") )
  CHECK_GOOD( "CS-10", DcmCodeString::checkStringValue("MARCO\\ MICHAEL \\JOERG\\") )
  CHECK_BAD ( "CS-11", DcmCodeString::checkStringValue("    1234567890    ") )
  CHECK_BAD ( "CS-12", DcmCodeString::checkStringValue("1234567890-ABCDE") )
  CHECK_GOOD( "CS-13", DcmCodeString::checkStringValue("                ") )
  CHECK_BAD ( "CS-14", DcmCodeString::checkStringValue("                 ") )

  /* test "Date" */
  CHECK_GOOD( "DA-01", DcmDate::checkStringValue("20090731", "1") )
  CHECK_GOOD( "DA-02", DcmDate::checkStringValue("18200101", "1") )
  CHECK_GOOD( "DA-03", DcmDate::checkStringValue("21001231", "1") )
  CHECK_BAD ( "DA-04", DcmDate::checkStringValue("1820.01.01", "1", OFFalse) )
  CHECK_GOOD( "DA-05", DcmDate::checkStringValue("1820.01.01", "1", OFTrue) )
  CHECK_GOOD( "DA-06", DcmDate::checkStringValue("20090731\\18200101") )
  CHECK_GOOD( "DA-07", DcmDate::checkStringValue("20090731\\18200101", "2-2n") )
  CHECK_GOOD( "DA-08", DcmDate::checkStringValue("20090731\\18200101\\20000101\\21001231", "2-2n") )
  CHECK_BAD ( "DA-09", DcmDate::checkStringValue("20090731", "6") )
  CHECK_BAD ( "DA-10", DcmDate::checkStringValue("20000001", "1") )
  CHECK_BAD ( "DA-11", DcmDate::checkStringValue("00001232", "1") )
  CHECK_BAD ( "DA-12", DcmDate::checkStringValue("20000000", "1") )

  /* test "Date Time" */
  CHECK_GOOD( "DT-01", DcmDateTime::checkStringValue("200907311230", "1") )
  CHECK_GOOD( "DT-02", DcmDateTime::checkStringValue("18200101000000", "1") )
  CHECK_GOOD( "DT-03", DcmDateTime::checkStringValue("21001231235959.000001", "1") )
  CHECK_GOOD( "DT-04", DcmDateTime::checkStringValue("20090731", "1") )
  CHECK_BAD ( "DT-05", DcmDateTime::checkStringValue("200907312400", "1") )
  CHECK_BAD ( "DT-06", DcmDateTime::checkStringValue("20090731 1200") )
  CHECK_GOOD( "DT-07", DcmDateTime::checkStringValue("2009073100\\1820010112", "2-2n") )
  CHECK_GOOD( "DT-08", DcmDateTime::checkStringValue("2009073100\\1820010112\\200001010030\\21001231171540.123", "2-2n") )
  CHECK_GOOD( "DT-09", DcmDateTime::checkStringValue("200907311230+0100", "1") )
  CHECK_GOOD( "DT-10", DcmDateTime::checkStringValue("20090731123000+1000", "1") )
  CHECK_BAD ( "DT-11", DcmDateTime::checkStringValue("200907311230+1900", "1") )
  CHECK_GOOD( "DT-12", DcmDateTime::checkStringValue("20090731123000.123456+0100", "1") )
  CHECK_BAD ( "DT-13", DcmDateTime::checkStringValue("20090731123000.123456+0100 ", "1") )
  CHECK_GOOD( "DT-14", DcmDateTime::checkStringValue("200907311230", "1") )
  CHECK_GOOD( "DT-15", DcmDateTime::checkStringValue("200907", "1") )
  CHECK_GOOD( "DT-16", DcmDateTime::checkStringValue("2009", "1") )
  CHECK_BAD ( "DT-17", DcmDateTime::checkStringValue("20091", "1") )

  /* test "Decimal String" */
  CHECK_GOOD( "DS-01", DcmDecimalString::checkStringValue(" 0", "1") )
  CHECK_GOOD( "DS-02", DcmDecimalString::checkStringValue("0.0 ", "1") )
  CHECK_GOOD( "DS-03", DcmDecimalString::checkStringValue("-1", "1") )
  CHECK_GOOD( "DS-04", DcmDecimalString::checkStringValue("+12345.67890", "1") )
  CHECK_BAD ( "DS-05", DcmDecimalString::checkStringValue("   -12345.6789   ", "1") )
  CHECK_GOOD( "DS-06", DcmDecimalString::checkStringValue("01.01", "1") )
  CHECK_BAD ( "DS-06", DcmDecimalString::checkStringValue("0.1.01", "1") )
  CHECK_BAD ( "DS-07", DcmDecimalString::checkStringValue("0 1", "1") )
  CHECK_BAD ( "DS-08", DcmDecimalString::checkStringValue("0-1", "1") )
  CHECK_GOOD( "DS-09", DcmDecimalString::checkStringValue("1.0E99", "1") )
  CHECK_BAD ( "DS-10", DcmDecimalString::checkStringValue("1.099E", "1") )
  CHECK_GOOD( "DS-11", DcmDecimalString::checkStringValue("-10e+99", "1") )
  CHECK_GOOD( "DS-12", DcmDecimalString::checkStringValue("-10e+99\\+1E-1", "2") )

  /* test "Integer String" */
  CHECK_GOOD( "IS-01", DcmIntegerString::checkStringValue("0", "1") )
  CHECK_GOOD( "IS-02", DcmIntegerString::checkStringValue("+0", "1") )
  CHECK_GOOD( "IS-03", DcmIntegerString::checkStringValue("-0", "1") )
  CHECK_GOOD( "IS-04", DcmIntegerString::checkStringValue("00", "1") )
  CHECK_GOOD( "IS-05", DcmIntegerString::checkStringValue("000000000001", "1") )
  CHECK_BAD ( "IS-06", DcmIntegerString::checkStringValue("1000000000001", "1") )
  CHECK_BAD ( "IS-07", DcmIntegerString::checkStringValue("-000000000001", "1") )
  CHECK_BAD ( "IS-08", DcmIntegerString::checkStringValue("3000000000", "1") )
  CHECK_GOOD( "IS-08", DcmIntegerString::checkStringValue("+2000000000", "1") )
  CHECK_GOOD( "IS-09", DcmIntegerString::checkStringValue("+2147483647", "1") )
  CHECK_BAD ( "IS-10", DcmIntegerString::checkStringValue("2147483648", "1") )
  CHECK_GOOD( "IS-11", DcmIntegerString::checkStringValue("-2147483648", "1") )
  CHECK_BAD ( "IS-12", DcmIntegerString::checkStringValue("-2147483649", "1") )
  CHECK_GOOD( "IS-13", DcmIntegerString::checkStringValue(" -2147483648\\+2147483647 ", "2") )
  CHECK_BAD ( "IS-14", DcmIntegerString::checkStringValue(" -2147483648 \\+2147483647", "2") )

  /* test "Long String" */
  CHECK_GOOD( "LO-01", DcmLongString::checkStringValue(" ", "1") )
  CHECK_GOOD( "LO-02", DcmLongString::checkStringValue(" . ", "1") )
  CHECK_GOOD( "LO-03", DcmLongString::checkStringValue("ABC 123 abc _+-/", "1") )
  CHECK_GOOD( "LO-04", DcmLongString::checkStringValue("  ", "1") )
  CHECK_GOOD( "LO-05", DcmLongString::checkStringValue("ABC\\123", "2") )
  CHECK_GOOD( "LO-06", DcmLongString::checkStringValue(" J\366rg Riesmeier ", "1", "ISO_IR 100") )
// maximum length cannot be checked if given in characters (and not bytes)
//  CHECK_BAD ( "LO-07", DcmLongString::checkStringValueu("OFFIS e.V., Escherweg 2, 26121 Oldenburg, Germany, http://www.offis.de/", "1") )
  CHECK_GOOD( "LO-08", DcmLongString::checkStringValue("\\ _2_ \\ _3_ \\ _4_ \\ _5_ \\", "6") )
  CHECK_GOOD( "LO-09", DcmLongString::checkStringValue("ESC\033aping", "1") )
  CHECK_BAD ( "LO-10", DcmLongString::checkStringValue("not allowed: \r\014", "1") )

  /* test "Long Text" */
  CHECK_GOOD( "LT-01", DcmLongText::checkStringValue(" Hello \\ 12345 \\ \344\366\374\337 ", "ISO_IR 100") )
  CHECK_GOOD( "LT-02", DcmLongText::checkStringValue(" permitted control characters: \011 \012 \014 \015 ") )
  CHECK_GOOD( "LT-03", DcmLongText::checkStringValue(" ") )

  /* test "Person Name" */
  CHECK_GOOD( "PN-01", DcmPersonName::checkStringValue("A^Riesmeier^^=R^J\366rg", "1", "ISO_IR 100") )
  CHECK_GOOD( "PN-02", DcmPersonName::checkStringValue("A^Riesmeier^^=R^J\366rg\\", "1-n", "ISO_IR 100") )
  CHECK_BAD ( "PN-03", DcmPersonName::checkStringValue("A^Riesmeier^^=R^J\366rg\\", "1") )
  CHECK_GOOD( "PN-04", DcmPersonName::checkStringValue("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", "1", "ISO_IR 100") )
  CHECK_GOOD( "PN-05", DcmPersonName::checkStringValue("Eichelberg\\Onken\\Riesmeier", "3") )
  CHECK_GOOD( "PN-06", DcmPersonName::checkStringValue(" ") )
  CHECK_GOOD( "PN-07", DcmPersonName::checkStringValue(" A") )
  CHECK_GOOD( "PN-08", DcmPersonName::checkStringValue("^^^^") )
  CHECK_BAD ( "PN-09", DcmPersonName::checkStringValue("^ J\366rg ^") )
  CHECK_GOOD( "PN-10", DcmPersonName::checkStringValue("^^^^MD ") )
  CHECK_BAD ( "PN-11", DcmPersonName::checkStringValue("^^^^^") )
  CHECK_GOOD( "PN-12", DcmPersonName::checkStringValue("A^B^C^D^E=F^G^H^I^J=K^L^M^N^O") )
  /* The almighty DICOM standard says:
   *   Any component group may be absent, including the first component group.
   *   In this case, the person name may start with one or more “=” delimiters.
   */
  CHECK_GOOD( "PN-13", DcmPersonName::checkStringValue("==Test") )
  CHECK_GOOD( "PN-14", DcmPersonName::checkStringValue("Test==") )
  CHECK_GOOD( "PN-15", DcmPersonName::checkStringValue("==") )
  CHECK_BAD ( "PN-16", DcmPersonName::checkStringValue("===") )
  CHECK_BAD ( "PN-17", DcmPersonName::checkStringValue("^=^=^=^") )
  CHECK_GOOD( "PN-18", DcmPersonName::checkStringValue("^^^^=^^^^=^^^^") )
  CHECK_GOOD( "PN-19", DcmPersonName::checkStringValue("") )
  /* further tests for component groups */
  CHECK_GOOD( "PN-20", DcmPersonName::checkStringValue("Unspecified^^^^=^^^^=^^^^") )
  CHECK_GOOD( "PN-21", DcmPersonName::checkStringValue("^^^^=^^Unspecified^^=^^^^") )
  CHECK_GOOD( "PN-22", DcmPersonName::checkStringValue("^^^^=^^^^=^^^^Unspecified") )

  /* test "Short String" */
  CHECK_GOOD( "SH-01", DcmShortString::checkStringValue(" ", "1") )
  CHECK_GOOD( "SH-02", DcmShortString::checkStringValue(" . ", "1") )
  CHECK_GOOD( "SH-03", DcmShortString::checkStringValue("ABC 123 abc _+-/", "1") )
  CHECK_GOOD( "SH-04", DcmShortString::checkStringValue("  ", "1") )
  CHECK_GOOD( "SH-05", DcmShortString::checkStringValue("ABC\\123", "2") )
  CHECK_GOOD( "SH-06", DcmShortString::checkStringValue(" J\366rg Riesmeier ", "1", "ISO_IR 100") )
// maximum length cannot be checked if given in characters (and not bytes)
//  CHECK_BAD ( "SH-07", DcmShortString::checkStringValue("OFFIS e.V., Oldenburg", "1") )
  CHECK_GOOD( "SH-08", DcmShortString::checkStringValue("\\ _2_ \\ _3_ \\ _4_ \\ _5_ \\", "6") )
  CHECK_BAD ( "SH-09", DcmShortString::checkStringValue(" ", "2") )
  CHECK_GOOD( "SH-10", DcmShortString::checkStringValue("", "2") )
  CHECK_GOOD( "SH-11", DcmShortString::checkStringValue("ESC\033aping", "1") )
  CHECK_BAD ( "SH-12", DcmShortString::checkStringValue("not allowed: \n\010\r\014", "1") )

  /* test "Short Text" */
  CHECK_GOOD( "ST-01", DcmShortText::checkStringValue(" umlaut characters are allowed: \304\326\334\344\366\374\naccented characters also: \341\340\351\350\355\354\342\352\364\rand control characters, of course, including \033=ESC ", "ISO_IR 100") )
  CHECK_BAD ( "ST-02", DcmShortText::checkStringValue(" other control characters are not allowed: \013 \010 \200 ") )
  CHECK_GOOD( "ST-03", DcmShortText::checkStringValue("  .  ") )
  CHECK_GOOD( "ST-04", DcmShortText::checkStringValue("     ") )

  /* test "Time" */
  CHECK_GOOD( "TM-01", DcmTime::checkStringValue("0000", "1") )
  CHECK_BAD ( "TM-02", DcmTime::checkStringValue("2400", "1") )
  CHECK_GOOD( "TM-03", DcmTime::checkStringValue("123045", "1") )
  CHECK_GOOD( "TM-04", DcmTime::checkStringValue("235959.123", "1") )
  CHECK_GOOD( "TM-05", DcmTime::checkStringValue("235959.987654", "1") )
  CHECK_BAD ( "TM-06", DcmTime::checkStringValue("000000.0000000", "1") )
  CHECK_GOOD( "TM-07", DcmTime::checkStringValue("1215 ", "1") )
  CHECK_BAD ( "TM-08", DcmTime::checkStringValue(" 1215", "1") )
  CHECK_BAD ( "TM-09", DcmTime::checkStringValue("12 15", "1") )
  CHECK_BAD ( "TM-10", DcmTime::checkStringValue("235959.987654    ", "1") )
  CHECK_GOOD( "TM-11", DcmTime::checkStringValue("0000   \\010101  \\020202.02 \\030303.0303", "4") )
  CHECK_BAD ( "TM-12", DcmTime::checkStringValue("12:30:00.123456", "1", OFFalse) )
  CHECK_GOOD( "TM-13", DcmTime::checkStringValue("12:30:00.123456", "1", OFTrue) )
  CHECK_BAD ( "TM-14", DcmTime::checkStringValue("12:30", "1", OFTrue) )
  CHECK_GOOD( "TM-15", DcmTime::checkStringValue("12 ", "1") )
  CHECK_BAD ( "TM-16", DcmTime::checkStringValue(" ", "1") )

  /* test "Unlimited Characters" */
  CHECK_GOOD( "UC-01", DcmUnlimitedCharacters::checkStringValue("ABC", "1") )
  CHECK_GOOD( "UC-02", DcmUnlimitedCharacters::checkStringValue("ABC\\123", "2") )
  CHECK_GOOD( "UC-03", DcmUnlimitedCharacters::checkStringValue(" J\366rg Riesmeier ", "1", "ISO_IR 100") )
  CHECK_GOOD( "UC-04", DcmUnlimitedCharacters::checkStringValue("ESC\033aping", "1") )
  CHECK_BAD ( "UC-05", DcmUnlimitedCharacters::checkStringValue("not allowed: \n\010\r\014", "1") )
  CHECK_GOOD( "UC-06", DcmUnlimitedCharacters::checkStringValue(" ", "1") )
  CHECK_GOOD( "UC-07", DcmUnlimitedCharacters::checkStringValue("A\\B", "2") )

  /* test "Unique Identifier" */
  CHECK_GOOD( "UI-01", DcmUniqueIdentifier::checkStringValue("0", "1") )
  CHECK_GOOD( "UI-02", DcmUniqueIdentifier::checkStringValue("0.1", "1") )
  CHECK_GOOD( "UI-03", DcmUniqueIdentifier::checkStringValue("1.2.3.4.5.6.7.8.9.0.123.4321", "1") )
  CHECK_BAD ( "UI-04", DcmUniqueIdentifier::checkStringValue("1.02", "1") )
  CHECK_BAD ( "UI-05", DcmUniqueIdentifier::checkStringValue(".2", "1") )
  CHECK_BAD ( "UI-06", DcmUniqueIdentifier::checkStringValue("1.", "1") )
  CHECK_BAD ( "UI-07", DcmUniqueIdentifier::checkStringValue("1.2.3.4.5.6.7.8.9.0 ", "1") )
  CHECK_BAD ( "UI-08", DcmUniqueIdentifier::checkStringValue(" 1.2.3.4.5.6.7.8.9.0", "1") )
  CHECK_BAD ( "UI-09", DcmUniqueIdentifier::checkStringValue("1.2.3.4.5. 6.7.8.9.0", "1") )
  CHECK_GOOD( "UI-10", DcmUniqueIdentifier::checkStringValue("1.2.3.4.5.6.7.8.9.0\\99", "2") )
  CHECK_GOOD( "UI-11", DcmUniqueIdentifier::checkStringValue("1.2.3.4.5.6.7.8.9.0.123.456.789.0.111222333444555666777888999000", "1") )
  CHECK_BAD ( "UI-12", DcmUniqueIdentifier::checkStringValue("1.2.3.4.5.6.7.8.9.0.123.456.789.10.111222333444555666777888999000", "1") )
  CHECK_BAD ( "UI-13", DcmUniqueIdentifier::checkStringValue("007", "1") )

  /* test "Universal Resource Identifier or Universal Resource Locator (URI/URL)" */
  CHECK_GOOD( "UR-01", DcmUniversalResourceIdentifierOrLocator::checkStringValue("http://www.dcmtk.org/ ") )
  CHECK_BAD ( "UR-02", DcmUniversalResourceIdentifierOrLocator::checkStringValue(" http://www.dcmtk.org/") )
  CHECK_BAD ( "UR-03", DcmUniversalResourceIdentifierOrLocator::checkStringValue("http:\\\\www.dcmtk.org") )
  CHECK_BAD ( "UR-04", DcmUniversalResourceIdentifierOrLocator::checkStringValue("http://localhost/some space") )
  CHECK_GOOD( "UR-05", DcmUniversalResourceIdentifierOrLocator::checkStringValue("http://localhost/some%20space") )
  CHECK_GOOD( "UR-06", DcmUniversalResourceIdentifierOrLocator::checkStringValue("foo://example.com:8042/over/there?name=ferret#nose") )
  CHECK_GOOD( "UR-07", DcmUniversalResourceIdentifierOrLocator::checkStringValue("urn:example:animal:ferret:nose") )
  CHECK_GOOD( "UR-08", DcmUniversalResourceIdentifierOrLocator::checkStringValue("http://joe:passwd@www.example.net:8080/index.html?action=something&session=A54C6FE2#info") )

  /* test "Unlimited Text" */
  OFString hugeString(1024 * 512, 'n');

  CHECK_GOOD( "UT-01", DcmUnlimitedText::checkStringValue(" This text can be pretty long ... ") )
  CHECK_GOOD( "UT-02", DcmUnlimitedText::checkStringValue(hugeString) )

  hugeString[hugeString.length() / 2] = '\t';
  CHECK_GOOD( "UT-03", DcmUnlimitedText::checkStringValue(hugeString) )

  hugeString[hugeString.length() - 1] = '\v';
  CHECK_BAD ( "UT-04", DcmUnlimitedText::checkStringValue(hugeString) )
}
