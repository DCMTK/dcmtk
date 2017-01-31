/*
*
*  Copyright (C) 2016-2017, OFFIS e.V.
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
*  Author:  Sebastian Grallert
*
*  Purpose: Convert the contents of a DICOM file to JSON format
*
*/

#include "dcmtk/config/osconfig.h"      /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"

#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"

#ifdef WITH_ZLIB
#include <zlib.h>                       /* for zlibVersion() */
#endif
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
#include "dcmtk/ofstd/ofchrenc.h"       /* for OFCharacterEncoding */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcm2json"
#define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM file and data set to JSON"

static OFLogger dcm2jsonLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

/* Function to call all writeJson() functions in DCMTK */
static OFCondition writeFile(STD_NAMESPACE ostream &out,
    const char *ifname,
    DcmFileFormat *dfile,
    const E_FileReadMode readMode,
    const OFBool format,
    const OFBool printMetaheaderInformation,
    const OFBool checkAllStrings)
{
    OFCondition result = EC_IllegalParameter;
    if ((ifname != NULL) && (dfile != NULL))
    {
        DcmDataset *dset = dfile->getDataset();

        /* write JSON document content */

        if (readMode == ERM_dataset)
        {
            result = format
            ?
                dset->writeJson(out, DcmJsonFormatPretty(printMetaheaderInformation))
            :
                dset->writeJson(out, DcmJsonFormatCompact(printMetaheaderInformation))
            ;
        }
        else
        {
            result = format
            ?
                dfile->writeJson(out, DcmJsonFormatPretty(printMetaheaderInformation))
            :
                dfile->writeJson(out, DcmJsonFormatCompact(printMetaheaderInformation))
            ;
        }
    }
    return result;
}

#define SHORTCOL 3
#define LONGCOL 20

int main(int argc, char *argv[])
{
    OFBool opt_checkAllStrings = OFFalse;
    OFBool opt_format = OFTrue;
    OFBool opt_addMetaInformation = OFFalse;

    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    OFString optStr;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",   "DICOM input filename to be converted", OFCmdParam::PM_Mandatory);
    cmd.addParam("jsonfile-out", "JSON output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                 "-h",  "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                     "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",          "+f",  "read file format or data set (default)");
        cmd.addOption("--read-file-only",     "+fo", "read file format only");
        cmd.addOption("--read-dataset",       "-f",  "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",     "-t=", "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",   "-td", "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",   "-te", "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",      "-tb", "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit", "-ti", "read with implicit VR little endian TS");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output format:");
        cmd.addOption("--formatted-code",     "+fc", "output file with human readable formatting (def.)");
        cmd.addOption("--compact-code",       "-fc", "output without formatting (single line of code)");
        cmd.addOption("--write-meta",         "+m",  "write data set with meta information");

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
#if !defined(WITH_ZLIB) && !defined(DCMTK_ENABLE_CHARSET_CONVERSION)
                COUT << " none" << OFendl;
#else
                COUT << OFendl;
#endif
#ifdef WITH_ZLIB
                COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
                COUT << "- " << OFCharacterEncoding::getLibraryVersionString() << OFendl;
#endif
                return 0;
            }
        }

        /* general options */
        OFLog::configureFromCommandLine(cmd, app);

        /* input options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--read-file")) opt_readMode = ERM_autoDetect;
        if (cmd.findOption("--read-file-only")) opt_readMode = ERM_fileOnly;
        if (cmd.findOption("--read-dataset")) opt_readMode = ERM_dataset;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-xfer-auto"))
            opt_ixfer = EXS_Unknown;
        if (cmd.findOption("--read-xfer-detect"))
            dcmAutoDetectDatasetXfer.set(OFTrue);
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

        /* format options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--formatted-code"))
            opt_format = OFTrue;
        if (cmd.findOption("--compact-code"))
            opt_format = OFFalse;
        cmd.endOptionBlock();

        /* meta option */
        if (cmd.findOption("--write-meta"))
            opt_addMetaInformation = OFTrue;
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcm2jsonLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcm2jsonLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    int result = 0;
    /* first parameter is treated as the input filename */
    const char *ifname = NULL;
    cmd.getParam(1, ifname);
    /* check input file */
    if ((ifname != NULL) && (strlen(ifname) > 0))
    {
        /* read DICOM file or data set */
        DcmFileFormat dfile;
        OFCondition status = dfile.loadFile(ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
        if (status.good())
        {
            DcmDataset *dset = dfile.getDataset();
            OFString csetString;
            if (dset->findAndGetOFStringArray(DCM_SpecificCharacterSet, csetString).good())
            {
                if (csetString.compare("ISO_IR 192") != 0 && csetString.compare("ISO_IR 6") != 0)
                {
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
                    /* convert all DICOM strings to UTF-8 */
                    OFLOG_INFO(dcm2jsonLogger, "converting all element values that are affected by SpecificCharacterSet (0008,0005) to UTF-8");
                    status = dset->convertToUTF8();
                    if (status.bad())
                    {
                        OFLOG_FATAL(dcm2jsonLogger, status.text() << ": converting file to UTF-8: " << ifname);
                        result = 4;
                    }
#else
                    OFLOG_FATAL(dcm2jsonLogger, "character set conversion not available");
                    return 4;
#endif
                }
            }
            if (result == 0)
            {
                /* if second parameter is present, it is treated as the output filename ("stdout" otherwise) */
                if (cmd.getParamCount() == 2)
                {
                    const char *ofname = NULL;
                    cmd.getParam(2, ofname);
                    STD_NAMESPACE ofstream stream(ofname);
                    if (stream.good())
                    {
                        /* write content in JSON format to file */
                        if (writeFile(stream, ifname, &dfile, opt_readMode, opt_format,
                            opt_addMetaInformation, opt_checkAllStrings).bad())
                            result = 2;
                    }
                    else
                        result = 1;
                }
                else
                {
                    /* write content in JSON format to standard output */
                    if (writeFile(COUT, ifname, &dfile, opt_readMode, opt_format,
                        opt_addMetaInformation, opt_checkAllStrings).bad())
                        result = 3;
                }
            }
        }
        else
            OFLOG_ERROR(dcm2jsonLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") reading file: " << ifname);
    }
    else
        OFLOG_ERROR(dcm2jsonLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");

    return result;
}
