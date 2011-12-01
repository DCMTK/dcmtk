/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2011-12-01 13:13:56 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrpn.h"


OFTEST(dcmdata_checkDcmPersonName)
{
  /* test getComponentGroup()*/
  OFString resultStr;
  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Eichelberg^Marco^^Dr.");
  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", 1, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Onken^Michael^");
  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", 2, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Riesmeier^Jörg^^Dr.^");

  OFCHECK( DcmPersonName::getComponentGroup("=Onken^Michael^", 1, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Onken^Michael^");
  OFCHECK( DcmPersonName::getComponentGroup("==Riesmeier^Jörg^^Dr.^", 2, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Riesmeier^Jörg^^Dr.^");

  OFCHECK( DcmPersonName::getComponentGroup("", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "");
  OFCHECK( DcmPersonName::getComponentGroup("Riesmeier^Jörg^^Dr.^==", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Riesmeier^Jörg^^Dr.^");

  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco", 1, resultStr).bad() );
  OFCHECK( DcmPersonName::getComponentGroup("Onken^Michael^=Riesmeier^Jörg^^Dr.^", 2, resultStr).bad() );
  OFCHECK( DcmPersonName::getComponentGroup("=Onken^Michael^=Riesmeier^Jörg^^Dr.^", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "");

  /* test getComponentsFromString(). So far only minimal tests to assure
   * changes to getComponentGroup does not introduce problems
   */
  OFString l, f, m, p, s;
  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", l,f,m,p,s, 0).good());
  OFCHECK_EQUAL(l, "Eichelberg");
  OFCHECK_EQUAL(f, "Marco");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "Dr.");
  OFCHECK_EQUAL(s, "");

  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", l,f,m,p,s, 1).good());
  OFCHECK_EQUAL(l, "Onken");
  OFCHECK_EQUAL(f, "Michael");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "");
  OFCHECK_EQUAL(s, "");

  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", l,f,m,p,s, 2).good());
  OFCHECK_EQUAL(l, "Riesmeier");
  OFCHECK_EQUAL(f, "Jörg");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "Dr.");
  OFCHECK_EQUAL(s, "");

  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^Jörg^^Dr.^", l,f,m,p,s, 2).good());
  OFCHECK_EQUAL(l, "Riesmeier");
  OFCHECK_EQUAL(f, "Jörg");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "Dr.");
  OFCHECK_EQUAL(s, "");
}


/*
 *
 * CVS/RCS Log:
 * $Log: tvrpn.cc,v $
 * Revision 1.1  2011-12-01 13:13:56  onken
 * Added support for Application Hosting's Native DICOM Model xml format
 * to dcm2xml.
 *
 * Revision 1.1  2011-05-25 10:05:55  uli
 * Imported oftest and converted existing tests to oftest.
 *
 * Revision 1.7  2010-10-14 13:15:05  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2010-09-02 10:26:17  uli
 * Added a test for UT VR with an invalid character.
 *
 * Revision 1.5  2010-08-26 12:18:09  uli
 * Added a test that breaks DcmUnlimitedText. This has to be fixed.
 *
 * Revision 1.4  2010-08-26 08:39:50  uli
 * Added a test that breaks DcmUnlimitedText. This has to be fixed.
 *
 * Revision 1.3  2010-04-23 14:34:45  joergr
 * Renamed static helper function checkValue() to checkStringValue().
 *
 * Revision 1.2  2010-01-07 17:49:31  joergr
 * Added tests for old time format "HH:MM:SS.FFFFFF".
 *
 * Revision 1.1  2009-08-03 09:07:59  joergr
 * Added methods that check whether a given string value conforms to the VR and
 * VM definitions of the DICOM standards.
 *
 *
 */
