/*
 *
 *  Copyright (C) 2003-2023, OFFIS e.V.
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
 *  @brief DCMTK cast macros that map to C++ casts if available
 *
 *  DCMTK defines its own C++ cast macros that are used throughout DCMTK
 *  instead of the original C++ cast operators. If the C++ cast operators
 *  are available on the platform the macros directly map to these. Otherwise
 *  they map to old C-style casts.
 *
 *  In this context, DCMTK has defined a macro OFdynamic_cast that maps to C++'
 *  dynamic_cast. However, so far dynamic casts are not used within DCMTK since
 *  they rely on C++' RTTI feature which is deliberately avoided in DCMTK at
 *  this moment. Thus it is highly recommended to not use dynamic casts in
 *  DCMTK at all.
 */

#ifdef HAVE_CONST_CAST
#define OFconst_cast(x,y) (const_cast< x >(y))
#else
#define OFconst_cast(x,y) ((x)(y))
#endif

// OFdynamic_cast should not be used in DCMTK since DCMTK
// avoids RTTI usage so far.
#ifdef HAVE_DYNAMIC_CAST
#define OFdynamic_cast(x,y) (dynamic_cast< x >(y))
#else
#error Your C++ compiler does not support dynamic_cast(), if you want to use this, report to bugs@dcmtk.org and/or see fix in ofcast.h
//#define OFdynamic_cast(x,y) ((x)(y)) // uncomment for using old-style cast instead
#endif

#ifdef HAVE_REINTERPRET_CAST
#define OFreinterpret_cast(x,y) (reinterpret_cast< x >(y))
#else
#error Your C++ compiler does not support reinterpret_cast(), if you want to use this, report to bugs@dcmtk.org and/or see fix in ofcast.h
//#define OFreinterpret_cast(x,y) ((x)(y)) // uncomment for using old-style cast instead
#endif

#ifdef HAVE_STATIC_CAST
#define OFstatic_cast(x,y) (static_cast< x >(y))
#else
#error Your C++ compiler does not support static_cast(), if you want to use this, report to bugs@dcmtk.org and/or see fix in ofcast.h
// #define OFstatic_cast(x,y) ((x)(y)) // uncomment for using old-style cast instead
#endif

#endif
