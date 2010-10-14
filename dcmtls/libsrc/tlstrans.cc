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
 *  Module: dcmtls
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTLSConnection
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:46 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#define INCLUDE_CSIGNAL
#define INCLUDE_CTIME
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
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <winbase.h>
#endif
#include <openssl/err.h>
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>	/* Use the Grand Unified Sockets Interface (GUSI) on Macintosh */
#endif

#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#include "dcmtk/dcmnet/dcompat.h"    /* to make sure we have a select prototype */


DcmTLSConnection::DcmTLSConnection(int openSocket, SSL *newTLSConnection)
: DcmTransportConnection(openSocket)
, tlsConnection(newTLSConnection)
, lastError(0)
{
}

DcmTLSConnection::~DcmTLSConnection()
{
  close();
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
  DCMTLS_TRACE("Starting TLS client handshake");
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
  if (tlsConnection) return SSL_read(tlsConnection, OFreinterpret_cast(char*, buf), nbyte);
  errno = EIO; /* IO Error */
  return -1;
}

ssize_t DcmTLSConnection::write(void *buf, size_t nbyte)
{
  if (tlsConnection) return SSL_write(tlsConnection, OFreinterpret_cast(char*, buf), nbyte);
  errno = EIO; /* IO Error */
  return -1;
}

void DcmTLSConnection::close()
{
  if (tlsConnection != NULL)
  {
    SSL_shutdown(tlsConnection);
    SSL_free(tlsConnection);
    tlsConnection = NULL;
  }
  if (getSocket()!=-1)
  {
#ifdef HAVE_WINSOCK_H
    (void) shutdown(getSocket(),  1 /* SD_SEND */);
    (void) closesocket(getSocket());
#else
    (void) ::close(getSocket());
#endif
    setSocket(-1);
  }
}

unsigned long DcmTLSConnection::getPeerCertificateLength()
{
  unsigned long result = 0;
  if (tlsConnection)
  {
    X509 *peerCert = SSL_get_peer_certificate(tlsConnection);
    if (peerCert)
    {
      result = OFstatic_cast(unsigned long, i2d_X509(peerCert, NULL));
      X509_free(peerCert);
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
      unsigned long certSize = OFstatic_cast(unsigned long, i2d_X509(peerCert, NULL));
      if (certSize <= bufLen)
      {
        unsigned char *p = OFreinterpret_cast(unsigned char *, buf);
        result = OFstatic_cast(unsigned long, i2d_X509(peerCert, &p));
      }
      X509_free(peerCert);
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

OFString& DcmTLSConnection::dumpConnectionParameters(OFString& str)
{
  if (tlsConnection == NULL)
  {
    // This should never happen (famous last words)
    str = "Transport connection: TLS/SSL over TCP/IP\n  Error: No Connection\n";
    return str;
  }

  X509 *peerCert = SSL_get_peer_certificate(tlsConnection);
  OFOStringStream stream;
  stream << "Transport connection: TLS/SSL over TCP/IP" << OFendl
         << "  Protocol: " << SSL_get_version(tlsConnection) << OFendl
         << "  Ciphersuite: " << SSL_CIPHER_get_name(SSL_get_current_cipher(tlsConnection))
         << ", version: " << SSL_CIPHER_get_version(SSL_get_current_cipher(tlsConnection))
         << ", encryption: " << SSL_CIPHER_get_bits(SSL_get_current_cipher(tlsConnection), NULL) << " bits" << OFendl
         << DcmTLSTransportLayer::dumpX509Certificate(peerCert) << OFendl;
  // out << "Certificate verification: " << X509_verify_cert_error_string(SSL_get_verify_result(tlsConnection)) << OFendl;
  X509_free(peerCert);
  stream << OFStringStream_ends;
  OFSTRINGSTREAM_GETSTR(stream, res)
  str = res;
  OFSTRINGSTREAM_FREESTR(res)
  return str;
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
 *  Revision 1.18  2010-10-14 13:14:46  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.17  2010-08-24 10:06:04  uli
 *  Fixed some resource leaks in dcmtls (FDs and memory was leaked).
 *
 *  Revision 1.16  2010-08-05 08:38:11  uli
 *  Fixed some warnings from -Wold-style-cast.
 *
 *  Revision 1.15  2010-06-02 12:32:58  joergr
 *  Appended missing OFStringStream_ends to the end of output streams because
 *  this is required when OFOStringStream is mapped to ostrstream.
 *
 *  Revision 1.14  2010-04-29 16:18:51  onken
 *  Added debug message noting the initiation of a TLS client handshake.
 *
 *  Revision 1.13  2010-02-22 11:39:55  uli
 *  Remove some unneeded includes.
 *
 *  Revision 1.12  2009-11-18 12:11:19  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.11  2006-08-15 16:02:56  meichel
 *  Updated the code in module dcmtls to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.10  2005/12/08 15:48:29  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2002/11/27 12:58:39  meichel
 *  Adapted module dcmtls to use of new header file ofstdinc.h
 *
 *  Revision 1.8  2001/12/19 10:00:04  meichel
 *  Added include to avoid warning on Ultrix.
 *
 *  Revision 1.7  2001/06/05 10:32:56  joergr
 *  Replaced some #ifdef _WIN32 statements by #ifdef HAVE_WINDOWS_H or #ifdef
 *  __CYGWIN__ respectively to reflect the fact that the latest Cygwin/gcc
 *  version does not define _WIN32 any more.
 *
 *  Revision 1.6  2001/06/01 15:51:13  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/12/19 12:24:27  meichel
 *  updated tlstrans for Irix 5 compatibility issues.
 *
 *  Revision 1.4  2000/10/19 08:12:29  meichel
 *  Fixed dcmtls module so that openssl headers are includes only
 *    if the symbol WITH_OPENSSL is defined.
 *
 *  Revision 1.3  2000/10/11 16:15:35  meichel
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

