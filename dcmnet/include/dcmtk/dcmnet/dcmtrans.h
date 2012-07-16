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
 *    classes: DcmTransportConnection, DcmTCPConnection
 *
 */

#ifndef DCMTRANS_H
#define DCMTRANS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"     /* for OFBool */
#include "dcmtk/dcmnet/dcmlayer.h"    /* for DcmTransportLayerStatus */
#include "dcmtk/ofstd/ofstream.h"    /* for ostream */

#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

/** this class represents a TCP/IP based transport connection
 *  which can be a transparent TCP/IP socket communication or a
 *  secure transport protocol such as TLS.
 */
class DCMTK_DCMNET_EXPORT DcmTransportConnection
{
public:

  /** constructor.
   *  @param openSocket TCP/IP socket to be used for the transport connection.
   *    the connection must already be establised on socket level. This object
   *    takes over control of the socket.
   */
  DcmTransportConnection(int openSocket);

  /** destructor
   */
  virtual ~DcmTransportConnection();

  /** performs server side handshake on established socket.
   *  This function is used to establish a secure transport connection
   *  over the established TCP connection. Abstract method.
   *  @return TCS_ok if successful, an error code otherwise.
   */
  virtual DcmTransportLayerStatus serverSideHandshake() = 0;

  /** performs client side handshake on established socket.
   *  This function is used to establish a secure transport connection
   *  over the established TCP connection. Abstract method.
   *  @return TCS_ok if successful, an error code otherwise.
   */
  virtual DcmTransportLayerStatus clientSideHandshake() = 0;

  /** performs a re-negotiation of the connection with different
   *  connection parameters. Used to change the parameters of the
   *  secure transport connection. Abstract method.
   *  @param newSuite string identifying the ciphersuite to be negotiated.
   *  @return TCS_ok if successful, an error code otherwise.
   */
  virtual DcmTransportLayerStatus renegotiate(const char *newSuite) = 0;

  /** attempts to read nbyte bytes from the transport connection and
   *  writes them into the given buffer. Abstract method.
   *  @param buf buffer
   *  @param nbyte number of bytes to read
   *  @return number of bytes read, negative number if unsuccessful.
   */
  virtual ssize_t read(void *buf, size_t nbyte) = 0;

  /** attempts to write nbyte bytes from the given buffer
   *  to the transport connection. Abstract method.
   *  @param buf buffer
   *  @param nbyte number of bytes to write
   *  @return number of bytes written, negative number if unsuccessful.
   */
  virtual ssize_t write(void *buf, size_t nbyte) = 0;

  /** Closes the transport connection. If a secure connection
   *  is used, a closure alert is sent before the connection
   *  is closed. Abstract method.
   */
  virtual void close() = 0;

  /** returns the size in bytes of the peer certificate of a secure connection.
   *  May return 0 if connection is transparent TCP/IP.
   *  @return peer certificate length in bytes
   */
  virtual unsigned long getPeerCertificateLength() = 0;

  /* copies the peer certificate of a secure connection into a buffer
   * specified by the caller. If the buffer is too small to hold the
   * certificate, nothing is copied and zero is returned.
   * @param buf buffer into which the certificate is written
   * @param bufLen size of the buffer in bytes
   * @return number of bytes written, always less or equal bufLen.
   */
  virtual unsigned long getPeerCertificate(void *buf, unsigned long bufLen) = 0;

  /** checks if data is available to be read on the transport connection.
   *  Abstract method.
   *  @param timeout maximum number of seconds to wait if no data is available.
   *     If this parameter is 0, the function does not block.
   *  @returns OFTrue if data is available, OFFalse otherwise.
   */
  virtual OFBool networkDataAvailable(int timeout) = 0;

  /** returns OFTrue if this connection is a transparent TCP connection,
   *  OFFalse if the connection is a secure connection.
   */
  virtual OFBool isTransparentConnection() = 0;

  /** dump the characteristics of the current connection
   *  @param str the string to dump into
   *  @return reference to string
   */
  virtual OFString& dumpConnectionParameters(OFString& str) = 0;

  /** prints the characteristics of the current connection
   *  on the given output stream.
   *  @param out output stream
   *  @deprecated Please use the other dumpConnectionParameters() function instead!
   */
  void dumpConnectionParameters(STD_NAMESPACE ostream& out);

  /** returns an error string for a given error code.
   *  @param code error code
   *  @return description for error code
   */
  virtual const char *errorString(DcmTransportLayerStatus code) = 0;

  /** indicates which of the specified transport connections is ready for
   *  reading. If none of the specified transport connections is ready
   *  for reading, this method blocks up to the specified timeout interval
   *  or until one of the connections becomes readable, whatever occurs
   *  first.
   *  @param connections list of transport connections. May contain NULL entries.
   *    Upon successful return of this method, all transport connections which are
   *    not ready for reading are set to NULL in this array.
   *  @param connCount number of entries in connections array.
   *  @param timeout number of seconds for timeout. If timeout is 0, this method
   *    does not block.
   *  @return OFTrue if one or more connections are readable upon return from
   *    this method, OFFalse if no connection is ready for reading.
   */
  static OFBool selectReadableAssociation(DcmTransportConnection *connections[], int connCount, int timeout);

protected:

  /** returns the socket file descriptor managed by this object.
   *  @return socket file descriptor
   */
  int getSocket() { return theSocket; }

  /** set the socket file descriptor managed by this object.
   *  @param socket file descriptor
   */
  void setSocket(int socket) { theSocket = socket; }

private:

  /// private undefined copy constructor
  DcmTransportConnection(const DcmTransportConnection&);

  /// private undefined assignment operator
  DcmTransportConnection& operator=(const DcmTransportConnection&);

  /** indicates which of the specified transport connections is ready for
   *  reading. If none of the specified transport connections is ready
   *  for reading, this method blocks up to the specified timeout interval
   *  or until one of the connections becomes readable, whatever occurs
   *  first. This method uses a safe approach that also works with secure
   *  transport connections, but which may be slower than a select() system
   *  call and consumes slightly more computation time.
   *  @param connections list of transport connections. May contain NULL entries.
   *    Upon successful return of this method, all transport connections which are
   *    not ready for reading are set to NULL in this array.
   *  @param connCount number of entries in connections array.
   *  @param timeout number of seconds for timeout. If timeout is 0, this method
   *    does not block.
   *  @return OFTrue if one or more connections are readable upon return from
   *    this method, OFFalse if no connection is ready for reading.
   */
  static OFBool safeSelectReadableAssociation(DcmTransportConnection *connections[], int connCount, int timeout);

  /** indicates which of the specified transport connections is ready for
   *  reading. If none of the specified transport connections is ready
   *  for reading, this method blocks up to the specified timeout interval
   *  or until one of the connections becomes readable, whatever occurs
   *  first. This method uses the select() system call. It may only be used
   *  with an array of transparent TCP transport connections. This precondition
   *  must be assured by the caller.
   *  @param connections list of transport connections. May contain NULL entries.
   *    Upon successful return of this method, all transport connections which are
   *    not ready for reading are set to NULL in this array.
   *  @param connCount number of entries in connections array.
   *  @param timeout number of seconds for timeout. If timeout is 0, this method
   *    does not block.
   *  @return OFTrue if one or more connections are readable upon return from
   *    this method, OFFalse if no connection is ready for reading.
   */
  static OFBool fastSelectReadableAssociation(DcmTransportConnection *connections[], int connCount, int timeout);

  /// the socket file descriptor used by the transport connection.
  int theSocket;
};


/** this class represents a TCP/IP based transport connection.
 */
class DCMTK_DCMNET_EXPORT DcmTCPConnection: public DcmTransportConnection
{
public:

  /** constructor.
   *  @param openSocket TCP/IP socket to be used for the transport connection.
   *    the connection must already be establised on socket level. This object
   *    takes over control of the socket.
   */
  DcmTCPConnection(int openSocket);

  /** destructor
   */
  virtual ~DcmTCPConnection();

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
   *  May return 0 if connection is transparent TCP/IP.
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
  DcmTCPConnection(const DcmTCPConnection&);

  /// private undefined assignment operator
  DcmTCPConnection& operator=(const DcmTCPConnection&);

};

#endif
