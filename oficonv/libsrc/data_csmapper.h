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

#ifndef DATA_CSMAPPER_H
#define DATA_CSMAPPER_H

#ifdef DCMTK_ENABLE_BUILTIN_OFICONV_DATA

extern unsigned char csmapper_charset_pivot_pvdb[];
extern unsigned int csmapper_charset_pivot_pvdb_len;
extern unsigned char csmapper_CP_CP936EXT_UCS_mps[];
extern unsigned int csmapper_CP_CP936EXT_UCS_mps_len;
extern unsigned char csmapper_CP_UCS_CP936EXT_mps[];
extern unsigned int csmapper_CP_UCS_CP936EXT_mps_len;
extern unsigned char csmapper_GB_GB18030_UCS_BMP_mps[];
extern unsigned int csmapper_GB_GB18030_UCS_BMP_mps_len;
extern unsigned char csmapper_GB_GB2312EXT_UCS_mps[];
extern unsigned int csmapper_GB_GB2312EXT_UCS_mps_len;
extern unsigned char csmapper_GB_GB2312GBK_UCS_mps[];
extern unsigned int csmapper_GB_GB2312GBK_UCS_mps_len;
extern unsigned char csmapper_GB_GB2312_UCS_mps[];
extern unsigned int csmapper_GB_GB2312_UCS_mps_len;
extern unsigned char csmapper_GB_GB2312UDA1_UCS_mps[];
extern unsigned int csmapper_GB_GB2312UDA1_UCS_mps_len;
extern unsigned char csmapper_GB_GB2312UDA2_UCS_mps[];
extern unsigned int csmapper_GB_GB2312UDA2_UCS_mps_len;
extern unsigned char csmapper_GB_GBKEXT_UCS_mps[];
extern unsigned int csmapper_GB_GBKEXT_UCS_mps_len;
extern unsigned char csmapper_GB_GBKUDA_UCS_mps[];
extern unsigned int csmapper_GB_GBKUDA_UCS_mps_len;
extern unsigned char csmapper_GB_UCS_BMP_GB18030_mps[];
extern unsigned int csmapper_GB_UCS_BMP_GB18030_mps_len;
extern unsigned char csmapper_GB_UCS_GB2312EXT_mps[];
extern unsigned int csmapper_GB_UCS_GB2312EXT_mps_len;
extern unsigned char csmapper_GB_UCS_GB2312_mps[];
extern unsigned int csmapper_GB_UCS_GB2312_mps_len;
extern unsigned char csmapper_GB_UCS_GB2312UDA1_mps[];
extern unsigned int csmapper_GB_UCS_GB2312UDA1_mps_len;
extern unsigned char csmapper_GB_UCS_GB2312UDA2_mps[];
extern unsigned int csmapper_GB_UCS_GB2312UDA2_mps_len;
extern unsigned char csmapper_GB_UCS_GBKEXT_mps[];
extern unsigned int csmapper_GB_UCS_GBKEXT_mps_len;
extern unsigned char csmapper_GB_UCS_GBKUDA_mps[];
extern unsigned int csmapper_GB_UCS_GBKUDA_mps_len;
extern unsigned char csmapper_ISO646_ISO646_JP_UCS_646[];
extern unsigned int csmapper_ISO646_ISO646_JP_UCS_646_len;
extern unsigned char csmapper_ISO646_ISO646_US_UCS_646[];
extern unsigned int csmapper_ISO646_ISO646_US_UCS_646_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_11_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_11_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_15_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_15_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_2_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_2_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_3_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_3_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_4_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_4_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_5_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_5_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_6_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_6_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_7_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_7_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_8_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_8_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_ISO_8859_9_UCS_mps[];
extern unsigned int csmapper_ISO_8859_ISO_8859_9_UCS_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_11_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_11_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_15_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_15_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_2_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_2_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_3_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_3_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_4_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_4_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_5_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_5_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_6_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_6_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_7_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_7_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_8_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_8_mps_len;
extern unsigned char csmapper_ISO_8859_UCS_ISO_8859_9_mps[];
extern unsigned int csmapper_ISO_8859_UCS_ISO_8859_9_mps_len;
extern unsigned char csmapper_JIS_JISX0201_KANA_UCS_mps[];
extern unsigned int csmapper_JIS_JISX0201_KANA_UCS_mps_len;
extern unsigned char csmapper_JIS_JISX0208_1990_UCS_mps[];
extern unsigned int csmapper_JIS_JISX0208_1990_UCS_mps_len;
extern unsigned char csmapper_JIS_JISX0212_UCS_mps[];
extern unsigned int csmapper_JIS_JISX0212_UCS_mps_len;
extern unsigned char csmapper_JIS_UCS_JISX0201_KANA_mps[];
extern unsigned int csmapper_JIS_UCS_JISX0201_KANA_mps_len;
extern unsigned char csmapper_JIS_UCS_JISX0208_1990_mps[];
extern unsigned int csmapper_JIS_UCS_JISX0208_1990_mps_len;
extern unsigned char csmapper_JIS_UCS_JISX0212_mps[];
extern unsigned int csmapper_JIS_UCS_JISX0212_mps_len;
extern unsigned char csmapper_KS_KSC5601HANGUL_UCS_mps[];
extern unsigned int csmapper_KS_KSC5601HANGUL_UCS_mps_len;
extern unsigned char csmapper_KS_KSC5601HANJA_UCS_mps[];
extern unsigned int csmapper_KS_KSC5601HANJA_UCS_mps_len;
extern unsigned char csmapper_KS_UCS_KSC5601HANGUL_mps[];
extern unsigned int csmapper_KS_UCS_KSC5601HANGUL_mps_len;
extern unsigned char csmapper_KS_UCS_KSC5601HANJA_mps[];
extern unsigned int csmapper_KS_UCS_KSC5601HANJA_mps_len;
extern unsigned char csmapper_mapper_dir_db[];
extern unsigned int csmapper_mapper_dir_db_len;
extern unsigned char csmapper_mapper_dir[];
extern unsigned int csmapper_mapper_dir_len;

#endif

#endif /* DATA_CSMAPPER_H */
