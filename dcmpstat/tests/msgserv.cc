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
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Sample message server for class DVPSIPCClient
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-11-08 18:38:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/tests/msgserv.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 
#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

BEGIN_EXTERN_C
#include <stdio.h>
#include <errno.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
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
#ifdef HAVE_TIME_H
#include <time.h>
#endif
END_EXTERN_C

#include "dvpsmsg.h"    /* for class DVPSIPCMessage */
#include "cmdlnarg.h"   /* for prepareCmdLineArgs */
#include "ofconapp.h"   /* for class OFConsoleApplication */
#include "dcdebug.h"    /* for SetDebugLevel */
#include "dcmtrans.h"   /* for class DcmTCPConnection */
#include "dcuid.h"

#define OFFIS_CONSOLE_APPLICATION "msgserv"

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

#define SHORTCOL 2
#define LONGCOL 9

int main(int argc, char *argv[])
{
        
#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif
        
    int         opt_debugMode   = 0;                   /* default: no debug */
    int         opt_verbose     = 0;                   /* default: not verbose */
    OFCmdUnsignedInt opt_port   = 0;                   /* listen port */
    Uint32      clientID        = 0;                   /* IDs assigned to connecting clients */
    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Sample message server for class DVPSIPCClient", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  
    cmd.addParam("port",  "port number to listen at");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
 
    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      cmd.getParam(1, opt_port);
      if (cmd.findOption("--verbose")) opt_verbose = 1;
      if (cmd.findOption("--debug")) opt_debugMode = 3;
    }
  
    if (opt_verbose)
    {
      CERR << rcsid << endl << endl;
    }
    
    SetDebugLevel((opt_debugMode));

    unsigned short networkPort    = (unsigned short) opt_port;    
    if (networkPort==0)
    {
        CERR << "error: no or invalid port number" << endl;
        return 10;
    }

#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if ((networkPort < 1024)&&(geteuid() != 0))
    {
        CERR << "error: cannot listen on port " << networkPort << ", insufficient privileges" << endl;
        return 10;
    }
#endif
        
    /* open listen socket */
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) 
    {
        CERR << "error: failed to create socket." << endl;
        return 10;
    }

#ifdef HAVE_GUSI_H
    /* GUSI always returns an error for setsockopt(...) */
#else
    int reuse = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
    {
      CERR << "error: failed to set socket options." << endl;
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
      CERR << "error: failed to bind socket to port, already in use?" << endl;
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
#elif HAVE_INTP_ACCEPT
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
          CERR << "error: unable to accept incoming connection" << endl;
          return 10;
        }

#ifdef HAVE_GUSI_H
        /* GUSI always returns an error for setsockopt(...) */
#else
        reuse = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
        {
          CERR << "error: failed to set socket options." << endl;
          return 10;
        }
#endif
      
        // now we can handle the incoming connection
        DcmTCPConnection connection(sock);
        DVPSIPCMessage msg;
        time_t now;
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
            // handle message
            time(&now);
            COUT << ctime(&now);
            msgType = msg.getMessageType();
            if (msgType == DVPSIPCMessage::OK)
            {
                COUT << "received 'OK' (should not happen)" << endl;
            } else if (msgType == DVPSIPCMessage::requestApplicationID) {
                COUT << "New client requests application ID, assigning #" << clientID+1 << endl
                     << "Application Type: ";
                if (msg.extractIntFromPayload(i)) COUT << applicationType(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::assignApplicationID) {
                COUT << "received 'AssignApplicationID' (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::applicationTerminates) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Application Terminates, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
            } else if (msgType == DVPSIPCMessage::receivedUnencryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Received Unencrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::receivedEncryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Received Encrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::connectionClosed) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Connection Closed, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
            } else if (msgType == DVPSIPCMessage::connectionAborted) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Connection Aborted, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::requestedUnencryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Requested Unencrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::requestedEncryptedDICOMConnection) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Requested Encrypted DICOM Connection, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::receivedDICOMObject) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Received DICOM Object, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else if (msgType == DVPSIPCMessage::sentDICOMObject) {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "Sent DICOM Object, status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
                if (msg.extractStringFromPayload(str)) COUT << str << endl; else COUT << "No description (should not happen)." << endl;
            } else {
                if (msg.extractIntFromPayload(i)) COUT << "#" << i << ": "; else COUT << "unknown client: ";
                COUT << "received unknown message type " << msg.getMessageType() << ", status: ";
                if (msg.extractIntFromPayload(i)) COUT << statusString(i) << endl; else COUT << "(none)" << endl;
            }
            COUT << endl;
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
              CERR << "warning: unable to send response message, closing connection." << endl;
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
 * Revision 1.2  2000-11-08 18:38:32  meichel
 * Updated dcmpstat IPC protocol for additional message parameters
 *
 * Revision 1.1  2000/10/10 12:24:13  meichel
 * Implemented test server for IPC message communication
 *
 */
