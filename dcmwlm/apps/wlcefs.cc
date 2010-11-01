/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 13:37:32 $
 *  CVS/RCS Revision: $Revision: 1.29 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
    opt_dfPath( "" ), opt_port( 0 ), opt_refuseAssociation( OFFalse ),
    opt_rejectWithoutImplementationUID( OFFalse ), opt_sleepAfterFind( 0 ), opt_sleepDuringFind( 0 ),
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
  opt_dfPath = "/home/www/wlist";

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

  cmd->addGroup("network options:");
    cmd->addSubGroup("preferred network transfer syntaxes:");
      cmd->addOption("--prefer-uncompr",      "+x=",     "prefer explicit VR local byte order (default)");
      cmd->addOption("--prefer-little",       "+xe",     "prefer explicit VR little endian TS");
      cmd->addOption("--prefer-big",          "+xb",     "prefer explicit VR big endian TS");
      cmd->addOption("--implicit",            "+xi",     "accept implicit VR little endian TS only");

#ifdef WITH_TCPWRAPPER
    cmd->addSubGroup("network host access control (tcp wrapper):");
      cmd->addOption("--access-full",         "-ac",     "accept connections from any host (default)");
      cmd->addOption("--access-control",      "+ac",     "enforce host access control rules");
#endif

    cmd->addSubGroup("post-1993 value representations:");
      cmd->addOption("--enable-new-vr",       "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd->addOption("--disable-new-vr",      "-u",      "disable support for new VRs, convert to OB");

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
  if( app->parseCommandLine( *cmd, argc, argv, OFCommandLine::PF_ExpandWildcards ) )
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
    cmd->endOptionBlock();

#ifdef WITH_TCPWRAPPER
    cmd->beginOptionBlock();
    if (cmd->findOption("--access-full")) dcmTCPWrapperDaemonName.set(NULL);
    if (cmd->findOption("--access-control")) dcmTCPWrapperDaemonName.set(applicationName);
    cmd->endOptionBlock();
#endif

    cmd->beginOptionBlock();
    if( cmd->findOption("--enable-new-vr") )
    {
      dcmEnableUnknownVRGeneration.set(OFTrue);
      dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
    }
    if( cmd->findOption("--disable-new-vr") )
    {
      dcmEnableUnknownVRGeneration.set(OFFalse);
      dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
    }
    cmd->endOptionBlock();

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
      dataSource, opt_port,
      opt_refuseAssociation,
      opt_rejectWithoutImplementationUID,
      opt_sleepAfterFind, opt_sleepDuringFind,
      opt_maxPDU, opt_networkTransferSyntax,
      opt_failInvalidQuery,
      opt_singleProcess, opt_maxAssociations,
      opt_blockMode, opt_dimse_timeout, opt_acse_timeout,
      opt_forkedChild, command_argc, command_argv );
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

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wlcefs.cc,v $
** Revision 1.29  2010-11-01 13:37:32  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.28  2010-10-14 13:13:53  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.27  2010-05-28 13:23:16  joergr
** Changed logger name from "dcmtk.apps.wlcefs" to "dcmtk.apps.wlmscpfs".
**
** Revision 1.26  2009-11-24 10:40:01  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.25  2009-11-18 12:17:30  uli
** Fix compiler errors due to removal of DUL_Debug() and DIMSE_Debug().
**
** Revision 1.24  2009-04-21 14:14:05  joergr
** Fixed minor inconsistencies in manpage / syntax usage.
**
** Revision 1.23  2009-04-17 11:54:13  joergr
** Added new command line option --fork in order to explicitly indicate what
** the default behavior is (multi-processing).
**
** Revision 1.22  2009-03-02 17:14:38  joergr
** Restructured command line options (be more consistent with other tools).
**
** Revision 1.21  2009-01-07 17:20:21  joergr
** Avoid double output of resource identifier for forked children (Win32).
**
** Revision 1.20  2008-09-26 12:05:17  joergr
** Changed the way exclusive command line options are checked.
**
** Revision 1.19  2008-09-25 15:16:17  joergr
** Added support for printing the expanded command line arguments.
**
** Revision 1.18  2007/08/10 14:25:20  meichel
** Added new command line option --keep-char-set that returns
**   any specific character set as encoded in the worklist file.
**
** Revision 1.17  2006/12/15 14:44:09  onken
** Changed member variable from char* to OFString and reintegrated correct
** intending of command line options, that were lost in of of last revisions.
** Removed (unchecked) command line options for group / sequence length
** calculations.
**
** Revision 1.16  2006/08/16 13:14:35  onken
** Hid (internal) "--forked-child" option from user
**
** Revision 1.15  2006/08/15 16:15:47  meichel
** Updated the code in module dcmwlm to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.14  2006/08/15 11:32:06  onken
** Added WIN32 multiprocess mode capabilities to wlmscpfs
**
** Revision 1.13  2006/08/14 15:30:40  onken
** Added WIN32 multiprocess mode to wlmscpfs.
**
** Revision 1.12  2006/07/27 14:53:49  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
** Option "--help" is no longer an exclusive option by default.
** Added optional library "LIBWRAP" to output of option "--version".
**
** Revision 1.11  2006/02/23 12:50:30  joergr
** Fixed layout and formatting issues.
**
** Revision 1.10  2005/12/08 15:48:30  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.9  2005/11/17 13:45:34  meichel
** Added command line options for DIMSE and ACSE timeouts
**
** Revision 1.8  2005/05/04 11:33:47  wilkens
** Added two command line options --enable-file-reject (default) and
** --disable-file-reject to wlmscpfs: these options can be used to enable or
** disable a file rejection mechanism which makes sure only complete worklist files
** will be used during the matching process. A worklist file is considered to be
** complete if it contains all necessary type 1 information which the SCP might
** have to return to an SCU in a C-Find response message.
**
** Revision 1.7  2004/02/24 14:45:47  meichel
** Added max-associations command line option, changed default to 50.
**
** Revision 1.6  2003/06/10 13:54:35  meichel
** Added support for TCP wrappers based host access control
**
** Revision 1.5  2003/02/17 12:02:00  wilkens
** Made some minor modifications to be able to modify a special variant of the
** worklist SCP implementation (wlmscpki).
**
** Revision 1.4  2002/11/26 08:53:00  meichel
** Replaced all includes for "zlib.h" with <zlib.h>
**   to avoid inclusion of zlib.h in the makefile dependencies.
**
** Revision 1.3  2002/09/23 18:36:58  joergr
** Added new command line option "--version" which prints the name and version
** number of external libraries used (incl. preparation for future support of
** 'config.guess' host identifiers).
**
** Revision 1.2  2002/08/12 10:55:47  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.1  2002/08/05 09:09:17  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.7  2002/07/17 13:10:21  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.6  2002/06/10 11:24:55  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.5  2002/05/08 13:20:40  wilkens
** Added new command line option -nse to wlmscpki and wlmscpdb.
**
** Revision 1.4  2002/04/18 14:19:55  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.3  2002/01/08 17:44:44  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 17:14:51  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/
