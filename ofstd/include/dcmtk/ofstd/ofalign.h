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
 *  Purpose: Implement alignment specifiers / query operators
 *           similar to C++11's alignas and alignof.
 *
 */

#ifndef OFALIGN_H
#define OFALIGN_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"

// use native functionality if C++11 is supported
#ifdef DCMTK_USE_CXX11_STL

#define OFalignof alignof
// these helper templates automatically resolve the alignment
// if a type is given and pass-through any numeric constant
template<typename T>
constexpr size_t OFalignof_or_identity_template() { return alignof(T); }
template<size_t Size>
constexpr size_t OFalignof_or_identity_template() { return Size; }
#define OFalignof_or_identity(A) OFalignof_or_identity_template<A>()

#elif !defined(DOXYGEN) // fallback implementations

// alignof
#ifdef HAVE_GNU_ALIGNOF
#define OFalignof __alignof__
#elif defined(HAVE_MS_ALIGNOF)
#define OFalignof __alignof
#endif

// these helper templates automatically resolve the alignment
// if a type is given and pass-through any numeric constant
template<size_t Size>
struct OFalignas_size_helper { Uint8 size[Size]; };
template<typename T>
OFalignas_size_helper
<
#ifdef OFalignof
    OFalignof(T)
#else // use sizeof instead
    sizeof(T)
#endif
> OFalignof_or_identity_template() {}
template<size_t Size>
OFalignas_size_helper<Size> OFalignof_or_identity_template() {}
#define OFalignof_or_identity(A) sizeof(OFalignof_or_identity_template<A>())

#else // DOXYGEN

/** @file ofalign.h
 *  Implements platform independent alignment, if possible similar to
 *  C++11's alignof / alignas functionality.
 */

/** Determine the alignment of a type T.
 *  @note OFalignof may not be available on your platform / compiler combination.
 *    Use <kbd>#ifdef OFalignof</kbd> to query availability.
 *  @details OFalignof behaves similar to
 *  <a href="http://en.cppreference.com/w/cpp/language/alignof">C++11's alignof</a>
 *  when it's supported.
 */
#define OFalignof <unspecified>

/** Align object or class like another type or as specified by an integral expression.
 *  @note OFalignas may not be available on your platform / compiler combination.
 *    Use <kbd>#ifdef OFalignas</kbd> to query availability. See OFalign for an alternative
 *    with limited functionality that supports more platforms in return.
 *  @details OFalignas behaves similar to
 *  <a href="http://en.cppreference.com/w/cpp/language/alignas">C++11's alignas</a>
 *  when it's supported.
 */
#define OFalignas <unspecified>

/** Determine a type with modified alignment for a given type T.
 *  @note OFalign may not be available on your platform / compiler combination.
 *    Use <kbd>#ifdef OFalign</kbd> to query availability.
 *  @sa OFalign_typename
 *  @details
 *  OFalign can be though of as the least common denominator of the alignment capabilities
 *  available on different platform / compiler combinations. Given a type T and an integral
 *  expression I, <kbd>OFalign(T,I)</kbd> evaluates to an appropriately aligned type corresponding
 *  to T.<br>
 *  You may also use another type to specify the desired alignment, e.g. <kbd>OFalign(T,int)</kbd>.
 *  OFalign will then calculate the alignment using <i>OFalignof</i> (if available) or use
 *  <i>sizeof()</i> as approximation.<br>
 *  To align arrays via OFalign, simply pass the array's extents within the parameter, e.g.
 *  <kbd>OFalign(char[12],float)</kbd> to align an array containing <i>12</i> chars like a float.<br>
 *  When using OFalign inside a dependant scope (i.e. inside templates), you may need
 *  to use OFalign_typename instead, e.g.
 *  @code typedef OFalign_typename(T,16) value_type; @endcode
 *  OFalign should support alignments as any power of two <= 8192 (examine your compiler's manual
 *  when in doubt).
 *  Other alignments may also be supported, but may not be available on every platform.
 *  OFalign may simply ignore your request if you specify an unsupported aligment
 *  (won't output an error). You may want to check the alignment via <i>OFalignof</i> in case
 *  it is likely to fail. Using another type as alignment specifier should always work,
 *  as the required alignment is obviously supported in that case
 *  (at least when OFalignof is available).
 */
#define OFalign <unspecified>

/** Alternate version of OFalign to be used within templates.
 *  @note OFalign_typename may not be available on your platform / compiler combination.
 *    Use <kbd>#ifdef OFalign</kbd> or <kbd>#ifdef OFalign_typename</kbd> to query availability.
 *  @sa OFalign
 *  @details
 *  OFalign_typename is an alternate version of OFalign that internally uses keywords like
 *  <i>OFTypename</i> so it can be used inside a dependant scope. See OFalign for more information.
 */
#define OFalign_typename OFTypename <unspecified>

#endif // C++11

// alignas
#if defined(DCMTK_USE_CXX11_STL) && defined(ALIGNAS_SUPPORTS_TYPEDEFS)

#define OFalignas alignas

#elif defined(HAVE_ATTRIBUTE_ALIGNED) && defined(ATTRIBUTE_ALIGNED_SUPPORTS_TEMPLATES)

#define OFalignas(A) __attribute__((aligned(OFalignof_or_identity(A))))

#elif defined(HAVE_DECLSPEC_ALIGN)

// Microsoft workaround: OFalign and OFalign_typename macros
// __declspec(align) does not understand integral expressions
// but instead requires an integral literal, Microsoft says:
// "Valid entries are integer powers of two from 1 to 8192 (bytes),
// such as 2, 4, 8, 16, 32, or 64."
// So this is fundamentally different to the real alignment specifiers
// from GNU and C++11.
#define OFalign(T,A) OFdeclspec_align<T>::as<OFalignof_or_identity(A)>::type
#define OFalign_typename(T,A) OFTypename OFdeclspec_align<T>::template as<OFalignof_or_identity(A)>::type

// The trick / hack: specialize a template for every valid
// integral expression and use an appropriate integral literal to
// define the desired type. At least Microsoft allows us to
// typedef this stuff, but the API is still fundamentally different
// to the favored "OFalignas".
// This also requires a specialization for arrays, since making
// an array from an aligned type is not the same as making an
// aligned array.
#define DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( N )\
template<typename T>\
struct make_type<T,N> { typedef __declspec(align(N)) T type; };\
template<typename T,size_t S>\
struct make_type<T[S],N> { typedef __declspec(align(N)) T type[S]; }

template<typename X>
class OFdeclspec_align
{
    template<typename T,size_t>
    struct make_type { typedef T type; };
    template<typename T,size_t S,size_t A>
    struct make_type<T[S],A> { typedef T type[S]; };
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 1 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 2 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 4 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 8 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 16 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 32 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 64 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 128 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 256 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 512 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 1024 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 2048 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 4096 );
    DCMTK_OFALIGN_HACK_CONSTANT_BY_SPECIALIZATION( 8192 );

public:
    template<size_t A>
    struct as { typedef OFTypename make_type<X,A>::type type; };
};
#endif

#if defined(OFalignas) && !defined(DOXYGEN)
// OFalign based on OFalignas, so this is "platform-independent".
#define OFalign(T,A) OFalignas_align<T>::as<OFalignof_or_identity(A)>::type
#define OFalign_typename(T,A) OFTypename OFalignas_align<T>::template as<OFalignof_or_identity(A)>::type
template<typename T>
struct OFalignas_align
{
    template<size_t A>
    struct as { typedef T type OFalignas(A); };
};
template<typename T,size_t S>
struct OFalignas_align<T[S]>
{
    template<size_t A>
    struct as { typedef T type[S] OFalignas(A); };
};
#endif

#endif // OFALIGN_H
