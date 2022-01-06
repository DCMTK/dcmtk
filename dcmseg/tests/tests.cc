/*
 *
 *  Copyright (C) 2015-2022, OFFIS e.V.
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmseg_bigdim);
OFTEST_REGISTER(dcmseg_utils);
OFTEST_REGISTER(dcmseg_roundtrip);
OFTEST_REGISTER(dcmseg_concat_binary);

OFTEST_MAIN("dcmseg")
