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
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef CMR_DEFINE_H
#define CMR_DEFINE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef cmr_EXPORTS
#define DCMTK_CMR_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_CMR_EXPORT DCMTK_DECL_IMPORT
#endif


#endif
