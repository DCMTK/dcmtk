// -*- C++ -*-
// Module:  Log4CPLUS
// File:    fstreams.h
// Created: 4/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2011 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** @file */

#ifndef DCMTK_LOG4CPLUS_FSTREAMS_HEADER_
#define DCMTK_LOG4CPLUS_FSTREAMS_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/tchar.h"
#include <iosfwd>


namespace dcmtk
{
namespace log4cplus
{


typedef STD_NAMESPACE basic_ofstream<tchar> tofstream;
typedef STD_NAMESPACE basic_ifstream<tchar> tifstream;

//! \def DCMTK_LOG4CPLUS_FSTREAM_PREFERED_FILE_NAME(X)
//! \brief Expands into expression that picks the right type for
//! STD_NAMESPACE fstream file name parameter.
#if defined (DCMTK_LOG4CPLUS_FSTREAM_ACCEPTS_WCHAR_T) && defined (UNICODE)
#  define DCMTK_LOG4CPLUS_FSTREAM_PREFERED_FILE_NAME(X) (X)
#else
#  define DCMTK_LOG4CPLUS_FSTREAM_PREFERED_FILE_NAME(X) (DCMTK_LOG4CPLUS_TSTRING_TO_STRING(X))
#endif


}
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_FSTREAMS_HEADER_
