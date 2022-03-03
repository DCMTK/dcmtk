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

#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

int
_citrus_map_file(struct _citrus_region * r,
    const char * path)
{
    struct stat st;
    void *head;
    int fd, ret;

    ret = 0;

    _citrus_region_init(r, NULL, 0);

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
    oficonv_log(1 /* debug */, "Opened oficonv data file '", path, "'");

error:
    (void)close(fd);
    return (ret);
}

void
_citrus_unmap_file(struct _citrus_region *r)
{

    if (_citrus_region_head(r) != NULL) {
        (void)munmap(_citrus_region_head(r), _citrus_region_size(r));
        _citrus_region_init(r, NULL, 0);
    }
}
