/*
 *
 *  Copyright (C) 2022, OFFIS e.V.
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
 *  Module:  oficonv
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Preprocessor definitions for shared library creation
 *
 */


#ifndef OIDEFINE_H
#define OIDEFINE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofexport.h"

#ifdef oficonv_EXPORTS
#define DCMTK_OFICONV_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_OFICONV_EXPORT DCMTK_DECL_IMPORT
#endif


#endif
