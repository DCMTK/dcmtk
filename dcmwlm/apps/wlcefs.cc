/*
 *
 *  Copyright (C) 1996-2019, OFFIS e.V.
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
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Class representing a console engine for basic worklist
 *           management service class providers based on the file system.
 *
 */

// ----------------------------------------------------------------------------

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmwlm/wltypdef.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmwlm/wlds.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmwlm/wldsfs.h"
#include "dcmtk/dcmwlm/wlmactmg.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

#include "wlcefs.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

// ----------------------------------------------------------------------------

#define SHORTCOL 4
#define LONGCOL 21

static OFLogger wlmscpfsLogger = OFLog::getLogger("dcmtk.apps.wlmscpfs");

// ----------------------------------------------------------------------------

WlmConsoleEngineFileSystem::WlmConsoleEngineFileSystem( int argc, char *argv[], const char *applicationName, WlmDataSource *dataSourcev )
// Date         : December 17, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : argc            - [in] Specifies how many arguments were passed to the program from the
//                                  command line.
//                argv            - [in] An array of null-terminated strings containing the arguments that
//                                  were passed to the program from the command line.
//                applicationName - [in] The name of this console application.
//                dataSourcev     - [in] Pointer to the dataSource object.
// Return Value : none.
  : opt_returnedCharacterSet( RETURN_NO_CHARACTER_SET ),
    opt_dfPath( "" ), opt_rfPath( "" ), opt_rfFormat( "#t.dump" ), opt_refuseAssociation( OFFalse ),
    opt_rejectWithoutImplementationUID( OFFalse ), opt_sleepBeforeFindReq ( 0 ),
    opt_sleepAfterFind( 0 ), opt_sleepDuringFind( 0 ),
    opt_maxPDU( ASC_DEFAULTMAXPDU ), opt_networkTransferSyntax( EXS_Unknown ),
    opt_failInvalidQuery( OFTrue ), opt_singleProcess( OFTrue ),
    opt_forkedChild( OFFalse ), opt_maxAssociations( 50 ), opt_noSequenceExpansion( OFFalse ),
    opt_enableRejectionOfIncompleteWlFiles( OFTrue ), opt_blockMode(DIMSE_BLOCKING),
    opt_dimse_timeout(0), opt_acse_timeout(30), app( NULL ), cmd( NULL ), command_argc( argc ),
    command_argv(argv), dataSource( dataSourcev )
{
  // Initialize application identification string.
  sprintf( rcsid, "$dcmtk: %s v%s %s $", applicationName, OFFIS_DCMTK_VERSION, OFFIS_DCMTK_RELEASEDATE );

  // Initialize starting values for variables pertaining to program options.
  opt_dfPath = ".";

  // default: spawn new process for each incoming connection (fork()-OS or WIN32)
#if defined(HAVE_FORK) || defined(_WIN32)
  opt_singleProcess = OFFalse;
#else
  opt_singleProcess = OFTrue;
#endif

  // Initialize program options and parameters.
  char tempstr[20];
  app = new OFConsoleApplication( applicationName , "DICOM Basic Worklist Management SCP (based on data files)", rcsid );

  cmd = new OFCommandLine();

  cmd->setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd->addParam("port", "tcp/ip port number to listen on");

  cmd->setOptionColumns(LONGCOL, SHORTCOL);
  cmd->addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd->addOption("--help",                  "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd->addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
    OFLog::addOptions(*cmd);

#if defined(HAVE_FORK) || defined(_WIN32)
  cmd->addGroup("multi-process options:", LONGCOL, SHORTCOL + 2);
    cmd->addOption("--single-process",        "-s",      "single process mode");
    cmd->addOption("--fork",                             "fork child process for each association (def.)");
#ifdef _WIN32
    cmd->addOption("--forked-child",                     "process is forked child, internal use only", OFCommandLine::AF_Internal);
#endif
#endif

  cmd->addGroup("input options:");
    cmd->addSubGroup("general:");
      OFString opt5 = "[p]ath: string (default: ";
      opt5 += opt_dfPath;
      opt5 += ")";

      cmd->addOption("--data-files-path",     "-dfp", 1, opt5.c_str(), "path to worklist data files" );
    cmd->addSubGroup("handling of worklist files:");
      cmd->addOption("--enable-file-reject",  "-efr",    "enable rejection of incomplete worklist files\n(default)");
      cmd->addOption("--disable-file-reject", "-dfr",    "disable rejection of incomplete worklist files");

  cmd->addGroup("processing options:");
    cmd->addSubGroup("returned character set:");
      cmd->addOption("--return-no-char-set",  "-cs0",    "return no specific character set (default)");
      cmd->addOption("--return-iso-ir-100",   "-cs1",    "return specific character set ISO IR 100");
      cmd->addOption("--keep-char-set",       "-csk",    "return character set provided in file");
    cmd->addSubGroup("other processing options:");
      cmd->addOption("--no-sq-expansion",     "-nse",    "disable expansion of empty sequences in C-FIND\nrequest messages");
      cmd->addOption("--request-file-path",   "-rfp", 1, "[p]ath: string", "path to store request files to");
      cmd->addOption("--request-file-format", "-rff", 1, "[f]ormat: string (default: #t.dump)", "request file name format");


  cmd->addGroup("network options:");
    cmd->addSubGroup("preferred network transfer syntaxes:");
      cmd->addOption("--prefer-uncompr",      "+x=",     "prefer explicit VR local byte order (default)");
      cmd->addOption("--prefer-little",       "+xe",     "prefer explicit VR little endian TS");
      cmd->addOption("--prefer-big",          "+xb",     "prefer explicit VR big endian TS");
#ifdef WITH_ZLIB
      cmd->addOption("--prefer-deflated",     "+xd",     "prefer deflated explicit VR little endian TS");
#endif
      cmd->addOption("--implicit",            "+xi",     "accept implicit VR little endian TS only");

#ifdef WITH_TCPWRAPPER
    cmd->addSubGroup("network host access control (tcp wrapper):");
      cmd->addOption("--access-full",         "-ac",     "accept connections from any host (default)");
      cmd->addOption("--access-control",      "+ac",     "enforce host access control rules");
#endif

    cmd->addSubGroup("post-1993 value representations:");
      cmd->addOption("--enable-new-vr",       "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd->addOption("--disable-new-vr",      "-u",      "disable support for new VRs, convert to OB");

#ifdef WITH_ZLIB
    cmd->addSubGroup("deflate compression level (only with --prefer-deflated):");
      cmd->addOption("--compression-level",   "+cl",  1, "[l]evel: integer (default: 6)",
                                                         "0=uncompressed, 1=fastest, 9=best compression");
#endif

    cmd->addSubGroup("other network options:");
      cmd->addOption("--acse-timeout",        "-ta",  1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd->addOption("--dimse-timeout",       "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");

      OFString opt6 = "[a]ssocs: integer (default: ";
      sprintf(tempstr, "%ld", OFstatic_cast(long, opt_maxAssociations));
      opt6 += tempstr;
      opt6 += ")";
      cmd->addOption("--max-associations",            1, opt6.c_str(), "limit maximum number of parallel associations");
      cmd->addOption("--refuse",                         "refuse association");
      cmd->addOption("--reject",                         "reject association if no implement. class UID");
      cmd->addOption("--no-fail",                        "don't fail on an invalid query");
      cmd->addOption("--sleep-before",                1, "[s]econds: integer", "sleep s seconds before find (default: 0)");
      cmd->addOption("--sleep-after",                 1, "[s]econds: integer", "sleep s seconds after find (default: 0)");
      cmd->addOption("--sleep-during",                1, "[s]econds: integer", "sleep s seconds during find (default: 0)");
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
      cmd->addOption("--max-pdu",             "-pdu", 1, opt4.c_str(), opt3.c_str());
      cmd->addOption("--disable-host-lookup", "-dhl",    "disable hostname lookup");

  // Evaluate command line.
  prepareCmdLineArgs( argc, argv, applicationName );
  if( app->parseCommandLine( *cmd, argc, argv ) )
  {
    /* check exclusive options first */
    if (cmd->hasExclusiveOption())
    {
      if (cmd->findOption("--version"))
      {
        app->printHeader(OFTrue /*print host identifier*/);
        ofConsole.lockCout() << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_TCPWRAPPER)
        ofConsole.getCout() << " none" << OFendl;
#else
        ofConsole.getCout() << OFendl;
#endif
#ifdef WITH_ZLIB
        ofConsole.getCout() << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_TCPWRAPPER
        ofConsole.getCout() << "- LIBWRAP" << OFendl;
#endif
        ofConsole.unlockCout();
        exit(0);
      }
    }
    /* command line parameters and options */
    app->checkParam(cmd->getParamAndCheckMinMax(1, opt_port, 1, 65535));

    OFLog::configureFromCommandLine(*cmd, *app);

#if defined(HAVE_FORK) || defined(_WIN32)
    cmd->beginOptionBlock();
    if (cmd->findOption("--single-process")) opt_singleProcess = OFTrue;
    if (cmd->findOption("--fork")) opt_singleProcess = OFFalse;
    cmd->endOptionBlock();
#ifdef _WIN32
    if (cmd->findOption("--forked-child")) opt_forkedChild = OFTrue;
#endif
#endif

    if( cmd->findOption("--data-files-path") ) app->checkValue(cmd->getValue(opt_dfPath));
    if( cmd->findOption("--request-file-path") ) app->checkValue(cmd->getValue(opt_rfPath));
    if( cmd->findOption("--request-file-format") )
    {
        app->checkDependence("--request-file-format", "--request-file-path", !opt_rfPath.empty());
        app->checkValue(cmd->getValue(opt_rfFormat));
    }

    cmd->beginOptionBlock();
    if( cmd->findOption("--enable-file-reject") ) opt_enableRejectionOfIncompleteWlFiles = OFTrue;
    if( cmd->findOption("--disable-file-reject") ) opt_enableRejectionOfIncompleteWlFiles = OFFalse;
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if( cmd->findOption("--return-no-char-set") ) opt_returnedCharacterSet = RETURN_NO_CHARACTER_SET;
    if( cmd->findOption("--return-iso-ir-100") ) opt_returnedCharacterSet = RETURN_CHARACTER_SET_ISO_IR_100;
    if( cmd->findOption("--keep-char-set") ) opt_returnedCharacterSet = RETURN_CHARACTER_SET_FROM_FILE;
    cmd->endOptionBlock();

    if( cmd->findOption("--no-sq-expansion") ) opt_noSequenceExpansion = OFTrue;

    cmd->beginOptionBlock();
    if( cmd->findOption("--prefer-uncompr") ) opt_networkTransferSyntax = EXS_Unknown;
    if( cmd->findOption("--prefer-little") ) opt_networkTransferSyntax = EXS_LittleEndianExplicit;
    if( cmd->findOption("--prefer-big") ) opt_networkTransferSyntax = EXS_BigEndianExplicit;
    if( cmd->findOption("--implicit") ) opt_networkTransferSyntax = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
    if (cmd->findOption("--prefer-deflated")) opt_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
    cmd->endOptionBlock();

#ifdef WITH_TCPWRAPPER
    cmd->beginOptionBlock();
    if (cmd->findOption("--access-full")) dcmTCPWrapperDaemonName.set(NULL);
    if (cmd->findOption("--access-control")) dcmTCPWrapperDaemonName.set(applicationName);
    cmd->endOptionBlock();
#endif

    cmd->beginOptionBlock();
    if (cmd->findOption("--enable-new-vr")) dcmEnableGenerationOfNewVRs();
    if (cmd->findOption("--disable-new-vr")) dcmDisableGenerationOfNewVRs();
    cmd->endOptionBlock();

#ifdef WITH_ZLIB
    if (cmd->findOption("--compression-level"))
    {
      OFCmdUnsignedInt opt_compressionLevel = 0;
      app->checkDependence("--compression-level", "--prefer-deflated",
        (opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit));
      app->checkValue(cmd->getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
      dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
    }
#endif

    if (cmd->findOption("--acse-timeout"))
    {
      OFCmdSignedInt opt_timeout = 0;
      app->checkValue(cmd->getValueAndCheckMin(opt_timeout, 1));
      opt_acse_timeout = OFstatic_cast(int, opt_timeout);
    }

    if (cmd->findOption("--dimse-timeout"))
    {
      OFCmdSignedInt opt_timeout = 0;
      app->checkValue(cmd->getValueAndCheckMin(opt_timeout, 1));
      opt_dimse_timeout = OFstatic_cast(int, opt_timeout);
      opt_blockMode = DIMSE_NONBLOCKING;
    }

    if( cmd->findOption("--max-associations") )
    {
        OFCmdSignedInt maxAssoc = 1;
        app->checkValue(cmd->getValueAndCheckMin(maxAssoc, 1));
        opt_maxAssociations = OFstatic_cast(int, maxAssoc);
    }
    if( cmd->findOption("--refuse") ) opt_refuseAssociation = OFTrue;
    if( cmd->findOption("--reject") ) opt_rejectWithoutImplementationUID = OFTrue;
    if( cmd->findOption("--no-fail") ) opt_failInvalidQuery = OFFalse;
    if( cmd->findOption("--sleep-before") ) app->checkValue(cmd->getValueAndCheckMin(opt_sleepBeforeFindReq, 0));
    if( cmd->findOption("--sleep-after") ) app->checkValue(cmd->getValueAndCheckMin(opt_sleepAfterFind, 0));
    if( cmd->findOption("--sleep-during") ) app->checkValue(cmd->getValueAndCheckMin(opt_sleepDuringFind, 0));
    if( cmd->findOption("--max-pdu") ) app->checkValue(cmd->getValueAndCheckMinMax(opt_maxPDU, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
    if( cmd->findOption("--disable-host-lookup") ) dcmDisableGethostbyaddr.set(OFTrue);
  }

  // dump application information
  if (!opt_forkedChild)
  {
    /* print resource identifier */
    OFLOG_DEBUG(wlmscpfsLogger, rcsid << OFendl);
  }

  // set general parameters in data source object
  dataSource->SetNoSequenceExpansion( opt_noSequenceExpansion );
  dataSource->SetReturnedCharacterSet( opt_returnedCharacterSet );

  // set specific parameters in data source object
  dataSource->SetDfPath( opt_dfPath );
  dataSource->SetEnableRejectionOfIncompleteWlFiles( opt_enableRejectionOfIncompleteWlFiles );
}

// ----------------------------------------------------------------------------

WlmConsoleEngineFileSystem::~WlmConsoleEngineFileSystem()
// Date         : December 17, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  delete cmd;
  delete app;
}

// ----------------------------------------------------------------------------

int WlmConsoleEngineFileSystem::StartProvidingService()
// Date         : December 17, 2001
// Author       : Thomas Wilkens
// Task         : Starts providing the implemented service for calling SCUs.
//                After having created an instance of this class, this function
//                shall be called in order to start the SCP.
// Parameters   : none.
// Return Value : Indicator that shows if the function was executed successfully or not.
{
  OFCondition cond;

  // connect to data source
  cond = dataSource->ConnectToDataSource();
  if( cond.bad() )
  {
    // in case something unexpected happened, dump a corresponding message
    OFLOG_ERROR(wlmscpfsLogger, cond.text());

    // return error
    return( 1 );
  }

  // start providing the basic worklist management service
  WlmActivityManager *activityManager = new WlmActivityManager(
      dataSource,
      opt_port,
      opt_refuseAssociation,
      opt_rejectWithoutImplementationUID,
      opt_sleepBeforeFindReq,
      opt_sleepAfterFind,
      opt_sleepDuringFind,
      opt_maxPDU,
      opt_networkTransferSyntax,
      opt_failInvalidQuery,
      opt_singleProcess,
      opt_maxAssociations,
      opt_blockMode,
      opt_dimse_timeout,
      opt_acse_timeout,
      opt_forkedChild,
      command_argc,
      command_argv );

  if (!activityManager->setRequestFilePath(opt_rfPath, opt_rfFormat))
  {
      // dump error if given directory is not sufficient
      OFLOG_ERROR(wlmscpfsLogger, "Request file directory (" << opt_rfPath << ") does not exist or is not writable");
      // free memory
      delete activityManager;
      // return error
      return( 1 );
  }

  cond = activityManager->StartProvidingService();
  if( cond.bad() )
  {
    // in case something unexpected happened, dump a corresponding message
    OFLOG_ERROR(wlmscpfsLogger, cond.text());

    // disconnect from data source
    dataSource->DisconnectFromDataSource();

    // free memory
    delete activityManager;

    // return error
    return( 1 );
  }

  // free memory
  delete activityManager;

  // disconnect from data source
  cond = dataSource->DisconnectFromDataSource();
  if( cond.bad() )
  {
    // in case something unexpected happened, dump a corresponding message
    OFLOG_ERROR(wlmscpfsLogger, cond.text());

    // return error
    return( 1 );
  }

  // return result
  return( 0 );
}
