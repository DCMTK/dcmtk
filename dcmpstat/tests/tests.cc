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
 *  Module:  dcmpstat
 *
 *  Author:  Michael Onken
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmnet/dcmlayer.h"

OFTEST_REGISTER(dcmpstat_graphic_layer_rgb_cielab);
OFTEST_REGISTER(dcmpstat_graphic_layer_read_write);
OFTEST_MAIN("dcmpstat")

// This dummy function creates a dependency on libdcmnet that is required when compiling
// on NetBSD with libwrap support enabled and OpenSSL support disabled. Otherwise there
// would be a linker error complaining about unresolved symbols allow_severity and deny_severity.

DcmTransportLayer *dcmpstat_tests_dummyFunction()
{
  return new DcmTransportLayer();
}
