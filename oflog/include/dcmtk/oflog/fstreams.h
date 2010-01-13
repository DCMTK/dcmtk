// Module:  Log4CPLUS
// File:    fstreams.h
// Created: 4/2003
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//

/** @file */

#ifndef LOG4CPLUS_FSTREAMS_HEADER_
#define LOG4CPLUS_FSTREAMS_HEADER_

#include "dcmtk/oflog/config.h"

//#include <fstream>
#include "dcmtk/ofstd/ofstream.h"

#if defined(__DECCXX) && !defined(__USE_STD_IOSTREAM)
#  define LOG4CPLUS_FSTREAM_NAMESPACE
#else
#  define LOG4CPLUS_FSTREAM_NAMESPACE std
#endif


namespace log4cplus {
    typedef LOG4CPLUS_FSTREAM_NAMESPACE::ofstream tofstream;
    typedef LOG4CPLUS_FSTREAM_NAMESPACE::ifstream tifstream;
}

#endif // LOG4CPLUS_FSTREAMS_HEADER_
