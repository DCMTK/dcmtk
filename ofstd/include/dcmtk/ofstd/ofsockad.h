/*
 *
 *  Copyright (C) 2017-2018, OFFIS e.V.
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
#include "dcmtk/ofstd/ofdefine.h"  // for memzero()
#include "dcmtk/ofstd/ofstream.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#ifndef DCOMPAT_SYS_SOCKET_H_
#define DCOMPAT_SYS_SOCKET_H_
/* some systems don't protect sys/socket.h (e.g. DEC Ultrix) */
#include <sys/socket.h>
#endif
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <ws2tcpip.h> /* for socklen_t */
#endif

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

  /// Default constructor
  OFSockAddr() { clear(); }

  /// Destructor
  ~OFSockAddr() { }

  /// initialize address storage object with memzero
  void clear() { memzero(&sa, sizeof(sa)); }

  /** access socket address storage object as struct sockaddr (opaque address)
   *  @return address storage object as struct sockaddr *.
   */
  struct sockaddr *getSockaddr() { return OFreinterpret_cast(struct sockaddr *, &sa); }

  /** access socket address storage object as struct sockaddr_in (IPv4 address)
   *  @return address storage object as struct sockaddr_in *.
   */
  struct sockaddr_in *getSockaddr_in() { return OFreinterpret_cast(struct sockaddr_in *, &sa); }

  /** access socket address storage object as struct sockaddr_in6 (IPv6 address)
   *  @return address storage object as struct sockaddr_in6 *.
   */
  struct sockaddr_in6 *getSockaddr_in6() { return OFreinterpret_cast(struct sockaddr_in6 *, &sa); }

  /** access socket address storage object as const struct sockaddr_in (IPv4 address)
   *  @return address storage object as const struct sockaddr_in *.
   */
  const struct sockaddr_in *getSockaddr_in_const() const { return OFreinterpret_cast(const struct sockaddr_in *, &sa); }

  /** access socket address storage object as const struct sockaddr_in6 (IPv6 address)
   *  @return address storage object as const struct sockaddr_in6 *.
   */
  const struct sockaddr_in6 *getSockaddr_in6_const() const { return OFreinterpret_cast(const struct sockaddr_in6 *, &sa); }

  /** return size of sockaddr struct depending on current protocol family
   *  @return size of sockaddr struct depending on current protocol family
   */
  socklen_t size() const;

  /** get current protocol family. Returns 0 if uninitialized, AF_INET or AF_INET6 otherwise.
   *  @return current protocol family of the socket address.
   */
  short getFamily() const { return sa.ss_family; }

  /** set current protocol family.
   *  @param family protocol family, should be AF_INET or AF_INET6.
   */
  void setFamily(short family) { sa.ss_family = family; }

  /** set port number for current protocol family.
   *  Only works if the family has been set to AF_INET or AF_INET6 prior to calling this method.
   *  @param port port number in network byte order (e.g. output of htons()).
   */
  void setPort(unsigned short port);

private:

  /** container for the socket address structure.
   *  Guaranteed to be large enough for all supported protocol types.
   */
  struct sockaddr_storage sa;

};

DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream& operator<< (STD_NAMESPACE ostream& o, const OFSockAddr& s);

#endif // OFSOCKAD_H
