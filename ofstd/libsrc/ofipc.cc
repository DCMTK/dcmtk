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

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

BEGIN_EXTERN_C
#ifdef HAVE_MQUEUE_H
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
END_EXTERN_C

/* ---------------------- OFIPCMessageQueueServer ---------------------- */

OFIPCMessageQueueServer::OFIPCMessageQueueServer()
#ifdef _WIN32
: queue_(OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE))
#elif HAVE_MQUEUE_H
: queue_(OFstatic_cast(mqd_t, -1))
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

#ifdef _WIN32
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

#elif HAVE_MQUEUE_H
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
  if (queue_ == OFstatic_cast(mqd_t, -1)) return EC_IPCMessageQueueFailure;
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
  return EC_Normal;

#endif
}

OFBool OFIPCMessageQueueServer::hasQueue() const
{
#ifdef _WIN32
  return (queue_ != OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE));
#elif HAVE_MQUEUE_H
  return (queue_ != OFstatic_cast(mqd_t, -1));
#else
  return (queue_ != -1);
#endif
}

OFCondition OFIPCMessageQueueServer::deleteQueue()
{
  if (!hasQueue()) return EC_IPCMessageNoQueue;

#ifdef _WIN32
  OFCondition result = EC_Normal;
  if (0 == CloseHandle(OFreinterpret_cast(HANDLE, queue_)))
  {
    // closing the handle failed.
    result = EC_IPCMessageQueueFailure;
  }

  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
  return result;

#elif HAVE_MQUEUE_H
  // close the message queue
  int result = mq_close(queue_);
  if (result) return EC_IPCMessageQueueFailure;

  // delete the message queue by name
  result =  mq_unlink(name_.c_str());

  // reset queue descriptor
  queue_ = OFstatic_cast(mqd_t, -1);

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

void OFIPCMessageQueueServer::detachQueue()
{
#ifdef _WIN32
  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
#elif HAVE_MQUEUE_H
  queue_ = OFstatic_cast(mqd_t, -1);
  name_.clear();
#else
  queue_ = -1;
#endif
}

OFBool OFIPCMessageQueueServer::messageWaiting() const
{
  return (numMessagesWaiting() > 0);
}

size_t OFIPCMessageQueueServer::numMessagesWaiting() const
{
  if (! hasQueue()) return 0;

#ifdef _WIN32
  DWORD cMessage = 0; 
  if (GetMailslotInfo( OFreinterpret_cast(HANDLE, queue_), NULL, NULL, &cMessage, NULL))
  {
    return OFstatic_cast(size_t, cMessage);
  }
  return 0;

#elif HAVE_MQUEUE_H
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

#ifdef _WIN32
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

#elif HAVE_MQUEUE_H
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


/* ---------------------- OFIPCMessageQueueClient ---------------------- */


OFIPCMessageQueueClient::OFIPCMessageQueueClient()
#ifdef _WIN32
: queue_(OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE))
#elif HAVE_MQUEUE_H
: queue_(OFstatic_cast(mqd_t, -1))
#else
: queue_(-1)
#endif
{
}


OFIPCMessageQueueClient::~OFIPCMessageQueueClient()
{
  // close the queue if the user has not already done so
#ifdef _WIN32
  if (queue_ != OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE)) (void) closeQueue();
#elif HAVE_MQUEUE_H
  if (queue_ != OFstatic_cast(mqd_t, -1)) (void) closeQueue();
#else
  if (queue_ != -1) (void) closeQueue();
#endif
}


OFCondition OFIPCMessageQueueClient::openQueue(const char *name, Uint32 port)
{
  if (name == NULL) return EC_IllegalParameter;
  if (hasQueue()) return EC_IPCMessageQueueExists;

#ifdef _WIN32
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

#elif HAVE_MQUEUE_H
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
  if (queue_ == OFstatic_cast(mqd_t, -1)) return EC_IPCMessageQueueFailure;
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
#ifdef _WIN32
  return (queue_ != OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE));
#elif HAVE_MQUEUE_H
  return (queue_ != OFstatic_cast(mqd_t, -1));
#else
  return (queue_ != OFstatic_cast(key_t, -1));
#endif
}

OFCondition OFIPCMessageQueueClient::closeQueue()
{
  if (!hasQueue()) return EC_IPCMessageNoQueue;

#ifdef _WIN32
  OFCondition result = EC_Normal;
  if (0 == CloseHandle(OFreinterpret_cast(HANDLE, queue_)))
  {
    // closing the handle failed.
    result = EC_IPCMessageQueueFailure;
  }

  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
  return result;

#elif HAVE_MQUEUE_H
  // close the message queue
  int result = mq_close(queue_);

  // reset queue descriptor
  queue_ = OFstatic_cast(mqd_t, -1);

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
#ifdef _WIN32
  queue_ = OFreinterpret_cast(OFuintptr_t, INVALID_HANDLE_VALUE);
#elif HAVE_MQUEUE_H
  queue_ = OFstatic_cast(mqd_t, -1);
#else
  queue_ = -1;
#endif
}

OFCondition OFIPCMessageQueueClient::sendMessage(const OFString& msg)
{
  if (!hasQueue()) return EC_IPCMessageNoQueue;
  if (msg.length() == 0) return EC_IPCEmptyMessage;

#ifdef _WIN32
  if (WriteFile(OFreinterpret_cast(HANDLE, queue_), msg.c_str(), OFstatic_cast(DWORD, msg.length()), NULL, NULL))
    return EC_Normal;
    else return EC_IPCMessageQueueFailure;

#elif HAVE_MQUEUE_H
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
  perror("sendMessage");

  if (result) return EC_IPCMessageQueueFailure;
  return EC_Normal;
#endif
}

