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
 *  Purpose: List the contents of a dicom structured reporting file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:52 $
 *  CVS/RCS Revision: $Revision: 1.34 $
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
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dsrdump"

static OFLogger dsrdumpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition dumpFile(STD_NAMESPACE ostream &out,
                            const char *ifname,
                            const E_FileReadMode readMode,
                            const E_TransferSyntax xfer,
                            const size_t readFlags,
                            const size_t printFlags)
{
    OFCondition result = EC_Normal;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        OFLOG_FATAL(dsrdumpLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
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
            OFLOG_FATAL(dsrdumpLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                << ") reading file: " << ifname);
        }
    } else
        result = EC_MemoryExhausted;

    if (result.good())
    {
        result = EC_CorruptedData;
        DSRDocument *dsrdoc = new DSRDocument();
        if (dsrdoc != NULL)
        {
            result = dsrdoc->read(*dfile->getDataset(), readFlags);
            if (result.good())
            {
                result = dsrdoc->print(out, printFlags);
                out << OFendl;
            }
            else
            {
                OFLOG_FATAL(dsrdumpLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
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
    size_t opt_printFlags = DSRTypes::PF_shortenLongItemValues;
    OFBool opt_printFilename = OFFalse;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Dump DICOM SR file and data set", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dsrfile-in", "DICOM SR input filename to be dumped", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                   "-h",  "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                       "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",            "+f",  "read file format or data set (default)");
        cmd.addOption("--read-file-only",       "+fo", "read file format only");
        cmd.addOption("--read-dataset",         "-f",  "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",       "-t=", "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",     "-td", "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",     "-te", "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",        "-tb", "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",   "-ti", "read with implicit VR little endian TS");

    cmd.addGroup("parsing options:");
      cmd.addSubGroup("additional information:");
        cmd.addOption("--processing-details",   "-Ip", "show currently processed content item");
      cmd.addSubGroup("error handling:");
        cmd.addOption("--unknown-relationship", "-Er", "accept unknown/missing relationship type");
        cmd.addOption("--ignore-constraints",   "-Ec", "ignore relationship content constraints");
        cmd.addOption("--ignore-item-errors",   "-Ee", "do not abort on content item errors, just warn\n(e.g. missing value type specific attributes)");
        cmd.addOption("--skip-invalid-items",   "-Ei", "skip invalid content items (incl. sub-tree)");

    cmd.addGroup("output options:");
      cmd.addSubGroup("printing:");
        cmd.addOption("--print-filename",       "+Pf", "print header with filename for each document");
        cmd.addOption("--no-document-header",   "-Ph", "do not print general document information");
        cmd.addOption("--number-nested-items",  "+Pn", "print position string in front of each line");
        cmd.addOption("--indent-nested-items",  "-Pn", "indent nested items by spaces (default)");
        cmd.addOption("--print-long-values",    "+Pl", "print long item values completely");
        cmd.addOption("--shorten-long-values",  "-Pl", "print long item values shortened (default)");
        cmd.addOption("--print-instance-uid",   "+Pu", "print SOP instance UID of referenced objects");
        cmd.addOption("--print-all-codes",      "+Pc", "print all codes (incl. concept name codes)");
        cmd.addOption("--print-template-id",    "+Pt", "print template identification information");

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

        /* options */
        OFLog::configureFromCommandLine(cmd, app);

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
            app.checkDependence("--processing-details", "verbose mode", dsrdumpLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
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

        if (cmd.findOption("--print-filename"))
            opt_printFilename = OFTrue;
        if (cmd.findOption("--no-document-header"))
            opt_printFlags |= DSRTypes::PF_printNoDocumentHeader;

        cmd.beginOptionBlock();
        if (cmd.findOption("--number-nested-items"))
            opt_printFlags |= DSRTypes::PF_printItemPosition;
        if (cmd.findOption("--indent-nested-items"))
            opt_printFlags &= ~DSRTypes::PF_printItemPosition;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--print-long-values"))
            opt_printFlags &= ~DSRTypes::PF_shortenLongItemValues;
        if (cmd.findOption("--shorten-long-values"))
            opt_printFlags |= DSRTypes::PF_shortenLongItemValues;
        cmd.endOptionBlock();

        if (cmd.findOption("--print-instance-uid"))
            opt_printFlags |= DSRTypes::PF_printSOPInstanceUID;
        if (cmd.findOption("--print-all-codes"))
            opt_printFlags |= DSRTypes::PF_printAllCodes;
        if (cmd.findOption("--print-template-id"))
            opt_printFlags |= DSRTypes::PF_printTemplateIdentification;
    }

    /* print resource identifier */
    OFLOG_DEBUG(dsrdumpLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dsrdumpLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    int errorCount = 0;
    const int count = cmd.getParamCount();
    const char *current = NULL;
    for (int i = 1; i <= count; i++)
    {
        cmd.getParam(i, current);
        if (opt_printFilename)
        {
            if (i > 1)
                COUT << OFString(79, '-') << OFendl;
            COUT << OFFIS_CONSOLE_APPLICATION << " (" << i << "/" << count << "): " << current << OFendl << OFendl;
        }
        if (dumpFile(COUT, current, opt_readMode, opt_ixfer, opt_readFlags, opt_printFlags).bad())
            errorCount++;
    }

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif
    return errorCount;
}


/*
 * CVS/RCS Log:
 * $Log: dsrdump.cc,v $
 * Revision 1.34  2010-10-14 13:13:52  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.33  2009-11-03 09:13:54  joergr
 * Switched to old behavior: Output input filename to COUT and not to the log
 * stream.
 *
 * Revision 1.32  2009-10-30 10:08:34  joergr
 * Option --processing-details now requires verbose mode.
 *
 * Revision 1.31  2009-10-14 10:51:56  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.30  2009-10-13 14:57:50  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.29  2008-09-25 14:14:21  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.28  2008-05-19 09:41:07  joergr
 * Added new command line options that enables reading of SR documents with
 * unknown/missing relationship type(s).
 *
 * Revision 1.27  2007/11/15 16:22:42  joergr
 * Fixed coding style to be more consistent.
 *
 * Revision 1.26  2006/08/15 16:40:02  meichel
 * Updated the code in module dcmsr to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.25  2006/07/27 14:52:00  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.24  2006/07/25 13:31:40  joergr
 * Fixed minor layout and formatting issues.
 *
 * Revision 1.23  2005/12/08 15:47:35  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.22  2005/12/02 10:37:30  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 *
 * Revision 1.21  2004/01/05 14:34:59  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.20  2003/10/30 17:44:19  joergr
 * Added new command line option which allows to print the template
 * identification of a content item.
 *
 * Revision 1.19  2003/10/06 09:56:10  joergr
 * Added new flag which allows to ignore content item errors when reading an SR
 * document (e.g. missing value type specific attributes).
 *
 * Revision 1.18  2002/11/26 08:45:35  meichel
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
 * Revision 1.15  2002/07/22 14:20:27  joergr
 * Added new command line option to suppress the output of general document
 * information.
 *
 * Revision 1.14  2002/04/16 13:49:53  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.13  2002/04/11 13:05:02  joergr
 * Use the new loadFile() and saveFile() routines from the dcmdata library.
 *
 * Revision 1.12  2002/01/11 14:35:56  joergr
 * Made spelling of option --print-filename consistent with other tools.
 *
 * Revision 1.11  2001/10/10 15:26:35  joergr
 * Additonal adjustments for new OFCondition class.
 *
 * Revision 1.10  2001/10/02 11:56:00  joergr
 * Adapted module "dcmsr" to the new class OFCondition. Introduced module
 * specific error codes.
 *
 * Revision 1.9  2001/09/26 13:04:01  meichel
 * Adapted dcmsr to class OFCondition
 *
 * Revision 1.8  2001/06/20 15:06:39  joergr
 * Added new debugging features (additional flags) to examine "corrupted" SR
 * documents.
 *
 * Revision 1.7  2001/06/01 15:50:58  meichel
 * Updated copyright header
 *
 * Revision 1.6  2001/04/03 08:22:53  joergr
 * Added new command line option: ignore relationship content constraints
 * specified for each SR document class.
 *
 * Revision 1.5  2000/11/07 18:36:05  joergr
 * Added useful code for debugging with dmalloc.
 *
 * Revision 1.4  2000/11/01 16:09:06  joergr
 * Updated comments/formatting.
 *
 * Revision 1.3  2000/10/18 16:56:33  joergr
 * Added new command line option --print-file-name.
 *
 * Revision 1.2  2000/10/16 11:50:31  joergr
 * Added new options: number nested items instead of indenting them, print SOP
 * instance UID of referenced composite objects.
 *
 * Revision 1.1  2000/10/13 07:46:21  joergr
 * Added new module 'dcmsr' providing access to DICOM structured reporting
 * documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
