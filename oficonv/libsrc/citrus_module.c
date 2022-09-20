/*-
 * Copyright (c)1999, 2000, 2001, 2002 Citrus Project,
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

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Paul Borman at Krystal Technologies.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "dcmtk/config/osconfig.h"
#include "citrus_module.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif


#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define I18NMODULE_MAJOR    5

#include "citrus_bcs.h"

#include "citrus_iconv_local.h"
#include "citrus_iconv_std.h"
#include "citrus_iconv_none.h"

#include "citrus_stdenc_local.h"
#include "citrus_big5.h"
#include "citrus_dechanyu.h"
#include "citrus_euc.h"
#include "citrus_euctw.h"
#include "citrus_gbk2k.h"
#include "citrus_hz.h"
#include "citrus_iso2022.h"
#include "citrus_johab.h"
#include "citrus_mskanji.h"
#include "citrus_ues.h"
#include "citrus_utf1632.h"
#include "citrus_utf7.h"
#include "citrus_utf8.h"
#include "citrus_viqr.h"
#include "citrus_zw.h"
#include "citrus_jisx0208.h"

#include "citrus_mapper_local.h"
#include "citrus_mapper_none.h"
#include "citrus_mapper_646.h"
#include "citrus_mapper_serial.h"
#include "citrus_mapper_zone.h"
#include "citrus_mapper_std.h"

#ifdef _MSC_VER
/* disable warning about typecasts from void * to function pointer
 * and vice versa in older MSVC versions (VS 2015 and older) */
#pragma warning(disable: 4054)
#pragma warning(disable: 4055)
#endif

void *
_citrus_find_getops(_citrus_module_t handle, const char *modname,
    const char *ifname)
{
    (void) modname;
    (void) ifname;
    return (void *) handle;
}

int
_citrus_load_module(_citrus_module_t *rhandle, const char *encname)
{
    if (strcmp(encname, "iconv_std") == 0) *rhandle = (_citrus_module_t) &_citrus_iconv_std_iconv_getops;
    else if (strcmp(encname, "iconv_none") == 0) *rhandle = (_citrus_module_t) &_citrus_iconv_none_iconv_getops;
    else if (strcmp(encname, "UTF8") == 0) *rhandle = (_citrus_module_t) &_citrus_UTF8_stdenc_getops;
    else if (strcmp(encname, "BIG5") == 0) *rhandle = (_citrus_module_t) &_citrus_BIG5_stdenc_getops;
    else if (strcmp(encname, "DECHanyu") == 0) *rhandle = (_citrus_module_t) &_citrus_DECHanyu_stdenc_getops;
    else if (strcmp(encname, "EUC") == 0) *rhandle = (_citrus_module_t) &_citrus_EUC_stdenc_getops;
    else if (strcmp(encname, "EUCTW") == 0) *rhandle = (_citrus_module_t) &_citrus_EUCTW_stdenc_getops;
    else if (strcmp(encname, "GBK2K") == 0) *rhandle = (_citrus_module_t) &_citrus_GBK2K_stdenc_getops;
    else if (strcmp(encname, "HZ") == 0) *rhandle = (_citrus_module_t) &_citrus_HZ_stdenc_getops;
    else if (strcmp(encname, "ISO2022") == 0) *rhandle = (_citrus_module_t) &_citrus_ISO2022_stdenc_getops;
    else if (strcmp(encname, "JISX0208") == 0) *rhandle = (_citrus_module_t) &_citrus_JISX0208_stdenc_getops;
    else if (strcmp(encname, "JOHAB") == 0) *rhandle = (_citrus_module_t) &_citrus_JOHAB_stdenc_getops;
    else if (strcmp(encname, "MSKanji") == 0) *rhandle = (_citrus_module_t) &_citrus_MSKanji_stdenc_getops;
    else if (strcmp(encname, "UES") == 0) *rhandle = (_citrus_module_t) &_citrus_UES_stdenc_getops;
    else if (strcmp(encname, "UTF1632") == 0) *rhandle = (_citrus_module_t) &_citrus_UTF1632_stdenc_getops;
    else if (strcmp(encname, "UTF7") == 0) *rhandle = (_citrus_module_t) &_citrus_UTF7_stdenc_getops;
    else if (strcmp(encname, "VIQR") == 0) *rhandle = (_citrus_module_t) &_citrus_VIQR_stdenc_getops;
    else if (strcmp(encname, "ZW") == 0) *rhandle = (_citrus_module_t) &_citrus_ZW_stdenc_getops;
    else if (strcmp(encname, "mapper_none") == 0) *rhandle = (_citrus_module_t) &_citrus_mapper_none_mapper_getops;
    else if (strcmp(encname, "mapper_std") == 0) *rhandle = (_citrus_module_t) &_citrus_mapper_std_mapper_getops;
    else if (strcmp(encname, "mapper_serial") == 0) *rhandle = (_citrus_module_t) &_citrus_mapper_serial_mapper_getops;
    else if (strcmp(encname, "mapper_zone") == 0) *rhandle = (_citrus_module_t) &_citrus_mapper_zone_mapper_getops;
    else if (strcmp(encname, "mapper_646") == 0) *rhandle = (_citrus_module_t) &_citrus_mapper_646_mapper_getops;
    else if (strcmp(encname, "mapper_parallel") == 0) *rhandle = (_citrus_module_t) &_citrus_mapper_parallel_mapper_getops;
    else
    {
        return (EINVAL);
    }
    return (0);
}

void
_citrus_unload_module(_citrus_module_t handle)
{
    (void) handle;
}
