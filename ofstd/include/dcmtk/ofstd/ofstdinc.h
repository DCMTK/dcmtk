/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

// we don't yet support <locale>, <map>, <memory>, <numeric>, <queue>, <set>

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

// we don't yet support <typeinfo>, <valarray>, <vector>


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
#ifdef USE_STD_CXX_INCLUDES
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
#ifdef USE_STD_CXX_INCLUDES
#include <cstddef>
#elif defined(HAVE_STDDEF_H)
BEGIN_EXTERN_C
#include <stddef.h>
END_EXTERN_C
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




/*
 * CVS/RCS Log:
 * $Log: ofstdinc.h,v $
 * Revision 1.15  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2010-03-11 08:37:24  uli
 * Use _vsnprintf() on MSVC6 since it doesn't know vsnprintf().
 *
 * Revision 1.13  2007-02-19 15:16:16  meichel
 * Namespace std is not imported into the default namespace anymore,
 *   unless DCMTK is compiled with macro USING_STD_NAMESPACE defined.
 *
 * Revision 1.12  2006/08/14 16:42:02  meichel
 * Defined two new macros: STD_NAMESPACE is defined to std:: if the standard
 *   namespace exists and empty otherwise. OFendl is defined as std::endl if
 *   the standard namespace exists and as endl otherwise.
 *
 * Revision 1.11  2005/12/08 16:06:05  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2004/08/03 11:45:09  meichel
 * Added macros INCLUDE_LIBC and INCLUDE_UNISTD that correctly include libc.h
 *
 * Revision 1.9  2004/05/07 10:46:32  meichel
 * Removed unneeded semicolon, reported by gcc 3.4
 *
 * Revision 1.8  2003/12/11 13:40:22  meichel
 * Added support for including <new> or <new.h>
 *
 * Revision 1.7  2003/10/13 13:38:44  meichel
 * Activated Borland stdlib workaround for compiler versions other than 4.
 *
 * Revision 1.6  2002/12/16 16:20:45  meichel
 * Added configure test that checks if extern "C" inclusion
 *   of <math.h> fails, e.g. on HP/UX 10 and WIN32
 *
 * Revision 1.5  2002/12/11 15:54:47  meichel
 * Added empty namespace std declaration, needed on MSVC.
 *
 * Revision 1.4  2002/11/28 17:16:39  meichel
 * Including <math.h> without extern "C" on Win32 to avoid problem with MSVC5.
 *
 * Revision 1.3  2002/11/27 17:21:18  meichel
 * Fixed bug in ofstack inclusion code
 *
 * Revision 1.2  2002/11/27 12:33:34  meichel
 * Now including <strings.h> even if <string.h> is present.
 *
 * Revision 1.1  2002/11/27 11:20:52  meichel
 * Added new file ofstdinc.h that encapsulates the inclusion
 *   of old style vs. ISO C++ standard header files.
 *
 *
 */
