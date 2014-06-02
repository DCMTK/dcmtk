/*
 *
 *  Copyright (C) 2014, OFFIS e.V.
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
 *  Purpose: Implement fallback support for modern techniques defined
 *           in the STL's <utility> header (e.g. move semantics)
 *           for older compilers.
 */

#ifndef OFUTIL_H
#define OFUTIL_H

/** @file ofutil.h
 *  Implement fallback support for modern techniques defined
 *  in the STL's <utility> header (e.g.\ move semantics)
 *  for older compilers.
 */

#if __cplusplus >= 201103L
#define OFmove std::move
#define OFswap std::swap
#else // fallback implementations
/** Obtains an rvalue reference to its argument and converts it
 *  to an xvalue. OFmove is meant to 'mark' an object for a
 *  move operation, e.g. to move an OFVector object into another
 *  OFVector instance instead of copying it.
 *  @note Currently move semantics are only supported when C++11
 *    support is available. If that's the case, OFmove is simply
 *    an alias for std::move. Otherwise OFmove has no effect
 *    and the object will still be copied.
 *  @param t The object to move.
 */
template<typename T>
#ifndef DOXYGEN
T& OFmove( T& t )
{
    return t;
}
#else // NOT DOXYGEN
OFconstexpr xvalue OFmove( T< unspecified > t );
#endif // DOXYGEN

/** Exchanges the given values.
 *  OFswap is an alias for std::swap if C++11 is supported.
 *  Otherwise OFswap simply creates a temporary copy of one
 *  argument to exchange both values.
 *  @note As intended for std::swap, there are some
 *    specializations for OFswap available, e.g. for OFoptional,
 *    which specializes OFswap to exchange optional objects
 *    more efficiently. When creating your own specializations
 *    for OFswap, make sure to specialize std::swap instead
 *    when C++11 support is available.
 *  @param t0 An object to be exchanged.
 *  @param t1 The object to be exchanged with t0.
 */
template<typename T>
void OFswap( T& t0, T& t1 )
#ifndef DOXYGEN
{
    T temp( OFmove( t0 ) );
    t0 = OFmove( t1 );
    t1 = OFmove( temp );
}
#else // NOT DOXYGEN
;
#endif // DOXYGEN
#endif // NOT C++11

#endif // OFUTIL_H
