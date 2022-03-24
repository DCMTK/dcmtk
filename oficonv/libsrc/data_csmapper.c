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
 *  Purpose: Binary data for compiling the csmapper files into the oficonv library
 *
 */

#include "dcmtk/config/osconfig.h"
#include "data_csmapper.h"

#ifdef DCMTK_ENABLE_BUILTIN_OFICONV_DATA

#include "csmapper_charset_pivot_pvdb.h"
#include "csmapper_cp_cp936ext_ucs_mps.h"
#include "csmapper_cp_ucs_cp936ext_mps.h"
#include "csmapper_gb_gb18030_ucs@bmp_mps.h"
#include "csmapper_gb_gb2312ext_ucs_mps.h"
#include "csmapper_gb_gb2312gbk_ucs_mps.h"
#include "csmapper_gb_gb2312_ucs_mps.h"
#include "csmapper_gb_gb2312uda1_ucs_mps.h"
#include "csmapper_gb_gb2312uda2_ucs_mps.h"
#include "csmapper_gb_gbkext_ucs_mps.h"
#include "csmapper_gb_gbkuda_ucs_mps.h"
#include "csmapper_gb_ucs@bmp_gb18030_mps.h"
#include "csmapper_gb_ucs_gb2312ext_mps.h"
#include "csmapper_gb_ucs_gb2312_mps.h"
#include "csmapper_gb_ucs_gb2312uda1_mps.h"
#include "csmapper_gb_ucs_gb2312uda2_mps.h"
#include "csmapper_gb_ucs_gbkext_mps.h"
#include "csmapper_gb_ucs_gbkuda_mps.h"
#include "csmapper_iso646_iso646_jp_ucs_646.h"
#include "csmapper_iso646_iso646_us_ucs_646.h"
#include "csmapper_iso_8859_iso_8859_11_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_15_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_2_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_3_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_4_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_5_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_6_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_7_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_8_ucs_mps.h"
#include "csmapper_iso_8859_iso_8859_9_ucs_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_11_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_15_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_2_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_3_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_4_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_5_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_6_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_7_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_8_mps.h"
#include "csmapper_iso_8859_ucs_iso_8859_9_mps.h"
#include "csmapper_jis_jisx0201_kana_ucs_mps.h"
#include "csmapper_jis_jisx0208@1990_ucs_mps.h"
#include "csmapper_jis_jisx0212_ucs_mps.h"
#include "csmapper_jis_ucs_jisx0201_kana_mps.h"
#include "csmapper_jis_ucs_jisx0208@1990_mps.h"
#include "csmapper_jis_ucs_jisx0212_mps.h"
#include "csmapper_ks_ksc5601hangul_ucs_mps.h"
#include "csmapper_ks_ksc5601hanja_ucs_mps.h"
#include "csmapper_ks_ucs_ksc5601hangul_mps.h"
#include "csmapper_ks_ucs_ksc5601hanja_mps.h"
#include "csmapper_mapper_dir_db.h"
#include "csmapper_mapper_dir.h"

#else

int oficonv_data_csmapper_c_dummy_to_keep_linker_from_moaning = 0;

#endif
