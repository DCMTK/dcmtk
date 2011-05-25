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
 *  Module:  ofstd
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: main test program
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-05-25 10:05:57 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(ofstd_OFDateTime);
OFTEST_REGISTER(ofstd_OFList_1);
OFTEST_REGISTER(ofstd_OFList_2);
OFTEST_REGISTER(ofstd_OFList_splice);
OFTEST_REGISTER(ofstd_OFMap);
OFTEST_REGISTER(ofstd_OFStack);
OFTEST_REGISTER(ofstd_OFString_compare);
OFTEST_REGISTER(ofstd_OFString_concatenate);
OFTEST_REGISTER(ofstd_OFString_constructor);
OFTEST_REGISTER(ofstd_OFString_identity_1);
OFTEST_REGISTER(ofstd_OFString_identity_2);
OFTEST_REGISTER(ofstd_OFString_identity_3);
OFTEST_REGISTER(ofstd_OFString_reserve);
OFTEST_REGISTER(ofstd_OFString_substr);
OFTEST_REGISTER(ofstd_OFVector);
OFTEST_REGISTER(ofstd_atof);
OFTEST_REGISTER(ofstd_base64_test1);
OFTEST_REGISTER(ofstd_base64_test2);
OFTEST_REGISTER(ofstd_base64_test3);
OFTEST_REGISTER(ofstd_ftoa);
OFTEST_REGISTER(ofstd_thread);
OFTEST_REGISTER(ofstd_testPaths);
OFTEST_REGISTER(ofstd_ReadWriteable);
OFTEST_MAIN("ofstd")

/*
**
** CVS/RCS Log:
** $Log: tests.cc,v $
** Revision 1.1  2011-05-25 10:05:57  uli
** Imported oftest and converted existing tests to oftest.
**
**
**
*/
