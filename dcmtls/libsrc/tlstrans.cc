/*
 *
 *  Copyright (C) 1998-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmtls
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTLSConnection
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-08-10 14:50:29 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <signal.h>
#include <time.h>
#include <openssl/err.h>
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>	/* Use the Grand Unified Sockets Interface (GUSI) on Macintosh */
#endif

#include "tlstrans.h"
#include "ofconsol.h"


DcmTLSConnection::DcmTLSConnection(int openSocket, SSL *newTLSConnection)
: DcmTransportConnection(openSocket)
, tlsConnection(newTLSConnection)
{
}

DcmTLSConnection::~DcmTLSConnection()
{
  if (tlsConnection) SSL_free(tlsConnection);
}

DcmTransportLayerStatus DcmTLSConnection::serverSideHandshake()
{
  if (tlsConnection == NULL) return TCS_noConnection;
  switch (SSL_get_error(tlsConnection, SSL_accept(tlsConnection)))
  {
    case SSL_ERROR_NONE:
      break;
    default:
      return TCS_unspecifiedError;
      /* break; */
  }
  // we should examine our peer's certificates here

  return TCS_ok;
}

DcmTransportLayerStatus DcmTLSConnection::clientSideHandshake()
{
  if (tlsConnection == NULL) return TCS_noConnection;
  switch (SSL_get_error(tlsConnection, SSL_connect(tlsConnection)))
  {
    case SSL_ERROR_NONE:
      break;
    case SSL_ERROR_SSL:
      cerr << ERR_error_string(ERR_peek_error(), NULL) << endl;
      return TCS_unspecifiedError;
      /* break; */
    case SSL_ERROR_WANT_READ:
      return TCS_unspecifiedError;
      /* break; */
    case SSL_ERROR_WANT_WRITE:
      return TCS_unspecifiedError;
      /* break; */
    case SSL_ERROR_WANT_X509_LOOKUP:
      return TCS_unspecifiedError;
      /* break; */
    case SSL_ERROR_SYSCALL:
      return TCS_unspecifiedError;
      /* break; */
    case SSL_ERROR_ZERO_RETURN:
      return TCS_unspecifiedError;
      /* break; */
    case SSL_ERROR_WANT_CONNECT:
      return TCS_unspecifiedError;
      /* break; */
    default:
      return TCS_unspecifiedError;
      /* break; */
  }
  // we should examine our peer's certificates here

  return TCS_ok;
}

DcmTransportLayerStatus DcmTLSConnection::renegotiate(const char *newSuite)
{
  if (tlsConnection == NULL) return TCS_noConnection;
  if (newSuite == NULL) return TCS_unspecifiedError;
  
  switch (SSL_get_error(tlsConnection, SSL_set_cipher_list(tlsConnection, newSuite)))
  {
    case SSL_ERROR_NONE:
      break;
    default:
      return TCS_unspecifiedError;
      /* break; */
  }
  switch (SSL_get_error(tlsConnection, SSL_renegotiate(tlsConnection)))
  {
    case SSL_ERROR_NONE:
      break;
    default:
      return TCS_unspecifiedError;
      /* break; */
  }
  return TCS_ok;
}

ssize_t DcmTLSConnection::read(void *buf, size_t nbyte)
{
  if (tlsConnection) return SSL_read(tlsConnection, (char*)buf, nbyte);
  errno = EIO; /* IO Error */
  return -1;
}

ssize_t DcmTLSConnection::write(void *buf, size_t nbyte)
{
  if (tlsConnection) return SSL_write(tlsConnection, (char*)buf, nbyte);
  errno = EIO; /* IO Error */
  return -1;
}

void DcmTLSConnection::close()
{
  if (tlsConnection) SSL_shutdown(tlsConnection);
#ifdef HAVE_WINSOCK_H
  (void) shutdown(getSocket(),  1 /* SD_SEND */);
  (void) closesocket(getSocket());
#else
  (void) ::close(getSocket());
#endif
}

unsigned long DcmTLSConnection::getPeerCertificateLength()
{
  unsigned long result = 0;
  if (tlsConnection)
  {
    X509 *peerCert = SSL_get_peer_certificate(tlsConnection);
    if (peerCert)
    {
      result = (unsigned long) i2d_X509(peerCert, NULL);
    }
  }
  return result;
}

unsigned long DcmTLSConnection::getPeerCertificate(void *buf, unsigned long bufLen)
{
  unsigned long result = 0;
  if (tlsConnection && buf)
  {
    X509 *peerCert = SSL_get_peer_certificate(tlsConnection);
    if (peerCert)
    {
      unsigned long certSize = (unsigned long) i2d_X509(peerCert, NULL);
      if (certSize <= bufLen)
      {
      	unsigned char *p = (unsigned char *)buf;
        result = (unsigned long) i2d_X509(peerCert, &p);
      }
    }
  }
  return result;
}

OFBool DcmTLSConnection::networkDataAvailable(int timeout)
{
  /* this is an approximation since SSL_pending does not support
   * waiting with a timeout. We first check SSL_pending and then
   * perform a conventional select().
   */
  if (tlsConnection == NULL) return OFFalse;
  if (SSL_pending(tlsConnection)) return OFTrue;

  struct timeval t;
  fd_set fdset;
  int nfound;

  FD_ZERO(&fdset);
  FD_SET(getSocket(), &fdset);
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

OFBool DcmTLSConnection::isTransparentConnection()
{
  return OFFalse;
}

#else  /* WITH_OPENSSL */

/* make sure that the object file is not completely empty if compiled 
 * without OpenSSL because some linkers might fail otherwise.
 */
static void tlstrans_dummy_function()
{
  return;
}

#endif /* WITH_OPENSSL */


/*
 *  $Log: tlstrans.cc,v $
 *  Revision 1.1  2000-08-10 14:50:29  meichel
 *  Added initial OpenSSL support.
 *
 *
 */

