//  Copyright (C) 2009-2010, Vaclav Haisman. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modifica-
//  tion, are permitted provided that the following conditions are met:
//
//  1. Redistributions of  source code must  retain the above copyright  notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
//  APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
//  DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
//  OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
//  ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
//  (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef DCMTK_LOG4CPLUS_CONFIG_HXX
#define DCMTK_LOG4CPLUS_CONFIG_HXX

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdefine.h"     /* We need DCMTK_DECL_EXPORT/_IMPORT */

#ifdef oflog_EXPORTS
#  define DCMTK_INSIDE_LOG4CPLUS
#endif

#if defined (_WIN32)
#  include "dcmtk/oflog/config/win32.h"
#elif (defined(__MWERKS__) && defined(__MACOS__))
#  include "dcmtk/oflog/config/macosx.h"
#else
#  include "dcmtk/oflog/config/defines.h"
#endif

#if ! defined (DCMTK_OFLOG_UNICODE) && ! defined (DCMTK_LOG4CPLUS_HAVE_VSNPRINTF_S) \
    && ! defined (DCMTK_LOG4CPLUS_HAVE__VSNPRINTF_S) \
    && ! defined (DCMTK_LOG4CPLUS_HAVE_VSNPRINTF) \
    && ! defined (DCMTK_LOG4CPLUS_HAVE__VSNPRINTF)
#  undef DCMTK_LOG4CPLUS_USE_POOR_MANS_SNPRINTF
#  define DCMTK_LOG4CPLUS_USE_POOR_MANS_SNPRINTF
#endif

# if ! defined (DCMTK_LOG4CPLUS_WORKING_LOCALE) \
  && ! defined (DCMTK_LOG4CPLUS_WORKING_C_LOCALE) \
  && ! defined (DCMTK_LOG4CPLUS_WITH_ICONV)
# define DCMTK_LOG4CPLUS_POOR_MANS_CHCONV
#endif

#ifndef DCMTK_LOG4CPLUS_DECLSPEC_EXPORT
#define DCMTK_LOG4CPLUS_DECLSPEC_EXPORT /* empty */
#endif

#ifndef DCMTK_LOG4CPLUS_DECLSPEC_IMPORT
#define DCMTK_LOG4CPLUS_DECLSPEC_IMPORT /* empty */
#endif

#ifndef DCMTK_LOG4CPLUS_DECLSPEC_PRIVATE
#define DCMTK_LOG4CPLUS_DECLSPEC_PRIVATE /* empty */
#endif

#define DCMTK_LOG4CPLUS_PRIVATE DCMTK_LOG4CPLUS_DECLSPEC_PRIVATE

#if !defined(_WIN32)
#  define DCMTK_LOG4CPLUS_USE_BSD_SOCKETS
#  if !defined(DCMTK_LOG4CPLUS_SINGLE_THREADED)
#    define DCMTK_LOG4CPLUS_USE_PTHREADS
#  endif
#endif // !_WIN32

#if defined (DCMTK_INSIDE_LOG4CPLUS)
#  undef DCMTK_LOG4CPLUS_EXPORT
#  define DCMTK_LOG4CPLUS_EXPORT DCMTK_DECL_EXPORT
#else
#  undef DCMTK_LOG4CPLUS_EXPORT
#  define DCMTK_LOG4CPLUS_EXPORT DCMTK_DECL_IMPORT
#endif // defined (DCMTK_INSIDE_LOG4CPLUS)

#if defined (DCMTK_LOG4CPLUS_INLINES_ARE_EXPORTED) \
    && defined (DCMTK_LOG4CPLUS_BUILD_DLL)
#  define DCMTK_LOG4CPLUS_INLINE_EXPORT inline
#else
#  define DCMTK_LOG4CPLUS_INLINE_EXPORT
#endif

#if defined (DCMTK_OFLOG_UNICODE)
#  if defined (_MSC_VER) && _MSC_VER >= 1400
#    define DCMTK_LOG4CPLUS_FSTREAM_ACCEPTS_WCHAR_T
#  endif
#  if defined (_MSC_VER) && _MSC_VER >= 1600
#    define DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF8_FACET
#    define DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF16_FACET
#  endif
#endif

// C++11 stuff

#if ! defined (__has_feature)
//! __has_feature(X) is Clangs way for testing features.
//! Define it to 0 if it does not exist.
#  define __has_feature(X) 0
#endif

#if (defined (_MSC_VER) && _MSC_VER >= 1600) \
    || defined (__GXX_EXPERIMENTAL_CXX0X__)
#  define DCMTK_LOG4CPLUS_HAVE_CXX11_SUPPORT
#endif

#if defined (DCMTK_LOG4CPLUS_HAVE_CXX11_SUPPORT) \
    || __has_feature (cxx_rvalue_references)
#  define DCMTK_LOG4CPLUS_HAVE_RVALUE_REFS
#endif

#if ! defined (DCMTK_OFLOG_UNICODE) && defined (__GNUC__) && __GNUC__ >= 3
#  define DCMTK_LOG4CPLUS_FORMAT_ATTRIBUTE(archetype, format_index, first_arg_index) \
    __attribute__ ((format (archetype, format_index, first_arg_index)))
#else
#  define DCMTK_LOG4CPLUS_FORMAT_ATTRIBUTE(archetype, fmt_index, first_arg_index) \
    /* empty */
#endif

#if defined (__GNUC__) && __GNUC__ >= 3
#  define DCMTK_LOG4CPLUS_ATTRIBUTE_NORETURN __attribute__ ((__noreturn__))
#  define DCMTK_LOG4CPLUS_ATTRIBUTE_PURE __attribute__ ((__pure__))
#  define DCMTK_LOG4CPLUS_BUILTIN_EXPECT(exp, c) __builtin_expect ((exp), (c))
#else
#  define DCMTK_LOG4CPLUS_ATTRIBUTE_NORETURN /* empty */
#  define DCMTK_LOG4CPLUS_ATTRIBUTE_PURE /* empty */
#  define DCMTK_LOG4CPLUS_BUILTIN_EXPECT(exp, c) (exp)
#endif

#define DCMTK_LOG4CPLUS_LIKELY(cond) DCMTK_LOG4CPLUS_BUILTIN_EXPECT(!! (cond), 1)
#define DCMTK_LOG4CPLUS_UNLIKELY(cond) DCMTK_LOG4CPLUS_BUILTIN_EXPECT(!! (cond), 0)

#if defined (_MSC_VER)                                             \
    || (defined (__COMO__) && __COMO_VERSION__ >= 400) /* ??? */   \
    || (defined (__DMC__) && __DMC__ >= 0x700) /* ??? */           \
    || (defined (__clang__) && __clang_major__ >= 3)               \
    || (defined (__GNUC__) && (__GNUC__ >= 4                       \
            || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)))
#  define DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE
#  pragma once
#endif

#include "dcmtk/oflog/helpers/threadcf.h"

#if defined(__cplusplus)
namespace dcmtk
{
namespace log4cplus
{

//! Per thread cleanup function. Users should call this function before
//! a thread ends its execution. It frees resources allocated in thread local
//! storage. It is important only for multi-threaded static library builds
//! of log4cplus and user threads. In all other cases the clean up is provided
//! automatically by other means.
DCMTK_LOG4CPLUS_EXPORT void threadCleanup ();

} // namespace log4cplus
} // end namespace dcmtk

#endif

#endif // DCMTK_LOG4CPLUS_CONFIG_HXX
