/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

#ifdef HAVE_WINDOWS_H
// on Windows, we need Winsock2 for network functions
#include <winsock2.h>
#endif

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
#include <openssl/err.h>
#include <openssl/ssl.h>
END_EXTERN_C

#ifdef DCMTK_HAVE_POLL
#include <poll.h>
#endif

#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#include "dcmtk/dcmnet/dcompat.h"    /* to make sure we have a select prototype */
#include "dcmtk/dcmnet/diutil.h"


DcmTLSConnection::DcmTLSConnection(DcmNativeSocketType openSocket, SSL *newTLSConnection)
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
      logTLSConnection();
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
      logTLSConnection();
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
  if (tlsConnection) return SSL_read(tlsConnection, OFreinterpret_cast(char*, buf), OFstatic_cast(int, nbyte));
  errno = EIO; /* IO Error */
  return -1;
}

ssize_t DcmTLSConnection::write(void *buf, size_t nbyte)
{
  if (tlsConnection) return SSL_write(tlsConnection, OFreinterpret_cast(char*, buf), OFstatic_cast(int, nbyte));
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
  int nfound;

#ifndef DCMTK_HAVE_POLL
  fd_set fdset;
  FD_ZERO(&fdset);
  FD_SET(getSocket(), &fdset);
#endif
  t.tv_sec = timeout;
  t.tv_usec = 0;

#ifdef DCMTK_HAVE_POLL
  struct pollfd pfd[] =
  {
   { getSocket(), POLLIN, 0 }
  };
  nfound = poll(pfd, 1, t.tv_sec*1000+(t.tv_usec/1000));
#else
#ifdef HAVE_INTP_SELECT
  nfound = select(OFstatic_cast(int, getSocket() + 1), (int *)(&fdset), NULL, NULL, &t);
#else
  // This is safe because on Windows the first select() parameter is ignored anyway
  nfound = select(OFstatic_cast(int, getSocket() + 1), &fdset, NULL, NULL, &t);
#endif /* HAVE_INTP_SELECT */
#endif /* DCMTK_HAVE_POLL */

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DU_logSelectResult(nfound);
  }
  if (nfound <= 0) return OFFalse;
  else
  {
#ifdef DCMTK_HAVE_POLL
    if (pfd[0].revents & POLLIN) return OFTrue;
    else return OFFalse;  /* This should not really happen */
#else
    if (FD_ISSET(getSocket(), &fdset)) return OFTrue;
    else return OFFalse;  /* This should not really happen */
#endif
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
    str = "Transport connection: TLS over TCP/IP\n  Error: No Connection";
    return str;
  }

  X509 *peerCert = SSL_get_peer_certificate(tlsConnection);
  OFOStringStream stream;
  stream << "Transport connection: TLS over TCP/IP" << OFendl
         << "  Protocol    : " << SSL_get_version(tlsConnection) << OFendl
         << "  Ciphersuite : " << SSL_CIPHER_get_name(SSL_get_current_cipher(tlsConnection))
         << ", encryption: " << SSL_CIPHER_get_bits(SSL_get_current_cipher(tlsConnection), NULL) << " bits" << OFendl
         << DcmTLSTransportLayer::dumpX509Certificate(peerCert);
  // stream << OFendl << "Certificate verification: " << X509_verify_cert_error_string(SSL_get_verify_result(tlsConnection));
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

void DcmTLSConnection::logTLSConnection()
{
  OFString s;
  dumpConnectionParameters(s);
  DCMTLS_DEBUG(
    "================== BEGIN TLS CONNECTION DETAILS =================\n" << s <<
    "\n=================== END TLS CONNECTION DETAILS ==================");
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
