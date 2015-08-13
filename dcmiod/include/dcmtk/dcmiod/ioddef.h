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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Contains preprocessor definitions for dcmiod module
 *
 */


#ifndef IODDEF_H
#define IODDEF_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofdefine.h"


// Make sure DLL exports work for this module

#ifdef dcmiod_EXPORTS
#define DCMTK_DCMIOD_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMIOD_EXPORT DCMTK_DECL_IMPORT
#endif


#endif // IODDEF_H
