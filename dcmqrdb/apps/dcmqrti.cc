/*
 *
 *  Copyright (C) 1993-2010, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Telnet Initiator (ti) Main Program
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 13:37:32 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#define INCLUDE_CTIME
#define INCLUDE_CSIGNAL
#include "dcmtk/ofstd/ofstdinc.h"
BEGIN_EXTERN_C
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_SIGNAL_H
// On Solaris with Sun Workshop 11, <signal.h> declares signal() but <csignal> does not
#include <signal.h>
#endif
END_EXTERN_C
#include "dcmtk/dcmqrdb/dcmqrtis.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmqrdb/dcmqrcnf.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/ofstd/ofconapp.h"
#ifdef WITH_ZLIB
#include <zlib.h>          /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmqrti"
#define MAXREMOTEDBTITLES 20
#define APPLICATIONTITLE "TELNET_INITIATOR"
#define SHORTCOL 4
#define LONGCOL 18

static OFLogger dcmqrtiLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";
DcmQueryRetrieveConfig config;
DcmQueryRetrieveTelnetInitiator conf(config);

/*
 * Handle interrupt signals.
 * We only really need to make sure that the display is clear
 * before quiting.
 */
#ifdef SIGNAL_HANDLER_WITH_ELLIPSE
extern "C" void TI_signalHandler(...)
#else
extern "C" void TI_signalHandler(int)
#endif
{
  conf.TI_detachAssociation(OFTrue);
  exit( 1 );
}

int main( int argc, char *argv[] )
{

  const char *remoteDBTitles[ MAXREMOTEDBTITLES ];
  int remoteDBTitlesCount = 0;
  const char *configFileName = DEFAULT_CONFIGURATION_DIR "dcmqrscp.cfg";
  E_TransferSyntax networkTransferSyntax = EXS_Unknown;
  int opt_acse_timeout = 30;

  const char *currentPeer = NULL, **vendorHosts, **aeTitleList;
  OFBool noCommandLineValueForMaxReceivePDULength = OFTrue;
  int peerCount, j, n, returnValue = 0;
  OFCondition cond = EC_Normal;
  char tempstr[20];
  OFString temp_str;

#ifdef HAVE_GUSI_H
  // needed for Macintosh
  GUSISetup( GUSIwithSIOUXSockets );
  GUSISetup( GUSIwithInternetSockets );
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  // we need at least version 1.1
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup( winSockVersionNeeded, &winSockData );
#endif

  // initialize conf structure
  conf.setAETitle(APPLICATIONTITLE);

  // setup console application and command line objects
  OFConsoleApplication app( OFFIS_CONSOLE_APPLICATION , "Telnet initiator", rcsid );
  OFCommandLine cmd;

  cmd.setParamColumn( LONGCOL + SHORTCOL + 2 );
  cmd.addParam( "peer", "peer host name or symbolic name from cfg file", OFCmdParam::PM_MultiMandatory );

  cmd.setOptionColumns( LONGCOL, SHORTCOL );
  cmd.addGroup( "general options:");
    cmd.addOption( "--help",                      "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive );
    cmd.addOption( "--version",                              "print version information and exit", OFCommandLine::AF_Exclusive );
    OFLog::addOptions(cmd);

    if (strlen(configFileName) > 21)
    {
        OFString opt0 = "use specific configuration file\n(default: ";
        opt0 += configFileName;
        opt0 += ")";
        cmd.addOption( "--config",                "-c",   1, "[f]ilename: string", opt0.c_str() );
    } else {
        OFString opt0 = "[f]ilename: string (default: ";
        opt0 += configFileName;
        opt0 += ")";
        cmd.addOption( "--config",                "-c",   1, opt0.c_str(), "use specific configuration file" );
    }

  cmd.addGroup( "network options:" );
    cmd.addOption( "--timeout",                   "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--acse-timeout",             "-ta",  1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",            "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");

    cmd.addOption( "--propose-implicit",          "-xi",     "propose implicit VR little endian TS only" );
    OFString opt1 = "set my AE title (default: ";
    opt1 += APPLICATIONTITLE;
    opt1 += ")";
    cmd.addOption( "--aetitle",                   "-aet", 1, "[a]etitle: string", opt1.c_str() );
    OFString opt2 = "[n]umber of bytes: integer (";
    sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MINIMUMPDUSIZE));
    opt2 += tempstr;
    opt2 += "..";
    sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MAXIMUMPDUSIZE));
    opt2 += tempstr;
    opt2 += ")";
    cmd.addOption( "--max-pdu",                   "-pdu", 1, opt2.c_str(), "set max receive pdu to n bytes\n(default: use value from configuration file)" );

  cmd.addGroup( "other options:" );
    cmd.addOption( "--disable-new-vr",            "-u",      "disable support for new VRs, convert to OB" );
    cmd.addOption( "--remote",                    "-rmt", 1, "[t]itle: string", "connect to remote database defined in cfg file" );

  // evaluate command line
  prepareCmdLineArgs( argc, argv, OFFIS_CONSOLE_APPLICATION );
  if( app.parseCommandLine( cmd, argc, argv, OFCommandLine::PF_ExpandWildcards ) )
  {
    /* check exclusive options first */
    if (cmd.hasExclusiveOption())
    {
      if( cmd.findOption("--version") )
      {
        app.printHeader( OFTrue /*print host identifier*/ );
        COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB)
        COUT << " none" << OFendl;
#else
        COUT << OFendl;
#endif
#ifdef WITH_ZLIB
        COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
        return( 0 );
      }
    }

    // command line parameters
    OFLog::configureFromCommandLine(cmd, app);

    if( cmd.findOption("--config") ) app.checkValue( cmd.getValue( configFileName ) );
    if( cmd.findOption("--propose-implicit") ) networkTransferSyntax = EXS_LittleEndianImplicit;

    conf.setXferSyntax(networkTransferSyntax);

    const char *myAE = NULL;
    if( cmd.findOption("--aetitle") )
    {
        app.checkValue( cmd.getValue( myAE ) );
        conf.setAETitle(myAE);
    }
    if( cmd.findOption("--max-pdu") )
    {
      OFCmdUnsignedInt pdu=0;
      app.checkValue( cmd.getValueAndCheckMinMax( pdu, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE ) );
      conf.setMaxPDU(pdu);
      noCommandLineValueForMaxReceivePDULength = OFFalse;
    }

    if (cmd.findOption("--timeout"))
    {
      OFCmdSignedInt opt_timeout = 0;
      app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
      dcmConnectionTimeout.set(OFstatic_cast(Sint32, opt_timeout));
    }

    if (cmd.findOption("--acse-timeout"))
    {
      OFCmdSignedInt opt_timeout = 0;
      app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
      opt_acse_timeout = OFstatic_cast(int, opt_timeout);
    }

    if (cmd.findOption("--dimse-timeout"))
    {
      OFCmdSignedInt opt_timeout = 0;
      app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
      conf.setBlockMode(DIMSE_NONBLOCKING, OFstatic_cast(int, opt_timeout));
    }

    if( cmd.findOption("--disable-new-vr") )
    {
      dcmEnableUnknownVRGeneration.set( OFFalse );
      dcmEnableUnlimitedTextVRGeneration.set( OFFalse );
    }

    if (cmd.findOption("--remote", 0, OFCommandLine::FOM_First))
    {
      do
      {
        if( remoteDBTitlesCount < MAXREMOTEDBTITLES )
        {
          app.checkValue( cmd.getValue( remoteDBTitles[remoteDBTitlesCount] ) );
          remoteDBTitlesCount++;
        }
        else OFLOG_ERROR(dcmqrtiLogger, "ti: Too many remote database titles");
      } while (cmd.findOption("--remote", 0, OFCommandLine::FOM_Next));
    }

  }

  /* print resource identifier */
  OFLOG_DEBUG(dcmqrtiLogger, rcsid << OFendl);

  // in case accessing the configuration file for reading is successful
  if( access( configFileName, R_OK ) != -1 )
  {
    // in case reading values from configuration file is successful
    if( config.init( configFileName ) == 1 )
    {
      // dump information
      if (dcmqrtiLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
        config.printConfig();

      // determine max pdu size from configuration file
      OFCmdUnsignedInt maxPDU = config.getMaxPDUSize();

      // in case the max pdu size was not set in the configuration file, or
      // in case its value is not in a certain range, use the default value
      if( maxPDU == 0 || maxPDU < ASC_MINIMUMPDUSIZE || maxPDU > ASC_MAXIMUMPDUSIZE )
      {
        OFLOG_WARN(dcmqrtiLogger, "ti: no or invalid max pdu size found in configuration file");
        maxPDU = ASC_DEFAULTMAXPDU;
      }

      // if no max pdu size was set on the command line then use config file value
      if( noCommandLineValueForMaxReceivePDULength ) conf.setMaxPDU(maxPDU);

      // go through all peers that were specified on the command line
      peerCount = cmd.getParamCount();
      for( int i=1 ; i<=peerCount ; i++ )
      {
        // determine current peer
        cmd.getParam( i, currentPeer );

        // in general, we now want to add host names to the conf structure; it might be
        // though that currentPeer is a symbolic name that stands for a number of hosts;
        // hence we need to check first if peer can stands for a symbolic name
        if( ( n = config.aeTitlesForSymbolicName( currentPeer, &aeTitleList ) ) > 0 )
        {
          // in case peer is a symbolic name and can be found in the host table,
          // determine corresponding host names and add them to conf structure
          const char *peerName = NULL;
          int portNumber;
          for( j=0 ; j<n ; j++ )
          {
            if( config.peerForAETitle( aeTitleList[j], &peerName, &portNumber ) )
              conf.addPeerName(peerName, configFileName);
          }

          // free memory
          if( aeTitleList )
            free( aeTitleList );
          aeTitleList = NULL;
        }
        else if( ( n = config.HostNamesForVendor( currentPeer, &vendorHosts ) ) > 0 )
        {
          // in case peer is a symbolic name and can be interpreted as a vendor name, add the
          // corresponding host names are known for for this vendor to the conf structure
          for( j=0 ; j<n ; j++ )
            conf.addPeerName(vendorHosts[j], configFileName);

          // free memory
          if( vendorHosts )
            free( vendorHosts );
          vendorHosts = NULL;
        }
        else
        {
          // in case peer is not a symbolic name but the name of a
          // specific host, add this host name to the conf structure
          conf.addPeerName(currentPeer, configFileName);
        }
      }

      // set "peer to talk to" to the first host
      // name in the array (this is the default)
      conf.activateFirstPeer();

      // load up configuration info
      conf.createConfigEntries(configFileName, remoteDBTitlesCount, remoteDBTitles);

      // only go ahead in case there is at least one database we know of
      if( conf.getdbCount() > 0 )
      {
        // dump information
        if (dcmqrtiLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
          conf.printConfig();

        // make sure data dictionary is loaded
        if( !dcmDataDict.isDictionaryLoaded() )
          OFLOG_WARN(dcmqrtiLogger, "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

        // if starting up network is successful
        cond = ASC_initializeNetwork( NET_REQUESTOR, 0, opt_acse_timeout, conf.accessNet() );
        if( cond.good() )
        {
          // set interrupts for signal handling
#ifdef SIGHUP
          signal( SIGHUP, TI_signalHandler );
#endif
#ifdef SIGINT
          signal( SIGINT, TI_signalHandler );
#endif
#ifdef SIGQUIT
          signal( SIGQUIT, TI_signalHandler );
#endif
#ifdef SIGTERM
          signal( SIGTERM, TI_signalHandler );
#endif

          // do the real work
          conf.TI_userInput();

          // clean up network
          cond = ASC_dropNetwork( conf.accessNet() );
          if( cond.bad() )
          {
            OFLOG_ERROR(dcmqrtiLogger, "ti: cannot drop network: " << DimseCondition::dump(temp_str, cond));
            returnValue = 1;
          }
        }
        else
        {
          OFLOG_ERROR(dcmqrtiLogger, "ti: cannot initialize network: " << DimseCondition::dump(temp_str, cond));
          returnValue = 1;
        }
      }
      else
      {
        OFLOG_ERROR(dcmqrtiLogger, "ti: no accessable databases");
        returnValue = 1;
      }
    }
    else
    {
      OFLOG_ERROR(dcmqrtiLogger, "ti: cannot read configuration file '" << configFileName << "'");
      returnValue = 1;
    }
  }
  else
  {
    OFLOG_ERROR(dcmqrtiLogger, "ti: cannot access configuration file '" << configFileName << "'");
    returnValue = 1;
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  // return result
  return( returnValue );
}

//-------------------------------------------------------------------------------------------------


/*
 * CVS Log
 * $Log: dcmqrti.cc,v $
 * Revision 1.20  2010-11-01 13:37:32  uli
 * Fixed some compiler warnings reported by gcc with additional flags.
 *
 * Revision 1.19  2010-10-14 13:13:47  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.18  2010-09-09 14:59:30  joergr
 * Made log messages more consistent. Replaced '\n' by OFendl where appropriate.
 *
 * Revision 1.17  2009-11-24 10:10:41  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.16  2009-11-18 12:17:30  uli
 * Fix compiler errors due to removal of DUL_Debug() and DIMSE_Debug().
 *
 * Revision 1.15  2009-08-21 09:49:41  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.14  2009-08-19 11:56:32  meichel
 * Function passed as 4th parameter to qsort() now declared extern "C",
 *   needed for Sun Studio 11 on Solaris.
 *
 * Revision 1.13  2009-04-24 12:29:29  joergr
 * Fixed minor inconsistencies regarding layout/formatting in syntax usage.
 *
 * Revision 1.12  2009-04-21 14:11:45  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.11  2008-09-25 15:34:37  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.10  2006/10/27 09:01:30  joergr
 * Fixed wrong name of configuration file.
 *
 * Revision 1.9  2006/08/15 16:09:33  meichel
 * Updated the code in module dcmqrdb to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.8  2006/07/27 14:48:05  joergr
 * Fixed typo.
 *
 * Revision 1.7  2006/07/27 14:47:05  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.6  2006/07/17 11:38:03  joergr
 * Modified behaviour of option "--config": By default, the file "dcmqrscp.cfg"
 * in the configuration directory (e.g. "/usr/local/etc") is used.
 *
 * Revision 1.5  2005/12/08 15:47:03  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2005/11/17 13:44:59  meichel
 * Added command line options for DIMSE and ACSE timeouts
 *
 * Revision 1.3  2005/11/16 14:59:00  meichel
 * Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
 *   the responsiveness of the tools if the peer blocks during assoc negotiation.
 *
 * Revision 1.2  2005/06/16 08:05:48  meichel
 * Fixed typo in method name
 *
 * Revision 1.1  2005/03/30 13:34:44  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
