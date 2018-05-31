/*
 *
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Purpose: Including run time and compile time assertion features and
 *           implementing fallback implementations for the ones that are not
 *           available.
 *
 */

#ifndef OFASSERT_H
#define OFASSERT_H

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"

/** @file ofassert.h
 *  Including run time and compile time assertion features and implementing
 *  fallback implementations for the ones that are not available.
 */

#ifndef DOXYGEN
#ifdef HAVE_STATIC_ASSERT
#define OFstatic_assert static_assert
#else // HAVE_STATIC_ASSERT
template<OFBool>
class OFstatic_assert_t;
template<>
struct OFstatic_assert_t<OFTrue> { OFstatic_assert_t( const char* = OFnullptr ) {} };
#define OFstatic_assert( E, M ) OFstatic_assert_t<E>(M);
#endif // HAVE_STATIC_ASSERT
#else // DOXYGEN
/** Assert a constant expression at compile time.
 *  @param E an expression that can be evaluated as a boolean constant at
 *    compile time.
 *  @param M a message that will be displayed in case the expression evaluates
 *    to OFFalse.
 *  @details
 *  OFstatic_assert will check the expression E at compile time, emitting a
 *  compiler error if the expression evaluates to OFFalse.
 *  The statement will be ignored in case E evaluates to OFTrue, i.e. there
 *  will be no performance impact.
 *  @remarks OFstatic_assert will simply be defined as 'static_assert' if the
 *    feature is available and fallback to an implementation based on template
 *    specialization otherwise. Depending on what compiler is being used, the
 *    error emitted by the fallback implementation can become rather cryptic,
 *    e.g. not even containing the message given as 'M'.
 *  @note OFstatic_assert is a preprocessor macro. If static_assert is
 *    available, it will simply be another name for that, otherwise it will be
 *    a macro taking two arguments. This means you will have to put an
 *    expression containing one or more commas into parentheses '(' and ')',
 *    since the preprocessor will otherwise break the expression into more
 *    than one argument (at each ',').
 *  @details
 *  <h3>Usage Example</h3>
 *  @code
 *  typedef OFintegral_constant<size_t,24000> available_money;
 *  typedef OFintegral_constant<size_t,36> dependability;
 *  typedef OFintegral_constant<OFBool,(available_money::value * dependability::value > 25000000)> can_i_have_a_pony;
 *  OFstatic_assert( can_i_have_a_pony::value, "no, you can't have a pony" );
 *  @endcode
 */
#define OFstatic_assert( E, M )
#endif // DOXYGEN

#endif // OFASSERT_H
