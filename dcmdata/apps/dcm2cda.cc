/*
 *
 *  Copyright (C) 2023-2024, OFFIS e.V.
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
 *  Author:  Tingyan Xu
 *
 *  Purpose: Extract CDA file from DICOM encapsulated CDA storage object
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcm2cda"

static OFLogger dcm2cdaLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 3
#define LONGCOL 20

void addInputOptions(OFCommandLine& cmd)
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

void addDCM2CDACommandlineOptions(OFCommandLine& cmd)
{
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in", "DICOM input filename (\"-\" for stdin)");
    cmd.addParam("cdafile-out", "CDA output filename");

    cmd.addGeneralOptions(LONGCOL, SHORTCOL);
    OFLog::addOptions(cmd);
    addInputOptions(cmd);
}

/**
 * function for parsing commandline arguments
 */
void parseArguments(OFConsoleApplication& app, OFCommandLine& cmd,
    E_FileReadMode& opt_readMode, E_TransferSyntax& opt_ixfer,
    const char*& opt_ifname,
    const char*& opt_ofname
)
{
    cmd.getParam(1, opt_ifname);
    cmd.getParam(2, opt_ofname);

    OFLog::configureFromCommandLine(cmd, app);

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-file"))
    {
        opt_readMode = ERM_autoDetect;
    }
    if (cmd.findOption("--read-file-only"))
    {
        opt_readMode = ERM_fileOnly;
    }
    if (cmd.findOption("--read-dataset"))
    {
        opt_readMode = ERM_dataset;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-xfer-auto"))
    {
        opt_ixfer = EXS_Unknown;
    }
    if (cmd.findOption("--read-xfer-detect"))
    {
        dcmAutoDetectDatasetXfer.set(OFTrue);
    }
    if (cmd.findOption("--read-xfer-little"))
    {
        app.checkDependence("--read-xfer-little", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_LittleEndianExplicit;
    }
    if (cmd.findOption("--read-xfer-big"))
    {
        app.checkDependence("--read-xfer-big", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_BigEndianExplicit;
    }
    if (cmd.findOption("--read-xfer-implicit"))
    {
        app.checkDependence("--read-xfer-implicit", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_LittleEndianImplicit;
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

int main(int argc, char* argv[])
{
    const char* opt_ifname = NULL;
    const char* opt_ofname = NULL;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Extract CDA file from DICOM encapsulated CDA", rcsid);
    OFCommandLine cmd;

    // necessary options
    addDCM2CDACommandlineOptions(cmd);

    // evaluating command line
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
        // checking exclusive options first
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

        /* command line parameters and options */
        parseArguments(app, cmd, opt_readMode, opt_ixfer, opt_ifname, opt_ofname);
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcm2cdaLogger, rcsid << OFendl);

    OFLOG_DEBUG(dcm2cdaLogger, "making sure data dictionary is loaded");
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcm2cdaLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open input file
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_ERROR(dcm2cdaLogger, "invalid filename: <empty string>");
        return EXITCODE_NO_INPUT_FILES;
    }

    OFCondition cond = EC_Normal;

    DcmFileFormat dfile;
    DcmDataset* dataset = dfile.getDataset();

    OFLOG_INFO(dcm2cdaLogger, "open input file " << opt_ifname);
    /* load file to dfile, using given transfer syntax and other variables */
    cond = dfile.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
    if (cond.bad())
    {
        OFLOG_ERROR(dcm2cdaLogger, cond.text() << ": reading file: " << opt_ifname);
        return EXITCODE_CANNOT_READ_INPUT_FILE;
    }

    /* check SOP */
    OFString sopClass;
    cond = dataset->findAndGetOFString(DCM_SOPClassUID, sopClass);
    if (cond.bad() || sopClass != UID_EncapsulatedCDAStorage)
    {
        OFLOG_ERROR(dcm2cdaLogger, "SOPClassUID not of SOP UID_EncapsulatedCDAStorage: " << opt_ifname);
        return EXITCODE_INVALID_INPUT_FILE;
    }

    /* get EncapsulatedDocument */
    Uint8* cdaDocument = NULL;
    unsigned long int delemlen = 0;
    cond = dataset->findAndGetUint8Array(DCM_EncapsulatedDocument, (const Uint8*&)cdaDocument, &delemlen);
    if (cond.bad() || cdaDocument == NULL || delemlen == 0)
    {
        OFLOG_ERROR(dcm2cdaLogger, "EncapsulatedDocument missing or has the wrong VR");
        return EXITCODE_INVALID_INPUT_FILE;
    }

    /* get and check element Encapsulated Document Length */
    Uint32 lenElem;
    cond = dataset->findAndGetUint32(DCM_EncapsulatedDocumentLength, lenElem);
    /* EncapsulatedDocumentLength Element is invalid or
    * it does not fit the length of the encapsulated document
    * (it has to be equal or equal to EncapsulatedDocumentLength -1)
    */
    if (cond.bad() || (lenElem != delemlen && lenElem != delemlen - 1))
    {
        OFLOG_DEBUG(dcm2cdaLogger, "EncapsulatedDocumentLength missing or invalid, "
                                "using length of EncapsulatedDocument");
        lenElem = delemlen;
        /* Strip pad byte at end of file, if there is one.
        * CDA documents end with a closing XML tag, optionally followed by whitespace.
        * If the last character of the file is not a CR ('\r', 13) or LF ('\n', 10), and not the
        * letter '>', we assume it is either trailing garbage or a pad byte, and remove it.
        */
        if (cdaDocument[lenElem - 1] != '\n' && cdaDocument[lenElem - 1] != '\r' && cdaDocument[lenElem - 1] != '>')
        {
            OFLOG_DEBUG(dcm2cdaLogger, "removing the pad byte at end of EncapsulatedDocument");
            --lenElem;
        }
    }

    OFLOG_INFO(dcm2cdaLogger, "writing CDA file to " << opt_ofname);
    FILE* cdafile = fopen(opt_ofname, "wb");
    if (cdafile == NULL)
    {
        OFLOG_ERROR(dcm2cdaLogger, "unable to create file " << opt_ofname);
        return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
    }

    if (lenElem != fwrite(cdaDocument, 1, lenElem, cdafile))
    {
        OFLOG_ERROR(dcm2cdaLogger, "write error in file " << opt_ofname);
        fclose(cdafile);
        return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
    }

    fclose(cdafile);

    OFLOG_INFO(dcm2cdaLogger, "conversion successful");

    return EXITCODE_NO_ERROR;
}
