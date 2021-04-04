/*
 *
 *  Copyright (C) 2002-2021, OFFIS e.V.
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

/* Print an error message in case some user code still uses the now unsupported
 * include macros. A clear error message is better than follow-up errors due to
 * "suddenly" missing includes.
 */

#ifdef INCLUDE_ALGORITHM
#error "Macro INCLUDE_ALGORITHM not supported anymore. Include 'dcmtk/ofstd/ofalgo.h' instead."
#endif

#ifdef INCLUDE_CASSERT
#error "Macro INCLUDE_CASSERT not supported anymore. Include <cassert> directly."
#endif

#ifdef INCLUDE_CCTYPE
#error "Macro INCLUDE_CCTYPE not supported anymore. Include <cctype> directly."
#endif

#ifdef INCLUDE_CERRNO
#error "Macro INCLUDE_CERRNO not supported anymore. Include <cerrno> directly."
#endif

#ifdef INCLUDE_CFLOAT
#error "Macro INCLUDE_CFLOAT not supported anymore. Include <cfloat> directly."
#endif

#ifdef INCLUDE_CINTTYPES
#error "Macro INCLUDE_CINTTYPES not supported anymore. Include <inttypes.h> (C99) or <cinttypes> (C++11) directly."
#endif

#ifdef INCLUDE_CISO646
#error "Macro INCLUDE_CISO646 not supported anymore. Include <ciso646> directly."
#endif

#ifdef INCLUDE_CLIMITS
#error "Macro INCLUDE_CLIMITS not supported anymore. Include <climits> directly."
#endif

#ifdef INCLUDE_CLOCALE
#error "Macro INCLUDE_CLOCALE not supported anymore. Include <clocale> directly."
#endif

#ifdef INCLUDE_CMATH
#error "Macro INCLUDE_CMATH not supported anymore. Include <cmath> directly."
#endif

#ifdef INCLUDE_CSETJMP
#error "Macro INCLUDE_CSETJMP not supported anymore. Include <csetjmp> directly."
#endif

#ifdef INCLUDE_CSIGNAL
#error "Macro INCLUDE_CSIGNAL not supported anymore. Include <csignal> directly."
#endif

#ifdef INCLUDE_CSTDARG
#error "Macro INCLUDE_CSTDARG not supported anymore. Include <cstdarg> directly."
#endif

#ifdef INCLUDE_CSTDDEF
#error "Macro INCLUDE_CSTDDEF not supported anymore. Include <cstddef> directly."
#endif

#ifdef INCLUDE_CSTDINT
#error "Macro INCLUDE_CSTDINT not supported anymore. Include <cstdint> directly."
#endif

#ifdef INCLUDE_CSTDIO
#error "Macro INCLUDE_CSTDIO not supported anymore. Include <cstdio> directly."
#endif

#ifdef INCLUDE_CSTDLIB
#error "Macro INCLUDE_CSTDLIB not supported anymore. Include <cstdlib> directly."
#endif

#ifdef INCLUDE_CSTRING
#error "Macro INCLUDE_CSTRING not supported anymore. Include <cstring> directly."
#endif

#ifdef INCLUDE_CTIME
#error "Macro INCLUDE_CTIME not supported anymore. Include <ctime> directly."
#endif

#ifdef INCLUDE_CWCTYPE
#error "Macro INCLUDE_CWCTYPE not supported anymore. Include <cwctype> directly."
#endif

#ifdef INCLUDE_FSTREAM
#error "Macro INCLUDE_FSTREAM not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_IOMANIP
#error "Macro INCLUDE_IOMANIP not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_IOS
#error "Macro INCLUDE_IOS not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_IOSFWD
#error "Macro INCLUDE_IOSFWD not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_IOSTREAM
#error "Macro INCLUDE_IOSTREAM not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_ISTREAM
#error "Macro INCLUDE_ISTREAM not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_LIBC
#error "Macro INCLUDE_LIBC not supported anymore. Include <libc.h> directly."
#endif

#ifdef INCLUDE_LIST
#error "Macro INCLUDE_LIST not supported anymore. Include 'dcmtk/ofstd/oflist.h' instead."
#endif

#ifdef INCLUDE_LOCALE
#error "Macro INCLUDE_LOCALE not supported anymore. Include <locale> directly."
#endif

#ifdef INCLUDE_MAP
#error "Macro INCLUDE_MAP not supported anymore. Include 'dcmtk/ofstd/ofmap.h' instead."
#endif

#ifdef INCLUDE_MEMORY
#error "Macro INCLUDE_MEMORY not supported anymore. Include <memory> directly."
#endif

#ifdef INCLUDE_NEW
#error "Macro INCLUDE_NEW not supported anymore. Include <new> directly."
#endif

#ifdef INCLUDE_OSTREAM
#error "Macro INCLUDE_OSTREAM not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_SSTREAM
#error "Macro INCLUDE_SSTREAM not supported anymore. Include 'dcmtk/ofstd/ofstream.h' instead."
#endif

#ifdef INCLUDE_STACK
#error "Macro INCLUDE_STACK not supported anymore. Include 'dcmtk/ofstd/ofstack.h' instead."
#endif

#ifdef INCLUDE_STREAMBUF
#error "Macro INCLUDE_STREAMBUF not supported anymore. Include <streambuf> directly."
#endif

#ifdef INCLUDE_STRING
#error "Macro INCLUDE_STRING not supported anymore. Include 'dcmtk/ofstd/ofstring.h' instead."
#endif

#ifdef INCLUDE_UNISTD
#error "Macro INCLUDE_UNISTD not supported anymore. Include <unistd.h> directly."
#endif

#ifdef INCLUDE_VECTOR
#error "Macro INCLUDE_VECTOR not supported anymore. Include 'dcmtk/ofstd/ofvector.h' instead."
#endif
