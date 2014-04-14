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
 *  Purpose:
 *      Implementing a subset of C++11's type_traits for old compilers.
 *
 */

#ifndef OFTRAITS_H
#define OFTRAITS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"

// use native classes if C++11 is supported
#if __cplusplus >= 201103L
#include <type_traits>

template<typename... ARGS>
using OFenable_if = std::enable_if<ARGS...>;

template<typename... ARGS>
using OFintegral_constant = std::integral_constant<ARGS...>;

template<typename... ARGS>
using OFis_signed = std::is_signed<ARGS...>;

template<typename... ARGS>
using OFis_unsigned = std::is_unsigned<ARGS...>;

using OFtrue_type = std::true_type;
using OFfalse_type = std::false_type;

#elif !defined(DOXYGEN) // fallback implementations

template<OFBool B,typename T=void>
struct OFenable_if {};

template<typename T>
struct OFenable_if<OFTrue,T> { typedef T type; };

template<typename T,T Value>
struct OFintegral_constant { static const T value = Value; };

template<typename T,T Value>
const T OFintegral_constant<T,Value>::value;

typedef OFintegral_constant<OFBool,OFTrue> OFtrue_type;
typedef OFintegral_constant<OFBool,OFFalse> OFfalse_type;

template<typename T>
struct OFis_signed : OFfalse_type {};

template<typename T>
struct OFis_unsigned : OFfalse_type {};

template<>
struct OFis_signed<Sint8> : OFtrue_type {};

template<>
struct OFis_unsigned<Uint8> : OFtrue_type {};

template<>
struct OFis_signed<Sint16> : OFtrue_type {};

template<>
struct OFis_unsigned<Uint16> : OFtrue_type {};

template<>
struct OFis_signed<Sint32> : OFtrue_type {};

template<>
struct OFis_unsigned<Uint32> : OFtrue_type {};

#ifndef OF_NO_SINT64
template<>
struct OFis_signed<Sint64> : OFtrue_type {};
#endif // OF_NO_SINT64

#ifndef OF_NO_UINT64
template<>
struct OFis_unsigned<Uint64> : OFtrue_type {};
#endif // OF_NO_UINT64

template<>
struct OFis_signed<Float32> : OFtrue_type {};

template<>
struct OFis_signed<Float64> : OFtrue_type {};

#else // for doxygen

/** Metafunction to conditionally remove functions and classes from overload
 *  resolution.
 *  This metafunction is a convenient way to leverage SFINAE to conditionally
 *  remove functions and classes from overload resolution based on type traits
 *  and to provide separate function overloads and specializations for different
 *  type traits. OFenable_if can be used as an additional function argument
 *  (not applicable to operator overloads), as a return type (not applicable to
 *  constructors and destructors), or as a class template or function template
 *  parameter.
 *  See http://en.cppreference.com/w/cpp/types/enable_if for details.
 *  @tparam B If B is OFtrue, OFenable_if has a public member typedef type, equal
 *    to T; otherwise, there is no member typedef.
 *  @tparam T The type to use for "type" if B equals OFtrue. Defaults to void.
 */
template<OFBool B,typename T=void>
struct OFenable_if {};

/** OFintegral_constant wraps a static constant of specified type.
 *  OFintegral_constant allows to manage compile-time constants as types,
 *  which allows to use them as template parameters. The value of an integral
 *  constant C can be accessed as C::value.
 *  See http://en.cppreference.com/w/cpp/types/integral_constant for details.
 *  @tparam T The integral type to use as base type for the constant, e.g. "int".
 *  @tparam Value The value of the constant, e.g. 42.
 */
template<typename T,T Value>
struct OFintegral_constant {};

/** Metafunction to determine if a type is signed.
 *  OFis_signed is based on OFintegral_constant and evaluates to OFTrue if
 *  a signed type is probed, otherwise Ois_signed<...>::value equals OFFalse.
 *  See http://en.cppreference.com/w/cpp/types/is_signed for details.
 *  @tparam T The type to inspect, e.g. "int" or "float".
 */
template<typename T>
struct OFis_signed {};

/** Metafunction to determine if a type is unsigned.
 *  OFis_unsigned is based on OFintegral_constant and evaluates to OFTrue if
 *  an unsigned type is probed, otherwise Ois_unsigned<...>::value equals OFFalse.
 *  See http://en.cppreference.com/w/cpp/types/is_unsigned for details.
 *  @tparam T The type to inspect, e.g. "unsigned int".
 */
template<typename T>
struct OFis_unsigned {};

/// An integral constant evaluating to OFTrue (OFtrue_type::value == OFTrue).
typedef unspecified OFtrue_type;

/// An integral constant evaluating to OFFalse (OFtrue_type::value == OFFalse).
typedef unspecified OFfalse_type;
#endif // C++11

#endif // OFTRAITS_H
