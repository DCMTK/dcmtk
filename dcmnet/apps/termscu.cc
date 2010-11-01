/*
 *
 *  Copyright (C) 2005-2010, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Termination Service Class User (negotiates the private shutdown
 *           SOP class in order to shutdown server applications)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 10:42:44 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"

#ifdef WITH_ZLIB
#include <zlib.h>
#endif

// ----------------------------------------------------------------------------

#define OFFIS_CONSOLE_APPLICATION "termscu"
#define APPLICATIONTITLE "TERMSCU"
#define PEERAPPLICATIONTITLE "ANY-SCP"
#define SHORTCOL 4
#define LONGCOL  11

static OFLogger termscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

// ----------------------------------------------------------------------------

int main( int argc, char *argv[] )
// Date       : September 12, 2005
// Author     : Thomas Wilkens
// Task       : Main function for this application.
// Parameters : args - [in] Number of command line arguments.
//              argv - [in] Command line arguments.
{
  const char *opt_peer = NULL;
  OFCmdUnsignedInt opt_port = 104;
  const char *opt_peerTitle = PEERAPPLICATIONTITLE;
  const char *opt_ourTitle = APPLICATIONTITLE;
  OFCmdUnsignedInt opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
  T_ASC_Network *net;
  T_ASC_Parameters *params;
  T_ASC_Association *assoc;
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";
  const char* transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax, UID_LittleEndianExplicitTransferSyntax, UID_BigEndianExplicitTransferSyntax };
  int transferSyntaxCount = 3;

#ifdef HAVE_GUSI_H
    // needed for Macintosh
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    // we need at least version 1.1
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

  char tempstr[20];
  OFString temp_str;
  OFConsoleApplication app( OFFIS_CONSOLE_APPLICATION , "DICOM termination SCU", rcsid );
  OFCommandLine cmd;

  cmd.setParamColumn( LONGCOL + SHORTCOL + 4 );
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");

  cmd.setOptionColumns( LONGCOL, SHORTCOL );
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
  cmd.addOption("--help",      "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
  cmd.addOption("--version",              "print version information and exit", OFCommandLine::AF_Exclusive);
  OFLog::addOptions(cmd);

  cmd.addGroup("network options:");
   cmd.addSubGroup("application entity titles:");
    OFString opt1 = "set my calling AE title (default: ";
    opt1 += APPLICATIONTITLE;
    opt1 += ")";
    cmd.addOption("--aetitle", "-aet", 1, "[a]etitle: string", opt1.c_str());
    OFString opt2 = "set called AE title of peer (default: ";
    opt2 += PEERAPPLICATIONTITLE;
    opt2 += ")";
    cmd.addOption("--call",    "-aec", 1, "[a]etitle: string", opt2.c_str());
   cmd.addSubGroup("other network options:");
    OFString opt3 = "set max receive pdu to n bytes (default: ";
    sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_DEFAULTMAXPDU));
    opt3 += tempstr;
    opt3 += ")";
    OFString opt4 = "[n]umber of bytes: integer (";
    sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MINIMUMPDUSIZE));
    opt4 += tempstr;
    opt4 += "..";
    sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MAXIMUMPDUSIZE));
    opt4 += tempstr;
    opt4 += ")";
    cmd.addOption("--max-pdu", "-pdu", 1, opt4.c_str(), opt3.c_str());

  // evaluate command line
  prepareCmdLineArgs( argc, argv, OFFIS_CONSOLE_APPLICATION );
  if( app.parseCommandLine( cmd, argc, argv ) )
  {
    // check exclusive options first
    if (cmd.hasExclusiveOption())
    {
      if( cmd.findOption("--version") )
      {
        app.printHeader( OFTrue /*print host identifier*/ );
        COUT << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
        COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
        COUT << " none" << OFendl;
#endif
        return( 0 );
      }
    }

    // command line parameters
    cmd.getParam( 1, opt_peer );
    app.checkParam( cmd.getParamAndCheckMinMax( 2, opt_port, 1, 65535 ) );

    // command line options
    OFLog::configureFromCommandLine(cmd, app);

    if( cmd.findOption("--aetitle") )
      app.checkValue( cmd.getValue( opt_ourTitle ) );
    if( cmd.findOption("--call") )
      app.checkValue( cmd.getValue( opt_peerTitle ) );
    if( cmd.findOption("--max-pdu") )
      app.checkValue( cmd.getValueAndCheckMinMax( opt_maxReceivePDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE ) );
  }

  /* print resource identifier */
  OFLOG_DEBUG(termscuLogger, rcsid << OFendl);

  // make sure data dictionary is loaded
  if( !dcmDataDict.isDictionaryLoaded() )
    OFLOG_WARN(termscuLogger, "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

  // initialize network
  OFCondition cond = ASC_initializeNetwork( NET_REQUESTOR, 0, 30, &net );
  if( cond.bad() )
  {
    OFLOG_FATAL(termscuLogger, DimseCondition::dump(temp_str, cond));
    exit( 1 );
  }

  // initialize asscociation parameters
  cond = ASC_createAssociationParameters( &params, opt_maxReceivePDULength );
  if( cond.bad() )
  {
    OFLOG_FATAL(termscuLogger, DimseCondition::dump(temp_str, cond));
    exit( 1 );
  }

  // set this application's title and the called application's title in the params structure
  ASC_setAPTitles( params, opt_ourTitle, opt_peerTitle, NULL );

  // set the transport layer type (type of network connection) in the params structure
  cond = ASC_setTransportLayerType( params, OFFalse );
  if( cond.bad() )
  {
    OFLOG_FATAL(termscuLogger, DimseCondition::dump(temp_str, cond));
    return( 1 );
  }

  // figure out the presentation addresses and copy the
  // corresponding values into the association parameters.
  gethostname( localHost, sizeof( localHost ) - 1 );
  sprintf( peerHost, "%s:%d", opt_peer, OFstatic_cast(int, opt_port));
  ASC_setPresentationAddresses( params, localHost, peerHost );

  // set the presentation context which will be negotiated
  // when the network connection will be established
  cond = ASC_addPresentationContext( params, 1, UID_PrivateShutdownSOPClass, transferSyntaxes, transferSyntaxCount );
  if( cond.bad() )
  {
    OFLOG_FATAL(termscuLogger, DimseCondition::dump(temp_str, cond));
    exit(1);
  }

  // dump presentation contexts
  OFLOG_DEBUG(termscuLogger, "Request Parameters:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ));

  // dump information
  OFLOG_INFO(termscuLogger, "Requesting Association");

  // create association
  cond = ASC_requestAssociation( net, params, &assoc );
  if( cond.bad() )
  {
    if( cond == DUL_ASSOCIATIONREJECTED )
    {
      T_ASC_RejectParameters rej;
      ASC_getRejectParameters( params, &rej );
      OFLOG_FATAL(termscuLogger, "Association Rejected: " << OFendl << ASC_printRejectParameters(temp_str, &rej));
      exit( 1 );
    }
    else
    {
      OFLOG_FATAL(termscuLogger, "Association Request Failed: " << DimseCondition::dump(temp_str, cond));
      exit( 1 );
    }
  }

  // dump the presentation contexts which have been accepted/refused
  OFLOG_DEBUG(termscuLogger, "Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

  // count the presentation contexts which have been accepted by the SCP
  // If there are none, finish the execution
  if( ASC_countAcceptedPresentationContexts( params ) == 0 )
  {
    OFLOG_FATAL(termscuLogger, "No Acceptable Presentation Contexts");
    exit( 1 );
  }

  // actually, the execution should never get to this point, because when the
  // private shutdown SOP class is negotiated, an association shall always be
  // rejected by an SCP; and if this application sends an association request
  // to another application which does not know of the private shutdown SOP
  // class, the above if-condition should have been met, so that we never
  // actually get to this point here; nevertheless, the following code is con-
  // tained in this file in order to "do nothing" if an association was accepted

  // dump general information concerning the establishment of the network connection if required
  OFLOG_INFO(termscuLogger, "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")");

  //
  // do nothing when the association was accepted
  //

  // dump information
  OFLOG_INFO(termscuLogger, "Releasing Association");

  // release association
  cond = ASC_releaseAssociation( assoc );
  if( cond.bad() )
  {
    OFLOG_FATAL(termscuLogger, "Association Release Failed: " << DimseCondition::dump(temp_str, cond));
    exit( 1 );
  }

  // destroy the association
  cond = ASC_destroyAssociation( &assoc );
  if( cond.bad() )
  {
    OFLOG_FATAL(termscuLogger, DimseCondition::dump(temp_str, cond));
    exit( 1 );
  }

  // drop the network
  cond = ASC_dropNetwork( &net );
  if( cond.bad() )
  {
    OFLOG_FATAL(termscuLogger, DimseCondition::dump(temp_str, cond));
    exit( 1 );
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  return( 0 );
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: termscu.cc,v $
** Revision 1.14  2010-11-01 10:42:44  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.13  2010-10-14 13:13:43  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.12  2010-05-18 16:10:25  joergr
** Replaced '\n' by OFendl in log messages.
**
** Revision 1.11  2009-11-18 11:53:58  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.10  2009-04-24 12:26:06  joergr
** Fixed minor inconsistencies regarding layout/formatting in syntax usage.
**
** Revision 1.9  2009-04-21 14:09:23  joergr
** Fixed minor inconsistencies in manpage / syntax usage.
**
** Revision 1.8  2008-09-25 16:00:58  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.7  2007/10/01 16:21:26  joergr
** Fixed layout issue with --version output.
**
** Revision 1.6  2006/08/15 16:04:28  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.5  2006/07/27 14:18:48  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Option "--help" is no longer an exclusive option
** by default.
** Removed option "ExpandWildcards" from parseComandLine() call.
**
** Revision 1.4  2005/12/08 15:44:23  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.3  2005/11/16 14:58:07  meichel
** Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
**   the responsiveness of the tools if the peer blocks during assoc negotiation.
**
** Revision 1.2  2005/09/15 08:41:42  joergr
** Fixed wrong value for LONGCOL.
**
** Revision 1.1  2005/09/12 11:39:04  wilkens
** Added command line tool termscu in dcmnet; this tool can be used to shutdown
** DCMTK server applications which support negotiating the private Shutdown SOP
** Class in DCMTK.
**
**
*/
