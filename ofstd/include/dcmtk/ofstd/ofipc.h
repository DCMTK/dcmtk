/*
 *
 *  Copyright (C) 2022, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Message based inter-process communication (IPC)
 *
 */

#ifndef OFIPC_H
#define OFIPC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofdefine.h"     /* for DCMTK_OFSTD_EXPORT */
#include "dcmtk/ofstd/oftypes.h"      /* for OFBool */
#include "dcmtk/ofstd/ofcond.h"       /* for OFCondition */
#include "dcmtk/ofstd/ofstring.h"     /* for OFString */

// in order to test or use the System V message queue implementation
// on platforms such as Linux that support both Posix and System V,
// uncomment the following line:
// #undef HAVE_MQUEUE_H

// Note: On FreeBSD, Posix message queues are available since FreeBSD 7,
// but must be explicitly enabled by loading the "mqueuefs" kernel module
// (i.e. by calling "kldload mqueuefs" as root user).
// System V message queues, however, are always available. Therefore,
// we prefer these when compiling on FreeBSD.

#if defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
BEGIN_EXTERN_C
#include <mqueue.h>                   /* for mqd_t */
END_EXTERN_C
#endif

/** a server class for one-directional IPC messaging. It enables the user to create
 *  an IPC message queue and to receive incoming messages.
 *  @note Note that there are platform specific limits to the message queue
 *  implementation depending on the underlying operating system API used.
 *  For example, Posix message queues on Linux only permit a maximum of 10 messages
 *  in the queue, and a maximum of 255 open queues system-wide.
 *  System V message queues on Linux only permit a maximum of 8 kBytes
 *  of data in a message queue. Furthermore, there are also limits on the
 *  maximum message size. For portable usage, messages should be text-based
 *  and short (e.g. a DICOM UID), and message queues should be kept short.
 */
class DCMTK_OFSTD_EXPORT OFIPCMessageQueueServer
{

public:
  /// default constructor
  OFIPCMessageQueueServer();

  /** destructor, will implicitly call deleteQueue() if not
   *  already called by the user.
   */
  virtual ~OFIPCMessageQueueServer();

  /** create a new message queue
   *  @param a name for the queue that is known both to the client and the server.
   *    Typically the name of the tool using the queue. Must not contain characters
   *    that are not permitted in a filename. Should not be longer than 128 bytes.
   *  @param a number that is known both to the client and the server.
   *    Usually this will be the port number under which the server process
   *    accepts incoming TCP/IP network connection. When used in other contexts,
   *    a number larger than 65535 should be used.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition createQueue(const char *name, Uint32 port);

  /** check if this object manages an open message queue
   *  @return true if message queue is open, false otherwise
   */
  OFBool hasQueue() const;

  /** delete the message queue
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition deleteQueue();

  /** resets this object to default constructed state without closing
   *  the message queue. This method may be called by forked child
   *  processes to make sure that the destructor executed in the child
   *  process will not close the message queue for the parent process as well.
   */
  void detachQueue();

  /** check if an incoming message is waiting in the queue.
   *  @return true if a message is waiting, false otherwise
   */
  OFBool messageWaiting() const;

  /** return the number of incoming messages waiting in the queue
   *  @return number of incoming messages waiting in the queue
   */
  size_t numMessagesWaiting() const;

  /** receive a message from the message queue and remove it from
   *  the queue. If no message is available, the method will
   *  immediately return EC_IPCMessageQueueEmpty (no blocking).
   *  @param msg message received into this parameter if successful
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition receiveMessage(OFString& msg);

private:

#ifdef _WIN32
  OFuintptr_t queue_; // Windows mailslot handle
#elif defined(__ANDROID__)

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  mqd_t queue_;       // Posix message queue ID
  OFString name_;     // name of the message queue
#elif defined(HAVE_SYS_MSG_H)
  int queue_;         // System V message queue ID
#else
#error Platform has neither WIN32, Posix nor System V IPC message queues.
#endif

};

/** a client class for one-directional IPC messaging. It enables the user to access
 *  an existing IPC message queue and to send messages to that queue.
 *  @note Note that there are platform specific limits to the message queue
 *  implementation depending on the underlying operating system API used.
 *  For example, Posix message queues on Linux only permit a maximum of 10 messages
 *  in the queue, and a maximum of 255 open queues system-wide.
 *  System V message queues on Linux only permit a maximum of 8 kBytes
 *  of data in a message queue. Furthermore, there are also limits on the
 *  maximum message size. For portable usage, messages should be text-based
 *  and short (e.g. a DICOM UID), and message queues should be kept short.
 */
class DCMTK_OFSTD_EXPORT OFIPCMessageQueueClient
{
public:
  /// default constructor
  OFIPCMessageQueueClient();

  /** destructor, will implicitly call closeQueue() if not
   *  already called by the user.
   */
  virtual ~OFIPCMessageQueueClient();

  /** open (access) an existing message queue
   *  @param a name for the queue that is known both to the client and the server.
   *    Typically the name of the tool using the queue. Must not contain characters
   *    that are not permitted in a filename.  Should not be longer than 128 bytes.
   *  @param a number that is known both to the client and the server.
   *    Usually this will be the port number under which the server process
   *    accepts incoming TCP/IP network connection. When used in other contexts,
   *    a number larger than 65535 should be used.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition openQueue(const char *name, Uint32 port);

  /** check if this object manages an open message queue
   *  @return true if message queue is open, false otherwise
   */
  OFBool hasQueue() const;

  /** close the message queue
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition closeQueue();

  /** resets this object to default constructed state without closing
   *  the message queue. This method may be called by forked child
   *  processes to make sure that the destructor executed in the child
   *  process will not close the message queue for the parent process as well.
   */
  void detachQueue();

  /** send a message by placing it into the message queue.
   *  @param msg the message to be sent. Note that there are platform specific
   *    limitations to the maximum message size, e.g. 8 kByte per message
   *    for Posix message queues on Linux and 4 kByte per message for System V
   *    message queues on Linux. In general, messages should be short.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition sendMessage(const OFString& msg);

private:

#ifdef _WIN32
  OFuintptr_t queue_; // Windows mailslot handle
#elif defined(__ANDROID__)

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  mqd_t queue_;       // Posix message queue ID
#elif defined(HAVE_SYS_MSG_H)
  int queue_;         // System V message queue ID
#endif

};

#endif
