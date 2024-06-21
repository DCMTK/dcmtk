/*
 *
 *  Copyright (C) 2023, OFFIS e.V.
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
 *  Purpose: test program for class DcmTag and DcmTagKey
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dctag.h"


OFTEST(dcmdata_compareTag)
{
  const DcmTag tag1(0x0011, 0x1000, "Private Creator");
  const DcmTag tag2(0x0011, 0x1000, "Other Private Creator");
  const DcmTag tag3(0x0011, 0x1000);
  const DcmTag tag4(0x0011, 0x1000);
  const DcmTag tag5(0x0011, 0x1001, "Private Creator");
  /* compare the tags */
  OFCHECK(tag1 == tag1);
  OFCHECK(tag1 != tag2);
  OFCHECK(!(tag1 == tag2));
  OFCHECK(tag1 != tag3);
  OFCHECK(!(tag1 == tag3));
  OFCHECK(tag2 != tag3);
  OFCHECK(!(tag2 == tag3));
  OFCHECK(tag3 == tag4);
  OFCHECK(tag1 != tag5);
  OFCHECK(!(tag1 == tag5));
}


OFTEST(dcmdata_compareTagKey)
{
  const DcmTag tag1(0x0011, 0x1000, "Private Creator");
  const DcmTag tag2(0x0011, 0x1000, "Other Private Creator");
  const DcmTagKey tag3(0x0011, 0x1000);
  const DcmTagKey tag4(0x0011, 0x1000);
  const DcmTagKey tag5(0x0011, 0x1001);
  /* compare the tags */
  OFCHECK(tag1 == tag2.getTagKey());
  OFCHECK(tag1 == tag3);
  OFCHECK(tag2 == tag3);
  OFCHECK(tag3 == tag4);
  OFCHECK(tag1 != tag5);
  OFCHECK(!(tag1 == tag5));
  OFCHECK(tag2 != tag5);
  OFCHECK(!(tag2 == tag5));
  OFCHECK(tag4 != tag5);
  OFCHECK(!(tag4 == tag5));
}
