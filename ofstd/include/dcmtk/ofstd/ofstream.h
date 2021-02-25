/*
 *
 *  Copyright (C) 2002-2019, OFFIS e.V.
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
 *  Author:  Andreas Barth
 *
 *  Purpose: C++ header to handle standard and old stream libraries.
 *
 */


#ifndef OFSTREAM_H
#define OFSTREAM_H

#include "dcmtk/config/osconfig.h"

#include <iostream>
#ifdef HAVE_IOS
#include <ios>
#endif
#include <fstream>
#include <iomanip>
// For standard STREAMS library: preference for standard stringstream
#if defined(HAVE_SSTREAM)
#include <sstream>
#define USE_STRINGSTREAM
#elif defined(HAVE_STRSTREAM)
#include <strstream>
#else
#error DCMTK needs stringstream or strstream type
#endif

/* DCMTK by default does not anymore pollute the default namespace by
 * importing namespace std. Earlier releases did this to simplify compatibility
 * with older compilers where STL classes were not consistently defined
 * in namespace std. We now have configure macros which should care for this.
 * If user code still relies on namespace std to be included, compile with
 * macro USING_STD_NAMESPACE defined.
 */
#include "dcmtk/ofstd/ofstdinc.h"

// define STD_NAMESPACE to std:: if the standard namespace exists
#ifndef STD_NAMESPACE
#ifdef HAVE_STD_NAMESPACE
#define STD_NAMESPACE std::
#else
#define STD_NAMESPACE
#endif
#endif

#define OFendl STD_NAMESPACE endl

#ifdef USE_STRINGSTREAM

typedef STD_NAMESPACE stringstream OFStringStream;
typedef STD_NAMESPACE ostringstream OFOStringStream;
typedef STD_NAMESPACE istringstream OFIStringStream;

#define OFStringStream_ends ""

#ifdef HAVE_STL_STRING
#define OFSTRINGSTREAM_GETOFSTRING(oss, strng) \
    OFString strng((oss).str());
#else
#define OFSTRINGSTREAM_GETOFSTRING(oss, strng) \
    STD_NAMESPACE string strng##__((oss).str()); \
    OFString strng(strng##__.c_str(), strng##__.length());
#endif

// The following two macros define a block structure. Please note that variables
// declared between xxx_GETSTR and xxx_FREESTR are only valid within this scope.
#define OFSTRINGSTREAM_GETSTR(oss, chptr) \
{ \
    STD_NAMESPACE string chptr##__ = (oss).str(); \
    const char *chptr = chptr##__.c_str();
#define OFSTRINGSTREAM_FREESTR(chptr) \
}

#else /* USE_STRINGSTREAM */

typedef STD_NAMESPACE strstream OFStringStream;
typedef STD_NAMESPACE ostrstream OFOStringStream;
typedef STD_NAMESPACE istrstream OFIStringStream;

#define OFStringStream_ends STD_NAMESPACE ends
#define OFSTRINGSTREAM_GETOFSTRING(oss, strng) \
    char *strng##__ = (oss).str(); \
    OFString strng(strng##__, (oss).pcount()); \
    delete[] strng##__;
// The following two macros define a block structure. Please note that variables
// declared between xxx_GETSTR and xxx_FREESTR are only valid within this scope.
#define OFSTRINGSTREAM_GETSTR(oss, chptr) \
{ \
    const char *chptr = (oss).str();
#define OFSTRINGSTREAM_FREESTR(chptr) \
    delete[] (char *)chptr; \
}

#endif /* USE_STRINGSTREAM */

// Define OFopenmode_in_nocreate as a macro that either expands
// to ios::in or to ios::in|ios::nocreate, if the historic
// nocreate flag is supported on the platform.
#if defined(HAVE_IOS_NOCREATE) && (__cplusplus < 201103L)
#define OFopenmode_in_nocreate STD_NAMESPACE ios::in|STD_NAMESPACE ios::nocreate
#else
#define OFopenmode_in_nocreate STD_NAMESPACE ios::in
#endif

#endif /* OFSTREAM_H */
