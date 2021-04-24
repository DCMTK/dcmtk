/*
 *
 *  Copyright (C) 2021, OFFIS e.V.
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
 *  Purpose: Marking functions as deprecated that shall no longer be used
 *
 */

#ifndef OFDEPREC_H
#define OFDEPREC_H

#ifdef DOXYGEN

/*! @file ofdeprec.h
 *  @brief Defines macros for marking functions as deprecated
 *  @details #include "dcmtk/ofstd/ofdeprec.h"
 */

/*! @brief A macro for marking a function as deprecated.
 *  @details
 *  <h3>Example</h3>
 *  @code{.cpp}
 *  OFdeprecated void myFunction() {...}
 *  @endcode
 *  @details
 *  If the compiler supports function deprecation, using this macro will lead
 *  to a diagnostic message being emitted whenever the marked function is
 *  used.
 */
#define OFdeprecated <unspecified>

/*! @brief A macro for marking a function as deprecated with a custom message.
 *  @param MESSAGE the message to print as part of the diagnostic output.
 *  @details
 *  <h3>Example</h3>
 *  @code{.cpp}
 *  OFdeprecated_msg("use myNewFunction() instead") void myFunction() {...}
 *  void myNewFunction() {...}
 *  @endcode
 *  @details
 *  If the compiler supports function deprecation with a custom message, using
 *  this macro will lead to a diagnostic message including the custom message
 *  being emitted whenever the marked function is used.
 *  If the compiler does not support custom messages, it will fall back to
 *  OFdeprecated.
 */
#define OFdeprecated_msg(MESSAGE) <unspecified>

#else // DOXYGEN

#ifdef HAVE_CXX14_DEPRECATED_ATTRIBUTE
#define OFdeprecated [[deprecated]]
#elif defined(HAVE_ATTRIBUTE_DEPRECATED)
#define OFdeprecated __attribute__((deprecated))
#elif defined(HAVE_DECLSPEC_DEPRECATED)
#define OFdeprecated __declspec(deprecated)
#else
#define OFdeprecated
#endif

#ifdef HAVE_CXX14_DEPRECATED_ATTRIBUTE_MSG
#define OFdeprecated_msg(MESSAGE) [[deprecated(#MESSAGE)]]
#elif defined(HAVE_ATTRIBUTE_DEPRECATED_MSG)
#define OFdeprecated_msg(MESSAGE) __attribute__((deprecated(#MESSAGE)))
#elif defined(HAVE_DECLSPEC_DEPRECATED_MSG)
#define OFdeprecated_msg(MESSAGE) __declspec(deprecated(#MESSAGE))
#else
#define OFdeprecated_msg(MESSAGE) OFdeprecated
#endif

#endif // NOT DOXYGEN

#endif // OFDEPREC_H
