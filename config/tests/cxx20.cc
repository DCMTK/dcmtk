/*
 *  Copyright (C) 2024, OFFIS e.V.
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
 *  Module:  config
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Presents constructs that every C++20 compliant
 *           compiler should be able to compile.
 *
 *
 *  This file is based on ax_cxx_compile_stdcxx from the GNU Autoconf project.
 *  The original license is as follows:
 *
 *  LICENSE
 *
 *   Copyright (c) 2008 Benjamin Kosnik <bkoz@redhat.com>
 *   Copyright (c) 2012 Zack Weinberg <zackw@panix.com>
 *   Copyright (c) 2013 Roy Stogner <roystgnr@ices.utexas.edu>
 *   Copyright (c) 2014, 2015 Google Inc.; contributed by Alexey Sokolov <sokolov@google.com>
 *   Copyright (c) 2015 Paul Norman <penorman@mac.com>
 *   Copyright (c) 2015 Moritz Klammler <moritz@klammler.eu>
 *   Copyright (c) 2016 Krzesimir Nowak <qdlacz@gmail.com>
 *   Copyright (c) 2019 Enji Cooper <yaneurabeya@gmail.com>
 *   Copyright (c) 2020 Jason Merrill <jason@redhat.com>
 *   Copyright (c) 2021 Joern Heusipp <osmanx@problemloesungsmaschine.de> 
 *
 *   Copying and distribution of this file, with or without modification, are
 *   permitted in any medium without royalty provided the copyright notice
 *   and this notice are preserved.  This file is offered as-is, without any
 *   warranty.
 */

// If the compiler admits that it is not ready for C++20, why torture it?
// Hopefully, this will speed up the test.

#ifndef __cplusplus

#error "This is not a C++ compiler"

#elif __cplusplus < 202002L

#error "This is not a C++20 compiler"

#else

int main()
{
  return 0;
}

namespace cxx20
{

// As C++20 supports feature test macros in the standard, there is no
// immediate need to actually test for feature availability on the
// Autoconf side.

}  // namespace cxx20

#endif  // __cplusplus <= 202002L
