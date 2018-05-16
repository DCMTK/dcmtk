/*
 *
 *  Copyright (C) 1998-2017, OFFIS e.V.
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
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTransportLayer
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcmlayer.h"
#include "dcmtk/dcmnet/dcmtrans.h"
#include "dcmtk/dcmnet/dndefine.h"

#ifdef WITH_TCPWRAPPER
BEGIN_EXTERN_C
#include <syslog.h>
END_EXTERN_C
#endif

DcmTransportLayer::~DcmTransportLayer()
{
}

DcmTransportConnection * DcmTransportLayer::createConnection(DcmNativeSocketType openSocket, OFBool useSecureLayer)
{
  if (useSecureLayer) return NULL;  /* secure layer connections not supported */
  else return new DcmTCPConnection(openSocket);
}

#ifdef WITH_TCPWRAPPER
#ifndef TCPWRAPPER_SEVERITY_EXTERN

/* libwrap expects that two global flags, deny_severity and allow_severity,
 * are defined and initialized by user code. If these flags are already present
 * somewhere else, compile DCMTK with TCPWRAPPER_SEVERITY_EXTERN defined
 * to avoid linker errors due to duplicate symbols.
 */
int DCMTK_DCMNET_EXPORT deny_severity = LOG_WARNING;
int DCMTK_DCMNET_EXPORT allow_severity = LOG_INFO;
#endif
#endif
