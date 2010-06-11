// Module:  Log4CPLUS
// File:    sleep.cxx
// Created: 5/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2009 Tad E. Smith
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

#include "dcmtk/oflog/helpers/sleep.h"

#include "dcmtk/ofstd/ofstd.h"

using namespace log4cplus;


///////////////////////////////////////////////////////////////////////////////
// public methods
///////////////////////////////////////////////////////////////////////////////

void
log4cplus::helpers::sleep(unsigned long secs)
{
    while (secs > 0) {
        secs = OFStandard::sleep(secs);
    }
}
