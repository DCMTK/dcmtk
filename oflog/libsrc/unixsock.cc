// Module:  Log4CPLUS
// File:    socket-unix.cxx
// Created: 4/2003
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

//#include <cstring>
//#include <vector>
//#include <algorithm>
#include "dcmtk/oflog/helpers/socket.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/threads.h"
#include "dcmtk/oflog/spi/logevent.h"

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef LOG4CPLUS_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef LOG4CPLUS_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef LOG4CPLUS_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef LOG4CPLUS_HAVE_NETDB_H
#include <netdb.h>
#endif

#define INCLUDE_CERRNO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDLIB
//#include <unistd.h>
#include "dcmtk/ofstd/ofstdinc.h"

//#include <algorithm>

using namespace log4cplus;
using namespace log4cplus::helpers;


namespace
{


#if !defined (LOG4CPLUS_HAVE_GETADDRINFO)
static LOG4CPLUS_MUTEX_PTR_DECLARE ghbn_mutex = LOG4CPLUS_MUTEX_CREATE;
#endif


static
int
get_host_by_name (char const * hostname, OFString* name,
    struct sockaddr_in * addr)
{
#if defined (LOG4CPLUS_HAVE_GETADDRINFO)
    struct addrinfo hints;
    STD_NAMESPACE memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_CANONNAME;

    if (inet_addr (hostname) != OFstatic_cast(in_addr_t, -1))
        hints.ai_flags |= AI_NUMERICHOST;

    struct addrinfo * res = 0;
    int ret = getaddrinfo (hostname, 0, &hints, &res);
    if (ret != 0)
        return ret;

    struct addrinfo const & ai = *res;
    assert (ai.ai_family == AF_INET);

    if (name)
        *name = ai.ai_canonname;

    if (addr)
        STD_NAMESPACE memcpy (addr, ai.ai_addr, ai.ai_addrlen);

    freeaddrinfo (res);

#else
    thread::Guard guard (ghbn_mutex);

    struct hostent * hp = gethostbyname (hostname);
    if (! hp)
        return 1;
    assert (hp->h_addrtype == AF_INET);

    if (name)
        *name = hp->h_name;

    if (addr)
    {
	assert (hp->h_length <= (int) sizeof (addr->sin_addr));
        STD_NAMESPACE memcpy (&addr->sin_addr, hp->h_addr_list[0], hp->h_length);
    }

#endif

    return 0;
}


} // namespace


/////////////////////////////////////////////////////////////////////////////
// Global Methods
/////////////////////////////////////////////////////////////////////////////

SOCKET_TYPE
log4cplus::helpers::openSocket(unsigned short port, SocketState& state)
{
    SOCKET_TYPE sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        return INVALID_SOCKET;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    int optval = 1;
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, OFreinterpret_cast(char*, &optval), sizeof(optval) );

    if(bind(sock, OFreinterpret_cast(struct sockaddr*, &server), sizeof(server)) < 0) {
        return INVALID_SOCKET;
    }

    if(::listen(sock, 10)) {
        return INVALID_SOCKET;
    }

    state = ok;
    return sock;
}


SOCKET_TYPE
log4cplus::helpers::connectSocket(const log4cplus::tstring& hostn,
                                  unsigned short port, SocketState& state)
{
    struct sockaddr_in server;
    SOCKET_TYPE sock;
    int retval;

    STD_NAMESPACE memset (&server, 0, sizeof (server));
    retval = get_host_by_name (LOG4CPLUS_TSTRING_TO_STRING(hostn).c_str(),
        0, &server);
    if (retval != 0)
        return INVALID_SOCKET;

    server.sin_port = htons(port);
    server.sin_family = AF_INET;

    sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        return INVALID_SOCKET;
    }

    while ((retval = ::connect(sock,
                OFreinterpret_cast(struct sockaddr *, &server), sizeof (server))) == -1
          && (errno == EINTR))
        ;
    if(retval == INVALID_SOCKET) {
        ::close(sock);
        return INVALID_SOCKET;
    }

    state = ok;
    return sock;
}



SOCKET_TYPE
log4cplus::helpers::acceptSocket(SOCKET_TYPE sock, SocketState& state)
{
    struct sockaddr_in net_client;
#ifdef HAVE_DECLARATION_SOCKLEN_T
    socklen_t len;
#elif !defined(HAVE_PROTOTYPE_ACCEPT) || defined(HAVE_INTP_ACCEPT)
    int len;
#else
    size_t len;
#endif
    len = sizeof(net_client);
    SOCKET_TYPE clientSock;

    while(   (clientSock = ::accept(sock, OFreinterpret_cast(struct sockaddr*, &net_client), &len)) == -1
          && (errno == EINTR))
        ;

    if(clientSock != INVALID_SOCKET) {
        state = ok;
    }

    return clientSock;
}



int
log4cplus::helpers::closeSocket(SOCKET_TYPE sock)
{
    return ::close(sock);
}



long
log4cplus::helpers::read(SOCKET_TYPE sock, SocketBuffer& buffer)
{
    long read = 0;

    do
    {
        long res = ::read(sock, buffer.getBuffer() + read,
            buffer.getMaxSize() - read);
        if( res <= 0 ) {
            return res;
        }
        read += res;
    } while( read < OFstatic_cast(long, buffer.getMaxSize()) );

    return read;
}



long
log4cplus::helpers::write(SOCKET_TYPE sock, const SocketBuffer& buffer)
{
#if defined(MSG_NOSIGNAL)
    int flags = MSG_NOSIGNAL;
#else
    int flags = 0;
#endif
    return ::send( sock, buffer.getBuffer(), buffer.getSize(), flags );
}


tstring
log4cplus::helpers::getHostname (bool fqdn)
{
    char const * hostname = "unknown";
    int ret;
    size_t hn_size = 1024;
    char *hn = OFstatic_cast(char *, malloc(hn_size));

    while (true)
    {
        ret = ::gethostname (&hn[0], OFstatic_cast(int, hn_size) - 1);
        if (ret == 0)
        {
            hostname = &hn[0];
            break;
        }
#if defined (LOG4CPLUS_HAVE_ENAMETOOLONG)
        else if (ret != 0 && errno == ENAMETOOLONG)
        {
            // Our buffer was too short. Retry with buffer twice the size.
            hn_size *= 2;
            hn = OFstatic_cast(char *, realloc(hn, hn_size));
        }
#endif
        else
            break;
    }

    if (ret != 0 || (ret == 0 && ! fqdn))
    {
        tstring res = LOG4CPLUS_STRING_TO_TSTRING (hostname);
        free(hn);
        return res;
    }

    OFString full_hostname;
    ret = get_host_by_name (hostname, &full_hostname, 0);
    if (ret == 0)
        hostname = full_hostname.c_str ();

    tstring res = LOG4CPLUS_STRING_TO_TSTRING (hostname);
    free(hn);
    return res;
}
