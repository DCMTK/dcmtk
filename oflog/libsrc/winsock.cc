// Module:  Log4CPLUS
// File:    socket-win32.cxx
// Created: 4/2003
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

#include "dcmtk/oflog/config.h"
#if defined (DCMTK_LOG4CPLUS_USE_WINSOCK)

#include <cassert>
#include <cerrno>
#include "dcmtk/ofstd/ofvector.h"
#include <cstring>
#include "dcmtk/oflog/internal/socket.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/thread/threads.h"


/////////////////////////////////////////////////////////////////////////////
// file LOCAL Classes
/////////////////////////////////////////////////////////////////////////////

namespace
{

enum WSInitStates
{
    WS_UNINITIALIZED,
    WS_INITIALIZING,
    WS_INITIALIZED
};


static WSADATA wsa;
static LONG volatile winsock_state = WS_UNINITIALIZED;


static LONG DoInterlockedCompareExchange(LPLONG p, LONG comp, LONG set)
{
    LONG ret;
#ifdef HAVE_OLD_INTERLOCKEDCOMPAREEXCHANGE
    void *tmp = ::InterlockedCompareExchange(OFreinterpret_cast(void **, p),
            OFreinterpret_cast(void *, comp), OFreinterpret_cast(void *, set));
    ret = OFreinterpret_cast(LONG, tmp);
#else
    ret = ::InterlockedCompareExchange(p, comp, set);
#endif
    return ret;
}


static
void
init_winsock_worker ()
{
    dcmtk::log4cplus::helpers::LogLog * loglog
        = dcmtk::log4cplus::helpers::LogLog::getLogLog ();

    // Try to change the state to WS_INITIALIZING.
    LONG val = DoInterlockedCompareExchange (
        OFconst_cast(LPLONG, &winsock_state), WS_INITIALIZING, WS_UNINITIALIZED);
    switch (val)
    {
    case WS_UNINITIALIZED:
    {
        int ret = WSAStartup (MAKEWORD (2, 2), &wsa);
        if (ret != 0)
        {
            // Revert the state back to WS_UNINITIALIZED to unblock other
            // threads and let them throw exception.
            val = DoInterlockedCompareExchange (
                OFconst_cast(LPLONG, &winsock_state), WS_UNINITIALIZED,
                WS_INITIALIZING);
            assert (val == WS_INITIALIZING);
            loglog->error (DCMTK_LOG4CPLUS_TEXT ("Could not initialize WinSock."),
                true);
        }

        // WinSock is initialized, change the state to WS_INITIALIZED.
        val = DoInterlockedCompareExchange (
            OFconst_cast(LPLONG, &winsock_state), WS_INITIALIZED,
            WS_INITIALIZING);
        assert (val == WS_INITIALIZING);
        return;
    }

    case WS_INITIALIZING:
        // Wait for state change.
        while (true)
        {
            switch (winsock_state)
            {
            case WS_INITIALIZED:
                return;

            case WS_INITIALIZING:
                dcmtk::log4cplus::thread::yield ();
                continue;
        
            default:
                assert (0);
                loglog->error (DCMTK_LOG4CPLUS_TEXT ("Unknown WinSock state."), true);
            }
        }

    case WS_INITIALIZED:
        // WinSock is already initialized.
        return;

    default:
        assert (0);
        loglog->error (DCMTK_LOG4CPLUS_TEXT ("Unknown WinSock state."), true);
    }
}


static
void
init_winsock ()
{
    // Quick check first to avoid the expensive interlocked compare
    // and exchange.
    if (winsock_state == WS_INITIALIZED)
        return;
    else
        init_winsock_worker ();
}


struct WinSockInitializer
{
    ~WinSockInitializer ()
    {
        if (winsock_state == WS_INITIALIZED)
            WSACleanup ();
    }

    static WinSockInitializer winSockInitializer;
};


WinSockInitializer WinSockInitializer::winSockInitializer;


} // namespace


namespace dcmtk {
namespace log4cplus { namespace helpers {


/////////////////////////////////////////////////////////////////////////////
// Global Methods
/////////////////////////////////////////////////////////////////////////////

SOCKET_TYPE
openSocket(unsigned short port, SocketState& state)
{
    struct sockaddr_in server;

    init_winsock ();

    SOCKET sock = WSASocket (AF_INET, SOCK_STREAM, AF_UNSPEC, 0, 0
#if defined (WSA_FLAG_NO_HANDLE_INHERIT)
        , WSA_FLAG_NO_HANDLE_INHERIT
#else
        , 0
#endif
        );

    if (sock == INVALID_OS_SOCKET_VALUE)
        goto error;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(sock, OFreinterpret_cast(struct sockaddr*, &server), OFstatic_cast(int, sizeof(server)))
        != 0)
        goto error;

    if (::listen(sock, 10) != 0)
        goto error;

    state = ok;
    return to_log4cplus_socket (sock);

error:
    int eno = WSAGetLastError ();

    if (sock != INVALID_OS_SOCKET_VALUE)
        ::closesocket (sock);

    set_last_socket_error (eno);
    return INVALID_SOCKET_VALUE;
}


SOCKET_TYPE
connectSocket(const tstring& hostn, unsigned short port, bool udp, SocketState& state)
{
    struct hostent * hp;
    struct sockaddr_in insock;
    int retval;

    init_winsock ();

    SOCKET sock = WSASocket (AF_INET, (udp ? SOCK_DGRAM : SOCK_STREAM),
        AF_UNSPEC, 0, 0
#if defined (WSA_FLAG_NO_HANDLE_INHERIT)
        , WSA_FLAG_NO_HANDLE_INHERIT
#else
        , 0
#endif
        );
    if (sock == INVALID_OS_SOCKET_VALUE)
        goto error;

    hp = ::gethostbyname( DCMTK_LOG4CPLUS_TSTRING_TO_STRING(hostn).c_str() );
    if (hp == 0 || hp->h_addrtype != AF_INET)
    {
        insock.sin_family = AF_INET;
        INT insock_size = OFstatic_cast(int, sizeof (insock));
#if defined (DCMTK_OFLOG_UNICODE)
        INT ret = WSAStringToAddress (OFconst_cast(LPTSTR, hostn.c_str ()),
            AF_INET, 0, OFreinterpret_cast(struct sockaddr *, &insock),
            &insock_size);
#else
        INT ret = WSAStringToAddressA (OFconst_cast(LPSTR, hostn.c_str ()),
            AF_INET, 0, OFreinterpret_cast(struct sockaddr *, &insock),
            &insock_size);
#endif
        if (ret == SOCKET_ERROR || insock_size != sizeof (insock)) 
        {
            state = bad_address;
            goto error;
        }
    }
    else
        memcpy (&insock.sin_addr, hp->h_addr_list[0],
            sizeof (insock.sin_addr));

    insock.sin_port = htons(port);
    insock.sin_family = AF_INET;

    while(   (retval = ::connect(sock, OFreinterpret_cast(struct sockaddr*, &insock), OFstatic_cast(int, sizeof(insock)))) == -1
          && (WSAGetLastError() == WSAEINTR))
        ;
    if (retval == SOCKET_ERROR)
        goto error;

    state = ok;
    return to_log4cplus_socket (sock);

error:
    int eno = WSAGetLastError ();

    if (sock != INVALID_OS_SOCKET_VALUE)
        ::closesocket (sock);

    set_last_socket_error (eno);
    return INVALID_SOCKET_VALUE;
}


SOCKET_TYPE
acceptSocket(SOCKET_TYPE sock, SocketState & state)
{
    init_winsock ();

    SOCKET connected_socket = ::accept (to_os_socket (sock), NULL, NULL);

    if (connected_socket != INVALID_OS_SOCKET_VALUE)
        state = ok;

    return to_log4cplus_socket (connected_socket);
}



int
closeSocket(SOCKET_TYPE sock)
{
    return ::closesocket (to_os_socket (sock));
}



long
read(SOCKET_TYPE sock, SocketBuffer& buffer)
{
    long res, read = 0;
 
    do
    { 
        res = ::recv(to_os_socket (sock), 
                     buffer.getBuffer() + read, 
                     OFstatic_cast(int, buffer.getMaxSize() - read),
                     0);
        if (res == SOCKET_ERROR)
        {
            set_last_socket_error (WSAGetLastError ());
            return res;
        }
        read += res;
    }
    while (read < OFstatic_cast(long, buffer.getMaxSize()));
 
    return read;
}



long
write(SOCKET_TYPE sock, const SocketBuffer& buffer)
{
    long ret = ::send (to_os_socket (sock), buffer.getBuffer(),
        OFstatic_cast(int, buffer.getSize()), 0);
    if (ret == SOCKET_ERROR)
        set_last_socket_error (WSAGetLastError ());
    return ret;
}


long
write(SOCKET_TYPE sock, const STD_NAMESPACE string & buffer)
{
    long ret = ::send (to_os_socket (sock), buffer.c_str (),
        OFstatic_cast(int, buffer.size ()), 0);
    if (ret == SOCKET_ERROR)
        set_last_socket_error (WSAGetLastError ());
    return ret;
}


tstring
getHostname (bool fqdn)
{
    char const * hostname = "unknown";
    int ret;
    OFVector<char> hn (1024, 0);

    while (true)
    {
        ret = ::gethostname (&hn[0], OFstatic_cast(int, hn.size ()) - 1);
        if (ret == 0)
        {
            hostname = &hn[0];
            break;
        }
        else if (ret != 0 && WSAGetLastError () == WSAEFAULT)
            // Out buffer was too short. Retry with buffer twice the size.
            hn.resize (hn.size () * 2, 0);
        else
            break;
    }

    if (ret != 0 || (ret == 0 && ! fqdn))
        return DCMTK_LOG4CPLUS_STRING_TO_TSTRING (hostname);

    struct ::hostent * hp = ::gethostbyname (hostname);
    if (hp)
        hostname = hp->h_name;

    return DCMTK_LOG4CPLUS_STRING_TO_TSTRING (hostname);
}


int
setTCPNoDelay (SOCKET_TYPE sock, bool val)
{
    int result;
    int enabled = OFstatic_cast(int, val);
    if ((result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
            OFreinterpret_cast(char*, &enabled),OFstatic_cast(int, sizeof(enabled)))) != 0)
    {
        int eno = WSAGetLastError ();
        set_last_socket_error (eno);
    }

    return result;
}


} } // namespace log4cplus { namespace helpers {
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_USE_WINSOCK
