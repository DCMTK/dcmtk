/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-21 09:10:08 $
 *  CVS/RCS Revision: $Revision: 1.37 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_ZLIB
#include <zlib.h>       /* for zlibVersion() */
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
                              const size_t renderFlags)
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
                const char *charset = dsrdoc->getSpecificCharacterSet();
                if ((charset == NULL || strlen(charset) == 0) && dset->containsExtendedCharacters())
                {
                    // we have an unspecified extended character set
                    if (defaultCharset == NULL)
                    {
                        /* the dataset contains non-ASCII characters that really should not be there */
                        OFLOG_FATAL(dsr2htmlLogger, OFFIS_CONSOLE_APPLICATION << ": (0008,0005) Specific Character Set absent "
                            << "but extended characters used in file: " << ifname);
                        result = EC_IllegalCall;
                    } else {
                        OFString charsetStr(defaultCharset);
                        if (charsetStr == "latin-1")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);
                        else if (charsetStr == "latin-2")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin2);
                        else if (charsetStr == "latin-3")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin3);
                        else if (charsetStr == "latin-4")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin4);
                        else if (charsetStr == "latin-5")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Latin5);
                        else if (charsetStr == "cyrillic")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Cyrillic);
                        else if (charsetStr == "arabic")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Arabic);
                        else if (charsetStr == "greek")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Greek);
                        else if (charsetStr == "hebrew")
                            dsrdoc->setSpecificCharacterSetType(DSRTypes::CS_Hebrew);
                    }
                }
                if (result.good()) result = dsrdoc->renderHTML(out, renderFlags, cssName);
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

    cmd.addGroup("parsing options:");
      cmd.addSubGroup("additional information:");
        cmd.addOption("--processing-details",   "-Ip",    "show currently processed content item");
      cmd.addSubGroup("error handling:");
        cmd.addOption("--unknown-relationship", "-Er",    "accept unknown/missing relationship type");
        cmd.addOption("--ignore-constraints",   "-Ec",    "ignore relationship content constraints");
        cmd.addOption("--ignore-item-errors",   "-Ee",    "do not abort on content item errors, just warn\n(e.g. missing value type specific attributes)");
        cmd.addOption("--skip-invalid-items",   "-Ei",    "skip invalid content items (incl. sub-tree)");
      cmd.addSubGroup("character set:");
        cmd.addOption("--charset-require",      "+Cr",    "require declaration of ext. charset (default)");
        cmd.addOption("--charset-assume",       "+Ca", 1, "[c]harset: string constant (latin-1 to -5,",
                                                          "greek, cyrillic, arabic, hebrew)\n"
                                                          "assume c if undeclared extended charset found");
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

        if (cmd.findOption("--processing-details"))
        {
            app.checkDependence("--processing-details", "verbose mode", dsr2htmlLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
            opt_readFlags |= DSRTypes::RF_showCurrentlyProcessedItem;
        }
        if (cmd.findOption("--unknown-relationship"))
            opt_readFlags |= DSRTypes::RF_acceptUnknownRelationshipType;
        if (cmd.findOption("--ignore-constraints"))
            opt_readFlags |= DSRTypes::RF_ignoreRelationshipConstraints;
        if (cmd.findOption("--ignore-item-errors"))
            opt_readFlags |= DSRTypes::RF_ignoreContentItemErrors;
        if (cmd.findOption("--skip-invalid-items"))
            opt_readFlags |= DSRTypes::RF_skipInvalidContentItems;

        /* charset options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--charset-require"))
        {
           opt_defaultCharset = NULL;
        }
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
    cmd.getParam(1, ifname);
    if (cmd.getParamCount() == 2)
    {
        const char *ofname = NULL;
        cmd.getParam(2, ofname);
        STD_NAMESPACE ofstream stream(ofname);
        if (stream.good())
        {
            if (renderFile(stream, ifname, opt_cssName, opt_defaultCharset, opt_readMode, opt_ixfer, opt_readFlags, opt_renderFlags).bad())
                result = 2;
        } else
            result = 1;
    } else {
        if (renderFile(COUT, ifname, opt_cssName, opt_defaultCharset, opt_readMode, opt_ixfer, opt_readFlags, opt_renderFlags).bad())
            result = 3;
    }

    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dsr2html.cc,v $
 * Revision 1.37  2010-10-21 09:10:08  joergr
 * Renamed variable to avoid warning reported by gcc with additional flags.
 *
 * Revision 1.36  2010-10-14 13:13:51  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.35  2009-10-30 10:08:34  joergr
 * Option --processing-details now requires verbose mode.
 *
 * Revision 1.34  2009-10-14 10:51:56  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.33  2009-10-13 14:57:49  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.32  2009-04-21 14:13:27  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.31  2008-09-25 14:14:21  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.30  2008-05-20 13:46:39  joergr
 * Modified code to avoid warning message on MSVC compiler (implicit type
 * conversion).
 *
 * Revision 1.29  2008-05-19 09:41:07  joergr
 * Added new command line options that enables reading of SR documents with
 * unknown/missing relationship type(s).
 *
 * Revision 1.28  2007/11/15 16:25:07  joergr
 * Added support for output in XHTML 1.1 format.
 *
 * Revision 1.27  2006/08/15 16:40:02  meichel
 * Updated the code in module dcmsr to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.26  2006/07/27 14:52:00  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.25  2006/07/25 13:33:30  joergr
 * Added new command line options --always-expand-inline, --section-title-inline
 * and --code-details-tooltip (according to new optional HTML rendering flags).
 * Changed short option of --render-all-codes from +Ca to +Cc in order to avoid
 * conflicts with option --charset-assume.
 *
 * Revision 1.24  2006/05/11 09:13:45  joergr
 * Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 * Revision 1.23  2005/12/08 15:47:33  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.22  2005/12/02 10:37:30  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 *
 * Revision 1.21  2004/11/22 17:20:16  meichel
 * Now checking whether extended characters are present in a DICOM SR document,
 *   preventing generation of incorrect HTML if undeclared extended charset used.
 *
 * Revision 1.20  2004/01/05 14:34:59  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.19  2003/10/06 09:56:10  joergr
 * Added new flag which allows to ignore content item errors when reading an SR
 * document (e.g. missing value type specific attributes).
 *
 * Revision 1.18  2002/11/26 08:45:34  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.17  2002/09/23 18:16:42  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.16  2002/08/02 12:37:16  joergr
 * Enhanced debug output of dcmsr command line tools (e.g. add position string
 * of invalid content items to error messages).
 *
 * Revision 1.15  2002/05/07 12:47:58  joergr
 * Fixed bug in an error message.
 *
 * Revision 1.14  2002/04/16 13:49:52  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.13  2002/04/11 13:05:02  joergr
 * Use the new loadFile() and saveFile() routines from the dcmdata library.
 *
 * Revision 1.12  2001/10/10 15:26:31  joergr
 * Additonal adjustments for new OFCondition class.
 *
 * Revision 1.11  2001/10/02 11:55:59  joergr
 * Adapted module "dcmsr" to the new class OFCondition. Introduced module
 * specific error codes.
 *
 * Revision 1.10  2001/09/26 13:04:01  meichel
 * Adapted dcmsr to class OFCondition
 *
 * Revision 1.9  2001/06/20 15:06:38  joergr
 * Added new debugging features (additional flags) to examine "corrupted" SR
 * documents.
 *
 * Revision 1.8  2001/06/01 15:50:57  meichel
 * Updated copyright header
 *
 * Revision 1.7  2001/04/03 08:22:54  joergr
 * Added new command line option: ignore relationship content constraints
 * specified for each SR document class.
 *
 * Revision 1.6  2000/12/08 16:06:19  joergr
 * Replaced empty code lines (";") by empty command blocks ("{}") to avoid
 * compiler warnings reported by MSVC6.
 *
 * Revision 1.5  2000/11/09 20:31:08  joergr
 * Added new command line options (document type and HTML version).
 *
 * Revision 1.4  2000/11/07 18:09:48  joergr
 * Added new command line option allowing to choose code value or meaning to be
 * rendered as the numeric measurement unit.
 *
 * Revision 1.3  2000/11/01 16:08:04  joergr
 * Added support for Cascading Style Sheet (CSS) used optionally for HTML
 * rendering. Optimized HTML rendering.
 *
 * Revision 1.2  2000/10/26 14:15:33  joergr
 * Added new flag specifying whether to add a "dcmtk" footnote to the rendered
 * HTML document or not.
 *
 * Revision 1.1  2000/10/13 07:46:21  joergr
 * Added new module 'dcmsr' providing access to DICOM structured reporting
 * documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
