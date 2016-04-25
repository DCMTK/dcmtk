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
 *      Implementing C++11's numeric limits for old compilers.
 *
 */
#ifndef OFLIMITS_H
#define OFLIMITS_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first
#include "dcmtk/config/arith.h"

#include "dcmtk/ofstd/oftypes.h"

// include this file in doxygen documentation

/** @file oflimits.h
 *  @brief Provides an interface to query properties of all fundamental numeric types.
 */

// use native classes if C++11 is supported
#if __cplusplus >= 201103L

#include <limits>
using OFfloat_round_style = std::float_round_style;
enum
{
    OFround_indeterminate       = std::round_indeterminate,
    OFround_toward_zero         = std::round_toward_zero,
    OFround_to_nearest          = std::round_to_nearest,
    OFround_toward_infinity     = std::round_toward_infinity,
    OFround_toward_neg_infinity = std::round_toward_neg_infinity
};
using OFfloat_denorm_style = std::float_denorm_style;
enum
{
    OFdenorm_indeterminate = std::denorm_indeterminate,
    OFdenorm_absent        = std::denorm_absent,
    OFdenorm_present       = std::denorm_present
};
template<typename T>
using OFnumeric_limits = std::numeric_limits<T>;

#else // fallback implementations

#define INCLUDE_CLIMITS
#define INCLUDE_CFLOAT
#define INCLUDE_CMATH
#include "dcmtk/ofstd/ofstdinc.h"

/** Enumeration constants of type OFfloat_round_style indicate the rounding style
 *  used by floating-point arithmetics whenever a result of an expression is stored
 *  in an object of a floating-point type.
 */
enum OFfloat_round_style
{
    /// Rounding style cannot be determined
    OFround_indeterminate       = -1,
    /// Rounding toward zero
    OFround_toward_zero         = 0,
    /// Rounding toward nearest representable value
    OFround_to_nearest          = 1,
    /// Rounding toward positive infinity
    OFround_toward_infinity     = 2,
    /// Rounding toward negative infinity
    OFround_toward_neg_infinity = 3
};

/** Enumeration constants of type OFfloat_denorm_style indicate support of
 *  subnormal values by floating-point types.
 */
enum OFfloat_denorm_style
{
    /// Support of subnormal values cannot be determined
    OFdenorm_indeterminate = -1,
    /// The type does not support subnormal values
    OFdenorm_absent        = 0,
    /// The type allows subnormal values
    OFdenorm_present       = 1
};

#ifdef DOXYGEN
/** A meta-template for querying various properties of fundamental types.
 *  @headerfile oflimits.h "dcmtk/ofstd/oflimits.h"
 *  @details
 *  The template OFnumeric_limits provides a standardized way to query various
 *  properties of fundamental types (e.g. the largest possible value for type
 *  int is OFnumeric_limits<int>::max()).
 *  OFnumeric_limits is compatible to C++11's std::numeric_limits.
 *  @tparam T The type to inspect.
 */
template<typename T>
struct OFnumeric_limits
{
    /** <kbd>OFTrue</kbd> for all <i>T</i> for which there exists a
     *  specialization of <kbd>OFnumeric_limits</kbd>, <kbd>OFFalse</kbd>
     *  otherwise.
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %is_specialized
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFTrue</kbd>
     *  char                    | <kbd>OFTrue</kbd>
     *  signed char             | <kbd>OFTrue</kbd>
     *  unsigned char           | <kbd>OFTrue</kbd>
     *  signed short            | <kbd>OFTrue</kbd>
     *  unsigned short          | <kbd>OFTrue</kbd>
     *  signed int              | <kbd>OFTrue</kbd>
     *  unsigned int            | <kbd>OFTrue</kbd>
     *  signed long             | <kbd>OFTrue</kbd>
     *  unsigned long           | <kbd>OFTrue</kbd>
     *  float                   | <kbd>OFTrue</kbd>
     *  double                  | <kbd>OFTrue</kbd>
     */
    static const OFBool is_specialized;

    /** <kbd>OFTrue for all signed arithmetic types <i>T</i> and
     *  <kbd>OFFalse</kbd> for the unsigned types.
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %is_signed
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <i>platform / compiler specific</i>
     *  signed char             | <kbd>OFTrue</kbd>
     *  unsigned char           | <kbd>OFFalse</kbd>
     *  signed short            | <kbd>OFTrue</kbd>
     *  unsigned short          | <kbd>OFFalse</kbd>
     *  signed int              | <kbd>OFTrue</kbd>
     *  unsigned int            | <kbd>OFFalse</kbd>
     *  signed long             | <kbd>OFTrue</kbd>
     *  unsigned long           | <kbd>OFFalse</kbd>
     *  float                   | <kbd>OFTrue</kbd>
     *  double                  | <kbd>OFTrue</kbd>
     */
    static const OFBool is_signed;

    /** <kbd>OFTrue</kbd> for all integer arithmetic types <i>T</i> and
     *  <kbd>OFFalse</kbd> otherwise.
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %is_integer
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFTrue</kbd>
     *  char                    | <kbd>OFTrue</kbd>
     *  signed char             | <kbd>OFTrue</kbd>
     *  unsigned char           | <kbd>OFTrue</kbd>
     *  signed short            | <kbd>OFTrue</kbd>
     *  unsigned short          | <kbd>OFTrue</kbd>
     *  signed int              | <kbd>OFTrue</kbd>
     *  unsigned int            | <kbd>OFTrue</kbd>
     *  signed long             | <kbd>OFTrue</kbd>
     *  unsigned long           | <kbd>OFTrue</kbd>
     *  float                   | <kbd>OFFalse</kbd>
     *  double                  | <kbd>OFFalse</kbd>
     */
    static const OFBool is_integer;

    /** <kbd>OFTrue</kbd> for all arithmetic types <i>T</i> that use exact
     *  representation.
     *  @note While all fundamental types <i>T</i> for which
     *    <kbd>OFnumeric_limits<T>::is_exact == OFTrue</kbd> are integer
     *    types, a library may define exact types that aren't integers,
     *    e.g. a rational arithmetics type representing fractions.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %is_exact
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFTrue</kbd>
     *  char                    | <kbd>OFTrue</kbd>
     *  signed char             | <kbd>OFTrue</kbd>
     *  unsigned char           | <kbd>OFTrue</kbd>
     *  signed short            | <kbd>OFTrue</kbd>
     *  unsigned short          | <kbd>OFTrue</kbd>
     *  signed int              | <kbd>OFTrue</kbd>
     *  unsigned int            | <kbd>OFTrue</kbd>
     *  signed long             | <kbd>OFTrue</kbd>
     *  unsigned long           | <kbd>OFTrue</kbd>
     *  float                   | <kbd>OFFalse</kbd>
     *  double                  | <kbd>OFFalse</kbd>
     */
    static const OFBool is_exact;

    /** <kbd>OFTrue</kbd> for all types <i>T</i> capable of representing
     *  the positive infinity as a distinct special value.
     *  @note This constant is meaningful for all
     *    floating-point types and is guaranteed to be <kbd>OFTrue</kbd> if
     *    <kbd>OFnumeric_limits<T>::is_iec559 == OFTrue</kbd>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %has_infinity
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>OFFalse</kbd>
     *  signed char             | <kbd>OFFalse</kbd>
     *  unsigned char           | <kbd>OFFalse</kbd>
     *  signed short            | <kbd>OFFalse</kbd>
     *  unsigned short          | <kbd>OFFalse</kbd>
     *  signed int              | <kbd>OFFalse</kbd>
     *  unsigned int            | <kbd>OFFalse</kbd>
     *  signed long             | <kbd>OFFalse</kbd>
     *  unsigned long           | <kbd>OFFalse</kbd>
     *  float                   | <i>usually</i> <kbd>OFTrue</kbd>
     *  double                  | <i>usually</i> <kbd>OFTrue</kbd>
     */
    static const OFBool has_infinity;

    /** <kbd>OFTrue</kbd> for all types <i>T</i> capable of representing
     *  the special value <i>Quiet Not-A-Number</i>.
     *  @note This constant is meaningful for all
     *    floating-point types and is guaranteed to be <kbd>OFTrue</kbd> if
     *    <kbd>OFnumeric_limits<T>::is_iec559 == OFTrue</kbd>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %has_quit_NaN
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>OFFalse</kbd>
     *  signed char             | <kbd>OFFalse</kbd>
     *  unsigned char           | <kbd>OFFalse</kbd>
     *  signed short            | <kbd>OFFalse</kbd>
     *  unsigned short          | <kbd>OFFalse</kbd>
     *  signed int              | <kbd>OFFalse</kbd>
     *  unsigned int            | <kbd>OFFalse</kbd>
     *  signed long             | <kbd>OFFalse</kbd>
     *  unsigned long           | <kbd>OFFalse</kbd>
     *  float                   | <i>usually</i> <kbd>OFTrue</kbd>
     *  double                  | <i>usually</i> <kbd>OFTrue</kbd>
     */
    static const OFBool has_quiet_NaN;

    /** <kbd>OFTrue</kbd> for all types <i>T</i> capable of representing
     *  the special value <i>Signaling Not-A-Number</i>.
     *  @note This constant is meaningful for all floating-point types
     *    and is guaranteed to be <kbd>OFTrue</kbd> if
     *    <kbd>OFnumeric_limits<T>::is_iec559 == OFTrue</kbd>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %has_signaling_NaN
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>OFFalse</kbd>
     *  signed char             | <kbd>OFFalse</kbd>
     *  unsigned char           | <kbd>OFFalse</kbd>
     *  signed short            | <kbd>OFFalse</kbd>
     *  unsigned short          | <kbd>OFFalse</kbd>
     *  signed int              | <kbd>OFFalse</kbd>
     *  unsigned int            | <kbd>OFFalse</kbd>
     *  signed long             | <kbd>OFFalse</kbd>
     *  unsigned long           | <kbd>OFFalse</kbd>
     *  float                   | <i>usually</i> <kbd>OFTrue</kbd>
     *  double                  | <i>usually</i> <kbd>OFTrue</kbd>
     */
    static const OFBool has_signaling_NaN;

    /** Identifies the floating-point types that support <i>subnormal values</i>.
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %has_denorm
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFdenorm_absent</kbd>
     *  OFBool                  | <kbd>OFdenorm_absent</kbd>
     *  char                    | <kbd>OFdenorm_absent</kbd>
     *  signed char             | <kbd>OFdenorm_absent</kbd>
     *  unsigned char           | <kbd>OFdenorm_absent</kbd>
     *  signed short            | <kbd>OFdenorm_absent</kbd>
     *  unsigned short          | <kbd>OFdenorm_absent</kbd>
     *  signed int              | <kbd>OFdenorm_absent</kbd>
     *  unsigned int            | <kbd>OFdenorm_absent</kbd>
     *  signed long             | <kbd>OFdenorm_absent</kbd>
     *  unsigned long           | <kbd>OFdenorm_absent</kbd>
     *  float                   | <i>usually</i> <kbd>OFdenorm_present</kbd>
     *  double                  | <i>usually</i> <kbd>OFdenorm_present</kbd>
     */
    static const OFfloat_denorm_style has_denorm;

    /** <kbd>OFTrue</kbd> for all floating-point types <i>T</i> capable of
     *  distinguishing loss of precision due to denormalization from other
     *  causes of inexact result.
     *  @note Standard-compliant IEEE 754 floating-point implementations may
     *    detect the floating-point underflow at three predefined moments:
     *    1. after computation of a result with absolute value smaller than
     *       <kbd>OFnumeric_limits<T>::min()</kbd>, such implementation detects
     *       <i>tinyness before rounding</i>.
     *    2. after rounding of the result to <kbd>OFnumeric_limits<T>::digits</kbd>
     *       bits, if the result is tiny, such implementation detects <i>tinyness
     *       after rounding</i>.
     *    3. if the conversion of the rounded tiny result to subnormal form
     *       resulted in the loss of precision, such implementation detects
     *       <i>denorm loss</i>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %has_denorm_loss
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>OFFalse</kbd>
     *  signed char             | <kbd>OFFalse</kbd>
     *  unsigned char           | <kbd>OFFalse</kbd>
     *  signed short            | <kbd>OFFalse</kbd>
     *  unsigned short          | <kbd>OFFalse</kbd>
     *  signed int              | <kbd>OFFalse</kbd>
     *  unsigned int            | <kbd>OFFalse</kbd>
     *  signed long             | <kbd>OFFalse</kbd>
     *  unsigned long           | <kbd>OFFalse</kbd>
     *  float                   | <i>platform / compiler specific</i>
     *  double                  | <i>platform / compiler specific</i>
     */
    static const OFBool has_denorm_loss;

    /** Identifies the rounding style used by the floating-point type <i>T</i> whenever
     *  a value that is not one of the exactly representable values of <i>T</i> is stored
     *  in an object of that type.
     *  <h3>C++11 standard definitions used:</h3>
     *  T                      | %round_style
     *  ---------------------- | ----------------------------
     *  <i>non-specialized</i> | <kbd>OFround_toward_zero</kbd>
     *  OFBool                 | <kbd>OFround_toward_zero</kbd>
     *  char                   | <kbd>OFround_toward_zero</kbd>
     *  signed char            | <kbd>OFround_toward_zero</kbd>
     *  unsigned char          | <kbd>OFround_toward_zero</kbd>
     *  signed short           | <kbd>OFround_toward_zero</kbd>
     *  unsigned short         | <kbd>OFround_toward_zero</kbd>
     *  signed int             | <kbd>OFround_toward_zero</kbd>
     *  unsigned int           | <kbd>OFround_toward_zero</kbd>
     *  signed long            | <kbd>OFround_toward_zero</kbd>
     *  unsigned long          | <kbd>OFround_toward_zero</kbd>
     *  float                  | <i>usually</i> <kbd>OFround_to_nearest</kbd>
     *  double                 | <i>usually</i> <kbd>OFround_to_nearest</kbd>
     */
    static const OFfloat_round_style round_style;

    /** <kbd>OFTrue</kbd> for all floating-point types <i>T</i> which fulfill the
     *  requirements of IEC 559 (IEEE 754) standard.
     *  @note If
     *    <kbd>OFnumeric_limits<T>::is_iec559 == OFTrue</kbd>,
     *    then <kbd>OFnumeric_limits<T>::has_infinity</kbd>,
     *    <kbd>OFnumeric_limits<T>::has_quiet_NaN</kbd>, and
     *    <kbd>OFnumeric_limits<T>::has_signaling_NaN</kbd> are also <kbd>OFTrue</kbd>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %is_iec559
     *  ----------------------- | ----------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>OFFalse</kbd>
     *  signed char             | <kbd>OFFalse</kbd>
     *  unsigned char           | <kbd>OFFalse</kbd>
     *  signed short            | <kbd>OFFalse</kbd>
     *  unsigned short          | <kbd>OFFalse</kbd>
     *  signed int              | <kbd>OFFalse</kbd>
     *  unsigned int            | <kbd>OFFalse</kbd>
     *  signed long             | <kbd>OFFalse</kbd>
     *  unsigned long           | <kbd>OFFalse</kbd>
     *  float                   | <i>usually</i> <kbd>OFTrue</kbd>
     *  double                  | <i>usually</i> <kbd>OFTrue</kbd>
     */
    static const OFBool is_iec559;

    /** <kbd>OFTrue</kbd> for all arithmetic types <i>T</i> that represent a
     *  finite set of values.
     *  @note While all fundamental types are bounded, this constant would be
     *    <kbd>OFFalse</kbd> in a specialization of <kbd>OFnumeric_limits</kbd>
     *    for a library-provided arbitrary precision arithmetic type.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %is_bounded
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFTrue</kbd>
     *  char                    | <kbd>OFTrue</kbd>
     *  signed char             | <kbd>OFTrue</kbd>
     *  unsigned char           | <kbd>OFTrue</kbd>
     *  signed short            | <kbd>OFTrue</kbd>
     *  unsigned short          | <kbd>OFTrue</kbd>
     *  signed int              | <kbd>OFTrue</kbd>
     *  unsigned int            | <kbd>OFTrue</kbd>
     *  signed long             | <kbd>OFTrue</kbd>
     *  unsigned long           | <kbd>OFTrue</kbd>
     *  float                   | <kbd>OFTrue</kbd>
     *  double                  | <kbd>OFTrue</kbd>
     */
    static const OFBool is_bounded;

    /** <kbd>OFTrue</kbd> for all arithmetic types <i>T</i> that handle overflows
     *  with modulo arithmetic, that is, if the result of addition, subtraction,
     *  multiplication, or division of this type would fall outside the
     *  range @f$[min(), max()]@f$, the value returned by such operation
     *  differs from the expected value by a multiple of @f$max()-min() + 1@f$.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %is_modulo
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <i>platform / compiler specific</i>
     *  signed char             | <i>platform / compiler specific</i>
     *  unsigned char           | <kbd>OFTrue</kbd>
     *  signed short            | <i>platform / compiler specific</i>
     *  unsigned short          | <kbd>OFTrue</kbd>
     *  signed int              | <i>platform / compiler specific</i>
     *  unsigned int            | <kbd>OFTrue</kbd>
     *  signed long             | <i>platform / compiler specific</i>
     *  unsigned long           | <kbd>OFTrue</kbd>
     *  float                   | <kbd>OFFalse</kbd>
     *  double                  | <kbd>OFFalse</kbd>
     */
    static const OFBool is_modulo;

    /** The number of digits in base-radix that can be represented by the
     *  type <i>T</i> without change. For integer types, this is the
     *  number of bits not counting the sign bit.
     *  For floating-point types, this is the number of digits in the
     *  mantissa.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %digits
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>1</kbd>
     *  char                    | <kbd>is_signed() ? CHAR_BIT : CHAR_BIT - 1</kbd>
     *  signed char             | <kbd>CHAR_BIT - 1</kbd>
     *  unsigned char           | <kbd>CHAR_BIT</kbd>
     *  signed short            | <kbd>CHAR_BIT * sizeof(signed short) - 1</kbd>
     *  unsigned short          | <kbd>CHAR_BIT * sizeof(unsigned short)</kbd>
     *  signed int              | <kbd>CHAR_BIT * sizeof(signed int) - 1</kbd>
     *  unsigned int            | <kbd>CHAR_BIT * sizeof(unsigned int)</kbd>
     *  signed long             | <kbd>CHAR_BIT * sizeof(signed long) - 1</kbd>
     *  unsigned long           | <kbd>CHAR_BIT * sizeof(unsigned long)</kbd>
     *  float                   | <kbd>FLT_MANT_DIG</kbd>
     *  double                  | <kbd>DBL_MANT_DIG</kbd>
     */
    static const int digits;

    /** The number of base-10 digits that can be represented by the type <i>T</i>
     *  without change, that is, any number with this many decimal digits can
     *  be converted to a value of type <i>T</i> and back to decimal form, without
     *  change due to rounding or overflow. For base-radix types, it is the
     *  value of digits (digits-1 for floating-point types) multiplied by
     *  @f$log_{10}(radix)@f$ and rounded down.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %digits10
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>0</kbd>
     *  char                    | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  signed char             | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  unsigned char           | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  signed short            | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  unsigned short          | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  signed int              | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  unsigned int            | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  signed long             | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  unsigned long           | <kbd>@f$digits \cdot log_{10}(2)@f$</kbd>
     *  float                   | <kbd>FLT_DIG</kbd>
     *  double                  | <kbd>DBL_DIG</kbd>
     */
    static const int digits10;

    /** The number of base-10 digits that are necessary to uniquely
     *  represent all distinct values of the type <i>T</i>, such as
     *  necessary for serialization/deserialization to text.
     *  @note This constant is meaningful for all floating-point
     *    types.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %max_digits10
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>0</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>@f$\lfloor digits \cdot log_{10}(2) + 2 \rfloor@f$</kbd>
     *  double                  | <kbd>@f$\lfloor digits \cdot log_{10}(2) + 2 \rfloor@f$</kbd>
     */
    static const int max_digits10;

    /** The base of the number system used in the representation of the type.
     *  It is 2 for all binary numeric types, but it may be, for example,
     *  10 for IEEE 754 decimal floating-point types or for third-party
     *  binary-coded decimal integers. This constant is meaningful for
     *  all specializations.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %radix
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>2</kbd>
     *  char                    | <kbd>2</kbd>
     *  signed char             | <kbd>2</kbd>
     *  unsigned char           | <kbd>2</kbd>
     *  signed short            | <kbd>2</kbd>
     *  unsigned short          | <kbd>2</kbd>
     *  signed int              | <kbd>2</kbd>
     *  unsigned int            | <kbd>2</kbd>
     *  signed long             | <kbd>2</kbd>
     *  unsigned long           | <kbd>2</kbd>
     *  float                   | <kbd>FLT_RADIX</kbd>
     *  double                  | <kbd>FLT_RADIX</kbd>
     */
    static const int radix;

    /** The lowest negative number @f$n@f$ such that @f$radix^{n-1}@f$ is a
     *  valid normalized value of the floating-point type <i>T</i>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %min_exponent
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>0</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>FLT_MIN_EXP</kbd>
     *  double                  | <kbd>DBL_MIN_EXP</kbd>
     */
    static const int min_exponent;

    /** The lowest negative number @f$n@f$ such that @f$10^n@f$ is a valid
     *  normalized value of the floating-point type <i>T</i>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %min_exponent10
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>0</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>FLT_MIN_10_EXP</kbd>
     *  double                  | <kbd>DBL_MIN_10_EXP</kbd>
     */
    static const int min_exponent10;

    /** The largest positive number @f$n@f$ such that @f$radix^{n-1}@f$ is a
     *  valid normalized value of the floating-point type <i>T</i>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %max_exponent
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>0</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>FLT_MAX_EXP</kbd>
     *  double                  | <kbd>DBL_MAX_EXP</kbd>
     */
    static const int max_exponent;

    /** The largest positive number @f$n@f$ such that @f$10^n@f$ is a valid
     *  normalized value of the floating-point type <i>T</i>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %max_exponent10
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>0</kbd>
     *  OFBool                  | <kbd>0</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>FLT_MAX_10_EXP</kbd>
     *  double                  | <kbd>DBL_MAX_10_EXP</kbd>
     */
    static const int max_exponent10;

    /** <kbd>OFTrue</kbd> for all arithmetic types <i>T</i> that have at least
     *  one value that, if used as an argument to an arithmetic operation, will
     *  generate a <i>trap</i>.
     *  @note On most platforms integer division by zero always traps,
     *    and <kbd>OFnumeric_limits<T>::traps</kbd> is <kbd>OFTrue</kbd> for all
     *    integer types that support the value <kbd>0</kbd>. The exception is the
     *    type <i>OFBool</i>, if support for the native type <i>bool</i> is available:
     *    even though division by <kbd>OFFalse</kbd> traps due to integral promotion
     *    from <i>OFBool</i> to <i>int</i>, it is the zero-valued <i>int</i> that traps.
     *    Zero is not a value of type <i>bool</i>.
     *  @note On most platforms, floating-point exceptions may be turned on and off at
     *    runtime, in which case the value of <kbd>OFnumeric_limits<T>::traps</kbd>
     *    for floating-point types reflects the state of floating-point trapping
     *    facility at the time of program startup.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %traps
     *  ----------------------- | ---------------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <i>usually</i> <kbd>OFFalse</kbd>
     *  char                    | <i>usually</i> <kbd>OFTrue</kbd>
     *  signed char             | <i>usually</i> <kbd>OFTrue</kbd>
     *  unsigned char           | <i>usually</i> <kbd>OFTrue</kbd>
     *  signed short            | <i>usually</i> <kbd>OFTrue</kbd>
     *  unsigned short          | <i>usually</i> <kbd>OFTrue</kbd>
     *  signed int              | <i>usually</i> <kbd>OFTrue</kbd>
     *  unsigned int            | <i>usually</i> <kbd>OFTrue</kbd>
     *  signed long             | <i>usually</i> <kbd>OFTrue</kbd>
     *  unsigned long           | <i>usually</i> <kbd>OFTrue</kbd>
     *  float                   | <i>usually</i> <kbd>OFFalse</kbd>
     *  double                  | <i>usually</i> <kbd>OFFalse</kbd>
     */
    static const OFBool traps;

    /** <kbd>OFTrue</kbd> for all floating-point types <i>T</i> that test
     *  results of floating-point expressions for underflow before rounding.
     *  @note Standard-compliant IEEE 754 floating-point implementations may
     *    detect the floating-point underflow at three predefined moments:
     *    1. after computation of a result with absolute value smaller than
     *       <kbd>OFnumeric_limits<T>::min()</kbd>, such implementation detects
     *       <i>tinyness before rounding</i>.
     *    2. after rounding of the result to <kbd>OFnumeric_limits<T>::digits</kbd>
     *       bits, if the result is tiny, such implementation detects <i>tinyness
     *       after rounding</i>.
     *    3. if the conversion of the rounded tiny result to subnormal form
     *       resulted in the loss of precision, such implementation detects
     *       <i>denorm loss</i>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %tinyness_before
     *  ----------------------- | ----------------------------------
     *  <i>non-specialized</i>  | <kbd>OFFalse</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>OFFalse</kbd>
     *  signed char             | <kbd>OFFalse</kbd>
     *  unsigned char           | <kbd>OFFalse</kbd>
     *  signed short            | <kbd>OFFalse</kbd>
     *  unsigned short          | <kbd>OFFalse</kbd>
     *  signed int              | <kbd>OFFalse</kbd>
     *  unsigned int            | <kbd>OFFalse</kbd>
     *  signed long             | <kbd>OFFalse</kbd>
     *  unsigned long           | <kbd>OFFalse</kbd>
     *  float                   | <i>platform / compiler specific</i>
     *  double                  | <i>platform / compiler specific</i>
     */
    static const OFBool tinyness_before;

    /** The minimum finite value representable by the numeric type <i>T</i>.
     *  For floating-point types with denormalization, min returns the
     *  minimum positive normalized value.
     *  @note This behavior may be unexpected, especially when compared to
     *    the behavior of min for integral types. To find the value that has
     *    no values less than it, use <kbd>OFnumeric_limits<T>::lowest()</kbd>.
     *  @note Min is only meaningful for bounded types and for unbounded
     *    unsigned types, that is, types that represent an infinite set of
     *    negative values have no meaningful minimum.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %min()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>CHAR_MIN</kbd>
     *  signed char             | <kbd>SCHAR_MIN</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>SHRT_MIN</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>INT_MIN</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>LONG_MIN</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>FLT_MIN</kbd>
     *  double                  | <kbd>DBL_MIN</kbd>
     */
    static T min();

    /** The lowest finite value representable by the numeric type <i>T</i>,
     *  that is, a finite value @f$x@f$ such that there is no other finite
     *  value @f$y@f$ where @f$y < x@f$.
     *  @note This is different from <kbd>OFnumeric_limits<T>::min()</kbd>
     *    for floating-point types.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %lowest()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>CHAR_MIN</kbd>
     *  signed char             | <kbd>SCHAR_MIN</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>SHRT_MIN</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>INT_MIN</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>LONG_MIN</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>-FLT_MAX</kbd>
     *  double                  | <kbd>-DBL_MAX</kbd>
     */
    static T lowest();

    /** The maximum finite value representable by the numeric type <i>T</i>.
     *  @note Meaningful for all bounded types.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %max()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFTrue</kbd>
     *  char                    | <kbd>CHAR_MAX</kbd>
     *  signed char             | <kbd>SCHAR_MAX</kbd>
     *  unsigned char           | <kbd>UCHAR_MAX</kbd>
     *  signed short            | <kbd>SHRT_MAX</kbd>
     *  unsigned short          | <kbd>USHRT_MAX</kbd>
     *  signed int              | <kbd>INT_MAN</kbd>
     *  unsigned int            | <kbd>UINT_MAX</kbd>
     *  signed long             | <kbd>LONG_MAX</kbd>
     *  unsigned long           | <kbd>ULONG_MAX</kbd>
     *  float                   | <kbd>FLT_MAX</kbd>
     *  double                  | <kbd>DBL_MAX</kbd>
     */
    static T max();

    /** The machine epsilon, that is, the difference between @f$1.0@f$
     *  and the next value representable by the floating-point type <i>T</i>.
     *  @note It is only meaningful if
     *    <kbd>OFnumeric_limits<T>::is_integer == OFFalse</kbd>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %epsilon()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>FLT_EPSILON</kbd>
     *  double                  | <kbd>DBL_EPSILON</kbd>
     */
    static T epsilon();

    /** The largest possible rounding error in ULPs (units in the last
     *  place) as defined by ISO 10967, which can vary from @f$0.5@f$
     *  (rounding to the nearest digit) to @f$1.0@f$ (rounding to zero or
     *  to infinity).
     *  @note It is only meaningful if
     *    <kbd>OFnumeric_limits<T>::is_integer == OFFalse</kbd>.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %round_error()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>0.5f</kbd>
     *  double                  | <kbd>0.5</kbd>
     */
    static T round_error();

    /** The special value <i>positive infinity</i>, as represented
     *  by the floating-point type <i>T</i>.
     *  @note Only meaningful if
     *    <kbd>OFnumeric_limits<T>::has_infinity == OFTrue</kbd>.
     *  @note In IEEE 754, the most common binary representation of
     *    floating-point numbers, the positive infinity is the value
     *    with all bits of the exponent set and all bits of the
     *    fraction cleared.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %infinity()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>HUGE_VALF</kbd>
     *  double                  | <kbd>HUGE_VAL</kbd>
     */
    static T infinity();

    /** The special value <i>quiet not-a-number</i>, as represented
     *  by the floating-point type <i>T</i>.
     *  @note Only meaningful if
     *    <kbd>OFnumeric_limits<T>::has_quiet_NaN == OFTrue</kbd>.
     *  @note In IEEE 754, the most common binary representation of
     *    floating-point numbers, any value with all bits of the exponent
     *    set and at least one bit of the fraction set represents a <i>NaN</i>.
     *    It is implementation-defined which values of the fraction
     *    represent quiet or signaling <i>NaN</i>s, and whether the sign bit is
     *    meaningful.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %quiet_NaN()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <kbd>NAN</kbd> or <i>platform / compiler specific</i>
     *  double                  | <i>platform / compiler specific</i>
     */
    static T quiet_NaN();

    /** The special value <i>signaling not-a-number</i>, as represented
     *  by the floating-point type <i>T</i>.
     *  @note Only meaningful if
     *    <kbd>OFnumeric_limits<T>::has_signaling_NaN == OFTrue</kbd>.
     *  @note In IEEE 754, the most common binary representation of
     *    floating-point numbers, any value with all bits of the exponent
     *    set and at least one bit of the fraction set represents a <i>NaN</i>.
     *    It is implementation-defined which values of the fraction
     *    represent quiet or signaling <i>NaN</i>s, and whether the sign bit is
     *      meaningful.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %signaling_NaN()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <i>platform / compiler specific</i>
     *  double                  | <i>platform / compiler specific</i>
     */
    static T signaling_NaN();

    /** The minimum positive subnormal value of the type <i>T</i>, if
     *  <kbd>OFnumeric_limits<T>::has_denorm != OFdenorm_absent</kbd>,
     *  otherwise returns <kbd>OFnumeric_limits<T>::min()</kbd>.
     *  @note Only meaningful for floating-point types.
     *  @details
     *  <h3>C++11 standard definitions used:</h3>
     *  T                       | %denorm_min()
     *  ----------------------- | --------------
     *  <i>non-specialized</i>  | <kbd>T()</kbd>
     *  OFBool                  | <kbd>OFFalse</kbd>
     *  char                    | <kbd>0</kbd>
     *  signed char             | <kbd>0</kbd>
     *  unsigned char           | <kbd>0</kbd>
     *  signed short            | <kbd>0</kbd>
     *  unsigned short          | <kbd>0</kbd>
     *  signed int              | <kbd>0</kbd>
     *  unsigned int            | <kbd>0</kbd>
     *  signed long             | <kbd>0</kbd>
     *  unsigned long           | <kbd>0</kbd>
     *  float                   | <i>usually</i> @f$2^{-149}@f$
     *  double                  | <i>usually</i> @f$2^{-1074}@f$
     */
    static T denorm_min();
};

#else // DOXYGEN

template<typename T>
struct OFnumeric_limits
{
    static const OFBool is_specialized              = OFFalse;
    static const OFBool is_signed                   = OFFalse;
    static const OFBool is_integer                  = OFFalse;
    static const OFBool is_exact                    = OFFalse;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFFalse;
    static const OFBool is_modulo                   = OFFalse;
    static const int digits                         = 0;
    static const int digits10                       = 0;
    static const int max_digits10                   = 0;
    static const int radix                          = 0;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = OFFalse;
    static const OFBool tinyness_before             = OFFalse;
    static inline T (min)()                         { return T(); }
    static inline T lowest()                        { return T(); }
    static inline T (max)()                         { return T(); }
    static inline T epsilon()                       { return T(); }
    static inline T round_error()                   { return T(); }
    static inline T infinity()                      { return T(); }
    static inline T quiet_NaN()                     { return T(); }
    static inline T signaling_NaN()                 { return T(); }
    static inline T denorm_min()                    { return T(); }
};

#ifdef HAVE_CXX_BOOL
template<>
struct OFnumeric_limits<bool>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFFalse;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = OFFalse;
    static const int digits                         = 1;
    static const int digits10                       = 0;
    static const int max_digits10                   = 0;
    static const int radix                          = 0;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = OFFalse;
    static const OFBool tinyness_before             = OFFalse;
    static inline bool (min)()                      { return OFFalse; }
    static inline bool lowest()                     { return OFFalse; }
    static inline bool (max)()                      { return OFTrue; }
    static inline bool epsilon()                    { return OFFalse; }
    static inline bool round_error()                { return OFFalse; }
    static inline bool infinity()                   { return OFFalse; }
    static inline bool quiet_NaN()                  { return OFFalse; }
    static inline bool signaling_NaN()              { return OFFalse; }
    static inline bool denorm_min()                 { return OFFalse; }
};
#endif

template<>
struct OFnumeric_limits<char>
{
    static const OFBool is_specialized              = OFTrue;
#ifndef C_CHAR_UNSIGNED
    static const OFBool is_signed                   = OFTrue;
#else
    static const OFBool is_signed                   = OFFalse;
#endif
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_CHAR_MODULO;
    static const int digits                         = OFnumeric_limits<char>::is_signed ? CHAR_BIT - 1 : CHAR_BIT;
    static const int digits10                       = OFstatic_cast( int, OFnumeric_limits<char>::digits * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_CHAR_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline char (min)()                      { return OFstatic_cast( char, CHAR_MIN ); }
    static inline char lowest()                     { return OFstatic_cast( char, CHAR_MIN ); }
    static inline char (max)()                      { return OFstatic_cast( char, CHAR_MAX ); }
    static inline char epsilon()                    { return OFstatic_cast( char, 0 ); }
    static inline char round_error()                { return OFstatic_cast( char, 0 ); }
    static inline char infinity()                   { return OFstatic_cast( char, 0 ); }
    static inline char quiet_NaN()                  { return OFstatic_cast( char, 0 ); }
    static inline char signaling_NaN()              { return OFstatic_cast( char, 0 ); }
    static inline char denorm_min()                 { return OFstatic_cast( char, 0 ); }
};

template<>
struct OFnumeric_limits<signed char>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFTrue;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_SIGNED_CHAR_MODULO;
    static const int digits                         = CHAR_BIT - 1;
    static const int digits10                       = OFstatic_cast( int, ( CHAR_BIT - 1 ) * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_SIGNED_CHAR_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline signed char (min)()               { return OFstatic_cast( signed char, SCHAR_MIN ); }
    static inline signed char lowest()              { return OFstatic_cast( signed char, SCHAR_MIN ); }
    static inline signed char (max)()               { return OFstatic_cast( signed char, SCHAR_MAX ); }
    static inline signed char epsilon()             { return OFstatic_cast( signed char, 0 ); }
    static inline signed char round_error()         { return OFstatic_cast( signed char, 0 ); }
    static inline signed char infinity()            { return OFstatic_cast( signed char, 0 ); }
    static inline signed char quiet_NaN()           { return OFstatic_cast( signed char, 0 ); }
    static inline signed char signaling_NaN()       { return OFstatic_cast( signed char, 0 ); }
    static inline signed char denorm_min()          { return OFstatic_cast( signed char, 0 ); }
};

template<>
struct OFnumeric_limits<unsigned char>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFFalse;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_UNSIGNED_CHAR_MODULO;
    static const int digits                         = CHAR_BIT;
    static const int digits10                       = OFstatic_cast( int, CHAR_BIT * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_UNSIGNED_CHAR_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline unsigned char (min)()             { return OFstatic_cast( unsigned char, 0 ); }
    static inline unsigned char lowest()            { return OFstatic_cast( unsigned char, 0 ); }
    static inline unsigned char (max)()             { return OFstatic_cast( unsigned char, UCHAR_MAX ); }
    static inline unsigned char epsilon()           { return OFstatic_cast( unsigned char, 0 ); }
    static inline unsigned char round_error()       { return OFstatic_cast( unsigned char, 0 ); }
    static inline unsigned char infinity()          { return OFstatic_cast( unsigned char, 0 ); }
    static inline unsigned char quiet_NaN()         { return OFstatic_cast( unsigned char, 0 ); }
    static inline unsigned char signaling_NaN()     { return OFstatic_cast( unsigned char, 0 ); }
    static inline unsigned char denorm_min()        { return OFstatic_cast( unsigned char, 0 ); }
};

template<>
struct OFnumeric_limits<signed short>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFTrue;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_SIGNED_SHORT_MODULO;
    static const int digits                         = OFstatic_cast( int, CHAR_BIT * sizeof( signed short ) - 1 );
    static const int digits10                       = OFstatic_cast( int, OFnumeric_limits<signed short>::digits * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_SIGNED_SHORT_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline signed short (min)()              { return OFstatic_cast( signed short, SHRT_MIN ); }
    static inline signed short lowest()             { return OFstatic_cast( signed short, SHRT_MIN ); }
    static inline signed short (max)()              { return OFstatic_cast( signed short, SHRT_MAX ); }
    static inline signed short epsilon()            { return OFstatic_cast( signed short, 0 ); }
    static inline signed short round_error()        { return OFstatic_cast( signed short, 0 ); }
    static inline signed short infinity()           { return OFstatic_cast( signed short, 0 ); }
    static inline signed short quiet_NaN()          { return OFstatic_cast( signed short, 0 ); }
    static inline signed short signaling_NaN()      { return OFstatic_cast( signed short, 0 ); }
    static inline signed short denorm_min()         { return OFstatic_cast( signed short, 0 ); }
};

template<>
struct OFnumeric_limits<unsigned short>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFFalse;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_UNSIGNED_SHORT_MODULO;
    static const int digits                         = OFstatic_cast( int, CHAR_BIT * sizeof( unsigned short ) );
    static const int digits10                       = OFstatic_cast( int, OFnumeric_limits<unsigned short>::digits * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_UNSIGNED_SHORT_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline unsigned short (min)()            { return OFstatic_cast( unsigned short, 0 ); }
    static inline unsigned short lowest()           { return OFstatic_cast( unsigned short, 0 ); }
    static inline unsigned short (max)()            { return OFstatic_cast( unsigned short, USHRT_MAX ); }
    static inline unsigned short epsilon()          { return OFstatic_cast( unsigned short, 0 ); }
    static inline unsigned short round_error()      { return OFstatic_cast( unsigned short, 0 ); }
    static inline unsigned short infinity()         { return OFstatic_cast( unsigned short, 0 ); }
    static inline unsigned short quiet_NaN()        { return OFstatic_cast( unsigned short, 0 ); }
    static inline unsigned short signaling_NaN()    { return OFstatic_cast( unsigned short, 0 ); }
    static inline unsigned short denorm_min()       { return OFstatic_cast( unsigned short, 0 ); }
};

template<>
struct OFnumeric_limits<signed int>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFTrue;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_SIGNED_INT_MODULO;
    static const int digits                         = OFstatic_cast( int, CHAR_BIT * sizeof( signed int ) - 1 );
    static const int digits10                       = OFstatic_cast( int, OFnumeric_limits<signed int>::digits * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_SIGNED_INT_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline signed int (min)()                { return OFstatic_cast( signed int, INT_MIN ); }
    static inline signed int lowest()               { return OFstatic_cast( signed int, INT_MIN ); }
    static inline signed int (max)()                { return OFstatic_cast( signed int, INT_MAX ); }
    static inline signed int epsilon()              { return OFstatic_cast( signed int, 0 ); }
    static inline signed int round_error()          { return OFstatic_cast( signed int, 0 ); }
    static inline signed int infinity()             { return OFstatic_cast( signed int, 0 ); }
    static inline signed int quiet_NaN()            { return OFstatic_cast( signed int, 0 ); }
    static inline signed int signaling_NaN()        { return OFstatic_cast( signed int, 0 ); }
    static inline signed int denorm_min()           { return OFstatic_cast( signed int, 0 ); }
};

template<>
struct OFnumeric_limits<unsigned int>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFFalse;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_UNSIGNED_INT_MODULO;
    static const int digits                         = OFstatic_cast( int, CHAR_BIT * sizeof( unsigned int ) );
    static const int digits10                       = OFstatic_cast( int, OFnumeric_limits<unsigned int>::digits * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_UNSIGNED_INT_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline unsigned int (min)()              { return OFstatic_cast( unsigned int, 0 ); }
    static inline unsigned int lowest()             { return OFstatic_cast( unsigned int, 0 ); }
    static inline unsigned int (max)()              { return OFstatic_cast( unsigned int, UINT_MAX ); }
    static inline unsigned int epsilon()            { return OFstatic_cast( unsigned int, 0 ); }
    static inline unsigned int round_error()        { return OFstatic_cast( unsigned int, 0 ); }
    static inline unsigned int infinity()           { return OFstatic_cast( unsigned int, 0 ); }
    static inline unsigned int quiet_NaN()          { return OFstatic_cast( unsigned int, 0 ); }
    static inline unsigned int signaling_NaN()      { return OFstatic_cast( unsigned int, 0 ); }
    static inline unsigned int denorm_min()         { return OFstatic_cast( unsigned int, 0 ); }
};

template<>
struct OFnumeric_limits<signed long>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFTrue;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_SIGNED_LONG_MODULO;
    static const int digits                         = OFstatic_cast( int, CHAR_BIT * sizeof( signed long ) - 1 );
    static const int digits10                       = OFstatic_cast( int, OFnumeric_limits<signed long>::digits * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_SIGNED_LONG_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline signed long (min)()               { return OFstatic_cast( signed long, LONG_MIN ); }
    static inline signed long lowest()              { return OFstatic_cast( signed long, LONG_MIN ); }
    static inline signed long (max)()               { return OFstatic_cast( signed long, LONG_MAX ); }
    static inline signed long epsilon()             { return OFstatic_cast( signed long, 0 ); }
    static inline signed long round_error()         { return OFstatic_cast( signed long, 0 ); }
    static inline signed long infinity()            { return OFstatic_cast( signed long, 0 ); }
    static inline signed long quiet_NaN()           { return OFstatic_cast( signed long, 0 ); }
    static inline signed long signaling_NaN()       { return OFstatic_cast( signed long, 0 ); }
    static inline signed long denorm_min()          { return OFstatic_cast( signed long, 0 ); }
};

template<>
struct OFnumeric_limits<unsigned long>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFFalse;
    static const OFBool is_integer                  = OFTrue;
    static const OFBool is_exact                    = OFTrue;
    static const OFBool has_infinity                = OFFalse;
    static const OFBool has_quiet_NaN               = OFFalse;
    static const OFBool has_signaling_NaN           = OFFalse;
    static const OFfloat_denorm_style has_denorm    = OFdenorm_absent;
    static const OFBool has_denorm_loss             = OFFalse;
    static const OFfloat_round_style round_style    = OFround_toward_zero;
    static const OFBool is_iec559                   = OFFalse;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = DCMTK_UNSIGNED_LONG_MODULO;
    static const int digits                         = OFstatic_cast( int, CHAR_BIT * sizeof( unsigned long ) );
    static const int digits10                       = OFstatic_cast( int, OFnumeric_limits<unsigned long>::digits * .30102999566398119521373889472449 );
    static const int max_digits10                   = 0;
    static const int radix                          = 2;
    static const int min_exponent                   = 0;
    static const int min_exponent10                 = 0;
    static const int max_exponent                   = 0;
    static const int max_exponent10                 = 0;
    static const OFBool traps                       = DCMTK_UNSIGNED_LONG_TRAPS;
    static const OFBool tinyness_before             = OFFalse;
    static inline unsigned long (min)()             { return OFstatic_cast( unsigned long, 0 ); }
    static inline unsigned long lowest()            { return OFstatic_cast( unsigned long, 0 ); }
    static inline unsigned long (max)()             { return OFstatic_cast( unsigned long, ULONG_MAX ); }
    static inline unsigned long epsilon()           { return OFstatic_cast( unsigned long, 0 ); }
    static inline unsigned long round_error()       { return OFstatic_cast( unsigned long, 0 ); }
    static inline unsigned long infinity()          { return OFstatic_cast( unsigned long, 0 ); }
    static inline unsigned long quiet_NaN()         { return OFstatic_cast( unsigned long, 0 ); }
    static inline unsigned long signaling_NaN()     { return OFstatic_cast( unsigned long, 0 ); }
    static inline unsigned long denorm_min()        { return OFstatic_cast( unsigned long, 0 ); }
};

template<>
struct OFnumeric_limits<float>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFTrue;
    static const OFBool is_integer                  = OFFalse;
    static const OFBool is_exact                    = OFFalse;
    static const OFBool has_infinity                = DCMTK_FLOAT_HAS_INFINITY;
    static const OFBool has_quiet_NaN               = DCMTK_FLOAT_HAS_QUIET_NAN;
    static const OFBool has_signaling_NaN           = DCMTK_FLOAT_HAS_SIGNALING_NAN;
    static const OFfloat_denorm_style has_denorm    = DCMTK_FLOAT_HAS_DENORM;
    static const OFBool has_denorm_loss             = DCMTK_FLOAT_HAS_DENORM_LOSS;
    static const OFfloat_round_style round_style    = OFstatic_cast( OFfloat_round_style, DCMTK_ROUND_STYLE );
    static const OFBool is_iec559                   = DCMTK_FLOAT_IS_IEC559;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = OFFalse;
    static const int digits                         = OFstatic_cast( int, FLT_MANT_DIG );
    static const int digits10                       = OFstatic_cast( int, FLT_DIG );
    static const int max_digits10                   = OFstatic_cast( int, OFnumeric_limits<float>::digits * .30102999566398119521373889472449  + 2 );
    static const int radix                          = FLT_RADIX;
    static const int min_exponent                   = FLT_MIN_EXP;
    static const int min_exponent10                 = FLT_MIN_10_EXP;
    static const int max_exponent                   = FLT_MAX_EXP;
    static const int max_exponent10                 = FLT_MAX_10_EXP;
    static const OFBool traps                       = DCMTK_FLOAT_TRAPS;
    static const OFBool tinyness_before             = DCMTK_FLOAT_TINYNESS_BEFORE;
    static inline float (min)()                     { return OFstatic_cast( float, FLT_MIN ); }
    static inline float lowest()                    { return OFstatic_cast( float, -FLT_MAX ); }
    static inline float (max)()                     { return OFstatic_cast( float, FLT_MAX ); }
    static inline float epsilon()                   { return OFstatic_cast( float, FLT_EPSILON ); }
    static inline float round_error()               { return OFstatic_cast( float, 0.5f ); }
    static inline float infinity()                  { return DCMTK_FLOAT_INFINITY; }
    static inline float quiet_NaN()                 { return DCMTK_FLOAT_QUIET_NAN; }
    static inline const float& signaling_NaN()      { return DCMTK_FLOAT_SIGNALING_NAN; }
    static inline float denorm_min()                { return DCMTK_FLOAT_DENORM_MIN; }
};

template<>
struct OFnumeric_limits<double>
{
    static const OFBool is_specialized              = OFTrue;
    static const OFBool is_signed                   = OFTrue;
    static const OFBool is_integer                  = OFFalse;
    static const OFBool is_exact                    = OFFalse;
    static const OFBool has_infinity                = DCMTK_DOUBLE_HAS_INFINITY;
    static const OFBool has_quiet_NaN               = DCMTK_DOUBLE_HAS_QUIET_NAN;
    static const OFBool has_signaling_NaN           = DCMTK_DOUBLE_HAS_SIGNALING_NAN;
    static const OFfloat_denorm_style has_denorm    = DCMTK_DOUBLE_HAS_DENORM;
    static const OFBool has_denorm_loss             = DCMTK_DOUBLE_HAS_DENORM_LOSS;
    static const OFfloat_round_style round_style    = OFstatic_cast( OFfloat_round_style, DCMTK_ROUND_STYLE );
    static const OFBool is_iec559                   = DCMTK_DOUBLE_IS_IEC559;
    static const OFBool is_bounded                  = OFTrue;
    static const OFBool is_modulo                   = OFFalse;
    static const int digits                         = OFstatic_cast( int, DBL_MANT_DIG );
    static const int digits10                       = OFstatic_cast( int, DBL_DIG );
    static const int max_digits10                   = OFstatic_cast( int, OFnumeric_limits<double>::digits * .30102999566398119521373889472449  + 2 );
    static const int radix                          = FLT_RADIX;
    static const int min_exponent                   = DBL_MIN_EXP;
    static const int min_exponent10                 = DBL_MIN_10_EXP;
    static const int max_exponent                   = DBL_MAX_EXP;
    static const int max_exponent10                 = DBL_MAX_10_EXP;
    static const OFBool traps                       = DCMTK_DOUBLE_TRAPS;
    static const OFBool tinyness_before             = DCMTK_DOUBLE_TINYNESS_BEFORE;
    static inline double (min)()                    { return OFstatic_cast( double, DBL_MIN ); }
    static inline double lowest()                   { return OFstatic_cast( double, -DBL_MAX ); }
    static inline double (max)()                    { return OFstatic_cast( double, DBL_MAX ); }
    static inline double epsilon()                  { return OFstatic_cast( double, DBL_EPSILON ); }
    static inline double round_error()              { return OFstatic_cast( double, 0.5 ); }
    static inline double infinity()                 { return DCMTK_DOUBLE_INFINITY; }
    static inline double quiet_NaN()                { return DCMTK_DOUBLE_QUIET_NAN; }
    static inline double signaling_NaN()            { return DCMTK_DOUBLE_SIGNALING_NAN; }
    static inline double denorm_min()               { return DCMTK_DOUBLE_DENORM_MIN; }
};
#endif // DOXYGEN

#endif // C++11

#endif // OFLIMITS_H
