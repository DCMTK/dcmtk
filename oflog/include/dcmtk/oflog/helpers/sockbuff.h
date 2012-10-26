// -*- C++ -*-
// Module:  Log4CPLUS
// File:    socketbuffer.h
// Created: 5/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2010 Tad E. Smith
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

#ifndef DCMTK_LOG4CPLUS_HELPERS_SOCKET_BUFFER_HEADER_
#define DCMTK_LOG4CPLUS_HELPERS_SOCKET_BUFFER_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/tstring.h"


namespace dcmtk {
namespace log4cplus {
namespace helpers {

/**
 *
 */
class DCMTK_LOG4CPLUS_EXPORT SocketBuffer
{
public:
    explicit SocketBuffer(size_t max);
    virtual ~SocketBuffer();

    char *getBuffer() const { return buffer; }
    size_t getMaxSize() const { return maxsize; }
    size_t getSize() const { return size; }
    void setSize(size_t s) { size = s; }
    size_t getPos() const { return pos; }

    unsigned char readByte();
    unsigned short readShort();
    unsigned int readInt();
    tstring readString(unsigned char sizeOfChar);

    void appendByte(unsigned char val);
    void appendShort(unsigned short val);
    void appendInt(unsigned int val);
    void appendString(const tstring& str);
    void appendBuffer(const SocketBuffer& buffer);

private:
    // Data
    size_t maxsize;
    size_t size;
    size_t pos;
    char *buffer;

    SocketBuffer(SocketBuffer const & rhs);
    SocketBuffer& operator= (SocketBuffer const& rhs);
};

} // end namespace helpers
} // end namespace log4cplus
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_HELPERS_SOCKET_HEADER_
