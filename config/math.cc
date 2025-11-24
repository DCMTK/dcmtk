/*
 *
 *  Copyright (C) 2015-2025, OFFIS e.V.
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
 *  Purpose: Platform independent definition of basic functions declared
 *           in <math.h> resp. <cmath>.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include <cfloat>
#include <cmath>
#include <climits>
#include <csignal>
#include <csetjmp>
#include <iostream>

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstream.h"

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

struct dcmtk_config_math
{
  static inline OFBool isnan( float f )
  {
    return STD_NAMESPACE isnan(f);
  }

  static inline OFBool isnan( double d )
  {
    return STD_NAMESPACE isnan(d);
  }

  static inline OFBool isinf( float f )
  {
    return STD_NAMESPACE isinf( f );
  }

  static inline OFBool isinf( double d )
  {
    return STD_NAMESPACE isinf( d );
  }

  static inline double sqrt( double d )
  {
    return STD_NAMESPACE sqrt(d);
  }
};
