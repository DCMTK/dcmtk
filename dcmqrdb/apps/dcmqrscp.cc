/*
 *
 *  Copyright (C) 1993-2013, OFFIS e.V.
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
 *  Purpose: Image Server Central Test Node (ctn) Main Program
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#define INCLUDE_CTIME
#define INCLUDE_LIBC
#include "dcmtk/ofstd/ofstdinc.h"

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
END_EXTERN_C

#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmqrdb/dcmqropt.h"
#include "dcmtk/dcmqrdb/dcmqrcnf.h"
#include "dcmtk/dcmqrdb/dcmqrsrv.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"    /* for dcmZlibCompressionLevel */
#include "dcmtk/ofstd/ofgrp.h"
#include "dcmtk/ofstd/ofpwd.h"

#ifdef WITH_SQL_DATABASE
#include "dcmtk/dcmqrdbx/dcmqrdbq.h"
#else
#include "dcmtk/dcmqrdb/dcmqrdbi.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#ifndef OFFIS_CONSOLE_APPLICATION
#define OFFIS_CONSOLE_APPLICATION "dcmqrscp"
#endif

static OFLogger dcmqrscpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define APPLICATIONTITLE "DCMQRSCP"

const char *opt_configFileName = DEFAULT_CONFIGURATION_DIR "dcmqrscp.cfg";
OFBool      opt_checkFindIdentifier = OFFalse;
OFBool      opt_checkMoveIdentifier = OFFalse;
OFCmdUnsignedInt opt_port = 0;

#define SHORTCOL 4
#define LONGCOL 22

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
  OFString temp_str;
#ifdef HAVE_FORK
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "DICOM image archive (central test node)", rcsid);
#else
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "DICOM image archive (central test node)\nThis version of dcmqrscp supports only single process mode.", rcsid);
#endif

  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("port", "tcp/ip port number to listen on\n(default: in config file)", OFCmdParam::PM_Optional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                     "-h",        "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                               "print version information and exit", OFCommandLine::AF_Exclusive);
    OFLog::addOptions(cmd);

    if (strlen(opt_configFileName) > 16)
    {
        OFString opt5 = "use specific configuration file\n(default: ";
        opt5 += opt_configFileName;
        opt5 += ")";
        cmd.addOption("--config",               "-c",     1, "[f]ilename: string", opt5.c_str());
    } else {
        OFString opt5 = "[f]ilename: string (default: ";
        opt5 += opt_configFileName;
        opt5 += ")";
        cmd.addOption("--config",               "-c",     1, opt5.c_str(), "use specific configuration file");
    }
#ifdef HAVE_FORK
  cmd.addGroup("multi-process options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--single-process",           "-s",        "single process mode");
    cmd.addOption("--fork",                                  "fork child process for each assoc. (default)");
#endif

  cmd.addGroup("database options:");
    cmd.addSubGroup("association negotiation:");
      cmd.addOption("--require-find",                        "reject all MOVE/GET presentation contexts for\nwhich no correspond. FIND context is proposed");
      cmd.addOption("--no-parallel-store",                   "reject multiple simultaneous STORE presentat.\ncontexts for one application entity title");
      cmd.addOption("--disable-get",                         "disable C-GET support");
      cmd.addOption("--allow-shutdown",                      "allow external shutdown via private SOP class");
    cmd.addSubGroup("checking identifier validity:");
      cmd.addOption("--check-find",             "-XF",       "check C-FIND identifier validity");
      cmd.addOption("--no-check-find",                       "do not check C-FIND identifier validity (def.)");
      cmd.addOption("--check-move",             "-XM",       "check C-MOVE identifier validity");
      cmd.addOption("--no-check-move",                       "do not check C-MOVE identifier validity (def.)");
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
    cmd.addSubGroup("preferred network transfer syntaxes (incoming associations):");
      cmd.addOption("--prefer-uncompr",         "+x=",       "prefer explicit VR local byte order (default)");
      cmd.addOption("--prefer-little",          "+xe",       "prefer explicit VR little endian TS");
      cmd.addOption("--prefer-big",             "+xb",       "prefer explicit VR big endian TS");
#ifndef DISABLE_COMPRESSION_EXTENSION
      cmd.addOption("--prefer-lossless",        "+xs",       "prefer default JPEG lossless TS");
      cmd.addOption("--prefer-jpeg8",           "+xy",       "prefer default JPEG lossy TS for 8 bit data");
      cmd.addOption("--prefer-jpeg12",          "+xx",       "prefer default JPEG lossy TS for 12 bit data");
      cmd.addOption("--prefer-j2k-lossless",    "+xv",       "prefer JPEG 2000 lossless TS");
      cmd.addOption("--prefer-j2k-lossy",       "+xw",       "prefer JPEG 2000 lossy TS");
      cmd.addOption("--prefer-jls-lossless",    "+xt",       "prefer JPEG-LS lossless TS");
      cmd.addOption("--prefer-jls-lossy",       "+xu",       "prefer JPEG-LS lossy TS");
      cmd.addOption("--prefer-mpeg2",           "+xm",       "prefer MPEG2 Main Profile @ Main Level TS");
      cmd.addOption("--prefer-mpeg2-high",      "+xh",       "prefer MPEG2 Main Profile @ High Level TS");
      cmd.addOption("--prefer-mpeg4",           "+xn",       "prefer MPEG4 AVC/H.264 HP / Level 4.1 TS");
      cmd.addOption("--prefer-mpeg4-bd",        "+xl",       "prefer MPEG4 AVC/H.264 BD-compatible TS");
      cmd.addOption("--prefer-rle",             "+xr",       "prefer RLE lossless TS");
#ifdef WITH_ZLIB
      cmd.addOption("--prefer-deflated",        "+xd",       "prefer deflated expl. VR little endian TS");
#endif
#endif
      cmd.addOption("--implicit",               "+xi",       "accept implicit VR little endian TS only");

#ifndef DISABLE_COMPRESSION_EXTENSION
    cmd.addSubGroup("proposed transmission transfer syntaxes (outgoing associations):");
      cmd.addOption("--propose-uncompr",        "-x=",       "propose all uncompressed TS, explicit VR\nwith local byte ordering first (default)");
      cmd.addOption("--propose-little",         "-xe",       "propose all uncompressed TS, explicit VR\nlittle endian first");
      cmd.addOption("--propose-big",            "-xb",       "propose all uncompressed TS, explicit VR\nbig endian first");
      cmd.addOption("--propose-implicit",       "-xi",       "propose implicit VR little endian TS only");
      cmd.addOption("--propose-lossless",       "-xs",       "propose default JPEG lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jpeg8",          "-xy",       "propose default JPEG lossy TS for 8 bit data\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jpeg12",         "-xx",       "propose default JPEG lossy TS for 12 bit data\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-j2k-lossless",   "-xv",       "propose JPEG 2000 lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-j2k-lossy",      "-xw",       "propose JPEG 2000 lossy TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jls-lossless",   "-xt",       "propose JPEG-LS lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jls-lossy",      "-xu",       "propose JPEG-LS lossy TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-mpeg2",          "-xm",       "propose MPEG2 Main Profile @ Main Level TS only");
      cmd.addOption("--propose-mpeg2-high",     "-xh",       "propose MPEG2 Main Profile @ High Level TS only");
      cmd.addOption("--propose-mpeg4",          "-xn",       "propose MPEG4 AVC/H.264 HP / Level 4.1 TS only");
      cmd.addOption("--propose-mpeg4-bd",       "-xl",       "propose MPEG4 AVC/H.264 BD-compatible TS only");
      cmd.addOption("--propose-rle",            "-xr",       "propose RLE lossless TS\nand all uncompressed transfer syntaxes");
#ifdef WITH_ZLIB
      cmd.addOption("--propose-deflated",       "-xd",       "propose deflated expl. VR little endian TS\nand all uncompressed transfer syntaxes");
#endif
#endif

#ifdef WITH_TCPWRAPPER
    cmd.addSubGroup("network host access control (tcp wrapper):");
      cmd.addOption("--access-full",            "-ac",       "accept connections from any host (default)");
      cmd.addOption("--access-control",         "+ac",       "enforce host access control rules");
#endif

    cmd.addSubGroup("other network options:");
      cmd.addOption("--timeout",                "-to",    1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--acse-timeout",           "-ta",    1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",          "-td",    1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");
      OFString opt4 = "[n]umber of bytes: integer (";
      sprintf(tempstr, "%ld", (long)ASC_MINIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += "..";
      sprintf(tempstr, "%ld", (long)ASC_MAXIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += ")";
      cmd.addOption("--max-pdu",                "-pdu",   1, opt4.c_str(), "set max receive pdu to n bytes\n(default: use value from configuration file)");
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
#ifdef WITH_ZLIB
      cmd.addOption("--write-xfer-deflated",    "+td",       "write with deflated expl. VR little endian TS");
#endif
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
#ifdef WITH_ZLIB
#ifdef DISABLE_COMPRESSION_EXTENSION
    cmd.addSubGroup("deflate compression level (only with --write-xfer-deflated/same):");
#else
    cmd.addSubGroup("deflate compression level (only with -xd or --write-xfer-deflated/same):");
#endif
      cmd.addOption("--compression-level",      "+cl",   1, "[l]evel: integer (default: 6)",
                                                            "0=uncompressed, 1=fastest, 9=best compression");
#endif

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
          app.printHeader(OFTrue /*print host identifier*/);
          COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_TCPWRAPPER)
          COUT << " none" << OFendl;
#else
          COUT << OFendl;
#endif
#ifdef WITH_ZLIB
          COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_TCPWRAPPER
          COUT << "- LIBWRAP" << OFendl;
#endif
          return 0;
        }
      }

      /* command line parameters and options */
      if (cmd.getParamCount() > 0) app.checkParam(cmd.getParamAndCheckMinMax(1, overridePort, 1, 65535));

      OFLog::configureFromCommandLine(cmd, app);

      if (cmd.findOption("--config")) app.checkValue(cmd.getValue(opt_configFileName));
#ifdef HAVE_FORK
      cmd.beginOptionBlock();
      if (cmd.findOption("--single-process")) options.singleProcess_ = OFTrue;
      if (cmd.findOption("--fork")) options.singleProcess_ = OFFalse;
      cmd.endOptionBlock();
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
      if (!options.supportPatientRoot_ && !options.supportStudyRoot_ && !options.supportPatientStudyOnly_)
      {
        app.printError("cannot disable all Q/R models");
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--prefer-uncompr")) options.networkTransferSyntax_ = EXS_Unknown;
      if (cmd.findOption("--prefer-little")) options.networkTransferSyntax_ = EXS_LittleEndianExplicit;
      if (cmd.findOption("--prefer-big")) options.networkTransferSyntax_ = EXS_BigEndianExplicit;
#ifndef DISABLE_COMPRESSION_EXTENSION
      if (cmd.findOption("--prefer-lossless")) options.networkTransferSyntax_ = EXS_JPEGProcess14SV1;
      if (cmd.findOption("--prefer-jpeg8")) options.networkTransferSyntax_ = EXS_JPEGProcess1;
      if (cmd.findOption("--prefer-jpeg12")) options.networkTransferSyntax_ = EXS_JPEGProcess2_4;
      if (cmd.findOption("--prefer-j2k-lossless")) options.networkTransferSyntax_ = EXS_JPEG2000LosslessOnly;
      if (cmd.findOption("--prefer-j2k-lossy")) options.networkTransferSyntax_ = EXS_JPEG2000;
      if (cmd.findOption("--prefer-jls-lossless")) options.networkTransferSyntax_ = EXS_JPEGLSLossless;
      if (cmd.findOption("--prefer-jls-lossy")) options.networkTransferSyntax_ = EXS_JPEGLSLossy;
      if (cmd.findOption("--prefer-mpeg2")) options.networkTransferSyntax_ = EXS_MPEG2MainProfileAtMainLevel;
      if (cmd.findOption("--prefer-mpeg2-high")) options.networkTransferSyntax_ = EXS_MPEG2MainProfileAtHighLevel;
      if (cmd.findOption("--prefer-mpeg4")) options.networkTransferSyntax_ = EXS_MPEG4HighProfileLevel4_1;
      if (cmd.findOption("--prefer-mpeg4-bd")) options.networkTransferSyntax_ = EXS_MPEG4BDcompatibleHighProfileLevel4_1;
      if (cmd.findOption("--prefer-rle")) options.networkTransferSyntax_ = EXS_RLELossless;
#ifdef WITH_ZLIB
      if (cmd.findOption("--prefer-deflated")) options.networkTransferSyntax_ = EXS_DeflatedLittleEndianExplicit;
#endif
      if (cmd.findOption("--implicit")) options.networkTransferSyntax_ = EXS_LittleEndianImplicit;
#endif
      cmd.endOptionBlock();

#ifndef DISABLE_COMPRESSION_EXTENSION
      cmd.beginOptionBlock();
      if (cmd.findOption("--propose-uncompr")) options.networkTransferSyntaxOut_ = EXS_Unknown;
      if (cmd.findOption("--propose-little")) options.networkTransferSyntaxOut_ = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big")) options.networkTransferSyntaxOut_ = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) options.networkTransferSyntaxOut_ = EXS_LittleEndianImplicit;
      if (cmd.findOption("--propose-lossless")) options.networkTransferSyntaxOut_ = EXS_JPEGProcess14SV1;
      if (cmd.findOption("--propose-jpeg8")) options.networkTransferSyntaxOut_ = EXS_JPEGProcess1;
      if (cmd.findOption("--propose-jpeg12")) options.networkTransferSyntaxOut_ = EXS_JPEGProcess2_4;
      if (cmd.findOption("--propose-j2k-lossless")) options.networkTransferSyntaxOut_ = EXS_JPEG2000LosslessOnly;
      if (cmd.findOption("--propose-j2k-lossy")) options.networkTransferSyntaxOut_ = EXS_JPEG2000;
      if (cmd.findOption("--propose-jls-lossless")) options.networkTransferSyntaxOut_ = EXS_JPEGLSLossless;
      if (cmd.findOption("--propose-jls-lossy")) options.networkTransferSyntaxOut_ = EXS_JPEGLSLossy;
      if (cmd.findOption("--propose-mpeg2")) options.networkTransferSyntaxOut_ = EXS_MPEG2MainProfileAtMainLevel;
      if (cmd.findOption("--propose-mpeg2-high")) options.networkTransferSyntaxOut_ = EXS_MPEG2MainProfileAtHighLevel;
      if (cmd.findOption("--propose-mpeg4")) options.networkTransferSyntaxOut_ = EXS_MPEG4HighProfileLevel4_1;
      if (cmd.findOption("--propose-mpeg4-bd")) options.networkTransferSyntaxOut_ = EXS_MPEG4BDcompatibleHighProfileLevel4_1;
      if (cmd.findOption("--propose-rle")) options.networkTransferSyntaxOut_ = EXS_RLELossless;
#ifdef WITH_ZLIB
      if (cmd.findOption("--propose-deflated")) options.networkTransferSyntaxOut_ = EXS_DeflatedLittleEndianExplicit;
#endif
      cmd.endOptionBlock();
#endif

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
        dcmEnableOtherFloatStringVRGeneration.set(OFTrue);
        dcmEnableOtherDoubleStringVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
        dcmEnableOtherFloatStringVRGeneration.set(OFFalse);
        dcmEnableOtherDoubleStringVRGeneration.set(OFFalse);
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
#ifndef DISABLE_COMPRESSION_EXTENSION
        app.checkConflict("--write-xfer-little", "--prefer-lossless", options.networkTransferSyntax_ == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-little", "--prefer-jpeg8", options.networkTransferSyntax_ == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-little", "--prefer-jpeg12", options.networkTransferSyntax_ == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-little", "--prefer-j2k-lossless", options.networkTransferSyntax_ == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-little", "--prefer-j2k-lossy", options.networkTransferSyntax_ == EXS_JPEG2000);
        app.checkConflict("--write-xfer-little", "--prefer-jls-lossless", options.networkTransferSyntax_ == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-little", "--prefer-jls-lossy", options.networkTransferSyntax_ == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg2", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg2-high", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg4", options.networkTransferSyntax_ == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg4-bd", options.networkTransferSyntax_ == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-little", "--prefer-rle", options.networkTransferSyntax_ == EXS_RLELossless);
        // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
#endif
        options.writeTransferSyntax_ = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-big"))
      {
        app.checkConflict("--write-xfer-big", "--bit-preserving", options.bitPreserving_);
#ifndef DISABLE_COMPRESSION_EXTENSION
        app.checkConflict("--write-xfer-big", "--prefer-lossless", options.networkTransferSyntax_ == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-big", "--prefer-jpeg8", options.networkTransferSyntax_ == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-big", "--prefer-jpeg12", options.networkTransferSyntax_ == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-big", "--prefer-j2k-lossless", options.networkTransferSyntax_ == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-big", "--prefer-j2k-lossy", options.networkTransferSyntax_ == EXS_JPEG2000);
        app.checkConflict("--write-xfer-big", "--prefer-jls-lossless", options.networkTransferSyntax_ == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-big", "--prefer-jls-lossy", options.networkTransferSyntax_ == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg2", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg2-high", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg4", options.networkTransferSyntax_ == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg4-bd", options.networkTransferSyntax_ == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-big", "--prefer-rle", options.networkTransferSyntax_ == EXS_RLELossless);
        // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
#endif
        options.writeTransferSyntax_ = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-implicit"))
      {
        app.checkConflict("--write-xfer-implicit", "--bit-preserving", options.bitPreserving_);
#ifndef DISABLE_COMPRESSION_EXTENSION
        app.checkConflict("--write-xfer-implicit", "--prefer-lossless", options.networkTransferSyntax_ == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-implicit", "--prefer-jpeg8", options.networkTransferSyntax_ == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-implicit", "--prefer-jpeg12", options.networkTransferSyntax_ == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-implicit", "--prefer-j2k-lossless", options.networkTransferSyntax_ == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-implicit", "--prefer-j2k-lossy", options.networkTransferSyntax_ == EXS_JPEG2000);
        app.checkConflict("--write-xfer-implicit", "--prefer-jls-lossless", options.networkTransferSyntax_ == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-implicit", "--prefer-jls-lossy", options.networkTransferSyntax_ == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg2", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg2-high", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg4", options.networkTransferSyntax_ == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg4-bd", options.networkTransferSyntax_ == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-implicit", "--prefer-rle", options.networkTransferSyntax_ == EXS_RLELossless);
        // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
#endif
        options.writeTransferSyntax_ = EXS_LittleEndianImplicit;
      }
#ifdef WITH_ZLIB
      if (cmd.findOption("--write-xfer-deflated"))
      {
        app.checkConflict("--write-xfer-deflated", "--bit-preserving", options.bitPreserving_);
#ifndef DISABLE_COMPRESSION_EXTENSION
        app.checkConflict("--write-xfer-deflated", "--prefer-lossless", options.networkTransferSyntax_ == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-deflated", "--prefer-jpeg8", options.networkTransferSyntax_ == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-deflated", "--prefer-jpeg12", options.networkTransferSyntax_ == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-deflated", "--prefer-j2k-lossless", options.networkTransferSyntax_ == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-deflated", "--prefer-j2k-lossy", options.networkTransferSyntax_ == EXS_JPEG2000);
        app.checkConflict("--write-xfer-deflated", "--prefer-jls-lossless", options.networkTransferSyntax_ == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-deflated", "--prefer-jls-lossy", options.networkTransferSyntax_ == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg2", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg2-high", options.networkTransferSyntax_ == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg4", options.networkTransferSyntax_ == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg4-bd", options.networkTransferSyntax_ == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-deflated", "--prefer-rle", options.networkTransferSyntax_ == EXS_RLELossless);
#endif
        options.writeTransferSyntax_ = EXS_DeflatedLittleEndianExplicit;
      }
#endif
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
        app.checkConflict("--padding-create", "--write-dataset", !options.useMetaheader_);
        app.checkConflict("--padding-create", "--bit-preserving", options.bitPreserving_);
        app.checkValue(cmd.getValueAndCheckMin(options.filepad_, 0));
        app.checkValue(cmd.getValueAndCheckMin(options.itempad_, 0));
        options.paddingType_ = EPD_withPadding;
      }
      cmd.endOptionBlock();

#ifdef WITH_ZLIB
      if (cmd.findOption("--compression-level"))
      {
        OFCmdUnsignedInt compressionLevel = 0;
#ifdef DISABLE_COMPRESSION_EXTENSION
        app.checkDependence("--compression-level", "--write-xfer-deflated or --write-xfer-same",
          (options.writeTransferSyntax_ == EXS_DeflatedLittleEndianExplicit) ||
          (options.writeTransferSyntax_ == EXS_Unknown));
#else
        app.checkDependence("--compression-level", "--propose-deflated, --write-xfer-deflated or --write-xfer-same",
          (options.networkTransferSyntaxOut_ == EXS_DeflatedLittleEndianExplicit) ||
          (options.writeTransferSyntax_ == EXS_DeflatedLittleEndianExplicit) ||
          (options.writeTransferSyntax_ == EXS_Unknown));
#endif
        app.checkValue(cmd.getValueAndCheckMinMax(compressionLevel, 0, 9));
        dcmZlibCompressionLevel.set(OFstatic_cast(int, compressionLevel));
      }
#endif
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmqrscpLogger, rcsid << OFendl);

    /* read config file */
    if (access(opt_configFileName, R_OK) < 0) {
      char buf[256];
      OFLOG_FATAL(dcmqrscpLogger, "cannot access " << opt_configFileName << ": "
        << OFStandard::strerror(errno, buf, sizeof(buf)));
      return 10;
    }

    DcmQueryRetrieveConfig config;

    if (!config.init(opt_configFileName)) {
      OFLOG_FATAL(dcmqrscpLogger, "bad config file: " << opt_configFileName);
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
      OFLOG_FATAL(dcmqrscpLogger, "invalid MaxPDUSize in config file");
      return 10;
    }
    if (overrideMaxPDU > 0) options.maxPDU_ = overrideMaxPDU;

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
      OFLOG_WARN(dcmqrscpLogger, "no data dictionary loaded, check environment variable: "
        << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

#ifndef DISABLE_PORT_PERMISSION_CHECK
#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if (opt_port < 1024) {
      if (geteuid() != 0) {
        OFLOG_FATAL(dcmqrscpLogger, "cannot listen on port " << opt_port << ", insufficient privileges");
        return 10;
      }
    }
#endif
#endif

    cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, (int)opt_port, options.acse_timeout_, &options.net_);
    if (cond.bad()) {
      OFLOG_FATAL(dcmqrscpLogger, "cannot initialize network: " << DimseCondition::dump(temp_str, cond));
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

#if defined(HAVE_SETUID) && defined(HAVE_GRP_H) && defined(HAVE_PWD_H)
     OFStandard::OFGroup grp;
     OFStandard::OFPasswd pwd;
     const char *opt_UserName = NULL;
     const char *opt_GroupName = NULL;

     if (((opt_GroupName = config.getGroupName()) != NULL) && strlen(opt_GroupName) > 0)
     {
       if (!(grp = OFStandard::getGrNam(opt_GroupName)))
       {
         OFLOG_FATAL(dcmqrscpLogger, "bad group name " << opt_GroupName);
         return 10;
       }
       if (setgid(grp.gr_gid) == -1)
       {
         OFLOG_FATAL(dcmqrscpLogger, "setgid: Unable to set group id to group " << (unsigned)grp.gr_gid);
         return 10;
       }
     }
     if (((opt_UserName = config.getUserName()) != NULL) && strlen(opt_UserName) > 0)
     {
       if (!(pwd = OFStandard::getPwNam(opt_UserName)))
       {
         OFLOG_FATAL(dcmqrscpLogger, "bad user name " << opt_UserName);
         return 10;
       }
       if (setuid(pwd.pw_uid) == -1)
       {
         OFLOG_FATAL(dcmqrscpLogger, "setuid: Unable to set user id to user " << (unsigned)pwd.pw_uid);
         return 10;
       }
     }
#endif // defined(HAVE_SETUID) && defined(HAVE_GRP_H) && defined(HAVE_PWD_H)

#ifdef WITH_SQL_DATABASE
    // use SQL database
    DcmQueryRetrieveSQLDatabaseHandleFactory factory;
#else
    // use linear index database (index.dat)
    DcmQueryRetrieveIndexDatabaseHandleFactory factory(&config);
#endif

    DcmQueryRetrieveSCP scp(config, options, factory);
    scp.setDatabaseFlags(opt_checkFindIdentifier, opt_checkMoveIdentifier);

    /* loop waiting for associations */
    while (cond.good())
    {
      cond = scp.waitForAssociation(options.net_);
      if (!options.singleProcess_) scp.cleanChildren();  /* clean up any child processes */
    }

    cond = ASC_dropNetwork(&options.net_);
    if (cond.bad()) {
      OFLOG_FATAL(dcmqrscpLogger, "cannot drop network: " << DimseCondition::dump(temp_str, cond));
      return 10;
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

    return 0;
}
