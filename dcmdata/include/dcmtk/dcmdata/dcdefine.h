/*
 *
 *  Copyright (C) 2011-2014, OFFIS e.V.
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef DCDEFINE_H
#define DCDEFINE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdefine.h"

// Workaround for Visual Studio to disable dll storage specifiers when linking directly into the executable.
// This is needed since the built-in dictionary code is created by the tools mkdeftag and mkdictbi and thus
// those tools statically link the few required dcmdata source files instead of linking to dcmdata as a whole.
#ifndef DCMDATA_BUILD_DICTIONARY
#ifdef dcmdata_EXPORTS
#define DCMTK_DCMDATA_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMDATA_EXPORT DCMTK_DECL_IMPORT
#endif
#else
#define DCMTK_DCMDATA_EXPORT
#endif


#endif // DCDEFINE_H
