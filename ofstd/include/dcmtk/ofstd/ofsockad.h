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

#ifndef OFSOCKAD_H
#define OFSOCKAD_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofstream.h"

BEGIN_EXTERN_C
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <ws2tcpip.h>    /* for struct sockaddr_in6 */
#endif

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
/* some systems (such as DEC Ultrix) don't protect <sys/socket.h> from double inclusion */
#ifndef DCOMPAT_SYS_SOCKET_H_
#define DCOMPAT_SYS_SOCKET_H_
#include <sys/socket.h>
#endif
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
#include <ws2tcpip.h> /* for socklen_t */
#endif

#include <cstring> // for memset()

/* forward declarations */
struct sockaddr;
struct sockaddr_in;
struct sockaddr_in6;

/** A simple wrapper class for a struct sockaddr_storage object 
 *  that can be used to store an TCP/IPv4 (struct sockaddr_in) or TCP/IPv6
 *  (struct sockaddr_in6) address.
 */
class DCMTK_OFSTD_EXPORT OFSockAddr
{
public:
  union socket_address
  {

    struct sockaddr         sa;
    struct sockaddr_in      s4;
    struct sockaddr_in6     s6;
    struct sockaddr_storage ss;
  };

  /// Default constructor
  OFSockAddr() { clear(); }

  /// Destructor
  ~OFSockAddr() { }

  /// initialize address storage object with memset
  void clear() { memset(&sa, 0, sizeof(sa)); }

  /** access socket address storage object as struct sockaddr (opaque address)
   *  @return address storage object as struct sockaddr *.
   */
  struct sockaddr *getSockaddr() { return &sa.sa; }

  /** access socket address storage object as struct sockaddr_in (IPv4 address)
   *  @return address storage object as struct sockaddr_in *.
   */
  struct sockaddr_in *getSockaddr_in() { return &sa.s4; }

  /** access socket address storage object as struct sockaddr_in6 (IPv6 address)
   *  @return address storage object as struct sockaddr_in6 *.
   */
  struct sockaddr_in6 *getSockaddr_in6() { return &sa.s6; }

  /** access socket address storage object as const struct sockaddr_in (IPv4 address)
   *  @return address storage object as const struct sockaddr_in *.
   */
  const struct sockaddr_in *getSockaddr_in_const() const { return &sa.s4; }

  /** access socket address storage object as const struct sockaddr_in6 (IPv6 address)
   *  @return address storage object as const struct sockaddr_in6 *.
   */
  const struct sockaddr_in6 *getSockaddr_in6_const() const { return &sa.s6; }

  /** return size of sockaddr struct depending on current protocol family
   *  @return size of sockaddr struct depending on current protocol family
   */
  socklen_t size() const;

  /** get current protocol family. Returns 0 if uninitialized, AF_INET or AF_INET6 otherwise.
   *  @return current protocol family of the socket address.
   */
  short getFamily() const { return sa.ss.ss_family; }

  /** set current protocol family.
   *  @param family protocol family, should be AF_INET or AF_INET6.
   */
  void setFamily(short family) { sa.ss.ss_family = family; }

  /** set port number for current protocol family.
   *  Only works if the family has been set to AF_INET or AF_INET6 prior to calling this method.
   *  @param port port number in network byte order (e.g. output of htons()).
   */
  void setPort(unsigned short port);

private:

  /** container for the socket address structure.
   *  Guaranteed to be large enough for all supported protocol types.
   */
  socket_address sa;

};

DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream& operator<< (STD_NAMESPACE ostream& o, const OFSockAddr& s);

#endif // OFSOCKAD_H
