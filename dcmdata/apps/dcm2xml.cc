/*
 *
 *  Copyright (C) 2002-2019, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Convert the contents of a DICOM file to XML format
 *
 */


#include "dcmtk/config/osconfig.h"      /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"

#ifdef WITH_ZLIB
#include <zlib.h>                       /* for zlibVersion() */
#endif
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
#include "dcmtk/ofstd/ofchrenc.h"       /* for OFCharacterEncoding */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcm2xml"
#define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM file and data set to XML"

#define DOCUMENT_TYPE_DEFINITION_FILE "dcm2xml.dtd"

static OFLogger dcm2xmlLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

static OFCondition writeFile(STD_NAMESPACE ostream &out,
                             const char *ifname,
                             DcmFileFormat *dfile,
                             const E_FileReadMode readMode,
                             const OFBool loadIntoMemory,
                             const char *dtdFilename,
                             const char *defaultCharset,
                             /*const*/ size_t writeFlags,
                             const OFBool checkAllStrings)
{
    OFCondition result = EC_IllegalParameter;
    if ((ifname != NULL) && (dfile != NULL))
    {
        DcmDataset *dset = dfile->getDataset();
        if (loadIntoMemory)
            dset->loadAllDataIntoMemory();
        /* determine dataset character encoding */
        OFString encString;
        OFString csetString;
        if (dset->findAndGetOFStringArray(DCM_SpecificCharacterSet, csetString).good())
        {
            if (csetString == "ISO_IR 6")   // should never be present in a dataset, but ...
                encString = "UTF-8";
            else if (csetString == "ISO_IR 192")
                encString = "UTF-8";
            else if (csetString == "ISO_IR 100")
                encString = "ISO-8859-1";
            else if (csetString == "ISO_IR 101")
                encString = "ISO-8859-2";
            else if (csetString == "ISO_IR 109")
                encString = "ISO-8859-3";
            else if (csetString == "ISO_IR 110")
                encString = "ISO-8859-4";
            else if (csetString == "ISO_IR 148")
                encString = "ISO-8859-9";
            else if (csetString == "ISO_IR 144")
                encString = "ISO-8859-5";
            else if (csetString == "ISO_IR 127")
                encString = "ISO-8859-6";
            else if (csetString == "ISO_IR 126")
                encString = "ISO-8859-7";
            else if (csetString == "ISO_IR 138")
                encString = "ISO-8859-8";
            else {
                if (!csetString.empty())
                {
                    OFLOG_WARN(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": SpecificCharacterSet (0008,0005) "
                        << "value '" << csetString << "' not supported ... quoting non-ASCII characters");
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
                    OFLOG_DEBUG(dcm2xmlLogger, "using option --convert-to-utf8 to convert the DICOM file to "
                        "UTF-8 encoding might also help to solve this problem more appropriately");
#endif
                }
                /* make sure that non-ASCII characters are quoted appropriately */
                writeFlags |= DCMTypes::XF_convertNonASCII;
            }
        } else {
            /* SpecificCharacterSet is not present in the dataset */
            if (dset->containsExtendedCharacters(checkAllStrings))
            {
                if (defaultCharset == NULL)
                {
                    /* the dataset contains non-ASCII characters that really should not be there */
                    OFLOG_ERROR(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": SpecificCharacterSet (0008,0005) "
                        << "element absent (on the main data set level) but extended characters used in file: " << ifname);
                    OFLOG_DEBUG(dcm2xmlLogger, "use option --charset-assume to manually specify an appropriate character set");
                    return makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, "Missing Specific Character Set");;
                } else {
                    OFString sopClass;
                    csetString = defaultCharset;
                    /* use the default character set specified by the user */
                    if (csetString == "ISO_IR 192")
                        encString = "UTF-8";
                    else if (csetString == "ISO_IR 100")
                        encString = "ISO-8859-1";
                    else if (csetString == "ISO_IR 101")
                        encString = "ISO-8859-2";
                    else if (csetString == "ISO_IR 109")
                        encString = "ISO-8859-3";
                    else if (csetString == "ISO_IR 110")
                        encString = "ISO-8859-4";
                    else if (csetString == "ISO_IR 148")
                        encString = "ISO-8859-9";
                    else if (csetString == "ISO_IR 144")
                        encString = "ISO-8859-5";
                    else if (csetString == "ISO_IR 127")
                        encString = "ISO-8859-6";
                    else if (csetString == "ISO_IR 126")
                        encString = "ISO-8859-7";
                    else if (csetString == "ISO_IR 138")
                        encString = "ISO-8859-8";
                    else {
                        OFLOG_FATAL(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": Character set '"
                            << defaultCharset << "' specified with option --charset-assume not supported");
                        return makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, "Cannot select character set");
                    }
                    /* check whether this file is a DICOMDIR */
                    if (dfile->getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass).bad() ||
                        (sopClass != UID_MediaStorageDirectoryStorage))
                    {
                        OFLOG_INFO(dcm2xmlLogger, "inserting SpecificCharacterSet (0008,0005) element with value '"
                            << defaultCharset << "'");
                        /* insert the SpecificCharacterSet (0008,0005) element */
                        dset->putAndInsertString(DCM_SpecificCharacterSet, defaultCharset);
                    }
                }
            } else {
                if (defaultCharset != NULL)
                {
                    /* use "debug" instead of "warn" in order to avoid too much output in default mode */
                    OFLOG_DEBUG(dcm2xmlLogger, "ignoring character set '" << defaultCharset
                        << "' specified with option --charset-assume since it is not needed for this data set");
                }
                /* by default, we use UTF-8 encoding */
                encString = "UTF-8";
            }
        }

        /* write XML document header */
        out << "<?xml version=\"1.0\"";
        /* optional character set */
        if (!encString.empty())
            out << " encoding=\"" << encString << "\"";
        out << "?>" << OFendl;
        /* add document type definition (DTD) */
        if (writeFlags & DCMTypes::XF_addDocumentType)
        {
            out << "<!DOCTYPE ";
            if (readMode == ERM_dataset)
               out << "data-set";
            else
               out << "file-format";
            /* embed DTD */
            if (writeFlags & DCMTypes::XF_embedDocumentType)
            {
                out << " [" << OFendl;
                /* copy content from DTD file */
#if defined(HAVE_IOS_NOCREATE) && !defined(HAVE_CXX11)
                STD_NAMESPACE ifstream dtdFile(dtdFilename, STD_NAMESPACE ios::in | STD_NAMESPACE ios::nocreate);
#else
                STD_NAMESPACE ifstream dtdFile(dtdFilename, STD_NAMESPACE ios::in);
#endif
                if (dtdFile)
                {
                    char c;
                    /* copy all characters */
                    while (dtdFile.get(c))
                        out << c;
                } else {
                    OFLOG_WARN(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": cannot open DTD file: " << dtdFilename);
                }
                out << "]";
            } else { /* reference DTD */
                out << " SYSTEM \"" << DOCUMENT_TYPE_DEFINITION_FILE << "\"";
            }
            out << ">" << OFendl;
        }
        /* write XML document content */
        if (readMode == ERM_dataset)
            result = dset->writeXML(out, writeFlags);
        else
            result = dfile->writeXML(out, writeFlags);
    }
    return result;
}


#define SHORTCOL 4
#define LONGCOL 20


int main(int argc, char *argv[])
{
    size_t opt_writeFlags = 0;
    OFBool opt_loadIntoMemory = OFFalse;
    OFBool opt_checkAllStrings = OFFalse;
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    OFBool opt_convertToUTF8 = OFFalse;
#endif
    const char *opt_defaultCharset = NULL;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    OFCmdUnsignedInt opt_maxReadLength = 4096; // default is 4 KB
    const char *opt_dtdFilename = DEFAULT_SUPPORT_DATA_DIR DOCUMENT_TYPE_DEFINITION_FILE;
    OFString optStr;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",  "DICOM input filename to be converted", OFCmdParam::PM_Mandatory);
    cmd.addParam("xmlfile-out", "XML output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                 "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
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
      cmd.addSubGroup("long tag values:");
        cmd.addOption("--load-all",           "+M",     "load very long tag values (e.g. pixel data)");
        cmd.addOption("--load-short",         "-M",     "do not load very long values (default)");
        cmd.addOption("--max-read-length",    "+R",  1, "[k]bytes: integer (4..4194302, default: 4)",
                                                        "set threshold for long values to k kbytes");
    cmd.addGroup("processing options:");
      cmd.addSubGroup("specific character set:");
        cmd.addOption("--charset-require",    "+Cr",    "require declaration of extended charset (def.)");
        cmd.addOption("--charset-assume",     "+Ca", 1, "[c]harset: string",
                                                        "assume charset c if no extended charset declared");
        cmd.addOption("--charset-check-all",  "+Cc",    "check all data elements with string values\n(default: only PN, LO, LT, SH, ST, UC and UT)");
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
        cmd.addOption("--convert-to-utf8",    "+U8",    "convert all element values that are affected\nby Specific Character Set (0008,0005) to UTF-8");
#endif
    cmd.addGroup("output options:");
      cmd.addSubGroup("general XML format:");
        cmd.addOption("--dcmtk-format",       "-dtk",   "output in DCMTK-specific format (default)");
        cmd.addOption("--native-format",      "-nat",   "output in Native DICOM Model format (part 19)");
        cmd.addOption("--use-xml-namespace",  "+Xn",    "add XML namespace declaration to root element");
      cmd.addSubGroup("DCMTK-specific format (not with --native-format):");
        cmd.addOption("--add-dtd-reference",  "+Xd",    "add reference to document type definition (DTD)");
        cmd.addOption("--embed-dtd-content",  "+Xe",    "embed document type definition into XML document");
        optStr = "use specified DTD file (only with +Xe)\n(default: ";
        optStr += opt_dtdFilename;
        optStr += ")";
        cmd.addOption("--use-dtd-file",       "+Xf", 1, "[f]ilename: string",
                                                        optStr.c_str());
        cmd.addOption("--write-element-name", "+Wn",    "write name of the DICOM data elements (default)");
        cmd.addOption("--no-element-name",    "-Wn",    "do not write name of the DICOM data elements");
        cmd.addOption("--write-binary-data",  "+Wb",    "write binary data of OB and OW elements\n(default: off, be careful with --load-all)");
      cmd.addSubGroup("encoding of binary data:");
        cmd.addOption("--encode-hex",         "+Eh",    "encode binary data as hex numbers\n(default for DCMTK-specific format)");
        cmd.addOption("--encode-uuid",        "+Eu",    "encode binary data as a UUID reference\n(default for Native DICOM Model)");
        cmd.addOption("--encode-base64",      "+Eb",    "encode binary data as Base64 (RFC 2045, MIME)");

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

        if (cmd.findOption("--max-read-length"))
        {
            app.checkValue(cmd.getValueAndCheckMinMax(opt_maxReadLength, 4, 4194302));
            opt_maxReadLength *= 1024; // convert kbytes to bytes
        }
        cmd.beginOptionBlock();
        if (cmd.findOption("--load-all"))
            opt_loadIntoMemory = OFTrue;
        if (cmd.findOption("--load-short"))
            opt_loadIntoMemory = OFFalse;
        cmd.endOptionBlock();

        /* processing options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--charset-require"))
           opt_defaultCharset = NULL;
        if (cmd.findOption("--charset-assume"))
            app.checkValue(cmd.getValue(opt_defaultCharset));
        cmd.endOptionBlock();
        if (cmd.findOption("--charset-check-all"))
            opt_checkAllStrings = OFTrue;
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
        if (cmd.findOption("--convert-to-utf8"))
            opt_convertToUTF8 = OFTrue;
#endif

        /* output options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--native-format"))
            opt_writeFlags |= DCMTypes::XF_useNativeModel;
        if (cmd.findOption("--dcmtk-format"))
            opt_writeFlags &= ~DCMTypes::XF_useNativeModel;
        cmd.endOptionBlock();

        if (cmd.findOption("--use-xml-namespace"))
            opt_writeFlags |= DCMTypes::XF_useXMLNamespace;

        cmd.beginOptionBlock();
        if (cmd.findOption("--add-dtd-reference"))
        {
            app.checkConflict("--add-dtd-reference", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            opt_writeFlags |= DCMTypes::XF_addDocumentType;
        }
        if (cmd.findOption("--embed-dtd-content"))
        {
            app.checkConflict("--embed-dtd-content", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            opt_writeFlags |= DCMTypes::XF_addDocumentType | DCMTypes::XF_embedDocumentType;
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--use-dtd-file"))
        {
            app.checkConflict("--use-dtd-file", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            app.checkDependence("--use-dtd-file", "--embed-dtd-content", (opt_writeFlags & DCMTypes::XF_embedDocumentType) > 0);
            app.checkValue(cmd.getValue(opt_dtdFilename));
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--write-element-name"))
        {
            app.checkConflict("--write-element-name", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            opt_writeFlags &= ~DCMTypes::XF_omitDataElementName;
        }
        if (cmd.findOption("--no-element-name"))
        {
            app.checkConflict("--no-element-name", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            opt_writeFlags |= DCMTypes::XF_omitDataElementName;
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--write-binary-data"))
        {
            app.checkConflict("--write-binary-data", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            opt_writeFlags |= DCMTypes::XF_writeBinaryData;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--encode-hex"))
        {
            app.checkConflict("--encode-hex", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            app.checkDependence("--encode-hex", "--write-binary-data", (opt_writeFlags & DCMTypes::XF_writeBinaryData) > 0);
            opt_writeFlags &= ~DCMTypes::XF_encodeBase64;
        }
        if (cmd.findOption("--encode-uuid"))
        {
            app.checkDependence("--encode-uuid", "--native-format", (opt_writeFlags & DCMTypes::XF_useNativeModel) > 0);
            opt_writeFlags &= ~DCMTypes::XF_encodeBase64;
        }
        if (cmd.findOption("--encode-base64"))
        {
            if (!(opt_writeFlags & DCMTypes::XF_useNativeModel))
                app.checkDependence("--encode-base64", "--write-binary-data", (opt_writeFlags & DCMTypes::XF_writeBinaryData) > 0);
            opt_writeFlags |= DCMTypes::XF_encodeBase64;
        }
        cmd.endOptionBlock();
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcm2xmlLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcm2xmlLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* make sure document type definition file exists */
    if ((opt_writeFlags & DCMTypes::XF_embedDocumentType) && !OFStandard::fileExists(opt_dtdFilename))
    {
        OFLOG_WARN(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": DTD file \"" << opt_dtdFilename
            << "\" does not exist ... adding reference instead");
        opt_writeFlags &= ~DCMTypes::XF_embedDocumentType;
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
        OFCondition status = dfile.loadFile(ifname, opt_ixfer, EGL_noChange, OFstatic_cast(Uint32, opt_maxReadLength), opt_readMode);
        if (status.good())
        {
            // map "old" charset names to DICOM defined terms
            if (opt_defaultCharset != NULL)
            {
                OFString charset(opt_defaultCharset);
                if (charset == "latin-1")
                    opt_defaultCharset = "ISO_IR 100";
                else if (charset == "latin-2")
                    opt_defaultCharset = "ISO_IR 101";
                else if (charset == "latin-3")
                    opt_defaultCharset = "ISO_IR 109";
                else if (charset == "latin-4")
                    opt_defaultCharset = "ISO_IR 110";
                else if (charset == "latin-5")
                    opt_defaultCharset = "ISO_IR 148";
                else if (charset == "cyrillic")
                    opt_defaultCharset = "ISO_IR 144";
                else if (charset == "arabic")
                    opt_defaultCharset = "ISO_IR 127";
                else if (charset == "greek")
                    opt_defaultCharset = "ISO_IR 126";
                else if (charset == "hebrew")
                    opt_defaultCharset = "ISO_IR 138";
            }
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
            DcmDataset *dset = dfile.getDataset();
            /* convert all DICOM strings to UTF-8 (if requested) */
            if (opt_convertToUTF8)
            {
                OFLOG_INFO(dcm2xmlLogger, "converting all element values that are affected by SpecificCharacterSet (0008,0005) to UTF-8");
                // check whether SpecificCharacterSet is absent but needed
                if ((opt_defaultCharset != NULL) && !dset->tagExistsWithValue(DCM_SpecificCharacterSet) &&
                    dset->containsExtendedCharacters(OFFalse /*checkAllStrings*/))
                {
                    // use the manually specified source character set
                    status = dset->convertCharacterSet(opt_defaultCharset, OFString("ISO_IR 192"));
                } else {
                    // expect that SpecificCharacterSet contains the correct value
                    status = dset->convertToUTF8();
                }
                if (status.bad())
                {
                    OFLOG_FATAL(dcm2xmlLogger, status.text() << ": converting file to UTF-8: " << ifname);
                    result = 4;
                }
            } else {
                OFString sopClass;
                /* check whether the file is a DICOMDIR ... */
                if (dfile.getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass).good() &&
                    (sopClass == UID_MediaStorageDirectoryStorage))
                {
                    /* ... with one or more SpecificCharacterSet elements */
                    if (dset->tagExistsWithValue(DCM_SpecificCharacterSet, OFTrue /*searchIntoSub*/))
                    {
                        OFLOG_WARN(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": this is a DICOMDIR file, which can contain more than one "
                            << "SpecificCharacterSet (0008,0005) element ... using option --convert-to-utf8 is strongly recommended");
                    }
                }
            }
#endif
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
                        /* write content in XML format to file */
                        status = writeFile(stream, ifname, &dfile, opt_readMode, opt_loadIntoMemory, opt_dtdFilename,
                            opt_defaultCharset, opt_writeFlags, opt_checkAllStrings);
                        if (status.bad())
                        {
                            OFLOG_ERROR(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") writing file: "<< ofname);
                            result = 2;
                        }
                    } else
                        result = 1;
                } else {
                    /* write content in XML format to standard output */
                    status = writeFile(COUT, ifname, &dfile, opt_readMode, opt_loadIntoMemory, opt_dtdFilename,
                        opt_defaultCharset, opt_writeFlags, opt_checkAllStrings);
                    if (status.bad())
                    {
                        OFLOG_ERROR(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") writing to standard output");
                        result = 3;
                    }
                }
            }
        } else {
            OFLOG_ERROR(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") reading file: "<< ifname);
            result = 5;
        }
    } else {
        OFLOG_ERROR(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
        result = 6;
    }

    return result;
}
