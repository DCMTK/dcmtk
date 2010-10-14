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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSIPCMessage
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dcompat.h"

BEGIN_EXTERN_C
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <winbase.h>
#endif
END_EXTERN_C

#include "dcmtk/dcmpstat/dvpsmsg.h"
#include "dcmtk/ofstd/ofstring.h"    /* for class OFString */
#include "dcmtk/ofstd/ofbmanip.h"    /* for OFBitmanipTemplate<> */
#include "dcmtk/dcmdata/dcswap.h"      /* for swapIfNecessary() */
#include "dcmtk/dcmnet/dcmtrans.h"    /* for class DcmTransportConnection */

/* --------------- class DVPSIPCMessage --------------- */

#define PAYLOAD_ALLOCATION_UNIT 1024
#define PAYLOAD_OFFSET 8

// constants for message type
const Uint32 DVPSIPCMessage::OK                                         = 0;
const Uint32 DVPSIPCMessage::requestApplicationID                       = 1;
const Uint32 DVPSIPCMessage::assignApplicationID                        = 2;
const Uint32 DVPSIPCMessage::applicationTerminates                      = 3;
const Uint32 DVPSIPCMessage::receivedUnencryptedDICOMConnection         = 5;
const Uint32 DVPSIPCMessage::receivedEncryptedDICOMConnection           = 7;
const Uint32 DVPSIPCMessage::connectionClosed                           = 9;
const Uint32 DVPSIPCMessage::connectionAborted                          = 11;
const Uint32 DVPSIPCMessage::requestedUnencryptedDICOMConnection        = 13;
const Uint32 DVPSIPCMessage::requestedEncryptedDICOMConnection          = 15;
const Uint32 DVPSIPCMessage::receivedDICOMObject                        = 17;
const Uint32 DVPSIPCMessage::sentDICOMObject                            = 19;

// message status constants
const Uint32 DVPSIPCMessage::statusOK                                   = 0;
const Uint32 DVPSIPCMessage::statusWarning                              = 1;
const Uint32 DVPSIPCMessage::statusError                                = 2;

// client type constants
const Uint32 DVPSIPCMessage::clientOther                                = 0;
const Uint32 DVPSIPCMessage::clientStoreSCP                             = 1;
const Uint32 DVPSIPCMessage::clientStoreSCU                             = 2;
const Uint32 DVPSIPCMessage::clientPrintSCP                             = 3;
const Uint32 DVPSIPCMessage::clientPrintSCU                             = 4;
const Uint32 DVPSIPCMessage::clientQRSCP                                = 5;


DVPSIPCMessage::DVPSIPCMessage()
: messageType(OK)
, payloadUsed(PAYLOAD_OFFSET)
, payloadAllocated(PAYLOAD_ALLOCATION_UNIT)
, payloadReadOffset(PAYLOAD_OFFSET)
, payload(NULL)
{
  payload = new unsigned char[payloadAllocated];
}

DVPSIPCMessage::DVPSIPCMessage(const DVPSIPCMessage& copy)
: messageType(copy.messageType)
, payloadUsed(copy.payloadUsed)
, payloadAllocated(copy.payloadAllocated)
, payloadReadOffset(copy.payloadReadOffset)
, payload(NULL)
{
  payload = new unsigned char[payloadAllocated];
  OFBitmanipTemplate<unsigned char>::copyMem(copy.payload, payload, payloadUsed);
}

DVPSIPCMessage::~DVPSIPCMessage()
{
  delete[] payload;
}

DVPSIPCMessage& DVPSIPCMessage::operator=(const DVPSIPCMessage& copy)
{
  messageType = copy.messageType;
  payloadUsed = copy.payloadUsed;
  payloadReadOffset = copy.payloadReadOffset;
  if (payloadAllocated < payloadUsed)
  {
    delete[] payload;
    payloadAllocated = copy.payloadAllocated;
    payload = new unsigned char[payloadAllocated];
  }
  OFBitmanipTemplate<unsigned char>::copyMem(copy.payload, payload, payloadUsed);
  return *this;
}

void DVPSIPCMessage::resizePayload(Uint32 i)
{
  Uint32 requiredSize = payloadUsed+i;
  if (requiredSize < payloadAllocated) return;

  while (payloadAllocated < requiredSize) payloadAllocated += PAYLOAD_ALLOCATION_UNIT;
  unsigned char *newpayload = new unsigned char[payloadAllocated];
  OFBitmanipTemplate<unsigned char>::copyMem(payload, newpayload, payloadUsed);
  delete[] payload;
  payload = newpayload;
  return;
}

void DVPSIPCMessage::addStringToPayload(const char *str)
{
  Uint32 length = 0;
  if (str) length = strlen(str); else str = "";
  Uint32 padBytes = 4 - (length % 4);
  resizePayload(sizeof(Uint32)+length+padBytes);

  // write string length
  addIntToPayload(length+padBytes);  

  // write string
  strcpy((char *)(payload + payloadUsed), str);
  payloadUsed += length;

  // write pad bytes
  for (Uint32 i=0; i < padBytes; i++) *(payload + payloadUsed++) = 0;
  return;
}

void DVPSIPCMessage::addIntToPayload(Uint32 i)
{
  resizePayload(sizeof(Uint32));
  unsigned char *target = payload + payloadUsed;
  // write integer value
  *(Uint32 *)target = i;
  // and swap to big endian
  swapIfNecessary(EBO_BigEndian, gLocalByteOrder, target, sizeof(Uint32), sizeof(Uint32));
  payloadUsed += sizeof(Uint32);
  return;
}

OFBool DVPSIPCMessage::extractStringFromPayload(OFString& str)
{
  Uint32 length = 0;
  if (! extractIntFromPayload(length)) return OFFalse;
  // check if we have sufficient data available
  if (payloadReadOffset + length > payloadUsed) return OFFalse;

  str = (const char *)(payload+payloadReadOffset); // guaranteed to be zero terminated string
  payloadReadOffset += length;
  return OFTrue;  
}

OFBool DVPSIPCMessage::extractIntFromPayload(Uint32& i)
{
  // check if we have sufficient data available
  if (payloadReadOffset + sizeof(Uint32) > payloadUsed) return OFFalse;

  // copy integer into temporary buffer and adjust byte order there
  unsigned char *temp = new unsigned char[sizeof(Uint32)+8]; // allocate a bit more than needed to be safe  
  OFBitmanipTemplate<unsigned char>::copyMem(payload+payloadReadOffset, temp, sizeof(Uint32));
  swapIfNecessary(gLocalByteOrder, EBO_BigEndian, temp, sizeof(Uint32), sizeof(Uint32));
  payloadReadOffset += sizeof(Uint32);
  
  i = *(Uint32 *)temp;
  delete[] temp;  
  return OFTrue;
}

void DVPSIPCMessage::rewindPayload()
{
  payloadReadOffset = PAYLOAD_OFFSET;
}

void DVPSIPCMessage::erasePayload()
{
  payloadUsed = PAYLOAD_OFFSET;
}

OFBool DVPSIPCMessage::send(DcmTransportConnection &connection)
{
  // adjust message type and length
  *(Uint32 *)payload = messageType;  
  *(Uint32 *)(payload + sizeof(Uint32)) = (payloadUsed - PAYLOAD_OFFSET);
  swapIfNecessary(EBO_BigEndian, gLocalByteOrder, payload, 2*sizeof(Uint32), sizeof(Uint32));

  // send
  if (connection.write(payload, (size_t)payloadUsed) <= 0) return OFFalse;
  return OFTrue;
}

OFBool DVPSIPCMessage::receive(DcmTransportConnection &connection)
{
  payloadReadOffset = PAYLOAD_OFFSET;

  // read message type and payload length
  if (connection.read(payload, 2*sizeof(Uint32)) <= 0) return OFFalse;
  swapIfNecessary(gLocalByteOrder, EBO_BigEndian, payload, 2*sizeof(Uint32), sizeof(Uint32));
  messageType = *(Uint32 *)payload;
  payloadUsed = *(Uint32 *)(payload+sizeof(Uint32));

  // check if we need to allocate more memory
  Uint32 requiredSize = payloadUsed + PAYLOAD_OFFSET;
  if (requiredSize > payloadAllocated)
  {
    delete[] payload;
    while (payloadAllocated < requiredSize) payloadAllocated += PAYLOAD_ALLOCATION_UNIT;
    payload = new unsigned char[payloadAllocated];
  }

  // read payload if any
  if (payloadUsed > 0) 
  {
    if (connection.read(payload+PAYLOAD_OFFSET, (size_t)payloadUsed) <= 0) 
    {
      payloadUsed = PAYLOAD_OFFSET;
      return OFFalse;
    } else {
      payloadUsed += PAYLOAD_OFFSET;
      return OFTrue;
    }
  } else payloadUsed = PAYLOAD_OFFSET;
  return OFTrue;
}

/* --------------- class DVPSIPCClient --------------- */

DVPSIPCClient::DVPSIPCClient(Uint32 clientType, const char *txt, int thePort, OFBool keepOpen)
: port(thePort)
, serverActive(OFTrue)
, applicationID(0)
, keepConnectionOpen(keepOpen)
, connection(NULL)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::requestApplicationID);
  msg.addIntToPayload(clientType);
  msg.addIntToPayload(DVPSIPCMessage::statusOK);
  msg.addStringToPayload(txt);
  if (performTransaction(msg))
  {
    if ((msg.getMessageType() != DVPSIPCMessage::assignApplicationID) || (! msg.extractIntFromPayload(applicationID)))
    {      
      // protocol violation
      serverActive = OFFalse;	
    }
  } else {
    serverActive = OFFalse;	
  }
  return;
}
 
DVPSIPCClient::~DVPSIPCClient()
{
  if (connection)
  {
    connection->close();
    delete connection;
  }
}

void DVPSIPCClient::requestConnection()
{
  if (connection) return; // connection already open

  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) return;

  struct hostent *hp = gethostbyname("localhost");
  if (NULL == hp) return;

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = (unsigned short) htons(port);
  memcpy(&server.sin_addr, hp->h_addr, (size_t) hp->h_length);

  if (connect(s, (struct sockaddr *) & server, sizeof(server)) < 0)
  {
#ifdef HAVE_WINSOCK_H
    (void) shutdown(s,  1 /* SD_SEND */); 
    (void) closesocket(s);
#else
    (void) close(s);
#endif
    return;
  }
  connection = new DcmTCPConnection(s);
}

OFBool DVPSIPCClient::performTransaction(DVPSIPCMessage& msg)
{
  if (!serverActive) return OFFalse;

  requestConnection();
  // this would be the right place to retry connections
  if (connection == NULL) return OFFalse;

  OFBool result = msg.send(*connection);
  if (result) result = msg.receive(*connection);

  if (! keepConnectionOpen)
  {
    connection->close();
    delete connection;
    connection = NULL;
  }

  return result;
}

void DVPSIPCClient::notifyApplicationTerminates(Uint32 status)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::applicationTerminates);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  performTransaction(msg);
}

void DVPSIPCClient::notifyReceivedUnencryptedDICOMConnection(Uint32 status, const char *txt)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::receivedUnencryptedDICOMConnection);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  msg.addStringToPayload(txt);
  performTransaction(msg);
}

void DVPSIPCClient::notifyReceivedEncryptedDICOMConnection(Uint32 status, const char *txt)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::receivedEncryptedDICOMConnection);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  msg.addStringToPayload(txt);
  performTransaction(msg);
}

void DVPSIPCClient::notifyConnectionClosed(Uint32 status)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::connectionClosed);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  performTransaction(msg);
}

void DVPSIPCClient::notifyConnectionAborted(Uint32 status, const char *txt)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::connectionAborted);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  msg.addStringToPayload(txt);
  performTransaction(msg);
}

void DVPSIPCClient::notifyRequestedUnencryptedDICOMConnection(Uint32 status, const char *txt)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::requestedUnencryptedDICOMConnection);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  msg.addStringToPayload(txt);
  performTransaction(msg);
}

void DVPSIPCClient::notifyRequestedEncryptedDICOMConnection(Uint32 status, const char *txt)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::requestedEncryptedDICOMConnection);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  msg.addStringToPayload(txt);
  performTransaction(msg);
}

void DVPSIPCClient::notifyReceivedDICOMObject(Uint32 status, const char *txt)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::receivedDICOMObject);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  msg.addStringToPayload(txt);
  performTransaction(msg);
}

void DVPSIPCClient::notifySentDICOMObject(Uint32 status, const char *txt)
{
  DVPSIPCMessage msg;
  msg.setMessageType(DVPSIPCMessage::sentDICOMObject);
  msg.addIntToPayload(applicationID);
  msg.addIntToPayload(status);
  msg.addStringToPayload(txt);
  performTransaction(msg);
}


/*
 *  $Log: dvpsmsg.cc,v $
 *  Revision 1.12  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2005/12/08 15:46:35  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2001/06/05 10:30:56  joergr
 *  Replaced some #ifdef _WIN32 statements by #ifdef HAVE_WINDOWS_H or #ifdef
 *  __CYGWIN__ respectively to reflect the fact that the latest Cygwin/gcc
 *  version does not define _WIN32 any more.
 *
 *  Revision 1.9  2001/06/01 15:50:33  meichel
 *  Updated copyright header
 *
 *  Revision 1.8  2000/12/19 12:12:53  meichel
 *  Inclusion of TCP header files now protected by extern "C", needed on Ultrix
 *
 *  Revision 1.7  2000/12/12 16:45:40  meichel
 *  Minor changes to keep gcc 2.7.x on SunOS 4.1.3 happy
 *
 *  Revision 1.6  2000/11/14 13:27:15  meichel
 *  Fixed system include files for Linux.
 *
 *  Revision 1.5  2000/11/08 18:38:23  meichel
 *  Updated dcmpstat IPC protocol for additional message parameters
 *
 *  Revision 1.4  2000/10/23 12:19:52  joergr
 *  Replaced non-Unix style newline in preprocessor statement.
 *
 *  Revision 1.3  2000/10/16 11:43:38  joergr
 *  Added explicit typecast to avoid compiler warnings (Sun CC 2.0.1).
 *
 *  Revision 1.2  2000/10/11 16:16:25  meichel
 *  Updated includes for Win32 environment
 *
 *  Revision 1.1  2000/10/10 12:24:41  meichel
 *  Added extensions for IPC message communication
 *
 *
 */


