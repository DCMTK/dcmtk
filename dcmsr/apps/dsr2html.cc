/*
 *
 *  Copyright (C) 2000-2013, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Renders the contents of a DICOM structured reporting file in
 *           HTML format
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif
#ifdef WITH_LIBICONV
#include "dcmtk/ofstd/ofchrenc.h"     /* for OFCharacterEncoding */
#endif

#define OFFIS_CONSOLE_APPLICATION "dsr2html"

static OFLogger dsr2htmlLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition renderFile(STD_NAMESPACE ostream &out,
                              const char *ifname,
                              const char *cssName,
                              const char *defaultCharset,
                              const E_FileReadMode readMode,
                              const E_TransferSyntax xfer,
                              const size_t readFlags,
                              const size_t renderFlags,
                              const OFBool checkAllStrings,
                              const OFBool convertToUTF8)
{
    OFCondition result = EC_Normal;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
        return EC_IllegalParameter;
    }

    DcmFileFormat *dfile = new DcmFileFormat();
    if (dfile != NULL)
    {
        if (readMode == ERM_dataset)
            result = dfile->getDataset()->loadFile(ifname, xfer);
        else
            result = dfile->loadFile(ifname, xfer);
        if (result.bad())
        {
            OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                << ") reading file: " << ifname);
        }
    } else
        result = EC_MemoryExhausted;

#ifdef WITH_LIBICONV
    /* convert all DICOM strings to UTF-8 (if requested) */
    if (result.good() && convertToUTF8)
    {
        DcmDataset *dset = dfile->getDataset();
        OFLOG_INFO(dsr2htmlLogger, "converting all element values that are affected by SpecificCharacterSet (0008,0005) to UTF-8");
        // check whether SpecificCharacterSet is absent but needed
        if ((defaultCharset != NULL) && !dset->tagExistsWithValue(DCM_SpecificCharacterSet) &&
            dset->containsExtendedCharacters(OFFalse /*checkAllStrings*/))
        {
            // use the manually specified source character set
            result = dset->convertCharacterSet(defaultCharset, OFString("ISO_IR 192"));
        } else {
            // expect that SpecificCharacterSet contains the correct value
            result = dset->convertToUTF8();
        }
        if (result.bad())
        {
            OFLOG_FATAL(dsr2htmlLogger, result.text() << ": converting file to UTF-8: " << ifname);
        }
    }
#endif
    if (result.good())
    {
        result = EC_CorruptedData;
        DcmDataset *dset = dfile->getDataset();
        DSRDocument *dsrdoc = new DSRDocument();
        if (dsrdoc != NULL)
        {
            result = dsrdoc->read(*dset, readFlags);
            if (result.good())
            {
                // check extended character set
                OFString charset;
                if ((dsrdoc->getSpecificCharacterSet(charset).bad() || charset.empty()) &&
                    dset->containsExtendedCharacters(checkAllStrings))
                {
                    // we have an unspecified extended character set
                    if (defaultCharset == NULL)
                    {
                        // the dataset contains non-ASCII characters that really should not be there
                        OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": SpecificCharacterSet (0008,0005) "
                            << "element absent but extended characters used in file: " << ifname);
                        OFLOG_DEBUG(dsr2htmlLogger, "use option --charset-assume to manually specify an appropriate character set");
                        result = EC_IllegalCall;
                    } else {
                        OFString charsetStr(defaultCharset);
                        // use the default character set specified by the user
                        if (charsetStr == "ISO_IR 192")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_UTF8);
                        else if (charsetStr == "ISO_IR 100")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);
                        else if (charsetStr == "ISO_IR 101")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin2);
                        else if (charsetStr == "ISO_IR 109")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin3);
                        else if (charsetStr == "ISO_IR 110")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin4);
                        else if (charsetStr == "ISO_IR 148")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin5);
                        else if (charsetStr == "ISO_IR 144")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Cyrillic);
                        else if (charsetStr == "ISO_IR 127")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Arabic);
                        else if (charsetStr == "ISO_IR 126")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Greek);
                        else if (charsetStr == "ISO_IR 138")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Hebrew);
                        else {
                            OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": Character set '"
                                << defaultCharset << "' specified with option --charset-assume not supported");
                            result = EC_IllegalCall;
                        }
                    }
                }
                if (result.good())
                    result = dsrdoc->renderHTML(out, renderFlags, cssName);
            } else {
                OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                    << ") parsing file: " << ifname);
            }
        }
        delete dsrdoc;
    }
    delete dfile;

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

    cmd.addParam("dsrfile-in",   "DICOM SR input filename to be rendered", OFCmdParam::PM_Mandatory);
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
                                                          "assume charset c if no extended charset declared");
        cmd.addOption("--charset-check-all",              "check all data elements with string values\n(default: only PN, LO, LT, SH, ST and UT)");
#ifdef WITH_LIBICONV
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
#if !defined(WITH_ZLIB) && !defined(WITH_LIBICONV)
                COUT << " none" << OFendl;
#else
                COUT << OFendl;
#endif
#ifdef WITH_ZLIB
                COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_LIBICONV
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
#ifdef WITH_LIBICONV
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

    int result = 0;
    const char *ifname = NULL;
    /* first parameter is treated as the input filename */
    cmd.getParam(1, ifname);
    if (cmd.getParamCount() == 2)
    {
        /* second parameter specifies the output filename */
        const char *ofname = NULL;
        cmd.getParam(2, ofname);
        STD_NAMESPACE ofstream stream(ofname);
        if (stream.good())
        {
            if (renderFile(stream, ifname, opt_cssName, opt_defaultCharset, opt_readMode, opt_ixfer, opt_readFlags,
                opt_renderFlags, opt_checkAllStrings, opt_convertToUTF8).bad())
            {
                result = 2;
            }
        } else
            result = 1;
    } else {
        /* use standard output */
        if (renderFile(COUT, ifname, opt_cssName, opt_defaultCharset, opt_readMode, opt_ixfer, opt_readFlags,
            opt_renderFlags, opt_checkAllStrings, opt_convertToUTF8).bad())
        {
            result = 3;
        }
    }

    return result;
}
