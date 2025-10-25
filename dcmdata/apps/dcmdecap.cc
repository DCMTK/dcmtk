/*
 *
 *  Copyright (C) 2007-2025, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg, Tingyan Xu
 *
 *  Purpose: Extract encapsulated file from DICOM encapsulated storage object
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */
#include "dcmtk/dcmdata/dcdocdec.h"    /* for class DcmDocumentDecapsulator */
#include "dcmtk/dcmdata/dcdict.h"      /* for data dictionary */

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmdecap"

static OFLogger dcmdecapLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);
static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 3
#define LONGCOL 20
#define EXITCODE_EXEC_FAILED                 91


static void addInputOptions(OFCommandLine& cmd)
{
    cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",            "+f", "read file format or data set (default)");
        cmd.addOption("--read-file-only",       "+fo", "read file format only");
        cmd.addOption("--read-dataset",         "-f", "read data set without file meta information");

    cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",       "-t=", "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",     "-td", "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",     "-te", "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",        "-tb", "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",   "-ti", "read with implicit VR little endian TS");

    cmd.addSubGroup("parsing of odd-length attributes:");
        cmd.addOption("--accept-odd-length",    "+ao", "accept odd length attributes (default)");
        cmd.addOption("--assume-even-length",   "+ae", "assume real length is one byte larger");

    cmd.addSubGroup("handling of undefined length UN elements:");
        cmd.addOption("--enable-cp246",         "+ui", "read undefined len UN as implicit VR (default)");
        cmd.addOption("--disable-cp246",        "-ui", "read undefined len UN as explicit VR");

    cmd.addSubGroup("handling of defined length UN elements:");
        cmd.addOption("--retain-un",            "-uc", "retain elements as UN (default)");
        cmd.addOption("--convert-un",           "+uc", "convert to real VR if known");

    cmd.addSubGroup("automatic data correction:");
        cmd.addOption("--enable-correction",    "+dc", "enable automatic data correction (default)");
        cmd.addOption("--disable-correction",   "-dc", "disable automatic data correction");

#ifdef WITH_ZLIB
    cmd.addSubGroup("bitstream format of deflated input:");
        cmd.addOption("--bitstream-deflated",   "+bd", "expect deflated bitstream (default)");
        cmd.addOption("--bitstream-zlib",       "+bz", "expect deflated zlib bitstream");
#endif
}

static void parseInputOptions(OFConsoleApplication& app, OFCommandLine& cmd, DcmDocumentDecapsulator& docdec)
{
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-file"))
    {
        opt_readMode = ERM_autoDetect;
        docdec.setReadMode(opt_readMode);
    }
    if (cmd.findOption("--read-file-only"))
    {
        opt_readMode = ERM_fileOnly;
        docdec.setReadMode(opt_readMode);
    }
    if (cmd.findOption("--read-dataset"))
    {
        opt_readMode = ERM_dataset;
        docdec.setReadMode(opt_readMode);
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-xfer-auto"))
    {
        opt_ixfer = EXS_Unknown;
        docdec.setInputXferSyntax(opt_ixfer);
    }
    if (cmd.findOption("--read-xfer-detect"))
    {
        dcmAutoDetectDatasetXfer.set(OFTrue);
    }
    if (cmd.findOption("--read-xfer-little"))
    {
        opt_ixfer = EXS_LittleEndianExplicit;
        app.checkDependence("--read-xfer-little", "--read-dataset", opt_readMode == ERM_dataset);
        docdec.setInputXferSyntax(opt_ixfer);
    }
    if (cmd.findOption("--read-xfer-big"))
    {
        opt_ixfer = EXS_BigEndianExplicit;
        app.checkDependence("--read-xfer-big", "--read-dataset", opt_readMode == ERM_dataset);
        docdec.setInputXferSyntax(opt_ixfer);
    }
    if (cmd.findOption("--read-xfer-implicit"))
    {
        opt_ixfer = EXS_LittleEndianImplicit;
        app.checkDependence("--read-xfer-implicit", "--read-dataset", opt_readMode == ERM_dataset);
        docdec.setInputXferSyntax(opt_ixfer);
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--accept-odd-length"))
    {
        dcmAcceptOddAttributeLength.set(OFTrue);
    }
    if (cmd.findOption("--assume-even-length"))
    {
        dcmAcceptOddAttributeLength.set(OFFalse);
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--enable-cp246"))
    {
        dcmEnableCP246Support.set(OFTrue);
    }
    if (cmd.findOption("--disable-cp246"))
    {
        dcmEnableCP246Support.set(OFFalse);
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--retain-un"))
    {
        dcmEnableUnknownVRConversion.set(OFFalse);
    }
    if (cmd.findOption("--convert-un"))
    {
        dcmEnableUnknownVRConversion.set(OFTrue);
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--enable-correction"))
    {
        dcmEnableAutomaticInputDataCorrection.set(OFTrue);
    }
    if (cmd.findOption("--disable-correction"))
    {
        dcmEnableAutomaticInputDataCorrection.set(OFFalse);
    }
    cmd.endOptionBlock();

#ifdef WITH_ZLIB
    cmd.beginOptionBlock();
    if (cmd.findOption("--bitstream-deflated"))
    {
        dcmZlibExpectRFC1950Encoding.set(OFFalse);
    }
    if (cmd.findOption("--bitstream-zlib"))
    {
        dcmZlibExpectRFC1950Encoding.set(OFTrue);
    }
    cmd.endOptionBlock();
#endif
}

static void addProcessingOptions(OFCommandLine& cmd)
{
    cmd.addGroup("processing options:");
    cmd.addSubGroup("execution options:");
        cmd.addOption("--exec",                 "-x",  1, "[c]ommand: string",
                                                          "execute command c after document extraction");
}

static void parseProcessingOptions(OFConsoleApplication& app, OFCommandLine& cmd, DcmDocumentDecapsulator& docdec)
{
    const char *c;
    if (cmd.findOption("--exec"))
    {
        app.checkValue(cmd.getValue(c));
        docdec.setExecString(c);
    }
}

static void addCommandLineOptions(OFCommandLine& cmd)
{
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
    cmd.addParam("dcmfile-in",  "DICOM input filename (\"-\" for stdin)");
    cmd.addParam("encfile-out", "Encapsulated document output filename\n(\"-\" for stdout)");
    cmd.addGeneralOptions(LONGCOL, SHORTCOL);

    OFLog::addOptions(cmd);

    addInputOptions(cmd);
    addProcessingOptions(cmd);
}

static void parseCommandLineOptions(OFConsoleApplication& app, OFCommandLine& cmd, DcmDocumentDecapsulator& docdec)
{
    const char *c;
    cmd.getParam(1, c);
    docdec.setInputFile(c);
    cmd.getParam(2, c);
    docdec.setOutputFile(c);
    OFLog::configureFromCommandLine(cmd, app);
    parseInputOptions(app, cmd, docdec);
    parseProcessingOptions(app, cmd, docdec);
}


int main(int argc, char* argv[])
{
    DcmDocumentDecapsulator docdec;
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Extract file from DICOM encapsulated storage", rcsid);
    OFCommandLine cmd;
    addCommandLineOptions(cmd);

    // evaluate command line
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
        // check exclusive options first
        if (cmd.hasExclusiveOption())
        {
            if (cmd.findOption("--version"))
            {
                app.printHeader(OFTrue /*print host identifier*/);
                COUT << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
                COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
                COUT << " none" << OFendl;
#endif
                return EXITCODE_NO_ERROR;
            }
        }

        // parse command line parameters and options
        parseCommandLineOptions(app, cmd, docdec);
    }

    // print resource identifier
    OFLOG_DEBUG(dcmdecapLogger, rcsid << OFendl);

    // make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmdecapLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // read the DICOM file
    OFCondition cond = docdec.loadDICOMFile();
    if (cond == EC_InvalidFilename) return EXITCODE_NO_INPUT_FILES;
    else if (cond.bad()) return EXITCODE_CANNOT_READ_INPUT_FILE;

    // extract the encapsulated document and write to file
    cond = docdec.writeEncapsulatedContentToFile();
    if (cond == EC_MissingAttribute) return EXITCODE_INVALID_INPUT_FILE;
    else if (cond.bad()) return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;

    // execute the command line (if defined)
    cond = docdec.executeCommand();
    if (cond.bad()) return EXITCODE_EXEC_FAILED;

    // all done
    return EXITCODE_NO_ERROR;
}
