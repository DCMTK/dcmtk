/*
 *
 *  Copyright (C) 2015-2016, OFFIS e.V.
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
 *  Purpose: Platform independent definition of several basic math
 *           utility functions.
 *
 */

// include configuration specific implementation of basic functionality
// (this also includes osconfig.h, etc.)
#include "../../config/math.cc"
// include the high-level interface definition
#include "dcmtk/ofstd/ofmath.h"

OFBool OFMath::isnan( float f )
{
  return (dcmtk_config_math::isnan)( f );
}

OFBool OFMath::isnan( double d )
{
  return (dcmtk_config_math::isnan)( d );
}

OFBool OFMath::isinf( float f )
{
  return (dcmtk_config_math::isinf)( f );
}

OFBool OFMath::isinf( double d )
{
  return (dcmtk_config_math::isinf)( d );
}
