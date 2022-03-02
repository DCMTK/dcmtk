/*
 *
 *  Copyright (C) 2016-2022, OFFIS e.V.
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

/*! @brief A macro to disable warnings about a conditional expression being constant
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning indicates that a conditional expression will always yield
 *    the same result. In C++17, the "if constexpr" statement should be used in this case,
 *    but DCMTK does not (yet) expect a fully C++17 compliant compiler.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_CONST_EXPRESSION_WARNING <unspecified>

/*! @brief A macro to disable warnings about a self-assignment of a variable
 *  using an overloaded operator.
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_SELF_ASSIGN_OVERLOADED_WARNING <unspecified>

/*! @brief A macro to disable warnings about an assignment within a conditional expression
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning indicates that an assignment takes place within a
 *    conditional expression, which might indicate that "=" was used erroneously instead of "==".
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_ASSIGNMENT_IN_COND_EXPRESSION_WARNING <unspecified>

/*! @brief A macro to disable warnings about raw memory access to class structures
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning indicates that the destination of a call to a raw memory
 *    function such as memset or memcpy is an object of class type, and when writing
 *    into such an object might bypass the class non-trivial or deleted constructor
 *    or copy assignment, violate const-correctness or encapsulation, or corrupt
 *    virtual table pointers.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_CLASS_MEMACCESS_WARNING <unspecified>

/*! @brief A macro to disable warnings about pessimizing use of std::move().
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning indicates that moving a local object in a return
 *    statement prevents copy elision and thus causes less efficient code
 *    to be generated.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_PESSIMIZING_MOVE_WARNING <unspecified>

/*! @brief A macro to disable warnings about code that breaks strict
 *  aliasing rules
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning indicates that certain code constructs will
 *    break strict aliasing rules, such as dereferencing a type-punned pointer.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_STRICT_ALIASING_WARNING <unspecified>

/*! @brief A macro to disable Visual Studio warnings about structure
 *    padding due to __declspec(align).
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning indicates that a certain structure has been
 *    padded at the end because of a __declspec(align) declaration.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_DECLSPEC_PADDING_WARNING <unspecified>

/*! @brief A macro to disable Visual Studio warnings about non-portability
 *    of the interaction between a function like setjmp and C++ object destruction
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @remarks The warning indicates that on some platforms, functions
 *    that include 'catch' may not support C++ object semantics of
 *    destruction when out of scope.
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_OBJECT_DESTRUCTION_WARNING <unspecified>

/*! @brief A macro to disable warnings about unreachable code
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_UNREACHABLE_CODE_WARNING <unspecified>

/*! @brief A macro to disable warnings about ignored attributes due to attribute redeclaration
 *  @note The macro may only be used in conjunction with <i>#include</i>!
 *  @see DCMTK_DIAGNOSTIC_PUSH for an example.
 */
#define DCMTK_DIAGNOSTIC_IGNORE_ATTRIBUTE_REDECLARATION <unspecified>

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
#define DCMTK_DIAGNOSTIC_IGNORE_CONST_EXPRESSION_WARNING "dcmtk/ofstd/diag/constexp.def"
#define DCMTK_DIAGNOSTIC_IGNORE_SELF_ASSIGN_OVERLOADED_WARNING "dcmtk/ofstd/diag/slfassov.def"
#define DCMTK_DIAGNOSTIC_IGNORE_ASSIGNMENT_IN_COND_EXPRESSION_WARNING "dcmtk/ofstd/diag/asincond.def"
#define DCMTK_DIAGNOSTIC_IGNORE_CLASS_MEMACCESS_WARNING "dcmtk/ofstd/diag/memacces.def"
#define DCMTK_DIAGNOSTIC_IGNORE_PESSIMIZING_MOVE_WARNING "dcmtk/ofstd/diag/pessmove.def"
#define DCMTK_DIAGNOSTIC_IGNORE_STRICT_ALIASING_WARNING "dcmtk/ofstd/diag/stralias.def"
#define DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_DECLSPEC_PADDING_WARNING "dcmtk/ofstd/diag/vsdeclpd.def"
#define DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_OBJECT_DESTRUCTION_WARNING "dcmtk/ofstd/diag/vsobjdes.def"
#define DCMTK_DIAGNOSTIC_IGNORE_UNREACHABLE_CODE_WARNING "dcmtk/ofstd/diag/unreachb.def"
#define DCMTK_DIAGNOSTIC_IGNORE_ATTRIBUTE_REDECLARATION "dcmtk/ofstd/diag/ignrattr.def"

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
