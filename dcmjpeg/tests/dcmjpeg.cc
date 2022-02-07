/*
 *
 *  Copyright (C) 2002-2014, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class OFStandard
 *
 */

#include "../../ofstd/include/dcmtk/ofstd/oftest.h"
#include "../../config/include/dcmtk/config/osconfig.h" 

OFTEST(dcmjpeg_test)
{
   int rc = chdir("dcmjpeg/apps");
   rc = system("./test.sh"); 
   OFCHECK_EQUAL(rc, 0);
}
