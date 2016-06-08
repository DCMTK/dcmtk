/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Module:  dcmtract
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef TRCDEF_H
#define TRCDEF_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofdefine.h"

// definitions for DLL/shared library exports

#ifdef dcmtract_EXPORTS
#define DCMTK_DCMTRACT_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMTRACT_EXPORT DCMTK_DECL_IMPORT
#endif

#endif // TRCDEF_H
