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
 *  Update Date:      $Date: 2000-10-11 16:15:35 $
 *  CVS/RCS Revision: $Revision: 1.3 $
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
#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif
#include <openssl/err.h>
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>	/* Use the Grand Unified Sockets Interface (GUSI) on Macintosh */
#endif

#include "tlstrans.h"
#include "tlslayer.h"
#include "ofconsol.h"


DcmTLSConnection::DcmTLSConnection(int openSocket, SSL *newTLSConnection)
: DcmTransportConnection(openSocket)
, tlsConnection(newTLSConnection)
, lastError(0)
{
}

DcmTLSConnection::~DcmTLSConnection()
{
  if (tlsConnection) SSL_free(tlsConnection);
}

DcmTransportLayerStatus DcmTLSConnection::serverSideHandshake()
{
  if (tlsConnection == NULL) return TCS_noConnection;
  DcmTransportLayerStatus result = TCS_ok;
  lastError = 0;
  switch (SSL_get_error(tlsConnection, SSL_accept(tlsConnection)))
  {
    case SSL_ERROR_NONE:
      /* success */
      break;
    case SSL_ERROR_SYSCALL:
    case SSL_ERROR_SSL:
      lastError = ERR_peek_error();
      result = TCS_tlsError;
      break;
    default:
      // case SSL_ERROR_WANT_READ:
      // case SSL_ERROR_WANT_WRITE:
      // case SSL_ERROR_WANT_X509_LOOKUP:
      // case SSL_ERROR_WANT_CONNECT:
      // case SSL_ERROR_ZERO_RETURN:
      result = TCS_tlsError;
      break;
  }

  // if the certificate verification has failed, the certificate is already
  // unavailable at this point. We know that something has gone wrong, but
  // OpenSSL does not tell us who tried to connect.

  return result;
}

DcmTransportLayerStatus DcmTLSConnection::clientSideHandshake()
{
  if (tlsConnection == NULL) return TCS_noConnection;
  lastError = 0;
  DcmTransportLayerStatus result = TCS_ok;
  switch (SSL_get_error(tlsConnection, SSL_connect(tlsConnection)))
  {
    case SSL_ERROR_NONE:
      /* success */
      break;
    case SSL_ERROR_SYSCALL:
    case SSL_ERROR_SSL:
      lastError = ERR_peek_error();
      result = TCS_tlsError;
      break;
    default:
      // case SSL_ERROR_WANT_READ:
      // case SSL_ERROR_WANT_WRITE:
      // case SSL_ERROR_WANT_X509_LOOKUP:
      // case SSL_ERROR_WANT_CONNECT:
      // case SSL_ERROR_ZERO_RETURN:
      result = TCS_tlsError;
      break;
  }
  return result;
}

DcmTransportLayerStatus DcmTLSConnection::renegotiate(const char *newSuite)
{
  if (tlsConnection == NULL) return TCS_noConnection;
  if (newSuite == NULL) return TCS_illegalCall;
  DcmTransportLayerStatus result = TCS_ok;
  
  switch (SSL_get_error(tlsConnection, SSL_set_cipher_list(tlsConnection, newSuite)))
  {
    case SSL_ERROR_NONE:
      /* success */
      break;
    case SSL_ERROR_SYSCALL:
    case SSL_ERROR_SSL:
      lastError = ERR_peek_error();
      result = TCS_tlsError;
      break;
    default:
      // case SSL_ERROR_WANT_READ:
      // case SSL_ERROR_WANT_WRITE:
      // case SSL_ERROR_WANT_X509_LOOKUP:
      // case SSL_ERROR_WANT_CONNECT:
      // case SSL_ERROR_ZERO_RETURN:
      result = TCS_tlsError;
      break;
  }
  if (result != TCS_ok) return result;

  switch (SSL_get_error(tlsConnection, SSL_renegotiate(tlsConnection)))
  {
    case SSL_ERROR_NONE:
      /* success */
      break;
    case SSL_ERROR_SYSCALL:
    case SSL_ERROR_SSL:
      lastError = ERR_peek_error();
      result = TCS_tlsError;
      break;
    default:
      // case SSL_ERROR_WANT_READ:
      // case SSL_ERROR_WANT_WRITE:
      // case SSL_ERROR_WANT_X509_LOOKUP:
      // case SSL_ERROR_WANT_CONNECT:
      // case SSL_ERROR_ZERO_RETURN:
      result = TCS_tlsError;
      break;
  }

  return result;
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

void DcmTLSConnection::dumpConnectionParameters(ostream &out)
{
  out << "Transport connection: TLS/SSL over TCP/IP" << endl
      << "  Protocol: " << SSL_get_version(tlsConnection) << endl 
      << "  Ciphersuite: " << SSL_CIPHER_get_name(SSL_get_current_cipher(tlsConnection))
      << ", version: " << SSL_CIPHER_get_version(SSL_get_current_cipher(tlsConnection))
      << ", encryption: " << SSL_CIPHER_get_bits(SSL_get_current_cipher(tlsConnection), NULL) << " bits" << endl;
  DcmTLSTransportLayer::printX509Certificate(out, SSL_get_peer_certificate(tlsConnection));
  // out << "Certificate verification: " << X509_verify_cert_error_string(SSL_get_verify_result(tlsConnection)) << endl;
  return;
}

const char *DcmTLSConnection::errorString(DcmTransportLayerStatus code)
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
      if (lastError)
      {
        const char *result = ERR_reason_error_string(lastError);
        if (result) return result;
      } 
      return "unspecified TLS error";
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

#else  /* WITH_OPENSSL */

/* make sure that the object file is not completely empty if compiled 
 * without OpenSSL because some linkers might fail otherwise.
 */
void tlstrans_dummy_function()
{
  return;
}


#endif /* WITH_OPENSSL */


/*
 *  $Log: tlstrans.cc,v $
 *  Revision 1.3  2000-10-11 16:15:35  meichel
 *  Updated includes for Win32 environment
 *
 *  Revision 1.2  2000/10/10 12:13:36  meichel
 *  Added routines for printing certificates and connection parameters.
 *
 *  Revision 1.1  2000/08/10 14:50:29  meichel
 *  Added initial OpenSSL support.
 *
 *
 */

