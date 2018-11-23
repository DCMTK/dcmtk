/*
 *
 *  Copyright (C) 2000-2018, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Sample message server for class DVPSIPCClient
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(HAVE_WINSOCK_H)
#include <winsock.h>  /* include winsock.h directly i.e. on MacOS */
#endif

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* we have to include this before sys/time.h on Solaris */
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>    /* for struct timeval on Linux */
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
END_EXTERN_C

#define INCLUDE_CSTDLIB
#define INCLUDE_CERRNO
#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmpstat/dvpsmsg.h"     /* for class DVPSIPCMessage */
#include "dcmtk/dcmdata/cmdlnarg.h"    /* for prepareCmdLineArgs */
#include "dcmtk/ofstd/ofconapp.h"    /* for class OFConsoleApplication */
#include "dcmtk/dcmnet/dcmtrans.h"    /* for class DcmTCPConnection */
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmnet/dcompat.h"     /* compatability routines */
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmnet/diutil.h"

#define OFFIS_CONSOLE_APPLICATION "msgserv"

static OFLogger msgservLogger = OFLog::getLogger("dcmtk.tests." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


static const char *statusString(Uint32 i)
{
  if (i==DVPSIPCMessage::statusOK) return "OK";
  if (i==DVPSIPCMessage::statusWarning) return "Warning";
  if (i==DVPSIPCMessage::statusError) return "Error";
  return "unknown status code";
}

static const char *applicationType(Uint32 i)
{
  if (i==DVPSIPCMessage::clientOther) return "unspecified";
  if (i==DVPSIPCMessage::clientStoreSCP) return "Receiver (Store SCP)";
  if (i==DVPSIPCMessage::clientStoreSCU) return "Sender (Store SCU)";
  if (i==DVPSIPCMessage::clientPrintSCP) return "Printer (Print SCP)";
  if (i==DVPSIPCMessage::clientPrintSCU) return "Print Client (Print SCU)";
  if (i==DVPSIPCMessage::clientQRSCP) return "Database (Query/Retrieve SCP)";
  return "unknown application type";
}

#define SHORTCOL 3
#define LONGCOL 12

int main(int argc, char *argv[])
{
    OFStandard::initializeNetwork();

#ifdef WITH_TCPWRAPPER
    // this code makes sure that the linker cannot optimize away
    // the DUL part of the network module where the external flags
    // for libwrap are defined. Needed on OpenBSD.
    dcmTCPWrapperDaemonName.set(NULL);
#endif

    OFCmdUnsignedInt opt_port = 0;                   /* listen port */
    Uint32           clientID = 0;                   /* IDs assigned to connecting clients */

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Sample message server for class DVPSIPCClient", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("port", "port number to listen at");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL);
      cmd.addOption("--help", "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      cmd.getParam(1, opt_port);
      OFLog::configureFromCommandLine(cmd, app);
    }

    OFLOG_DEBUG(msgservLogger, rcsid << OFendl);

    unsigned short networkPort = (unsigned short) opt_port;
    if (networkPort==0)
    {
      OFLOG_FATAL(msgservLogger, "no or invalid port number");
      return 10;
    }

#ifndef DISABLE_PORT_PERMISSION_CHECK
#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if ((networkPort < 1024)&&(geteuid() != 0))
    {
      OFLOG_FATAL(msgservLogger, "cannot listen on port " << networkPort << ", insufficient privileges");
      return 10;
    }
#endif
#endif

    /* open listen socket */
#ifdef _WIN32
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
#else
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
#endif
    {
      OFLOG_FATAL(msgservLogger, "failed to create socket");
      return 10;
    }

    int reuse = 1;

#ifdef _WIN32
    if (setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &reuse, sizeof(reuse)) < 0)
#else
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
#endif
    {
      OFLOG_FATAL(msgservLogger, "failed to set socket options");
      return 10;
    }

    /* Name socket using wildcards */
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = (unsigned short) htons(networkPort);
    if (bind(s, (struct sockaddr *) & server, sizeof(server)))
    {
      OFLOG_FATAL(msgservLogger, "failed to bind socket to port, already in use?");
      return 10;
    }
    listen(s, 64);  // accept max 64 pending TCP connections on this socket

    /* drop root privileges now and revert to the calling user id (if we are running as setuid root) */
    if (OFStandard::dropPrivileges().bad())
    {
          OFLOG_FATAL(msgservLogger, "setuid() failed, maximum number of processes/threads for uid already running.");
          return 10;
    }

    fd_set fdset;
    struct timeval t;
    int nfound;
    while (1)
    {
      // wait for next incoming connection
      FD_ZERO(&fdset);
      FD_SET(s, &fdset);
      t.tv_sec = 10;  // 10 seconds timeout
      t.tv_usec = 0;

#ifdef HAVE_INTP_SELECT
      nfound = select(OFstatic_cast(int, s + 1), (int *)(&fdset), NULL, NULL, &t);
#else
      // the typecast is safe because Windows ignores the first select() parameter anyway
      nfound = select(OFstatic_cast(int, s + 1), &fdset, NULL, NULL, &t);
#endif

      if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
      {
          DU_logSelectResult(nfound);
      }

      if (nfound > 0)
      {
        // incoming connection detected
#ifdef _WIN32
        SOCKET sock=0;
#else
        int sock=0;
#endif
        struct sockaddr from;
#ifdef HAVE_DECLARATION_SOCKLEN_T
        socklen_t len = sizeof(from);
#elif !defined(HAVE_PROTOTYPE_ACCEPT) || defined(HAVE_INTP_ACCEPT)
        int len = sizeof(from);
#else
        size_t len = sizeof(from);
#endif
        do
        {
          sock = accept(s, &from, &len);
#ifdef _WIN32
        } while ((sock == INVALID_SOCKET)&&(errno == WSAEINTR));
        if (sock == INVALID_SOCKET)
#else
        } while ((sock == -1)&&(errno == EINTR));
        if (sock < 0)
#endif
        {
          OFLOG_FATAL(msgservLogger, "unable to accept incoming connection");
          return 10;
        }

        reuse = 1;

#ifdef _WIN32
        if (setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &reuse, sizeof(reuse)) < 0)
#else
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
#endif
        {
          OFLOG_FATAL(msgservLogger, "failed to set socket options");
          return 10;
        }

        // now we can handle the incoming connection
        DcmTCPConnection connection(sock);
        DVPSIPCMessage msg;
        Uint32 i=0;
        Uint32 msgType=0;
        OFString str;

        OFBool finished = OFFalse;
        while (!finished)
        {
          while (! connection.networkDataAvailable(1))
          {
            // waiting for network data to become available or connection to be closed
          }
          if (msg.receive(connection))
          {
            OFOStringStream oss;
            // handle message
            msgType = msg.getMessageType();
            if (msgType == DVPSIPCMessage::OK)
            {
                oss << "received 'OK' (should not happen)" << OFendl;
            } else if (msgType == DVPSIPCMessage::requestApplicationID) {
                oss << "New client requests application ID, assigning #" << clientID+1 << OFendl
                     << "Application Type: ";
                if (msg.extractIntFromPayload(i)) oss << applicationType(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::assignApplicationID) {
                oss << "received 'AssignApplicationID' (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::applicationTerminates) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Application Terminates, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
            } else if (msgType == DVPSIPCMessage::receivedUnencryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Received Unencrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::receivedEncryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Received Encrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::connectionClosed) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Connection Closed, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
            } else if (msgType == DVPSIPCMessage::connectionAborted) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Connection Aborted, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::requestedUnencryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Requested Unencrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::requestedEncryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Requested Encrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::receivedDICOMObject) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Received DICOM Object, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else if (msgType == DVPSIPCMessage::sentDICOMObject) {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "Sent DICOM Object, status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
                if (msg.extractStringFromPayload(str)) oss << str << OFendl; else oss << "No description (should not happen)." << OFendl;
            } else {
                if (msg.extractIntFromPayload(i)) oss << "#" << i << ": "; else oss << "unknown client: ";
                oss << "received unknown message type " << msg.getMessageType() << ", status: ";
                if (msg.extractIntFromPayload(i)) oss << statusString(i) << OFendl; else oss << "(none)" << OFendl;
            }
            oss << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, result);
            OFLOG_INFO(msgservLogger, result);
            OFSTRINGSTREAM_FREESTR(result);
            msg.erasePayload();
            if (msg.getMessageType() == DVPSIPCMessage::requestApplicationID)
            {
              msg.setMessageType(DVPSIPCMessage::assignApplicationID);
              msg.addIntToPayload(++clientID);
            } else {
              msg.setMessageType(DVPSIPCMessage::OK);
            }
            if (! msg.send(connection))
            {
              OFLOG_WARN(msgservLogger, "unable to send response message, closing connection");
              finished = OFTrue;
            }
          } else finished = OFTrue;
        }
        // connection has been closed by the client or something has gone wrong.
        // clean up connection and wait for next client.
        connection.close();
      }
    }

    OFStandard::shutdownNetwork();

    return 0;
}
