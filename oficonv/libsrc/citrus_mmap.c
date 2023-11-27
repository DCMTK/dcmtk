/*-
 * Copyright (c)2003 Citrus Project,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "dcmtk/config/osconfig.h"
#include "citrus_mmap.h"
#include "oficonv_logger.h"

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <errno.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>  /* for O_RDONLY, O_CLOEXEC */
#endif
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "citrus_bcs.h"
#include "citrus_region.h"
#include "windows_mmap.h"
#include "data_esdb.h"
#include "data_csmapper.h"

#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif


void *lookup_builtin_data_file(const char *path, size_t *len)
{
  if (len == NULL || path == NULL) return NULL;

#ifdef DCMTK_ENABLE_BUILTIN_OFICONV_DATA
    const char *default_path;
    const char *current_path;
    size_t default_path_len;
#ifdef HAVE_WINDOWS_H
    char buf[OFICONV_PATH_MAX+1];
#endif

    /* check if we are trying to load from the default location */
    default_path = DEFAULT_SUPPORT_DATA_DIR;
    current_path = path;
#ifdef HAVE_WINDOWS_H
    memset(buf, 0, sizeof(buf));
    (void) ExpandEnvironmentStringsA(default_path, buf, sizeof(buf));
    default_path = buf;
#endif
    default_path_len = strlen(default_path);

    if (0 == strncmp(current_path, default_path, default_path_len))
    {
      /* advance to the relative path after the default location */
      current_path += default_path_len;

      /* skip a path separator following the default location, if any */
      if ((current_path[0] == PATH_SEPARATOR) || (current_path[0] == '/')) current_path++;

      /* check if the filename starts with csmapper */
#ifdef HAVE_WINDOWS_H
      if (0 == strncmp(current_path, "csmapper\\", 9) || 0 == strncmp(current_path, "csmapper/", 9))
#else
      if (0 == strncmp(current_path, "csmapper/", 9))
#endif
      {
        current_path += 9;

        if (0 == strcmp(current_path, "charset.pivot.pvdb")) { *len = csmapper_charset_pivot_pvdb_len; return csmapper_charset_pivot_pvdb; }
        if (0 == strcmp(current_path, "mapper.dir")) { *len = csmapper_mapper_dir_len; return csmapper_mapper_dir; }
        if (0 == strcmp(current_path, "mapper.dir.db")) { *len = csmapper_mapper_dir_db_len; return csmapper_mapper_dir_db; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-11%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_11_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_11_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-15%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_15_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_15_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-2%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_2_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_2_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-3%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_3_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_3_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-4%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_4_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_4_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-5%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_5_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_5_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-6%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_6_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_6_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-7%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_7_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_7_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-8%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_8_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_8_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-9%UCS.mps")) { *len = csmapper_ISO_8859_ISO_8859_9_UCS_mps_len; return csmapper_ISO_8859_ISO_8859_9_UCS_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-11.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_11_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_11_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-15.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_15_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_15_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-2.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_2_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_2_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-3.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_3_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_3_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-4.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_4_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_4_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-5.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_5_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_5_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-6.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_6_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_6_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-7.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_7_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_7_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-8.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_8_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_8_mps; }
        if (0 == strcmp(current_path, "ISO-8859/UCS%ISO-8859-9.mps")) { *len = csmapper_ISO_8859_UCS_ISO_8859_9_mps_len; return csmapper_ISO_8859_UCS_ISO_8859_9_mps; }
        if (0 == strcmp(current_path, "CP/CP936EXT%UCS.mps")) { *len = csmapper_CP_CP936EXT_UCS_mps_len; return csmapper_CP_CP936EXT_UCS_mps; }
        if (0 == strcmp(current_path, "CP/UCS%CP936EXT.mps")) { *len = csmapper_CP_UCS_CP936EXT_mps_len; return csmapper_CP_UCS_CP936EXT_mps; }
        if (0 == strcmp(current_path, "GB/GB18030%UCS@BMP.mps")) { *len = csmapper_GB_GB18030_UCS_BMP_mps_len; return csmapper_GB_GB18030_UCS_BMP_mps; }
        if (0 == strcmp(current_path, "GB/GB2312EXT%UCS.mps")) { *len = csmapper_GB_GB2312EXT_UCS_mps_len; return csmapper_GB_GB2312EXT_UCS_mps; }
        if (0 == strcmp(current_path, "GB/GB2312GBK%UCS.mps")) { *len = csmapper_GB_GB2312GBK_UCS_mps_len; return csmapper_GB_GB2312GBK_UCS_mps; }
        if (0 == strcmp(current_path, "GB/GB2312UDA1%UCS.mps")) { *len = csmapper_GB_GB2312UDA1_UCS_mps_len; return csmapper_GB_GB2312UDA1_UCS_mps; }
        if (0 == strcmp(current_path, "GB/GB2312UDA2%UCS.mps")) { *len = csmapper_GB_GB2312UDA2_UCS_mps_len; return csmapper_GB_GB2312UDA2_UCS_mps; }
        if (0 == strcmp(current_path, "GB/GB2312%UCS.mps")) { *len = csmapper_GB_GB2312_UCS_mps_len; return csmapper_GB_GB2312_UCS_mps; }
        if (0 == strcmp(current_path, "GB/GBKEXT%UCS.mps")) { *len = csmapper_GB_GBKEXT_UCS_mps_len; return csmapper_GB_GBKEXT_UCS_mps; }
        if (0 == strcmp(current_path, "GB/GBKUDA%UCS.mps")) { *len = csmapper_GB_GBKUDA_UCS_mps_len; return csmapper_GB_GBKUDA_UCS_mps; }
        if (0 == strcmp(current_path, "GB/UCS@BMP%GB18030.mps")) { *len = csmapper_GB_UCS_BMP_GB18030_mps_len; return csmapper_GB_UCS_BMP_GB18030_mps; }
        if (0 == strcmp(current_path, "GB/UCS%GB2312EXT.mps")) { *len = csmapper_GB_UCS_GB2312EXT_mps_len; return csmapper_GB_UCS_GB2312EXT_mps; }
        if (0 == strcmp(current_path, "GB/UCS%GB2312UDA1.mps")) { *len = csmapper_GB_UCS_GB2312UDA1_mps_len; return csmapper_GB_UCS_GB2312UDA1_mps; }
        if (0 == strcmp(current_path, "GB/UCS%GB2312UDA2.mps")) { *len = csmapper_GB_UCS_GB2312UDA2_mps_len; return csmapper_GB_UCS_GB2312UDA2_mps; }
        if (0 == strcmp(current_path, "GB/UCS%GB2312.mps")) { *len = csmapper_GB_UCS_GB2312_mps_len; return csmapper_GB_UCS_GB2312_mps; }
        if (0 == strcmp(current_path, "GB/UCS%GBKEXT.mps")) { *len = csmapper_GB_UCS_GBKEXT_mps_len; return csmapper_GB_UCS_GBKEXT_mps; }
        if (0 == strcmp(current_path, "GB/UCS%GBKUDA.mps")) { *len = csmapper_GB_UCS_GBKUDA_mps_len; return csmapper_GB_UCS_GBKUDA_mps; }
        if (0 == strcmp(current_path, "ISO646/ISO646-JP%UCS.646")) { *len = csmapper_ISO646_ISO646_JP_UCS_646_len; return csmapper_ISO646_ISO646_JP_UCS_646; }
        if (0 == strcmp(current_path, "ISO646/ISO646-US%UCS.646")) { *len = csmapper_ISO646_ISO646_US_UCS_646_len; return csmapper_ISO646_ISO646_US_UCS_646; }
        if (0 == strcmp(current_path, "JIS/JISX0201-KANA%UCS.mps")) { *len = csmapper_JIS_JISX0201_KANA_UCS_mps_len; return csmapper_JIS_JISX0201_KANA_UCS_mps; }
        if (0 == strcmp(current_path, "JIS/JISX0208@1990%UCS.mps")) { *len = csmapper_JIS_JISX0208_1990_UCS_mps_len; return csmapper_JIS_JISX0208_1990_UCS_mps; }
        if (0 == strcmp(current_path, "JIS/JISX0212%UCS.mps")) { *len = csmapper_JIS_JISX0212_UCS_mps_len; return csmapper_JIS_JISX0212_UCS_mps; }
        if (0 == strcmp(current_path, "JIS/UCS%JISX0201-KANA.mps")) { *len = csmapper_JIS_UCS_JISX0201_KANA_mps_len; return csmapper_JIS_UCS_JISX0201_KANA_mps; }
        if (0 == strcmp(current_path, "JIS/UCS%JISX0208@1990.mps")) { *len = csmapper_JIS_UCS_JISX0208_1990_mps_len; return csmapper_JIS_UCS_JISX0208_1990_mps; }
        if (0 == strcmp(current_path, "JIS/UCS%JISX0212.mps")) { *len = csmapper_JIS_UCS_JISX0212_mps_len; return csmapper_JIS_UCS_JISX0212_mps; }
        if (0 == strcmp(current_path, "KS/KSC5601HANGUL%UCS.mps")) { *len = csmapper_KS_KSC5601HANGUL_UCS_mps_len; return csmapper_KS_KSC5601HANGUL_UCS_mps; }
        if (0 == strcmp(current_path, "KS/KSC5601HANJA%UCS.mps")) { *len = csmapper_KS_KSC5601HANJA_UCS_mps_len; return csmapper_KS_KSC5601HANJA_UCS_mps; }
        if (0 == strcmp(current_path, "KS/UCS%KSC5601HANGUL.mps")) { *len = csmapper_KS_UCS_KSC5601HANGUL_mps_len; return csmapper_KS_UCS_KSC5601HANGUL_mps; }
        if (0 == strcmp(current_path, "KS/UCS%KSC5601HANJA.mps")) { *len = csmapper_KS_UCS_KSC5601HANJA_mps_len; return csmapper_KS_UCS_KSC5601HANJA_mps; }

        oficonv_log(1 /* debug */, "oficonv data file '", path, "' not present in built-in library.");
      }
      /* check if the filename starts with esdb */
#ifdef HAVE_WINDOWS_H
      else if (0 == strncmp(current_path, "esdb/", 5) || 0 == strncmp(current_path, "esdb\\", 5))
#else
      else if (0 == strncmp(current_path, "esdb/", 5))
#endif
      {
        current_path += 5;

        if (0 == strcmp(current_path, "esdb.alias.db")) { *len = esdb_esdb_alias_db_len; return esdb_esdb_alias_db; }
        if (0 == strcmp(current_path, "esdb.dir.db")) { *len = esdb_esdb_dir_db_len; return esdb_esdb_dir_db; }
        if (0 == strcmp(current_path, "UTF/UTF-8.esdb")) { *len = esdb_UTF_UTF_8_esdb_len; return esdb_UTF_UTF_8_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-1.esdb")) { *len = esdb_ISO_8859_ISO_8859_1_esdb_len; return esdb_ISO_8859_ISO_8859_1_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-11.esdb")) { *len = esdb_ISO_8859_ISO_8859_11_esdb_len; return esdb_ISO_8859_ISO_8859_11_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-15.esdb")) { *len = esdb_ISO_8859_ISO_8859_15_esdb_len; return esdb_ISO_8859_ISO_8859_15_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-2.esdb")) { *len = esdb_ISO_8859_ISO_8859_2_esdb_len; return esdb_ISO_8859_ISO_8859_2_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-3.esdb")) { *len = esdb_ISO_8859_ISO_8859_3_esdb_len; return esdb_ISO_8859_ISO_8859_3_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-4.esdb")) { *len = esdb_ISO_8859_ISO_8859_4_esdb_len; return esdb_ISO_8859_ISO_8859_4_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-5.esdb")) { *len = esdb_ISO_8859_ISO_8859_5_esdb_len; return esdb_ISO_8859_ISO_8859_5_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-6.esdb")) { *len = esdb_ISO_8859_ISO_8859_6_esdb_len; return esdb_ISO_8859_ISO_8859_6_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-7.esdb")) { *len = esdb_ISO_8859_ISO_8859_7_esdb_len; return esdb_ISO_8859_ISO_8859_7_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-8.esdb")) { *len = esdb_ISO_8859_ISO_8859_8_esdb_len; return esdb_ISO_8859_ISO_8859_8_esdb; }
        if (0 == strcmp(current_path, "ISO-8859/ISO-8859-9.esdb")) { *len = esdb_ISO_8859_ISO_8859_9_esdb_len; return esdb_ISO_8859_ISO_8859_9_esdb; }
        if (0 == strcmp(current_path, "EUC/EUC-CN.esdb")) { *len = esdb_EUC_EUC_CN_esdb_len; return esdb_EUC_EUC_CN_esdb; }
        if (0 == strcmp(current_path, "EUC/EUC-KR.esdb")) { *len = esdb_EUC_EUC_KR_esdb_len; return esdb_EUC_EUC_KR_esdb; }
        if (0 == strcmp(current_path, "GB/GB18030.esdb")) { *len = esdb_GB_GB18030_esdb_len; return esdb_GB_GB18030_esdb; }
        if (0 == strcmp(current_path, "GB/GBK.esdb")) { *len = esdb_GB_GBK_esdb_len; return esdb_GB_GBK_esdb; }
        if (0 == strcmp(current_path, "ISO646/ISO646-US.esdb")) { *len = esdb_ISO646_ISO646_US_esdb_len; return esdb_ISO646_ISO646_US_esdb; }
        if (0 == strcmp(current_path, "MISC/JISX0201-KANA.esdb")) { *len = esdb_MISC_JISX0201_KANA_esdb_len; return esdb_MISC_JISX0201_KANA_esdb; }
        if (0 == strcmp(current_path, "MISC/JISX0208@1990.esdb")) { *len = esdb_MISC_JISX0208_1990_esdb_len; return esdb_MISC_JISX0208_1990_esdb; }
        if (0 == strcmp(current_path, "MISC/JISX0212.esdb")) { *len = esdb_MISC_JISX0212_esdb_len; return esdb_MISC_JISX0212_esdb; }
        if (0 == strcmp(current_path, "MISC/Shift_JIS.esdb")) { *len = esdb_MISC_Shift_JIS_esdb_len; return esdb_MISC_Shift_JIS_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-16-INTERNAL.esdb")) { *len = esdb_UTF_UTF_16_INTERNAL_esdb_len; return esdb_UTF_UTF_16_INTERNAL_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-16-SWAPPED.esdb")) { *len = esdb_UTF_UTF_16_SWAPPED_esdb_len; return esdb_UTF_UTF_16_SWAPPED_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-16.esdb")) { *len = esdb_UTF_UTF_16_esdb_len; return esdb_UTF_UTF_16_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-16BE.esdb")) { *len = esdb_UTF_UTF_16BE_esdb_len; return esdb_UTF_UTF_16BE_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-16LE.esdb")) { *len = esdb_UTF_UTF_16LE_esdb_len; return esdb_UTF_UTF_16LE_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-32-INTERNAL.esdb")) { *len = esdb_UTF_UTF_32_INTERNAL_esdb_len; return esdb_UTF_UTF_32_INTERNAL_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-32-SWAPPED.esdb")) { *len = esdb_UTF_UTF_32_SWAPPED_esdb_len; return esdb_UTF_UTF_32_SWAPPED_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-32.esdb")) { *len = esdb_UTF_UTF_32_esdb_len; return esdb_UTF_UTF_32_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-32BE.esdb")) { *len = esdb_UTF_UTF_32BE_esdb_len; return esdb_UTF_UTF_32BE_esdb; }
        if (0 == strcmp(current_path, "UTF/UTF-32LE.esdb")) { *len = esdb_UTF_UTF_32LE_esdb_len; return esdb_UTF_UTF_32LE_esdb; }

        oficonv_log(1 /* debug */, "oficonv data file '", path, "' not present in built-in library.");
      }
    }

#endif /* DCMTK_ENABLE_BUILTIN_OFICONV_DATA */
    return NULL;
}

int
_citrus_map_file(struct _citrus_region * r,
    const char * path)
{
    struct stat st;
    size_t len;
    void *head;
    int fd, ret;

    ret = 0;

    _citrus_region_init(r, NULL, 0);

    /* check if we have a built-in array that we can use instead of loading from file */
    head = lookup_builtin_data_file(path, &len);
    if (head)
    {
        _citrus_region_init(r, head, len);
        return 0;
    }

    /* otherwise continue with the file system */
#ifdef O_CLOEXEC
    if ((fd = open(path, O_RDONLY | O_CLOEXEC)) == -1)
#else
    if ((fd = open(path, O_RDONLY)) == -1)
#endif
    {
        oficonv_log(3 /* warning */, "Failed to open oficonv data file '", path, "', check environment variable DCMICONVPATH");
        return (errno);
    }
    if (fstat(fd, &st)  == -1) {
        ret = errno;
        oficonv_log(3 /* warning */, "Failed to open oficonv data file '", path, "', check environment variable DCMICONVPATH");
        goto error;
    }
    if (!S_ISREG(st.st_mode)) {
        ret = EOPNOTSUPP;
        oficonv_log(3 /* warning */, "Failed to open oficonv data file '", path, "', check environment variable DCMICONVPATH");
        goto error;
    }

#ifdef MAP_FILE
    head = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, (off_t)0);
#else
    head = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, (off_t)0);
#endif
    if (head == MAP_FAILED) {
        ret = errno;
        goto error;
    }
    _citrus_region_init(r, head, (size_t)st.st_size);
    _citrus_set_is_file(r);
    oficonv_log(1 /* debug */, "Opened oficonv data file '", path, "'");
error:
    (void)close(fd);
    return (ret);
}

void
_citrus_unmap_file(struct _citrus_region *r)
{
    if (_citrus_region_head(r) != NULL) {
        if ( _citrus_is_file(r)) {
            (void)munmap(_citrus_region_head(r), _citrus_region_size(r));
        }
        _citrus_region_init(r, NULL, 0);
    }
}
