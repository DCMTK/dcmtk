/*
 *
 *  Copyright (C) 2019-2024, OFFIS e.V.
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
 *  Module:  dcmect
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Main test program for dcmect
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmect_huge_concat);
OFTEST_REGISTER(dcmect_overflow);
OFTEST_REGISTER(dcmect_roundtrip);
OFTEST_MAIN("dcmect")
