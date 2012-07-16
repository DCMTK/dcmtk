/*
 *
 *  Copyright (C) 2009-2011, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Logger (Header)
 *
 */


#ifndef DILOGGER_H
#define DILOGGER_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/oflog/oflog.h"

#include "dcmtk/dcmimage/dicdefin.h"

extern DCMTK_DCMIMAGE_EXPORT OFLogger DCM_dcmimageLogger;

#define DCMIMAGE_TRACE(msg) OFLOG_TRACE(DCM_dcmimageLogger, msg)
#define DCMIMAGE_DEBUG(msg) OFLOG_DEBUG(DCM_dcmimageLogger, msg)
#define DCMIMAGE_INFO(msg)  OFLOG_INFO(DCM_dcmimageLogger, msg)
#define DCMIMAGE_WARN(msg)  OFLOG_WARN(DCM_dcmimageLogger, msg)
#define DCMIMAGE_ERROR(msg) OFLOG_ERROR(DCM_dcmimageLogger, msg)
#define DCMIMAGE_FATAL(msg) OFLOG_FATAL(DCM_dcmimageLogger, msg)

#endif
