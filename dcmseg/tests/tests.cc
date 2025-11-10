/*
 *
 *  Copyright (C) 2015-2025, OFFIS e.V.
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
OFTEST_REGISTER(dcmseg_debugByte2Bin);
OFTEST_REGISTER(dcmseg_packBinaryFrame);
OFTEST_REGISTER(dcmseg_packAndUnpackBinaryFrame);
OFTEST_REGISTER(dcmseg_concatBinaryFrames);
OFTEST_REGISTER(dcmseg_roundtrip);
OFTEST_REGISTER(dcmseg_concat_binary);
OFTEST_REGISTER(dcmseg_labelmap_8bit_mono2);
OFTEST_REGISTER(dcmseg_labelmap_16bit_mono2);
OFTEST_REGISTER(dcmseg_labelmap_8bit_palette);
OFTEST_REGISTER(dcmseg_labelmap_16bit_palette);
OFTEST_MAIN("dcmseg")
