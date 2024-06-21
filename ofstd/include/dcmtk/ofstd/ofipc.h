/*
 *
 *  Copyright (C) 2022-2023, OFFIS e.V.
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
#include "dcmtk/ofstd/ofthread.h"     /* for OFMutex */
#include "dcmtk/ofstd/oflist.h"       /* for OFList */

// in order to test or use the System V message queue implementation
// on platforms such as Linux that support both Posix and System V,
// uncomment the following line:
// #undef HAVE_MQUEUE_H

// in order to test or use the message queue implementation based
// on unix domain sockets and a separate queue handler thread,
// uncomment the following line:
// #define DCMTK_USE_UNIX_SOCKET_QUEUE

// Android has <mqueue.h> and <sys/msg.h> but no implementation
// of Posix or System V message queues
#ifdef __ANDROID__
#define DCMTK_USE_UNIX_SOCKET_QUEUE
#endif

// GNU Hurd has <mqueue.h> but no working implementation
// of Posix message queues
#ifdef __gnu_hurd__
#define DCMTK_USE_UNIX_SOCKET_QUEUE
#endif

// There may be other platforms where we have threads and
// Unix domain sockets but neither Posix nor System V
// message queues. Use the Unix domain socket based implementation
// in this case.
#if defined(WITH_THREADS) && defined(HAVE_SYS_UN_H)  && !defined(_WIN32) && !defined(HAVE_MQUEUE_H) && !defined(HAVE_SYS_MSG_H)
#define DCMTK_USE_UNIX_SOCKET_QUEUE
#endif

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

/** A function that can be executed in a signal handler for an abnormal
 *  termination of the application through SIGINT (CTRL-C) or SIGTERM
 *  (kill) in order to globally delete all message queues created
 *  by this process through calls to OFIPCMessageQueueServer::createQueue(),
 *  if the queues would otherwise persist past the termination of the
 *  process, as Posix and System V queues would. Not to be called by
 *  normal user code, not thread safe!
 */
extern void closeAllMessageQueues();

/** a server class for one-directional IPC messaging. It enables the user to create
 *  an IPC message queue and to receive incoming messages. Depending on the
 *  platform, the implementation is based on Windows mailslots, Posix message
 *  queues, System V message queues or, on Android, on Unix domain sockets
 *  and a separate handler thread that handles incoming connections. The latter
 *  implementation is preferred over the alternatives if the macro
 *  DCMTK_USE_UNIX_SOCKET_QUEUE is defined.
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
  OFBool messageWaiting();

  /** return the number of incoming messages waiting in the queue
   *  @return number of incoming messages waiting in the queue
   */
  size_t numMessagesWaiting();

  /** receive a message from the message queue and remove it from
   *  the queue. If no message is available, the method will
   *  immediately return EC_IPCMessageQueueEmpty (no blocking).
   *  @param msg message received into this parameter if successful
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition receiveMessage(OFString& msg);

  /** register signal handlers for SIGINT and SIGTERM that
   *  call closeAllMessageQueues() upon receiving the signal,
   *  thus globally closing all message queues created by the
   *  process, if these message queues would persist after the
   *  end of the process otherwise. Does nothing on Windows.
   */
  static void registerSignalHandler();

private:

  /// friend function that will call deleteQueueInternal().
  friend void closeAllMessageQueues();

  /** delete the message queue without touching the internally
   *  managed global list of message queues.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition deleteQueueInternal();

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
#ifndef WITH_THREADS
#error Cannot use Unix Domain Sockets based message queues without threads
#endif
  OFList< OFString > queue_; // locally stored list of string
  OFString queue_name_; // name of the unix domain socket
  OFMutex mutex_; // mutex for accessing queue_ and shutdownRequested_
  OFThread *handler_; // thread that handles the unix domain socket
  OFBool shutdownRequested_; // true of shutdown of handler thread is requested
  int queue_socket_; // unix domain socket
#elif defined(_WIN32)
  OFuintptr_t queue_; // Windows mailslot handle
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  mqd_t queue_;       // Posix message queue ID
  OFString name_;     // name of the message queue
#elif defined(HAVE_SYS_MSG_H)
  int queue_;         // System V message queue ID
#else
#error Platform has neither WIN32, Posix nor System V IPC message queues. Try #define DCMTK_USE_UNIX_SOCKET_QUEUE.
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

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  OFString queue_name_;
#elif defined(_WIN32)
  OFuintptr_t queue_; // Windows mailslot handle
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  mqd_t queue_;       // Posix message queue ID
#elif defined(HAVE_SYS_MSG_H)
  int queue_;         // System V message queue ID
#endif

};

#endif
