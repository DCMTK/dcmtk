/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
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
 *    classes: DcmTransportLayer
 *
 */

#ifndef DCMLAYER_H
#define DCMLAYER_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"

#include "dcmtk/ofstd/ofutil.h"

#include "dcmtk/dcmnet/dndefine.h"
#include "dcmtk/dcmnet/dntypes.h"

/** @file dcmlayer.h
 *  @brief type definitions and classes for transport connections
 */

class DcmTransportConnection;

/** factory class which creates transport layer connections.
 *  Base class only supports transparent TCP connections, subclasses
 *  may also support secure transport layer connections.
 */
class DCMTK_DCMNET_EXPORT DcmTransportLayer
{
public:

  /** constructor.
   */
  DcmTransportLayer() { /* empty */ }

  /** move constructor.
   *  @param rhs an rvalue reference to another DcmTransportLayer object that
   *    will be moved.
   */
  DcmTransportLayer(OFrvalue_ref(DcmTransportLayer) rhs) { OFstatic_cast(void, rhs); }

  /** move assignment.
   *  @param rhs an rvalue reference to another DcmTransportLayer object that will
   *    be move assigned.
   *  @return *this.
   */
  DcmTransportLayer& operator=(OFrvalue_ref(DcmTransportLayer) rhs) { OFstatic_cast(void, rhs); return *this; }

  /// destructor
  virtual ~DcmTransportLayer();

  /** factory method that returns a new transport connection for the
   *  given socket.  Depending on the second parameter, either a transparent
   *  or a secure connection is established.  If the object cannot be created
   *  (e. g. because no secure layer is available), returns NULL.
   *  @param openSocket TCP/IP socket to be used for the transport connection.
   *    the connection must already be established on socket level. If a non-null
   *    pointer is returned, the new connection object takes over control of the socket.
   *  @param useSecureLayer if true, a secure layer is used. If false, a
   *    transparent layer is used.
   *  @return pointer to new connection object if successful, NULL otherwise.
   */
  virtual DcmTransportConnection *createConnection(DcmNativeSocketType openSocket, OFBool useSecureLayer);

private:

  /// private undefined copy constructor
  DcmTransportLayer(const DcmTransportLayer&);

  /// private undefined assignment operator
  DcmTransportLayer& operator=(const DcmTransportLayer&);

};


#endif
