/*
 *
 *  Copyright (C) 2000-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: List the contents of a dicom structured reporting file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-23 18:16:42 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmsr/apps/dsrdump.cc,v $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoc.h"
#include "dcdebug.h"
#include "cmdlnarg.h"
#include "ofstream.h"
#include "ofconapp.h"
#include "dcuid.h"       /* for dcmtk version name */

#ifdef WITH_ZLIB
#include "zlib.h"        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dsrdump"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition dumpFile(ostream &out,
                            const char *ifname,
                            const OFBool isDataset,
                            const E_TransferSyntax xfer,
                            const size_t readFlags,
                            const size_t printFlags,
                            const OFBool debugMode)
{
    OFCondition result = EC_Normal;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>" << endl;
        return EC_IllegalParameter;
    }

    DcmFileFormat *dfile = new DcmFileFormat();
    if (dfile != NULL)
    {
        if (isDataset)
            result = dfile->getDataset()->loadFile(ifname, xfer);
        else
            result = dfile->loadFile(ifname, xfer);
        if (result.bad())
        {
            CERR << OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                 << ") reading file: "<< ifname << endl;
        }
    } else
        result = EC_MemoryExhausted;

    if (result.good())
    {
        result = EC_CorruptedData;
        DSRDocument *dsrdoc = new DSRDocument();
        if (dsrdoc != NULL)
        {
            if (debugMode)
                dsrdoc->setLogStream(&ofConsole);
            result = dsrdoc->read(*dfile->getDataset(), readFlags);
            if (result.good())
            {
                result = dsrdoc->print(out, printFlags);
                out << endl;
            }
            else
            {
                CERR << OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                     << ") parsing file: "<< ifname << endl;
            }
        }
        delete dsrdoc;
    }
    delete dfile;

    return result;
}


#define SHORTCOL 3
#define LONGCOL 21


int main(int argc, char *argv[])
{
    int opt_debugMode = 0;
    size_t opt_readFlags = 0;
    size_t opt_printFlags = DSRTypes::PF_shortenLongItemValues;
    OFBool opt_printFilename = OFFalse;
    OFBool isDataset = OFFalse;
    E_TransferSyntax xfer = EXS_Unknown;

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Dump DICOM SR file and data set", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dsrfile-in", "DICOM SR input filename to be dumped", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                   "-h",  "print this help text and exit");
      cmd.addOption("--version",                       "print version information and exit", OFTrue /* exclusive */);
      cmd.addOption("--debug",                  "-d",  "debug mode, print debug information");
      cmd.addOption("--verbose-debug",          "-dd", "verbose debug mode, print more details");

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",            "+f",  "read file format or data set (default)");
        cmd.addOption("--read-dataset",         "-f",  "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
        cmd.addOption("--read-xfer-auto",       "-t=", "use TS recognition (default)");
        cmd.addOption("--read-xfer-little",     "-te", "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",        "-tb", "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",   "-ti", "read with implicit VR little endian TS");

    cmd.addGroup("parsing options:");
      cmd.addSubGroup("additional information:");
        cmd.addOption("--processing-details",   "-Ip", "show currently processed content item");
      cmd.addSubGroup("error handling:");
        cmd.addOption("--ignore-constraints",   "-Ec", "ignore relationship content constraints");
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

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
        /* check exclusive options first */
        if (cmd.getParamCount() == 0)
        {
          if (cmd.findOption("--version"))
          {
              app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
              CERR << endl << "External libraries used:";
#ifdef WITH_ZLIB
              CERR << endl << "- ZLIB, Version " << zlibVersion() << endl;
#else
              CERR << " none" << endl;
#endif
              return 0;
           }
        }

        /* options */
        if (cmd.findOption("--debug"))
            opt_debugMode = 2;
        if (cmd.findOption("--verbose-debug"))
        {
            opt_debugMode = 5;
            opt_readFlags |= DSRTypes::RF_verboseDebugMode;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-file"))
            isDataset = OFFalse;
        if (cmd.findOption("--read-dataset"))
            isDataset = OFTrue;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-xfer-auto"))
        {
            app.checkDependence("--read-xfer-auto", "--read-dataset", isDataset);
            xfer = EXS_Unknown;
        }
        if (cmd.findOption("--read-xfer-little"))
        {
            app.checkDependence("--read-xfer-little", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-big"))
        {
            app.checkDependence("--read-xfer-big", "--read-dataset", isDataset);
            xfer = EXS_BigEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-implicit"))
        {
            app.checkDependence("--read-xfer-implicit", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianImplicit;
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--processing-details"))
            opt_readFlags |= DSRTypes::RF_showCurrentlyProcessedItem;
        if (cmd.findOption("--ignore-constraints"))
            opt_readFlags |= DSRTypes::RF_ignoreRelationshipConstraints;
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
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    int errorCount = 0;
    const int count = cmd.getParamCount();
    const char *current = NULL;
    for (int i = 1; i <= count; i++)
    {
        cmd.getParam(i, current);
        if (opt_printFilename)
        {
            COUT << OFString(79, '-') << endl;
            COUT << OFFIS_CONSOLE_APPLICATION << " (" << i << "/" << count << "): " << current << endl << endl;
        }
        if (dumpFile(COUT, current, isDataset, xfer, opt_readFlags, opt_printFlags, opt_debugMode != 0).bad())
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
 * Revision 1.17  2002-09-23 18:16:42  joergr
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
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
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
