/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    definition of logger for this submodule
 *
 */


#ifndef CMR_LOGGER_H
#define CMR_LOGGER_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/oflog/oflog.h"

#include "dcmtk/dcmsr/cmr/define.h"


// global definitions for logging mechanism provided by the oflog module

extern DCMTK_CMR_EXPORT OFLogger DCM_dcmsrCmrLogger;

#define DCMSR_CMR_TRACE(msg) OFLOG_TRACE(DCM_dcmsrCmrLogger, msg)
#define DCMSR_CMR_DEBUG(msg) OFLOG_DEBUG(DCM_dcmsrCmrLogger, msg)
#define DCMSR_CMR_INFO(msg)  OFLOG_INFO(DCM_dcmsrCmrLogger, msg)
#define DCMSR_CMR_WARN(msg)  OFLOG_WARN(DCM_dcmsrCmrLogger, msg)
#define DCMSR_CMR_ERROR(msg) OFLOG_ERROR(DCM_dcmsrCmrLogger, msg)
#define DCMSR_CMR_FATAL(msg) OFLOG_FATAL(DCM_dcmsrCmrLogger, msg)


#endif
