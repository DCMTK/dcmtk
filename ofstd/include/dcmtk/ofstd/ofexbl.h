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
 *      Implementing the explicit boolean type OFExplicitBool.
 *
 */

#ifndef OFEXBL_H
#define OFEXBL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftraits.h"

/** Helper class for resolving overloaded functions.
 *  OFExplicitBool can be used to restrict an overloaded function's
 *  parameter to "real" OFBool values, e.g. pointers and numeric values
 *  can NOT be converted to OFExplicitBool implicitly.
 *  @note OFBool falls back to "int" on platforms that don't support
 *   the keyword "bool". Therefore the conversion of int values to
 *   OFExplicitBool can't be prevented on those platforms.
 */
class OFExplicitBool
{
public:
#ifndef DOXYGEN
    // copy constructor
    inline OFExplicitBool( const OFExplicitBool& rhs )
    : m_Value( rhs.m_Value ) {}

    // generic constructor accepting only real OFBool values
    // distinguished via SFINAE
    template<typename Bool>
    inline OFExplicitBool( Bool value,
                           typename OFenable_if
                           <
                               OFis_same
                               <
                                    OFBool,
                                    typename OFdecay<Bool>::type
                               >::value
                           >::type* = OFnullptr )
    : m_Value( value ) {}
#else
    /** Construct an OFExplicitBool object from an explicit OFBool value.
     *  @param value either OFTrue or OFFalse.
     *  @note "explicit OFBool" is not a valid C++ type. It's just a term
     *    to explain the concept of OFExplicitBool, that is realized by
     *    SFINAE mechanisms in reality.
     */
    inline OFExplicitBool( explicit OFBool value );
#endif

    /** Implicit conversion to OFBool.
     *  This allows to use OFExplicitBool like a normal OFBool value,
     *  for example in if-statements.
     */
    inline operator OFBool() const
#ifndef DOXYGEN
    { return m_Value; }
#else
    ;
#endif

    /** Negation operator.
     *  This allows to negate OFExplicitBool like a normal OFBool value,
     *  for example to use the negated value in if-statements.
     */
    inline OFBool operator!() const
#ifndef DOXYGEN
    { return !m_Value; }
#else
    ;
#endif

private:
    /// The underlying boolean value.
    OFBool m_Value;
};

#endif // OFEXBL_H
