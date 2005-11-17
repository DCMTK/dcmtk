/*
 *
 *  Copyright (C) 1993-2005, OFFIS
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
 *  Module:  dcmqrdb
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Image Server Central Test Node (ctn) Main Program
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-11-17 13:44:59 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmqrdb/apps/dcmqrscp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#define INCLUDE_CTIME
#define INCLUDE_LIBC
#include "ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif     
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
END_EXTERN_C

#include "dicom.h"
#include "dcmqropt.h"
#include "dimse.h"
#include "dcmqrcnf.h"
#include "dcmqrscp.h"
#include "dcdict.h"
#include "dcdebug.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"       /* for dcmtk version name */

#ifdef WITH_SQL_DATABASE
#include "dcmqrdbq.h"
#else
#include "dcmqrdbi.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#ifndef OFFIS_CONSOLE_APPLICATION
#define OFFIS_CONSOLE_APPLICATION "dcmqrscp"
#endif

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define APPLICATIONTITLE    "DCMQRSCP"

const char *opt_configFileName = "dcmqrscp.cfg";
OFBool      opt_checkFindIdentifier = OFFalse;
OFBool      opt_checkMoveIdentifier = OFFalse;
OFCmdUnsignedInt opt_port = 0;

void errmsg(const char* msg, ...)
{
    va_list args;

    fprintf(stderr, "%s: ", OFFIS_CONSOLE_APPLICATION);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

#define SHORTCOL 4
#define LONGCOL 21

int
main(int argc, char *argv[])
{
    OFCondition cond = EC_Normal;
    OFCmdUnsignedInt overridePort = 0;
    OFCmdUnsignedInt overrideMaxPDU = 0;
    DcmQueryRetrieveOptions options;

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

  char tempstr[20];
#ifdef HAVE_FORK
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM image archive (central test node)", rcsid);
#else
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM image archive (central test node)\nThis version of dcmqrscp supports only single process mode.", rcsid);
#endif

  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL+SHORTCOL+4);
  cmd.addParam("port", "tcp/ip port number to listen on\n(default: in config file)", OFCmdParam::PM_Optional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL+2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--version",                                "print version information and exit", OFTrue /* exclusive */);
   cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
   cmd.addOption("--very-verbose",              "-vv",       "print more processing details");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
   OFString opt5 = "[f]ilename: string (default: ";
   opt5 += opt_configFileName;
   opt5 += ")";
   cmd.addOption("--config",                    "-c",     1, opt5.c_str(), "use specific configuration file");
#ifdef HAVE_FORK
   cmd.addOption("--single-process",            "-s",        "single process mode");
#endif

  cmd.addGroup("database options:");
    cmd.addSubGroup("association negotiation:");
      cmd.addOption("--require-find",                        "reject all MOVE/GET presentation contexts for\nwhich no corresponding FIND context is proposed");
      cmd.addOption("--no-parallel-store",                   "reject multiple simultaneous STORE presentation\ncontexts for one application entity title");
      cmd.addOption("--disable-get",                         "disable C-GET support");
      cmd.addOption("--allow-shutdown",                      "allow external shutdown via a private SOP class");
    cmd.addSubGroup("checking identifier validity:");
      cmd.addOption("--check-find",             "-XF",       "check C-FIND identifier validity");
      cmd.addOption("--no-check-find",                       "do not check C-FIND identif. validity (default)");
      cmd.addOption("--check-move",             "-XM",       "check C-MOVE identifier validity");
      cmd.addOption("--no-check-move",                       "do not check C-MOVE identif. validity (default)");
    cmd.addSubGroup("restriction of move targets:");
      cmd.addOption("--move-unrestricted",                   "do not restrict move destination (default)");
      cmd.addOption("--move-aetitle",           "-ZA",       "restrict move dest. to requesting AE title");
      cmd.addOption("--move-host",              "-ZH",       "restrict move destination to requesting host");
      cmd.addOption("--move-vendor",            "-ZV",       "restrict move destination to requesting vendor");
    cmd.addSubGroup("restriction of query/retrieve models:");
      cmd.addOption("--no-patient-root",        "-QP",       "do not support Patient Root Q/R models");
      cmd.addOption("--no-study-root",          "-QS",       "do not support Study Root Q/R models");
#ifndef NO_PATIENTSTUDYONLY_SUPPORT
      cmd.addOption("--no-patient-study",       "-QO",       "do not support Patient/Study Only Q/R models");
#endif

  cmd.addGroup("network options:");
    cmd.addSubGroup("preferred network transfer syntaxes for uncompressed transfer:");
      cmd.addOption("--prefer-uncompr",         "+x=",       "prefer explicit VR local byte order (default)");
      cmd.addOption("--prefer-little",          "+xe",       "prefer explicit VR little endian TS");
      cmd.addOption("--prefer-big",             "+xb",       "prefer explicit VR big endian TS");
      cmd.addOption("--implicit",               "+xi",       "accept implicit VR little endian TS only");

#ifdef WITH_TCPWRAPPER
    cmd.addSubGroup("network host access control (tcp wrapper) options:");
      cmd.addOption("--access-full",            "-ac",       "accept connections from any host (default)");
      cmd.addOption("--access-control",         "+ac",       "enforce host access control rules");
#endif

    cmd.addSubGroup("other network options:");
      cmd.addOption("--timeout",                "-to",    1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--acse-timeout",           "-ta", 1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",          "-td", 1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");
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
      cmd.addOption("--max-pdu",                "-pdu",   1,  opt4.c_str(), opt3.c_str());
      cmd.addOption("--disable-host-lookup",    "-dhl",      "disable hostname lookup");
      cmd.addOption("--refuse",                              "refuse association");
      cmd.addOption("--reject",                              "reject association if no implement. class UID");
      cmd.addOption("--ignore",                              "ignore store data, receive but do not store");
      cmd.addOption("--uid-padding",            "-up",       "silently correct space-padded UIDs");

  cmd.addGroup("encoding options:");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");

  cmd.addGroup("output options:");
    cmd.addSubGroup("bit preserving mode:");
      cmd.addOption("--normal",                 "-B",        "allow implicit format conversions (default)");
      cmd.addOption("--bit-preserving",         "+B",        "write data exactly as read");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",             "+F",        "write file format (default)");
      cmd.addOption("--write-dataset",          "-F",        "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax (not with --bit-preserving or compressed transmission):");
      cmd.addOption("--write-xfer-same",        "+t=",       "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",      "+te",       "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",         "+tb",       "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",    "+ti",       "write with implicit VR little endian TS");
    cmd.addSubGroup("group length encoding (not with --bit-preserving):");
      cmd.addOption("--group-length-recalc",    "+g=",       "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create",    "+g",        "always write with group length elements");
      cmd.addOption("--group-length-remove",    "-g",        "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items (not with --bit-preserving):");
      cmd.addOption("--length-explicit",        "+e",        "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",       "-e",        "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset or --bit-preserving):");
      cmd.addOption("--padding-off",            "-p",        "no padding (default)");
      cmd.addOption("--padding-create",         "+p",    2,  "[f]ile-pad [i]tem-pad: integer", "align file on multiple of f bytes\nand items on multiple of i bytes");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      /* check exclusive options first */
      if (cmd.getParamCount() == 0)
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
            CERR << endl << "External libraries used:";
#ifdef WITH_ZLIB
            CERR << endl << "- ZLIB, Version " << zlibVersion() << endl;
#else
            CERR << " none" << endl;
#endif
            return 0;
         }
      }

      /* command line parameters and options */
      if (cmd.getParamCount() > 0) app.checkParam(cmd.getParamAndCheckMinMax(1, overridePort, 1, 65535));

      if (cmd.findOption("--verbose")) options.verbose_=1;
      if (cmd.findOption("--very-verbose")) options.verbose_=2;
      if (cmd.findOption("--debug"))
      {
        options.debug_ = OFTrue;
        DUL_Debug(OFTrue);
        DIMSE_debug(OFTrue);
        SetDebugLevel(3);
      }
      if (cmd.findOption("--config")) app.checkValue(cmd.getValue(opt_configFileName));
#ifdef HAVE_FORK
      if (cmd.findOption("--single-process")) options.singleProcess_ = OFTrue;
#endif

      if (cmd.findOption("--require-find")) options.requireFindForMove_ = OFTrue;
      if (cmd.findOption("--no-parallel-store")) options.refuseMultipleStorageAssociations_ = OFTrue;
      if (cmd.findOption("--disable-get")) options.disableGetSupport_ = OFTrue;
      if (cmd.findOption("--allow-shutdown")) options.allowShutdown_ = OFTrue;
      cmd.beginOptionBlock();
      if (cmd.findOption("--check-find")) opt_checkFindIdentifier = OFTrue;
      if (cmd.findOption("--no-check-find")) opt_checkFindIdentifier = OFFalse;
      cmd.endOptionBlock();
      cmd.beginOptionBlock();
      if (cmd.findOption("--check-move")) opt_checkMoveIdentifier = OFTrue;
      if (cmd.findOption("--no-check-move")) opt_checkMoveIdentifier = OFFalse;
      cmd.endOptionBlock();
      cmd.beginOptionBlock();
      if (cmd.findOption("--move-unrestricted"))
      {
    options.restrictMoveToSameAE_ = OFFalse;
    options.restrictMoveToSameHost_ = OFFalse;
    options.restrictMoveToSameVendor_ = OFFalse;
      }
      if (cmd.findOption("--move-aetitle")) options.restrictMoveToSameAE_ = OFTrue;
      if (cmd.findOption("--move-host")) options.restrictMoveToSameHost_ = OFTrue;
      if (cmd.findOption("--move-vendor")) options.restrictMoveToSameVendor_ = OFTrue;
      cmd.endOptionBlock();

      if (cmd.findOption("--no-patient-root")) options.supportPatientRoot_ = OFFalse;
      if (cmd.findOption("--no-study-root")) options.supportStudyRoot_ = OFFalse;
#ifndef NO_PATIENTSTUDYONLY_SUPPORT
      if (cmd.findOption("--no-patient-study")) options.supportPatientStudyOnly_ = OFFalse;
#endif
      if ((!options.supportPatientRoot_)&&(!options.supportStudyRoot_)&&(!options.supportPatientStudyOnly_))
      {
        app.printError("cannot disable all Q/R models");
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--prefer-uncompr"))  options.networkTransferSyntax_ = EXS_Unknown;
      if (cmd.findOption("--prefer-little"))   options.networkTransferSyntax_ = EXS_LittleEndianExplicit;
      if (cmd.findOption("--prefer-big"))      options.networkTransferSyntax_ = EXS_BigEndianExplicit;
      if (cmd.findOption("--implicit"))        options.networkTransferSyntax_ = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

#ifdef WITH_TCPWRAPPER
      cmd.beginOptionBlock();
      if (cmd.findOption("--access-full")) dcmTCPWrapperDaemonName.set(NULL);
      if (cmd.findOption("--access-control")) dcmTCPWrapperDaemonName.set(OFFIS_CONSOLE_APPLICATION);
      cmd.endOptionBlock();
#endif

      if (cmd.findOption("--timeout"))
      {
        OFCmdSignedInt opt_timeout = 0;
        app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
        dcmConnectionTimeout.set((Sint32) opt_timeout);
      }

      if (cmd.findOption("--acse-timeout"))
      {
        OFCmdSignedInt opt_timeout = 0;
        app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
        options.acse_timeout_ = OFstatic_cast(int, opt_timeout);
      }

      if (cmd.findOption("--dimse-timeout"))
      {
        OFCmdSignedInt opt_timeout = 0;
        app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
        options.dimse_timeout_ = OFstatic_cast(int, opt_timeout);
        options.blockMode_ = DIMSE_NONBLOCKING;
      }

      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(overrideMaxPDU, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--disable-host-lookup")) dcmDisableGethostbyaddr.set(OFTrue);
      if (cmd.findOption("--refuse")) options.refuse_ = OFTrue;
      if (cmd.findOption("--reject")) options.rejectWhenNoImplementationClassUID_ = OFTrue;
      if (cmd.findOption("--ignore")) options.ignoreStoreData_ = OFTrue;
      if (cmd.findOption("--uid-padding")) options.correctUIDPadding_ = OFTrue;

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--normal")) options.bitPreserving_ = OFFalse;
      if (cmd.findOption("--bit-preserving")) options.bitPreserving_ = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) options.useMetaheader_ = OFTrue;
      if (cmd.findOption("--write-dataset")) options.useMetaheader_ = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-same")) options.writeTransferSyntax_ = EXS_Unknown;
      if (cmd.findOption("--write-xfer-little"))
      {
        app.checkConflict("--write-xfer-little", "--bit-preserving", options.bitPreserving_);
        options.writeTransferSyntax_ = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-big"))
      {
        app.checkConflict("--write-xfer-big", "--bit-preserving", options.bitPreserving_);
        options.writeTransferSyntax_ = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-implicit"))
      {
        app.checkConflict("--write-xfer-implicit", "--bit-preserving", options.bitPreserving_);
        options.writeTransferSyntax_ = EXS_LittleEndianImplicit;
      }
      cmd.endOptionBlock();


      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-recalc"))
      {
        app.checkConflict("--group-length-recalc", "--bit-preserving", options.bitPreserving_);
        options.groupLength_ = EGL_recalcGL;
      }
      if (cmd.findOption("--group-length-create"))
      {
        app.checkConflict("--group-length-create", "--bit-preserving", options.bitPreserving_);
        options.groupLength_ = EGL_withGL;
      }
      if (cmd.findOption("--group-length-remove"))
      {
        app.checkConflict("--group-length-remove", "--bit-preserving", options.bitPreserving_);
        options.groupLength_ = EGL_withoutGL;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit"))
      {
        app.checkConflict("--length-explicit", "--bit-preserving", options.bitPreserving_);
        options.sequenceType_ = EET_ExplicitLength;
      }
      if (cmd.findOption("--length-undefined"))
      {
        app.checkConflict("--length-undefined", "--bit-preserving", options.bitPreserving_);
        options.sequenceType_ = EET_UndefinedLength;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--padding-off")) options.paddingType_ = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
        app.checkConflict("--padding-create", "--write-dataset", ! options.useMetaheader_);
        app.checkConflict("--padding-create", "--bit-preserving", options.bitPreserving_);
        app.checkValue(cmd.getValueAndCheckMin(options.filepad_, 0));
        app.checkValue(cmd.getValueAndCheckMin(options.itempad_, 0));
        options.paddingType_ = EPD_withPadding;
      }
      cmd.endOptionBlock();

   }

    /* read config file */
    if (access(opt_configFileName, R_OK) < 0) {
    errmsg("cannot access %s: %s", opt_configFileName, strerror(errno));
    return 10;
    }

    DcmQueryRetrieveConfig config;
    
    if (!config.init(opt_configFileName)) {
    errmsg("bad config file: %s", opt_configFileName);
    return 10;
    }
    options.maxAssociations_ = config.getMaxAssociations();

    opt_port = config.getNetworkTCPPort();
    if (opt_port == 0) opt_port = 104; /* not set, use default */
    if (overridePort > 0) opt_port = overridePort;

    options.maxPDU_ = config.getMaxPDUSize();
    if (options.maxPDU_ == 0) options.maxPDU_ = ASC_DEFAULTMAXPDU; /* not set, use default */
    if (options.maxPDU_ < ASC_MINIMUMPDUSIZE || options.maxPDU_ > ASC_MAXIMUMPDUSIZE)
    {
      app.printError("invalid MaxPDUSize in config file");
    }
    if (overrideMaxPDU > 0) options.maxPDU_ = overrideMaxPDU;

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
    fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
        DCM_DICT_ENVIRONMENT_VARIABLE);
    }

#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if (opt_port < 1024) {
        if (geteuid() != 0) {
        errmsg("cannot listen on port %d, insufficient privileges", (int)opt_port);
            return 10;
    }
    }
#endif

    cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, (int)opt_port, options.acse_timeout_, &options.net_);
    if (cond.bad()) {
    errmsg("Error initialising network:");
    DimseCondition::dump(cond);
        return 10;
    }

#if defined(HAVE_SETUID) && defined(HAVE_GETUID)
    /* return to normal uid so that we can't do too much damage in case
     * things go very wrong.   Only relevant if the program is setuid root,
     * and run by another user.  Running as root user may be
     * potentially disasterous if this program screws up badly.
     */
    setuid(getuid());
#endif

#if defined(HAVE_GETGRNAM) && defined(HAVE_GETPWNAM) && defined(HAVE_SETUID)
     struct group *grp = NULL;
     struct passwd *pwd = NULL;
     const char *opt_UserName = NULL;
     const char *opt_GroupName = NULL;

     if (((opt_GroupName = config.getGroupName()) != NULL) && strlen(opt_GroupName) > 0)
     {     
       if (!(grp = getgrnam(opt_GroupName)))
       {
         errmsg("Bad group name %s", opt_GroupName);
         return 10;
       }
       if (setgid(grp->gr_gid) == -1)
       {
         errmsg("setgid: Unable to set group id to group %u", (unsigned)grp->gr_gid);
         return 10;
       }
     }
     if (((opt_UserName = config.getUserName()) != NULL) && strlen(opt_UserName) > 0)
     {
       if (!(pwd = getpwnam(opt_UserName)))
       {
         errmsg("Bad user name %s\n", opt_UserName);
         return 10;
       }
       if (setuid(pwd->pw_uid) == -1)
       {
         errmsg("setuid: Unable to set user id to user %u", (unsigned)pwd->pw_uid);
         return 10;
       }
     }
#endif

#ifdef WITH_SQL_DATABASE
    // use SQL database
    DcmQueryRetrieveSQLDatabaseHandleFactory factory;
#else
    // use linear index database (index.dat)
    DcmQueryRetrieveIndexDatabaseHandleFactory factory(&config);
#endif

    DcmQueryRetrieveSCP scp(config, options, factory);
    scp.setDatabaseFlags(opt_checkFindIdentifier, opt_checkMoveIdentifier, options.debug_);
    
    /* loop waiting for associations */
    while (cond.good())
    {
      cond = scp.waitForAssociation(options.net_);
      if (!options.singleProcess_) scp.cleanChildren(options.verbose_ ? OFTrue : OFFalse);  /* clean up any child processes */
    }

    cond = ASC_dropNetwork(&options.net_);
    if (cond.bad()) {
        errmsg("Error dropping network:");
        DimseCondition::dump(cond);
        return 10;
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

    return 0;
}


/*
 * CVS Log
 * $Log: dcmqrscp.cc,v $
 * Revision 1.4  2005-11-17 13:44:59  meichel
 * Added command line options for DIMSE and ACSE timeouts
 *
 * Revision 1.3  2005/11/16 14:59:00  meichel
 * Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
 *   the responsiveness of the tools if the peer blocks during assoc negotiation.
 *
 * Revision 1.2  2005/04/22 15:37:34  meichel
 * Minor update to avoid warning on Win32
 *
 * Revision 1.1  2005/04/04 14:23:13  meichel
 * Renamed application "dcmqrdb" into "dcmqrscp" to avoid name clash with
 *   dcmqrdb library, which confuses the MSVC build system.
 *
 * Revision 1.3  2005/04/04 13:15:12  meichel
 * Added username/groupname configuration option that allows to start the
 *   image database as root and let it call setuid/setgid to execute under an
 *   unprivileged account once the listen socket has been opened.
 *
 * Revision 1.2  2005/04/04 10:05:43  meichel
 * Minor corrections for use with external DB interface
 *
 * Revision 1.1  2005/03/30 13:34:44  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
