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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSIPCMessage
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-10-10 12:24:36 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSMSG_H__
#define __DVPSMSG_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"     /* for Uint32 */

class OFString;
class DcmTransportConnection;

/** class for IPC message exchange between different processes of the 
 *  DICOMscope application
 */
class DVPSIPCMessage
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
class DVPSIPCClient
{
public:

  /** constructor
   *  @param thePort TCP/IP port on which the server is listening
   *  @param keepOpen flag indicating whether the connection should be kept
   *    open all the time or should be opened/closed for each transaction.
   */
  DVPSIPCClient(int thePort, OFBool keepOpen);
  
  /// destructor
  virtual ~DVPSIPCClient();

  /** sends ApplicationTerminates notification to server.
   */
  void notifyApplicationTerminates();

  /** sends ReceivedUnencryptedDICOMConnection notification to server.
   *  @param txt textual description of notification for server
   */
  void notifyReceivedUnencryptedDICOMConnection(const char *txt);

  /** sends ReceivedEncryptedDICOMConnection notification to server.
   *  @param txt textual description of notification for server
   */
  void notifyReceivedEncryptedDICOMConnection(const char *txt);

  /** sends ConnectionClosed notification to server.
   */
  void notifyConnectionClosed();

  /** sends ConnectionAborted notification to server.
   *  @param txt textual description of notification for server
   */
  void notifyConnectionAborted(const char *txt);

  /** sends RequestedUnencryptedDICOMConnection notification to server.
   *  @param txt textual description of notification for server
   */
  void notifyRequestedUnencryptedDICOMConnection(const char *txt);

  /** sends RequestedEncryptedDICOMConnection notification to server.
   *  @param txt textual description of notification for server
   */
  void notifyRequestedEncryptedDICOMConnection(const char *txt);

  /** sends ReceivedDICOMObject notification to server.
   */
  void notifyReceivedDICOMObject();

  /** sends SentDICOMObject notification to server.
   */
  void notifySentDICOMObject();

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

/*
 *  $Log: dvpsmsg.h,v $
 *  Revision 1.1  2000-10-10 12:24:36  meichel
 *  Added extensions for IPC message communication
 *
 *
 */
