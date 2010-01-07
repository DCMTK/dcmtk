/*
 *
 *  Copyright (C) 2009-2010, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for checkValue() methods
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-01-07 17:49:31 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"


#define CHECK_GOOD(number, check) \
  COUT << "Test " << number << ": " << (check.good() ? "OK" : "Error") << OFendl;

#define CHECK_BAD(number, check) \
  COUT << "Test " << number << ": " << (check.bad() ? "OK" : "Error") << OFendl;


int main(int , char *[])
{
  /* test "Application Entity" */
  CHECK_BAD ( "AE-01", DcmApplicationEntity::checkValue(" ", "1") )
  CHECK_GOOD( "AE-02", DcmApplicationEntity::checkValue("A\\") )
  CHECK_GOOD( "AE-03", DcmApplicationEntity::checkValue(" AETITLE", "1") )
  CHECK_GOOD( "AE-04", DcmApplicationEntity::checkValue("AE-Title  ", "1") )
  CHECK_GOOD( "AE-05", DcmApplicationEntity::checkValue(" AE Title ", "1") )
  CHECK_GOOD( "AE-06", DcmApplicationEntity::checkValue("A \\ B", "2") )
  CHECK_BAD ( "AE-07", DcmApplicationEntity::checkValue("Jörg", "1") )
  CHECK_BAD ( "AE-08", DcmApplicationEntity::checkValue("1234567890-ABCDE ") )
  CHECK_GOOD( "AE-09", DcmApplicationEntity::checkValue("\\ AE \\", "3-3n") )
  COUT << OFendl;

  /* test "Age String" */
  CHECK_GOOD( "AS-01", DcmAgeString::checkValue("001Y", "1") )
  CHECK_GOOD( "AS-02", DcmAgeString::checkValue("099Y", "1") )
  CHECK_BAD ( "AS-03", DcmAgeString::checkValue("099Z", "1") )
  CHECK_GOOD( "AS-04", DcmAgeString::checkValue("199D\\000W\\500M", "2-n") )
  CHECK_BAD ( "AS-05", DcmAgeString::checkValue("Y010", "1") )
  CHECK_BAD ( "AS-06", DcmAgeString::checkValue("10Y", "1") )
  CHECK_BAD ( "AS-07", DcmAgeString::checkValue("010 Y", "1") )
  CHECK_BAD ( "AS-08", DcmAgeString::checkValue(" 010Y", "1") )
  CHECK_BAD ( "AS-09", DcmAgeString::checkValue("010Y ", "1") )
  CHECK_BAD ( "AS-10", DcmAgeString::checkValue("0109Y", "1") )
  COUT << OFendl;

  /* test "Code String" */
  CHECK_GOOD( "CS-01", DcmCodeString::checkValue("TEST 1234567890\\TEST", "2-2n") )
  CHECK_BAD ( "CS-02", DcmCodeString::checkValue("TEST 1234567890\\TEST\\", "2-2n") )
  CHECK_GOOD( "CS-03", DcmCodeString::checkValue("", "2-2n") )
  CHECK_GOOD( "CS-04", DcmCodeString::checkValue("\\TEST 1234567890", "2") )
  CHECK_GOOD( "CS-05", DcmCodeString::checkValue("\\ ", "2") )
  CHECK_BAD ( "CS-06", DcmCodeString::checkValue("\\TEST 1234567890 A", "2") )
  CHECK_BAD ( "CS-07", DcmCodeString::checkValue("Jörg", "1-2") )
  CHECK_BAD ( "CS-08", DcmCodeString::checkValue("Joerg", "1-2") )
  CHECK_GOOD( "CS-09", DcmCodeString::checkValue("MARCO \\ JOERG", "1-2") )
  CHECK_GOOD( "CS-10", DcmCodeString::checkValue("MARCO\\ MICHAEL \\JOERG\\") )
  CHECK_BAD ( "CS-11", DcmCodeString::checkValue("    1234567890    ") )
  CHECK_BAD ( "CS-12", DcmCodeString::checkValue("1234567890-ABCDE") )
  CHECK_GOOD( "CS-13", DcmCodeString::checkValue("                ") )
  CHECK_BAD ( "CS-14", DcmCodeString::checkValue("                 ") )
  COUT << OFendl;

  /* test "Date" */
  CHECK_GOOD( "DA-01", DcmDate::checkValue("20090731", "1") )
  CHECK_GOOD( "DA-02", DcmDate::checkValue("18200101", "1") )
  CHECK_GOOD( "DA-03", DcmDate::checkValue("21001231", "1") )
  CHECK_BAD ( "DA-04", DcmDate::checkValue("1820.01.01", "1", OFFalse) )
  CHECK_GOOD( "DA-05", DcmDate::checkValue("1820.01.01", "1", OFTrue) )
  CHECK_GOOD( "DA-06", DcmDate::checkValue("20090731\\18200101") )
  CHECK_GOOD( "DA-07", DcmDate::checkValue("20090731\\18200101", "2-2n") )
  CHECK_GOOD( "DA-08", DcmDate::checkValue("20090731\\18200101\\20000101\\21001231", "2-2n") )
  CHECK_BAD ( "DA-09", DcmDate::checkValue("20090731", "6") )
  CHECK_BAD ( "DA-10", DcmDate::checkValue("20000001", "1") )
  CHECK_BAD ( "DA-11", DcmDate::checkValue("00001232", "1") )
  CHECK_BAD ( "DA-12", DcmDate::checkValue("20000000", "1") )
  COUT << OFendl;

  /* test "Date Time" */
  CHECK_GOOD( "DT-01", DcmDateTime::checkValue("200907311230", "1") )
  CHECK_GOOD( "DT-02", DcmDateTime::checkValue("18200101000000", "1") )
  CHECK_GOOD( "DT-03", DcmDateTime::checkValue("21001231235959.000001", "1") )
  CHECK_BAD ( "DT-04", DcmDateTime::checkValue("20090731", "1") )
  CHECK_BAD ( "DT-05", DcmDateTime::checkValue("200907312400", "1") )
  CHECK_BAD ( "DT-06", DcmDateTime::checkValue("20090731 1200") )
  CHECK_GOOD( "DT-07", DcmDateTime::checkValue("2009073100\\1820010112", "2-2n") )
  CHECK_GOOD( "DT-08", DcmDateTime::checkValue("2009073100\\1820010112\\200001010030\\21001231171540.123", "2-2n") )
  CHECK_GOOD( "DT-09", DcmDateTime::checkValue("200907311230+0100", "1") )
  CHECK_GOOD( "DT-10", DcmDateTime::checkValue("20090731123000+1000", "1") )
  CHECK_BAD ( "DT-11", DcmDateTime::checkValue("200907311230+1900", "1") )
  CHECK_GOOD( "DT-12", DcmDateTime::checkValue("20090731123000.123456+0100", "1") )
  CHECK_BAD ( "DT-13", DcmDateTime::checkValue("20090731123000.123456+0100 ", "1") )
  COUT << OFendl;

  /* test "Decimal String" */
  CHECK_GOOD( "DS-01", DcmDecimalString::checkValue(" 0", "1") )
  CHECK_GOOD( "DS-02", DcmDecimalString::checkValue("0.0 ", "1") )
  CHECK_GOOD( "DS-03", DcmDecimalString::checkValue("-1", "1") )
  CHECK_GOOD( "DS-04", DcmDecimalString::checkValue("+12345.67890", "1") )
  CHECK_BAD ( "DS-05", DcmDecimalString::checkValue("   -12345.6789   ", "1") )
  CHECK_GOOD( "DS-06", DcmDecimalString::checkValue("01.01", "1") )
  CHECK_BAD ( "DS-06", DcmDecimalString::checkValue("0.1.01", "1") )
  CHECK_BAD ( "DS-07", DcmDecimalString::checkValue("0 1", "1") )
  CHECK_BAD ( "DS-08", DcmDecimalString::checkValue("0-1", "1") )
  CHECK_GOOD( "DS-09", DcmDecimalString::checkValue("1.0E99", "1") )
  CHECK_BAD ( "DS-10", DcmDecimalString::checkValue("1.099E", "1") )
  CHECK_GOOD( "DS-11", DcmDecimalString::checkValue("-10e+99", "1") )
  CHECK_GOOD( "DS-12", DcmDecimalString::checkValue("-10e+99\\+1E-1", "2") )
  COUT << OFendl;

  /* test "Integer String" */
  CHECK_GOOD( "IS-01", DcmIntegerString::checkValue("0", "1") )
  CHECK_GOOD( "IS-02", DcmIntegerString::checkValue("+0", "1") )
  CHECK_GOOD( "IS-03", DcmIntegerString::checkValue("-0", "1") )
  CHECK_GOOD( "IS-04", DcmIntegerString::checkValue("00", "1") )
  CHECK_GOOD( "IS-05", DcmIntegerString::checkValue("000000000001", "1") )
  CHECK_BAD ( "IS-06", DcmIntegerString::checkValue("1000000000001", "1") )
  CHECK_BAD ( "IS-07", DcmIntegerString::checkValue("-000000000001", "1") )
  CHECK_BAD ( "IS-08", DcmIntegerString::checkValue("3000000000", "1") )
  CHECK_GOOD( "IS-08", DcmIntegerString::checkValue("+2000000000", "1") )
  CHECK_GOOD( "IS-09", DcmIntegerString::checkValue("+2147483647", "1") )
  CHECK_BAD ( "IS-10", DcmIntegerString::checkValue("2147483648", "1") )
  CHECK_GOOD( "IS-11", DcmIntegerString::checkValue("-2147483648", "1") )
  CHECK_BAD ( "IS-12", DcmIntegerString::checkValue("-2147483649", "1") )
  CHECK_GOOD( "IS-13", DcmIntegerString::checkValue(" -2147483648\\+2147483647 ", "2") )
  CHECK_BAD ( "IS-14", DcmIntegerString::checkValue(" -2147483648 \\+2147483647", "2") )
  COUT << OFendl;

  /* test "Long String" */
  CHECK_GOOD( "LO-01", DcmLongString::checkValue(" ", "1") )
  CHECK_GOOD( "LO-02", DcmLongString::checkValue(" . ", "1") )
  CHECK_GOOD( "LO-03", DcmLongString::checkValue("ABC 123 abc _+-/", "1") )
  CHECK_GOOD( "LO-04", DcmLongString::checkValue("  ", "1") )
  CHECK_GOOD( "LO-05", DcmLongString::checkValue("ABC\\123", "2") )
  CHECK_GOOD( "LO-06", DcmLongString::checkValue(" Jörg Riesmeier ", "1") )
//  CHECK_BAD ( "LO-07", DcmLongString::checkValueu("OFFIS e.V., Escherweg 2, 26121 Oldenburg, Germany, http://www.offis.de/", "1") )
  CHECK_GOOD( "LO-08", DcmLongString::checkValue("\\ _2_ \\ _3_ \\ _4_ \\ _5_ \\", "6") )
  CHECK_GOOD( "LO-09", DcmLongString::checkValue("ESC\033aping", "1") )
  CHECK_BAD ( "LO-10", DcmLongString::checkValue("not allowed: \r\014", "1") )
  COUT << OFendl;

  /* test "Long Text" */
  CHECK_GOOD( "LT-01", DcmLongText::checkValue(" Hallo \\ 12345 \\ äöüß ") )
  COUT << OFendl;

  /* test "Person Name" */
  CHECK_GOOD( "PN-01", DcmPersonName::checkValue("A^Riesmeier^^=R^Jörg", "1") )
  CHECK_GOOD( "PN-02", DcmPersonName::checkValue("A^Riesmeier^^=R^Jörg\\", "1-n") )
  CHECK_BAD ( "PN-03", DcmPersonName::checkValue("A^Riesmeier^^=R^Jörg\\", "1") )
  CHECK_GOOD( "PN-03", DcmPersonName::checkValue("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", "1") )
  CHECK_GOOD( "PN-05", DcmPersonName::checkValue("Eichelberg\\Onken\\Riesmeier", "3", OFTrue) )
  CHECK_BAD ( "PN-06", DcmPersonName::checkValue(" ") )
  CHECK_GOOD( "PN-07", DcmPersonName::checkValue(" A") )
  CHECK_GOOD( "PN-08", DcmPersonName::checkValue("^^^^") )
  CHECK_GOOD( "PN-09", DcmPersonName::checkValue("^ Jörg ^") )
  CHECK_GOOD( "PN-10", DcmPersonName::checkValue("^^^^MD ") )
  CHECK_BAD ( "PN-11", DcmPersonName::checkValue("^^^^^") )
  COUT << OFendl;

  /* test "Short String" */
  CHECK_GOOD( "SH-01", DcmShortString::checkValue(" ", "1") )
  CHECK_GOOD( "SH-02", DcmShortString::checkValue(" . ", "1") )
  CHECK_GOOD( "SH-03", DcmShortString::checkValue("ABC 123 abc _+-/", "1") )
  CHECK_GOOD( "SH-04", DcmShortString::checkValue("  ", "1") )
  CHECK_GOOD( "SH-05", DcmShortString::checkValue("ABC\\123", "2") )
  CHECK_GOOD( "SH-06", DcmShortString::checkValue(" Jörg Riesmeier ", "1") )
//  CHECK_BAD ( "SH-07", DcmShortString::checkValue("OFFIS e.V., Oldenburg", "1") )
  CHECK_GOOD( "SH-08", DcmShortString::checkValue("\\ _2_ \\ _3_ \\ _4_ \\ _5_ \\", "6") )
  CHECK_BAD ( "SH-09", DcmShortString::checkValue(" ", "2") )
  CHECK_GOOD( "SH-10", DcmShortString::checkValue("", "2") )
  CHECK_GOOD( "SH-11", DcmShortString::checkValue("ESC\033aping", "1") )
  CHECK_BAD ( "SH-12", DcmShortString::checkValue("not allowed: \n\010\r\014", "1") )
  COUT << OFendl;

  /* test "Short Text" */
  CHECK_GOOD( "ST-01", DcmShortText::checkValue(" umlaut characters are allowed: ÄÖÜäöü\naccented characters also: áàéèíìâêô\rand control characters, of course, including \033=ESC ") )
  CHECK_BAD ( "ST-02", DcmShortText::checkValue(" other control characters are not allowed: \013 \010 \200 ") )
  COUT << OFendl;

  /* test "Time" */
  CHECK_GOOD( "TM-01", DcmTime::checkValue("0000", "1") )
  CHECK_BAD ( "TM-02", DcmTime::checkValue("2400", "1") )
  CHECK_GOOD( "TM-03", DcmTime::checkValue("123045", "1") )
  CHECK_GOOD( "TM-04", DcmTime::checkValue("235959.123", "1") )
  CHECK_GOOD( "TM-05", DcmTime::checkValue("235959.987654", "1") )
  CHECK_BAD ( "TM-06", DcmTime::checkValue("000000.0000000", "1") )
  CHECK_GOOD( "TM-07", DcmTime::checkValue("1215 ", "1") )
  CHECK_BAD ( "TM-08", DcmTime::checkValue(" 1215", "1") )
  CHECK_BAD ( "TM-09", DcmTime::checkValue("12 15", "1") )
  CHECK_BAD ( "TM-10", DcmTime::checkValue("235959.987654    ", "1") )
  CHECK_GOOD( "TM-11", DcmTime::checkValue("0000   \\010101  \\020202.02 \\030303.0303", "4") )
  CHECK_BAD ( "TM-12", DcmTime::checkValue("12:30:00.123456", "1", OFFalse) )
  CHECK_GOOD( "TM-13", DcmTime::checkValue("12:30:00.123456", "1", OFTrue) )
  CHECK_BAD ( "TM-14", DcmTime::checkValue("12:30", "1", OFTrue) )
  COUT << OFendl;

  /* test "Unique Identifier" */
  CHECK_GOOD( "UI-01", DcmUniqueIdentifier::checkValue("0", "1") )
  CHECK_GOOD( "UI-02", DcmUniqueIdentifier::checkValue("0.1", "1") )
  CHECK_GOOD( "UI-03", DcmUniqueIdentifier::checkValue("1.2.3.4.5.6.7.8.9.0.123.4321", "1") )
  CHECK_BAD ( "UI-04", DcmUniqueIdentifier::checkValue("1.02", "1") )
  CHECK_BAD ( "UI-05", DcmUniqueIdentifier::checkValue(".2", "1") )
  CHECK_BAD ( "UI-06", DcmUniqueIdentifier::checkValue("1.", "1") )
  CHECK_BAD ( "UI-07", DcmUniqueIdentifier::checkValue("1.2.3.4.5.6.7.8.9.0 ", "1") )
  CHECK_BAD ( "UI-08", DcmUniqueIdentifier::checkValue(" 1.2.3.4.5.6.7.8.9.0", "1") )
  CHECK_BAD ( "UI-09", DcmUniqueIdentifier::checkValue("1.2.3.4.5. 6.7.8.9.0", "1") )
  CHECK_GOOD( "UI-10", DcmUniqueIdentifier::checkValue("1.2.3.4.5.6.7.8.9.0\\99", "2") )
  CHECK_GOOD( "UI-11", DcmUniqueIdentifier::checkValue("1.2.3.4.5.6.7.8.9.0.123.456.789.0.111222333444555666777888999000", "1") )
  CHECK_BAD ( "UI-12", DcmUniqueIdentifier::checkValue("1.2.3.4.5.6.7.8.9.0.123.456.789.10.111222333444555666777888999000", "1") )
  COUT << OFendl;

  /* test "Unlimited Text" */
  CHECK_GOOD( "UT-01", DcmUnlimitedText::checkValue(" This text can be pretty long ... ") )
  COUT << OFendl;

  return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tstchval.cc,v $
 * Revision 1.2  2010-01-07 17:49:31  joergr
 * Added tests for old time format "HH:MM:SS.FFFFFF".
 *
 * Revision 1.1  2009-08-03 09:07:59  joergr
 * Added methods that check whether a given string value conforms to the VR and
 * VM definitions of the DICOM standards.
 *
 *
 */
