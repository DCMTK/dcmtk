/*
 *
 *  Copyright (C) 2026, Grok Image Compression Inc.
 *  All rights reserved.
 *
 *  This software and supporting documentation were developed by
 *  Grok Image Compression Inc.
 *
 *  Module:  dcmgrok
 *
 *  Author:  Aaron Boxer
 *
 *  Purpose: enums and utilities for the Grok JPEG 2000 codec module
 *
 */

#ifndef DCMGROK_GKUTILS_H
#define DCMGROK_GKUTILS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmgrok/gkdefine.h"

extern DCMTK_DCMGROK_EXPORT OFLogger DCM_dcmgrokLogger;

#define DCMGROK_TRACE(msg) OFLOG_TRACE(DCM_dcmgrokLogger, msg)
#define DCMGROK_DEBUG(msg) OFLOG_DEBUG(DCM_dcmgrokLogger, msg)
#define DCMGROK_INFO(msg)  OFLOG_INFO(DCM_dcmgrokLogger, msg)
#define DCMGROK_WARN(msg)  OFLOG_WARN(DCM_dcmgrokLogger, msg)
#define DCMGROK_ERROR(msg) OFLOG_ERROR(DCM_dcmgrokLogger, msg)
#define DCMGROK_FATAL(msg) OFLOG_FATAL(DCM_dcmgrokLogger, msg)

/** describes how new SOP instance UIDs are handled during decompression
 */
enum GRK_UIDCreation
{
    /** default: assign new UID if lossy compression
     */
    EGRKUC_default = 0,
    /** always assign new UID
     */
    EGRKUC_always = 1,
    /** never assign new UID
     */
    EGRKUC_never = 2
};

/** describes how planar configuration of decompressed color images is handled
 */
enum GRK_PlanarConfiguration
{
    /** restore planar configuration from dataset
     */
    EGRKPC_restore = 0,
    /** automatically determine best planar configuration
     */
    EGRKPC_auto = 1,
    /** color-by-pixel (planarConfiguration = 0)
     */
    EGRKPC_colorByPixel = 2,
    /** color-by-plane (planarConfiguration = 1)
     */
    EGRKPC_colorByPlane = 3
};

#endif
