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
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcvrov.h"       /* for DcmOther64bitVeryLong */
#include "dcmtk/dcmdata/dcxfer.h"       /* for E_TransferSyntax */
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/ofstd/ofcond.h"         /* for class OFCondition */

#define JSMN_HEADER
#include "dcmtk/ofstd/ofjsmn.h"         /* for JSMN declarations  */

#include <climits>

#ifdef WITH_ZLIB
#include <zlib.h>                       /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "json2dcm"
#define OFFIS_CONSOLE_DESCRIPTION "Convert JSON document to DICOM file or data set"

/* Error codes */

#define EXITCODE_INVALID_JSON_CONTENT                   65
#define EXITCODE_BULKDATA_URI_NOT_SUPPORTED             66

static OFLogger json2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

typedef jsmn_parser OFJsmnParser;
typedef jsmntok_t OFJsmnToken;
typedef OFJsmnToken* OFJsmnTokenPtr;

class DcmFileFormat;
class DcmItem;
class DcmElement;
class DcmPixelSequence;
class DcmSequenceOfItems;
class DcmMetaInfo;
class DcmTagKey;
class DcmTag;

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
void addJSON2DCMommandlineOptions(OFCommandLine& cmd)
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
void parseArguments(OFConsoleApplication& app, OFCommandLine& cmd,
    OFBool& opt_metaInfo,
    OFBool& opt_generateUIDs,
    OFBool& opt_overwriteUIDs,
    OFBool& opt_ignoreBulkdataURI,
    OFBool& opt_stopOnErrors,
    E_TransferSyntax& opt_xfer,
    E_EncodingType& opt_enctype,
    OFBool& opt_replaceCharset,
    E_FileWriteMode& opt_writeMode,
    const char*& opt_ifname,
    const char*& opt_ofname
)
{
    cmd.getParam(1, opt_ifname);
    cmd.getParam(2, opt_ofname);

    OFLog::configureFromCommandLine(cmd, app);

    /* input options */
    cmd.beginOptionBlock();
    if (cmd.findOption("--read-meta-info"))
        opt_metaInfo = OFTrue;
    if (cmd.findOption("--ignore-meta-info"))
        opt_metaInfo = OFFalse;
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
        opt_ignoreBulkdataURI = OFTrue;
    }
    if (cmd.findOption("--parse-bulkdata-uri"))
    {
        opt_ignoreBulkdataURI = OFFalse;
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
    if (cmd.findOption("--stop-on-error")) opt_stopOnErrors = OFTrue;
    if (cmd.findOption("--ignore-errors")) opt_stopOnErrors = OFFalse;
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
}

/** parse the JSON file with the given filename
 *  @param fileformat DcmFileFormat instance to be populated with the parsed JSON content
 *  @param opt_ifname name of the JSON file to be read
 *  @param xfer transfer syntax in which the JSON file is read
 *  @param ignoreBulkdataURI whether to ignore or parse the bulk data URI if present
 *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
 *    otherwise ignore parse errors and continue
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition jsmnParse(
    DcmFileFormat& fileformat,
    const char *opt_ifname,
    OFBool& opt_metaInfo,
    E_TransferSyntax& xfer,
    OFBool ignoreBulkdataURI,
    const OFBool stopOnError
);

int main(int argc, char *argv[])
{
    OFBool opt_metaInfo = OFTrue;
    OFBool opt_generateUIDs = OFFalse;
    OFBool opt_overwriteUIDs = OFFalse;
    OFBool opt_ignoreBulkdataURI = OFFalse;
    OFBool opt_stopOnErrors = OFTrue;
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
    addJSON2DCMommandlineOptions(cmd);

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

    /* command line parameters and options */
    parseArguments(app, cmd,
        opt_metaInfo,
        opt_generateUIDs, opt_overwriteUIDs,
        opt_ignoreBulkdataURI,
        opt_stopOnErrors, opt_xfer, opt_enctype,
        opt_replaceCharset,
        opt_writeMode,
        opt_ifname, opt_ofname
    );

    /* print resource identifier */
    OFLOG_DEBUG(json2dcmLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(json2dcmLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    OFCondition result = EC_Normal;

    /* check filenames */
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

    if (result.good())
    {
       // create a DICOM file object
        DcmFileFormat fileformat;
        E_TransferSyntax xfer = EXS_LittleEndianExplicit;

        // read JSON file and convert to DICOM
        OFLOG_INFO(json2dcmLogger, "reading JSON input file: " << opt_ifname);
        result = jsmnParse(fileformat, opt_ifname, opt_metaInfo, xfer, opt_ignoreBulkdataURI, opt_stopOnErrors);

        if (result.bad())
        {
            if (result == EC_BulkDataURINotSupported)
                return EXITCODE_BULKDATA_URI_NOT_SUPPORTED;
            else if (result == EC_InvalidFilename)
                return EXITCODE_CANNOT_READ_INPUT_FILE;
            else
                return EXITCODE_INVALID_JSON_CONTENT;
        }

        DcmDataset* dataset = fileformat.getDataset();

        /* generate new UIDs (if required) */
        if (opt_generateUIDs)
        {
            char uid[100];
            if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_StudyInstanceUID))
            {
                OFLOG_INFO(json2dcmLogger, "generating new Study Instance UID");
                dataset->putAndInsertString(DCM_StudyInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
            }
            if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SeriesInstanceUID))
            {
                OFLOG_INFO(json2dcmLogger, "generating new Series Instance UID");
                dataset->putAndInsertString(DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
            }
            if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SOPInstanceUID))
            {
                OFLOG_INFO(json2dcmLogger, "generating new SOP Instance UID");
                dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
                /* make sure that the file meta information is updated correspondingly */
                if (opt_writeMode == EWM_fileformat)
                    opt_writeMode = EWM_updateMeta;
            }
        }

        OFString allowedCharset = "ISO_IR 192";
        OFString asciiCharset = "ISO_IR 6";
        const char *elemValue;
        result = dataset->findAndGetString(DCM_SpecificCharacterSet, elemValue, OFTrue /*searchIntoSub*/);
        if (opt_replaceCharset)
        {
            if (result.bad() || elemValue == NULL)
            {
                OFLOG_WARN(json2dcmLogger, "JSON file '" << opt_ifname << "' does not specify a character set, it will be set to UTF-8 ('" << allowedCharset << "')");
                dataset->putAndInsertString(DCM_SpecificCharacterSet, allowedCharset.c_str());
            }
            else if (allowedCharset.compare(elemValue) != 0)
            {
                OFLOG_WARN(json2dcmLogger, "JSON file '" << opt_ifname << "' specifies a character set other than UTF-8, it will be set to UTF-8 ('" << allowedCharset << "')");
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
                    OFLOG_WARN(json2dcmLogger, "JSON file '" << opt_ifname << "' does not specify a character set, the file is encoded in UTF-8 ('" << allowedCharset << "')");
                }
            }
            else if (allowedCharset.compare(elemValue) != 0)
            {
                // JSON dataset specifies a specific character set other then ISO_IR 192
                if (dataset->containsExtendedCharacters() || (asciiCharset.compare(elemValue) != 0))
                {
                    // JSON dataset is not ASCII (ISO_IR 6) either
                    OFLOG_WARN(json2dcmLogger, "JSON file '" << opt_ifname << "' specifies the character set '"
                        << elemValue << "' but the file is encoded in UTF-8 ('" << allowedCharset << "')");
                }
            }
        }

        OFLOG_INFO(json2dcmLogger, "writing DICOM output file: " << opt_ofname);

        /* determine transfer syntax to write the file */
        if (opt_xfer == EXS_Unknown)
        {
            /* use Explicit VR Little Endian as default if no transfer syntax is given otherwise */
            if (xfer == EXS_Unknown)
                opt_xfer = EXS_LittleEndianExplicit;
            else
                opt_xfer = xfer;
        }

        /* check whether it is possible to write the file */
        if (fileformat.canWriteXfer(opt_xfer))
        {
            /* check whether pixel data is encapsulated */
            if ((opt_writeMode == EWM_dataset) && DcmXfer(xfer).usesEncapsulatedFormat())
            {
                OFLOG_WARN(json2dcmLogger, "encapsulated pixel data require file format, ignoring --write-dataset");
                opt_writeMode = EWM_fileformat;
            }

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

    }
    return 0;
}

/** read the file and store the content in a char array
 *  @param iframe name of the JSON file to be read
 *  @param jsonStrLen length of the string
 *  @return jsonString upon success, NULL otherwise
 */
char *readFile(const char *opt_ifname, size_t& jsonStrLen)
{
    OFFile jsonFile;
    if (!(jsonFile.fopen(opt_ifname, "rb")))
    {
        OFLOG_ERROR(json2dcmLogger, "input file does not exist: " << opt_ifname);
        return NULL;
    }

    // obtain file size
    const size_t len = OFStandard::getFileSize(opt_ifname);
    char *jsonString = new (std::nothrow) char[len + 1];
    if (jsonString == NULL)
    {
        OFLOG_FATAL(json2dcmLogger, "out of memory: failed to allocate buffer");
        return NULL;
    }

    // read the whole file to buffer
    jsonString[len] = '\0';
    jsonStrLen = len;

    size_t res = jsonFile.fread(jsonString, 1, len);
    jsonFile.fclose();
    if (res != len)
    {
        OFLOG_FATAL(json2dcmLogger, "file read error: " << opt_ifname);
        delete[] jsonString;
        return NULL;
    }
    return jsonString;
}

// Begin with a 1 MByte buffer when reading from stdin.
// Buffer size will increase if necessary.
#define JSON2DCM_STDIN_BLOCKSIZE 1048576

/** read the JSON dataset from stdin and store the content in a char array
 *  @param jsonStrLen length of the string
 *  @return jsonString upon success, NULL otherwise
 */
char *readStdin(size_t& jsonStrLen)
{
    size_t bufSize = JSON2DCM_STDIN_BLOCKSIZE;
    size_t bufFree;
    size_t bytesInBuffer = 0;
    size_t readResult;
    char *jsonString = NULL;;
    char *oldBuf = NULL;
    char *writePtr;
    do
    {
        if (jsonString == NULL) jsonString = new (std::nothrow) char[bufSize+1];
        if (jsonString == NULL)
        {
            OFLOG_FATAL(json2dcmLogger, "out of memory: failed to allocate buffer");
            delete[] oldBuf;
            return NULL;
        }
        if (oldBuf)
        {
            memmove(jsonString, oldBuf, bytesInBuffer);
            delete[] oldBuf;
            oldBuf = NULL;
        }
        writePtr = jsonString + bytesInBuffer;
        bufFree = bufSize - bytesInBuffer;
        readResult = fread(writePtr, 1, bufFree, stdin);
        bytesInBuffer += readResult;
        bufFree -= readResult;
        if ((! feof(stdin)) && (bufFree == 0))
        {
            // we need more buffer. Double the size.
            oldBuf = jsonString;
            jsonString = NULL;
            bufSize *= 2;
        }
        else if (feof(stdin))
        {
            jsonString[bytesInBuffer] = '\0';
            jsonStrLen = bytesInBuffer;
            return jsonString;
        }
    } while (OFTrue);
}


/** calculate the required tokens number for the JSON string
 *  @param jsonString C string storing the file content
 *  @param jsonStrLen length of the C string
 *  @param tokenNum token array length, determined in reserveTokens
 *  @return reserved tokens upon success, NULL otherwise
 */
OFJsmnTokenPtr reserveTokens(
    OFJsmnParser& jsmnParser,
    char*& jsonString,
    const size_t& jsonStrLen,
    int& tokenNum
)
{
    tokenNum = jsmn_parse(&jsmnParser, jsonString, jsonStrLen, NULL, 0);
    jsmnParser.pos = 0;
    if (tokenNum <= 0) {
        return NULL;
    }

    // allocate one additional token
    OFJsmnTokenPtr tokenArray = new OFJsmnToken[tokenNum+1];

    // fill dummy token at the end of the array with values
    // that allow the loop in parseElement() to reliably terminate
    tokenArray[tokenNum].start = INT_MAX;
    tokenArray[tokenNum].end = INT_MAX;
    tokenArray[tokenNum].size = 0;
    return tokenArray;
}

/** parse the JSON string using JSMN parser
 *  @param jsonString C string storing the file content
 *  @param jsonStrLen length of the C string
 *  @param tokenArray JSMN token array to store the JSON content
 *  @param tokenNum token array length, determined in reserveTokens
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition parseJson(
    OFJsmnParser& jsmnParser,
    char*& jsonString,
    const size_t& jsonStrLen,
    OFJsmnTokenPtr& tokenArray, int& tokenNum
)
{
    if (tokenNum <= 0) return EC_IllegalParameter;

    int parsRes = jsmn_parse(&jsmnParser, jsonString, jsonStrLen, tokenArray, tokenNum);

    if (parsRes < 0)
    {
       // a parse error in jsmn_parse occurred
        OFLOG_ERROR(json2dcmLogger, "parse error in JSON file");
        if (parsRes == JSMN_ERROR_INVAL)
            return EC_InvalidCharacter;
            else return EC_CorruptedData;
    }
    return EC_Normal;
}

/** parse the dataset part of an XML file containing a DICOM file or a DICOM dataset.
 *  @param dataset dataset stored in this parameter
 *  @param metaheader metaheader stored in this parameter
 *  @param current pointer to current JSMN Token
 *  @param xfer transfer syntax in which the file is read
 *  @param ignoreBulkdataURI whether to ignore or parse the bulk data URI if present
 *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
 *    otherwise ignore parse errors and continue
 *  @param jsonString C string storing the file content
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition parseDataSet(
    DcmItem* dataset,
    DcmItem* metaheader,
    OFJsmnTokenPtr& current,
    E_TransferSyntax& xfer,
    OFBool ignoreBulkdataURI,
    const OFBool stopOnError,
    char*& jsonString
);

/** parse a DICOM sequence
 *  @param sequence DICOM Sequence
 *  @param current pointer to current token in the tokenArray
 *  @param xfer transfer syntax in which the file is read
 *  @param ignoreBulkdataURI whether to ignore or parse the bulk data URI if present
 *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
 *    otherwise ignore parse errors and continue
 *  @param jsonString C string storing the file content
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition parseSequence(
    DcmSequenceOfItems& sequence,
    OFJsmnTokenPtr& current,
    E_TransferSyntax& xfer,
    OFBool ignoreBulkdataURI,
    const OFBool stopOnError,
    char*& jsonString
);

/** parse the JSON file with the given filename
 *  @param fileformat DcmFileFormat instance to be populated with the parsed JSON content
 *  @param opt_ifname name of the JSON file to be read
 *  @param xfer transfer syntax in which the JSON file is read
 *  @param ignoreBulkdataURI whether to ignore or parse the bulk data URI if present
 *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
 *    otherwise ignore parse errors and continue
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition jsmnParse(
    DcmFileFormat& fileformat,
    const char *opt_ifname,
    OFBool& opt_metaInfo,
    E_TransferSyntax& xfer,
    OFBool ignoreBulkdataURI,
    const OFBool stopOnError
)
{
    OFJsmnParser jsmnParser;
    size_t jsonStrLen;
    int tokenNum;

    jsmn_init(&jsmnParser);

    DcmMetaInfo* metaheader = NULL;
    if (opt_metaInfo)
        metaheader = fileformat.getMetaInfo();
    DcmDataset* dataset = fileformat.getDataset();
    OFCondition result;

    // readin the input file to a memory buffer
    OFString stdinName("-");
    char *jsonString;
    if (opt_ifname == stdinName)
        jsonString = readStdin(jsonStrLen);
        else jsonString = readFile(opt_ifname, jsonStrLen);

    if (jsonString == NULL)
        return EC_InvalidFilename;

    // calculate tokens needed for the parser and allocate tokens
    OFJsmnTokenPtr tokenArray = reserveTokens(jsmnParser, jsonString, jsonStrLen, tokenNum);
    OFLOG_TRACE(json2dcmLogger, "JSMN tokens reserved: " << jsonStrLen);
    if (tokenArray == NULL)
    {
        delete[] jsonString;
        return EC_IllegalParameter;
    }

    // use the JSON library to parse the string and save it to the token array.
    result = parseJson(jsmnParser, jsonString, jsonStrLen, tokenArray, tokenNum);
    if (result.bad() && stopOnError)
    {
        delete[] jsonString;
        delete[] tokenArray;
        return result;
    }

    OFJsmnTokenPtr current = tokenArray;

    // parse dataset
    result = parseDataSet(dataset, metaheader, current, xfer, ignoreBulkdataURI, stopOnError, jsonString);
    delete[] jsonString;
    delete[] tokenArray;

    if (!stopOnError) result = EC_Normal;
    return result;
}


/** helper function to retrieve the content of the token.
 *  There is no check on boundaries!
 *  @param value the result string
 *  @param t token pointer
 *  @param jsonString C string storing the file content
 */
void getTokenContent(OFString& value, OFJsmnTokenPtr t, char*& jsonString)
{
    int size = t->end - t->start;

    // remember the character immediately following the token
    char c = jsonString[t->start+size];

    // convert the token string to a null terminated C string
    jsonString[t->start+size] = '\0';

    // copy token string into output parameter
    value = jsonString + t->start;

    // restore jsonString array
    jsonString[t->start+size] = c;
    return;
}

/** create DICOM element using the given tag and VR
 *  @param newElem DICOM element to be created
 *  @param dcmTag with the DICOM tag
 *  @param vr the given string representation of VR if present
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition createElement(
    DcmElement*& newElem, DcmTag& dcmTag, OFString vr
)
{
    OFCondition result = EC_Normal;

    OFLOG_TRACE(json2dcmLogger, "parsing VR: " << vr);
    /* convert vr string */
    const DcmVR dcmVR(vr.c_str());
    if (dcmVR.isUnknown() || dcmVR.isInvalid())
    {
        /* check whether "vr" attribute exists */
        if (vr.empty() || vr == "")
        {
            OFLOG_WARN(json2dcmLogger, "missing 'vr' attribute for " << dcmTag
                << ", using the tag's VR (" << dcmTag.getVR().getVRName() << ")");
        }
        else {
            OFLOG_WARN(json2dcmLogger, "invalid 'vr' attribute (" << vr << ") for " << dcmTag
                << ", using the tag's VR (" << dcmTag.getVR().getVRName() << ")");
        }
    }
    else {
        const DcmEVR dcmEVR = dcmVR.getEVR();
        const DcmEVR tagEVR = dcmTag.getEVR();
        /* check for correct vr */
        // normally, tagevr == dcmever or the tagevr is unknown
        if ((tagEVR != dcmEVR) && (tagEVR != EVR_UNKNOWN) &&
            // if its LUTData, the evr can be US, SS or OW
            ((dcmTag.getTagKey() != DCM_LUTData) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS) && (dcmEVR != EVR_OW))) &&
            // if the tag allows xs, the EVR can either be US or SS
            ((tagEVR != EVR_xs) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS))) &&
            // if the tag is ox or px, the EVR can either be OB or OW
            (((tagEVR != EVR_ox) && (tagEVR != EVR_px)) || ((dcmEVR != EVR_OB) && (dcmEVR != EVR_OW))))
        {
            // there are inconsistencies concerning the VR in the JSON file. There may be resulting errors in the DICOM dataset
            OFLOG_WARN(json2dcmLogger, "element " << dcmTag << " has wrong VR (" << dcmVR.getVRName()
                << "), correct is '" << dcmTag.getVR().getVRName() << "'");
        }
        dcmTag.setVR(dcmVR);
    }
    /* create DICOM element */
    result = DcmItem::newDicomElementWithVR(newElem, dcmTag);
    if (result.bad())
    {
        OFLOG_ERROR(json2dcmLogger, "failed to create DICOM element "  << dcmTag << ": " << result.text());
    }

    /* if this is pixel data, create an empty unencapsulated representation
     * to make sure that writing the element with empty value will not fail
     */
    if (dcmTag == DCM_PixelData) newElem->putUint16Array(NULL,0);
    return result;
}

/** extract DICOM tag from the given string
 *  @param keyToken token containing the string representation of the tag in form of "ggggeeee"
 *  @param tagkey stores the extracted DICOM tag
 *  @param jsonString C string storing the file content
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition extractTag(
    OFJsmnTokenPtr keyToken, DcmTagKey& tagkey,
    char*& jsonString
)
{
    OFString tagString;
    getTokenContent(tagString, keyToken, jsonString);
    if (tagString.empty() || tagString.size() != 8)
    {
        OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: expected attribute tag string with 8 characters, found '" << tagString << "'");
        return EC_InvalidTag;
    }
    unsigned long int group, element;
    OFString gStr = tagString.substr(0, 4);
    OFString eStr = tagString.substr(4);
    if (sscanf(gStr.c_str(), "%lx", &group) != 1
        || sscanf(eStr.c_str(), "%lx", &element) != 1)
    {
       OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: attribute tag must consist of two 16-bit hex numbers");
       return EC_InvalidTag;
    }

    tagkey.set(OFstatic_cast(Uint16, group), OFstatic_cast(Uint16, element));
    return EC_Normal;
}

/** interpreting given uint8 inline data as element specific data
 *  @param newElem where the value will be inserted
 *  @param data containing the inline data
 *  @param length of the data
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition parseElementInlineValue(
    DcmElement*& newElem, Uint8* data, const size_t length
)
{
    OFCondition result = EC_Normal;
    DcmEVR evr = newElem->getVR();
    size_t singleSize = sizeof(Uint8);
    if (data == NULL)
    {
        result = newElem->putUint8Array(data, OFstatic_cast(Uint32, length));
    }
    else if (evr == EVR_OW)
    {
        /* Base64 decoder produces big endian output data, convert to local byte order */
        singleSize = sizeof(Uint16);
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), singleSize);
        result = newElem->putUint16Array(OFreinterpret_cast(Uint16 *, data), OFstatic_cast(Uint32, length / singleSize));
    }
    else if (evr == EVR_OF)
    {
        singleSize = sizeof(Float32);
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), singleSize);
        result = newElem->putFloat32Array(OFreinterpret_cast(Float32 *, data), OFstatic_cast(Uint32, length / singleSize));
    }
    else if (evr == EVR_OD)
    {
        singleSize = sizeof(Float64);
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), singleSize);
        result = newElem->putFloat64Array(OFreinterpret_cast(Float64 *, data), OFstatic_cast(Uint32, length / singleSize));
    }
    else if (evr == EVR_OL)
    {
        singleSize = sizeof(Uint32);
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), singleSize);
        result = newElem->putUint32Array(OFreinterpret_cast(Uint32 *, data), OFstatic_cast(Uint32, length / singleSize));
    }
    else if (evr == EVR_OV)
    {
        singleSize = sizeof(Uint64);
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), singleSize);
        result = OFreinterpret_cast(DcmOther64bitVeryLong*, newElem)
                    ->putUint64Array(OFreinterpret_cast(Uint64 *, data), OFstatic_cast(Uint32, length / singleSize));
    }
    // OB, UN
    else if (evr == EVR_OB || evr == EVR_UN)
    {
        result = newElem->putUint8Array(data, OFstatic_cast(Uint32, length));
    }
    else
    {
        OFLOG_WARN(json2dcmLogger, "invalid VR Type for inline value");
        return EC_InvalidVR;
    }

    if (result.bad())
    {
        OFLOG_ERROR(json2dcmLogger, "failed to store inline binary value for DICOM element "  << newElem->getTag() << ": " << result.text());
    }
    return result;
}

/** static helper function processing escaped characters in JSON strings
 *  @param value containing the string. The string will be changed
 *  @return EC_Normal upon success, an error code otherwise
 */
static OFCondition processJSONEscapeCharacters(OFString& value)
{
    /*
        \" represents the quotation mark character (U+0022).
        \\ represents the reverse solidus character (U+005C).
        \/ represents the solidus character (U+002F).
        \b represents the backspace character (U+0008).
        \f represents the form feed character (U+000C).
        \n represents the line feed character (U+000A).
        \r represents the carriage return character (U+000D).
        \t represents the character tabulation character (U+0009).
    */
    OFString escapeSeq = "\"\\/bfnrt";

    // find backslash
    for (size_t backSlash = value.find('\\');
        backSlash < value.length();
        backSlash = value.find('\\', backSlash + 1))
    {
        if (value.length() < backSlash + 2)
        {
            // the given string is shorter than expected. just return the string
            OFLOG_ERROR(json2dcmLogger, "incomplete JSON escape sequence");
            return EC_InvalidJSONContent;
        }
        char sigChar = value[backSlash + 1];
        OFString front = value.substr(0, backSlash);
        OFString escString, back, replacement;
        unsigned int unicodeCodepoint = 0;

        if (escapeSeq.find(sigChar) < escapeSeq.length())
        {
            back = value.substr(backSlash + 2);
            escString = sigChar;

            switch (sigChar)
            {
            case 'b':
                unicodeCodepoint = 0x0008;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 'f':
                unicodeCodepoint = 0x000C;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 'n':
                unicodeCodepoint = 0x000A;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 'r':
                unicodeCodepoint = 0x000D;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 't':
                unicodeCodepoint = 0x0009;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            default: // \ / "
                replacement = sigChar;
                break;
            }
        }
        else
        {
            // does not contain the full pattern of "\uXXXX"
            if (sigChar != 'u' || value.length() < backSlash + 6)
            {
                // unknown escape sequence or incomplete unicode code point
                OFLOG_ERROR(json2dcmLogger, "unknown JSON escape sequence or incomplete Unicode code point");
                return EC_InvalidJSONContent;
            }

            escString = value.substr(backSlash + 2, 4);
            // parse escString to unicodeCodepoint
            if (sscanf(escString.c_str(), "%x", &unicodeCodepoint) != 1)
            {
                // Invalid hex code
                OFLOG_ERROR(json2dcmLogger, "invalid hex code in JSON escape sequence");
                return EC_InvalidJSONContent;
            }

            // convert Unicode codepoint to UTF-8 string
            if (unicodeCodepoint < 0x80)
            {
                // UTF-8 sequence with one byte
                replacement = OFstatic_cast(char, unicodeCodepoint);
                back = value.substr(backSlash + 6);
            }
            else if (unicodeCodepoint < 0x800)
            {
                // UTF-8 sequence with two bytes
                replacement = OFstatic_cast(char, ((unicodeCodepoint >> 6) & 0x1F) | 0xC0);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 0) & 0x3F) | 0x80);
                back = value.substr(backSlash + 6);
            }
            else if ((unicodeCodepoint >= 0xD800) && ((unicodeCodepoint < 0xE000)))
            {
                // UTF-16 surrogate pair consisting two Unicode code points
                if ((value.length() < backSlash + 12) || (value[backSlash + 6] != '\\') || (value[backSlash + 7] != 'u'))
                {
                    OFLOG_ERROR(json2dcmLogger, "invalid JSON UTF-16 surrogate pair escape sequence");
                    return EC_InvalidJSONContent;
                }

                escString = value.substr(backSlash + 8, 4);
                unsigned int unicodeCodepoint2;
                if (sscanf(escString.c_str(), "%x", &unicodeCodepoint2) != 1)
                {
                    // Invalid hex code
                    OFLOG_ERROR(json2dcmLogger, "invalid hex code in JSON escape sequence");
                    return EC_InvalidJSONContent;
                }

                // combine the two UTF-16 surrogates to a single Unicode code point.
                // The first code point is the high surrogate (D800..DBFF), the second one the low surrogate (DC00..DFFF)
                unicodeCodepoint = ((unicodeCodepoint - 0xD800) << 10) + unicodeCodepoint2 - 0xDC00 + 0x10000;
                // UTF-8 sequence with four bytes
                replacement = OFstatic_cast(char, ((unicodeCodepoint >> 18) & 0x07) | 0xF0);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 12) & 0x3F) | 0x80);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 6) & 0x3F) | 0x80);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 0) & 0x3F) | 0x80);
                back = value.substr(backSlash + 12);
            }
            else
            {
                // UTF-8 sequence with three bytes
                replacement = OFstatic_cast(char, ((unicodeCodepoint >> 12) & 0x0F) | 0xE0);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 6) & 0x3F) | 0x80);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 0) & 0x3F) | 0x80);
                back = value.substr(backSlash + 6);
            }
        }
        OFLOG_TRACE(json2dcmLogger, "the escaped string [" << escString << "] is parsed to UTF-8:"
            << replacement << " - unicode:" << std::hex << unicodeCodepoint);
        value = front + replacement + back;
    }
    return EC_Normal;
}

static OFString PersonGroupNames[] = { "Alphabetic", "Ideographic", "Phonetic" };
/** parse PN value
 *  @param value string containing the PN DICOM value
 *  @param delimiter PN-delimiter between different PN representations, normally '='
 *  @param current pointer to current token in the tokenArray
 *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
 *    otherwise ignore parse errors and continue
 *  @param jsonString C string storing the file content
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition parsePNValue(
    OFString& value, OFJsmnTokenPtr& current, const char delimiter,
    const OFBool stopOnError,
    char*& jsonString
)
{
    OFCondition result;
    int size = current->size;
    if (size > 3)
    {
        OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: PN must have at most three component groups");
        return EC_InvalidJSONType;
    }

    OFVector<OFString> pn(3);
    for (int i = 0; i < size; i++)
    {
        current++;
        if (current->type != JSMN_STRING)
        {
            OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: PN values must be JSON strings");
            return EC_InvalidJSONType;
        }

        OFString key;
        getTokenContent(key, current, jsonString);
        int index = -1;
        for (int j = 0; j < 3; j++)
        {
            if (key == PersonGroupNames[j])
            {
                index = j;
                break;
            }
        }
        if (index < 0)
        {

            OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: unsupported PN component group type '" << key << "'");
            return EC_InvalidJSONType;
        }

        // if pn[index] is not empty, it will be overwritten. May be
        current++;
        if (current->type != JSMN_STRING)
        {
            OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: PN values must be JSON strings");
            return EC_InvalidJSONType;
        }

        getTokenContent(pn[index], current, jsonString);

        OFLOG_TRACE(json2dcmLogger, "person name (PN) with " << key << " val " << pn[index]);
        result = processJSONEscapeCharacters(pn[index]);
        if (result.bad())
        {
            if (stopOnError) return result; else result = EC_Normal;
        }
    }
    // PN format is: "alphPN=ideoPN=phonPN"
    if (!pn[0].empty())
        value += pn[0];
    if (!pn[1].empty() || !pn[2].empty())
        value += delimiter + pn[1];
    if (!pn[2].empty())
        value += delimiter + pn[2];
    OFLOG_TRACE(json2dcmLogger, "PN value " << value);
    return result;
}

/** parse a value array from JSON to DICOM
 *  @param newElem Pointer to element where the value should be stored
 *  @param current pointer to current token in the tokenArray
 *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
 *    otherwise ignore parse errors and continue
 *  @param jsonString C string storing the file content
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition parseElemValueArray(
    DcmElement*& newElem, OFJsmnTokenPtr& current,
    const OFBool stopOnError,
    char*& jsonString
)
{
    OFCondition result;
    int elemVM = current->size;

    OFString vmString;
    const DcmVR dcmVR = newElem->getTag().getVR();

    char delimiter = dcmVR.getDelimiterChars()[0];
    if (delimiter == 0)
        delimiter = '\\';

    OFString value;
    for (int count = 0; count < elemVM; count++)
    {
        value.clear();
        current++;
        if (newElem->ident() == EVR_PN)
        {
            OFString tokenValue;
            getTokenContent(tokenValue, current, jsonString);
            OFLOG_TRACE(json2dcmLogger, "element value array, parsing PN value: " << tokenValue);
            if (current->type != JSMN_OBJECT)
            {
                if (tokenValue == "null")
                {
                    value = "";
                }
                else
                {
                    OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: PN components must be JSON strings or null");
                    return EC_InvalidJSONType;
                }
            }
            else
            {
                result = parsePNValue(value, current, newElem->getTag().getVR().getDelimiterChars()[2], stopOnError, jsonString);
                if (result.bad())
                {
                    if (stopOnError) return result; else result = EC_Normal;
                }
            }
            if (count > 0)
                vmString += delimiter;
            vmString += value;
        }
        else if (newElem->ident() == EVR_AT)
        {//  "(0008,0020)\(0008,0030)"
            if (current->type != JSMN_STRING)
            {
                OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: AT values must be JSON strings");
                return EC_InvalidJSONType;
            }

            DcmTagKey tagkey;
            result = extractTag(current, tagkey, jsonString);
            if (result.bad())
            {
                if (stopOnError) return result; else result = EC_Normal;
            }
            OFLOG_TRACE(json2dcmLogger, "element value array, parsing AT value: " << tagkey.toString());

            if (count > 0)
                vmString += delimiter;
            vmString += tagkey.toString();
        }
        else
        {
            if (current->type != JSMN_PRIMITIVE && current->type != JSMN_STRING)
            {
                OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: expected string value or null");
                return EC_InvalidJSONType;
            }

            getTokenContent(value, current, jsonString);
            if (current->type & JSMN_STRING)
            {
                OFLOG_TRACE(json2dcmLogger, "element value array, parsing string value: " << value);
                result = processJSONEscapeCharacters(value);
                if (result.bad())
                {
                    if (stopOnError) return result; else result = EC_Normal;
                }
            }
            else /* PRIMITIVE */
            {
                if (value == "null") {
                    value = "";
                }
            }
            if (count > 0)
                vmString += delimiter;
            vmString += value;
            OFLOG_TRACE(json2dcmLogger, "element value array: all values: " << vmString);
        }
    }

    result = newElem->putOFStringArray(vmString);
    if (result.bad())
    {
       OFLOG_ERROR(json2dcmLogger, "failed to store string value for element " << newElem->getTag() << ": " << result.text());
    }
    return result;
}

/** parse a DICOM element that is not a sequence
 *  @param dataset dataset stored in this parameter
 *  @param metaheader metaheader stored in this parameter
 *  @param current pointer to current JSMN Token
 *  @param xfer transfer syntax in which the file is read
 *  @param ignoreBulkdataURI whether to ignore or parse the bulk data URI if present
 *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
 *    otherwise ignore parse errors and continue
 *  @param jsonString C string storing the file content
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition parseElement(
    DcmItem* dataset,
    DcmItem* metaheader,
    OFJsmnTokenPtr& current,
    E_TransferSyntax& xfer,
    OFBool ignoreBulkdataURI,
    const OFBool stopOnError,
    char*& jsonString
)
{
    OFCondition result = EC_Normal;
    OFLOG_TRACE(json2dcmLogger, "element at " << current->start);

    // the key for the element tag has to be a string
    if (current->type != JSMN_STRING)
    {
        OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: element tag must be a JSON string");
        return EC_InvalidJSONType;
    }

    DcmTagKey tagkey;
    result = extractTag(current, tagkey, jsonString);
    if (result.bad())
    {
        if (stopOnError) return result; else result = EC_Normal;
    }
    DcmTag dcmTag(tagkey);

    // content
    current++;
    // this token contains the whole Content Object.
    // the content has to be a object
    if (current->type != JSMN_OBJECT)
    {
        OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: element content must be a JSON object");
        return EC_InvalidJSONType;
    }

    // count of the attributes
    int contentSize = current->size;
    DcmElement* newElem = NULL;
    current++;

    OFJsmnTokenPtr vrToken = NULL, valueToken = NULL;
    OFString valueType;
    for (int count = 0; count < contentSize; count++)
    {
        OFString attrName;
        getTokenContent(attrName, current, jsonString);
        attrName = OFStandard::toLower(attrName);
        OFLOG_TRACE(json2dcmLogger, "attribute '" << attrName << "' token at " << current->start << " - " << current->end);
        current++;

        if (attrName == "vr")
        {
            if (vrToken == NULL)
                vrToken = current;
            else
                OFLOG_WARN(json2dcmLogger, "attribute '" << attrName << " already present in this JSON object. This token will be ignored");
        }
        else
        {
            if (attrName == "bulkdatauri" || attrName == "inlinebinary" || attrName == "value")
            {
                if (valueToken == NULL)
                {
                    valueToken = current;
                    valueType = attrName;
                }
                else
                    OFLOG_WARN(json2dcmLogger, "attribute '" << attrName << " already present in this JSON object. This token will be ignored");
            }
            else
            {
                OFLOG_ERROR(json2dcmLogger, "unknown JSON attribute name \"" << attrName << "\"");
                return EC_InvalidJSONContent;
            }
        }
        // find the next token in the current hierarchie
        OFJsmnTokenPtr tmpToken = current;
        while (current->start < tmpToken->end)
            current++;
    }

    OFString vr = "";
    if (vrToken != NULL)
        getTokenContent(vr, vrToken, jsonString);

    /* create DICOM element */
    result = createElement(newElem, dcmTag, vr);
    if (result.bad())
    {
        if (stopOnError) return result; else result = EC_Normal;
    }

    if (valueToken == NULL) // no content following, input empty value
    {
        OFLOG_TRACE(json2dcmLogger, "no value token for element " << dcmTag << ", using empty value");
    }

    /* bulk data content is stored in a file. this is not yet supported */
    else if (valueType == "bulkdatauri")
    {
        if (ignoreBulkdataURI)
        {
            // leave the element with BulkdataURI empty
            OFLOG_INFO(json2dcmLogger, "ignoring BulkdataURI for element: " << dcmTag << ", leaving element empty");
        }
        else
        {
            OFLOG_ERROR(json2dcmLogger, "loading Bulkdata from 'BulkDataURI' not yet possible");
            delete newElem;
            return EC_BulkDataURINotSupported;
        }
    }

    /* inlinebinary - content is base64 encoded */
    else if (valueType == "inlinebinary")
    {
        // the binary value has to be a string
        if (valueToken->type != JSMN_STRING)
        {
            OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: InlineBinary value must be a JSON string");
            delete newElem;
            return EC_InvalidJSONType;
        }

        OFString value;
        getTokenContent(value, valueToken, jsonString);
        Uint8* data = NULL;
        const size_t length = OFStandard::decodeBase64(value, data);
        OFLOG_TRACE(json2dcmLogger, "parsing inline binary (" << length << "): " << value << " | " << data);
        if (length > 0)
            result = parseElementInlineValue(newElem, data, length);

        /* delete buffer since data is copied into the element */
        delete[] data;
        if (result.bad())
        {
            if (stopOnError) return result; else result = EC_Normal;
        }
    }
    else if (valueType == "value")
    {

        if (valueToken->type != JSMN_ARRAY)
        {
            // the value of the element has to be an array
            OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: attribute value must be a JSON array");
            delete newElem;
            return EC_InvalidJSONType;
        }

        /* Sequence */
        if (newElem->ident() == EVR_SQ)
        {
            result = parseSequence(*(OFstatic_cast(DcmSequenceOfItems*, newElem)),
                                    valueToken, xfer, ignoreBulkdataURI,
                                    stopOnError, jsonString);
        }/* special handling for compressed pixel data */
        else if (newElem->getTag() == DCM_PixelData)
        {
            OFLOG_ERROR(json2dcmLogger, "pixel data not allowed as value in DICOM JSON model");
            delete newElem;
            return EC_InvalidJSONContent;
        }
        else // interpret value array
        {
            OFLOG_TRACE(json2dcmLogger, "parsing values array of size " << valueToken->size);
            result = parseElemValueArray(newElem, valueToken, stopOnError, jsonString);
        }
        if (result.bad())
        {
            if (stopOnError) return result; else result = EC_Normal;
        }
    }
    else
    {
        OFLOG_ERROR(json2dcmLogger, "unknown JSON attribute name: " << valueType);
        delete newElem;
        return EC_InvalidJSONContent;
    }

    if (result.good())
    {
        // insert the new attribute to the metaheader if the tag is (0002,xxxx), otherwise to the dataset
        if (dcmTag.getGroup() == 0x0002 && metaheader != NULL)
        {
            // if we have found (0002,0010), extract the transfer syntax
            if (dcmTag.getElement() == 0x0010) {
                OFString value;
                newElem->getOFString(value, 0);
                xfer = DcmXfer(value.c_str()).getXfer();
            }

            result = metaheader->insert(newElem, OFFalse /*replaceOld*/);
        }
        else
        {
            result = dataset->insert(newElem, OFFalse /*replaceOld*/);
        }
        if (result.bad())
        {
            OFLOG_WARN(json2dcmLogger, "element " << dcmTag << " found twice in one data set or item, ignoring second entry");
            delete newElem;
        }
    }
    else // result.bad()
    {
        /* delete element if insertion or putting the value failed */
        delete newElem;
    }
    return result;
}

OFCondition parseSequence(
    DcmSequenceOfItems& sequence,
    OFJsmnTokenPtr& current,
    E_TransferSyntax& xfer,
    OFBool ignoreBulkdataURI,
    const OFBool stopOnError,
    char*& jsonString
)
{
    OFCondition result = EC_Normal;
    int sqSize = current->size;
    int sqStart = current->start;

    OFLOG_TRACE(json2dcmLogger, "sequence start: " << sqStart << " with size: " << sqSize);
    current++;
    // iterate over sequence items
    for (int i = 0; i < sqSize; i++)
    {
        OFLOG_TRACE(json2dcmLogger, "item " << sqStart << ":" << i << " -- " << current->start);
        if (current->type != JSMN_OBJECT)
            // the Items in a Sequence has to be an object
            return EC_InvalidJSONType;
        /* create new sequence item */
        DcmItem* newItem = new DcmItem();
        if (newItem != NULL)
        {
            sequence.insert(newItem);
            /* proceed parsing the item content */
            result = parseDataSet(newItem, NULL, current, xfer, ignoreBulkdataURI, stopOnError, jsonString);
            if (result.bad() && stopOnError) return result;
        }
        OFLOG_TRACE(json2dcmLogger, "item " << sqStart << ":" << i << " end, next up : " << current->start);
    }
    OFLOG_TRACE(json2dcmLogger, "sequence end: " << sqStart << "; next element: " << current->start);
    return result;
}

OFCondition parseDataSet(
    DcmItem* dataset,
    DcmItem* metaheader,
    OFJsmnTokenPtr& current,
    E_TransferSyntax& xfer,
    OFBool ignoreBulkdataURI,
    const OFBool stopOnError,
    char*& jsonString
)
{
    OFCondition result = EC_Normal;

    // we expext a JSON object as the whole content object
    if (current->type != JSMN_OBJECT)
    {
        OFLOG_ERROR(json2dcmLogger, "not a valid DICOM JSON dataset: datasets must be encapsulated in a JSON object");
        return EC_InvalidJSONType;
    }
    int dsSize = current->size;
    int dsStart = current->start;

    OFLOG_TRACE(json2dcmLogger, "dataset start " << dsStart << " - size: " << dsSize);

    current++;
    for (int i = 0; i < dsSize; i++)
    {
        // read each entry in the content object as a DICOM element
        result = parseElement(dataset, metaheader, current, xfer, ignoreBulkdataURI, stopOnError, jsonString);
        if (result.bad() && stopOnError) return result;
    }
    OFLOG_TRACE(json2dcmLogger, "dataset end " << dsStart << "; next element: " << current->start);
    return result;
}
