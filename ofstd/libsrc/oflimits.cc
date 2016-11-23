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
 *  Purpose: Implementation of OFnumeric_limits.
 *
 */

#include "dcmtk/config/osconfig.h"

// only provide the implementation if C++11's std::numeric_limits is unavailable
#if __cplusplus < 201103L
#include "dcmtk/ofstd/oflimits.h"

const int OFnumeric_limits<char>::digits10 = OFstatic_cast( int, OFnumeric_limits<char>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<signed char>::digits10 = OFstatic_cast( int, OFnumeric_limits<signed char>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<unsigned char>::digits10 = OFstatic_cast( int, OFnumeric_limits<unsigned char>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<signed short>::digits10 = OFstatic_cast( int, OFnumeric_limits<signed short>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<unsigned short>::digits10 = OFstatic_cast( int, OFnumeric_limits<unsigned short>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<signed int>::digits10 = OFstatic_cast( int, OFnumeric_limits<signed int>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<unsigned int>::digits10 = OFstatic_cast( int, OFnumeric_limits<unsigned int>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<signed long>::digits10 = OFstatic_cast( int, OFnumeric_limits<signed long>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<unsigned long>::digits10 = OFstatic_cast( int, OFnumeric_limits<unsigned long>::digits * .30102999566398119521373889472449 );
const int OFnumeric_limits<float>::max_digits10 = OFstatic_cast( int, OFnumeric_limits<float>::digits * .30102999566398119521373889472449 + 2 );
const int OFnumeric_limits<double>::max_digits10 = OFstatic_cast( int, OFnumeric_limits<double>::digits * .30102999566398119521373889472449 + 2 );
#endif // NOT C++11
