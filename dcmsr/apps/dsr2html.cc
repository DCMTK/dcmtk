/*
 *
 *  Copyright (C) 2000-2024, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    render the contents of a DICOM structured reporting file in HTML format
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoc.h"       /* for main interface class DSRDocument */

#include "dcmtk/dcmdata/dctk.h"       /* for typical set of "dcmdata" headers */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
#include "dcmtk/ofstd/ofchrenc.h"     /* for OFCharacterEncoding */
#endif

#define OFFIS_CONSOLE_APPLICATION "dsr2html"

static OFLogger dsr2htmlLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition checkCharacterSet(const char *ifname,
                                     DcmFileFormat &dfile,
                                     const char *defaultCharset,
                                     const OFBool checkAllStrings)
{
    OFCondition result = EC_IllegalParameter;
    if (ifname != NULL)
    {
        DcmDataset *dset = dfile.getDataset();
        /* determine character set encoding of the dataset */
        OFString csetString;
        if (dset->findAndGetOFStringArray(DCM_SpecificCharacterSet, csetString).good())
        {
            /* check whether character set is supported */
            if (DSRTypes::definedTermToCharacterSet(csetString) == DSRTypes::CS_invalid)
            {
                if (!csetString.empty())
                {
                    OFLOG_WARN(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": SpecificCharacterSet (0008,0005) "
                        << "value '" << csetString << "' not supported ... quoting non-ASCII characters");
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
                    OFLOG_DEBUG(dsr2htmlLogger, "using option --convert-to-utf8 to convert the DICOM file to "
                        "UTF-8 encoding might also help to solve this problem more appropriately");
#endif
                }
            }
            /* no error */
            result = EC_Normal;
        } else {
            /* SpecificCharacterSet is not present in the dataset */
            if (dset->containsExtendedCharacters(checkAllStrings))
            {
                if (defaultCharset == NULL)
                {
                    /* the dataset contains non-ASCII characters that really should not be there */
                    OFLOG_ERROR(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": SpecificCharacterSet (0008,0005) "
                        << "element absent (on the main data set level) but extended characters used in file: " << ifname);
                    OFLOG_DEBUG(dsr2htmlLogger, "use option --charset-assume to manually specify an appropriate character set");
                    result = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, "Missing Specific Character Set");;
                } else {
                    result = EC_Normal;
                    csetString = defaultCharset;
                    /* first, map "old" character set names to DICOM defined terms */
                    if (csetString == "latin-1")
                        csetString = "ISO_IR 100";
                    else if (csetString == "latin-2")
                        csetString = "ISO_IR 101";
                    else if (csetString == "latin-3")
                        csetString = "ISO_IR 109";
                    else if (csetString == "latin-4")
                        csetString = "ISO_IR 110";
                    else if (csetString == "latin-5")
                        csetString = "ISO_IR 148";
                    else if (csetString == "cyrillic")
                        csetString = "ISO_IR 144";
                    else if (csetString == "arabic")
                        csetString = "ISO_IR 127";
                    else if (csetString == "greek")
                        csetString = "ISO_IR 126";
                    else if (csetString == "hebrew")
                        csetString = "ISO_IR 138";
                    else if (csetString == "latin-9")
                        csetString = "ISO_IR 203";
                    /* then, check whether the character set is actually supported */
                    if (DSRTypes::definedTermToCharacterSet(csetString) == DSRTypes::CS_invalid)
                    {
                        OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": Character set '"
                            << defaultCharset << "' specified with option --charset-assume not supported");
                        result = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, "Cannot select character set");
                    } else {
                        OFLOG_INFO(dsr2htmlLogger, "inserting SpecificCharacterSet (0008,0005) element with value '" << csetString << "'");
                        /* insert the SpecificCharacterSet (0008,0005) element with new value */
                        result = dset->putAndInsertOFStringArray(DCM_SpecificCharacterSet, csetString);
                    }
                }
            } else {
                if (defaultCharset != NULL)
                {
                    /* use "debug" instead of "warn" in order to avoid too much output in default mode */
                    OFLOG_DEBUG(dsr2htmlLogger, "ignoring character set '" << defaultCharset
                        << "' specified with option --charset-assume since it is not needed for this data set");
                }
                /* no error */
                result = EC_Normal;
            }
        }
    }
    return result;
}


#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
static OFCondition convertCharacterSet(const char *ifname,
                                       DcmFileFormat &dfile,
                                       const OFBool convertToUTF8)
{
    OFCondition result = EC_IllegalParameter;
    if (ifname != NULL)
    {
        /* convert all DICOM strings to UTF-8 (if requested) */
        if (convertToUTF8)
        {
            OFLOG_INFO(dsr2htmlLogger, "converting all element values that are affected by SpecificCharacterSet (0008,0005) to UTF-8");
            /* expect that SpecificCharacterSet contains the correct value (defined term) */
            result = dfile.convertToUTF8();
            if (result.bad())
                OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text() << ") converting file to UTF-8: " << ifname);
        } else {
            /* no error */
            result = EC_Normal;
        }
    }
    return result;
}
#endif


static OFCondition renderFile(STD_NAMESPACE ostream &out,
                              const char *ifname,
                              DcmFileFormat &dfile,
                              const char *cssName,
                              const size_t readFlags,
                              const size_t renderFlags)
{
    OFCondition result = EC_IllegalParameter;
    if (ifname != NULL)
    {
        DSRDocument dsrdoc;
        /* read SR data structures */
        result = dsrdoc.read(*dfile.getDataset(), readFlags);
        if (result.good())
        {
            /* render document in HTML format and write it to the output stream */
            result = dsrdoc.renderHTML(out, renderFlags, cssName);
        } else {
            OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text() << ") parsing file: " << ifname);
            /* make sure that the caller does not report this error, too */
            result =  EC_InternalError;
        }
    }
    return result;
}


#define SHORTCOL 3
#define LONGCOL 22


int main(int argc, char *argv[])
{
    size_t opt_readFlags = 0;
    size_t opt_renderFlags = DSRTypes::HF_renderDcmtkFootnote;
    const char *opt_cssName = NULL;
    const char *opt_defaultCharset = NULL;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    OFBool opt_checkAllStrings = OFFalse;
    OFBool opt_convertToUTF8 = OFFalse;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Render DICOM SR file and data set to HTML/XHTML", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dsrfile-in",   "DICOM SR input filename to be rendered\n(\"-\" for stdin)", OFCmdParam::PM_Mandatory);
    cmd.addParam("htmlfile-out", "HTML/XHTML output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                   "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",            "+f",     "read file format or data set (default)");
        cmd.addOption("--read-file-only",       "+fo",    "read file format only");
        cmd.addOption("--read-dataset",         "-f",     "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",       "-t=",    "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",     "-td",    "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",     "-te",    "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",        "-tb",    "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",   "-ti",    "read with implicit VR little endian TS");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("additional information:");
        cmd.addOption("--processing-details",   "-Ip",    "show currently processed content item");
      cmd.addSubGroup("error handling:");
        cmd.addOption("--unknown-relationship", "-Er",    "accept unknown/missing relationship type");
        cmd.addOption("--invalid-item-value",   "-Ev",    "accept invalid content item value\n(e.g. violation of VR or VM definition)");
        cmd.addOption("--ignore-constraints",   "-Ec",    "ignore relationship content constraints");
        cmd.addOption("--ignore-item-errors",   "-Ee",    "do not abort on content item errors, just warn\n(e.g. missing value type specific attributes)");
        cmd.addOption("--skip-invalid-items",   "-Ei",    "skip invalid content items (incl. sub-tree)");
        cmd.addOption("--disable-vr-checker",   "-Dv",    "disable check for VR-conformant string values");
      cmd.addSubGroup("character set:");
        cmd.addOption("--charset-require",      "+Cr",    "require declaration of ext. charset (default)");
        cmd.addOption("--charset-assume",       "+Ca", 1, "[c]harset: string",
                                                          "assume charset c if no ext. charset declared");
        cmd.addOption("--charset-check-all",              "check all data elements with string values\n(default: only PN, LO, LT, SH, ST, UC and UT)");
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
        cmd.addOption("--convert-to-utf8",      "+U8",    "convert all element values that are affected\nby Specific Character Set (0008,0005) to UTF-8");
#endif
    cmd.addGroup("output options:");
      cmd.addSubGroup("HTML/XHTML compatibility:");
        cmd.addOption("--html-3.2",             "+H3",    "use only HTML version 3.2 compatible features");
        cmd.addOption("--html-4.0",             "+H4",    "allow all HTML version 4.01 features (default)");
        cmd.addOption("--xhtml-1.1",            "+X1",    "comply with XHTML version 1.1 specification");
        cmd.addOption("--add-document-type",    "+Hd",    "add reference to SGML document type definition");
      cmd.addSubGroup("cascading style sheet (CSS), not with HTML 3.2:");
        cmd.addOption("--css-reference",        "+Sr", 1, "URL: string",
                                                          "add reference to specified CSS to document");
        cmd.addOption("--css-file",             "+Sf", 1, "[f]ilename: string",
                                                          "embed content of specified CSS into document");
      cmd.addSubGroup("general rendering:");
        cmd.addOption("--expand-inline",        "+Ri",    "expand short content items inline (default)");
        cmd.addOption("--never-expand-inline",  "-Ri",    "never expand content items inline");
        cmd.addOption("--always-expand-inline", "+Ra",    "always expand content items inline");
        cmd.addOption("--render-full-data",     "+Rd",    "render full data of content items");
        cmd.addOption("--section-title-inline", "+Rt",    "render section titles inline, not separately");
      cmd.addSubGroup("document rendering:");
        cmd.addOption("--document-type-title",  "+Dt",    "use document type as document title (default)");
        cmd.addOption("--patient-info-title",   "+Dp",    "use patient information as document title");
        cmd.addOption("--no-document-header",   "-Dh",    "do not render general document information");
      cmd.addSubGroup("code rendering:");
        cmd.addOption("--render-inline-codes",  "+Ci",    "render codes in continuous text blocks");
        cmd.addOption("--concept-name-codes",   "+Cn",    "render code of concept names");
        cmd.addOption("--numeric-unit-codes",   "+Cu",    "render code of numeric measurement units");
        cmd.addOption("--code-value-unit",      "+Cv",    "use code value as measurement unit (default)");
        cmd.addOption("--code-meaning-unit",    "+Cm",    "use code meaning as measurement unit");
        cmd.addOption("--render-all-codes",     "+Cc",    "render all codes (implies +Ci, +Cn and +Cu)");
        cmd.addOption("--code-details-tooltip", "+Ct",    "render code details as a tooltip (implies +Cc)");

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

        /* processing options */

        if (cmd.findOption("--processing-details"))
        {
            app.checkDependence("--processing-details", "verbose mode", dsr2htmlLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
            opt_readFlags |= DSRTypes::RF_showCurrentlyProcessedItem;
        }
        if (cmd.findOption("--unknown-relationship"))
            opt_readFlags |= DSRTypes::RF_acceptUnknownRelationshipType;
        if (cmd.findOption("--invalid-item-value"))
            opt_readFlags |= DSRTypes::RF_acceptInvalidContentItemValue;
        if (cmd.findOption("--ignore-constraints"))
            opt_readFlags |= DSRTypes::RF_ignoreRelationshipConstraints;
        if (cmd.findOption("--ignore-item-errors"))
            opt_readFlags |= DSRTypes::RF_ignoreContentItemErrors;
        if (cmd.findOption("--skip-invalid-items"))
            opt_readFlags |= DSRTypes::RF_skipInvalidContentItems;
        if (cmd.findOption("--disable-vr-checker"))
            dcmEnableVRCheckerForStringValues.set(OFFalse);

        /* character set options */
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

        /* HTML compatibility */
        cmd.beginOptionBlock();
        if (cmd.findOption("--html-3.2"))
            opt_renderFlags = (opt_renderFlags & ~DSRTypes::HF_XHTML11Compatibility) | DSRTypes::HF_HTML32Compatibility;
        if (cmd.findOption("--html-4.0"))
            opt_renderFlags = (opt_renderFlags & ~(DSRTypes::HF_XHTML11Compatibility | DSRTypes::HF_HTML32Compatibility));
        if (cmd.findOption("--xhtml-1.1"))
            opt_renderFlags = (opt_renderFlags & ~DSRTypes::HF_HTML32Compatibility) | DSRTypes::HF_XHTML11Compatibility | DSRTypes::HF_addDocumentTypeReference;
        cmd.endOptionBlock();

        if (cmd.findOption("--add-document-type"))
            opt_renderFlags |= DSRTypes::HF_addDocumentTypeReference;

        /* cascading style sheet */
        cmd.beginOptionBlock();
        if (cmd.findOption("--css-reference"))
        {
            app.checkConflict("--css-reference", "--html-3.2", (opt_renderFlags & DSRTypes::HF_HTML32Compatibility) > 0);
            opt_renderFlags &= ~DSRTypes::HF_copyStyleSheetContent;
            app.checkValue(cmd.getValue(opt_cssName));
        }
        if (cmd.findOption("--css-file"))
        {
            app.checkConflict("--css-file", "--html-3.2", (opt_renderFlags & DSRTypes::HF_HTML32Compatibility) > 0);
            opt_renderFlags |= DSRTypes::HF_copyStyleSheetContent;
            app.checkValue(cmd.getValue(opt_cssName));
        }
        cmd.endOptionBlock();

        /* general rendering */
        cmd.beginOptionBlock();
        if (cmd.findOption("--expand-inline"))
        {
            /* default */
        }
        if (cmd.findOption("--never-expand-inline"))
            opt_renderFlags |= DSRTypes::HF_neverExpandChildrenInline;
        if (cmd.findOption("--always-expand-inline"))
            opt_renderFlags |= DSRTypes::HF_alwaysExpandChildrenInline;
        cmd.endOptionBlock();

        if (cmd.findOption("--render-full-data"))
            opt_renderFlags |= DSRTypes::HF_renderFullData;

        if (cmd.findOption("--section-title-inline"))
            opt_renderFlags |= DSRTypes::HF_renderSectionTitlesInline;

        /* document rendering */
        cmd.beginOptionBlock();
        if (cmd.findOption("--document-type-title"))
        {
            /* default */
        }
        if (cmd.findOption("--patient-info-title"))
            opt_renderFlags |= DSRTypes::HF_renderPatientTitle;
        cmd.endOptionBlock();

        if (cmd.findOption("--no-document-header"))
            opt_renderFlags |= DSRTypes::HF_renderNoDocumentHeader;

        /* code rendering */
        if (cmd.findOption("--render-inline-codes"))
            opt_renderFlags |= DSRTypes::HF_renderInlineCodes;
        if (cmd.findOption("--concept-name-codes"))
            opt_renderFlags |= DSRTypes::HF_renderConceptNameCodes;
        if (cmd.findOption("--numeric-unit-codes"))
            opt_renderFlags |= DSRTypes::HF_renderNumericUnitCodes;
        if (cmd.findOption("--code-value-unit"))
            opt_renderFlags &= ~DSRTypes::HF_useCodeMeaningAsUnit;
        if (cmd.findOption("--code-meaning-unit"))
            opt_renderFlags |= DSRTypes::HF_useCodeMeaningAsUnit;
        if (cmd.findOption("--render-all-codes"))
            opt_renderFlags |= DSRTypes::HF_renderAllCodes;
        if (cmd.findOption("--code-details-tooltip"))
        {
            app.checkConflict("--code-details-tooltip", "--html-3.2", (opt_renderFlags & DSRTypes::HF_HTML32Compatibility) > 0);
            opt_renderFlags |= DSRTypes::HF_useCodeDetailsTooltip;
        }
    }

    /* print resource identifier */
    OFLOG_DEBUG(dsr2htmlLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dsr2htmlLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    int result = 0;
    const char *ifname = NULL;
    /* first parameter is treated as the input filename */
    cmd.getParam(1, ifname);
    /* check input file */
    if ((ifname != NULL) && (strlen(ifname) > 0))
    {
        /* read DICOM file or data set */
        DcmFileFormat dfile;
        OFCondition status = dfile.loadFile(ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength /* default */, opt_readMode);
        if (status.good())
        {
            /* check specific character set (and possibly fix it) */
            if (checkCharacterSet(ifname, dfile, opt_defaultCharset, opt_checkAllStrings).good())
            {
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
                /* convert character set of dataset, e.g. to UTF-8 */
                if (convertCharacterSet(ifname, dfile, opt_convertToUTF8).bad())
                    result = 4;
#endif
            } else
                result = 7;
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
                        /* render to output file */
                        status = renderFile(stream, ifname, dfile, opt_cssName, opt_readFlags, opt_renderFlags);
                        if (status.bad() && (status != EC_InternalError))
                        {
                            OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") writing file: "<< ofname);
                            result = 2;
                        }
                    } else
                        result = 1;
                } else {
                    /* use standard output */
                    status = renderFile(COUT, ifname, dfile, opt_cssName, opt_readFlags, opt_renderFlags);
                    if (status.bad() && (status != EC_InternalError))
                    {
                        OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") writing to standard output");
                        result = 3;
                    }
                }
            }
        } else {
            OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text() << ") reading file: " << ifname);
            result = 5;
        }
    } else {
        OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
        result = 6;
    }

    return result;
}
