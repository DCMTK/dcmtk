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
 *  Module:  dcmimage
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef DICDEFIN_H
#define DICDEFIN_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmimage_EXPORTS
#define DCMTK_DCMIMAGE_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMIMAGE_EXPORT DCMTK_DECL_IMPORT
#endif


#endif
