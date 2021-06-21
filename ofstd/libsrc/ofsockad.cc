/*
 *
 *  Copyright (C) 2017-2021, OFFIS e.V.
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

socklen_t OFSockAddr::size() const
{
  switch (getFamily())
  {
    case AF_INET:
      return sizeof(struct sockaddr_in);
    case AF_INET6:
      return sizeof(struct sockaddr_in6);
    default:
      return 0;
  }
}

void OFSockAddr::setPort(unsigned short port)
{
  struct sockaddr_in *si = NULL;
  struct sockaddr_in6 *si6 = NULL;
  switch (getFamily())
  {
      case AF_INET:
        si = getSockaddr_in();
        si->sin_port = port;
        break;
      case AF_INET6:
        si6 = getSockaddr_in6();
        si6->sin6_port = port;
        break;
      default:
        /* unknown protocol family, do nothing */
        break;
  }
}

DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream& operator<< (STD_NAMESPACE ostream& o, const OFSockAddr& s)
{
  o << "SOCKADDR_BEGIN\n  Family: ";
#ifdef _WIN32
  unsigned long bufsize = 512;
#endif
  char buf[512];
  buf[0] = '\0';
  const struct sockaddr_in *si = NULL;
  const struct sockaddr_in6 *si6 = NULL;

  switch (s.getFamily())
  {
    case 0:
      o << "not set\n";
      break;
    case AF_INET:
      si = s.getSockaddr_in_const();
      o << "AF_INET";

#ifdef _WIN32
      /* MinGW and some Visual Studio versions do not have inet_ntop() */
      WSAAddressToStringA((struct sockaddr*) si, OFstatic_cast(DWORD, s.size()), NULL, buf, &bufsize);
      o  << "\n  IP address: " << buf;
#else
      // The typecast is necessary for older MSVC compilers, which expect a non-const void * parameter.
      o << "\n  IP address: " << inet_ntop(AF_INET,  OFconst_cast(void *, OFreinterpret_cast(const void *, &si->sin_addr)), buf, 512);
#endif
      o << "\n  Port: " << ntohs(si->sin_port) << "\n";
      break;
    case AF_INET6:
      si6 = s.getSockaddr_in6_const();
      o << "  AF_INET6";
#ifdef _WIN32
      /* MinGW and some Visual Studio versions do not have inet_ntop() */
      WSAAddressToStringA((struct sockaddr*) si6, OFstatic_cast(DWORD, s.size()), NULL, buf, &bufsize);
      o  << "\n  IP address: " << buf;
#else
      o << "\n  IP address: " << inet_ntop(AF_INET6, OFconst_cast(void *, OFreinterpret_cast(const void *, &si6->sin6_addr)), buf, 512);
#endif
      o << "\n  Port: " << ntohs(si6->sin6_port)
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
