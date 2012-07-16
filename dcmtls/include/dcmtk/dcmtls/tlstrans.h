/*
 *
 *  Copyright (C) 1998-2011, OFFIS e.V.
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
 *    classes: DcmTransportConnection
 *
 */

#ifndef TLSTRANS_H
#define TLSTRANS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcmtrans.h"    /* for DcmTransportConnection */
#include "dcmtk/ofstd/ofstream.h"    /* for ostream */
#include "dcmtk/dcmtls/tlsdefin.h"

#ifdef WITH_OPENSSL

BEGIN_EXTERN_C
#include <openssl/ssl.h>
END_EXTERN_C


/** this class represents a TLS (Transport Layer Security) V1 based secure
 *  transport connection.
 */
class DCMTK_DCMTLS_EXPORT DcmTLSConnection: public DcmTransportConnection
{
public:

  /** constructor.
   *  @param openSocket TCP/IP socket to be used for the transport connection.
   *    the connection must already be establised on socket level. This object
   *    takes over control of the socket.
   *  @param newTLSConnection pointer to intialized OpenSSL connection object
   *    to be used for this connection.
   */
  DcmTLSConnection(int openSocket, SSL *newTLSConnection);

  /** destructor
   */
  virtual ~DcmTLSConnection();

  /** performs server side handshake on established socket.
   *  This function is used to establish a secure transport connection
   *  over the established TCP connection.
   *  @return TCS_ok if successful, an error code otherwise.
   */
  virtual DcmTransportLayerStatus serverSideHandshake();

  /** performs client side handshake on established socket.
   *  This function is used to establish a secure transport connection
   *  over the established TCP connection.
   *  @return TCS_ok if successful, an error code otherwise.
   */
  virtual DcmTransportLayerStatus clientSideHandshake();

  /** performs a re-negotiation of the connection with different
   *  connection parameters. Used to change the parameters of the
   *  secure transport connection.
   *  @param newSuite string identifying the ciphersuite to be negotiated.
   *  @return TCS_ok if successful, an error code otherwise.
   */
  virtual DcmTransportLayerStatus renegotiate(const char *newSuite);

  /** attempts to read nbyte bytes from the transport connection and
   *  writes them into the given buffer.
   *  @param buf buffer
   *  @param nbyte number of bytes to read
   *  @return number of bytes read, negative number if unsuccessful.
   */
  virtual ssize_t read(void *buf, size_t nbyte);

  /** attempts to write nbyte bytes from the given buffer
   *  to the transport connection.
   *  @param buf buffer
   *  @param nbyte number of bytes to write
   *  @return number of bytes written, negative number if unsuccessful.
   */
  virtual ssize_t write(void *buf, size_t nbyte);

  /** Closes the transport connection. If a secure connection
   *  is used, a closure alert is sent before the connection
   *  is closed.
   */
  virtual void close();

  /** returns the size in bytes of the peer certificate of a secure connection.
   *  @return peer certificate length in bytes
   */
  virtual unsigned long getPeerCertificateLength();

  /* copies the peer certificate of a secure connection into a buffer
   * specified by the caller. If the buffer is too small to hold the
   * certificate, nothing is copied and zero is returned.
   * @param buf buffer into which the certificate is written
   * @param bufLen size of the buffer in bytes
   * @return number of bytes written, always less or equal bufLen.
   */
  virtual unsigned long getPeerCertificate(void *buf, unsigned long bufLen);

  /** checks if data is available to be read on the transport connection.
   *  @param timeout maximum number of seconds to wait if no data is available.
   *     If this parameter is 0, the function does not block.
   *  @returns OFTrue if data is available, OFFalse otherwise.
   */
  virtual OFBool networkDataAvailable(int timeout);

  /** returns OFTrue if this connection is a transparent TCP connection,
   *  OFFalse if the connection is a secure connection.
   */
  virtual OFBool isTransparentConnection();

  /** dump the characteristics of the current connection
   *  @param str the string to dump into
   *  @return reference to string
   */
  virtual OFString& dumpConnectionParameters(OFString& str);

  /** returns an error string for a given error code.
   *  @param code error code
   *  @return description for error code
   */
  virtual const char *errorString(DcmTransportLayerStatus code);

private:

  /// private undefined copy constructor
  DcmTLSConnection(const DcmTLSConnection&);

  /// private undefined assignment operator
  DcmTLSConnection& operator=(const DcmTLSConnection&);

  /// pointer to the TLS connection structure used by the OpenSSL library
  SSL *tlsConnection;

  /// last error code returned by the OpenSSL library
  unsigned long lastError;
};

#endif /* WITH_OPENSSL */

#endif
