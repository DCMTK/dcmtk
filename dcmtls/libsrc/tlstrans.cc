/*
 *
 *  Copyright (C) 1998-2023, OFFIS e.V.
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
#include "dcmtk/dcmtls/tlscond.h"

static OFCondition convertSSLError(int sslError)
{
  unsigned long e;
  switch (sslError)
  {
    case SSL_ERROR_NONE:
      return EC_Normal;
      break;
    case SSL_ERROR_SYSCALL:
      // there may or may not be an error code in the error queue.
      // If there is an error, report it; otherwise report a generic OpenSSL I/O error.
      e = ERR_get_error();
      if (e == 0)
        return DCMTLS_EC_OpenSSLIOError;
        else return DcmTLSTransportLayer::convertOpenSSLError(e, OFFalse);
      break;
    case SSL_ERROR_SSL:
      return DcmTLSTransportLayer::convertOpenSSLError(ERR_get_error(), OFFalse);
      break;
    case SSL_ERROR_WANT_READ:
      return DCMTLS_EC_TLSReadOperationDidNotComplete;
      break;
    case SSL_ERROR_WANT_WRITE:
      return DCMTLS_EC_TLSWriteOperationDidNotComplete;
      break;
    case SSL_ERROR_WANT_X509_LOOKUP:
      return DCMTLS_EC_TLSX509LookupOperationDidNotComplete;
      break;
    case SSL_ERROR_ZERO_RETURN:
      return DCMTLS_EC_TLSConnectionClosedByPeer;
      break;
    case SSL_ERROR_WANT_CONNECT:
      return DCMTLS_EC_TLSConnectOperationDidNotComplete;
      break;
    case SSL_ERROR_WANT_ACCEPT:
      return DCMTLS_EC_TLSAcceptOperationDidNotComplete;
      break;

#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC
    // SSL_ERROR_WANT_ASYNC is defined starting with OpenSSL 1.1.0
    case SSL_ERROR_WANT_ASYNC:
      return DCMTLS_EC_TLSAsyncOperationDidNotComplete;
      break;
#endif

#ifdef HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC_JOB
    // SSL_ERROR_WANT_ASYNC_JOB is defined starting with OpenSSL 1.1.0
    case SSL_ERROR_WANT_ASYNC_JOB:
      return DCMTLS_EC_TLSAsyncJobCouldNotBeStarted;
      break;
#endif

#if HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_CLIENT_HELLO_CB
    // SSL_ERROR_WANT_CLIENT_HELLO_CB is defined starting with OpenSSL 1.1.1
    case SSL_ERROR_WANT_CLIENT_HELLO_CB:
      return DCMTLS_EC_TLSClientHelloCallbackNeeded;
      break;
#endif
  }
  return DCMTLS_EC_OtherSSLError;
}


DcmTLSConnection::DcmTLSConnection(DcmNativeSocketType openSocket, SSL *newTLSConnection)
: DcmTransportConnection(openSocket)
, tlsConnection(newTLSConnection)
{
}

DcmTLSConnection::~DcmTLSConnection()
{
  close();
}

OFCondition DcmTLSConnection::serverSideHandshake()
{
  if (tlsConnection == NULL) return DCMTLS_EC_NoTLSTransportConnectionPresent;
  int result = SSL_get_error(tlsConnection, SSL_accept(tlsConnection));

  // if the certificate verification has failed, the certificate is already
  // unavailable at this point. We know that something has gone wrong, but
  // OpenSSL does not tell us who tried to connect.
  if (result == SSL_ERROR_NONE) logTLSConnection();

  return convertSSLError(result);
}

OFCondition DcmTLSConnection::clientSideHandshake()
{
  DCMTLS_TRACE("Starting TLS client handshake");
  if (tlsConnection == NULL) return DCMTLS_EC_NoTLSTransportConnectionPresent;
  int result = SSL_get_error(tlsConnection, SSL_connect(tlsConnection));
  if (result == SSL_ERROR_NONE) logTLSConnection();

  return convertSSLError(result);
}

OFCondition DcmTLSConnection::renegotiate(const char *newSuite)
{
  if (tlsConnection == NULL) return DCMTLS_EC_NoTLSTransportConnectionPresent;
  if (newSuite == NULL) return EC_IllegalCall;

  int result = SSL_get_error(tlsConnection, SSL_set_cipher_list(tlsConnection, newSuite));
  if (result != SSL_ERROR_NONE) return convertSSLError(result);

  result = SSL_get_error(tlsConnection, SSL_renegotiate(tlsConnection));
  return convertSSLError(result);
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
    // execute SSL_shutdown(), which sends the TLS close_notify alert to the peer,
    // unless we are the parent process after a fork() operation and this connection
    // will be handled by the client.
    if (! isParentProcessMode()) SSL_shutdown(tlsConnection);
    SSL_free(tlsConnection);
    tlsConnection = NULL;
  }
  closeTransportConnection();
}

void DcmTLSConnection::closeTransportConnection()
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
    setSocket(OFstatic_cast(DcmNativeSocketType, (-1)));
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

void DcmTLSConnection::logTLSConnection()
{
  OFString s;
  DCMTLS_DEBUG(
    "================== BEGIN TLS CONNECTION DETAILS =================\n" << dumpConnectionParameters(s) <<
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
