/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Wrapper class for struct sockaddr and related structs
 *
 */

#include "dcmtk/config/osconfig.h"     /* include OS specific configuration first */
#include "dcmtk/ofstd/ofsockad.h"
#include "dcmtk/ofstd/ofstream.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <ws2tcpip.h>    /* for struct sockaddr_in6 */
#endif

const size_t OFSockAddr::size() const
{
  switch (sa.ss_family)
  {
    case AF_INET:
      return sizeof(struct sockaddr_in);
    case AF_INET6:
      return sizeof(struct sockaddr_in6);
    default:
      return 0;
  }
}

DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream& operator<< (STD_NAMESPACE ostream& o, const OFSockAddr& s)
{
  o << "SOCKADDR_BEGIN\n  Family: ";
  char buf[512];
  const struct sockaddr_in *si = NULL;
  const struct sockaddr_in6 *si6 = NULL;

  switch (s.getFamily())
  {
    case 0:
      o << "not set\n";
      break;
    case AF_INET:
      si = s.getSockaddr_in_const();
      o << "AF_INET"
        << "\n  IP address: " << inet_ntop(AF_INET, OFreinterpret_cast(const void *, &si->sin_addr), buf, 512)
        << "\n  Port: " << si->sin_port << "\n";
      break;
    case AF_INET6:
      si6 = s.getSockaddr_in6_const();
      o << "  AF_INET6"
        << "\n  IP address: " << inet_ntop(AF_INET6, &si6->sin6_addr, buf, 512)
        << "\n  Port: " << si6->sin6_port
        << "\n  Flow info: " << si6->sin6_flowinfo
        << "\n  Scope: " << si6->sin6_scope_id
        << "\n";
      break;
    default:
      o << "unknown protocol: " << s.getFamily() << "\n";
      break;
  }
  o << "SOCKADDR_END" << OFendl;
  return o;
}
