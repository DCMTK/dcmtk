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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofipc.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofthread.h"

#include <climits>
#include <csignal>

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

BEGIN_EXTERN_C
#if defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
#include <mqueue.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_MSG_H
#include <sys/msg.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef DCMTK_HAVE_POLL
#include <poll.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#ifndef DCOMPAT_SYS_SOCKET_H_
#define DCOMPAT_SYS_SOCKET_H_
/* some systems don't protect sys/socket.h (e.g. DEC Ultrix) */
#include <sys/socket.h>
#endif
#endif

#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
END_EXTERN_C


/* --------- Handler code for the Unix Domain Socket implementation -------- */


#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE

/* The handler thread will wait for incoming connections for a certain
 * period of time, and then check whether a shutdown of the handler thread
 * has been requested by the main thread. This constant defines the timeout
 * while waiting for incoming connections, in microseconds. 50000 means
 * that the handler tasks checks for shutdown requests 20 times per second.
 */
#define OFIPC_POLL_INTERVAL_USEC 50000

/* Handler thread for the Unix domain socket implementation. This thread
 * will alternatingly check for incoming connections (with a timeout of 50 msec)
 * and a shutdown request from the main thread, and handle incoming
 * messages until requested to terminate.
 */
class OFUnixDomainSocketQueueHandler: public OFThread
{
public:

  OFUnixDomainSocketQueueHandler(OFMutex& mtx, OFList< OFString > & q, int& queue_socket, OFBool& shutdownRequested)
  : queue_(q)
  , mutex_(mtx)
  , shutdownRequested_(shutdownRequested)
  , queue_socket_(queue_socket)
  {
  }


  virtual void run()
  {
#ifdef DCMTK_HAVE_POLL
    struct pollfd pfd[] =
    {
        { queue_socket_, POLLIN, 0 }
    };
#else
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = OFIPC_POLL_INTERVAL_USEC;

    fd_set fdset;
    FD_ZERO(&fdset);
#ifdef __MINGW32__
    // on MinGW, FD_SET expects an unsigned first argument
    FD_SET((unsigned int) queue_socket_, &fdset);
#else
    FD_SET(queue_socket_, &fdset);
#endif /* __MINGW32__ */
#endif /* DCMTK_HAVE_POLL */

    // main thread worker loop
    OFBool finished = OFFalse;
    int nfound;
    int msgsock;
    size_t msglen;

    while (! finished)
    {
#ifdef DCMTK_HAVE_POLL
      nfound = poll(pfd, 1, OFIPC_POLL_INTERVAL_USEC/1000);
#else
#ifdef HAVE_INTP_SELECT
      nfound = select(queue_socket_+1, (int *)(&fdset), NULL, NULL, &t);
#else
      nfound = select(queue_socket_+1, &fdset, NULL, NULL, &t);
#endif /* HAVE_INTP_SELECT */
#endif /* DCMTK_HAVE_POLL */

      if (nfound > 0)
      {
        // a client is trying to connect, let's handle it
        msgsock = accept(queue_socket_, NULL, NULL);
        if (msgsock > 0)
        {
          if ((read(msgsock, &msglen, sizeof(msglen)) == sizeof(msglen)) && (msglen > 0))
          {
            // message has non-zero length, let's receive it
            char *buf = new char[msglen];
            if (OFstatic_cast(ssize_t, msglen )== read(msgsock, buf, msglen))
            {
              // we have received the message, now let's store it
              mutex_.lock();
              queue_.push_back(OFString(buf, msglen));
              finished = shutdownRequested_;
              mutex_.unlock();
            }
            else
            {
              // message was incomplete. We ignore it, but still check for a shutdown request
              mutex_.lock();
              finished = shutdownRequested_;
              mutex_.unlock();
            }
            delete[] buf;
          }
          (void) close(msgsock);
        }
      }
      else
      {
        // No client. Check if we have been asked to terminate
        mutex_.lock();
        finished = shutdownRequested_;
        mutex_.unlock();
      }
    }
  }

private:
  OFList< OFString > & queue_; // locally stored list of string
  OFMutex& mutex_; // mutex for accessing queue_ and shutdownRequested_
  OFBool& shutdownRequested_; // true of shutdown of handler thread is requested
  int& queue_socket_; // unix domain socket
};

#endif


/* ---------------- Signal handling code for Posix platforms --------------- */


#ifndef _WIN32

#ifdef WITH_THREADS
static OFMutex OFIPCMessageQueueServerSignalMutex;
#endif

static OFList< OFIPCMessageQueueServer * > OFIPCMessageQueueServerList;

static void addMessageQueueServer(OFIPCMessageQueueServer *s)
{
#ifdef WITH_THREADS
  OFIPCMessageQueueServerSignalMutex.lock();
#endif
  OFIPCMessageQueueServerList.push_back(s);
#ifdef WITH_THREADS
  OFIPCMessageQueueServerSignalMutex.unlock();
#endif
}

static void removeMessageQueueServer(OFIPCMessageQueueServer *s)
{
#ifdef WITH_THREADS
  OFIPCMessageQueueServerSignalMutex.lock();
#endif
  OFIPCMessageQueueServerList.remove(s);
#ifdef WITH_THREADS
  OFIPCMessageQueueServerSignalMutex.unlock();
#endif
}

extern void closeAllMessageQueues()
{
  // this function is only called when the process has received
  // SIGINT or SIGTERM, i.e. is in the process of shutdown.
  // Here, we deliberately ignore the mutex and just close all queues
  // to make sure they don't remain after the end of the application.
  OFListIterator(OFIPCMessageQueueServer *) first = OFIPCMessageQueueServerList.begin();
  OFListIterator(OFIPCMessageQueueServer *) last = OFIPCMessageQueueServerList.end();
  while (first != last)
  {
    (*first)->deleteQueueInternal();
    first = OFIPCMessageQueueServerList.erase(first);
  }
}

#ifdef SIGNAL_HANDLER_WITH_ELLIPSE
extern "C" void OFIPCMessageQueueServerSIGINTHandler(...)
#else
extern "C" void OFIPCMessageQueueServerSIGINTHandler(int)
#endif
{
  // globally close all message queues
  closeAllMessageQueues();

  // re-install the default handler for SIGINT and execute it
  signal(SIGINT, SIG_DFL);
  raise(SIGINT);
}

#ifdef SIGNAL_HANDLER_WITH_ELLIPSE
extern "C" void OFIPCMessageQueueServerSIGTERMHandler(...)
#else
extern "C" void OFIPCMessageQueueServerSIGTERMHandler(int)
#endif
{
  // globally close all message queues
  closeAllMessageQueues();

  // re-install the default handler for SIGTERM and execute it
  signal(SIGTERM, SIG_DFL);
  raise(SIGTERM);
}

#endif /* #ifndef _WIN32 */


/* ---------------------- OFIPCMessageQueueServer ---------------------- */


OFIPCMessageQueueServer::OFIPCMessageQueueServer()
#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
: queue_()
, queue_name_()
, mutex_()
, handler_(NULL)
, shutdownRequested_(OFFalse)
, queue_socket_(-1)
#elif defined(_WIN32)
: queue_(OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE))
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
: queue_((mqd_t) -1)
, name_()
#else
: queue_(-1)
#endif
{
}


OFIPCMessageQueueServer::~OFIPCMessageQueueServer()
{
  // delete the queue if the user has not already done so
  if (hasQueue()) (void) deleteQueue();
}


OFCondition OFIPCMessageQueueServer::createQueue(const char *name, Uint32 port)
{
  if (name == NULL) return EC_IllegalParameter;
  if (hasQueue()) return EC_IPCMessageQueueExists;

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  // construct name of unix domain socket
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  queue_name_ = "/tmp/";
  queue_name_ += name;
  queue_name_ += "_";
  queue_name_ += port_str;

  // create socket
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0)
  {
    (void) unlink(queue_name_.c_str());
    return EC_IPCMessageQueueFailure;
  }

  // fill-in socket address
  struct sockaddr_un server;
  memset(&server, 0, sizeof(server));
  server.sun_family = AF_UNIX;
  OFStandard::strlcpy(server.sun_path, queue_name_.c_str(), sizeof(server.sun_path));

  // bind socket to socket address
  if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)))
  {
    (void) close(sock);
    (void) unlink(queue_name_.c_str());
    return EC_IPCMessageQueueFailure;
  }

  // start listening, with up to 10 clients on the backlog
  if (listen(sock, 10))
  {
    (void) close(sock);
    (void) unlink(queue_name_.c_str());
    return EC_IPCMessageQueueFailure;
  }

  // start socket handler thread
  queue_socket_ = sock;
  handler_ = new OFUnixDomainSocketQueueHandler(mutex_, queue_, queue_socket_, shutdownRequested_);
  handler_->start();

  // make sure that the socket gets closed in case of abnormal
  // termination of the application
  addMessageQueueServer(this);
  return EC_Normal;

#elif defined(_WIN32)
  // construct name of mailslot
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  OFString slotname = "\\\\.\\mailslot\\";
  slotname += name;
  slotname += "\\";
  slotname += port_str;

  // create mailslot
  HANDLE hSlot = CreateMailslot(slotname.c_str(), 0, 0, NULL);
  if (hSlot == INVALID_HANDLE_VALUE)
  {
    // report an error if the mailslot creation failed
    return EC_IPCMessageQueueFailure;
  }

  // store the handle
  queue_ = OFreinterpret_cast(OFuintptr_t, hSlot);
  return EC_Normal;

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  // construct name of queue
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  name_ = "/";
  name_ += name;
  name_ += "_";
  name_ += port_str;

  // check for the maximum permitted length of the queue name
  if (name_.length() > NAME_MAX) return EC_IllegalParameter;

  // try to delete any old message queue associated with this name that may still exist,
  // for example because an application terminated without deleting it.
  (void) mq_unlink(name_.c_str());

  // create a new queue in non-blocking mode, fail if queue already exists.
  // We use the system defaults for max message size and max queue length,
  // which should be set to the maximum values permitted.
  queue_ = mq_open(name_.c_str(), O_RDONLY|O_CREAT|O_EXCL|O_NONBLOCK, 0666, NULL);

  // return an error if creating the queue failed
  if (queue_ == (mqd_t) -1) return EC_IPCMessageQueueFailure;

  addMessageQueueServer(this);
  return EC_Normal;

#else
  // construct name of queue
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  OFString namestr = "/tmp/";
  namestr += name;
  namestr += "_";
  namestr += port_str;

  // create empty temporary file based on queue name and port number
  FILE *tmpfile = fopen(namestr.c_str(),"wb");
  if (tmpfile == NULL) return EC_IPCMessageQueueFailure;
  (void) fclose (tmpfile);

  // use ftok() to generate a key_t identifier for the queue
  key_t key = ftok(namestr.c_str(), 'D' /* for DCMTK */ );
  if (key == OFstatic_cast(key_t, -1)) return EC_IPCMessageQueueFailure;

  // try to delete any old message queue associated with this key that may still exist,
  // for example because an application terminated without deleting it.
  int old_queue = msgget(key, 0);
  (void) msgctl(old_queue, IPC_RMID, NULL);

  // create a new System V IPC message queue, fail if queue already exists.
  queue_ = msgget(key, IPC_CREAT | IPC_EXCL | 0666 );
  if (queue_ < 0) return EC_IPCMessageQueueFailure;

  addMessageQueueServer(this);
  return EC_Normal;

#endif
}


OFBool OFIPCMessageQueueServer::hasQueue() const
{
#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  return (handler_ != NULL);
#elif defined(_WIN32)
  return (queue_ != OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE));
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  return (queue_ != (mqd_t) -1);
#else
  return (queue_ != -1);
#endif
}


OFCondition OFIPCMessageQueueServer::deleteQueueInternal()
{
  if (!hasQueue()) return EC_IPCMessageNoQueue;

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  // this code will only be called when we have received
  // SIGINT or SIGTERM. We don't interact with the handler
  // thread, but just close the socket.
  (void) close(queue_socket_);
  (void) unlink(queue_name_.c_str());
  return EC_Normal;

#elif defined(_WIN32)
  OFCondition result = EC_Normal;
  if (0 == CloseHandle(OFreinterpret_cast(HANDLE, queue_)))
  {
    // closing the handle failed.
    result = EC_IPCMessageQueueFailure;
  }

  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
  return result;

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  // close the message queue
  int result = mq_close(queue_);
  if (result) return EC_IPCMessageQueueFailure;

  // delete the message queue by name
  result =  mq_unlink(name_.c_str());

  // reset queue descriptor
  queue_ = (mqd_t) -1;

  if (result) return EC_IPCMessageQueueFailure;
  return EC_Normal;

#else
  // delete the message queue by queue ID
  int result = msgctl(queue_, IPC_RMID, NULL);

  // reset queue descriptor
  queue_ = -1;

  if (result) return EC_IPCMessageQueueFailure;
  return EC_Normal;

#endif
}


OFCondition OFIPCMessageQueueServer::deleteQueue()
{
#ifndef _WIN32
  removeMessageQueueServer(this);
#endif

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  // request the handler thread to shutdown
  mutex_.lock();
  shutdownRequested_ = OFTrue;
  mutex_.unlock();

  // wait for handler thread to terminate
  int result1 = handler_->join();

  // delete thread object
  delete handler_;

  // close socket
  int result2 = close(queue_socket_);

  // delete socket from filesystem
  int result3 = unlink(queue_name_.c_str());

  // reset object status to default constructed state
  queue_.clear();
  queue_name_.clear();
  handler_ = NULL;
  shutdownRequested_ = OFFalse;
  queue_socket_ = -1;

  // return error message if something went wrong with terminating the thread
  if (result1 || result2 || result3) return EC_IPCMessageQueueFailure;
    else return EC_Normal;

#elif defined(_WIN32)
  OFCondition result = EC_Normal;
  if (0 == CloseHandle(OFreinterpret_cast(HANDLE, queue_)))
  {
    // closing the handle failed.
    result = EC_IPCMessageQueueFailure;
  }

  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
  return result;
#else
  return deleteQueueInternal();
#endif
}


void OFIPCMessageQueueServer::detachQueue()
{
#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  // request the handler thread to shutdown
  mutex_.lock();
  shutdownRequested_ = OFTrue;
  mutex_.unlock();

  // wait for handler thread to terminate
  (void) handler_->join();

  // delete thread object
  delete handler_;

  // reset object status to default constructed state
  // without closing the queue socket
  queue_.clear();
  handler_ = NULL;
  shutdownRequested_ = OFFalse;
  queue_socket_ = -1;

#elif defined(_WIN32)
  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  queue_ = (mqd_t) -1;
  name_.clear();
#else
  queue_ = -1;
#endif
}


OFBool OFIPCMessageQueueServer::messageWaiting()
{
  return (numMessagesWaiting() > 0);
}


size_t OFIPCMessageQueueServer::numMessagesWaiting()
{
  if (! hasQueue()) return 0;

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  mutex_.lock();
  size_t result = queue_.size();
  mutex_.unlock();
  return result;

#elif defined(_WIN32)
  DWORD cMessage = 0;
  if (GetMailslotInfo( OFreinterpret_cast(HANDLE, queue_), NULL, NULL, &cMessage, NULL))
  {
    return OFstatic_cast(size_t, cMessage);
  }
  return 0;

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  // retrieve message queue status
  struct mq_attr attr;
  memset(&attr, 0, sizeof(attr));
  if (mq_getattr(queue_, &attr)) return 0;
  return OFstatic_cast(size_t, attr.mq_curmsgs);

#else
  // retrieve message queue status
  struct msqid_ds attr;
  memset(&attr, 0, sizeof(attr));
  if (msgctl(queue_, IPC_STAT, &attr)) return 0;
  return OFstatic_cast(size_t, attr.msg_qnum);

#endif
}


OFCondition OFIPCMessageQueueServer::receiveMessage(OFString& msg)
{
  if (!hasQueue()) return EC_IPCMessageNoQueue;

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  OFCondition result = EC_Normal;
  mutex_.lock();
  if (queue_.size() > 0)
  {
    msg = queue_.front();
    queue_.pop_front();
  }
  else result = EC_IPCMessageQueueEmpty;
  mutex_.unlock();
  return result;

#elif defined(_WIN32)
  HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (NULL == hEvent) return EC_IPCMessageQueueFailure;

  DWORD cbMessage = 0;
  DWORD cMessage = 0;
  OVERLAPPED ov;
  ov.Offset = 0;
  ov.OffsetHigh = 0;
  ov.hEvent = hEvent;
  char *buf = NULL;
  OFCondition result = EC_Normal;

  // retrieve mailslot status
  if (GetMailslotInfo( OFreinterpret_cast(HANDLE, queue_), NULL, &cbMessage, &cMessage, NULL))
  {
    // check if there is at least one non-empty message in the mailslot
    if (cbMessage > 0 && cMessage > 0)
    {
      buf = new char[cbMessage];
      buf[0] = '\0';
      // read message from mailslot
      if (ReadFile(OFreinterpret_cast(HANDLE, queue_), buf, cbMessage, NULL, &ov))
      {
        // copy message to msg
        msg.assign(buf, cbMessage);
      }
      else result = EC_IPCMessageQueueEmpty;
    }
    else
    {
      result = EC_IPCMessageQueueEmpty;
    }
  } else result = EC_IPCMessageQueueFailure;

  //clean up and return
  (void) CloseHandle(hEvent);
  delete[] buf;
  return result;

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  // retrieve message queue status
  struct mq_attr attr;
  memset(&attr, 0, sizeof(attr));
  if (mq_getattr(queue_, &attr)) return EC_IPCMessageQueueFailure;

  // check if there is at least one message in the queue
  if (attr.mq_curmsgs == 0) return EC_IPCMessageQueueEmpty;

  // allocate buffer for the maximum possible message size
  char *buf = new char[attr.mq_msgsize];

  // receive the message
  ssize_t len = mq_receive(queue_, buf, attr.mq_msgsize, NULL);

  // mq_receive() returns -1 when receiving the message fails
  if (len < 0)
  {
    delete[] buf;
    return EC_IPCMessageQueueFailure;
  }

  // copy message to msg
  msg.assign(buf, len);

  //clean up and return
  delete[] buf;
  return EC_Normal;

#else
  // define message buffer struct
  struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
  };

  // retrieve message queue status
  struct msqid_ds attr;
  memset(&attr, 0, sizeof(attr));
  if (msgctl(queue_, IPC_STAT, &attr)) return EC_IPCMessageQueueFailure;

  // check if there is at least one message in the queue
  if (attr.msg_qnum == 0) return EC_IPCMessageQueueEmpty;

  // allocate buffer for the maximum possible message size
  char *buf = new char[attr.msg_qbytes];

  // receive message
  ssize_t len = msgrcv(queue_, buf, attr.msg_qbytes, 0, IPC_NOWAIT);

  // msgrcv() returns -1 when receiving the message fails
  if (len < 0)
  {
    delete[] buf;
    return EC_IPCMessageQueueFailure;
  }

  // copy message to msg
  msg.assign(OFreinterpret_cast(msgbuf *, buf)->mtext, len);

  //clean up and return
  delete[] buf;
  return EC_Normal;

#endif
}


void OFIPCMessageQueueServer::registerSignalHandler()
{
#ifndef _WIN32
  signal(SIGINT, OFIPCMessageQueueServerSIGINTHandler);
  signal(SIGTERM, OFIPCMessageQueueServerSIGTERMHandler);
#endif
}


/* ---------------------- OFIPCMessageQueueClient ---------------------- */


OFIPCMessageQueueClient::OFIPCMessageQueueClient()
#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
: queue_name_()
#elif defined(_WIN32)
: queue_(OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE))
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
: queue_((mqd_t) -1)
#else
: queue_(-1)
#endif
{
}


OFIPCMessageQueueClient::~OFIPCMessageQueueClient()
{
  // close the queue if the user has not already done so
#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  if (queue_name_.length() > 0) (void) closeQueue();
#elif defined(_WIN32)
  if (queue_ != OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE)) (void) closeQueue();
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  if (queue_ != (mqd_t) -1) (void) closeQueue();
#else
  if (queue_ != -1) (void) closeQueue();
#endif
}


OFCondition OFIPCMessageQueueClient::openQueue(const char *name, Uint32 port)
{
  if (name == NULL) return EC_IllegalParameter;
  if (hasQueue()) return EC_IPCMessageQueueExists;

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  // construct name of unix domain socket
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  OFString socketname = "/tmp/";
  socketname += name;
  socketname += "_";
  socketname += port_str;

  // create socket
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) return EC_IPCMessageQueueFailure;

  // fill-in socket address
  struct sockaddr_un server;
  memset(&server, 0, sizeof(server));
  server.sun_family = AF_UNIX;
  OFStandard::strlcpy(server.sun_path, socketname.c_str(), sizeof(server.sun_path));

  // connect to server
  if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0)
  {
    close(sock);
    return EC_IPCMessageQueueFailure;
  }

  // write zero-length message
  size_t msglen = 0;
  if (write(sock, &msglen, sizeof(msglen)) < 0)
  {
    close(sock);
    return EC_IPCMessageQueueFailure;
  }

  // disconnect from server
  close(sock);

  // everything worked as expected, store queue name
  queue_name_ = socketname;
  return EC_Normal;

#elif defined(_WIN32)
  // construct name of mailslot
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  OFString slotname = "\\\\.\\mailslot\\";
  slotname += name;
  slotname += "\\";
  slotname += port_str;

  // open mailslot
  HANDLE hFile = CreateFile(slotname.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE)
  {
    // report an error if opening of the mailslot creation
    return EC_IPCMessageQueueFailure;
  }

  // store the handle
  queue_ = OFreinterpret_cast(OFuintptr_t, hFile);
  return EC_Normal;

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  // construct name of queue
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  OFString queuename = "/";
  queuename += name;
  queuename += "_";
  queuename += port_str;
  // check for the maximum permitted length of the queue name
  if (queuename.length() > NAME_MAX) return EC_IllegalParameter;

  // open the queue in non-blocking mode, fail if it does not yet exist
  queue_ = mq_open(queuename.c_str(), O_WRONLY|O_NONBLOCK);

  // return an error if opening the queue failed
  if (queue_ == (mqd_t) -1) return EC_IPCMessageQueueFailure;
  return EC_Normal;

#else
  // construct name of queue
  char port_str[12];
  OFStandard::snprintf(port_str, sizeof(port_str), "%lu", OFstatic_cast(unsigned long, port));
  OFString namestr = "/tmp/";
  namestr += name;
  namestr += "_";
  namestr += port_str;

  // use ftok() to generate a key_t identifier for the queue based on the
  // temporary file that the message queue servers should have created
  key_t key = ftok(namestr.c_str(), 'D' /* for DCMTK */ );
  if (key == OFstatic_cast(key_t, -1)) return EC_IPCMessageQueueFailure;

  // open existing System V IPC message queue, fail if queue does not exist
  queue_ = msgget(key, 0);
  if (queue_ < 0) return EC_IPCMessageQueueFailure;
  return EC_Normal;

#endif
}


OFBool OFIPCMessageQueueClient::hasQueue() const
{
#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  return queue_name_.length() > 0;
#elif defined(_WIN32)
  return (queue_ != OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE));
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  return (queue_ != (mqd_t) -1);
#else
  return (queue_ != OFstatic_cast(key_t, -1));
#endif
}


OFCondition OFIPCMessageQueueClient::closeQueue()
{
  if (!hasQueue()) return EC_IPCMessageNoQueue;

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  queue_name_.clear();
  return EC_Normal;

#elif defined(_WIN32)
  OFCondition result = EC_Normal;
  if (0 == CloseHandle(OFreinterpret_cast(HANDLE, queue_)))
  {
    // closing the handle failed.
    result = EC_IPCMessageQueueFailure;
  }

  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
  return result;
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  // close the message queue
  int result = mq_close(queue_);

  // reset queue descriptor
  queue_ = (mqd_t) -1;

  if (result) return EC_IPCMessageQueueFailure;
  return EC_Normal;

#else
  // System V message queues do not have to be closed
  queue_ = -1;
  return EC_Normal;

#endif
}


void OFIPCMessageQueueClient::detachQueue()
{
#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  queue_name_.clear();
#elif defined(_WIN32)
  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  queue_ = (mqd_t) -1;
#else
  queue_ = -1;
#endif
}


OFCondition OFIPCMessageQueueClient::sendMessage(const OFString& msg)
{
  if (!hasQueue()) return EC_IPCMessageNoQueue;
  if (msg.length() == 0) return EC_IPCEmptyMessage;

#ifdef DCMTK_USE_UNIX_SOCKET_QUEUE
  // create socket
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) return EC_IPCMessageQueueFailure;

  // fill-in socket address
  struct sockaddr_un server;
  memset(&server, 0, sizeof(server));
  server.sun_family = AF_UNIX;
  OFStandard::strlcpy(server.sun_path, queue_name_.c_str(), sizeof(server.sun_path));

  // connect to server
  if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0)
  {
    close(sock);
    return EC_IPCMessageQueueFailure;
  }

  // write message length
  size_t msglen = msg.length();
  if (write(sock, &msglen, sizeof(msglen)) < 0)
  {
    close(sock);
    return EC_IPCMessageQueueFailure;
  }

  // write message content
  if (write(sock, msg.c_str(), msglen) < 0)
  {
    close(sock);
    return EC_IPCMessageQueueFailure;
  }

  // disconnect from server
  close(sock);
  return EC_Normal;

#elif defined(_WIN32)
  DWORD written = 0;
  if (WriteFile(OFreinterpret_cast(HANDLE, queue_), msg.c_str(), OFstatic_cast(DWORD, msg.length()), &written, NULL))
    return EC_Normal;
    else return EC_IPCMessageQueueFailure;

#elif defined(HAVE_MQUEUE_H) && !defined(__FreeBSD__)
  // send the message and report an error if this fails
  if (mq_send(queue_, msg.c_str(), msg.length(), 0)) return EC_IPCMessageQueueFailure;
  return EC_Normal;

#else
  // define message buffer struct
  struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
  };

  // create message buffer large enough for this message
  char *buf = new char[sizeof(msgbuf) + msg.length()];
  OFreinterpret_cast(msgbuf *, buf)->mtype = 1;

  // copy string to message buffer
  memcpy(OFreinterpret_cast(msgbuf *, buf)->mtext, msg.c_str(), msg.length());

  // send the message
  int result = msgsnd(queue_, buf, msg.length(), IPC_NOWAIT);
  delete[] buf;

  if (result) return EC_IPCMessageQueueFailure;
  return EC_Normal;
#endif
}
