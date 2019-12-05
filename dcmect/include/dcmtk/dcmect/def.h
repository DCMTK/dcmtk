/*
 *
 *  Copyright (C) 2019, Open Connections GmbH
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
 *  Module:  dcmect
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Contains preprocessor definitions
 *
 */

#ifndef DCMECT_DEF_H
#define DCMECT_DEF_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"

// definitions for DLL/shared library exports

#ifdef dcmect_EXPORTS
#define DCMTK_DCMECT_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMECT_EXPORT DCMTK_DECL_IMPORT
#endif

#endif // DCMECT_DEF_H
