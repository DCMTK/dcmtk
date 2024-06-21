/*
 *
 *  Copyright (C) 1993-2024, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

// On Solaris with Sun Workshop 11, <signal.h> declares signal() but <csignal> does not
#include <signal.h>
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
#include "dcmtk/ofstd/ofstd.h"

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
 * before quitting.
 */
extern "C" void TI_signalHandler(int)
{
  conf.TI_detachAssociation(OFTrue);
  exit( 1 );
}

int main( int argc, char *argv[] )
{
  const char *remoteDBTitles[ MAXREMOTEDBTITLES ];
  int remoteDBTitlesCount = 0;
  OFString configFileName = OFStandard::getDefaultConfigurationDir();
  configFileName += "dcmqrscp.cfg";
  E_TransferSyntax networkTransferSyntax = EXS_Unknown;
  int opt_acse_timeout = 30;

  const char *currentPeer = NULL, **vendorHosts, **aeTitleList;
  OFBool noCommandLineValueForMaxReceivePDULength = OFTrue;
  int peerCount, j, n, returnValue = 0;
  OFCondition cond = EC_Normal;
  char tempstr[20];
  OFString temp_str;

  OFStandard::initializeNetwork();

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

    if (configFileName.length() > 21)
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
    OFStandard::snprintf(tempstr, sizeof(tempstr), "%ld", OFstatic_cast(long, ASC_MINIMUMPDUSIZE));
    opt2 += tempstr;
    opt2 += "..";
    OFStandard::snprintf(tempstr, sizeof(tempstr), "%ld", OFstatic_cast(long, ASC_MAXIMUMPDUSIZE));
    opt2 += tempstr;
    opt2 += ")";
    cmd.addOption( "--max-pdu",                   "-pdu", 1, opt2.c_str(), "set max receive pdu to n bytes\n(default: use value from configuration file)" );

  cmd.addGroup( "other options:" );
    cmd.addOption( "--disable-new-vr",            "-u",      "disable support for new VRs, convert to OB" );
    cmd.addOption( "--remote",                    "-rmt", 1, "[t]itle: string", "connect to remote database defined in cfg file" );

  // evaluate command line
  prepareCmdLineArgs( argc, argv, OFFIS_CONSOLE_APPLICATION );
  if( app.parseCommandLine( cmd, argc, argv ) )
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

    if (cmd.findOption("--disable-new-vr"))
    {
      dcmDisableGenerationOfNewVRs();
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
  if( access( configFileName.c_str(), R_OK ) != -1 )
  {
    // in case reading values from configuration file is successful
    if( config.init( configFileName.c_str() ) == 1 )
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
              conf.addPeerName(peerName, configFileName.c_str());
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
            conf.addPeerName(vendorHosts[j], configFileName.c_str());

          // free memory
          if( vendorHosts )
            free( vendorHosts );
          vendorHosts = NULL;
        }
        else
        {
          // in case peer is not a symbolic name but the name of a
          // specific host, add this host name to the conf structure
          conf.addPeerName(currentPeer, configFileName.c_str());
        }
      }

      // set "peer to talk to" to the first host
      // name in the array (this is the default)
      conf.activateFirstPeer();

      // load up configuration info
      conf.createConfigEntries(configFileName.c_str(), remoteDBTitlesCount, remoteDBTitles);

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
        OFLOG_ERROR(dcmqrtiLogger, "ti: no accessible databases");
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

  OFStandard::shutdownNetwork();

  // return result
  return( returnValue );
}
