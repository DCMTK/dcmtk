/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:18:41 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

/* Don't try to include unistd.h which doesn't exist on windows */
#define YY_NO_UNISTD_H

#endif /* VRSCANI_H */

/*
** CVS/RCS Log:
** $Log: vrscani.h,v $
** Revision 1.4  2010-10-14 13:18:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.3  2010-10-01 10:21:05  uli
** Fixed most compiler warnings from -Wall -Wextra -pedantic in dcmdata.
**
** Revision 1.2  2010-09-03 07:26:19  uli
** Make the VR scanner work on windows again.
**
** Revision 1.1  2010-09-02 12:02:06  uli
** Use longjmp() for error handling in the VR scanner.
**
**
*/
