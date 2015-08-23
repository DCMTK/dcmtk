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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef SEGDEF_H
#define SEGDEF_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofdefine.h"

// definitions for DLL/shared library exports

#ifdef dcmseg_EXPORTS
#define DCMTK_DCMSEG_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMSEG_EXPORT DCMTK_DECL_IMPORT
#endif

#endif
