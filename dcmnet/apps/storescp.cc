/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Module:  dcmnet
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Storage Service Class Provider (C-STORE operation)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-02-29 11:49:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/storescp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */                        

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dimse.h"
#include "diutil.h"
#include "dcfilefo.h"
#include "dcdebug.h"
#include "dcuid.h"
#include "dcdict.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "storescp"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define APPLICATIONTITLE "STORESCP"     /* our application entity title */

static CONDITION processCommands(T_ASC_Association *assoc);
static CONDITION acceptAssociation(T_ASC_Network *net);
static CONDITION echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID);
static CONDITION storeSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID);
	
OFCmdUnsignedInt  opt_port = 0;
OFBool            opt_refuseAssociation = OFFalse;
OFBool            opt_rejectWithoutImplementationUID = OFFalse;
OFCmdUnsignedInt  opt_sleepAfter = 0;
OFCmdUnsignedInt  opt_sleepDuring = 0;
OFCmdUnsignedInt  opt_maxPDU = ASC_DEFAULTMAXPDU;
OFBool            opt_useMetaheader = OFTrue;
E_TransferSyntax  opt_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;
E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;
E_EncodingType    opt_sequenceType = EET_ExplicitLength;
E_PaddingEncoding opt_paddingType = EPD_withoutPadding;
OFCmdUnsignedInt  opt_filepad = 0;
OFCmdUnsignedInt  opt_itempad = 0; 
OFBool            opt_verbose = OFFalse;
OFBool            opt_debug = OFFalse;
OFBool            opt_bitPreserving = OFFalse;
OFBool            opt_ignore = OFFalse;
OFBool            opt_abortDuringStore = OFFalse;
OFBool            opt_abortAfterStore = OFFalse;
const char *      opt_respondingaetitle = APPLICATIONTITLE;

#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{
    CONDITION cond;
    T_ASC_Network *net;
    
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
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM storage (C-STORE) SCP", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL+SHORTCOL+4);
  cmd.addParam("port", "tcp/ip port number to listen on");

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL+2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");

  cmd.addGroup("network options:");
    cmd.addSubGroup("preferred network transfer syntaxes:");
      cmd.addOption("--prefer-uncompr",         "+x=",       "prefer explicit VR local byte order (default)");
      cmd.addOption("--prefer-little",          "+xe",       "prefer explicit VR little endian TS");
      cmd.addOption("--prefer-big",             "+xb",       "prefer explicit VR big endian TS");
      cmd.addOption("--prefer-lossless",        "+xs",       "prefer default JPEG lossless TS");
      cmd.addOption("--prefer-jpeg8",           "+xy",       "prefer default JPEG lossy TS for 8 bit data");
      cmd.addOption("--prefer-jpeg12",          "+xx",       "prefer default JPEG lossy TS for 12 bit data");
      cmd.addOption("--prefer-rle",             "+xr",       "prefer RLE lossless TS");
      cmd.addOption("--implicit",               "+xi",       "accept implicit VR little endian TS only");

    cmd.addSubGroup("other network options:");
      OFString opt1 = "set my AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",                "-aet",   1, "aetitle: string", opt1.c_str());
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
      cmd.addOption("--refuse",                              "refuse association");
      cmd.addOption("--reject",                              "reject association if no implement. class UID");
      cmd.addOption("--ignore",                              "ignore store data, receive but do not store");
      cmd.addOption("--sleep-after",                      1, "[s]econds: integer", "sleep s seconds after store (default: 0)");
      cmd.addOption("--sleep-during",                     1, "[s]econds: integer", "sleep s seconds during store (default: 0)");
      cmd.addOption("--abort-after",                         "abort association after receipt of C-STORE-RQ\n(but before sending response)");
      cmd.addOption("--abort-during",                        "abort association during receipt of C-STORE-RQ");

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
    cmd.addSubGroup("post-1993 value representations (not with --bit-preserving):");
      cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");
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
      /* check for --help first */
      if (cmd.findOption("--help")) app.printUsage();

      app.checkParam(cmd.getParam(1, opt_port, 1, (OFCmdUnsignedInt)65535));

      if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
      if (cmd.findOption("--debug")) 
      {
      	opt_debug = OFTrue;
        DUL_Debug(OFTrue);
        DIMSE_debug(OFTrue);
      	SetDebugLevel(3);
      }
      cmd.beginOptionBlock();
      if (cmd.findOption("--prefer-uncompr"))  opt_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--prefer-little"))   opt_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--prefer-big"))      opt_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--prefer-lossless")) opt_networkTransferSyntax = EXS_JPEGProcess14SV1TransferSyntax;
      if (cmd.findOption("--prefer-jpeg8"))    opt_networkTransferSyntax = EXS_JPEGProcess1TransferSyntax;
      if (cmd.findOption("--prefer-jpeg12"))   opt_networkTransferSyntax = EXS_JPEGProcess2_4TransferSyntax;
      if (cmd.findOption("--prefer-rle"))      opt_networkTransferSyntax = EXS_RLELossless;
      if (cmd.findOption("--implicit"))        opt_networkTransferSyntax = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_respondingaetitle));
      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValue(opt_maxPDU, ASC_MINIMUMPDUSIZE, (OFCmdUnsignedInt)ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--refuse")) opt_refuseAssociation = OFTrue;
      if (cmd.findOption("--reject")) opt_rejectWithoutImplementationUID = OFTrue;
      if (cmd.findOption("--ignore")) opt_ignore = OFTrue;
      if (cmd.findOption("--sleep-after")) app.checkValue(cmd.getValue(opt_sleepAfter, 0));
      if (cmd.findOption("--sleep-during")) app.checkValue(cmd.getValue(opt_sleepDuring, 0));
      if (cmd.findOption("--abort-after")) opt_abortAfterStore = OFTrue;
      if (cmd.findOption("--abort-during")) opt_abortDuringStore = OFTrue;

      cmd.beginOptionBlock();
      if (cmd.findOption("--normal")) opt_bitPreserving = OFFalse;
      if (cmd.findOption("--bit-preserving")) opt_bitPreserving = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_useMetaheader = OFTrue;
      if (cmd.findOption("--write-dataset")) opt_useMetaheader = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-same")) opt_writeTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--write-xfer-little"))
      {
      	app.checkConflict("--write-xfer-little", "--bit-preserving", opt_bitPreserving);
      	app.checkConflict("--write-xfer-little", "--prefer-lossless", opt_networkTransferSyntax==EXS_JPEGProcess14SV1TransferSyntax);
      	app.checkConflict("--write-xfer-little", "--prefer-jpeg8", opt_networkTransferSyntax==EXS_JPEGProcess1TransferSyntax);
      	app.checkConflict("--write-xfer-little", "--prefer-jpeg12", opt_networkTransferSyntax==EXS_JPEGProcess2_4TransferSyntax);
      	app.checkConflict("--write-xfer-little", "--prefer-rle", opt_networkTransferSyntax==EXS_RLELossless);
        opt_writeTransferSyntax = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-big"))
      {
      	app.checkConflict("--write-xfer-big", "--bit-preserving", opt_bitPreserving);
      	app.checkConflict("--write-xfer-big", "--prefer-lossless", opt_networkTransferSyntax==EXS_JPEGProcess14SV1TransferSyntax);
      	app.checkConflict("--write-xfer-big", "--prefer-jpeg8", opt_networkTransferSyntax==EXS_JPEGProcess1TransferSyntax);
      	app.checkConflict("--write-xfer-big", "--prefer-jpeg12", opt_networkTransferSyntax==EXS_JPEGProcess2_4TransferSyntax);
      	app.checkConflict("--write-xfer-big", "--prefer-rle", opt_networkTransferSyntax==EXS_RLELossless);
        opt_writeTransferSyntax = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-implicit"))
      {
      	app.checkConflict("--write-xfer-implicit", "--bit-preserving", opt_bitPreserving);
      	app.checkConflict("--write-xfer-implicit", "--prefer-lossless", opt_networkTransferSyntax==EXS_JPEGProcess14SV1TransferSyntax);
      	app.checkConflict("--write-xfer-implicit", "--prefer-jpeg8", opt_networkTransferSyntax==EXS_JPEGProcess1TransferSyntax);
      	app.checkConflict("--write-xfer-implicit", "--prefer-jpeg12", opt_networkTransferSyntax==EXS_JPEGProcess2_4TransferSyntax);
      	app.checkConflict("--write-xfer-implicit", "--prefer-rle", opt_networkTransferSyntax==EXS_RLELossless);
        opt_writeTransferSyntax = EXS_LittleEndianImplicit;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr")) 
      {
      	app.checkConflict("--enable-new-vr", "--bit-preserving", opt_bitPreserving);
        dcmEnableUnknownVRGeneration = OFTrue;
        dcmEnableUnlimitedTextVRGeneration = OFTrue;
      }
      if (cmd.findOption("--disable-new-vr"))
      {
      	app.checkConflict("--disable-new-vr", "--bit-preserving", opt_bitPreserving);
        dcmEnableUnknownVRGeneration = OFFalse;
        dcmEnableUnlimitedTextVRGeneration = OFFalse;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-recalc")) 
      {
      	app.checkConflict("--group-length-recalc", "--bit-preserving", opt_bitPreserving);
      	opt_groupLength = EGL_recalcGL;
      }
      if (cmd.findOption("--group-length-create")) 
      {
      	app.checkConflict("--group-length-create", "--bit-preserving", opt_bitPreserving);
      	opt_groupLength = EGL_withGL;
      }
      if (cmd.findOption("--group-length-remove")) 
      {
      	app.checkConflict("--group-length-remove", "--bit-preserving", opt_bitPreserving);
      	opt_groupLength = EGL_withoutGL;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit")) 
      {
      	app.checkConflict("--length-explicit", "--bit-preserving", opt_bitPreserving);
      	opt_sequenceType = EET_ExplicitLength;
      }
      if (cmd.findOption("--length-undefined")) 
      {
      	app.checkConflict("--length-undefined", "--bit-preserving", opt_bitPreserving);
      	opt_sequenceType = EET_UndefinedLength;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--padding-off")) opt_paddingType = EPD_withoutPadding;
      if (cmd.findOption("--padding-create")) 
      {
      	app.checkConflict("--padding-create", "--write-dataset", ! opt_useMetaheader);
      	app.checkConflict("--padding-create", "--bit-preserving", opt_bitPreserving);
        app.checkValue(cmd.getValue(opt_filepad, 0));
        app.checkValue(cmd.getValue(opt_itempad, 0));
        opt_paddingType = EPD_withPadding;
      }
      cmd.endOptionBlock();
      
   }

#ifdef HAVE_GETEUID
  /* if port is privileged we must be as well */
  if (opt_port < 1024)
  {
    if (geteuid() != 0)
    {
	  fprintf(stderr, "storescp: cannot listen on port %lu, insufficient privileges\n", opt_port);
      return 1;
    }
  }
#endif

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
	  DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  cond = ASC_initializeNetwork(NET_ACCEPTOR, (int)opt_port, 1000, &net);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    return 1;
  }

#ifdef HAVE_GETUID
  /* return to normal uid so that we can't do too much damage in case
   * things go very wrong.   Only does someting if the program is setuid 
   * root, and run by another user.  Running as root user may be
   * potentially disasterous if this program screws up badly.
   */
  setuid(getuid());
#endif

  while (SUCCESS(cond)) cond = acceptAssociation(net);

  cond = ASC_dropNetwork(&net);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    return 1;
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  return 0;
}

static CONDITION 
acceptAssociation(T_ASC_Network * net)
{
  CONDITION cond;
  char buf[BUFSIZ];
  T_ASC_Association *assoc;

  const char* knownAbstractSyntaxes[] =
  {
    UID_VerificationSOPClass
  };
  
  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
  int numTransferSyntaxes = 0;
  
  cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU);
  if (!SUCCESS(cond))
  {
    if (opt_verbose) COND_DumpConditions();
    goto cleanup;
  }
  if (opt_verbose) printf("Association Received\n");

  if (opt_debug)
  {
    printf("Parameters:\n");
    ASC_dumpParameters(assoc->params);
  }

  if (opt_refuseAssociation)
  {
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    if (opt_verbose) printf("Refusing Association (forced via command line)\n");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond != ASC_NORMAL)
    {
      printf("Association Reject Failed:\n");
      COND_DumpConditions();
    }
    goto cleanup;
  }

  switch (opt_networkTransferSyntax)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_JPEGProcess14SV1TransferSyntax:
      /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
      transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess1TransferSyntax:
      /* we prefer JPEGBaseline (default lossy for 8 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess2_4TransferSyntax:
      /* we prefer JPEGExtended (default lossy for 12 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_RLELossless:
      /* we prefer RLE Lossless */
      transferSyntaxes[0] = UID_RLELossless;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    default:
      /* We prefer explicit transfer syntaxes. 
       * If we are running on a Little Endian machine we prefer 
       * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
       */
      if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
      {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      } else {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      }
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
  }

  /* accept the Verification SOP Class if presented */
  cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
    assoc->params, 
    knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
    transferSyntaxes, numTransferSyntaxes);
  if (!SUCCESS(cond))
  {
    if (opt_verbose) COND_DumpConditions();
    goto cleanup;
  }

  /* the array of Storage SOP Class UIDs comes from dcuid.h */
  cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
    assoc->params, 
    dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs,
    transferSyntaxes, numTransferSyntaxes);
  if (!SUCCESS(cond))
  {
    if (opt_verbose) COND_DumpConditions();
    goto cleanup;
  }

  /* set our app title */
  ASC_setAPTitles(assoc->params, NULL, NULL, opt_respondingaetitle);

  /* acknowledge or reject this association */
  if ((cond = ASC_getApplicationContextName(assoc->params, buf) != 
    ASC_NORMAL) || strcmp(buf, UID_StandardApplicationContext) != 0)
  {
    /* reject: the application context name is not supported */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
    };

    if (opt_verbose) printf("Association Rejected: bad application context name: %s\n", buf);
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond != ASC_NORMAL)
    {
      if (opt_verbose) COND_DumpConditions();
    }
    goto cleanup;

  } else if (opt_rejectWithoutImplementationUID && 
      strlen(assoc->params->theirImplementationClassUID) == 0)
  {
    /* reject: the no implementation Class UID provided */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    if (opt_verbose) printf("Association Rejected: No Implementation Class UID provided\n");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond != ASC_NORMAL)
    {
      if (opt_verbose) COND_DumpConditions();
    }
    goto cleanup;
  } else {
    cond = ASC_acknowledgeAssociation(assoc);
    if (cond != ASC_NORMAL)
    {
      COND_DumpConditions();
      goto cleanup;
    }
    if (opt_verbose)
    {
      printf("Association Acknowledged (Max Send PDV: %lu)\n", assoc->sendPDVLength);
      if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
        printf("    (but no valid presentation contexts)\n");
      if (opt_debug) ASC_dumpParameters(assoc->params);
    }
  }

  /* now do the real work */
  cond = processCommands(assoc);

  if (cond == DIMSE_PEERREQUESTEDRELEASE)
  {
    COND_PopCondition(OFFalse); 
      /* pop only the peer requested
       * release condition from the stack */
    if (opt_verbose) printf("Association Release\n");
    cond = ASC_acknowledgeRelease(assoc);
  } 
  else if (cond == DIMSE_PEERABORTEDASSOCIATION)
  {
    COND_PopCondition(OFFalse);  /* pop DIMSE abort */
    COND_PopCondition(OFFalse);  /* pop DUL abort */
    if (opt_verbose) printf("Association Aborted\n");
  } else {
    fprintf(stderr, "storescp: DIMSE Failure (aborting association)\n");
    COND_PopCondition(OFTrue); // pop all conditions
    /* some kind of error so abort the association */
    cond = ASC_abortAssociation(assoc);
  }

cleanup:
  cond = ASC_dropSCPAssociation(assoc);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    exit(1);
  }
  cond = ASC_destroyAssociation(&assoc);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    exit(1);
  }

  if (opt_verbose)
  {
    /* are there any conditions sitting on the condition stack? */
    char buf2[BUFSIZ];
    CONDITION c;

    if (COND_TopCondition(&c, buf2, BUFSIZ) != COND_NORMAL)
    {
      fprintf(stderr, "CONDITIONS Remaining\n");
      COND_DumpConditions();
      COND_PopCondition(OFTrue); // pop all conditions
    }
  }
  return cond;
}


static CONDITION
processCommands(T_ASC_Association * assoc)
{
  CONDITION cond = DIMSE_NORMAL;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID = 0;
  DcmDataset *statusDetail = NULL;

  while (cond == DIMSE_NORMAL)
  {
    cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID,
        &msg, &statusDetail);
    if (statusDetail != NULL)
    {
      printf("Extra Status Detail: \n");
      statusDetail->print();
      delete statusDetail;
    }

    /* did peer release, abort, or do we have a valid message ? */
    switch (cond)
    {
      case DIMSE_NORMAL:
        /* process command */
        switch (msg.CommandField)
        {
          case DIMSE_C_ECHO_RQ:
            cond = echoSCP(assoc, &msg, presID);
            break;
          case DIMSE_C_STORE_RQ:
            cond = storeSCP(assoc, &msg, presID);
            break;
          default:
            /* we cannot handle this kind of message */
            cond = DIMSE_BADCOMMANDTYPE;
            fprintf(stderr, "storescp: Cannot handle command: 0x%x\n", (unsigned)msg.CommandField);
            break;
        }
        break;
      case DIMSE_PEERREQUESTEDRELEASE:
      case DIMSE_PEERABORTEDASSOCIATION:
        /* association gone */
        break;
      default:
        break;
    }
  }
  return cond;
}


static CONDITION echoSCP(
  T_ASC_Association * assoc,
  T_DIMSE_Message * msg,
  T_ASC_PresentationContextID presID)
{
  CONDITION cond;

  if (opt_verbose)
  {
    printf("Received ");
    DIMSE_printCEchoRQ(stdout, &msg->msg.CEchoRQ);
  }

  /* the echo succeeded !! */
  cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);
  if (!SUCCESS(cond))
  {
    fprintf(stderr, "storescp: Echo SCP Failed:\n");
    COND_DumpConditions();
  }
  return cond;
}

struct StoreCallbackData
{
  char* imageFileName;
  DcmFileFormat* dcmff;
  T_ASC_Association* assoc;
};

static void 
storeSCPCallback(
    /* in */
    void *callbackData, 
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName, DcmDataset **imageDataSet, /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if ((opt_abortDuringStore > 0 && progress->state != DIMSE_StoreBegin) ||
        (opt_abortAfterStore > 0 && progress->state == DIMSE_StoreEnd)) {
        if (opt_verbose) {
            printf("ABORT initiated (due to command line options)\n");
        }
        ASC_abortAssociation(((StoreCallbackData*) callbackData)->assoc);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
    }

    if (opt_sleepDuring > 0)
    {
      sleep((unsigned int)opt_sleepDuring);
    }
    
    if (opt_verbose)
    {
      switch (progress->state)
      {
        case DIMSE_StoreBegin:    
          printf("RECV:");
          break;
        case DIMSE_StoreEnd:
          printf("\n");
          break;
        default:
          putchar('.');
          break;
      }
      fflush(stdout);
    }
    
    if (progress->state == DIMSE_StoreEnd)
    {
       *statusDetail = NULL;    /* no status detail */

       /* could save the image somewhere else, put it in database, etc */
       /* 
        * An appropriate status code is already set in the resp structure, it need not be success.
        * For example, if the caller has already detected an out of resources problem then the
        * status will reflect this.  The callback function is still called to allow cleanup.
        */
       // rsp->DimseStatus = STATUS_Success;

       if ((imageDataSet)&&(*imageDataSet)&&(!opt_bitPreserving)&&(!opt_ignore))
       {
         StoreCallbackData *cbdata = (StoreCallbackData*) callbackData;
         const char* fileName = cbdata->imageFileName;

         DcmFileStream outf(fileName, DCM_WriteMode);
         if (outf.Fail())
         {
           fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
           rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
         } else {
           E_TransferSyntax xfer = opt_writeTransferSyntax;
           if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

           if (opt_useMetaheader)
           {
             /* write as fileformat */
             cbdata->dcmff->transferInit();
             cbdata->dcmff->write(outf, xfer, opt_sequenceType, opt_groupLength,
               opt_paddingType, (Uint32)opt_filepad, (Uint32)opt_itempad);
             cbdata->dcmff->transferEnd();
             if (cbdata->dcmff->error() != EC_Normal)
             {
               fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
               rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
             }
           } else {
             /* write as dataset */
             (*imageDataSet)->transferInit();
             (*imageDataSet)->write(outf, xfer, opt_sequenceType, opt_groupLength,
               opt_paddingType, (Uint32)opt_filepad, (Uint32)opt_itempad);
             (*imageDataSet)->transferEnd();
             if ((*imageDataSet)->error() != EC_Normal)
             {
               fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
               rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
             }
           }
         }
      
        /* should really check the image to make sure it is consistent,
         * that its sopClass and sopInstance correspond with those in
         * the request.
         */
        if ((rsp->DimseStatus == STATUS_Success)&&(!opt_ignore))
        {
          /* which SOP class and SOP instance ? */
          if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance))
          {
             fprintf(stderr, "storescp: Bad image file: %s\n", imageFileName);
             rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
          }
          else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
          {
            rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
          else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
          {
            rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
        }
      }
    }
    return;
}

static CONDITION storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_Message *msg,
  T_ASC_PresentationContextID presID)
{
    CONDITION cond;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[2048];

    req = &msg->msg.CStoreRQ;
    if (opt_ignore)
    {
#ifdef _WIN32
        tmpnam(imageFileName);
#else
        strcpy(imageFileName, "/dev/null");
#endif
    } else {
      sprintf(imageFileName, "%s.%s", 
        dcmSOPClassUIDToModality(req->AffectedSOPClassUID),
        req->AffectedSOPInstanceUID);
    }

    if (opt_verbose)
    {
      printf("Received ");
      DIMSE_printCStoreRQ(stdout, req);
    }

    StoreCallbackData callbackData;
    callbackData.assoc = assoc;
    callbackData.imageFileName = imageFileName;
    DcmFileFormat dcmff;
    callbackData.dcmff = &dcmff;

    DcmDataset *dset = dcmff.getDataset();

    if (opt_bitPreserving)
    {
      cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader,
        NULL, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
    } else {
      cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL, opt_useMetaheader,
        &dset, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
    }

    if (!SUCCESS(cond))
    {
      fprintf(stderr, "storescp: Store SCP Failed:\n");
      COND_DumpConditions();
      /* remove file */
      if (!opt_ignore)
      {
        unlink(imageFileName);
      }
#ifdef _WIN32
    } else if (opt_ignore) {
        unlink(imageFileName); // delete the temporary file
#endif
    }

    if (opt_sleepAfter > 0)
    {
      sleep((unsigned int)opt_sleepAfter);
    }
    return cond;
}


/*
** CVS Log
** $Log: storescp.cc,v $
** Revision 1.28  2000-02-29 11:49:50  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.27  2000/02/23 15:12:21  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.26  2000/02/03 11:50:09  meichel
** Moved UID related functions from dcmnet (diutil.h) to dcmdata (dcuid.h)
**   where they belong. Renamed access functions to dcmSOPClassUIDToModality
**   and dcmGuessModalityBytes.
**
** Revision 1.25  2000/02/01 10:24:03  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.24  1999/10/07 17:26:19  meichel
** Corrected typo in storescup error message
**
** Revision 1.23  1999/04/29 10:02:54  meichel
** Adapted findscu to new command line option scheme
**
** Revision 1.22  1999/04/28 08:29:07  meichel
** Adapted storescp to new command line option scheme.
**   Added support for transmission of compressed images.
**
** Revision 1.21  1999/04/27 17:24:40  meichel
** Adapted storescp to new command line option scheme.
**   Added support for transmission of compressed images.
**
** Revision 1.20  1999/03/29 11:19:55  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.19  1998/08/10 08:53:36  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.18  1998/02/06 15:07:30  meichel
** Removed many minor problems (name clashes, unreached code)
**   reported by Sun CC4 with "+w" or Sun CC2.
**
** Revision 1.17  1998/01/14 14:35:55  hewett
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.16  1997/09/12 13:21:50  meichel
** Command line option '-h' in storescp now works correctly.
**
** Revision 1.15  1997/08/05 07:36:21  andreas
** - Corrected error in DUL finite state machine
**   SCPs shall close sockets after the SCU have closed the socket in
**   a normal association release. Therfore, an ARTIM timer is described
**   in DICOM part 8 that is not implemented correctly in the
**   DUL. Since the whole DUL finite state machine is affected, we
**   decided to solve the proble outside the fsm. Now it is necessary to call the
**   ASC_DropSCPAssociation() after the calling ASC_acknowledgeRelease().
** - Change needed version number of WINSOCK to 1.1
**   to support WINDOWS 95
**
** Revision 1.14  1997/07/24 13:10:55  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.13  1997/07/21 08:37:04  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.12  1997/05/29 15:52:57  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.11  1997/05/27 15:44:53  meichel
** Corrected typo in storescp help texts.
**
** Revision 1.10  1997/05/23 10:44:20  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes to interfaces of some DIMSE functions.
**
** Revision 1.9  1997/05/22 13:29:59  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.8  1997/05/16 08:31:34  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.7  1997/03/27 16:11:27  hewett
** Added command line switches allowing generation of UN to
** be disabled (it is enabled by default).
**
** Revision 1.6  1996/12/16 15:14:00  hewett
** Added bugfix for WINSOCK support.  The required WINSOCK version
** number was being incorrectly set to version 0.1.  The fixed
** WINSOCK initialisation now uses the MAKEWORD macro to correctly
** set the required version number. This bugfix was contributed
** by Dr. Yongjian Bao of Innomed GmbH, Germany.
**
** Revision 1.5  1996/09/27 14:05:05  hewett
** Added calls to initialise WINSOCK library for Win32 environment.  Only
** compiled in if HAVE_WINSOCK_H
**
** Revision 1.4  1996/09/27 08:24:30  hewett
** System header files now enclosed with BEGIN_EXTERN_C/END_EXTERN_C
**
** Revision 1.3  1996/09/24 16:20:32  hewett
** Now uses global table of Storage SOP Class UIDs (from dcuid.h).
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.2  1996/09/03 11:39:03  hewett
** Added copyright information.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
**
*/
