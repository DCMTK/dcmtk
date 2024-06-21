/*
 *
 *  Copyright (C) 2003-2024, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Portable macros for new-style typecasts
 *
 */

#ifndef OFCAST_H
#define OFCAST_H

#include "dcmtk/config/osconfig.h"

// include this file in doxygen documentation

/** @file ofcast.h
 *  @brief DCMTK cast macros that map to C++ casts
 *
 *  DCMTK defines its own C++ cast macros that are used throughout DCMTK
 *  instead of the original C++ cast operators. Historical artefact
 *  dating back to the time when not all compilers supported the C++98 casts.
 */

#define OFconst_cast(x,y) (const_cast< x >(y))
#define OFdynamic_cast(x,y) (dynamic_cast< x >(y))
#define OFreinterpret_cast(x,y) (reinterpret_cast< x >(y))
#define OFstatic_cast(x,y) (static_cast< x >(y))

#endif
