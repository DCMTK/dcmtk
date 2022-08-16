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
 *  Purpose: OFIPCMessageQueue unit test
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofipc.h"
#include "dcmtk/ofstd/ofstd.h"

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

// timout for IPC client component, in milliseconds
const int IPC_CLIENT_TIMEOUT = 30000;

// timout for IPC server component, in milliseconds
const int IPC_SERVER_TIMEOUT = 30000;

const char *appName = NULL;

void ipc_client(Uint32 port)
{
  // Note: since the ipc_client() function is run in a separate process,
  // we cannot use the OFCHECK macros and report errors on stdout instead.

  OFIPCMessageQueueClient client;
  OFCondition cond;
  unsigned int elapsed = 0;

  // try to open the message queue until success or timeout
  do
  {
    cond = client.openQueue("ofstd_tipc", port);

    // if this fails, wait 100 msec
    if (cond.bad())
    {
      OFStandard::milliSleep(100);
      elapsed += 100;
    }
  } while (cond.bad() && elapsed < IPC_CLIENT_TIMEOUT);

  // in case of timeout, bail out
  if (cond.bad())
  {
    COUT << "ofstd_tipc: openQueue failed: " << cond.text() << OFendl;
    return;
  }

  OFString msg = "1.2.276.0.7230010.3.1.2.1787205428.2023412.1655632532.864827";
  cond = client.sendMessage(msg);
  if (cond.bad())
  {
    COUT << "ofstd_tipc: sendMessage #1 failed: " << cond.text() << OFendl;
    return;
  }

  msg = "1.2.276.0.7230010.3.1.2.1787205428.2023422.1655632583.732834";
  cond = client.sendMessage(msg);
  if (cond.bad())
  {
    COUT << "ofstd_tipc: sendMessage #2 failed: " << cond.text() << OFendl;
    return;
  }

  msg = "1.2.276.0.7230010.3.1.4.1787205428.2023426.1655632595.148700";
  cond = client.sendMessage(msg);
  if (cond.bad())
  {
    COUT << "ofstd_tipc: sendMessage #3 failed: " << cond.text() << OFendl;
    return;
  }

  cond = client.closeQueue();
  if (cond.bad())
  {
    COUT << "ofstd_tipc: closeQueue failed: " << cond.text() << OFendl;
    return;
  }

  return;
}


void ipc_server(Uint32 port)
{
  OFIPCMessageQueueServer server;
  OFCondition cond;

  cond = server.createQueue("ofstd_tipc", port);
  if (cond.bad())
  {
    OFCHECK_FAIL("OFIPCMessageQueueServer::createQueue failed: " << cond.text());
    return;
  }

  int numMessages = 0;
  unsigned int elapsed = 0;

  // try to read messages from the queue until success or timeout
  do
  {
    while (! server.messageWaiting() && elapsed < IPC_CLIENT_TIMEOUT)
    {
      OFStandard::milliSleep(100);
      elapsed += 100;
    }

    if (! server.messageWaiting())
    {
      OFCHECK_FAIL("ofstd_ipc: ipc_server experienced timeout after receiving " << numMessages << " of 3 messages");
      return;
    }

    OFString msg;
    cond = server.receiveMessage(msg);
    if (cond.bad())
    {
      OFCHECK_FAIL("OFIPCMessageQueueServer::receiveMessage failed: " << cond.text());
      return;
    } else ++numMessages;

    switch (numMessages)
    {
      case 1:
        OFCHECK(msg == "1.2.276.0.7230010.3.1.2.1787205428.2023412.1655632532.864827");
        break;
      case 2:
        OFCHECK(msg == "1.2.276.0.7230010.3.1.2.1787205428.2023422.1655632583.732834");
        break;
      case 3:
        OFCHECK(msg == "1.2.276.0.7230010.3.1.4.1787205428.2023426.1655632595.148700");
        break;
      default:
        OFCHECK_FAIL("ofstd_ipc: ipc_server received too many messages");
        return;
        break;
    }

  } while (numMessages < 3 && elapsed < IPC_SERVER_TIMEOUT);

  cond = server.deleteQueue();
  if (cond.bad())
  {
    OFCHECK_FAIL("OFIPCMessageQueueServer::deleteQueue failed: " << cond.text());
  }
}


OFTEST(ofstd_ipc)
{
#ifdef _WIN32
  // prepare the command line
  OFString cmdLine = appName;
  long pid = OFStandard::getProcessID() + 65536;
  cmdLine += " --ipc ";
  char port_str[20];
  OFStandard::snprintf(port_str, sizeof(port_str), "%ld", pid);
  cmdLine += port_str;

  // we need a STARTUPINFO and a PROCESS_INFORMATION structure for CreateProcess.
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  memset(&pi,0,sizeof(pi));
  memset(&si,0,sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags |= STARTF_USESTDHANDLES;
  si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

  // create child process.
  if (!CreateProcessA(NULL,OFconst_cast(char *, cmdLine.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
  {
    OFCHECK_FAIL("ofstd_ipc: CreateProcessA() failed");
  }
  else
  {
    ipc_server(pid);
  }

#elif defined (HAVE_FORK)
  pid_t pid = fork();
  if (pid < 0)
  {
    OFCHECK_FAIL("ofstd_ipc: fork() failed");
  }
  else if (pid > 0)
  {
    // parent process
    ipc_server(pid + 65536);
  }
  else
  {
    // child process, here we use the parent process ID
    ipc_client(getpid() + 65536);
    exit(0);
  }
#else
  OFCHECK_FAIL("Cannot run ofstd_ipc test case: platform does not have fork()");
#endif
}
