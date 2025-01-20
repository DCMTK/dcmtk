/*
 *
 *  Copyright (C) 1994-2024, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: common defines for configuration
 *
 */

#ifndef OFDEFINE_H
#define OFDEFINE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofexport.h"

#ifdef ofstd_EXPORTS
#define DCMTK_OFSTD_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_OFSTD_EXPORT DCMTK_DECL_IMPORT
#endif

/* define OFconstexpr to 'constexpr' or '' if not supported */
#ifdef HAVE_CXX11
#define OFconstexpr constexpr
#else // C++11
#define OFconstexpr
#endif // NOT C++11

#endif
