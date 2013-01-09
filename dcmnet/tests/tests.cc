/*
 *
 *  Copyright (C) 2012-2013, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmnet_dimseDump_nullByte);

#ifdef WITH_THREADS
OFTEST_REGISTER(dcmnet_scp_pool);
#endif // WITH_THREADS

OFTEST_MAIN("dcmnet")
