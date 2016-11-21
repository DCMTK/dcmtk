/*
 *
 *  Copyright (C) 2016, OFFIS e.V.
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
 *  Purpose: Platform independent macros for suppressing unwanted
 *           compiler diagnostic output i.e. 'warnings'
 *
 */

#ifndef OFDIAG_H
#define OFDIAG_H

#ifdef DOXYGEN

/*! @file ofdiag.h
 *  @brief Defines macros for controlling compiler diagnostic output, i.e. warnings.
 */

/*! @brief A macro for pushing a compiler diagnostic scope.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @details
 *  If the compiler supports diagnostic scopes, using this macro
 *  will create a new one inside which warning messages might be
 *  disabled selectively.
 *  The macro will have no effect if the compiler does not support
 *  diagnostic scopes.
 *  @remarks Currently supported compilers are GCC (version 4.6 and later, including
 *    derivates like MinGW), Clang and Visual Studio.
 *  @details
 *  <h3>Example</h3>
 *  @code{.cpp}
 *  int i = 3;
 *  unsigned ui = 8;
 *  // will potentially emit a warning, although perfectly safe
 *  if( i < ui )
 *    doStuff();
 *  // create a new diagnostic scope
 *  #include DCMTK_DIAGNOSTIC_PUSH
 *  // disable the sign mismatch warning
 *  #include DCMTK_DIAGNOSTIC_IGNORE_SIGN_COMPARE
 *  // the warning is omitted!
 *  if( i < ui )
 *    doStuff();
 *  // restore the parent diagnostic scope potentially enabling
 *  // the sign mismatch warning again
 *  #include DCMTK_DIAGNOSTIC_POP
 *  @endcode
 */
#define DCMTK_DIAGNOSTIC_PUSH <unspecified>

/*! @brief A macro for pop-ing a compiler diagnostic scope, restoring the parent scope.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_POP <unspecified>

/*! @brief A macro to disable warnings about comparing signed with unsigned values.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_SIGN_COMPARE <unspecified>

/*! @brief A macro to disable warnings about overflowing constant expressions.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_OVERFLOW <unspecified>

/*! @brief A macro to disable warnings about mixing the keywords struct and class.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS <unspecified>

/*! @brief A macro to disable warnings about using \_\_func\_\_ etc. outside of a function.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_PREDEFINED_IDENTIFIER_OUTSIDE_FUNCTION <unspecified>

/*! @brief A macro to disable warnings about implicit type conversions that may alter the value.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_IMPLICIT_CONVERSION <unspecified>

/*! @brief A macro to disable warnings about variable/type name shadowing.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks This warning is typically helpful to prevent unintended behavior, however, small
 *    structs often have members with matching names as constructor arguments (for consistent
 *    use/assignment). It makes sense to suppress this warning in that case.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_SHADOW <unspecified>

/*! @brief A macro to disable a Visual Studio specific warning about explicit template instantiation.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_NO_SUITABLE_DEFINITION_FOR_TEMPLATE_INSTANTIATION <unspecified>

/*! @brief A macro to disable the Visual Studio specific "performance warning" about implicit conversion
 *    to a boolean value.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning was meant to indicate that a non boolean value should probably be stored
 *    as a boolean value in the first place, instead of using an integer (i.e. when C programmers do C++).
 *    However, that is hardly ever the case and it is mostly and annoyingly emitted in perfectly fine code
 *    that checks if a value is not equal to zero etc., therefore, suppressing it is usually appropriate.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_PERFORMANCE_WARNING <unspecified>

#else // DOXYGEN

#define DCMTK_DIAGNOSTIC_PUSH "dcmtk/ofstd/diag/push.def"
#define DCMTK_DIAGNOSTIC_POP "dcmtk/ofstd/diag/pop.def"
#define DCMTK_DIAGNOSTIC_IGNORE_SIGN_COMPARE "dcmtk/ofstd/diag/signcmp.def"
#define DCMTK_DIAGNOSTIC_IGNORE_OVERFLOW "dcmtk/ofstd/diag/overflow.def"
#define DCMTK_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS "dcmtk/ofstd/diag/mmtag.def"
#define DCMTK_DIAGNOSTIC_IGNORE_PREDEFINED_IDENTIFIER_OUTSIDE_FUNCTION "dcmtk/ofstd/diag/piof.def"
#define DCMTK_DIAGNOSTIC_IGNORE_IMPLICIT_CONVERSION "dcmtk/ofstd/diag/cnvrsn.def"
#define DCMTK_DIAGNOSTIC_IGNORE_SHADOW "dcmtk/ofstd/diag/shadow.def"
#define DCMTK_DIAGNOSTIC_IGNORE_NO_SUITABLE_DEFINITION_FOR_TEMPLATE_INSTANTIATION "dcmtk/ofstd/diag/nsdfti.def"
#define DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_PERFORMANCE_WARNING "dcmtk/ofstd/diag/vsprfw.def"

// readable shorthands for compiler version checks
#define DCMTK_DIAGNOSTIC_MIN_GCC_VERSION(MAJOR, MINOR, PATCH)\
(\
    defined(__GNUC__) &&\
    (\
        __GNUC__ > MAJOR ||\
        (\
            __GNUC__ == MAJOR &&\
            (\
                __GNUC_MINOR__ > MINOR ||\
                (\
                    __GNUC_MINOR == MINOR &&\
                    __GNUC_PATCHLEVEL__ >= PATCH\
                )\
            )\
        )\
    )\
)

#define DCMTK_DIAGNOSTIC_MIN_MSC_VER(VERSION)\
(\
    defined(_MSC_VER) && _MSC_VER >= VERSION\
)

#endif // NOT DOXYGEN

#endif // OFDIAG_H
