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
 *  Purpose: Convert the contents of a DICOM structured reporting file to
 *           XML format
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-21 09:10:08 $
 *  CVS/RCS Revision: $Revision: 1.42 $
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
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dsr2xml"

static OFLogger dsr2xmlLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition writeFile(STD_NAMESPACE ostream &out,
                             const char *ifname,
                             DcmDataset *dset,
                             const size_t readFlags,
                             const size_t writeFlags,
                             const char *defaultCharset,
                             const OFBool checkAllStrings)
{
    OFCondition result = EC_IllegalParameter;
    if ((ifname != NULL) && (dset != NULL))
    {
        DSRDocument *dsrdoc = new DSRDocument();
        if (dsrdoc != NULL)
        {
            result = dsrdoc->read(*dset, readFlags);
            if (result.good())
            {
                // check extended character set
                const char *charset = dsrdoc->getSpecificCharacterSet();
                if ((charset == NULL || strlen(charset) == 0) && dset->containsExtendedCharacters(checkAllStrings))
                {
                    // we have an unspecified extended character set
                    if (defaultCharset == NULL)
                    {
                      /* the dataset contains non-ASCII characters that really should not be there */
                      OFLOG_FATAL(dsr2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": error: (0008,0005) Specific Character Set absent "
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
                if (result.good())
                    result = dsrdoc->writeXML(out, writeFlags);
            } else {
                OFLOG_FATAL(dsr2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                    << ") parsing file: " << ifname);
            }
        } else
            result = EC_MemoryExhausted;
        delete dsrdoc;
    }
    return result;
}


#define SHORTCOL 3
#define LONGCOL 22


int main(int argc, char *argv[])
{
    size_t opt_readFlags = 0;
    size_t opt_writeFlags = 0;
    const char *opt_defaultCharset = NULL;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    OFBool opt_checkAllStrings = OFFalse;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Convert DICOM SR file and data set to XML", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dsrfile-in",  "DICOM SR input filename to be converted", OFCmdParam::PM_Mandatory);
    cmd.addParam("xmlfile-out", "XML output filename (default: stdout)", OFCmdParam::PM_Optional);

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
      cmd.addSubGroup("character set:");
        cmd.addOption("--charset-require",      "+Cr",    "require declaration of ext. charset (default)");
        cmd.addOption("--charset-assume",       "+Ca", 1, "[c]harset: string constant (latin-1 to -5,",
                                                          "greek, cyrillic, arabic, hebrew)\n"
                                                          "assume charset c if no extended charset found");
        cmd.addOption("--charset-check-all",    "+Cc",    "check all data elements with string values\n(default: only PN, LO, LT, SH, ST and UT)");
    cmd.addGroup("output options:");
      cmd.addSubGroup("encoding:");
        cmd.addOption("--attr-all",             "+Ea",    "encode everything as XML attribute\n(shortcut for +Ec, +Er, +Ev and +Et)");
        cmd.addOption("--attr-code",            "+Ec",    "encode code value, coding scheme designator\nand coding scheme version as XML attribute");
        cmd.addOption("--attr-relationship",    "+Er",    "encode relationship type as XML attribute");
        cmd.addOption("--attr-value-type",      "+Ev",    "encode value type as XML attribute");
        cmd.addOption("--attr-template-id",     "+Et",    "encode template id as XML attribute");
        cmd.addOption("--template-envelope",    "+Ee",    "template element encloses content items\n(requires +Wt, implies +Et)");
      cmd.addSubGroup("XML structure:");
        cmd.addOption("--add-schema-reference", "+Xs",    "add reference to XML Schema \"" DCMSR_XML_XSD_FILE "\"\n(not with +Ea, +Ec, +Er, +Ev, +Et, +Ee, +We)");
        cmd.addOption("--use-xml-namespace",    "+Xn",    "add XML namespace declaration to root element");
      cmd.addSubGroup("writing:");
        cmd.addOption("--write-empty-tags",     "+We",    "write all tags even if their value is empty");
        cmd.addOption("--write-item-id",        "+Wi",    "always write item identifier");
        cmd.addOption("--write-template-id",    "+Wt",    "write template identification information");

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
        if (cmd.findOption("--attr-all"))
            opt_writeFlags |= DSRTypes::XF_encodeEverythingAsAttribute;
        if (cmd.findOption("--attr-code"))
            opt_writeFlags |= DSRTypes::XF_codeComponentsAsAttribute;
        if (cmd.findOption("--attr-relationship"))
            opt_writeFlags |= DSRTypes::XF_relationshipTypeAsAttribute;
        if (cmd.findOption("--attr-value-type"))
            opt_writeFlags |= DSRTypes::XF_valueTypeAsAttribute;
        if (cmd.findOption("--attr-template-id"))
            opt_writeFlags |= DSRTypes::XF_templateIdentifierAsAttribute;
        if (cmd.findOption("--template-envelope"))
            opt_writeFlags |= DSRTypes::XF_templateElementEnclosesItems;

        if (cmd.findOption("--add-schema-reference"))
            opt_writeFlags |= DSRTypes::XF_addSchemaReference;
        if (cmd.findOption("--use-xml-namespace"))
            opt_writeFlags |= DSRTypes::XF_useDcmsrNamespace;

        if (cmd.findOption("--write-empty-tags"))
            opt_writeFlags |= DSRTypes::XF_writeEmptyTags;
        if (cmd.findOption("--write-item-id"))
            opt_writeFlags |= DSRTypes::XF_alwaysWriteItemIdentifier;
        if (cmd.findOption("--write-template-id"))
            opt_writeFlags |= DSRTypes::XF_writeTemplateIdentification;

        /* check conflicts and dependencies */
        if (opt_writeFlags & DSRTypes::XF_addSchemaReference)
        {
            app.checkConflict("--add-schema-reference", "--attr-all", (opt_writeFlags & DSRTypes::XF_encodeEverythingAsAttribute) > 0);
            app.checkConflict("--add-schema-reference", "--attr-code", (opt_writeFlags & DSRTypes::XF_codeComponentsAsAttribute) > 0);
            app.checkConflict("--add-schema-reference", "--attr-relationship", (opt_writeFlags & DSRTypes::XF_relationshipTypeAsAttribute) > 0);
            app.checkConflict("--add-schema-reference", "--attr-value-type", (opt_writeFlags & DSRTypes::XF_valueTypeAsAttribute) > 0);
            app.checkConflict("--add-schema-reference", "--attr-template-id", (opt_writeFlags & DSRTypes::XF_templateIdentifierAsAttribute) > 0);
            app.checkConflict("--add-schema-reference", "--template-envelope", (opt_writeFlags & DSRTypes::XF_templateElementEnclosesItems) > 0);
            app.checkConflict("--add-schema-reference", "--write-empty-tags", (opt_writeFlags & DSRTypes::XF_writeEmptyTags) > 0);
        }
        if (opt_writeFlags & DSRTypes::XF_templateElementEnclosesItems)
            app.checkDependence("--template-envelope", "--write-template-id", (opt_writeFlags & DSRTypes::XF_writeTemplateIdentification) > 0);
    }

    /* print resource identifier */
    OFLOG_DEBUG(dsr2xmlLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dsr2xmlLogger, "no data dictionary loaded, check environment variable: "
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
        OFCondition status = dfile.loadFile(ifname, opt_ixfer);
        if (status.good())
        {
            DcmDataset *dset = dfile.getDataset();
            /* if second parameter is present, it is treated as the output filename ("stdout" otherwise) */
            if (cmd.getParamCount() == 2)
            {
                const char *ofname = NULL;
                cmd.getParam(2, ofname);
                STD_NAMESPACE ofstream stream(ofname);
                if (stream.good())
                {
                    /* write content in XML format to file */
                    if (writeFile(stream, ifname, dset, opt_readFlags, opt_writeFlags, opt_defaultCharset, opt_checkAllStrings).bad())
                        result = 2;
                } else
                    result = 1;
            } else {
                /* write content in XML format to standard output */
                if (writeFile(COUT, ifname, dset, opt_readFlags, opt_writeFlags, opt_defaultCharset, opt_checkAllStrings).bad())
                    result = 3;
            }
        } else {
            OFLOG_FATAL(dsr2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << status.text()
                << ") reading file: " << ifname);
        }
    } else
        OFLOG_FATAL(dsr2xmlLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");

    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dsr2xml.cc,v $
 * Revision 1.42  2010-10-21 09:10:08  joergr
 * Renamed variable to avoid warning reported by gcc with additional flags.
 *
 * Revision 1.41  2010-10-14 13:13:51  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.40  2009-10-14 10:51:56  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.39  2009-10-13 14:57:49  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.38  2009-04-21 14:13:27  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.37  2008-09-25 14:14:21  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.36  2008-01-23 15:13:26  joergr
 * Restructured code in order to avoid empty output file in case the input file
 * could not be read.
 *
 * Revision 1.35  2007/11/15 16:22:42  joergr
 * Fixed coding style to be more consistent.
 *
 * Revision 1.34  2006/12/13 14:16:15  joergr
 * Added new command line option that allows to check all data elements with
 * string values for extended characters, not only those affected by Specific
 * CharacterSet (0008,0005).
 *
 * Revision 1.33  2006/08/15 16:40:02  meichel
 * Updated the code in module dcmsr to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.32  2006/07/27 14:52:00  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.31  2006/07/25 13:32:01  joergr
 * Fixed minor layout and formatting issues.
 *
 * Revision 1.30  2006/05/11 09:13:45  joergr
 * Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 * Revision 1.29  2006/02/23 12:49:43  joergr
 * Fixed layout and formatting issues.
 *
 * Revision 1.28  2005/12/08 15:47:34  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.27  2005/12/02 10:37:30  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 *
 * Revision 1.26  2004/11/29 17:07:19  joergr
 * Fixed minor formatting issues.
 *
 * Revision 1.25  2004/11/22 17:05:19  meichel
 * Removed command lin option for Thai and Katakana character sets
 *   which cannot currently be converted to XML
 *
 * Revision 1.24  2004/11/22 16:45:07  meichel
 * Now checking whether extended characters are present in a DICOM SR document,
 *   preventing generation of incorrect XML if undeclared extended charset used.
 *
 * Revision 1.23  2004/09/09 13:58:36  joergr
 * Added option to control the way the template identification is encoded for
 * the XML output ("inside" or "outside" of the content items).
 *
 * Revision 1.22  2004/09/03 09:20:49  joergr
 * Added check for conflicting command line options.
 *
 * Revision 1.21  2004/01/20 15:37:51  joergr
 * Fixed typo.
 *
 * Revision 1.20  2004/01/20 15:33:26  joergr
 * Added new command line option which allows to write the item identifier "id"
 * (XML attribute) even if it is not required (because the item is not referenced
 * by any other item). Useful for debugging purposes.
 *
 * Revision 1.19  2004/01/05 14:34:59  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.18  2003/10/31 13:31:04  joergr
 * Added command line option +Ea (--attr-all), a shortcut for +Ec, +Er and +Er.
 *
 * Revision 1.17  2003/10/30 17:43:54  joergr
 * Added new command line option which allows to write the template
 * identification of a content item in XML format.
 *
 * Revision 1.16  2003/08/07 11:53:12  joergr
 * Added new option --add-schema-reference to command line tool dsr2xml.
 *
 * Revision 1.15  2003/04/01 14:58:32  joergr
 * Added support for XML namespaces.
 *
 * Revision 1.14  2002/11/26 08:45:34  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.13  2002/09/23 18:16:42  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.12  2002/05/07 12:47:59  joergr
 * Fixed bug in an error message.
 *
 * Revision 1.11  2002/04/16 13:49:52  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.10  2002/04/11 13:05:02  joergr
 * Use the new loadFile() and saveFile() routines from the dcmdata library.
 *
 * Revision 1.9  2001/11/09 16:09:35  joergr
 * Added new command line option allowing to encode codes as XML attributes
 * (instead of tags).
 *
 * Revision 1.8  2001/10/10 15:26:33  joergr
 * Additonal adjustments for new OFCondition class.
 *
 * Revision 1.7  2001/10/02 11:56:00  joergr
 * Adapted module "dcmsr" to the new class OFCondition. Introduced module
 * specific error codes.
 *
 * Revision 1.6  2001/09/26 13:04:01  meichel
 * Adapted dcmsr to class OFCondition
 *
 * Revision 1.5  2001/06/20 15:06:38  joergr
 * Added new debugging features (additional flags) to examine "corrupted" SR
 * documents.
 *
 * Revision 1.4  2001/05/07 16:12:51  joergr
 * Updated CVS header.
 *
 * Revision 1.3  2001/02/02 14:36:27  joergr
 * Added new option to dsr2xml allowing to specify whether value and/or
 * relationship type are to be encoded as XML attributes or elements.
 *
 * Revision 1.2  2000/11/09 11:31:21  joergr
 * Corrected typo.
 *
 * Revision 1.1  2000/11/01 16:09:57  joergr
 * Added command line tool to convert DICOM SR documents to XML.
 *
 *
 */
