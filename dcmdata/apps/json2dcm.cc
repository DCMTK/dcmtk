/*
 *
 *  Copyright (C) 2024-2025, OFFIS e.V.
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
 *  Purpose: Convert JSON DICOM document to binary DICOM file
 *
 */

#include "dcmtk/config/osconfig.h"      /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcostrmz.h"     /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmdata/dcfilefo.h"     /* for class DcmFileFormat */
#include "dcmtk/dcmdata/dcdeftag.h"     /* for tag keys */
#include "dcmtk/dcmdata/dcuid.h"        /* for UID constants */
#include "dcmtk/dcmdata/dcdict.h"       /* for the data dictionary */
#include "dcmtk/dcmdata/dcjsonrd.h"     /* for class DcmJSONReader */

#ifdef WITH_ZLIB
#include <zlib.h>                       /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "json2dcm"
#define OFFIS_CONSOLE_DESCRIPTION "Convert JSON document to DICOM file or data set"

/* Error codes */

#define EXITCODE_INVALID_JSON_CONTENT                   65
#define EXITCODE_BULKDATA_URI_NOT_SUPPORTED             66

static OFLogger json2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

void addInputOptions(OFCommandLine& cmd)
{
    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-meta-info",      "+f",   "read meta information if present (default)");
        cmd.addOption("--ignore-meta-info",    "-f",   "ignore file meta information");
}

void addProcessOptions(OFCommandLine& cmd)
{
    cmd.addGroup("processing options:");
      cmd.addSubGroup("unique identifiers:");
        cmd.addOption("--generate-new-uids",   "+Ug",    "generate new Study/Series/SOP Instance UID");
        cmd.addOption("--dont-overwrite-uids", "-Uo",    "do not overwrite existing UIDs (default)");
        cmd.addOption("--overwrite-uids",      "+Uo",    "overwrite existing UIDs");
      cmd.addSubGroup("bulkdata URI handling:");
        cmd.addOption("--parse-bulkdata-uri",  "+Bu",    "parse Bulkdata URIs (default)");
        cmd.addOption("--ignore-bulkdata-uri", "-Bu",    "ignore Bulkdata URIs");
        cmd.addOption("--add-bulkdata-dir",    "+Bd", 1, "[d]irectory: string",
                                                         "add d to list of permitted bulk data sources");
      cmd.addSubGroup("handling of arrays with multiple data sets:");
        cmd.addOption("--array-reject",        "-ar",    "reject multiple data sets (default)");
        cmd.addOption("--array-select",        "+as", 1, "[n]umber: integer",
                                                         "select data set n from array");
        cmd.addOption("--array-sequence",      "+ar",    "store all data sets in private sequence");
}

void addOutputOptions(OFCommandLine& cmd)
{
    cmd.addGroup("output options:");
      cmd.addSubGroup("output file format:");
        cmd.addOption("--write-file",          "+F",     "write file format (default)");
        cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
        cmd.addOption("--update-meta-info",    "+Fu",    "update particular file meta information");
      cmd.addSubGroup("output transfer syntax:");
        cmd.addOption("--write-xfer-same",     "+t=",    "write with same TS as input (default)");
        cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian TS");
        cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
        cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
        cmd.addOption("--write-xfer-deflated", "+td",    "write with deflated expl. VR little endian TS");
#endif
      cmd.addSubGroup("error handling:");
        cmd.addOption("--stop-on-error",       "-E",     "do not write if document is invalid (default)");
        cmd.addOption("--ignore-errors",       "+E",     "attempt to write even if document is invalid");
      cmd.addSubGroup("post-1993 value representations:");
        cmd.addOption("--enable-new-vr",       "+u",     "enable support for new VRs (UN/UT) (default)");
        cmd.addOption("--disable-new-vr",      "-u",     "disable support for new VRs, convert to OB");
      cmd.addSubGroup("length encoding in sequences and items:");
        cmd.addOption("--length-explicit",     "+e",     "write with explicit lengths (default)");
        cmd.addOption("--length-undefined",    "-e",     "write with undefined lengths");
      cmd.addSubGroup("charset handling:");
        cmd.addOption("--charset-accept",      "+c",     "write with the given charset in JSON (default)");
        cmd.addOption("--charset-replace",     "-c",     "replace the given charset in JSON with UTF-8");
#ifdef WITH_ZLIB
      cmd.addSubGroup("deflate compression level (only with --write-xfer-deflated):");
        cmd.addOption("--compression-level",   "+cl", 1, "[l]evel: integer (default: 6)",
                                                         "0=uncompressed, 1=fastest, 9=best compression");
#endif
}

#define SHORTCOL 3
#define LONGCOL 21

void addJSON2DCMCommandlineOptions(OFCommandLine& cmd)
{
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("jsonfile-in", "JSON input filename to be converted\n(\"-\" for stdin)");
    cmd.addParam("dcmfile-out", "DICOM output filename\n(\"-\" for stdout)");

    cmd.addGeneralOptions(LONGCOL, SHORTCOL);
    OFLog::addOptions(cmd);
    addInputOptions(cmd);
    addProcessOptions(cmd);
    addOutputOptions(cmd);
}

/**
 * function for parsing commandline arguments
 */
OFCondition parseArguments(
    OFConsoleApplication& app,
    OFCommandLine& cmd,
    OFBool& opt_generateUIDs,
    OFBool& opt_overwriteUIDs,
    E_TransferSyntax& opt_xfer,
    E_EncodingType& opt_enctype,
    OFBool& opt_replaceCharset,
    E_FileWriteMode& opt_writeMode,
    const char*& opt_ifname,
    const char*& opt_ofname,
    DcmJSONReader& jsonReader
)
{
    cmd.getParam(1, opt_ifname);
    cmd.getParam(2, opt_ofname);

    OFLog::configureFromCommandLine(cmd, app);

    /* input options */
    cmd.beginOptionBlock();
    if (cmd.findOption("--read-meta-info"))
        jsonReader.setIgnoreMetaInfoPolicy(OFFalse);
    if (cmd.findOption("--ignore-meta-info"))
        jsonReader.setIgnoreMetaInfoPolicy(OFTrue);
    cmd.endOptionBlock();

    /* processing options */
    if (cmd.findOption("--generate-new-uids"))
        opt_generateUIDs = OFTrue;

    cmd.beginOptionBlock();
    if (cmd.findOption("--dont-overwrite-uids"))
    {
        app.checkDependence("--dont-overwrite-uids", "--generate-new-uids", opt_generateUIDs);
        opt_overwriteUIDs = OFFalse;
    }
    if (cmd.findOption("--overwrite-uids"))
    {
        app.checkDependence("--overwrite-uids", "--generate-new-uids", opt_generateUIDs);
        opt_overwriteUIDs = OFTrue;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--ignore-bulkdata-uri"))
    {
        jsonReader.setIgnoreBulkdataURIPolicy(OFTrue);
    }
    if (cmd.findOption("--parse-bulkdata-uri"))
    {
        jsonReader.setIgnoreBulkdataURIPolicy(OFFalse);
    }
    cmd.endOptionBlock();

    if (cmd.findOption("--add-bulkdata-dir", 0, OFCommandLine::FOM_First))
    {
        OFCondition result;
        const char *current = NULL;
        do
        {
            app.checkValue(cmd.getValue(current));
            result = jsonReader.addPermittedBulkdataPath(current);
            if (result.bad()) return result;
        } while (cmd.findOption("--add-bulkdata-dir", 0, OFCommandLine::FOM_Next));
    }

    cmd.beginOptionBlock();
    if (cmd.findOption("--array-reject"))
    {
        jsonReader.setArrayHandlingPolicy(-1);
    }
    if (cmd.findOption("--array-sequence"))
    {
        jsonReader.setArrayHandlingPolicy(0);
    }
    if (cmd.findOption("--array-select"))
    {
        OFCmdSignedInt arrayHandling = -1;
        cmd.getValueAndCheckMin(arrayHandling, 1);
        jsonReader.setArrayHandlingPolicy(arrayHandling);
    }
    cmd.endOptionBlock();

    /* output options */
    cmd.beginOptionBlock();
    if (cmd.findOption("--write-file"))
        opt_writeMode = EWM_fileformat;
    if (cmd.findOption("--write-dataset"))
        opt_writeMode = EWM_dataset;
    cmd.endOptionBlock();

    if (cmd.findOption("--update-meta-info"))
    {
        app.checkConflict("--update-meta-info", "--write-dataset", opt_writeMode == EWM_dataset);
        opt_writeMode = EWM_updateMeta;
    }

    cmd.beginOptionBlock();
    if (cmd.findOption("--write-xfer-same"))
        opt_xfer = EXS_Unknown;
    if (cmd.findOption("--write-xfer-little"))
        opt_xfer = EXS_LittleEndianExplicit;
    if (cmd.findOption("--write-xfer-big"))
        opt_xfer = EXS_BigEndianExplicit;
    if (cmd.findOption("--write-xfer-implicit"))
        opt_xfer = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
    if (cmd.findOption("--write-xfer-deflated"))
        opt_xfer = EXS_DeflatedLittleEndianExplicit;
#endif
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--stop-on-error"))
        jsonReader.setStopOnErrorPolicy(OFTrue);
    if (cmd.findOption("--ignore-errors"))
        jsonReader.setStopOnErrorPolicy(OFFalse);
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--enable-new-vr"))
        dcmEnableGenerationOfNewVRs();
    if (cmd.findOption("--disable-new-vr"))
        dcmDisableGenerationOfNewVRs();
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--length-explicit"))
        opt_enctype = EET_ExplicitLength;
    if (cmd.findOption("--length-undefined"))
        opt_enctype = EET_UndefinedLength;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--charset-accept"))
        opt_replaceCharset = OFFalse;
    if (cmd.findOption("--charset-replace"))
        opt_replaceCharset = OFTrue;
    cmd.endOptionBlock();

#ifdef WITH_ZLIB
    if (cmd.findOption("--compression-level"))
    {
        OFCmdUnsignedInt comprLevel = 0;
        app.checkDependence("--compression-level", "--write-xfer-deflated", opt_xfer == EXS_DeflatedLittleEndianExplicit);
        app.checkValue(cmd.getValueAndCheckMinMax(comprLevel, 0, 9));
        dcmZlibCompressionLevel.set(OFstatic_cast(int, comprLevel));
    }
#endif

    return EC_Normal;
}

void generateUIDs(DcmItem *dataset, OFBool overwriteUIDs, E_FileWriteMode& writeMode)
{
    char uid[100];
    if (overwriteUIDs || !dataset->tagExistsWithValue(DCM_StudyInstanceUID))
    {
        OFLOG_INFO(json2dcmLogger, "generating new Study Instance UID");
        dataset->putAndInsertString(DCM_StudyInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
    }
    if (overwriteUIDs || !dataset->tagExistsWithValue(DCM_SeriesInstanceUID))
    {
        OFLOG_INFO(json2dcmLogger, "generating new Series Instance UID");
        dataset->putAndInsertString(DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
    }
    if (overwriteUIDs || !dataset->tagExistsWithValue(DCM_SOPInstanceUID))
    {
        OFLOG_INFO(json2dcmLogger, "generating new SOP Instance UID");
        dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));

        /* make sure that the file meta information is updated correspondingly */
        if (writeMode == EWM_fileformat)
            writeMode = EWM_updateMeta;
    }
}

void updateCharacterSet(DcmItem *dataset, const char *ifname, OFBool replaceCharset)
{
    OFString allowedCharset = "ISO_IR 192";
    OFString asciiCharset = "ISO_IR 6";
    const char *elemValue;
    OFCondition result = dataset->findAndGetString(DCM_SpecificCharacterSet, elemValue, OFTrue /*searchIntoSub*/);
    if (replaceCharset)
    {
        if (result.bad() || elemValue == NULL)
        {
            OFLOG_WARN(json2dcmLogger, "JSON file '" << ifname << "' does not specify a character set, it will be set to UTF-8 ('" << allowedCharset << "')");
            dataset->putAndInsertString(DCM_SpecificCharacterSet, allowedCharset.c_str());
        }
        else if (allowedCharset.compare(elemValue) != 0)
        {
            OFLOG_WARN(json2dcmLogger, "JSON file '" << ifname << "' specifies a character set other than UTF-8, it will be set to UTF-8 ('" << allowedCharset << "')");
            dataset->putAndInsertString(DCM_SpecificCharacterSet, allowedCharset.c_str());
        }
    }
    else
    {
        if ((result.bad() || elemValue == NULL))
        {
            if (dataset->containsExtendedCharacters())
            {
                // JSON dataset contains non-ASCII characters but no specific character set
                OFLOG_WARN(json2dcmLogger, "JSON file '" << ifname << "' does not specify a character set, the file is encoded in UTF-8 ('" << allowedCharset << "')");
            }
        }
        else if (allowedCharset.compare(elemValue) != 0)
        {
            // JSON dataset specifies a specific character set other then ISO_IR 192
            if (dataset->containsExtendedCharacters() || (asciiCharset.compare(elemValue) != 0))
            {
                // JSON dataset is not ASCII (ISO_IR 6) either
                OFLOG_WARN(json2dcmLogger, "JSON file '" << ifname << "' specifies the character set '"
                    << elemValue << "' but the file is encoded in UTF-8 ('" << allowedCharset << "')");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    OFBool opt_generateUIDs = OFFalse;
    OFBool opt_overwriteUIDs = OFFalse;
    E_TransferSyntax opt_xfer = EXS_Unknown;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    OFBool opt_replaceCharset = OFFalse;
    E_FileWriteMode opt_writeMode = EWM_fileformat;

    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;

    /* set-up command line parameters and options */
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;

    // add supported command line options
    addJSON2DCMCommandlineOptions(cmd);

    // evaluate command line
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
    }

    /* print resource identifier */
    OFLOG_DEBUG(json2dcmLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(json2dcmLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    DcmFileFormat fileformat;
    DcmJSONReader jsonReader;

    /* command line parameters and options */
    OFCondition result = parseArguments(
        app, cmd,
        opt_generateUIDs,
        opt_overwriteUIDs,
        opt_xfer,
        opt_enctype,
        opt_replaceCharset,
        opt_writeMode,
        opt_ifname,
        opt_ofname,
        jsonReader
    );

    if (result.bad())
        return EXITCODE_COMMANDLINE_SYNTAX_ERROR;

    // check filenames
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_ERROR(json2dcmLogger, OFFIS_CONSOLE_APPLICATION << ": invalid input filename: <empty string>");
        return EXITCODE_COMMANDLINE_SYNTAX_ERROR;
    }
    if ((opt_ofname == NULL) || (strlen(opt_ofname) == 0))
    {
        OFLOG_ERROR(json2dcmLogger, OFFIS_CONSOLE_APPLICATION << ": invalid output filename: <empty string>");
        return EXITCODE_COMMANDLINE_SYNTAX_ERROR;
    }

    // read JSON file and convert to DICOM
    OFLOG_INFO(json2dcmLogger, "reading JSON input file: " << opt_ifname);
    result = jsonReader.readAndConvertJSONFile(fileformat, opt_ifname);
    if (result.bad())
    {
        if (result == EC_BulkDataURINotSupported)
            return EXITCODE_BULKDATA_URI_NOT_SUPPORTED;
        else if (result == EC_InvalidFilename)
            return EXITCODE_CANNOT_READ_INPUT_FILE;
        else
            return EXITCODE_INVALID_JSON_CONTENT;
    }

    // generate new UIDs (if required)
    if (opt_generateUIDs)
    {
        generateUIDs(fileformat.getDataset(), opt_overwriteUIDs, opt_writeMode);
    }

    // check and update SpecificCharacterSet
    updateCharacterSet(fileformat.getDataset(), opt_ifname, opt_replaceCharset);

    // determine the transfer syntax for writing the file
    if (opt_xfer == EXS_Unknown)
    {
        /* use Explicit VR Little Endian as default if no transfer syntax is given otherwise */
        if (jsonReader.getTransferSyntax() == EXS_Unknown)
            opt_xfer = EXS_LittleEndianExplicit;
        else
            opt_xfer = jsonReader.getTransferSyntax();
    }

    // check whether it is possible to write the file in the requested transfer syntax
    if (fileformat.canWriteXfer(opt_xfer))
    {
        // encapsulated pixel data requires a DICOM file, in this case ignore --write-dataset
        if ((opt_writeMode == EWM_dataset) && DcmXfer(opt_xfer).usesEncapsulatedFormat())
        {
            OFLOG_WARN(json2dcmLogger, "encapsulated pixel data require file format, ignoring --write-dataset");
            opt_writeMode = EWM_fileformat;
        }

        OFLOG_INFO(json2dcmLogger, "writing DICOM output file: " << opt_ofname);
        result = fileformat.saveFile(opt_ofname, opt_xfer, opt_enctype, EGL_withoutGL, EPD_noChange,
            OFstatic_cast(Uint32, 0), OFstatic_cast(Uint32, 0), opt_writeMode);

        if (result.bad())
        {
            OFLOG_ERROR(json2dcmLogger, result.text() << ": writing file: " << opt_ofname);
            return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
        }
    }
    else
    {
        OFLOG_ERROR(json2dcmLogger, "no conversion to transfer syntax " << DcmXfer(opt_xfer).getXferName() << " possible!");
        return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
    }

    return 0;
}

