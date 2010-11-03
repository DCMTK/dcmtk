/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-03 12:32:07 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
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

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef WITH_TCPWRAPPER
    // this code makes sure that the linker cannot optimize away
    // the DUL part of the network module where the external flags
    // for libwrap are defined. Needed on OpenBSD.
    dcmTCPWrapperDaemonName.set(NULL);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    OFCmdUnsignedInt opt_port = 0;                   /* listen port */
    Uint32      clientID      = 0;                   /* IDs assigned to connecting clients */

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
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
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

#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if ((networkPort < 1024)&&(geteuid() != 0))
    {
      OFLOG_FATAL(msgservLogger, "cannot listen on port " << networkPort << ", insufficient privileges");
      return 10;
    }
#endif

    /* open listen socket */
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
      OFLOG_FATAL(msgservLogger, "failed to create socket");
      return 10;
    }

#ifdef HAVE_GUSI_H
    /* GUSI always returns an error for setsockopt(...) */
#else
    int reuse = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
    {
      OFLOG_FATAL(msgservLogger, "failed to set socket options");
      return 10;
    }
#endif

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

#if defined(HAVE_SETUID) && defined(HAVE_GETUID)
      /* return to normal uid so that we can't do too much damage in case
       * things go very wrong.   Only relevant if the program is setuid root,
       * and run by another user.  Running as root user may be
       * potentially disasterous if this program screws up badly.
       */
      setuid(getuid());
#endif

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
      nfound = select(s + 1, (int *)(&fdset), NULL, NULL, &t);
#else
      nfound = select(s + 1, &fdset, NULL, NULL, &t);
#endif


      if (nfound > 0)
      {
        // incoming connection detected
        int sock=0;
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
        } while ((sock == -1)&&(errno == EINTR));

        if (sock < 0)
        {
          OFLOG_FATAL(msgservLogger, "unable to accept incoming connection");
          return 10;
        }

#ifdef HAVE_GUSI_H
        /* GUSI always returns an error for setsockopt(...) */
#else
        reuse = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
        {
          OFLOG_FATAL(msgservLogger, "failed to set socket options");
          return 10;
        }
#endif

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

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: msgserv.cc,v $
 * Revision 1.19  2010-11-03 12:32:07  uli
 * Fixed some more warnings by gcc with by additional flags.
 *
 * Revision 1.18  2010-10-14 13:15:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.17  2010-06-02 12:31:12  joergr
 * Appended missing OFStringStream_ends to the end of output streams because
 * this is required when OFOStringStream is mapped to ostrstream.
 *
 * Revision 1.16  2010-04-29 10:44:56  joergr
 * Use printError() method for command line parsing errors only. In all other
 * cases use log output. Added flag for exclusive options to --help.
 * Fixed issue with syntax usage (e.g. layout and formatting).
 *
 * Revision 1.15  2009-11-24 14:12:59  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.14  2009-08-19 14:46:14  meichel
 * Added additional includes needed for Sun Studio 11 on Solaris.
 *
 * Revision 1.13  2006/08/15 16:57:02  meichel
 * Updated the code in module dcmpstat to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.12  2006/07/27 14:42:59  joergr
 * Prepended prefix "PF_" to parseLine() flags.
 *
 * Revision 1.11  2005/12/14 17:43:42  meichel
 * Adapted code for compilation with TCP wrappers to NetBSD
 *
 * Revision 1.10  2005/12/12 15:14:34  meichel
 * Added code needed for compilation with TCP wrappers on OpenBSD
 *
 * Revision 1.9  2005/12/08 15:47:00  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.8  2005/11/14 18:07:38  meichel
 * Included cstdlib, needed on HP/UX 10.
 *
 * Revision 1.7  2004/04/21 17:15:45  joergr
 * Included "dcompat" header file required for definition of bzero() on IRIX 5.
 *
 * Revision 1.6  2004/02/13 14:02:08  joergr
 * Added "#include <sys/time.h>" to compile with gcc 2.95.x on Linux 2.2.x.
 *
 * Revision 1.5  2002/11/27 15:48:19  meichel
 * Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 * Revision 1.4  2001/06/01 15:50:43  meichel
 * Updated copyright header
 *
 * Revision 1.3  2000/12/12 16:45:41  meichel
 * Minor changes to keep gcc 2.7.x on SunOS 4.1.3 happy
 *
 * Revision 1.2  2000/11/08 18:38:32  meichel
 * Updated dcmpstat IPC protocol for additional message parameters
 *
 * Revision 1.1  2000/10/10 12:24:13  meichel
 * Implemented test server for IPC message communication
 *
 */
