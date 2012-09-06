/*
 *
 *  Copyright (C) 2011-2012, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: test program for class DcmPersonName
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrpn.h"


OFTEST(dcmdata_personName)
{
  /* test getComponentGroup()*/
  OFString resultStr;
  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Eichelberg^Marco^^Dr.");
  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", 1, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Onken^Michael^");
  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", 2, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Riesmeier^J\366rg^^Dr.^");

  OFCHECK( DcmPersonName::getComponentGroup("=Onken^Michael^", 1, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Onken^Michael^");
  OFCHECK( DcmPersonName::getComponentGroup("==Riesmeier^J\366rg^^Dr.^", 2, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Riesmeier^J\366rg^^Dr.^");

  OFCHECK( DcmPersonName::getComponentGroup("", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "");
  OFCHECK( DcmPersonName::getComponentGroup("Riesmeier^J\366rg^^Dr.^==", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "Riesmeier^J\366rg^^Dr.^");

  OFCHECK( DcmPersonName::getComponentGroup("Eichelberg^Marco", 1, resultStr).bad() );
  OFCHECK( DcmPersonName::getComponentGroup("Onken^Michael^=Riesmeier^J\366rg^^Dr.^", 2, resultStr).bad() );
  OFCHECK( DcmPersonName::getComponentGroup("=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", 0, resultStr).good() );
  OFCHECK_EQUAL(resultStr, "");

  /* test getComponentsFromString(). So far only minimal tests to assure
   * changes to getComponentGroup does not introduce problems
   */
  OFString l, f, m, p, s;
  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", l,f,m,p,s, 0).good());
  OFCHECK_EQUAL(l, "Eichelberg");
  OFCHECK_EQUAL(f, "Marco");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "Dr.");
  OFCHECK_EQUAL(s, "");

  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", l,f,m,p,s, 1).good());
  OFCHECK_EQUAL(l, "Onken");
  OFCHECK_EQUAL(f, "Michael");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "");
  OFCHECK_EQUAL(s, "");

  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", l,f,m,p,s, 2).good());
  OFCHECK_EQUAL(l, "Riesmeier");
  OFCHECK_EQUAL(f, "J\366rg");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "Dr.");
  OFCHECK_EQUAL(s, "");

  OFCHECK( DcmPersonName::getNameComponentsFromString("Eichelberg^Marco^^Dr.=Onken^Michael^=Riesmeier^J\366rg^^Dr.^", l,f,m,p,s, 2).good());
  OFCHECK_EQUAL(l, "Riesmeier");
  OFCHECK_EQUAL(f, "J\366rg");
  OFCHECK_EQUAL(m, "");
  OFCHECK_EQUAL(p, "Dr.");
  OFCHECK_EQUAL(s, "");
}
