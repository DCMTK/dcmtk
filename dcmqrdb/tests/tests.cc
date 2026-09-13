/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Michael Onken
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmqrdb_config_writableStorageArea);
OFTEST_REGISTER(dcmqrdb_config_readableStorageArea);
OFTEST_REGISTER(dcmqrdb_oversizedQueryRetrieveLevel);
OFTEST_REGISTER(dcmqrdb_lowercaseQueryRetrieveLevel);
OFTEST_REGISTER(dcmqrdb_commandAbstractSyntaxMatch);
OFTEST_REGISTER(dcmqrdb_commandAbstractSyntaxMismatch);
OFTEST_REGISTER(dcmqrdb_quotaDeletesOnlySelectedStudy);

OFTEST_MAIN("dcmqrdb")
