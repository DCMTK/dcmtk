/*
 *
 *  Copyright (C) 2002-2014, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: encapsulation of old style vs. ISO C++ standard includes
 *
 */

// this file is not and should not be protected against multiple inclusion

#include "dcmtk/config/osconfig.h"

/* DCMTK by default does not anymore pollute the default namespace by
 * importing namespace std. Earlier releases did this to simplify compatibility
 * with older compilers where STL classes were not consistently defined
 * in namespace std. We now have configure macros which should care for this.
 * If user code still relies on namespace std to be included, compile with
 * macro USING_STD_NAMESPACE defined.
 */
#ifdef USING_STD_NAMESPACE
namespace std { }
using namespace std;
#endif

// define STD_NAMESPACE to std:: if the standard namespace exists
#ifndef STD_NAMESPACE
#ifdef HAVE_STD_NAMESPACE
#define STD_NAMESPACE std::
#else
#define STD_NAMESPACE
#endif
#endif

/* Header files as defined in ISO/IEC 14882:1998, Section 17.4.1.2, Table 11
 * These are header files for the Standard C++ Library.
 */

// define INCLUDE_ALGORITHM to include "ofalgo.h"
#ifdef INCLUDE_ALGORITHM
#include "dcmtk/ofstd/ofalgo.h"
#endif

// we don't yet support <bitset>, <complex>, <deque>, <exception>, <functional>

// all stream related headers are handled by "ofstream.h"
#if defined(INCLUDE_IOSFWD) || defined(INCLUDE_IOSTREAM) || defined(INCLUDE_ISTREAM) || \
    defined(INCLUDE_OSTREAM) || defined(INCLUDE_IOMANIP) || defined(INCLUDE_IOS) || \
    defined(INCLUDE_FSTREAM) || defined(INCLUDE_SSTREAM)
#include "dcmtk/ofstd/ofstream.h"
#endif

// we don't yet support <iterator>, <limits>

// define INCLUDE_LIST to include "oflist.h"
#ifdef INCLUDE_LIST
#include "dcmtk/ofstd/oflist.h"
#endif

// define INCLUDE_LOCALE to include <locale> or <locale.h> if available
#ifdef INCLUDE_LOCALE
#ifdef USE_STD_CXX_INCLUDES
#include <locale>
#elif defined(HAVE_LOCALE_H)
#include <locale.h>
#endif
#endif

// we don't yet support <numeric>, <queue>, <set>

// define INCLUDE_MAP to include "ofmap.h"
#ifdef INCLUDE_MAP
#include "dcmtk/ofstd/ofmap.h"
#endif

// define INCLUDE_MEMORY to include <memory> or <memory.h> if available
#ifdef INCLUDE_MEMORY
#ifdef USE_STD_CXX_INCLUDES
#include <memory>
#elif defined(HAVE_MEMORY_H)
#include <memory.h>
#endif
#endif

// define INCLUDE_NEW to include <new> or <new.h> if available
#ifdef INCLUDE_NEW
#ifdef USE_STD_CXX_INCLUDES
#include <new>
#elif defined(HAVE_NEW_H)
#include <new.h>
#endif
#endif

// define INCLUDE_STACK to include "ofstack.h"
#ifdef INCLUDE_STACK
#include "dcmtk/ofstd/ofstack.h"
#endif

// we don't yet support <stdexcept>

// define INCLUDE_STREAMBUF to include <streambuf> or <streambuf.h> if available
#ifdef INCLUDE_STREAMBUF
#ifdef USE_STD_CXX_INCLUDES
#include <streambuf>
#elif defined(HAVE_STREAMBUF_H)
#include <streambuf.h>
#endif
#endif

// define INCLUDE_STRING to include "ofstring.h"
#ifdef INCLUDE_STRING
#include "dcmtk/ofstd/ofstring.h"
#endif

// we don't yet support <typeinfo>, <valarray>

// define INCLUDE_VECTOR to include "ofvector.h"
#ifdef INCLUDE_VECTOR
#include "dcmtk/ofstd/ofvector.h"
#endif


/* Header files as defined in ISO/IEC 14882:1998, Section 17.4.1.2, Table 12
 * These are header files for the Standard C Library.
 */

// define INCLUDE_CASSERT to include <cassert> or <assert.h> if available
#ifdef INCLUDE_CASSERT
#ifdef USE_STD_CXX_INCLUDES
#include <cassert>
#elif defined(HAVE_ASSERT_H)
BEGIN_EXTERN_C
#include <assert.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CCTYPE to include <cctype> or <ctype.h> if available
#ifdef INCLUDE_CCTYPE
#ifdef USE_STD_CXX_INCLUDES
#include <cctype>
#elif defined(HAVE_CTYPE_H)
BEGIN_EXTERN_C
#include <ctype.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CERRNO  to include <cerrno> or <errno.h> if available
#ifdef INCLUDE_CERRNO
#ifdef USE_STD_CXX_INCLUDES
#include <cerrno>
#elif defined(HAVE_ERRNO_H)
BEGIN_EXTERN_C
#include <errno.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CFLOAT  to include <cfloat> or <float.h> if available
#ifdef INCLUDE_CFLOAT
#ifdef USE_STD_CXX_INCLUDES
#include <cfloat>
#elif defined(HAVE_FLOAT_H)
BEGIN_EXTERN_C
#include <float.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CISO646 to include <ciso646> or <iso646.h> if available
#ifdef INCLUDE_CISO646
#ifdef USE_STD_CXX_INCLUDES
#include <ciso646>
#elif defined(HAVE_ISO646_H)
BEGIN_EXTERN_C
#include <iso646.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CLIMITS to include <climits> or <limits.h> if available
#ifdef INCLUDE_CLIMITS
#ifdef USE_STD_CXX_INCLUDES
#include <climits>
#elif defined(HAVE_LIMITS_H)
BEGIN_EXTERN_C
#include <limits.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CLOCALE to include <clocale> or <locale.h> if available
#ifdef INCLUDE_CLOCALE
#ifdef USE_STD_CXX_INCLUDES
#include <clocale>
#elif defined(HAVE_LOCALE_H)
BEGIN_EXTERN_C
#include <locale.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CMATH to include <cmath> or <math.h> if available
#ifdef INCLUDE_CMATH
#ifdef HAVE_CMATH
#include <cmath>
#elif defined(HAVE_MATH_H)
#ifndef INCLUDE_MATH_H_AS_CXX
// some systems use C++ language features in <math.h>
BEGIN_EXTERN_C
#endif
#include <math.h>
#ifndef INCLUDE_MATH_H_AS_CXX
END_EXTERN_C
#endif
#endif
#endif

// define INCLUDE_CSETJMP to include <csetjmp> or <setjmp.h> if available
#ifdef INCLUDE_CSETJMP
#ifdef USE_STD_CXX_INCLUDES
#include <csetjmp>
#elif defined(HAVE_SETJMP_H)
BEGIN_EXTERN_C
#include <setjmp.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSIGNAL to include <csignal> or <signal.h> if available
#ifdef INCLUDE_CSIGNAL
#ifdef USE_STD_CXX_INCLUDES
#include <csignal>
#elif defined(HAVE_SIGNAL_H)
BEGIN_EXTERN_C
#include <signal.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSTDARG to include <cstdarg> or <stdarg.h> if available
#ifdef INCLUDE_CSTDARG
#ifdef USE_STD_CXX_INCLUDES
#include <cstdarg>
#elif defined(HAVE_STDARG_H)
BEGIN_EXTERN_C
#include <stdarg.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSTDDEF to include <cstddef> or <stddef.h> if available
#ifdef INCLUDE_CSTDDEF
#ifdef HAVE_CSTDDEF
#include <cstddef>
#elif defined(HAVE_STDDEF_H)
BEGIN_EXTERN_C
#include <stddef.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSTDDINT to include <cstdint>, <stdint.h> or <sys/types.h> if available
#ifdef INCLUDE_CSTDINT
#ifdef HAVE_CSTDINT
#include <cstdint>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#elif defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#endif

// define INCLUDE_CSTDIO  to include <cstdio> or <stdio.h> if available
#ifdef INCLUDE_CSTDIO
#ifdef USE_STD_CXX_INCLUDES
#include <cstdio>
#elif defined(HAVE_STDIO_H)
BEGIN_EXTERN_C
#include <stdio.h>
END_EXTERN_C
#endif
// MSVC6 doesn't know vsnprintf(), but it does know _vsnprintf()
#if defined(_WIN32) && !defined(HAVE_VSNPRINTF)
#define vsnprintf _vsnprintf
#endif
#endif

// define INCLUDE_CSTDLIB to include <cstdlib> or <stdlib.h> if available
#ifdef INCLUDE_CSTDLIB
#ifdef USE_STD_CXX_INCLUDES
#include <cstdlib>
#elif defined(HAVE_STDLIB_H)
/* workaround for bug in Borland C++ Builder */
#ifndef __BORLANDC__
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef __BORLANDC__
END_EXTERN_C
#endif
#endif
#endif

// define INCLUDE_CSTRING to include <cstring> or <string.h> if available
#ifdef INCLUDE_CSTRING
#ifdef USE_STD_CXX_INCLUDES
#include <cstring>
#elif defined(HAVE_STRING_H)
BEGIN_EXTERN_C
#include <string.h>
END_EXTERN_C
#endif
// Some platforms define additional string functions like bzero or
// strcasecmp in <strings.h>, so we always include this file if available.
#ifdef HAVE_STRINGS_H
BEGIN_EXTERN_C
#include <strings.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CTIME to include <ctime> or <time.h> if available
#ifdef INCLUDE_CTIME
#ifdef USE_STD_CXX_INCLUDES
#include <ctime>
#elif defined(HAVE_TIME_H)
BEGIN_EXTERN_C
#include <time.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CWCTYPE to include <cwctype> or <wctype.h> if available
#ifdef INCLUDE_CWCTYPE
#ifdef USE_STD_CXX_INCLUDES
#include <cwctype>
#elif defined(HAVE_WCTYPE_H)
BEGIN_EXTERN_C
#include <wctype.h>
END_EXTERN_C
#endif
#endif


/* Headers other than ISO C++, such as BSD and Posix.1 headers */

// define INCLUDE_LIBC to include <libc.h> if available
#ifdef INCLUDE_LIBC
#ifdef HAVE_LIBC_H
#ifndef INCLUDE_LIBC_H_AS_CXX
BEGIN_EXTERN_C
#endif
#include <libc.h>
#ifndef INCLUDE_LIBC_H_AS_CXX
END_EXTERN_C
#endif
#endif
#endif

// define INCLUDE_UNISTD to include <unistd.h> if available
#ifdef INCLUDE_UNISTD
#ifdef HAVE_UNISTD_H
BEGIN_EXTERN_C
#include <unistd.h>
END_EXTERN_C
#endif
#endif
