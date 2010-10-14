/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmnet
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTransportConnection, DcmTCPConnection
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:28 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcmtrans.h"
#include "dcmtk/dcmnet/dcompat.h"     /* compatibility code for certain Unix dialects such as SunOS */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CTIME
#define INCLUDE_CERRNO
#define INCLUDE_CSIGNAL
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifdef HAVE_GUSI_H
#include <GUSI.h>	/* Use the Grand Unified Sockets Interface (GUSI) on Macintosh */
#endif

DcmTransportConnection::DcmTransportConnection(int openSocket)
: theSocket(openSocket)
{
}

DcmTransportConnection::~DcmTransportConnection()
{
}

OFBool DcmTransportConnection::safeSelectReadableAssociation(DcmTransportConnection *connections[], int connCount, int timeout)
{
  int numberOfRounds = timeout+1;
  if (numberOfRounds < 0) numberOfRounds = 0xFFFF; /* a long time */

  OFBool found = OFFalse;
  OFBool firstRound = OFTrue;
  int timeToWait=0;
  int i=0;
  while ((numberOfRounds > 0)&&(! found))
  {
    if (firstRound)
    {
      timeToWait = 0;
      firstRound = OFFalse;
    }
    else timeToWait = 1;
    for (i=0; i<connCount; i++)
    {
      if (connections[i])
      {
      	if (connections[i]->networkDataAvailable(timeToWait))
      	{
      	  i = connCount; /* break out of for loop */
      	  found = OFTrue; /* break out of while loop */
      	}
      	timeToWait = 0;
      }
    } /* for */
    if (timeToWait == 1) return OFFalse; /* all entries NULL */
    numberOfRounds--;
  } /* while */

  /* number of rounds == 0 (timeout over), do final check */
  found = OFFalse;
  for (i=0; i<connCount; i++)
  {
    if (connections[i])
    {
      if (connections[i]->networkDataAvailable(0)) found = OFTrue; else connections[i]=NULL;
    }
  }
  return found;
}

OFBool DcmTransportConnection::fastSelectReadableAssociation(DcmTransportConnection *connections[], int connCount, int timeout)
{
  int socketfd = -1;
  int maxsocketfd = -1;
  int i=0;
  struct timeval t;
  fd_set fdset;

  FD_ZERO(&fdset);
  t.tv_sec = timeout;
  t.tv_usec = 0;

  for (i=0; i<connCount; i++)
  {
    if (connections[i])
    {
      socketfd = connections[i]->getSocket();
#ifdef __MINGW32__
      /* on MinGW, FD_SET expects an unsigned first argument */
      FD_SET((unsigned int)socketfd, &fdset);
#else
      FD_SET(socketfd, &fdset);
#endif

      if (socketfd > maxsocketfd) maxsocketfd = socketfd;
    }
  }

#ifdef HAVE_INTP_SELECT
  int nfound = select(maxsocketfd + 1, (int *)(&fdset), NULL, NULL, &t);
#else
  int nfound = select(maxsocketfd + 1, &fdset, NULL, NULL, &t);
#endif
  if (nfound<=0) return OFFalse;      /* none available for reading */

  for (i=0; i<connCount; i++)
  {
    if (connections[i])
    {
      socketfd = connections[i]->getSocket();
      /* if not available, set entry in array to NULL */
      if (!FD_ISSET(socketfd, &fdset)) connections[i] = NULL;
    }
  }
  return OFTrue;
}

OFBool DcmTransportConnection::selectReadableAssociation(DcmTransportConnection *connections[], int connCount, int timeout)
{
  OFBool canUseFastMode = OFTrue;
  for (int i=0; i<connCount; i++)
  {
    if ((connections[i]) && (! connections[i]->isTransparentConnection())) canUseFastMode = OFFalse;
  }
  if (canUseFastMode) return fastSelectReadableAssociation(connections, connCount, timeout);
  return safeSelectReadableAssociation(connections, connCount, timeout);
}

void DcmTransportConnection::dumpConnectionParameters(STD_NAMESPACE ostream& out)
{
    OFString str;
    out << dumpConnectionParameters(str) << OFendl;
}

/* ================================================ */

DcmTCPConnection::DcmTCPConnection(int openSocket)
: DcmTransportConnection(openSocket)
{
}

DcmTCPConnection::~DcmTCPConnection()
{
  close();
}

DcmTransportLayerStatus DcmTCPConnection::serverSideHandshake()
{
  return TCS_ok;
}

DcmTransportLayerStatus DcmTCPConnection::clientSideHandshake()
{
  return TCS_ok;
}

DcmTransportLayerStatus DcmTCPConnection::renegotiate(const char * /* newSuite */ )
{
  return TCS_ok;
}

ssize_t DcmTCPConnection::read(void *buf, size_t nbyte)
{
#ifdef HAVE_WINSOCK_H
  return recv(getSocket(), (char *)buf, nbyte, 0);
#else
  return ::read(getSocket(), (char *)buf, nbyte);
#endif
}

ssize_t DcmTCPConnection::write(void *buf, size_t nbyte)
{
#ifdef HAVE_WINSOCK_H
  return send(getSocket(), (char *)buf, nbyte, 0);
#else
  return ::write(getSocket(), (char *)buf, nbyte);
#endif
}

void DcmTCPConnection::close()
{
  if (getSocket() != -1)
  {
#ifdef HAVE_WINSOCK_H
    (void) shutdown(getSocket(), 1 /* SD_SEND */);
    (void) closesocket(getSocket());
#else
    (void) ::close(getSocket());
#endif
  /* forget about this socket (now closed) */
    setSocket(-1);
  }
}

unsigned long DcmTCPConnection::getPeerCertificateLength()
{
  return 0;
}

unsigned long DcmTCPConnection::getPeerCertificate(void * /* buf */ , unsigned long /* bufLen */ )
{
  return 0;
}

OFBool DcmTCPConnection::networkDataAvailable(int timeout)
{
  struct timeval t;
  fd_set fdset;
  int nfound;

  FD_ZERO(&fdset);

#ifdef __MINGW32__
  /* on MinGW, FD_SET expects an unsigned first argument */
  FD_SET((unsigned int) getSocket(), &fdset);
#else
  FD_SET(getSocket(), &fdset);
#endif

  t.tv_sec = timeout;
  t.tv_usec = 0;

#ifdef HAVE_INTP_SELECT
  nfound = select(getSocket() + 1, (int *)(&fdset), NULL, NULL, &t);
#else
  nfound = select(getSocket() + 1, &fdset, NULL, NULL, &t);
#endif
  if (nfound <= 0) return OFFalse;
  else
  {
    if (FD_ISSET(getSocket(), &fdset)) return OFTrue;
    else return OFFalse;  /* This should not really happen */
  }
}

OFBool DcmTCPConnection::isTransparentConnection()
{
  return OFTrue;
}

OFString& DcmTCPConnection::dumpConnectionParameters(OFString& str)
{
  str = "Transport connection: TCP/IP, unencrypted.";
  return str;
}

const char *DcmTCPConnection::errorString(DcmTransportLayerStatus code)
{
  switch (code)
  {
    case TCS_ok:
      return "no error";
      /* break; */
    case TCS_noConnection:
      return "no secure connection in place";
      /* break; */
    case TCS_tlsError:
      return "TLS error";
      /* break; */
    case TCS_illegalCall:
      return "illegal call";
      /* break; */
    case TCS_unspecifiedError:
      return "unspecified error";
      /* break; */
  }
  return "unknown error code";
}

/*
 *  $Log: dcmtrans.cc,v $
 *  Revision 1.14  2010-10-14 13:14:28  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.13  2010-08-24 10:06:04  uli
 *  Fixed some resource leaks in dcmtls (FDs and memory was leaked).
 *
 *  Revision 1.12  2009-11-18 11:53:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.11  2009-01-29 11:39:20  joergr
 *  Fixed issue with missing invalidation of socket variable during close method.
 *  Please note that this is only required if the connection objects exists after
 *  the TCP/IP connection has been closed (which is currently not the case).
 *
 *  Revision 1.10  2006/08/15 16:04:29  meichel
 *  Updated the code in module dcmnet to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.9  2005/12/08 15:44:35  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2003/07/03 14:21:10  meichel
 *  Added special handling for FD_SET() on MinGW, which expects an
 *    unsigned first argument.
 *
 *  Revision 1.7  2002/11/27 13:04:38  meichel
 *  Adapted module dcmnet to use of new header file ofstdinc.h
 *
 *  Revision 1.6  2001/06/01 15:50:05  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/12/12 16:44:49  meichel
 *  Minor changes to keep gcc 2.7.x on SunOS 4.1.3 happy
 *
 *  Revision 1.4  2000/10/10 12:06:56  meichel
 *  Updated transport layer error codes and routines for printing
 *    connection parameters.
 *
 *  Revision 1.3  2000/09/05 16:52:41  joergr
 *  Removed unnecessary '#ifdef HAVE_WINDOWS_H' statements.
 *
 *  Revision 1.2  2000/09/05 15:24:18  joergr
 *  Adapted source code to compile on Windows (MSVC++ 5.0).
 *
 *  Revision 1.1  2000/08/10 14:50:56  meichel
 *  Added initial OpenSSL support.
 *
 *
 */
