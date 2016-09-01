/*
 *
 *  Copyright (C) 2016, OFFIS e.V.
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
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Implementing tagged unions similar to C++17's std::variant.
 *
 */

#ifndef OFVARIANT_H
#define OFVARIANT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

// Include the actual implementation (that emulates variadic templates)
#include "dcmtk/ofstd/variadic/variant.h"

/// TODO
struct OFmonostate {};

// Doxygen implementation that looks nice
#if DOXYGEN

// TODO

#endif // DOXYGEN

#endif // OFVARIANT_H
