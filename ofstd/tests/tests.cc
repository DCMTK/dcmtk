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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-11-29 16:06:27 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(ofstd_OFDate);
OFTEST_REGISTER(ofstd_OFTime);
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
OFTEST_REGISTER(ofstd_base64_1);
OFTEST_REGISTER(ofstd_base64_2);
OFTEST_REGISTER(ofstd_base64_3);
OFTEST_REGISTER(ofstd_ftoa);
OFTEST_REGISTER(ofstd_thread);
OFTEST_REGISTER(ofstd_testPaths);
OFTEST_REGISTER(ofstd_OFStandard_isReadWriteable);
OFTEST_REGISTER(ofstd_markup_1);
OFTEST_REGISTER(ofstd_markup_2);
OFTEST_REGISTER(ofstd_markup_3);
OFTEST_REGISTER(ofstd_markup_4);
OFTEST_REGISTER(ofstd_markup_5);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_1);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_2);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_3);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_4);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_5);
OFTEST_REGISTER(ofstd_OFCharacterEncoding_6);
OFTEST_REGISTER(ofstd_xmlParser);
OFTEST_MAIN("ofstd")

/*
 *
 * CVS/RCS Log:
 * $Log: tests.cc,v $
 * Revision 1.7  2011-11-29 16:06:27  joergr
 * Added new tests and also separated tests for OFDate, OFTime and OFDateTime.
 *
 * Revision 1.6  2011-11-24 09:53:43  joergr
 * Added first version of a simple non-validating XML parser written by Frank
 * Vanden Berghen. This parser is intended to be used for configuration files
 * and the like. Therefore, it is integrated into the DCMTK (unlike libxml2).
 *
 * Revision 1.5  2011-10-24 15:07:36  joergr
 * Added static method counting the characters in a given UTF-8 string.
 *
 * Revision 1.4  2011-10-21 09:15:03  joergr
 * Added class for managing and converting between different character encodings
 * based on the libiconv toolkit.
 *
 * Revision 1.3  2011-10-12 12:03:04  joergr
 * Added tests for convertToMarkupString() method.
 *
 * Revision 1.2  2011-07-06 13:04:18  uli
 * Fixed some inconsistencies in test names.
 *
 * Revision 1.1  2011-05-25 10:05:57  uli
 * Imported oftest and converted existing tests to oftest.
 *
 *
 */
