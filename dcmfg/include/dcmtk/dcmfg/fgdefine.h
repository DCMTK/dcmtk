/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef FGDEFINE_H
#define FGDEFINE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmfg_EXPORTS
#define DCMTK_DCMFG_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMFG_EXPORT DCMTK_DECL_IMPORT
#endif

#endif // FGDEFINE_H
