/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens, Andrew Hewett
 *
 *  Purpose: Class representing a console engine for basic worklist
 *           management service class providers.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 17:14:51 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wlmceng.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"
#include "dicom.h"
#include "ofcmdln.h"
#include "wltypdef.h"
#include "dcxfer.h"
#include "ofconapp.h"
#include "assoc.h"
#include "cmdlnarg.h"
#include "dimse.h"
#include "dcdebug.h"
#include "dcvrat.h"
#include "dcvrlo.h"
#include "wlmactmg.h"

#include "wlmceng.h"

// ----------------------------------------------------------------------------

#define SHORTCOL 4
#define LONGCOL 21

// ----------------------------------------------------------------------------

WlmConsoleEngine::WlmConsoleEngine( int argc, char *argv[], WlmDataSourceType dataSourceTypev, const char *applicationName )
// Date         : December 17, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : argc            - [in] Specifies how many arguments were passed to the program from the
//                                  command line.
//                argv            - [in] An array of null-terminated strings containing the arguments that
//                                  were passed to the program from the command line.
//                dataSourceTypev - [in] The type of data source.
//                applicationName - [in] The name of this console application.
// Return Value : none.
  : dataSourceType( dataSourceTypev ), opt_dbDsn( NULL ), opt_dbUserName( NULL ), opt_dbUserPassword( NULL ),
    opt_serialNumber( 1 ), opt_dfPath( NULL ), opt_port( 0 ), opt_refuseAssociation( OFFalse ),
    opt_rejectWithoutImplementationUID( OFFalse ), opt_sleepAfterFind( 0 ), opt_sleepDuringFind( 0 ),
    opt_maxPDU( ASC_DEFAULTMAXPDU ), opt_networkTransferSyntax( EXS_Unknown ), opt_groupLength( EGL_recalcGL ),
    opt_sequenceType( EET_ExplicitLength ), opt_verbose( OFFalse ), opt_debug( OFFalse ),
    opt_failInvalidQuery( OFTrue ), opt_maxAssociations( 20 ), app( NULL ), cmd( NULL )
{
  // Initialize application identification string.
  sprintf( rcsid, "$dcmtk: %s v%s %s $", applicationName, OFFIS_DCMTK_VERSION, OFFIS_DCMTK_RELEASEDATE );

  // Initialize starting values for variables pertaining to program options.
  if( dataSourceType == DATA_SOURCE_IS_DATABASE )
  {
    opt_dbDsn = "text";
    opt_dbUserName = "pki.txt";
    opt_dbUserPassword = "";
  }
  else if( dataSourceType == DATA_SOURCE_IS_DATA_FILES )
  {
    opt_dfPath = "/home/www/wlist";
  }
#ifdef HAVE_FORK
  opt_singleProcess = OFFalse;
#else
  opt_singleProcess = OFTrue;
#endif

  // Initialize program options and parameters.
  char tempstr[20];
  app = new OFConsoleApplication( applicationName , "DICOM Basic Worklist Management SCP", rcsid );
  cmd = new OFCommandLine();

  cmd->setParamColumn(LONGCOL+SHORTCOL+4);
  cmd->addParam("port", "tcp/ip port number to listen on");

  cmd->setOptionColumns(LONGCOL, SHORTCOL);
  cmd->addGroup("general options:", LONGCOL, SHORTCOL+2);
    cmd->addOption("--help",                      "-h",        "print this help text and exit");
    cmd->addOption("--verbose",                   "-v",        "verbose mode, print processing details");
    cmd->addOption("--debug",                     "-d",        "debug mode, print debug information");
#ifdef HAVE_FORK
    cmd->addOption("--single-process",            "-s",        "single process mode");
#endif

  if( dataSourceType == DATA_SOURCE_IS_DATABASE )
  {
    OFString opt6 = "data source name of database\n(default: ";
    opt6 += opt_dbDsn;
    opt6 += ")";
    cmd->addOption("--data-source-name",          "-dsn",    1, "[n]ame: string", opt6.c_str() );

    OFString opt7 = "name of database user\n(default: ";
    opt7 += opt_dbUserName;
    opt7 += ")";
    cmd->addOption("--db-user-name",              "-dbu",    1, "[n]ame: string", opt7.c_str() );

    OFString opt8 = "password for database user\n(default: <none>)";
//    opt8 += opt_dbUserPassword;
//    opt8 += ")";
    cmd->addOption("--db-user-password",          "-dbp",    1, "[p]assword: string", opt8.c_str() );
    cmd->addOption("--serial-number",             "-sn",     1, "[s]erial number: integer (1..9999)", 
                                                                "serial number of this installation,\nwill be added to StudyInstanceUID" );
  }
  else if( dataSourceType == DATA_SOURCE_IS_DATA_FILES )
  {
    OFString opt5 = "path to worklist data files\n(default: ";
    opt5 += opt_dfPath;
    opt5 += ")";
    cmd->addOption("--data-files-path",           "-dfp",    1, "[p]ath: string", opt5.c_str() );
  }

  cmd->addGroup("network options:");
    cmd->addSubGroup("preferred network transfer syntaxes:");
      cmd->addOption("--prefer-uncompr",        "+x=",       "prefer explicit VR local byte order (default)");
      cmd->addOption("--prefer-little",         "+xe",       "prefer explicit VR little endian TS");
      cmd->addOption("--prefer-big",            "+xb",       "prefer explicit VR big endian TS");
      cmd->addOption("--implicit",              "+xi",       "accept implicit VR little endian TS only");
    cmd->addSubGroup("other network options:");
      cmd->addOption("--refuse",                             "refuse association");
      cmd->addOption("--reject",                             "reject association if no implement. class UID");
      cmd->addOption("--no-fail",                            "don't fail on an invalid query");
      cmd->addOption("--sleep-after",                     1, "[s]econds: integer", "sleep s seconds after find (default: 0)");
      cmd->addOption("--sleep-during",                    1, "[s]econds: integer", "sleep s seconds during find (default: 0)");
      OFString opt3 = "set max receive pdu to n bytes (default: ";
      sprintf(tempstr, "%ld", (long)ASC_DEFAULTMAXPDU);
      opt3 += tempstr;
      opt3 += ")";
      OFString opt4 = "[n]umber of bytes: integer [";
      sprintf(tempstr, "%ld", (long)ASC_MINIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += "..";
      sprintf(tempstr, "%ld", (long)ASC_MAXIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += "]";
      cmd->addOption("--max-pdu",                "-pdu",   1,  opt4.c_str(), opt3.c_str());
      cmd->addOption("--disable-host-lookup",    "-dhl",      "disable hostname lookup");
    cmd->addSubGroup("group length encoding (when sending C-FIND response data):");
      cmd->addOption("--group-length-recalc",    "+g=",       "recalculate group lengths if present (default)");
      cmd->addOption("--group-length-create",    "+g",        "always write with group length elements");
      cmd->addOption("--group-length-remove",    "-g",        "always write without group length elements");
    cmd->addSubGroup("length encoding in sequences and items (when sending C-FIND response data):");
      cmd->addOption("--length-explicit",        "+e",        "write with explicit lengths (default)");
      cmd->addOption("--length-undefined",       "-e",        "write with undefined lengths");

  cmd->addGroup("encoding options:");
    cmd->addSubGroup("post-1993 value representations:");
      cmd->addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd->addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");

  // Evaluate command line.
  prepareCmdLineArgs( argc, argv, applicationName );
  if( app->parseCommandLine( *cmd, argc, argv, OFCommandLine::ExpandWildcards ) )
  {
    // Check for --help first
    if( cmd->findOption("--help") ) app->printUsage();

    app->checkParam(cmd->getParamAndCheckMinMax(1, opt_port, 1, 65535));
    if( cmd->findOption("--verbose") ) opt_verbose = OFTrue;
    if( cmd->findOption("--debug") )
    {
      opt_debug = OFTrue;
      DUL_Debug(OFTrue);
      DIMSE_debug(OFTrue);
      SetDebugLevel(3);
    }
#ifdef HAVE_FORK
    if( cmd->findOption("--single-process") ) opt_singleProcess = OFTrue;
#endif
    if( dataSourceType == DATA_SOURCE_IS_DATABASE )
    {
      if( cmd->findOption("--data-source-name") ) app->checkValue(cmd->getValue(opt_dbDsn));
      if( cmd->findOption("--db-user-name") ) app->checkValue(cmd->getValue(opt_dbUserName));
      if( cmd->findOption("--db-user-password") ) app->checkValue(cmd->getValue(opt_dbUserPassword));
      if( cmd->findOption("--serial-number") ) app->checkValue(cmd->getValueAndCheckMinMax(opt_serialNumber, 1, 9999));
    }
    else if( dataSourceType == DATA_SOURCE_IS_DATA_FILES )
    {
      if( cmd->findOption("--data-files-path") ) app->checkValue(cmd->getValue(opt_dfPath));
    }
    cmd->beginOptionBlock();
    if( cmd->findOption("--prefer-uncompr") )  opt_networkTransferSyntax = EXS_Unknown;
    if( cmd->findOption("--prefer-little") )   opt_networkTransferSyntax = EXS_LittleEndianExplicit;
    if( cmd->findOption("--prefer-big") )      opt_networkTransferSyntax = EXS_BigEndianExplicit;
    if( cmd->findOption("--implicit") )        opt_networkTransferSyntax = EXS_LittleEndianImplicit;
    cmd->endOptionBlock();
    if( cmd->findOption("--refuse") ) opt_refuseAssociation = OFTrue;
    if( cmd->findOption("--reject") ) opt_rejectWithoutImplementationUID = OFTrue;
    if( cmd->findOption("--no-fail") ) opt_failInvalidQuery = OFFalse;
    if( cmd->findOption("--sleep-after") ) app->checkValue(cmd->getValueAndCheckMin(opt_sleepAfterFind, 0));
    if( cmd->findOption("--sleep-during") ) app->checkValue(cmd->getValueAndCheckMin(opt_sleepDuringFind, 0));
    if( cmd->findOption("--max-pdu") ) app->checkValue(cmd->getValueAndCheckMinMax(opt_maxPDU, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
    if( cmd->findOption("--disable-host-lookup") ) dcmDisableGethostbyaddr.set(OFTrue);
    cmd->beginOptionBlock();
    if( cmd->findOption("--group-length-recalc") ) opt_groupLength = EGL_recalcGL;
    if( cmd->findOption("--group-length-create") ) opt_groupLength = EGL_withGL;
    if( cmd->findOption("--group-length-remove") ) opt_groupLength = EGL_withoutGL;
    cmd->endOptionBlock();
    cmd->beginOptionBlock();
    if( cmd->findOption("--length-explicit") ) opt_sequenceType = EET_ExplicitLength;
    if( cmd->findOption("--length-undefined") ) opt_sequenceType = EET_UndefinedLength;
    cmd->endOptionBlock();
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
  }
}

// ----------------------------------------------------------------------------

WlmConsoleEngine::~WlmConsoleEngine()
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

int WlmConsoleEngine::StartProvidingService()
// Date         : December 17, 2001
// Author       : Thomas Wilkens
// Task         : Starts providing the implemented service for calling SCUs.
//                After having created an instance of this class, this function
//                shall be called from main.
// Parameters   : none.
// Return Value : Return value that is supposed to be returned from main().
{
  int result = 0;

  // start providing the basic worklist management service
  WlmActivityManager *activityManager = new WlmActivityManager( dataSourceType, opt_dbDsn, opt_dbUserName,
                                                                opt_dbUserPassword, opt_dfPath, opt_port,
                                                                opt_refuseAssociation,
                                                                opt_rejectWithoutImplementationUID,
                                                                opt_sleepAfterFind, opt_sleepDuringFind,
                                                                opt_maxPDU, opt_networkTransferSyntax,
                                                                opt_groupLength, opt_sequenceType, opt_verbose,
                                                                opt_debug, opt_failInvalidQuery,
                                                                opt_singleProcess, opt_maxAssociations,
                                                                &ofConsole, opt_serialNumber );
  OFCondition cond = activityManager->StartProvidingService();
  if( cond.bad() )
  {
    // in case something unexpected happened, dump a corresponding message
    ofConsole.lockCout();
    ofConsole.getCout() << cond.text() << endl;
    ofConsole.unlockCout();

    // set result to 1 (indicating an error); this value will be returned from main.
    result = 1;
  }
  delete activityManager;

  // return result
  return( result );
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wlmceng.cc,v $
** Revision 1.2  2002-01-08 17:14:51  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/
