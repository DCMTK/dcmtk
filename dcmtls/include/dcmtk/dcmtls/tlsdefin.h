/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef TLSDEFIN_H
#define TLSDEFIN_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmtls_EXPORTS
#define DCMTK_DCMTLS_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMTLS_EXPORT DCMTK_DECL_IMPORT
#endif


#endif
