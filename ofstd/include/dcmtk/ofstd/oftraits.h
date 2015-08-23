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
#ifdef DCMTK_USE_CXX11_STL
#include <type_traits>

template<bool B,typename... ARGS>
using OFenable_if = std::enable_if<B,ARGS...>;

template<bool B,typename... ARGS>
using OFconditional = std::conditional<B,ARGS...>;

template<typename T,T Value>
using OFintegral_constant = std::integral_constant<T,Value>;

template<typename... ARGS>
using OFis_signed = std::is_signed<ARGS...>;

template<typename... ARGS>
using OFis_unsigned = std::is_unsigned<ARGS...>;

template<typename... ARGS>
using OFis_same = std::is_same<ARGS...>;

template<typename... ARGS>
using OFis_array = std::is_array<ARGS...>;

template<typename... ARGS>
using OFis_function = std::is_function<ARGS...>;

template<typename... ARGS>
using OFis_integral = std::is_integral<ARGS...>;

template<typename... ARGS>
using OFremove_pointer = std::remove_pointer<ARGS...>;

template<typename... ARGS>
using OFremove_reference = std::remove_reference<ARGS...>;

template<typename... ARGS>
using OFremove_cv = std::remove_cv<ARGS...>;

template<typename... ARGS>
using OFremove_const = std::remove_const<ARGS...>;

template<typename... ARGS>
using OFremove_volatile = std::remove_volatile<ARGS...>;

template<typename... ARGS>
using OFremove_extent = std::remove_extent<ARGS...>;

template<typename... ARGS>
using OFadd_pointer = std::add_pointer<ARGS...>;

template<typename... ARGS>
using OFdecay = std::decay<ARGS...>;

using OFtrue_type = std::true_type;
using OFfalse_type = std::false_type;

#elif !defined(DOXYGEN) // fallback implementations

template<OFBool B,typename T=void>
struct OFenable_if {};

template<typename T>
struct OFenable_if<OFTrue,T> { typedef T type; };

template<OFBool B,typename T,typename F>
struct OFconditional { typedef T type; };

template<typename T,typename F>
struct OFconditional<OFFalse,T,F> { typedef F type; };

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

template<typename T0,typename T1>
struct OFis_same : OFfalse_type {};

template<typename T>
struct OFis_array : OFfalse_type {};

template<typename T>
struct OFis_function : OFfalse_type {};

template<typename T>
struct OFis_integral : OFfalse_type {};

template<typename T>
struct OFremove_reference { typedef T type; };

template<typename T>
struct OFremove_const { typedef T type; };

template<typename T>
struct OFremove_volatile { typedef T type; };

template<typename T>
struct OFremove_extent { typedef T type; };

#ifdef HAVE_CXX_BOOL
template<>
struct OFis_unsigned<bool> : OFtrue_type {};
#endif

#ifndef C_CHAR_UNSIGNED
template<>
struct OFis_signed<char> : OFtrue_type {};
#else
template<>
struct OFis_unsigned<char> : OFtrue_type {};
#endif

template<>
struct OFis_signed<signed char> : OFtrue_type {};

template<>
struct OFis_unsigned<unsigned char> : OFtrue_type {};

template<>
struct OFis_signed<signed short> : OFtrue_type {};

template<>
struct OFis_unsigned<unsigned short> : OFtrue_type {};

template<>
struct OFis_signed<signed int> : OFtrue_type {};

template<>
struct OFis_unsigned<unsigned int> : OFtrue_type {};

template<>
struct OFis_signed<signed long> : OFtrue_type {};

template<>
struct OFis_unsigned<unsigned long> : OFtrue_type {};

#ifdef OFlonglong
template<>
struct OFis_signed<OFlonglong> : OFtrue_type {};
#endif

#ifdef OFulonglong
template<>
struct OFis_unsigned<OFulonglong> : OFtrue_type {};
#endif

template<>
struct OFis_signed<float> : OFtrue_type {};

template<>
struct OFis_signed<double> : OFtrue_type {};

template<typename T>
struct OFis_same<T,T> : OFtrue_type {};

template<typename T>
struct OFis_array<T[]> : OFtrue_type {};

template<typename T,size_t N>
struct OFis_array<T[N]> : OFtrue_type {};

template<typename R>
struct OFis_function<R()> : OFtrue_type {};

template<typename R>
struct OFis_function<R(...)> : OFtrue_type {};

template<typename R,typename T0>
struct OFis_function<R(T0)> : OFtrue_type {};

template<typename R,typename T0>
struct OFis_function<R(T0,...)> : OFtrue_type {};

template<typename R,typename T0,typename T1>
struct OFis_function<R(T0,T1)> : OFtrue_type {};

template<typename R,typename T0,typename T1>
struct OFis_function<R(T0,T1,...)> : OFtrue_type {};

template<typename R,typename T0,typename T1,typename T2>
struct OFis_function<R(T0,T1,T2)> : OFtrue_type {};

template<typename R,typename T0,typename T1,typename T2>
struct OFis_function<R(T0,T1,T2,...)> : OFtrue_type {};

/* TODO: handle functions with more than three arguments */

template<>
struct OFis_integral<char> : OFtrue_type {};

template<>
struct OFis_integral<signed char> : OFtrue_type {};

template<>
struct OFis_integral<unsigned char> : OFtrue_type {};

template<>
struct OFis_integral<signed short> : OFtrue_type {};

template<>
struct OFis_integral<unsigned short> : OFtrue_type {};

template<>
struct OFis_integral<signed int> : OFtrue_type {};

template<>
struct OFis_integral<unsigned int> : OFtrue_type {};

template<>
struct OFis_integral<signed long> : OFtrue_type {};

template<>
struct OFis_integral<unsigned long> : OFtrue_type {};

#ifdef OFlonglong
template<>
struct OFis_integral<OFlonglong> : OFtrue_type {};
#endif

#ifdef OFulonglong
template<>
struct OFis_integral<OFulonglong> : OFtrue_type {};
#endif

template<typename T>
struct OFremove_reference<T&> { typedef T type; };

template<typename T>
struct OFremove_const<const T> { typedef T type; };

template<typename T>
struct OFremove_volatile<volatile T> { typedef T type; };

template<typename T>
struct OFremove_extent<T[]> { typedef T type; };

template<typename T,size_t N>
struct OFremove_extent<T[N]> { typedef T type;};

template<typename T>
struct OFremove_cv
{
    typedef typename OFremove_volatile<typename OFremove_const<T>::type>::type type;
};

template< class T > struct OFremove_pointer                    {
    typedef T type;
};
template< class T > struct OFremove_pointer<T*>                {
    typedef T type;
};
template< class T > struct OFremove_pointer<T* const>          {
    typedef T type;
};
template< class T > struct OFremove_pointer<T* volatile>       {
    typedef T type;
};
template< class T > struct OFremove_pointer<T* const volatile> {
    typedef T type;
};

template<typename T>
struct OFadd_pointer
{
    typedef typename OFremove_reference<T>::type* type;
};

template<typename T>
struct OFdecay
{
    typedef typename OFconditional
    <
        OFis_array<typename OFremove_reference<T>::type>::value,
        typename OFremove_extent<typename OFremove_reference<T>::type>::type*,
        typename OFconditional
        <
            OFis_function<typename OFremove_reference<T>::type>::value,
            typename OFadd_pointer<typename OFremove_reference<T>::type>::type,
            typename OFremove_cv<typename OFremove_reference<T>::type>::type
        >::type
    >::type type;
};

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
 *  @see http://en.cppreference.com/w/cpp/types/enable_if for details.
 *  @tparam B If B is OFTrue, OFenable_if has a public member typedef type, equal
 *    to T; otherwise, there is no member typedef.
 *  @tparam T The type to use for "type" if B equals OFTrue. Defaults to void.
 */
template<OFBool B,typename T=void>
struct OFenable_if {};

/** Metafunction to select one of two types depending on a template parameter.
 *  @see http://en.cppreference.com/w/cpp/types/conditional for details.
 *  @tparam B If B is OFtrue, OFconditional has a public member typedef type, equal
 *    to T; otherwise, type equals F.
 *  @tparam T The type to use for "type" if B equals OFTrue.
 *  @tparam F The type to use for "type" if B equals OFFalse.
 */
template<OFBool B,typename T,typename F>
struct OFconditional {};

/** OFintegral_constant wraps a static constant of specified type.
 *  OFintegral_constant allows to manage compile-time constants as types,
 *  which allows to use them as template parameters. The value of an integral
 *  constant C can be accessed as C::value.
 *  @see http://en.cppreference.com/w/cpp/types/integral_constant for details.
 *  @tparam T The integral type to use as base type for the constant, e.g. "int".
 *  @tparam Value The value of the constant, e.g. 42.
 */
template<typename T,T Value>
struct OFintegral_constant {};

/** Metafunction to determine if a type is signed.
 *  OFis_signed is based on OFintegral_constant and evaluates to OFTrue if
 *  a signed type is probed, otherwise OFis_signed<...>::value equals OFFalse.
 *  @see http://en.cppreference.com/w/cpp/types/is_signed for details.
 *  @tparam T The type to inspect, e.g. "int" or "float".
 */
template<typename T>
struct OFis_signed {};

/** Metafunction to determine if a type is unsigned.
 *  OFis_unsigned is based on OFintegral_constant and evaluates to OFTrue if
 *  an unsigned type is probed, otherwise OFis_unsigned<...>::value equals OFFalse.
 *  @see http://en.cppreference.com/w/cpp/types/is_unsigned for details.
 *  @tparam T The type to inspect, e.g. "unsigned int".
 */
template<typename T>
struct OFis_unsigned {};

/** Metafunction to determine if two types are identical.
 *  OFis_same is based on OFintegral_constant and evaluates to OFTrue if
 *  two identical types are given, otherwise OFis_same<...>::value equals OFFalse.
 *  @see http://en.cppreference.com/w/cpp/types/is_same for details.
 *  @tparam T0 The first type to compare.
 *  @tparam T1 The second type to compare.
 */
template<typename T0,typename T1>
struct OFis_same {};

/** Metafunction to determine if a type is an array.
 *  OFis_array is based on OFintegral_constant and evaluates to OFTrue if
 *  an array type is probed, otherwise OFis_array<...>::value equals OFFalse.
 *  @see http://en.cppreference.com/w/cpp/types/is_array for details.
 *  @tparam T The type to inspect, e.g. "int[]".
 */
template<typename T>
struct OFis_array {};

/** Metafunction to determine if a type refers to function.
 *  OFis_function is based on OFintegral_constant and evaluates to OFTrue if
 *  a function type is probed, otherwise OFis_function<...>::value equals OFFalse.
 *  @note The current implementation only supports detecting functions taking up
 *    to three arguments.
 *  @see http://en.cppreference.com/w/cpp/types/is_function for details.
 *  @tparam T The type to inspect, e.g. "int(OFBool,const OFString&)".
 */
template<typename T>
struct OFis_function {};

/** Metafunction to determine if a type is an integer.
 *  OFis_integral is based on OFintegral_constant and evaluates to OFTrue if
 *  an integral type is probed, otherwise OFis_integral<...>::value equals OFFalse.
 *  @see http://en.cppreference.com/w/cpp/types/is_integral for details.
 *  @tparam T The type to inspect, e.g. "int" or "long".
 */
template<typename T>
struct OFis_integral {};

/** Metafunction to remove the reference from a type.
 *  OFremove_reference provides a public member typedef "type" for the type
 *  referred by the given parameter.
 *  @see http://en.cppreference.com/w/cpp/types/remove_reference for details.
 *  @tparam T The type its reference should be removed.
 */
template<typename T>
struct OFremove_reference {};

/** Metafunction to remove the const modifier from a type.
 *  OFremove_const provides a public member typedef "type" that is equal
 *  to the given parameter except existing const modifiers are removed.
 *  @see http://en.cppreference.com/w/cpp/types/remove_const for details.
 *  @tparam T The type its const modifiers should be removed.
 */
template<typename T>
struct OFremove_const {};

/** Metafunction to remove the volatile modifier from a type.
 *  OFremove_volatile provides a public member typedef "type" that is equal
 *  to the given parameter except existing volatile modifiers are removed.
 *  @see http://en.cppreference.com/w/cpp/types/remove_volatile for details.
 *  @tparam T The type its volatile modifiers should be removed.
 */
template<typename T>
struct OFremove_volatile {};

/** Metafunction to remove the extent from an array type.
 *  OFremove_extent provides a public member typedef "type" that is equal
 *  to the given parameter except existing extents are removed.
 *  @note If the parameter is a multidimensional array, only the first
 *    dimension is removed.
 *  @see http://en.cppreference.com/w/cpp/types/remove_extent for details.
 *  @tparam T The type its extents should be removed.
 */
template<typename T>
struct OFremove_extent {};

/** Metafunction to remove both the const and the volatile modifier from a type.
 *  OFremove_cv provides a public member typedef "type" that is equal
 *  to the given parameter except existing const and volatile modifiers are removed.
 *  @see http://en.cppreference.com/w/cpp/types/remove_cv for details.
 *  @tparam T The type its const and volatile modifiers should be removed.
 */
template<typename T>
struct OFremove_cv {};

/** Metafunction to add another pointer-level to a type.
 *  OFadd_pointer provides a public member typedef "type" that is equal
 *  to the given parameter except existing references are removed and instead
 *  another pointer-level is added.
 *  @see http://en.cppreference.com/w/cpp/types/add_pointer for details.
 *  @tparam T The type to add a pointer-level to.
 */
template<typename T>
struct OFadd_pointer {};

/** Metafunction to remove several modifiers from a type, for example to realize by-value
 *  parameter passing.
 *  OFdecay applies lvalue-to-rvalue, array-to-pointer, and function-to-pointer implicit
 *  conversions to the parameter type, removes cv-qualifiers, and defines the resulting type
 *  as the public member typedef "type".
 *  @see http://en.cppreference.com/w/cpp/types/decay for details.
 *  @tparam T The type to decay.
 */
template<typename T>
struct OFdecay {};

/// An integral constant evaluating to OFTrue (OFtrue_type::value == OFTrue).
typedef unspecified OFtrue_type;

/// An integral constant evaluating to OFFalse (OFtrue_type::value == OFFalse).
typedef unspecified OFfalse_type;
#endif // C++11

#endif // OFTRAITS_H
