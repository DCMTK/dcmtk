/*
 *
 *  Copyright (C) 2016-2025, OFFIS e.V.
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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmiod_component_check_invalid_chars);
OFTEST_REGISTER(dcmiod_component_check_max_length_violated);
OFTEST_REGISTER(dcmiod_component_check_missing_value);
OFTEST_REGISTER(dcmiod_component_check_vm_violated);
OFTEST_REGISTER(dcmiod_component_check_vr_violated);
OFTEST_REGISTER(dcmiod_codes);
OFTEST_REGISTER(dcmiod_content_identification_macro);
OFTEST_REGISTER(dcmiod_icc_profile_module);
OFTEST_REGISTER(dcmiod_imagepixel);
OFTEST_REGISTER(dcmiod_tcielabutil);
OFTEST_REGISTER(dcmiod_palette_color_lut_module);
OFTEST_REGISTER(dcmiod_palette_color_lut_module_segmented);
OFTEST_REGISTER(dcmiod_palette_color_lut_module_extra_checks);
OFTEST_MAIN("dcmiod")
