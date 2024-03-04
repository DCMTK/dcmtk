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

#ifdef HAVE_CXX11

template<typename T>
using OFnumeric_limits = std::numeric_limits<T>;

#else // fallback implementation of C++11 features

template<typename T>
struct OFnumeric_limits : std::numeric_limits<T>
{
    static inline T lowest() { return (std::numeric_limits<T>::min)(); }
};
template<>
struct OFnumeric_limits<float> : std::numeric_limits<float>
{
    static inline float lowest() { return -(std::numeric_limits<float>::max)(); }
};
template<>
struct OFnumeric_limits<double> : std::numeric_limits<double>
{
    static inline double lowest() { return -(std::numeric_limits<double>::max)(); }
};

#endif // fallback implementation of C++11 features based on std::numeric_limits

#endif // OFLIMITS_H
