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
#include "citrus_esdb.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif


#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "citrus_bcs.h"
#include "citrus_types.h"
#include "citrus_region.h"
#include "citrus_memstream.h"
#include "citrus_mmap.h"
#include "citrus_lookup.h"
#include "citrus_db.h"
#include "citrus_db_hash.h"
#include "citrus_esdb_file.h"

#define ESDB_DIR    "esdb.dir"
#define ESDB_ALIAS  "esdb.alias"

/* write the full path to the ESDB directory and the given filename (which may be NULL)
 * to the path_out buffer, which is expected to be OFICONV_PATH_MAX in size.
 */
static void getESDBPath(char *path_out, size_t path_size, const char *filename)
{
    get_data_path(path_out, path_size, OFICONV_ESDB_DIR, filename);
}


/*
 * _citrus_esdb_alias:
 *  resolve encoding scheme name aliases.
 */
const char *
_citrus_esdb_alias(const char *esname, char *buf, size_t bufsize)
{
    char esdb_path[OFICONV_PATH_MAX];
    getESDBPath(esdb_path, sizeof(esdb_path), ESDB_ALIAS);
    return (_citrus_lookup_alias(esdb_path, esname, buf, bufsize, _CITRUS_LOOKUP_CASE_IGNORE));
}


/*
 * conv_esdb:
 *  external representation -> local structure.
 */
static int
conv_esdb(struct _citrus_esdb *esdb, struct _citrus_region *fr)
{
    struct _citrus_db *db;
    const char *str;
    char buf[100];
    uint32_t csid, i, num_charsets, tmp, version;
    int ret;

    /* open db */
    ret = _citrus_db_open(&db, fr, _CITRUS_ESDB_MAGIC, &_citrus_db_hash_std, NULL);
    if (ret)
        goto err0;

    /* check version */
    ret = _citrus_db_lookup32_by_string(db, _CITRUS_ESDB_SYM_VERSION, &version, NULL);
    if (ret)
        goto err1;
    switch (version) {
    case 0x00000001:
        /* current version */
        /* initial version */
        break;
    default:
        ret = EFTYPE;
        goto err1;
    }

    /* get encoding/variable */
    ret = _citrus_db_lookup_string_by_string(db, _CITRUS_ESDB_SYM_ENCODING, &str, NULL);
    if (ret)
        goto err1;
    esdb->db_encname = strdup(str);
    if (esdb->db_encname == NULL) {
        ret = errno;
        goto err1;
    }

    esdb->db_len_variable = 0;
    esdb->db_variable = NULL;
    ret = _citrus_db_lookup_string_by_string(db, _CITRUS_ESDB_SYM_VARIABLE, &str, NULL);
    if (ret == 0) {
        esdb->db_len_variable = strlen(str) + 1;
        esdb->db_variable = strdup(str);
        if (esdb->db_variable == NULL) {
            ret = errno;
            goto err2;
        }
    } else if (ret != ENOENT)
        goto err2;

    /* get number of charsets */
    ret = _citrus_db_lookup32_by_string(db, _CITRUS_ESDB_SYM_NUM_CHARSETS,
        &num_charsets, NULL);
    if (ret)
        goto err3;
    esdb->db_num_charsets = num_charsets;

    /* get invalid character */
    ret = _citrus_db_lookup32_by_string(db, _CITRUS_ESDB_SYM_INVALID, &tmp, NULL);
    if (ret == 0) {
        esdb->db_use_invalid = 1;
        esdb->db_invalid = tmp;
    } else if (ret == ENOENT)
        esdb->db_use_invalid = 0;
    else
        goto err3;

    /* get charsets */
    esdb->db_charsets = malloc(num_charsets * sizeof(*esdb->db_charsets));
    if (esdb->db_charsets == NULL) {
        ret = errno;
        goto err3;
    }
    for (i = 0; i < num_charsets; i++) {
        snprintf(buf, sizeof(buf),
            _CITRUS_ESDB_SYM_CSID_PREFIX "%d", i);
        ret = _citrus_db_lookup32_by_string(db, buf, &csid, NULL);
        if (ret)
            goto err4;
        esdb->db_charsets[i].ec_csid = csid;

        snprintf(buf, sizeof(buf),
            _CITRUS_ESDB_SYM_CSNAME_PREFIX "%d", i);
        ret = _citrus_db_lookup_string_by_string(db, buf, &str, NULL);
        if (ret)
            goto err4;
        esdb->db_charsets[i].ec_csname = strdup(str);
        if (esdb->db_charsets[i].ec_csname == NULL) {
            ret = errno;
            goto err4;
        }
    }

    _citrus_db_close(db);
    return (0);

err4:
    for (; i > 0; i--)
        free(esdb->db_charsets[i - 1].ec_csname);
    free(esdb->db_charsets);
err3:
    free(esdb->db_variable);
err2:
    free(esdb->db_encname);
err1:
    _citrus_db_close(db);
    if (ret == ENOENT)
        ret = EFTYPE;
err0:
    return (ret);
}

/*
 * _citrus_esdb_open:
 *  open an ESDB file.
 */
int
_citrus_esdb_open(struct _citrus_esdb *db, const char *esname)
{
    struct _citrus_region fr;
    const char *realname, *encfile;
    char buf1[OFICONV_PATH_MAX], buf2[OFICONV_PATH_MAX], path[OFICONV_PATH_MAX];
    int ret;

    getESDBPath(path, sizeof(path), ESDB_ALIAS);
    realname = _citrus_lookup_alias(path, esname, buf1, sizeof(buf1),
        _CITRUS_LOOKUP_CASE_IGNORE);

    getESDBPath(path, sizeof(path), ESDB_DIR);
    encfile = _citrus_lookup_simple(path, realname, buf2, sizeof(buf2),
        _CITRUS_LOOKUP_CASE_IGNORE);
    if (encfile == NULL)
        return (ENOENT);

    /* open file */
    getESDBPath(path, sizeof(path), encfile);
    ret = _citrus_map_file(&fr, path);
    if (ret)
        return (ret);

    ret = conv_esdb(db, &fr);

    _citrus_unmap_file(&fr);

    return (ret);
}

/*
 * _citrus_esdb_close:
 *  free an ESDB.
 */
void
_citrus_esdb_close(struct _citrus_esdb *db)
{
    int i;
    for (i = 0; i < db->db_num_charsets; i++)
        free(db->db_charsets[i].ec_csname);
    db->db_num_charsets = 0;
    free(db->db_charsets); db->db_charsets = NULL;
    free(db->db_encname); db->db_encname = NULL;
    db->db_len_variable = 0;
    free(db->db_variable); db->db_variable = NULL;
}

/*
 * _citrus_esdb_free_list:
 *  free the list.
 */
void
_citrus_esdb_free_list(char **list, size_t num)
{
    size_t i;
    for (i = 0; i < num; i++)
        free(list[i]);
    free(list);
}

/*
 * _citrus_esdb_get_list:
 *  get esdb entries.
 */
int
_citrus_esdb_get_list(char ***rlist, size_t *rnum, bool sorted)
{
    struct _citrus_lookup *cla, *cld;
    struct _citrus_region key, data;
    char **list, **q;
    char buf[OFICONV_PATH_MAX];
    size_t num;
    int ret;
    char esdb_path[OFICONV_PATH_MAX];

    num = 0;

    getESDBPath(esdb_path, sizeof(esdb_path), ESDB_ALIAS);
    ret = _citrus_lookup_seq_open(&cla, esdb_path, _CITRUS_LOOKUP_CASE_IGNORE);
    if (ret)
        goto quit0;

    getESDBPath(esdb_path, sizeof(esdb_path), ESDB_DIR);
    ret = _citrus_lookup_seq_open(&cld, esdb_path, _CITRUS_LOOKUP_CASE_IGNORE);
    if (ret)
        goto quit1;

    /* count number of entries */
    num = _citrus_lookup_get_number_of_entries(cla) + _citrus_lookup_get_number_of_entries(cld);

    _citrus_lookup_seq_rewind(cla);
    _citrus_lookup_seq_rewind(cld);

    /* allocate list pointer space */
    list = malloc(num * sizeof(char *));
    num = 0;
    if (list == NULL) {
        ret = errno;
        goto quit3;
    }

    /* get alias entries */
    while ((ret = _citrus_lookup_seq_next(cla, &key, &data)) == 0) {
#if 0
        /* this code fragment is disabled in FreeBSD */
        if (sorted)
            snprintf(buf, sizeof(buf), "%.*s/%.*s",
                (int)_citrus_region_size(&data),
                (const char *)_citrus_region_head(&data),
                (int)_citrus_region_size(&key),
                (const char *)_citrus_region_head(&key));
        else
#endif
            snprintf(buf, sizeof(buf), "%.*s/%.*s",
                (int)_citrus_region_size(&data),
                (const char *)_citrus_region_head(&data),
                (int)_citrus_region_size(&key),
                (const char *)_citrus_region_head(&key));
        _citrus_bcs_convert_to_upper(buf);
        list[num] = strdup(buf);
        if (list[num] == NULL) {
            ret = errno;
            goto quit3;
        }
        num++;
    }
    if (ret != ENOENT)
        goto quit3;
    /* get dir entries */
    while ((ret = _citrus_lookup_seq_next(cld, &key, &data)) == 0) {
        if (!sorted)
            snprintf(buf, sizeof(buf), "%.*s",
                (int)_citrus_region_size(&key),
                (const char *)_citrus_region_head(&key));
        else {
            /* check duplicated entry */
            char *p;
            char buf1[OFICONV_PATH_MAX];

            snprintf(buf1, sizeof(buf1), "%.*s",
                (int)_citrus_region_size(&data),
                (const char *)_citrus_region_head(&data));
            if ((p = strchr(buf1, '/')) != NULL)
                memmove(buf1, p + 1, strlen(p) - 1);
            if ((p = strstr(buf1, ".esdb")) != NULL)
                *p = '\0';

#if ( defined(__GNUC__) && (__GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 1 ) ) )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif

            snprintf(buf, sizeof(buf), "%s/%.*s", buf1,
                (int)_citrus_region_size(&key),
                (const char *)_citrus_region_head(&key));

#if ( defined(__GNUC__) && (__GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 1 ) ) )
#pragma GCC diagnostic pop
#endif

        }
        _citrus_bcs_convert_to_upper(buf);
        ret = _citrus_lookup_seq_lookup(cla, buf, NULL);
        if (ret) {
            if (ret != ENOENT)
                goto quit3;
            /* not duplicated */
            list[num] = strdup(buf);
            if (list[num] == NULL) {
                ret = errno;
                goto quit3;
            }
            num++;
        }
    }
    if (ret != ENOENT)
        goto quit3;

    ret = 0;
    /* XXX: why reallocing the list space posteriorly?
        shouldn't be done earlier? */
    q = realloc(list, num * sizeof(char *));

    if (!q) {
        ret = ENOMEM;
        goto quit3;
    }
    list = q;
    *rlist = list;
    *rnum = num;
quit3:
    if (ret)
        _citrus_esdb_free_list(list, num);
    _citrus_lookup_seq_close(cld);
quit1:
    _citrus_lookup_seq_close(cla);
quit0:
    return (ret);
}
