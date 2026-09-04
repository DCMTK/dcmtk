/*
 *
 *  Copyright (C) 2015-2026, Open Connections GmbH
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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmfg_concatenation_creator);
OFTEST_REGISTER(dcmfg_concatenation_creator_fail);
OFTEST_REGISTER(dcmfg_concatenation_loader);
OFTEST_REGISTER(dcmfg_concatenation_loader_short_pixdata);

OFTEST_REGISTER(dcmfg_ct_image_frame_type);
OFTEST_REGISTER(dcmfg_ct_acquisition_type);
OFTEST_REGISTER(dcmfg_ct_acquisition_details);
OFTEST_REGISTER(dcmfg_ct_table_dynamics);
OFTEST_REGISTER(dcmfg_ct_position);
OFTEST_REGISTER(dcmfg_derivation_image);
OFTEST_REGISTER(dcmfg_fgbase_fgunknown);
OFTEST_REGISTER(dcmfg_frame_content);
OFTEST_REGISTER(dcmfg_irradiation_event_identification);

OFTEST_REGISTER(dcmfg_fginterface_add_get);
OFTEST_REGISTER(dcmfg_fginterface_shared);
OFTEST_REGISTER(dcmfg_fginterface_delete_perframe);
OFTEST_REGISTER(dcmfg_fginterface_delete_frame);
OFTEST_REGISTER(dcmfg_fginterface_out_of_order_insert);
OFTEST_REGISTER(dcmfg_fginterface_replace_perframe);
OFTEST_REGISTER(dcmfg_fginterface_write_read_roundtrip);
OFTEST_REGISTER(dcmfg_fginterface_clear);
OFTEST_REGISTER(dcmfg_fginterface_convert_shared_to_perframe);
OFTEST_REGISTER(dcmfg_fginterface_write_1based_frames);

OFTEST_MAIN("dcmfg")
