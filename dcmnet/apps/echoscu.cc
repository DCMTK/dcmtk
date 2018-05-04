/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Verification Service Class User (C-ECHO operation)
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcmtrans.h"    /* for dcmSocketSend/ReceiveTimeout */
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmtls/tlsopt.h"      /* for DcmTLSOptions */

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#ifdef PRIVATE_ECHOSCU_DECLARATIONS
PRIVATE_ECHOSCU_DECLARATIONS
#else
#define OFFIS_CONSOLE_APPLICATION "echoscu"
#endif

static OFLogger echoscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE     "ECHOSCU"
#define PEERAPPLICATIONTITLE "ANY-SCP"

static T_DIMSE_BlockingMode opt_blockMode = DIMSE_BLOCKING;
static int opt_dimse_timeout = 0;

static OFCondition cecho(T_ASC_Association * assoc, unsigned long num_repeat);

/* DICOM standard transfer syntaxes */
static const char* transferSyntaxes[] = {
      UID_LittleEndianImplicitTransferSyntax, /* default xfer syntax first */
      UID_LittleEndianExplicitTransferSyntax,
      UID_BigEndianExplicitTransferSyntax,
      UID_JPEGProcess1TransferSyntax,
      UID_JPEGProcess2_4TransferSyntax,
      UID_JPEGProcess3_5TransferSyntax,
      UID_JPEGProcess6_8TransferSyntax,
      UID_JPEGProcess7_9TransferSyntax,
      UID_JPEGProcess10_12TransferSyntax,
      UID_JPEGProcess11_13TransferSyntax,
      UID_JPEGProcess14TransferSyntax,
      UID_JPEGProcess15TransferSyntax,
      UID_JPEGProcess16_18TransferSyntax,
      UID_JPEGProcess17_19TransferSyntax,
      UID_JPEGProcess20_22TransferSyntax,
      UID_JPEGProcess21_23TransferSyntax,
      UID_JPEGProcess24_26TransferSyntax,
      UID_JPEGProcess25_27TransferSyntax,
      UID_JPEGProcess28TransferSyntax,
      UID_JPEGProcess29TransferSyntax,
      UID_JPEGProcess14SV1TransferSyntax,
      UID_RLELosslessTransferSyntax,
      UID_DeflatedExplicitVRLittleEndianTransferSyntax,
      UID_JPEGLSLosslessTransferSyntax,
      UID_JPEGLSLossyTransferSyntax,
      UID_JPEG2000LosslessOnlyTransferSyntax,
      UID_JPEG2000TransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,
      UID_MPEG2MainProfileAtMainLevelTransferSyntax,
      UID_MPEG2MainProfileAtHighLevelTransferSyntax,
      UID_MPEG4HighProfileLevel4_1TransferSyntax,
      UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,
      UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,
      UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,
      UID_MPEG4StereoHighProfileLevel4_2TransferSyntax,
      UID_HEVCMainProfileLevel5_1TransferSyntax,
      UID_HEVCMain10ProfileLevel5_1TransferSyntax
};

// ********************************************

/* helper macro for converting stream output to a string */
#define CONVERT_TO_STRING(output, string) \
    optStream.str(""); \
    optStream.clear(); \
    optStream << output << OFStringStream_ends; \
    OFSTRINGSTREAM_GETOFSTRING(optStream, string)

#define SHORTCOL 4
#define LONGCOL 19

int
main(int argc, char *argv[])
{
  OFOStringStream optStream;

  const char *     opt_peer                = NULL;
  OFCmdUnsignedInt opt_port                = 104;
  const char *     opt_peerTitle           = PEERAPPLICATIONTITLE;
  const char *     opt_ourTitle            = APPLICATIONTITLE;
  OFCmdUnsignedInt opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
  OFCmdUnsignedInt opt_repeatCount         = 1;
  OFBool           opt_abortAssociation    = OFFalse;
  OFCmdUnsignedInt opt_numXferSyntaxes     = 1;
  OFCmdUnsignedInt opt_numPresentationCtx  = 1;
  OFCmdUnsignedInt maxXferSyntaxes         = OFstatic_cast(OFCmdUnsignedInt, (DIM_OF(transferSyntaxes)));
  int              opt_acse_timeout        = 30;
  OFCmdSignedInt   opt_socket_timeout      = 60;
  DcmTLSOptions    tlsOptions(NET_REQUESTOR);

  T_ASC_Network *net;
  T_ASC_Parameters *params;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc;
  OFString temp_str;

  OFStandard::initializeNetwork();
#ifdef WITH_OPENSSL
  DcmTLSTransportLayer::initializeOpenSSL();
#endif

  char tempstr[20];
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM verification (C-ECHO) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",              "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                      "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

  cmd.addGroup("network options:");
    cmd.addSubGroup("application entity titles:");
      cmd.addOption("--aetitle",        "-aet", 1, "[a]etitle: string", "set my calling AE title (default: " APPLICATIONTITLE ")");
      cmd.addOption("--call",           "-aec", 1, "[a]etitle: string", "set called AE title of peer (default: " PEERAPPLICATIONTITLE ")");
    cmd.addSubGroup("association negotiation debugging:");
      OFString opt5 = "[n]umber: integer (1..";
      sprintf(tempstr, "%ld", OFstatic_cast(long, maxXferSyntaxes));
      opt5 += tempstr;
      opt5 += ")";
      cmd.addOption("--propose-ts",     "-pts", 1, opt5.c_str(), "propose n transfer syntaxes");
      cmd.addOption("--propose-pc",     "-ppc", 1, "[n]umber: integer (1..128)", "propose n presentation contexts");

    cmd.addSubGroup("other network options:");
      cmd.addOption("--timeout",        "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      CONVERT_TO_STRING("[s]econds: integer (default: " << opt_socket_timeout << ")", optString1);
      cmd.addOption("--socket-timeout", "-ts",  1, optString1.c_str(), "timeout for network socket (0 for none)");
      CONVERT_TO_STRING("[s]econds: integer (default: " << opt_acse_timeout << ")", optString2);
      cmd.addOption("--acse-timeout",   "-ta",  1, optString2.c_str(), "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",  "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");

      CONVERT_TO_STRING("[n]umber of bytes: integer (" << ASC_MINIMUMPDUSIZE << ".." << ASC_MAXIMUMPDUSIZE << ")", optString3);
      CONVERT_TO_STRING("set max receive pdu to n bytes (default: " << opt_maxReceivePDULength << ")", optString4);
      cmd.addOption("--max-pdu",        "-pdu", 1, optString3.c_str(), optString4.c_str());
      cmd.addOption("--repeat",                 1, "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                     "abort association instead of releasing it");

    // add TLS specific command line options if (and only if) we are compiling with OpenSSL
    tlsOptions.addTLSCommandlineOptions(cmd);

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
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
          COUT << " none" << OFendl;
#else
          COUT << OFendl;
#endif
#ifdef WITH_ZLIB
          COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
          // print OpenSSL version if (and only if) we are compiling with OpenSSL
          tlsOptions.printLibraryVersion();
          return 0;
        }

        // check if the command line contains the --list-ciphers option
        if (tlsOptions.listOfCiphersRequested(cmd))
        {
            tlsOptions.printSupportedCiphersuites(app, COUT);
            return 0;
        }
      }

      /* command line parameters */

      cmd.getParam(1, opt_peer);
      app.checkParam(cmd.getParamAndCheckMinMax(2, opt_port, 1, 65535));

      OFLog::configureFromCommandLine(cmd, app);

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

      if (cmd.findOption("--timeout"))
      {
        OFCmdSignedInt opt_timeout = 0;
        app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
        dcmConnectionTimeout.set(OFstatic_cast(Sint32, opt_timeout));
      }

      if (cmd.findOption("--socket-timeout"))
        app.checkValue(cmd.getValueAndCheckMin(opt_socket_timeout, -1));
      // always set the timeout values since the global default might be different
      dcmSocketSendTimeout.set(OFstatic_cast(Sint32, opt_socket_timeout));
      dcmSocketReceiveTimeout.set(OFstatic_cast(Sint32, opt_socket_timeout));

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
        opt_dimse_timeout = OFstatic_cast(int, opt_timeout);
        opt_blockMode = DIMSE_NONBLOCKING;
      }

      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(opt_maxReceivePDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--repeat")) app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
      if (cmd.findOption("--abort")) opt_abortAssociation=OFTrue;
      if (cmd.findOption("--propose-ts")) app.checkValue(cmd.getValueAndCheckMinMax(opt_numXferSyntaxes, 1, maxXferSyntaxes));
      if (cmd.findOption("--propose-pc")) app.checkValue(cmd.getValueAndCheckMinMax(opt_numPresentationCtx, 1, 128));

      // evaluate (most of) the TLS command line options (if we are compiling with OpenSSL)
      tlsOptions.parseArguments(app, cmd);

    }

    /* print resource identifier */
    OFLOG_DEBUG(echoscuLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(echoscuLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* initialize network, i.e. create an instance of T_ASC_Network*. */
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, opt_acse_timeout, &net);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

    /* initialize association parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

    /* create a secure transport layer if requested and OpenSSL is available */
    cond = tlsOptions.createTransportLayer(net, params, app, cmd);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

#ifdef PRIVATE_ECHOSCU_CODE
    PRIVATE_ECHOSCU_CODE
#endif

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    sprintf(peerHost, "%s:%d", opt_peer, OFstatic_cast(int, opt_port));
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    int presentationContextID = 1; /* odd byte value 1, 3, 5, .. 255 */
    for (unsigned long ii=0; ii<opt_numPresentationCtx; ii++)
    {
        cond = ASC_addPresentationContext(params, presentationContextID, UID_VerificationSOPClass,
                 transferSyntaxes, OFstatic_cast(int, opt_numXferSyntaxes));
        presentationContextID += 2;
        if (cond.bad())
        {
            OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }

    /* dump presentation contexts if required */
    OFLOG_DEBUG(echoscuLogger, "Request Parameters:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ));

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    OFLOG_INFO(echoscuLogger, "Requesting Association");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED)
        {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            OFLOG_FATAL(echoscuLogger, "Association Rejected:" << OFendl << ASC_printRejectParameters(temp_str, &rej));
            exit(1);
        } else {
            OFLOG_FATAL(echoscuLogger, "Association Request Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    OFLOG_DEBUG(echoscuLogger, "Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        OFLOG_FATAL(echoscuLogger, "No Acceptable Presentation Contexts");
        exit(1);
    }

    /* dump general information concerning the establishment of the network connection if required */
    OFLOG_INFO(echoscuLogger, "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")");

    /* do the real work, i.e. send a number of C-ECHO-RQ messages to the DICOM application */
    /* this application is connected with and handle corresponding C-ECHO-RSP messages. */
    cond = cecho(assoc, opt_repeatCount);

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (opt_abortAssociation) {
            OFLOG_INFO(echoscuLogger, "Aborting Association");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad())
            {
                OFLOG_FATAL(echoscuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
                exit(1);
            }
        } else {
            /* release association */
            OFLOG_INFO(echoscuLogger, "Releasing Association");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                OFLOG_FATAL(echoscuLogger, "Association Release Failed: " << DimseCondition::dump(temp_str, cond));
                exit(1);
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        OFLOG_FATAL(echoscuLogger, "Protocol Error: Peer requested release (Aborting)");
        OFLOG_INFO(echoscuLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            OFLOG_FATAL(echoscuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        OFLOG_INFO(echoscuLogger, "Peer Aborted Association");
    }
    else
    {
        OFLOG_ERROR(echoscuLogger, "Echo SCU Failed: " << DimseCondition::dump(temp_str, cond));
        OFLOG_INFO(echoscuLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            OFLOG_FATAL(echoscuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

    OFStandard::shutdownNetwork();

    cond = tlsOptions.writeRandomSeed();
    if (cond.bad()) {
        // failure to write back the random seed is a warning, not an error
        OFLOG_WARN(echoscuLogger, DimseCondition::dump(temp_str, cond));
    }

    return 0;
}

static OFCondition
echoSCU(T_ASC_Association * assoc)
    /*
     * This function will send a C-ECHO-RQ over the network to another DICOM application
     * and handle the response.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     */
{
    DIC_US msgId = assoc->nextMsgID++;
    DIC_US status;
    DcmDataset *statusDetail = NULL;

    /* dump information if required */
    OFLOG_INFO(echoscuLogger, "Sending Echo Request (MsgID " << msgId << ")");

    /* send C-ECHO-RQ and handle response */
    OFCondition cond = DIMSE_echoUser(assoc, msgId, opt_blockMode, opt_dimse_timeout, &status, &statusDetail);

    /* depending on if a response was received, dump some information */
    if (cond.good()) {
        OFLOG_INFO(echoscuLogger, "Received Echo Response (" << DU_cechoStatusString(status) << ")");
    } else {
        OFString temp_str;
        OFLOG_ERROR(echoscuLogger, "Echo Failed: " << DimseCondition::dump(temp_str, cond));
    }

    /* check for status detail information, there should never be any */
    if (statusDetail != NULL) {
        OFLOG_DEBUG(echoscuLogger, "Status Detail (should never be any):" << OFendl << DcmObject::PrintHelper(*statusDetail));
        delete statusDetail;
    }

    /* return result value */
    return cond;
}

static OFCondition
cecho(T_ASC_Association * assoc, unsigned long num_repeat)
    /*
     * This function will send num_repeat C-ECHO-RQ messages to the DICOM application
     * this application is connected with and handle corresponding C-ECHO-RSP messages.
     *
     * Parameters:
     *   assoc      - [in] The association (network connection to another DICOM application).
     *   num_repeat - [in] The amount of C-ECHO-RQ messages which shall be sent.
     */
{
    OFCondition cond = EC_Normal;
    unsigned long n = num_repeat;

    /* as long as no error occurred and the counter does not equal 0 */
    /* send an C-ECHO-RQ and handle the response */
    while (cond.good() && n--) cond = echoSCU(assoc);

    return cond;
}
