/*
 *
 *  Copyright (C) 2010-2018, OFFIS e.V.
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
 *  Module: dcmdata
 *
 *  Author: Uli Schlachter
 *
 *  Purpose: Internal header for vrscanl.c and vrscan.cc
 *
 */

#ifndef VRSCANI_H
#define VRSCANI_H

/* This needs its own header because both vrscanl.c and vrscan.cc need it. */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include <setjmp.h>

struct vrscan_error {
    jmp_buf setjmp_buffer;
    const char *error_msg;
};

#define YY_EXTRA_TYPE struct vrscan_error *
#define YY_FATAL_ERROR(msg) do { \
    yyget_extra(yyscanner)->error_msg = msg; \
    longjmp(yyget_extra(yyscanner)->setjmp_buffer, 1); \
} while (0);

#ifndef HAVE_UNISTD_H

/* Don't try to include unistd.h which doesn't exist on windows */
#define YY_NO_UNISTD_H

/* Declare isatty() to avoid a warning when compiling vrscanl.l */
int isatty(int fd);

#endif

#endif /* VRSCANI_H */
