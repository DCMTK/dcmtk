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
 *  Purpose: Contains preprocessor definitions
 *
 */

#ifndef DCMGROK_GKDEFINE_H
#define DCMGROK_GKDEFINE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofdefine.h"

#ifdef dcmgrok_EXPORTS
#define DCMTK_DCMGROK_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMGROK_EXPORT DCMTK_DECL_IMPORT
#endif

#endif
