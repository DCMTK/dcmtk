/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSIPCMessage
 *
 */

#ifndef DVPSMSG_H
#define DVPSMSG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmdata/dctypes.h"    /* for Uint32 */
#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */

class DcmTransportConnection;

/** class for IPC message exchange between different processes of the
 *  DICOMscope application
 */
class DCMTK_DCMPSTAT_EXPORT DVPSIPCMessage
{
public:

  /// default constructor
  DVPSIPCMessage();

  /// copy constructor
  DVPSIPCMessage(const DVPSIPCMessage& copy);

  /// destructor
  virtual ~DVPSIPCMessage();

  /// copy assignment operator
  DVPSIPCMessage& operator=(const DVPSIPCMessage&);

  /** sets the message type
   *  @param msgtype new message type
   */
  void setMessageType(Uint32 msgtype) { messageType = msgtype; }

  /** returns the message type
   *  @return message type
   */
  Uint32 getMessageType() { return messageType; }

  /** adds a character string into the message payload.
   *  @param str zero terminated string, may be NULL (in which case an empty string is added)
   */
  void addStringToPayload(const char *str);

  /** adds an integer into the message payload.
   *  @param value to write
   */
  void addIntToPayload(Uint32 i);

  /** extracts a string from the message payload
   *  and copies it into the given str object.
   *  @param str string is written into this parameter
   *  @return OFTrue if successful, OFFalse otherwise
   */
  OFBool extractStringFromPayload(OFString& str);

  /** extracts an integer from the message payload.
   *  @param i integer is written into this parameter
   *  @return OFTrue if successful, OFFalse otherwise
   */
  OFBool extractIntFromPayload(Uint32& i);

  /** rewinds the read offset to the beginning of the message payload
   */
  void rewindPayload();

  /** removes all payload
   */
  void erasePayload();

  /** sends the current message over the given transport connection.
   *  @param connection transport connection to be used
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool send(DcmTransportConnection &connection);

  /** receives a messages from the given transport connection
   *  and stores it in the current object, replacing any existing
   *  payload.
   *  @param connection transport connection to be used
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool receive(DcmTransportConnection &connection);


  // constants for message type
  static const Uint32 OK;
  static const Uint32 requestApplicationID;
  static const Uint32 assignApplicationID;
  static const Uint32 applicationTerminates;
  static const Uint32 receivedUnencryptedDICOMConnection;
  static const Uint32 receivedEncryptedDICOMConnection;
  static const Uint32 connectionClosed;
  static const Uint32 connectionAborted;
  static const Uint32 requestedUnencryptedDICOMConnection;
  static const Uint32 requestedEncryptedDICOMConnection;
  static const Uint32 receivedDICOMObject;
  static const Uint32 sentDICOMObject;

  // message status constants
  static const Uint32 statusOK;      // OK
  static const Uint32 statusWarning; // warning
  static const Uint32 statusError;   // error

  // client type constants
  static const Uint32 clientOther;    // client is of unspecified type
  static const Uint32 clientStoreSCP; // client is Store SCP
  static const Uint32 clientStoreSCU; // client is Store SCU
  static const Uint32 clientPrintSCP; // client is Print SCP
  static const Uint32 clientPrintSCU; // client is Print SCU
  static const Uint32 clientQRSCP;    // client is Query/Retrieve (Find/Move/Get) SCP

private:

  /** resize payload if necessary such that at least i bytes can be written
   *  @param i number of bytes required in buffer
   */
  void resizePayload(Uint32 i);

  /// type of message
  Uint32 messageType;

  /// number of bytes actually used in payload
  Uint32 payloadUsed;

  /// number of bytes allocated in payload
  Uint32 payloadAllocated;

  /// read offset into payload in bytes
  Uint32 payloadReadOffset;

  /// pointer to raw payload data in big endian byte order
  unsigned char *payload;
};


/** a client for IPC message exchange between different processes of the
 *  DICOMscope application
 */
class DCMTK_DCMPSTAT_EXPORT DVPSIPCClient
{
public:

  /** constructor
   *  @param clientType type of client application, see constants defined in DVPSIPCMessage
   *  @param txt textual description of client application
   *  @param thePort TCP/IP port on which the server is listening
   *  @param keepOpen flag indicating whether the connection should be kept
   *    open all the time or should be opened/closed for each transaction.
   */
  DVPSIPCClient(Uint32 clientType, const char *txt, int thePort, OFBool keepOpen);

  /// destructor
  virtual ~DVPSIPCClient();

  /** sends ApplicationTerminates notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   */
  void notifyApplicationTerminates(Uint32 status);

  /** sends ReceivedUnencryptedDICOMConnection notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   *  @param txt textual description of notification for server
   */
  void notifyReceivedUnencryptedDICOMConnection(Uint32 status, const char *txt);

  /** sends ReceivedEncryptedDICOMConnection notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   *  @param txt textual description of notification for server
   */
  void notifyReceivedEncryptedDICOMConnection(Uint32 status, const char *txt);

  /** sends ConnectionClosed notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   */
  void notifyConnectionClosed(Uint32 status);

  /** sends ConnectionAborted notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   *  @param txt textual description of notification for server
   */
  void notifyConnectionAborted(Uint32 status, const char *txt);

  /** sends RequestedUnencryptedDICOMConnection notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   *  @param txt textual description of notification for server
   */
  void notifyRequestedUnencryptedDICOMConnection(Uint32 status, const char *txt);

  /** sends RequestedEncryptedDICOMConnection notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   *  @param txt textual description of notification for server
   */
  void notifyRequestedEncryptedDICOMConnection(Uint32 status, const char *txt);

  /** sends ReceivedDICOMObject notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   *  @param txt textual description of DICOM object
   */
  void notifyReceivedDICOMObject(Uint32 status, const char *txt);

  /** sends SentDICOMObject notification to server.
   *  @param Uint32 message status, see constants defined in DVPSIPCMessage
   *  @param txt textual description of DICOM object
   */
  void notifySentDICOMObject(Uint32 status, const char *txt);

  /** checks whether the message server has been found active
   *  upon creation of this object.
   *  @return OFTrue if server is active, OFFalse otherwise.
   */
  OFBool isServerActive() { return serverActive; }

private:

  /// private undefined copy constructor
  DVPSIPCClient(const DVPSIPCClient& copy);

  /// private undefined copy assignment operator
  DVPSIPCClient& operator=(const DVPSIPCClient&);

  /** request connection to server, store in 'connection' if successful.
   */
  void requestConnection();

  /** perform message transaction with server. If serverActive is false,
   *  no connection attempt is performed.
   *  @param msg contains message to be sent, overwritten with message
   *    received from server if successful
   *  @return OFTrue if successful, OFFalse otherwise
   */
  OFBool performTransaction(DVPSIPCMessage& msg);

  /// TCP/IP port number on which the server is listening
  int port;

  /// true if the first connection attempt has succeeded, false otherwise
  OFBool serverActive;

  /// application ID assigned by the server
  Uint32 applicationID;

  /// flag indicating whether we want to keep the connection open
  OFBool keepConnectionOpen;

  /// current transport connection
  DcmTransportConnection *connection;
};


#endif
