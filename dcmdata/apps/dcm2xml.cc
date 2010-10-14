/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.39 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
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
                             const size_t writeFlags,
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
        if (dset->findAndGetOFString(DCM_SpecificCharacterSet, csetString).good())
        {
            if (csetString == "ISO_IR 6")
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
            else if (!csetString.empty())
                OFLOG_WARN(dcm2xmlLogger, "(0008,0005) Specific Character Set '" << csetString << "' not supported");
        } else {
            /* SpecificCharacterSet is not present in the dataset */
            if (dset->containsExtendedCharacters(checkAllStrings))
            {
                if (defaultCharset == NULL)
                {
                    /* the dataset contains non-ASCII characters that really should not be there */
                    OFLOG_WARN(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": (0008,0005) Specific Character Set absent "
                        << "but extended characters used in file: " << ifname);
                    return EC_IllegalCall;
                } else {
                    OFString charset(defaultCharset);
                    if (charset == "latin-1")
                    {
                        csetString = "ISO_IR 100";
                        encString = "ISO-8859-1";
                    }
                    else if (charset == "latin-2")
                    {
                        csetString = "ISO_IR 101";
                        encString = "ISO-8859-2";
                    }
                    else if (charset == "latin-3")
                    {
                        csetString = "ISO_IR 109";
                        encString = "ISO-8859-3";
                    }
                    else if (charset == "latin-4")
                    {
                      csetString = "ISO_IR 110";
                      encString = "ISO-8859-4";
                    }
                    else if (charset == "latin-5")
                    {
                        csetString = "ISO_IR 148";
                        encString = "ISO-8859-9";
                    }
                    else if (charset == "cyrillic")
                    {
                        csetString = "ISO_IR 144";
                        encString = "ISO-8859-5";
                    }
                    else if (charset == "arabic")
                    {
                        csetString = "ISO_IR 127";
                        encString = "ISO-8859-6";
                    }
                    else if (charset == "greek")
                    {
                        csetString = "ISO_IR 126";
                        encString = "ISO-8859-7";
                    }
                    else if (charset == "hebrew")
                    {
                        csetString = "ISO_IR 138";
                        encString = "ISO-8859-8";
                    }
                    dset->putAndInsertString(DCM_SpecificCharacterSet, csetString.c_str());
                }
            }
        }

        /* write XML document header */
        out << "<?xml version=\"1.0\"";
        /* optional character set */
        if (encString.length() > 0)
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
#ifdef HAVE_IOS_NOCREATE
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
                }
                else
                {
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


#define SHORTCOL 3
#define LONGCOL 20


int main(int argc, char *argv[])
{
    size_t opt_writeFlags = 0;
    OFBool opt_loadIntoMemory = OFFalse;
    OFBool opt_checkAllStrings = OFFalse;
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
      cmd.addSubGroup("character set:");
        cmd.addOption("--charset-require",    "+Cr",    "require declaration of extended charset (default)");
        cmd.addOption("--charset-assume",     "+Ca", 1, "[c]harset: string constant",
                                                        "(latin-1 to -5, cyrillic, arabic, greek, hebrew)\n"
                                                        "assume charset c if no extended charset declared");
        cmd.addOption("--charset-check-all",  "+Cc",    "check all data elements with string values\n(default: only PN, LO, LT, SH, ST and UT)");
    cmd.addGroup("output options:");
      cmd.addSubGroup("XML structure:");
        cmd.addOption("--add-dtd-reference",  "+Xd",    "add reference to document type definition (DTD)");
        cmd.addOption("--embed-dtd-content",  "+Xe",    "embed document type definition into XML document");
        optStr = "use specified DTD file (only with +Xe)\n(default: ";
        optStr += opt_dtdFilename;
        optStr += ")";
        cmd.addOption("--use-dtd-file",       "+Xf", 1, "[f]ilename: string",
                                                        optStr.c_str());
        cmd.addOption("--use-xml-namespace",  "+Xn",    "add XML namespace declaration to root element");
      cmd.addSubGroup("DICOM data elements:");
        cmd.addOption("--write-element-name", "+Wn",    "write name of the DICOM data elements (default)");
        cmd.addOption("--no-element-name",    "-Wn",    "do not write name of the DICOM data elements");
        cmd.addOption("--write-binary-data",  "+Wb",    "write binary data of OB and OW elements\n(default: off, be careful with --load-all)");
        cmd.addOption("--encode-hex",         "+Eh",    "encode binary data as hex numbers (default)");
        cmd.addOption("--encode-base64",      "+Eb",    "encode binary data as Base64 (RFC 2045, MIME)");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
        /* check exclusive options first */
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
        {
            app.checkValue(cmd.getValue(opt_defaultCharset));
            OFString charset(opt_defaultCharset);
            if (charset != "latin-1" && charset != "latin-2" && charset != "latin-3" &&
                charset != "latin-4" && charset != "latin-5" && charset != "cyrillic" &&
                charset != "arabic" && charset != "greek" && charset != "hebrew")
            {
                app.printError("unknown value for --charset-assume. known values are latin-1 to -5, cyrillic, arabic, greek, hebrew.");
            }
        }
        cmd.endOptionBlock();
        if (cmd.findOption("--charset-check-all"))
            opt_checkAllStrings = OFTrue;

        /* output options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--add-dtd-reference"))
            opt_writeFlags |= DCMTypes::XF_addDocumentType;
        if (cmd.findOption("--embed-dtd-content"))
            opt_writeFlags |= DCMTypes::XF_addDocumentType | DCMTypes::XF_embedDocumentType;
        cmd.endOptionBlock();

        if (cmd.findOption("--use-dtd-file"))
        {
            app.checkDependence("--use-dtd-file", "--embed-dtd-content", (opt_writeFlags & DCMTypes::XF_embedDocumentType) > 0);
            app.checkValue(cmd.getValue(opt_dtdFilename));
        }

        if (cmd.findOption("--use-xml-namespace"))
            opt_writeFlags |= DCMTypes::XF_useDcmtkNamespace;

        cmd.beginOptionBlock();
        if (cmd.findOption("--write-element-name"))
            opt_writeFlags &= ~DCMTypes::XF_omitDataElementName;
        if (cmd.findOption("--no-element-name"))
            opt_writeFlags |= DCMTypes::XF_omitDataElementName;
        cmd.endOptionBlock();

        if (cmd.findOption("--write-binary-data"))
            opt_writeFlags |= DCMTypes::XF_writeBinaryData;

        cmd.beginOptionBlock();
        if (cmd.findOption("--encode-hex"))
        {
            app.checkDependence("--encode-hex", "--write-binary-data", (opt_writeFlags & DCMTypes::XF_writeBinaryData) > 0);
            opt_writeFlags &= ~DCMTypes::XF_encodeBase64;
        }
        if (cmd.findOption("--encode-base64"))
        {
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
        OFCondition status = dfile.loadFile(ifname, opt_ixfer, EGL_noChange, opt_maxReadLength, opt_readMode);
        if (status.good())
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
                    if (writeFile(stream, ifname, &dfile, opt_readMode, opt_loadIntoMemory, opt_dtdFilename,
                                  opt_defaultCharset, opt_writeFlags, opt_checkAllStrings).bad())
                        result = 2;
                } else
                    result = 1;
            } else {
                /* write content in XML format to standard output */
                if (writeFile(COUT, ifname, &dfile, opt_readMode, opt_loadIntoMemory, opt_dtdFilename,
                              opt_defaultCharset, opt_writeFlags, opt_checkAllStrings).bad())
                    result = 3;
            }
        } else
            OFLOG_WARN(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": (" << status.text() << ") reading file: "<< ifname);
    } else
        OFLOG_WARN(dcm2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");

    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dcm2xml.cc,v $
 * Revision 1.39  2010-10-14 13:13:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.38  2009-11-13 13:20:23  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.37  2009-11-04 09:58:05  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.36  2009-04-24 12:20:41  joergr
 * Fixed minor inconsistencies regarding layout/formatting in syntax usage.
 *
 * Revision 1.35  2009-04-21 14:02:49  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.34  2009-03-25 17:54:35  joergr
 * Added parentheses in order to keep MSVC quiet.
 *
 * Revision 1.33  2009-03-11 13:06:12  joergr
 * Added support for specifying the filename of the DTD on the command line.
 * Added command line option for quiet mode (print no warnings and errors).
 *
 * Revision 1.32  2009-02-04 18:08:18  joergr
 * Fixed various layout and formatting issues.
 *
 * Revision 1.31  2008-09-25 14:38:48  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.30  2008-09-25 11:19:48  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.29  2008-01-23 15:09:26  joergr
 * Restructured code in order to avoid empty output file in case the input file
 * could not be read.
 *
 * Revision 1.28  2006/12/13 14:21:56  joergr
 * Removed dependence of the new command line option "--charset-check-all" from
 * option "--charset-assume".
 *
 * Revision 1.27  2006/12/13 13:55:38  joergr
 * Added new command line option that allows to check all data elements with
 * string values for extended characters, not only those affected by Specific
 * CharacterSet (0008,0005).
 *
 * Revision 1.26  2006/08/15 15:50:56  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.25  2006/07/27 13:52:42  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.24  2006/05/11 08:44:57  joergr
 * Added new option that allows to omit the element name in the XML output.
 * Moved checkForNonASCIICharacters() from application to library.
 *
 * Revision 1.23  2006/02/23 12:46:51  joergr
 * Added new default option --encode-hex. Fixed layout and formatting issues.
 *
 * Revision 1.22  2005/12/08 15:40:42  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.21  2005/12/07 16:42:49  onken
 * Changed default and minimum value of --max-read-length to 4 KB
 *
 * Revision 1.20  2005/12/02 08:58:44  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 * Removed superfluous local variable. Changed type of variable "maxReadLength".
 * Made description of option --max-read-length more consistent with the other
 * command line tools.
 *
 * Revision 1.19  2005/12/01 11:25:44  joergr
 * Removed superfluous local variable. Changed type of variable "maxReadLength".
 *
 * Revision 1.18  2005/11/28 15:28:54  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.17  2005/11/17 11:26:11  onken
 * Option --max-read-length now uses OFCommandLine to check, whether option
 * value is in range
 *
 * Revision 1.16  2005/11/16 14:59:11  onken
 * *** empty log message ***
 *
 * Revision 1.15  2005/11/16 14:55:56  onken
 * Added "--max-read-length" option to dcmdump and dcm2xml to override
 * DCMTK-internal threshold (4096K) for long attribute values.
 *
 * Revision 1.14  2005/06/24 10:06:46  joergr
 * Check dependence between command line options --write-binary-data and
 * --encode-base64.
 *
 * Revision 1.13  2004/11/29 17:02:17  joergr
 * Added warning message when character set is unknown, unsupported  or cannot
 * be mapped to the output format. Added support for UTF-8 character set.
 *
 * Revision 1.12  2004/11/22 16:30:19  meichel
 * Now checking whether extended characters are present in a DICOM dataset,
 *   preventing generation of incorrect XML if undeclared extended charset used.
 *
 * Revision 1.11  2003/04/22 08:23:33  joergr
 * Added new command line option which allows to embed the content of the DTD
 * instead of referencing the DTD file.
 *
 * Revision 1.10  2003/04/01 14:56:14  joergr
 * Added support for XML namespaces.
 *
 * Revision 1.9  2002/11/26 08:42:58  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.8  2002/09/23 17:52:01  joergr
 * Prepared code for future support of 'config.guess' host identifiers.
 *
 * Revision 1.7  2002/09/23 13:50:39  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used.
 *
 * Revision 1.6  2002/08/21 10:14:13  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.5  2002/06/10 17:35:47  joergr
 * Fixed inconsistency regarding spelling of the "file-format" element.
 *
 * Revision 1.4  2002/05/14 08:19:22  joergr
 * Added support for Base64 (MIME) encoded binary data.
 *
 * Revision 1.3  2002/05/07 12:47:41  joergr
 * Fixed bug in an error message.
 *
 * Revision 1.2  2002/04/25 14:56:35  joergr
 * Removed unused function parameter to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/04/25 10:08:35  joergr
 * Added new command line tool to convert DICOM files to XML.
 *
 *
 */
