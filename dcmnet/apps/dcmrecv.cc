/*
 *
 *  Copyright (C) 2013-2023, OFFIS e.V.
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
 *  Purpose: Simple Storage Service Class Provider
 *
 */


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"       /* for OFStandard functions */
#include "dcmtk/ofstd/ofconapp.h"    /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofstream.h"    /* for OFStringStream et al. */
#include "dcmtk/dcmdata/dcdict.h"    /* for global data dictionary */
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmtk version name */
#include "dcmtk/dcmdata/cmdlnarg.h"  /* for prepareCmdLineArgs */
#include "dcmtk/dcmnet/dstorscp.h"   /* for DcmStorageSCP */
#include "dcmtk/dcmtls/tlsopt.h"     /* for DcmTLSOptions */


/* general definitions */

#define OFFIS_CONSOLE_APPLICATION "dcmrecv"

static OFLogger dcmrecvLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


/* exit codes for this command line tool */
/* (common codes are defined in "ofexit.h" included from "ofconapp.h") */

// network errors
#define EXITCODE_CANNOT_INITIALIZE_NETWORK       60      // placeholder, currently not used
#define EXITCODE_CANNOT_START_SCP_AND_LISTEN     64
#define EXITCODE_INVALID_ASSOCIATION_CONFIG      66
#define EXITCODE_CANNOT_CREATE_TRANSPORT_LAYER   71


/* helper macro for converting stream output to a string */
#define CONVERT_TO_STRING(output, string) \
    optStream.str(""); \
    optStream.clear(); \
    optStream << output << OFStringStream_ends; \
    OFSTRINGSTREAM_GETOFSTRING(optStream, string)


/* main program */

#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{

#ifdef WITH_OPENSSL
    DcmTLSTransportLayer::initializeOpenSSL();
#endif

    OFOStringStream optStream;
    DcmTLSOptions tlsOptions(NET_ACCEPTOR);

    const char *opt_configFile = NULL;
    const char *opt_profileName = NULL;
    const char *opt_aeTitle = NULL;                 // default: respond with called AE title
    const char *opt_outputDirectory = ".";          // default: current directory
    const char *opt_filenameExtension = "";

    OFCmdUnsignedInt opt_port = 0;
    OFCmdUnsignedInt opt_dimseTimeout = 0;
    OFCmdUnsignedInt opt_acseTimeout = 30;
    OFCmdUnsignedInt opt_maxPDULength = ASC_DEFAULTMAXPDU;
    T_DIMSE_BlockingMode opt_blockingMode = DIMSE_BLOCKING;

    OFBool opt_showPresentationContexts = OFFalse;  // default: do not show presentation contexts in verbose mode
    OFBool opt_HostnameLookup = OFTrue;             // default: perform hostname lookup (for log output)

    DcmStorageSCP::E_DirectoryGenerationMode opt_directoryGeneration = DcmStorageSCP::DGM_NoSubdirectory;
    DcmStorageSCP::E_FilenameGenerationMode opt_filenameGeneration = DcmStorageSCP::FGM_SOPInstanceUID;
    DcmStorageSCP::E_DatasetStorageMode opt_datasetStorage = DcmStorageSCP::DGM_StoreToFile;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Simple DICOM storage SCP (receiver)", rcsid);
    OFCommandLine cmd;

    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
    cmd.addParam("port", "tcp/ip port number to listen on");

    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);
      cmd.addOption("--verbose-pc",            "+v",      "show presentation contexts in verbose mode");

    cmd.addGroup("network options:");
      cmd.addSubGroup("association negotiation profile from configuration file:");
        cmd.addOption("--config-file",         "-xf",  2, "[f]ilename, [p]rofile: string",
                                                          "use profile p from configuration file f");
      cmd.addSubGroup("application entity title:");
        cmd.addOption("--use-called-aetitle",  "-uca",    "always respond with called AE title (default)");
        cmd.addOption("--aetitle",             "-aet", 1, "[a]etitle: string",
                                                          "set my AE title and check called AE title");
      cmd.addSubGroup("other network options:");
        CONVERT_TO_STRING("[s]econds: integer (default: " << opt_acseTimeout << ")", optString1);
        cmd.addOption("--acse-timeout",        "-ta",  1, optString1.c_str(),
                                                          "timeout for ACSE messages");
        cmd.addOption("--dimse-timeout",       "-td",  1, "[s]econds: integer (default: unlimited)",
                                                          "timeout for DIMSE messages");
        CONVERT_TO_STRING("[n]umber of bytes: integer (" << ASC_MINIMUMPDUSIZE << ".." << ASC_MAXIMUMPDUSIZE << ")", optString2);
        CONVERT_TO_STRING("set max receive pdu to n bytes (default: " << opt_maxPDULength << ")", optString3);
        cmd.addOption("--max-pdu",             "-pdu", 1, optString2.c_str(),
                                                          optString3.c_str());
        cmd.addOption("--disable-host-lookup", "-dhl",    "disable hostname lookup");

    /* add TLS specific command line options if (and only if) we are compiling with OpenSSL */
    tlsOptions.addTLSCommandlineOptions(cmd);

    cmd.addGroup("output options:");
      cmd.addSubGroup("general:");
        CONVERT_TO_STRING("[d]irectory: string (default: \"" << opt_outputDirectory << "\")", optString4);
        cmd.addOption("--output-directory",    "-od",  1, optString4.c_str(),
                                                          "write received objects to existing directory d");
      cmd.addSubGroup("subdirectory generation:");
        cmd.addOption("--no-subdir",           "-s",      "do not generate any subdirectories (default)");
        cmd.addOption("--series-date-subdir",  "+ssd",    "generate subdirectories from series date");
      cmd.addSubGroup("filename generation:");
        cmd.addOption("--default-filenames",   "+fd",     "generate filename from instance UID (default)");
        cmd.addOption("--unique-filenames",    "+fu",     "generate unique filename based on new UID");
        cmd.addOption("--short-unique-names",  "+fsu",    "generate short pseudo-random unique filename");
        cmd.addOption("--system-time-names",   "+fst",    "generate filename from current system time");
        cmd.addOption("--filename-extension",  "-fe",  1, "[e]xtension: string (default: none)",
                                                          "append e to all generated filenames");
      cmd.addSubGroup("storage mode:");
        cmd.addOption("--normal",              "-B",      "allow implicit format conversions (default)");
        cmd.addOption("--bit-preserving",      "+B",      "write dataset exactly as received");
        cmd.addOption("--ignore",                         "ignore dataset, receive but do not store it");

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
#ifdef WITH_OPENSSL
                COUT << OFendl;
                tlsOptions.printLibraryVersion();
#else
                COUT << " none" << OFendl;
#endif
                return EXITCODE_NO_ERROR;
            }

            /* check if the command line contains the --list-ciphers option */
            if (tlsOptions.listOfCiphersRequested(cmd))
            {
                tlsOptions.printSupportedCiphersuites(app, COUT);
                return EXITCODE_NO_ERROR;
            }

        }

        /* general options */
        OFLog::configureFromCommandLine(cmd, app);
        if (cmd.findOption("--verbose-pc"))
        {
            app.checkDependence("--verbose-pc", "verbose mode", dcmrecvLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
            opt_showPresentationContexts = OFTrue;
        }

        /* network options */
        if (cmd.findOption("--config-file"))
        {
            app.checkValue(cmd.getValue(opt_configFile));
            app.checkValue(cmd.getValue(opt_profileName));
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--aetitle"))
            app.checkValue(cmd.getValue(opt_aeTitle));
        if (cmd.findOption("--use-called-aetitle"))
            opt_aeTitle = NULL;
        cmd.endOptionBlock();

        if (cmd.findOption("--acse-timeout"))
            app.checkValue(cmd.getValueAndCheckMin(opt_acseTimeout, 1));
        if (cmd.findOption("--dimse-timeout"))
        {
            app.checkValue(cmd.getValueAndCheckMin(opt_dimseTimeout, 1));
            opt_blockingMode = DIMSE_NONBLOCKING;
        }
        if (cmd.findOption("--max-pdu"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_maxPDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
        if (cmd.findOption("--disable-host-lookup"))
            opt_HostnameLookup = OFFalse;

        /* output options */
        if (cmd.findOption("--output-directory"))
            app.checkValue(cmd.getValue(opt_outputDirectory));

        cmd.beginOptionBlock();
        if (cmd.findOption("--no-subdir"))
            opt_directoryGeneration = DcmStorageSCP::DGM_NoSubdirectory;
        if (cmd.findOption("--series-date-subdir"))
            opt_directoryGeneration = DcmStorageSCP::DGM_SeriesDate;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--default-filenames"))
            opt_filenameGeneration = DcmStorageSCP::FGM_SOPInstanceUID;
        if (cmd.findOption("--unique-filenames"))
            opt_filenameGeneration = DcmStorageSCP::FGM_UniqueFromNewUID;
        if (cmd.findOption("--short-unique-names"))
            opt_filenameGeneration = DcmStorageSCP::FGM_ShortUniquePseudoRandom;
        if (cmd.findOption("--system-time-names"))
            opt_filenameGeneration = DcmStorageSCP::FGM_CurrentSystemTime;
        cmd.endOptionBlock();

        if (cmd.findOption("--filename-extension"))
            app.checkValue(cmd.getValue(opt_filenameExtension));

        cmd.beginOptionBlock();
        if (cmd.findOption("--normal"))
            opt_datasetStorage = DcmStorageSCP::DGM_StoreToFile;
        if (cmd.findOption("--bit-preserving"))
        {
            app.checkConflict("--bit-preserving", "--series-date-subdir", opt_directoryGeneration == DcmStorageSCP::DGM_SeriesDate);
            opt_datasetStorage = DcmStorageSCP::DGM_StoreBitPreserving;
        }
        if (cmd.findOption("--ignore"))
            opt_datasetStorage = DcmStorageSCP::DSM_Ignore;
        cmd.endOptionBlock();

        /* command line parameters */
        app.checkParam(cmd.getParamAndCheckMinMax(1, opt_port, 1, 65535));

        /* evaluate (most of) the TLS command line options (if we are compiling with OpenSSL) */
        tlsOptions.parseArguments(app, cmd);
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmrecvLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmrecvLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* start with the real work */
    DcmStorageSCP storageSCP;
    OFCondition status;

    OFLOG_INFO(dcmrecvLogger, "configuring service class provider ...");

    /* set general network parameters */
    storageSCP.setPort(OFstatic_cast(Uint16, opt_port));
    if (opt_aeTitle != NULL)
        storageSCP.setAETitle(opt_aeTitle);
    storageSCP.setMaxReceivePDULength(OFstatic_cast(Uint32, opt_maxPDULength));
    storageSCP.setACSETimeout(OFstatic_cast(Uint32, opt_acseTimeout));
    storageSCP.setDIMSETimeout(OFstatic_cast(Uint32, opt_dimseTimeout));
    storageSCP.setDIMSEBlockingMode(opt_blockingMode);
    storageSCP.setVerbosePCMode(opt_showPresentationContexts);
    storageSCP.setRespondWithCalledAETitle(opt_aeTitle == NULL);
    storageSCP.setHostLookupEnabled(opt_HostnameLookup);
    storageSCP.setDirectoryGenerationMode(opt_directoryGeneration);
    storageSCP.setFilenameGenerationMode(opt_filenameGeneration);
    storageSCP.setFilenameExtension(opt_filenameExtension);
    storageSCP.setDatasetStorageMode(opt_datasetStorage);

    /* load association negotiation profile from configuration file (if specified) */
    if ((opt_configFile != NULL) && (opt_profileName != NULL))
    {
        status = storageSCP.loadAssociationConfiguration(opt_configFile, opt_profileName);
        if (status.bad())
        {
            OFLOG_FATAL(dcmrecvLogger, "cannot load association configuration: " << status.text());
            return EXITCODE_INVALID_ASSOCIATION_CONFIG;
        }
    } else {
        /* report a warning message that the SCP will not accept any Storage SOP Classes */
        OFLOG_WARN(dcmrecvLogger, "no configuration file specified, SCP will only support the Verification SOP Class");
    }

    /* specify the output directory (also checks whether directory exists and is writable) */
    status = storageSCP.setOutputDirectory(opt_outputDirectory);
    if (status.bad())
    {
        OFLOG_FATAL(dcmrecvLogger, "cannot specify output directory: " << status.text());
        return EXITCODE_INVALID_OUTPUT_DIRECTORY;
    }

    OFLOG_INFO(dcmrecvLogger, "starting service class provider and listening ...");

    /* create a secure transport layer if requested and OpenSSL is available */
    status = tlsOptions.createTransportLayer(NULL, NULL, app, cmd);
    if (status.bad())
    {
        OFString tempStr;
        OFLOG_FATAL(dcmrecvLogger, DimseCondition::dump(tempStr, status));
        return EXITCODE_CANNOT_CREATE_TRANSPORT_LAYER;
    }

    if (tlsOptions.secureConnectionRequested())
        storageSCP.getConfig().setTransportLayer(tlsOptions.getTransportLayer());

    /* start SCP and listen on the specified port */
    status = storageSCP.listen();
    if (status.bad())
    {
        OFLOG_FATAL(dcmrecvLogger, "cannot start SCP and listen on port " << opt_port << ": " << status.text());
        return EXITCODE_CANNOT_START_SCP_AND_LISTEN;
    }

    /* make sure that everything is cleaned up properly */
#ifdef DEBUG
    /* useful for debugging with dmalloc */
    dcmDataDict.clear();
#endif

    return EXITCODE_NO_ERROR;
}
