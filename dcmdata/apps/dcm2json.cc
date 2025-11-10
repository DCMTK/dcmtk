/*
 *
 *  Copyright (C) 2016-2025, OFFIS e.V.
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
 *  Author:  Sebastian Grallert, Marco Eichelberg
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
#include "dcmtk/ofstd/ofexit.h"
#include "dcmtk/ofstd/ofstd.h"

#include <cstdlib>

#ifdef WITH_ZLIB
#include <zlib.h>                       /* for zlibVersion() */
#endif
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
#include "dcmtk/ofstd/ofchrenc.h"       /* for OFCharacterEncoding */
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#define OFFIS_CONSOLE_APPLICATION "dcm2json"
#define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM file and data set to JSON"

#define EXITCODE_CANNOT_CONVERT_TO_UNICODE 80
#define EXITCODE_CANNOT_WRITE_VALID_JSON   81

#define DCM2JSON_WELL_KNOWN_UID "1.2.276.0.7230010.3.1.4.1787205428.3192777.1748004619.679033"

static OFLogger dcm2jsonLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

/** Convert a DICOM file to JSON.
 *  @param out output stream to write JSON to
 *  @param dfile pointer to the DICOM object, must not be NULL
 *  @param readMode mode used to read the file. Determines whether conversion
 *    takes place at DcmFileFormat or DcmDataset level.
 *  @param format OFTrue for pretty printing, OFFalse for compact code
 *  @param printMetaInfo If true, the meta-header attributes are also converted
 *     to JSON as a non-standard DCMTK extension
 *  @param encode_extended enables an extension of the JSON syntax to permit
 *     "-inf", "nan" and "inf" to be generated for infinity and not-a-number values
 *  @param opt_ns_policy policy for converting IS/DS values to JSON
 *  @param min_bulk_size minimum size for bulk data, negative number to disable bulk data
 *  @param bulk_uri_prefix prefix string for bulk data URIs
 *  @param bulk_dir directory to which bulk data should be written
 *  @return EC_Normal if successful, and error code otherwise
 */
static OFCondition writeFile(
    STD_NAMESPACE ostream &out,
    DcmFileFormat *dfile,
    const E_FileReadMode readMode,
    const OFBool format,
    const OFBool printMetaInfo,
    const OFBool encode_extended,
    const DcmJsonFormat::NumStringPolicy opt_ns_policy,
    const OFCmdSignedInt min_bulk_size,
    const char *bulk_uri_prefix,
    const char *bulk_dir)
{
    OFCondition result = EC_IllegalParameter;
    if (dfile != NULL)
    {
        /* write JSON document content */
        DcmDataset *dset = dfile->getDataset();
        if (format)
        {
            DcmJsonFormatPretty fmt(printMetaInfo);
            fmt.setJsonExtensionEnabled(encode_extended);
            fmt.setJsonNumStringPolicy(opt_ns_policy);
            fmt.setMinBulkSize(OFstatic_cast(ssize_t, min_bulk_size));
            fmt.setBulkURIPrefix(bulk_uri_prefix);
            fmt.setBulkDir(bulk_dir);
            if (readMode == ERM_dataset)
               result = dset->writeJsonExt(out, fmt, OFTrue, OFTrue);
               else result = dfile->writeJson(out, fmt);
        }
        else
        {
            DcmJsonFormatCompact fmt(printMetaInfo);
            fmt.setJsonExtensionEnabled(encode_extended);
            fmt.setJsonNumStringPolicy(opt_ns_policy);
            fmt.setMinBulkSize(OFstatic_cast(ssize_t, min_bulk_size));
            fmt.setBulkURIPrefix(bulk_uri_prefix);
            fmt.setBulkDir(bulk_dir);
            if (readMode == ERM_dataset)
               result = dset->writeJsonExt(out, fmt, OFTrue, OFTrue);
               else result = dfile->writeJson(out, fmt);
        }
    }
    return result;
}

/** append the given file path to the output URL while URL-encoding
 *  special characters. Note that the reserved characters '/' and
 *  ':' are not encoding since they are routinely used in file: URLs.
 *  @param path file path
 *  @param output_url output URL
 */
static void appendURLEncodedPath(const char *path, OFString& output_url)
{
  if (path)
  {
    char c;
    for (const char *p=path; *p != '\0'; ++p)
    {
      c = *p;
      // URL encode all characters except a-z, A-Z, 0-9, and "-_./!~$:"
      // The reserved characters "/" and ":" are not URL encoded because they routinely occur in file: URLs
      if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '/' || c == '!' || c == '~' || c == '$' || c == ':') output_url.append(1, *p);
      else if (c == '\\')
      {
          // convert backslashes to forward slashes
          output_url.append("/");
      }
      else
      {
          char buf[5];
          OFStandard::snprintf(buf, 5, "%%%02X", c);
          output_url.append(buf);
      }
    }
  }
}

/** determine the real path to the given working directory
 *  and return it in the form of a file:// URI.
 *  @param input_dir current working directory, NULL for current directory
 *  @param output_dir file URI to real path (without symbolic links) returned in this parameter if successful
 *  @return EC_Normal if successful, and error code otherwise
 */
static OFCondition getCurrentWorkingDir(const char *input_dir, OFString& output_dir)
{
    // if dir is empty, assume current directory
    if (input_dir == NULL) input_dir = ".";

#ifdef HAVE_WINDOWS_H
    char *resolved_path = _fullpath(NULL, input_dir, 0);
#else
    char *resolved_path = realpath(input_dir, NULL);
#endif
    if (resolved_path == NULL)
    {
        OFLOG_ERROR(dcm2jsonLogger, OFFIS_CONSOLE_APPLICATION << ": Cannot create or determine bulk data directory");
        return EC_DirectoryDoesNotExist;
    }
    output_dir = "file://localhost";
    if (resolved_path[0] != '/') output_dir.append("/");
    appendURLEncodedPath(resolved_path, output_dir);
    output_dir.append("/");
    free(resolved_path);
    return EC_Normal;
}

#define SHORTCOL 3
#define LONGCOL 20

int main(int argc, char *argv[])
{
    OFBool opt_format = OFTrue;
    OFBool opt_addMetaInformation = OFFalse;
    OFBool opt_encode_extended = OFFalse;
    DcmJsonFormat::NumStringPolicy opt_ns_policy = DcmJsonFormat::NSP_auto;

    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    OFString optStr;

    OFCmdSignedInt opt_min_bulk_size = -1;
    const char *opt_bulk_uri_prefix = NULL;
    const char *opt_bulk_dir = ".";
    OFBool opt_bulk_subdir = OFFalse;
    OFString bulkURIPrefix;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",   "DICOM input filename to be converted\n(\"-\" for stdin)", OFCmdParam::PM_Mandatory);
    cmd.addParam("jsonfile-out", "JSON output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                 "-h",  "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                     "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",          "+f",     "read file format or data set (default)");
        cmd.addOption("--read-file-only",     "+fo",    "read file format only");
        cmd.addOption("--read-dataset",       "-f",     "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",     "-t=",    "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",   "-td",    "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",   "-te",    "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",      "-tb",    "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit", "-ti",    "read with implicit VR little endian TS");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("encoding of infinity and not-a-number:");
        cmd.addOption("--encode-strict",      "-es",    "report error for 'inf' and 'nan' (default)");
        cmd.addOption("--encode-extended",    "-ee",    "permit 'inf' and 'nan' in JSON numbers");
      cmd.addSubGroup("encoding of IS and DS (integer/decimal string) elements:");
        cmd.addOption("--is-ds-auto",         "-ia",    "encode as number if valid, as string\notherwise (default)");
        cmd.addOption("--is-ds-num",          "-in",    "always encode as number, fail if invalid");
        cmd.addOption("--is-ds-string",       "-is",    "always encode as string");
      cmd.addSubGroup("bulk data URI options:");
        cmd.addOption("--bulk-disabled",      "-b",     "write everything as inline binary (default)");
        cmd.addOption("--bulk-enabled",       "+b",     "write large attributes as bulk data");
        cmd.addOption("--bulk-size",          "+bz", 1, "[s]ize: integer (default: 1)", "use bulk data for attributes >= s kBytes");
        cmd.addOption("--bulk-uri-prefix",    "+bp", 1, "[u]ri prefix: string",  "use prefix u when generating bulk data URIs\n(default: file URI)");
        cmd.addOption("--bulk-dir",           "+bd", 1, "[d]irectory: string",  "write bulk data files to d (default: '.')");
        cmd.addOption("--bulk-subdir",        "+bs",    "create subdirectory for each SOP instance\n(default: no subdirectory)");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output format:");
        cmd.addOption("--formatted-code",     "+fc",    "enable whitespace formatting (default)");
        cmd.addOption("--compact-code",       "-fc",    "print only required characters");
        cmd.addOption("--write-meta",         "+m",     "write data set with meta information\n(warning: not conforming to the DICOM standard)");

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

        /* number encoding options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--encode-strict"))
            opt_encode_extended = OFFalse;
        if (cmd.findOption("--encode-extended"))
            opt_encode_extended = OFTrue;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--is-ds-auto"))
            opt_ns_policy = DcmJsonFormat::NSP_auto;
        if (cmd.findOption("--is-ds-num"))
            opt_ns_policy = DcmJsonFormat::NSP_always_number;
        if (cmd.findOption("--is-ds-string"))
            opt_ns_policy = DcmJsonFormat::NSP_always_string;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--bulk-disabled"))
            opt_min_bulk_size = -1;
        if (cmd.findOption("--bulk-enabled"))
            opt_min_bulk_size = 1;
        cmd.endOptionBlock();

        if (cmd.findOption("--bulk-size"))
        {
            app.checkDependence("--bulk-size", "--bulk-enabled", opt_min_bulk_size >= 0);
            app.checkValue(cmd.getValueAndCheckMin(opt_min_bulk_size, 0));
        }

        if (cmd.findOption("--bulk-uri-prefix"))
        {
            app.checkDependence("--bulk-uri-prefix", "--bulk-enabled", opt_min_bulk_size >= 0);
            app.checkValue(cmd.getValue(opt_bulk_uri_prefix));
        }

        if (cmd.findOption("--bulk-dir"))
        {
            app.checkDependence("--bulk-dir", "--bulk-enabled", opt_min_bulk_size >= 0);
            app.checkValue(cmd.getValue(opt_bulk_dir));
        }

        if (cmd.findOption("--bulk-subdir"))
        {
            app.checkDependence("--bulk-subdir", "--bulk-enabled", opt_min_bulk_size >= 0);
            opt_bulk_subdir = OFTrue;
        }

        /* format options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--formatted-code"))
            opt_format = OFTrue;
        if (cmd.findOption("--compact-code"))
            opt_format = OFFalse;
        cmd.endOptionBlock();

        /* meta option */
        if (cmd.findOption("--write-meta"))
        {
            app.checkConflict("--write-meta", "--read-dataset", opt_readMode == ERM_dataset);
            opt_addMetaInformation = OFTrue;
        }
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcm2jsonLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcm2jsonLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    OFCondition status;
    if ((opt_min_bulk_size >= 0) && (opt_bulk_uri_prefix == NULL))
    {
       // try to create bulk directory, ignore failure
       (void) OFStandard::createDirectory(opt_bulk_dir, "");

        // determine default Bulk URI prefix
        status = getCurrentWorkingDir(opt_bulk_dir, bulkURIPrefix);
        if (status.bad())
        {
            return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
        }
        opt_bulk_uri_prefix = bulkURIPrefix.c_str();
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
        status = dfile.loadFile(ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
        if (status.good())
        {
            DcmDataset *dset = dfile.getDataset();
            /* check for SpecificCharacterSet on any data set level */
            if (dset->tagExistsWithValue(DCM_SpecificCharacterSet, OFTrue /*searchIntoSub*/))
            {
                /* convert entire DICOM file or data set to UTF-8 encoding */
                status = dfile.convertToUTF8();
                if (status.bad())
                {
                    OFLOG_FATAL(dcm2jsonLogger, status.text() << ": converting file to UTF-8: " << ifname);
                    result = EXITCODE_CANNOT_CONVERT_TO_UNICODE;
                }
            }
            else
            {
                if (dset->containsExtendedCharacters(OFFalse /*checkAllStrings*/))
                {
                    OFLOG_ERROR(dcm2jsonLogger, OFFIS_CONSOLE_APPLICATION << ": SpecificCharacterSet (0008,0005) element "
                        << "absent (at all levels of the data set) but extended characters used in file: " << ifname);
                    result = EXITCODE_CANNOT_CONVERT_TO_UNICODE;
                }
            }

            if (result == 0)
            {

                // look for the SOP instance UID, first in the dataset, then in the metaheader
                OFString subDir;
                OFString bulkDir = opt_bulk_dir;
                OFString bulkURIPrefixWithSubdir;
                if (opt_bulk_uri_prefix) bulkURIPrefixWithSubdir = opt_bulk_uri_prefix;
                if ((opt_min_bulk_size >= 0) && opt_bulk_subdir)
                {
                    if (dfile.getDataset()->findAndGetOFString(DCM_SOPInstanceUID, subDir).bad()
                       && dfile.getMetaInfo()->findAndGetOFString(DCM_SOPInstanceUID, subDir).bad())
                    {
                        // no SOP instance UID found. Use hard coded default UID instead.
                        subDir = DCM2JSON_WELL_KNOWN_UID;
                    }

                    bulkURIPrefixWithSubdir.append(subDir);
                    bulkURIPrefixWithSubdir.append("/");

                    bulkDir.append(1, PATH_SEPARATOR);
                    bulkDir.append(subDir);
                    if (! OFStandard::dirExists(bulkDir))
                    {
                        status = OFStandard::createDirectory(bulkDir, opt_bulk_dir);
                        if (status.bad())
                        {
                            OFLOG_FATAL(dcm2jsonLogger, status.text() << ": " << ifname);
                            return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
                        }
                    }
                }

                /* if second parameter is present, it is treated as the output filename ("stdout" otherwise) */
                if (cmd.getParamCount() == 2)
                {
                    const char *ofname = NULL;
                    cmd.getParam(2, ofname);
                    STD_NAMESPACE ofstream stream(ofname);
                    if (stream.good())
                    {
                        /* write content in JSON format to file */
                        status = writeFile(stream, &dfile, opt_readMode, opt_format, opt_addMetaInformation,
                            opt_encode_extended, opt_ns_policy, opt_min_bulk_size, bulkURIPrefixWithSubdir.c_str(), bulkDir.c_str());
                        if (status.bad())
                        {
                            OFLOG_FATAL(dcm2jsonLogger, status.text() << ": " << ifname);
                            result = EXITCODE_CANNOT_WRITE_VALID_JSON;
                        }
                    }
                    else
                        result = EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
                }
                else
                {
                    /* write content in JSON format to standard output */
                    status = writeFile(COUT, &dfile, opt_readMode, opt_format, opt_addMetaInformation,
                        opt_encode_extended, opt_ns_policy, opt_min_bulk_size, bulkURIPrefixWithSubdir.c_str(), bulkDir.c_str());
                    if (status.bad())
                    {
                        OFLOG_FATAL(dcm2jsonLogger, status.text() << ": " << ifname);
                        result = EXITCODE_CANNOT_WRITE_VALID_JSON;
                    }
                }
            }
        }
        else
        {
            OFLOG_ERROR(dcm2jsonLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") reading file: " << ifname);
            return EXITCODE_CANNOT_READ_INPUT_FILE;
        }
    }
    else
    {
        OFLOG_ERROR(dcm2jsonLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
        return EXITCODE_NO_INPUT_FILES;
    }
    return result;
}
