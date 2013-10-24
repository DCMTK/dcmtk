/*
 *
 *  Copyright (C) 2011-2013, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(ofstd_OFCharacterEncoding_1);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_2);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_3);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_4);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_5);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_6);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_7);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_8);
OFTEST_REGISTER(ofstd_OFDate);
OFTEST_REGISTER(ofstd_OFDateTime);
OFTEST_REGISTER(ofstd_OFList_1);
OFTEST_REGISTER(ofstd_OFList_2);
OFTEST_REGISTER(ofstd_OFList_splice);
OFTEST_REGISTER(ofstd_OFMap);
OFTEST_REGISTER(ofstd_OFStack);
OFTEST_REGISTER(ofstd_OFStandard_isReadWriteable);
OFTEST_REGISTER(ofstd_OFFile);
OFTEST_REGISTER(ofstd_OFString_compare);
OFTEST_REGISTER(ofstd_OFString_concatenate);
OFTEST_REGISTER(ofstd_OFString_constructor);
OFTEST_REGISTER(ofstd_OFString_copy);
OFTEST_REGISTER(ofstd_OFString_identity_1);
OFTEST_REGISTER(ofstd_OFString_identity_2);
OFTEST_REGISTER(ofstd_OFString_identity_3);
OFTEST_REGISTER(ofstd_OFString_reserve);
OFTEST_REGISTER(ofstd_OFString_substr);
OFTEST_REGISTER(ofstd_OFTime);
OFTEST_REGISTER(ofstd_OFUUID_1);
OFTEST_REGISTER(ofstd_OFUUID_2);
OFTEST_REGISTER(ofstd_OFVector);
OFTEST_REGISTER(ofstd_atof);
OFTEST_REGISTER(ofstd_base64_1);
OFTEST_REGISTER(ofstd_base64_2);
OFTEST_REGISTER(ofstd_base64_3);
OFTEST_REGISTER(ofstd_ftoa);
OFTEST_REGISTER(ofstd_markup_1);
OFTEST_REGISTER(ofstd_markup_2);
OFTEST_REGISTER(ofstd_markup_3);
OFTEST_REGISTER(ofstd_markup_4);
OFTEST_REGISTER(ofstd_markup_5);
OFTEST_REGISTER(ofstd_markup_6);
OFTEST_REGISTER(ofstd_testPaths);
#ifdef WITH_THREADS
OFTEST_REGISTER(ofstd_thread);
#endif // WITH_THREADS
OFTEST_REGISTER(ofstd_xmlParser);
OFTEST_REGISTER(ofstd_memory);
OFTEST_MAIN("ofstd")
