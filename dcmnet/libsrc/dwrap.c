/*
 *
 *  Copyright (C) 2012-2018, OFFIS e.V.
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
 *  Module: dcmnet
 *
 *  Author: Uli Schlachter
 *
 *  Purpose: C wrappers for libwrap
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#ifdef WITH_TCPWRAPPER

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <tcpd.h>               /* for hosts_ctl */

/* Some versions of libwrap omit the full prototype from tcpd.h.
 * Instead something like this is used:
 *
 *   int hosts_access();
 *
 * This makes it impossible to call hosts_access() from C++, so we need a C
 * wrapper around it which provides the correct prototype.
 */
int dcmtk_hosts_access(struct request_info *req)
{
    return hosts_access(req);
}

#else /* WITH_TCPWRAPPER */

int dwrap_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
