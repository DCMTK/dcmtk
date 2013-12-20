/*
 *
 *  Copyright (C) 2011-2013, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Simple Storage Service Class User
 *
 */


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"       /* for OFStandard functions */
#include "dcmtk/ofstd/ofconapp.h"    /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofstream.h"    /* for OFStringStream et al. */
#include "dcmtk/dcmdata/dcdict.h"    /* for global data dictionary */
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmtk version name */
#include "dcmtk/dcmdata/cmdlnarg.h"  /* for prepareCmdLineArgs */
#include "dcmtk/dcmdata/dcostrmz.h"  /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmnet/dstorscu.h"   /* for DcmStorageSCU */

#include "dcmtk/dcmjpeg/djdecode.h"  /* for JPEG decoders */
#include "dcmtk/dcmjpls/djdecode.h"  /* for JPEG-LS decoders */
#include "dcmtk/dcmdata/dcrledrg.h"  /* for RLE decoder */
#include "dcmtk/dcmjpeg/dipijpeg.h"  /* for dcmimage JPEG plugin */


#ifdef WITH_ZLIB
#include <zlib.h>                    /* for zlibVersion() */
#endif

#if defined (HAVE_WINDOWS_H) || defined(HAVE_FNMATCH_H)
#define PATTERN_MATCHING_AVAILABLE
#endif


/* general definitions */

#define OFFIS_CONSOLE_APPLICATION "dcmsend"

static OFLogger dcmsendLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application entity titles */
#define APPLICATIONTITLE     "DCMSEND"
#define PEERAPPLICATIONTITLE "ANY-SCP"


/* exit codes for this command line tool */
/* (EXIT_SUCCESS and EXIT_FAILURE are standard codes) */

// general
#define EXITCODE_NO_ERROR                         0
#define EXITCODE_COMMANDLINE_SYNTAX_ERROR         1      // this code is the default for printError()

// input file errors
#define EXITCODE_CANNOT_READ_INPUT_FILE          20      // placeholder, currently not used
#define EXITCODE_NO_INPUT_FILES                  21
#define EXITCODE_INVALID_INPUT_FILE              22
#define EXITCODE_NO_VALID_INPUT_FILES            23

// output file errors
#define EXITCODE_CANNOT_WRITE_OUTPUT_FILE        40      // placeholder, currently not used
#define EXITCODE_CANNOT_WRITE_REPORT_FILE        43

// network errors
#define EXITCODE_CANNOT_INITIALIZE_NETWORK       60
#define EXITCODE_CANNOT_NEGOTIATE_ASSOCIATION    61
#define EXITCODE_CANNOT_SEND_REQUEST             62
#define EXITCODE_CANNOT_ADD_PRESENTATION_CONTEXT 65


/* helper macro for converting stream output to a string */
#define CONVERT_TO_STRING(output, string) \
    optStream.str(""); \
    optStream.clear(); \
    optStream << output << OFStringStream_ends; \
    OFSTRINGSTREAM_GETOFSTRING(optStream, string)


/* static helper functions */

// make sure that everything is cleaned up properly
static void cleanup()
{
    // deregister JPEG decoder
    DJDecoderRegistration::cleanup();
    // deregister JPEG-LS decoder
    DJLSDecoderRegistration::cleanup();
    // deregister RLE decoder
    DcmRLEDecoderRegistration::cleanup();
#ifdef DEBUG
    /* useful for debugging with dmalloc */
    dcmDataDict.clear();
#endif
}


/* main program */

#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{
    OFOStringStream optStream;

    const char *opt_peer = NULL;
    const char *opt_peerTitle = PEERAPPLICATIONTITLE;
    const char *opt_ourTitle = APPLICATIONTITLE;

    E_FileReadMode opt_readMode = ERM_fileOnly;

    OFCmdUnsignedInt opt_port = 0;
    OFCmdUnsignedInt opt_timeout = 0;
    OFCmdUnsignedInt opt_dimseTimeout = 0;
    OFCmdUnsignedInt opt_acseTimeout = 30;
    OFCmdUnsignedInt opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
    OFCmdUnsignedInt opt_maxSendPDULength = 0;
    T_DIMSE_BlockingMode opt_blockMode = DIMSE_BLOCKING;
#ifdef WITH_ZLIB
    OFCmdUnsignedInt opt_compressionLevel = 0;
#endif

    OFBool opt_showPresentationContexts = OFFalse;
    OFBool opt_haltOnInvalidFile = OFTrue;
    OFBool opt_haltOnUnsuccessfulStore = OFTrue;
    OFBool opt_allowIllegalProposal = OFTrue;
    OFBool opt_checkUIDValues = OFTrue;
    OFBool opt_multipleAssociations = OFTrue;
    DcmStorageSCU::E_DecompressionMode opt_decompressionMode = DcmStorageSCU::DM_losslessOnly;

    OFBool opt_dicomDir = OFFalse;
    OFBool opt_scanDir = OFFalse;
    OFBool opt_recurse = OFFalse;
    const char *opt_scanPattern = "";
    const char *opt_reportFilename = "";

    int paramCount = 0;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Simple DICOM storage SCU (sender)", rcsid);
    OFCommandLine cmd;

    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
    cmd.addParam("peer", "hostname of DICOM peer");
    cmd.addParam("port", "tcp/ip port number of peer");
    cmd.addParam("dcmfile-in", "DICOM file or directory to be transmitted", OFCmdParam::PM_MultiMandatory);

    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--list-decoders",                    "list transfer syntaxes of decoders and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);
      cmd.addOption("--verbose-pc",            "+v",      "show presentation contexts in verbose mode");

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",           "+f",      "read file format or data set");
        cmd.addOption("--read-file-only",      "+fo",     "read file format only (default)");
        cmd.addOption("--read-dataset",        "-f",      "read data set without file meta information");
      cmd.addSubGroup("input files:");
        cmd.addOption("--read-from-dicomdir",  "+rd",     "read information on input files from DICOMDIR");
        cmd.addOption("--scan-directories",    "+sd",     "scan directories for input files (dcmfile-in)");
#ifdef PATTERN_MATCHING_AVAILABLE
        cmd.addOption("--scan-pattern",        "+sp",  1, "[p]attern: string (only w/ --scan-directories)",
                                                          "pattern for filename matching (wildcards)");
#endif
        cmd.addOption("--no-recurse",          "-r",      "do not recurse within directories (default)");
        cmd.addOption("--recurse",             "+r",      "recurse within specified directories");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("transfer syntax conversion:");
        cmd.addOption("--decompress-never",    "-dn",     "never decompress compressed data sets");
        cmd.addOption("--decompress-lossless", "+dls",    "only decompress lossless compression (default)");
        cmd.addOption("--decompress-lossy",    "+dly",    "decompress both lossy and lossless compression");
#ifdef WITH_ZLIB
      cmd.addSubGroup("deflate compression level:");
        cmd.addOption("--compression-level",   "+cl",  1, "[l]evel: integer (default: 6)",
                                                          "0=uncompressed, 1=fastest, 9=best compression");
#endif
      cmd.addSubGroup("other processing options:");
        cmd.addOption("--no-halt",             "-nh",     "do not halt on first invalid input file\nor if unsuccessful store encountered");
        cmd.addOption("--no-illegal-proposal", "-nip",    "do not propose any presentation context that\ndoes not contain the default TS (if needed)");
        cmd.addOption("--no-uid-checks",       "-nuc",    "do not check UID values of input files");

    cmd.addGroup("network options:");
      cmd.addSubGroup("application entity titles:");
        cmd.addOption("--aetitle",             "-aet", 1, "[a]etitle: string",
                                                          "set my calling AE title (default: " APPLICATIONTITLE ")");
        cmd.addOption("--call",                "-aec", 1, "[a]etitle: string",
                                                          "set called AE title of peer (default: " PEERAPPLICATIONTITLE ")");
      cmd.addSubGroup("association handling:");
        cmd.addOption("--multi-associations",  "+ma",     "use multiple associations (one after the other)\nif needed to transfer the instances (default)");
        cmd.addOption("--single-association",  "-ma",     "always use a single association");
      cmd.addSubGroup("other network options:");
        cmd.addOption("--timeout",             "-to",  1, "[s]econds: integer (default: unlimited)",
                                                          "timeout for connection requests");
        CONVERT_TO_STRING("[s]econds: integer (default: " << opt_acseTimeout << ")", optString1);
        cmd.addOption("--acse-timeout",        "-ta",  1, optString1.c_str(),
                                                          "timeout for ACSE messages");
        cmd.addOption("--dimse-timeout",       "-td",  1, "[s]econds: integer (default: unlimited)",
                                                          "timeout for DIMSE messages");
        CONVERT_TO_STRING("[n]umber of bytes: integer (" << ASC_MINIMUMPDUSIZE << ".." << ASC_MAXIMUMPDUSIZE << ")", optString2);
        CONVERT_TO_STRING("set max receive pdu to n bytes (default: " << opt_maxReceivePDULength << ")", optString3);
        cmd.addOption("--max-pdu",             "-pdu", 1, optString2.c_str(),
                                                          optString3.c_str());
        cmd.addOption("--max-send-pdu",                1, optString2.c_str(),
                                                          "restrict max send pdu to n bytes");
    cmd.addGroup("output options:");
      cmd.addSubGroup("general:");
        cmd.addOption("--create-report-file",  "+crf", 1, "[f]ilename: string",
                                                          "create a detailed report on the transfer\n(if successful) and write it to text file f");

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
                COUT << OFendl << "External libraries used:" << OFendl;
#ifdef WITH_ZLIB
                COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
                COUT << "- " << DiJPEGPlugin::getLibraryVersionString() << OFendl;
                COUT << "- " << DJLSDecoderRegistration::getLibraryVersionString() << OFendl;
                return EXITCODE_NO_ERROR;
            }
            if (cmd.findOption("--list-decoders"))
            {
                app.printHeader(OFFalse /*print host identifier*/);
                COUT << OFendl << "Transfer syntaxes supported natively:" << OFendl;
                COUT << "- " << DcmXfer(EXS_LittleEndianImplicit).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_LittleEndianExplicit).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_BigEndianExplicit).getXferName() << OFendl;
                COUT << OFendl << "Transfer syntaxes supported by decoders:" << OFendl;
#ifdef WITH_ZLIB
                COUT << "- " << DcmXfer(EXS_DeflatedLittleEndianExplicit).getXferName() << OFendl;
#endif
                COUT << "- " << DcmXfer(EXS_JPEGProcess1).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_JPEGProcess2_4).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_JPEGProcess6_8).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_JPEGProcess10_12).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_JPEGProcess14).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_JPEGProcess14SV1).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_JPEGLSLossless).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_JPEGLSLossy).getXferName() << OFendl;
                COUT << "- " << DcmXfer(EXS_RLELossless).getXferName() << OFendl;
                return EXITCODE_NO_ERROR;
            }
        }

        /* general options */
        OFLog::configureFromCommandLine(cmd, app);
        if (cmd.findOption("--verbose-pc"))
        {
            app.checkDependence("--verbose-pc", "verbose mode", dcmsendLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
            opt_showPresentationContexts = OFTrue;
        }

        /* input options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--read-file")) opt_readMode = ERM_autoDetect;
        if (cmd.findOption("--read-file-only")) opt_readMode = ERM_fileOnly;
        if (cmd.findOption("--read-dataset")) opt_readMode = ERM_dataset;
        cmd.endOptionBlock();

        if (cmd.findOption("--read-from-dicomdir")) opt_dicomDir = OFTrue;
        if (cmd.findOption("--scan-directories")) opt_scanDir = OFTrue;
#ifdef PATTERN_MATCHING_AVAILABLE
        if (cmd.findOption("--scan-pattern"))
        {
            app.checkDependence("--scan-pattern", "--scan-directories", opt_scanDir);
            app.checkValue(cmd.getValue(opt_scanPattern));
        }
#endif
        cmd.beginOptionBlock();
        if (cmd.findOption("--no-recurse")) opt_recurse = OFFalse;
        if (cmd.findOption("--recurse"))
        {
            app.checkDependence("--recurse", "--scan-directories", opt_scanDir);
            opt_recurse = OFTrue;
        }
        cmd.endOptionBlock();

        /* processing options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--decompress-never")) opt_decompressionMode = DcmStorageSCU::DM_never;
        if (cmd.findOption("--decompress-lossless")) opt_decompressionMode = DcmStorageSCU::DM_losslessOnly;
        if (cmd.findOption("--decompress-lossy")) opt_decompressionMode = DcmStorageSCU::DM_lossyAndLossless;
        cmd.endOptionBlock();
#ifdef WITH_ZLIB
        if (cmd.findOption("--compression-level"))
        {
            app.checkValue(cmd.getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
            dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
        }
#endif
        if (cmd.findOption("--no-halt"))
        {
            opt_haltOnInvalidFile = OFFalse;
            opt_haltOnUnsuccessfulStore = OFFalse;
        }
        if (cmd.findOption("--no-illegal-proposal")) opt_allowIllegalProposal = OFFalse;
        if (cmd.findOption("--no-uid-checks")) opt_checkUIDValues = OFFalse;

        /* network options */
        if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
        if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

        cmd.beginOptionBlock();
        if (cmd.findOption("--multi-associations")) opt_multipleAssociations = OFTrue;
        if (cmd.findOption("--single-association")) opt_multipleAssociations = OFFalse;
        cmd.endOptionBlock();

        if (cmd.findOption("--timeout"))
        {
            app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
            dcmConnectionTimeout.set(OFstatic_cast(Sint32, opt_timeout));
        }
        if (cmd.findOption("--acse-timeout"))
            app.checkValue(cmd.getValueAndCheckMin(opt_acseTimeout, 1));
        if (cmd.findOption("--dimse-timeout"))
        {
            app.checkValue(cmd.getValueAndCheckMin(opt_dimseTimeout, 1));
            opt_blockMode = DIMSE_NONBLOCKING;
        }
        if (cmd.findOption("--max-pdu"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_maxReceivePDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
        if (cmd.findOption("--max-send-pdu"))
        {
            app.checkValue(cmd.getValueAndCheckMinMax(opt_maxSendPDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
            dcmMaxOutgoingPDUSize.set(OFstatic_cast(Uint32, opt_maxSendPDULength));
        }

        /* output options */
        if (cmd.findOption("--create-report-file"))
            app.checkValue(cmd.getValue(opt_reportFilename));

        /* command line parameters */
        paramCount = cmd.getParamCount();
        cmd.getParam(1, opt_peer);
        app.checkParam(cmd.getParamAndCheckMinMax(2, opt_port, 1, 65535));
    }

    // register JPEG decoder
    DJDecoderRegistration::registerCodecs();
    // register JPEG-LS decoder
    DJLSDecoderRegistration::registerCodecs();
    // register RLE decoder
    DcmRLEDecoderRegistration::registerCodecs();

    /* print resource identifier */
    OFLOG_DEBUG(dcmsendLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmsendLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* start with the real work */
    if (opt_scanDir)
        OFLOG_INFO(dcmsendLogger, "determining input files ...");
    /* iterate over all input filenames/directories */
    OFList<OFString> inputFiles;
    const char *paramString = NULL;
    for (int i = 3; i <= paramCount; i++)
    {
        cmd.getParam(i, paramString);
        /* search directory recursively (if required) */
        if (OFStandard::dirExists(paramString))
        {
            if (opt_scanDir)
                OFStandard::searchDirectoryRecursively(paramString, inputFiles, opt_scanPattern, "" /* dirPrefix */, opt_recurse);
            else
                OFLOG_WARN(dcmsendLogger, "ignoring directory because option --scan-directories is not set: " << paramString);
        } else
            inputFiles.push_back(paramString);
    }

    /* check whether there are any input files at all */
    if (inputFiles.empty())
    {
        OFLOG_FATAL(dcmsendLogger, "no input files to be processed");
        cleanup();
        return EXITCODE_NO_INPUT_FILES;
    }

    DcmStorageSCU storageSCU;
    OFCondition status;
    unsigned long numInvalidFiles = 0;

    /* set parameters used for processing the input files */
    storageSCU.setReadFromDICOMDIRMode(opt_dicomDir);
    storageSCU.setHaltOnInvalidFileMode(opt_haltOnInvalidFile);

    OFLOG_INFO(dcmsendLogger, "checking input files ...");
    /* iterate over all input filenames */
    const char *currentFilename = NULL;
    OFListIterator(OFString) if_iter = inputFiles.begin();
    OFListIterator(OFString) if_last = inputFiles.end();
    while (if_iter != if_last)
    {
        currentFilename = (*if_iter).c_str();
        /* and add them to the list of instances to be transmitted */
        status = storageSCU.addDicomFile(currentFilename, opt_readMode, opt_checkUIDValues);
        if (status.bad())
        {
            /* check for empty filename */
            if (strlen(currentFilename) == 0)
                currentFilename = "<empty string>";
            /* if something went wrong, we either terminate or ignore the file */
            if (opt_haltOnInvalidFile)
            {
                OFLOG_FATAL(dcmsendLogger, "bad DICOM file: " << currentFilename << ": " << status.text());
                cleanup();
                return EXITCODE_INVALID_INPUT_FILE;
            } else {
                OFLOG_WARN(dcmsendLogger, "bad DICOM file: " << currentFilename << ": " << status.text() << ", ignoring file");
            }
            ++numInvalidFiles;
        }
        ++if_iter;
    }

    /* check whether there are any valid input files */
    if (storageSCU.getNumberOfSOPInstances() == 0)
    {
        OFLOG_FATAL(dcmsendLogger, "no valid input files to be processed");
        cleanup();
        return EXITCODE_NO_VALID_INPUT_FILES;
    } else {
        OFLOG_DEBUG(dcmsendLogger, "in total, there are " << storageSCU.getNumberOfSOPInstances()
            << " SOP instances to be sent, " << numInvalidFiles << " invalid files are ignored");
    }

    /* set network parameters */
    storageSCU.setPeerHostName(opt_peer);
    storageSCU.setPeerPort(OFstatic_cast(Uint16, opt_port));
    storageSCU.setPeerAETitle(opt_peerTitle);
    storageSCU.setAETitle(opt_ourTitle);
    storageSCU.setMaxReceivePDULength(opt_maxReceivePDULength);
    storageSCU.setACSETimeout(opt_acseTimeout);
    storageSCU.setDIMSETimeout(opt_dimseTimeout);
    storageSCU.setDIMSEBlockingMode(opt_blockMode);
    storageSCU.setVerbosePCMode(opt_showPresentationContexts);
    storageSCU.setDatasetConversionMode(opt_decompressionMode != DcmStorageSCU::DM_never);
    storageSCU.setDecompressionMode(opt_decompressionMode);
    storageSCU.setHaltOnUnsuccessfulStoreMode(opt_haltOnUnsuccessfulStore);
    storageSCU.setAllowIllegalProposalMode(opt_allowIllegalProposal);

    /* output information on the single/multiple associations setting */
    if (opt_multipleAssociations)
    {
        OFLOG_DEBUG(dcmsendLogger, "multiple associations allowed (option --multi-associations used)");
    } else {
        OFLOG_DEBUG(dcmsendLogger, "only a single associations allowed (option --single-association used)");
    }

    /* add presentation contexts to be negotiated (if there are still any) */
    while ((status = storageSCU.addPresentationContexts()).good())
    {
        if (opt_multipleAssociations)
        {
            /* output information on the start of the new association */
            if (dcmsendLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
            {
                OFLOG_DEBUG(dcmsendLogger, OFString(65, '-') << OFendl
                    << "starting association #" << (storageSCU.getAssociationCounter() + 1));
            } else {
                OFLOG_INFO(dcmsendLogger, "starting association #" << (storageSCU.getAssociationCounter() + 1));
            }
        }
        OFLOG_INFO(dcmsendLogger, "initializing network ...");
        /* initialize network */
        status = storageSCU.initNetwork();
        if (status.bad())
        {
            OFLOG_FATAL(dcmsendLogger, "cannot initialize network: " << status.text());
            cleanup();
            return EXITCODE_CANNOT_INITIALIZE_NETWORK;
        }
        OFLOG_INFO(dcmsendLogger, "negotiating network association ...");
        /* negotiate network association with peer */
        status = storageSCU.negotiateAssociation();
        if (status.bad())
        {
            // check whether we can continue with a new association
            if (status == NET_EC_NoAcceptablePresentationContexts)
            {
                OFLOG_ERROR(dcmsendLogger, "cannot negotiate network association: " << status.text());
                // check whether there are any SOP instances to be sent
                const size_t numToBeSent = storageSCU.getNumberOfSOPInstancesToBeSent();
                if (numToBeSent > 0)
                {
                    OFLOG_WARN(dcmsendLogger, "trying to continue with a new association "
                        << "in order to send the remaining " << numToBeSent << " SOP instances");
                }
            } else {
                OFLOG_FATAL(dcmsendLogger, "cannot negotiate network association: " << status.text());
                cleanup();
                return EXITCODE_CANNOT_NEGOTIATE_ASSOCIATION;
            }
        }
        if (status.good())
        {
            OFLOG_INFO(dcmsendLogger, "sending SOP instances ...");
            /* send SOP instances to be transferred */
            status = storageSCU.sendSOPInstances();
            if (status.bad())
            {
                OFLOG_FATAL(dcmsendLogger, "cannot send SOP instance: " << status.text());
                // handle certain error conditions (initiated by the communication peer)
                if (status == DUL_PEERREQUESTEDRELEASE)
                {
                    // peer requested release (aborting)
                    storageSCU.closeAssociation(DCMSCU_PEER_REQUESTED_RELEASE);
                }
                else if (status == DUL_PEERABORTEDASSOCIATION)
                {
                    // peer aborted the association
                    storageSCU.closeAssociation(DCMSCU_PEER_ABORTED_ASSOCIATION);
                }
                cleanup();
                return EXITCODE_CANNOT_SEND_REQUEST;
            }
        }
        /* close current network association */
        storageSCU.releaseAssociation();
        /* check whether multiple associations are permitted */
        if (!opt_multipleAssociations)
            break;
    }

    /* if anything went wrong, report it to the logger */
    if (status.bad() && (status != NET_EC_NoPresentationContextsDefined))
    {
        OFLOG_ERROR(dcmsendLogger, "cannot add presentation contexts: " << status.text());
        cleanup();
        return EXITCODE_CANNOT_ADD_PRESENTATION_CONTEXT;
    }

    /* create a detailed report on the transfer of instances ... */
    if ((opt_reportFilename != NULL) && (strlen(opt_reportFilename) > 0))
    {
        /* ... and write it to the specified text file */
        status = storageSCU.createReportFile(opt_reportFilename);
        if (status.bad())
        {
            cleanup();
            return EXITCODE_CANNOT_WRITE_REPORT_FILE; // TODO: do we really want to exit?
        }
    }

    /* output some status information on the overall sending process */
    if (dcmsendLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
    {
        OFString summaryText;
        storageSCU.getStatusSummary(summaryText);
        OFLOG_INFO(dcmsendLogger, OFendl << summaryText);
    }

    /* make sure that everything is cleaned up properly */
    cleanup();
    return EXITCODE_NO_ERROR;
}
