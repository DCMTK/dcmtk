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
 *  Purpose: List the contents of a dicom structured reporting file
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

#ifndef HAVE_WINDOWS_H
#define ANSI_ESCAPE_CODES_AVAILABLE
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
                            const size_t printFlags,
                            const OFBool convertToUTF8)
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

#ifdef WITH_LIBICONV
    if (result.good())
    {
        if (convertToUTF8)
        {
            OFLOG_INFO(dsrdumpLogger, "converting all element values that are affected by Specific Character Set (0008,0005) to UTF-8");
            result = dfile->convertToUTF8();
            if (result.bad())
            {
                OFLOG_FATAL(dsrdumpLogger, result.text() << ": converting file to UTF-8: " << ifname);
            }
        }
    }
#endif
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


#define SHORTCOL 4
#define LONGCOL 22


int main(int argc, char *argv[])
{
    size_t opt_readFlags = 0;
    size_t opt_printFlags = DSRTypes::PF_shortenLongItemValues;
    OFBool opt_printFilename = OFFalse;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    OFBool opt_convertToUTF8 = OFFalse;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Dump DICOM SR file and data set", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dsrfile-in", "DICOM SR input filename to be dumped", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                   "-h",   "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",            "+f",   "read file format or data set (default)");
        cmd.addOption("--read-file-only",       "+fo",  "read file format only");
        cmd.addOption("--read-dataset",         "-f",   "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",       "-t=",  "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",     "-td",  "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",     "-te",  "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",        "-tb",  "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",   "-ti",  "read with implicit VR little endian TS");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("additional information:");
        cmd.addOption("--processing-details",   "-Ip",  "show currently processed content item");
      cmd.addSubGroup("error handling:");
        cmd.addOption("--unknown-relationship", "-Er",  "accept unknown/missing relationship type");
        cmd.addOption("--invalid-item-value",   "-Ev",  "accept invalid content item value\n(e.g. violation of VR or VM definition)");
        cmd.addOption("--ignore-constraints",   "-Ec",  "ignore relationship content constraints");
        cmd.addOption("--ignore-item-errors",   "-Ee",  "do not abort on content item errors, just warn\n(e.g. missing value type specific attributes)");
        cmd.addOption("--skip-invalid-items",   "-Ei",  "skip invalid content items (incl. sub-tree)");
#ifdef WITH_LIBICONV
      cmd.addSubGroup("specific character set:");
        cmd.addOption("--convert-to-utf8",      "+U8",  "convert all element values that are affected\nby Specific Character Set (0008,0005) to UTF-8");
#endif

    cmd.addGroup("output options:");
      cmd.addSubGroup("general printing:");
        cmd.addOption("--print-filename",       "+Pf",  "print header with filename for each document");
        cmd.addOption("--no-document-header",   "-Ph",  "do not print general document information");
        cmd.addOption("--number-nested-items",  "+Pn",  "print position string in front of each line");
        cmd.addOption("--indent-nested-items",  "-Pn",  "indent nested items by spaces (default)");
      cmd.addSubGroup("printing values:");
        cmd.addOption("--print-long-values",    "+Pl",  "print long item values completely");
        cmd.addOption("--shorten-long-values",  "-Pl",  "print long item values shortened (default)");
        cmd.addOption("--print-instance-uid",   "+Pu",  "print SOP instance UID of referenced objects");
        cmd.addOption("--print-sopclass-short", "-Ps",  "print short SOP class name of referenced\nimage objects, e.g. \"CT image\" (default)");
        cmd.addOption("--print-sopclass-long",  "+Ps",  "print long SOP class name of ref. objects");
        cmd.addOption("--print-sopclass-uid",   "+Psu", "print SOP class UID of referenced objects");
        cmd.addOption("--print-all-codes",      "+Pc",  "print all codes (incl. concept name codes)");
        cmd.addOption("--print-invalid-codes",  "+Pi",  "print invalid codes (for debugging purposes)");
        cmd.addOption("--no-invalid-codes",     "-Pi",  "print text \"invalid code\" instead (default)");
        cmd.addOption("--print-template-id",    "+Pt",  "print template identification information");
#ifdef ANSI_ESCAPE_CODES_AVAILABLE
      cmd.addSubGroup("color:");
        cmd.addOption("--print-color",          "+C",   "use ANSI escape codes for colored output");
        cmd.addOption("--no-color",             "-C",   "do not use any ANSI escape codes (default)");
#endif

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
            app.checkDependence("--processing-details", "verbose mode", dsrdumpLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
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
#ifdef WITH_LIBICONV
        if (cmd.findOption("--convert-to-utf8")) opt_convertToUTF8 = OFTrue;
#endif

        /* output options */
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

        cmd.beginOptionBlock();
        if (cmd.findOption("--print-sopclass-short"))
            opt_printFlags = (opt_printFlags & ~(DSRTypes::PF_printLongSOPClassName | DSRTypes::PF_printSOPClassUID));
        if (cmd.findOption("--print-sopclass-long"))
            opt_printFlags = (opt_printFlags & ~DSRTypes::PF_printSOPClassUID) | DSRTypes::PF_printLongSOPClassName;
        if (cmd.findOption("--print-sopclass-uid"))
            opt_printFlags = (opt_printFlags & ~DSRTypes::PF_printLongSOPClassName) | DSRTypes::PF_printSOPClassUID;
        cmd.endOptionBlock();

        if (cmd.findOption("--print-all-codes"))
            opt_printFlags |= DSRTypes::PF_printAllCodes;

        cmd.beginOptionBlock();
        if (cmd.findOption("--print-invalid-codes"))
            opt_printFlags |= DSRTypes::PF_printInvalidCodes;
        if (cmd.findOption("--no-invalid-codes"))
            opt_printFlags &= ~DSRTypes::PF_printInvalidCodes;
        cmd.endOptionBlock();

        if (cmd.findOption("--print-template-id"))
            opt_printFlags |= DSRTypes::PF_printTemplateIdentification;

#ifdef ANSI_ESCAPE_CODES_AVAILABLE
        cmd.beginOptionBlock();
        if (cmd.findOption("--print-color"))
            opt_printFlags |= DSRTypes::PF_useANSIEscapeCodes;
        if (cmd.findOption("--no-color"))
            opt_printFlags &= ~DSRTypes::PF_useANSIEscapeCodes;
        cmd.endOptionBlock();
#endif
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
        if (dumpFile(COUT, current, opt_readMode, opt_ixfer, opt_readFlags, opt_printFlags, opt_convertToUTF8).bad())
            errorCount++;
    }

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif
    return errorCount;
}
