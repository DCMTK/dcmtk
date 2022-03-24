/*
 *
 *  Copyright (C) 2022, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Binary data for compiling the esdb files into the oficonv library
 *
 */

#include "dcmtk/config/osconfig.h"
#include "data_esdb.h"

#ifdef DCMTK_ENABLE_BUILTIN_OFICONV_DATA

#include "esdb_esdb_alias_db.h"
#include "esdb_esdb_dir_db.h"
#include "esdb_euc_euc_cn_esdb.h"
#include "esdb_euc_euc_kr_esdb.h"
#include "esdb_gb_gb18030_esdb.h"
#include "esdb_gb_gbk_esdb.h"
#include "esdb_iso646_iso646_us_esdb.h"
#include "esdb_iso_8859_iso_8859_11_esdb.h"
#include "esdb_iso_8859_iso_8859_15_esdb.h"
#include "esdb_iso_8859_iso_8859_1_esdb.h"
#include "esdb_iso_8859_iso_8859_2_esdb.h"
#include "esdb_iso_8859_iso_8859_3_esdb.h"
#include "esdb_iso_8859_iso_8859_4_esdb.h"
#include "esdb_iso_8859_iso_8859_5_esdb.h"
#include "esdb_iso_8859_iso_8859_6_esdb.h"
#include "esdb_iso_8859_iso_8859_7_esdb.h"
#include "esdb_iso_8859_iso_8859_8_esdb.h"
#include "esdb_iso_8859_iso_8859_9_esdb.h"
#include "esdb_misc_jisx0201_kana_esdb.h"
#include "esdb_misc_jisx0208@1990_esdb.h"
#include "esdb_misc_jisx0212_esdb.h"
#include "esdb_misc_shift_jis_esdb.h"
#include "esdb_utf_utf_16be_esdb.h"
#include "esdb_utf_utf_16_esdb.h"
#include "esdb_utf_utf_16_internal_esdb.h"
#include "esdb_utf_utf_16le_esdb.h"
#include "esdb_utf_utf_16_swapped_esdb.h"
#include "esdb_utf_utf_32be_esdb.h"
#include "esdb_utf_utf_32_esdb.h"
#include "esdb_utf_utf_32_internal_esdb.h"
#include "esdb_utf_utf_32le_esdb.h"
#include "esdb_utf_utf_32_swapped_esdb.h"
#include "esdb_utf_utf_8_esdb.h"

#else

int oficonv_data_esdb_c_dummy_to_keep_linker_from_moaning = 0;

#endif
