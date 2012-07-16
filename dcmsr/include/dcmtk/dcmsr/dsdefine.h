/*
 *
 *  Copyright (C) 2012, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef DSDEFINE_H
#define DSDEFINE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmsr_EXPORTS
#define DCMTK_DCMSR_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMSR_EXPORT DCMTK_DECL_IMPORT
#endif


#endif
