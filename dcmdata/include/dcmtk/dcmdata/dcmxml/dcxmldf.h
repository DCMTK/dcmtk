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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Preprocessor definitions for DLL import/export
 *
 */

#ifndef DCXMLDEF_H
#define DCXMLDEF_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofdefine.h"

#ifdef dcmxml_EXPORTS
#define DCMTK_DCMXML_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMXML_EXPORT DCMTK_DECL_IMPORT
#endif

#endif // DCDEFINE_H
