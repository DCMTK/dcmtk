// Module:  Log4CPLUS
// File:    socketbuffer.cxx
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

#include <cstring>
#include <limits>
#include "dcmtk/oflog/helpers/sockbuff.h"
#include "dcmtk/oflog/helpers/loglog.h"

#if !defined(_WIN32)
#  include <netdb.h>
#else
#  include "dcmtk/oflog/config/windowsh.h"
#endif

#if defined (DCMTK_LOG4CPLUS_HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif

// helper methods to fix old-style casts warnings
BEGIN_EXTERN_C
static unsigned short OFntohs(unsigned short us) { return ntohs(us); }
static unsigned short OFhtons(unsigned short us) { return htons(us); }
END_EXTERN_C


namespace dcmtk {
namespace log4cplus { namespace helpers {


//////////////////////////////////////////////////////////////////////////////
// SocketBuffer ctors and dtor
//////////////////////////////////////////////////////////////////////////////

SocketBuffer::SocketBuffer(size_t maxsize_)
: maxsize(maxsize_),
  size(0),
  pos(0),
  buffer(new char[maxsize])
{
}


SocketBuffer::~SocketBuffer()
{
    delete [] buffer;
}


//////////////////////////////////////////////////////////////////////////////
// SocketBuffer methods
//////////////////////////////////////////////////////////////////////////////

unsigned char
SocketBuffer::readByte()
{
    if(pos >= maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readByte()- end of buffer reached"));
        return 0;
    }
    else if((pos + sizeof(unsigned char)) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readByte()- Attempt to read beyond end of buffer"));
        return 0;
    }

    unsigned char ret = OFstatic_cast(unsigned char, buffer[pos]);
    pos += sizeof(unsigned char);

    return ret;
}



unsigned short
SocketBuffer::readShort()
{
    if(pos >= maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readShort()- end of buffer reached"));
        return 0;
    }
    else if((pos + sizeof(unsigned short)) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readShort()- Attempt to read beyond end of buffer"));
        return 0;
    }

    unsigned short ret;
    memcpy(&ret, buffer + pos, sizeof(ret));
    ret = OFntohs(ret);
    pos += sizeof(unsigned short);

    return ret;
}



unsigned int
SocketBuffer::readInt()
{
    if(pos >= maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readInt()- end of buffer reached"));
        return 0;
    }
    else if((pos + sizeof(unsigned int)) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readInt()- Attempt to read beyond end of buffer"));
        return 0;
    }

    unsigned int ret;
    memcpy (&ret, buffer + pos, sizeof(ret));
    ret = ntohl(ret);
    pos += sizeof(unsigned int);
    
    return ret;
}


tstring
SocketBuffer::readString(unsigned char sizeOfChar)
{
    size_t strlen = readInt();
    size_t bufferLen = strlen * sizeOfChar;

    if(strlen == 0) {
        return tstring();
    }
    if(pos > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readString()- end of buffer reached"));
        return tstring();
    }

    if((pos + bufferLen) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readString()- Attempt to read beyond end of buffer"));
        bufferLen = (maxsize - 1) - pos;
        strlen = bufferLen / sizeOfChar;
    }

#ifndef DCMTK_OFLOG_UNICODE
    if(sizeOfChar == 1) {
        tstring ret(&buffer[pos], strlen);
        pos += strlen;
        return ret;
    }
    else if(sizeOfChar == 2) {
        tstring ret;
        for(tstring::size_type i=0; i<strlen; ++i) {
            unsigned short tmp = readShort();
            ret += OFstatic_cast(char, tmp < 256 ? tmp : ' ');
        }
        return ret;
    }
    else {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readString()- Invalid sizeOfChar!!!!"));
    }

#else /* DCMTK_OFLOG_UNICODE */
    if(sizeOfChar == 1) {
        STD_NAMESPACE string ret(&buffer[pos], strlen);
        pos += strlen;
        return towstring(ret);
    }
    else if(sizeOfChar == 2) {
        tstring ret;
        for(tstring::size_type i=0; i<strlen; ++i) {
            ret += OFstatic_cast(tchar, readShort());
        }
        return ret;
    }
    else {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::readString()- Invalid sizeOfChar!!!!"));
    }
#endif

    return tstring();
}



void
SocketBuffer::appendByte(unsigned char val)
{
    if((pos + sizeof(unsigned char)) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::appendByte()- Attempt to write beyond end of buffer"));
        return;
    }

    buffer[pos] = OFstatic_cast(char, val);
    pos += sizeof(unsigned char);
    size = pos;
}



void
SocketBuffer::appendShort(unsigned short val)
{
    if((pos + sizeof(unsigned short)) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::appendShort()- Attempt to write beyond end of buffer"));
        return;
    }

    unsigned short s = OFhtons(val);
    memcpy(buffer + pos, &s, sizeof (s));
    pos += sizeof(s);
    size = pos;
}



void
SocketBuffer::appendInt(unsigned int val)
{
    if((pos + sizeof(unsigned int)) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::appendInt()- Attempt to write beyond end of buffer"));
        return;
    }

    int i = htonl(val);
    memcpy(buffer + pos, &i, sizeof (i));
    pos += sizeof(i);
    size = pos;
}



void
SocketBuffer::appendString(const tstring& str)
{
    size_t const strlen = str.length();
    static size_t const sizeOfChar = sizeof (tchar) == 1 ? 1 : 2;

    if((pos + sizeof(unsigned int) + strlen * sizeOfChar) > maxsize)
    {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::appendString()-")
            DCMTK_LOG4CPLUS_TEXT(" Attempt to write beyond end of buffer"));
        return;
    }

    appendInt(OFstatic_cast(unsigned, strlen));
#ifndef DCMTK_OFLOG_UNICODE
    memcpy(&buffer[pos], str.data(), strlen);
    pos += strlen;
    size = pos;
#else
    for(tstring::size_type i=0; i<str.length(); ++i) {
        appendShort(OFstatic_cast(unsigned short, str[i]));
    }
#endif
}



void
SocketBuffer::appendBuffer(const SocketBuffer& buf)
{
    if((pos + buf.getSize()) > maxsize) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("SocketBuffer::appendBuffer()- Attempt to write beyond end of buffer"));
        return;
    }

    memcpy(&buffer[pos], buf.buffer, buf.getSize());
    pos += buf.getSize();
    size = pos;
}


} } // namespace log4cplus { namespace helpers {
} // end namespace dcmtk
