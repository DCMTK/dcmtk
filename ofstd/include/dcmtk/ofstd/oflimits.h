/*
 *
 *  Copyright (C) 2014-2024, OFFIS e.V.
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
 *  Purpose:
 *      typedefs for numeric limits.
 *
 */
#ifndef OFLIMITS_H
#define OFLIMITS_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first
#include <limits>

template<typename T>
using OFnumeric_limits = std::numeric_limits<T>;

#endif // OFLIMITS_H
