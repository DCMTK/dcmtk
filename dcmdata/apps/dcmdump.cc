/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: List the contents of a dicom file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-29 13:54:22 $
 *  CVS/RCS Revision: $Revision: 1.86 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/dcistrmz.h"   /* for dcmZlibExpectRFC1950Encoding */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#if defined (HAVE_WINDOWS_H) || defined(HAVE_FNMATCH_H)
#define PATTERN_MATCHING_AVAILABLE
#endif

#ifndef HAVE_WINDOWS_H
#define ANSI_ESCAPE_CODES_AVAILABLE
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmdump"

static OFLogger dcmdumpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#ifdef HAVE_GUSI_H
/* needed for Macintosh */
#include <GUSI.h>
#include <SIOUX.h>
#endif

static int dumpFile(STD_NAMESPACE ostream &out,
                    const char *ifname,
                    const E_FileReadMode readMode,
                    const E_TransferSyntax xfer,
                    const size_t printFlags,
                    const OFBool loadIntoMemory,
                    const OFBool stopOnErrors,
                    const char *pixelDirectory);

// ********************************************

static OFBool printFilename = OFFalse;
static OFBool printFileSearch = OFFalse;
static OFBool printAllInstances = OFTrue;
static OFBool prependSequenceHierarchy = OFFalse;
static int printTagCount = 0;
static const int MAX_PRINT_TAG_NAMES = 1024;
static const char *printTagNames[MAX_PRINT_TAG_NAMES];
static const DcmTagKey *printTagKeys[MAX_PRINT_TAG_NAMES];
static OFCmdUnsignedInt maxReadLength = 4096; // default is 4 KB
static size_t fileCounter = 0;


static DcmTagKey parseTagKey(const char *tagName)
{
    unsigned int group = 0xffff;
    unsigned int elem = 0xffff;
    if (sscanf(tagName, "%x,%x", &group, &elem) != 2)
    {
        DcmTagKey tagKey;
        /* it is a name */
        const DcmDataDictionary &globalDataDict = dcmDataDict.rdlock();
        const DcmDictEntry *dicent = globalDataDict.findEntry(tagName);
        if (dicent == NULL) {
            OFLOG_WARN(dcmdumpLogger, "unrecognized tag name: '" << tagName << "'");
            tagKey = DCM_UndefinedTagKey;
        } else {
            tagKey = dicent->getKey();
        }
        dcmDataDict.unlock();
        return tagKey;
    } else     /* tag name has format "gggg,eeee" */
    {
        return DcmTagKey(group,elem);
    }
}

static OFBool addPrintTagName(const char *tagName)
{
    if (printTagCount >= MAX_PRINT_TAG_NAMES) {
        OFLOG_WARN(dcmdumpLogger, "too many print tag options (max: " << MAX_PRINT_TAG_NAMES << ")");
        return OFFalse;
    }

    unsigned int group = 0xffff;
    unsigned int elem = 0xffff;
    if (sscanf(tagName, "%x,%x", &group, &elem) != 2)
    {
        /* it is a name */
        const DcmDataDictionary &globalDataDict = dcmDataDict.rdlock();
        const DcmDictEntry *dicent = globalDataDict.findEntry(tagName);
        if (dicent == NULL) {
            OFLOG_WARN(dcmdumpLogger, "unrecognized tag name: '" << tagName << "'");
            dcmDataDict.unlock();
            return OFFalse;
        } else {
            /* note for later */
            printTagKeys[printTagCount] = new DcmTagKey(dicent->getKey());
        }
        dcmDataDict.unlock();
    } else {
        /* tag name has format xxxx,xxxx */
        /* do not lookup in dictionary, tag could be private */
        printTagKeys[printTagCount] = NULL;
    }

    printTagNames[printTagCount] = strcpy(OFstatic_cast(char*, malloc(strlen(tagName)+1)), tagName);
    printTagCount++;
    return OFTrue;
}


#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{
    OFBool loadIntoMemory = OFTrue;
    size_t printFlags = DCMTypes::PF_shortenLongTagValues;
    E_FileReadMode readMode = ERM_autoDetect;
    E_TransferSyntax xfer = EXS_Unknown;
    OFBool stopOnErrors = OFTrue;
    OFBool scanDir = OFFalse;
    OFBool recurse = OFFalse;
    const char *scanPattern = "";
    const char *pixelDirectory = NULL;

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    /* set options for the Metrowerks CodeWarrior SIOUX console */
    SIOUXSettings.autocloseonquit = OFFalse;
    SIOUXSettings.asktosaveonclose = OFFalse;
    SIOUXSettings.showstatusline = OFTrue;
    SIOUXSettings.setupmenus = OFTrue;
    /* set options for the GUSI sockets library */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Dump DICOM file and data set", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in", "DICOM input file or directory to be dumped", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",           "+f",     "read file format or data set (default)");
        cmd.addOption("--read-file-only",      "+fo",    "read file format only");
        cmd.addOption("--read-dataset",        "-f",     "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",      "-t=",    "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",    "-td",    "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",    "-te",    "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",       "-tb",    "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",  "-ti",    "read with implicit VR little endian TS");
      cmd.addSubGroup("input files:");
        cmd.addOption("--scan-directories",    "+sd",    "scan directories for input files (dcmfile-in)");
#ifdef PATTERN_MATCHING_AVAILABLE
        cmd.addOption("--scan-pattern",        "+sp", 1, "[p]attern: string (only with --scan-directories)",
                                                         "pattern for filename matching (wildcards)");
#endif
        cmd.addOption("--no-recurse",          "-r",     "do not recurse within directories (default)");
        cmd.addOption("--recurse",             "+r",     "recurse within specified directories");
      cmd.addSubGroup("parsing of file meta information:");
        cmd.addOption("--use-meta-length",     "+ml",    "use file meta information group length (default)");
        cmd.addOption("--ignore-meta-length",  "-ml",    "ignore file meta information group length");
      cmd.addSubGroup("parsing of odd-length attributes:");
        cmd.addOption("--accept-odd-length",   "+ao",    "accept odd length attributes (default)");
        cmd.addOption("--assume-even-length",  "+ae",    "assume real length is one byte larger");
      cmd.addSubGroup("handling of non-standard VR:");
        cmd.addOption("--treat-as-unknown",    "+vr",    "treat non-standard VR as unknown (default)");
        cmd.addOption("--assume-implicit",     "-vr",    "try to read with implicit VR little endian TS");
      cmd.addSubGroup("handling of undefined length UN elements:");
        cmd.addOption("--enable-cp246",        "+ui",    "read undefined len UN as implicit VR (default)");
        cmd.addOption("--disable-cp246",       "-ui",    "read undefined len UN as explicit VR");
      cmd.addSubGroup("handling of defined length UN elements:");
        cmd.addOption("--retain-un",           "-uc",    "retain elements as UN (default)");
        cmd.addOption("--convert-un",          "+uc",    "convert to real VR if known");
      cmd.addSubGroup("handling of private max-length elements (implicit VR):");
        cmd.addOption("--maxlength-dict",      "-sq",    "read as defined in dictionary (default)");
        cmd.addOption("--maxlength-seq",       "+sq",    "read as sequence with undefined length");
      cmd.addSubGroup("general handling of parser errors: ");
        cmd.addOption("--ignore-parse-errors", "+Ep",    "try to recover from parse errors");
        cmd.addOption("--handle-parse-errors", "-Ep",    "handle parse errors and stop parsing (default)");
      cmd.addSubGroup("other parsing options:");
        cmd.addOption("--stop-after-elem",     "+st", 1, "[t]ag: \"gggg,eeee\" or dictionary name",
                                                         "stop parsing after element specified by t");
      cmd.addSubGroup("automatic data correction:");
        cmd.addOption("--enable-correction",   "+dc",    "enable automatic data correction (default)");
        cmd.addOption("--disable-correction",  "-dc",    "disable automatic data correction");
#ifdef WITH_ZLIB
      cmd.addSubGroup("bitstream format of deflated input:");
        cmd.addOption("--bitstream-deflated",  "+bd",    "expect deflated bitstream (default)");
        cmd.addOption("--bitstream-zlib",      "+bz",    "expect deflated zlib bitstream");
#endif

    cmd.addGroup("output options:");
      cmd.addSubGroup("loading:");
        cmd.addOption("--load-all",            "+M",     "load very long tag values (default)");
        cmd.addOption("--load-short",          "-M",     "do not load very long values (e.g. pixel data)");
        cmd.addOption("--max-read-length",     "+R",  1, "[k]bytes: integer (4..4194302, default: 4)",
                                                         "set threshold for long values to k kbytes");
      cmd.addSubGroup("printing:");
        cmd.addOption("--print-all",           "+L",     "print long tag values completely");
        cmd.addOption("--print-short",         "-L",     "print long tag values shortened (default)");
        cmd.addOption("--print-tree",          "+T",     "print hierarchical structure as a simple tree");
        cmd.addOption("--print-indented",      "-T",     "print hierarchical structure indented (default)");
        cmd.addOption("--print-filename",      "+F",     "print header with filename for each input file");
        cmd.addOption("--print-file-search",   "+Fs",    "print header with filename only for those input\nfiles that contain one of the searched tags");
        cmd.addOption("--map-uid-names",       "+Un",    "map well-known UID numbers to names (default)");
        cmd.addOption("--no-uid-names",        "-Un",    "do not map well-known UID numbers to names");
        cmd.addOption("--quote-nonascii",      "+Qn",    "quote non-ASCII and control chars as XML markup");
        cmd.addOption("--print-nonascii",      "-Qn",    "print non-ASCII and control chars (default)");
#ifdef ANSI_ESCAPE_CODES_AVAILABLE
        cmd.addOption("--print-color",         "+C",     "use ANSI escape codes for colored output");
#endif

      cmd.addSubGroup("error handling:");
        cmd.addOption("--stop-on-error",       "-E",     "do not print if file is damaged (default)");
        cmd.addOption("--ignore-errors",       "+E",     "attempt to print even if file is damaged");

      cmd.addSubGroup("searching:");
        cmd.addOption("--search",              "+P",  1, "[t]ag: \"gggg,eeee\" or dictionary name",
                                                         "print the value of tag t\nthis option can be specified multiple times\n(default: the complete file is printed)");
        cmd.addOption("--search-all",          "+s",     "print all instances of searched tags (default)");
        cmd.addOption("--search-first",        "-s",     "only print first instance of searched tags");
        cmd.addOption("--prepend",             "+p",     "prepend sequence hierarchy to printed tag,\ndenoted by: (gggg,eeee).(gggg,eeee).*\n(only with --search-all or --search-first)");
        cmd.addOption("--no-prepend",          "-p",     "do not prepend hierarchy to tag (default)");

      cmd.addSubGroup("writing:");
        cmd.addOption("--write-pixel",         "+W",  1, "[d]irectory: string",
                                                         "write pixel data to a .raw file stored in d\n(little endian, filename created automatically)");

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
      if (cmd.findOption("--read-file")) readMode = ERM_autoDetect;
      if (cmd.findOption("--read-file-only")) readMode = ERM_fileOnly;
      if (cmd.findOption("--read-dataset")) readMode = ERM_dataset;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-xfer-auto"))
        xfer = EXS_Unknown;
      if (cmd.findOption("--read-xfer-detect"))
        dcmAutoDetectDatasetXfer.set(OFTrue);
      if (cmd.findOption("--read-xfer-little"))
      {
        app.checkDependence("--read-xfer-little", "--read-dataset", readMode == ERM_dataset);
        xfer = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-big"))
      {
        app.checkDependence("--read-xfer-big", "--read-dataset", readMode == ERM_dataset);
        xfer = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-implicit"))
      {
        app.checkDependence("--read-xfer-implicit", "--read-dataset", readMode == ERM_dataset);
        xfer = EXS_LittleEndianImplicit;
      }
      cmd.endOptionBlock();

      if (cmd.findOption("--scan-directories")) scanDir = OFTrue;
#ifdef PATTERN_MATCHING_AVAILABLE
      if (cmd.findOption("--scan-pattern"))
      {
        app.checkDependence("--scan-pattern", "--scan-directories", scanDir);
        app.checkValue(cmd.getValue(scanPattern));
      }
#endif
      cmd.beginOptionBlock();
      if (cmd.findOption("--no-recurse")) recurse = OFFalse;
      if (cmd.findOption("--recurse"))
      {
        app.checkDependence("--recurse", "--scan-directories", scanDir);
        recurse = OFTrue;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--use-meta-length"))
      {
        dcmIgnoreFileMetaInformationGroupLength.set(OFFalse);
      }
      if (cmd.findOption("--ignore-meta-length"))
      {
        dcmIgnoreFileMetaInformationGroupLength.set(OFTrue);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--accept-odd-length"))
      {
        dcmAcceptOddAttributeLength.set(OFTrue);
      }
      if (cmd.findOption("--assume-even-length"))
      {
        dcmAcceptOddAttributeLength.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--treat-as-unknown"))
      {
        dcmAcceptUnexpectedImplicitEncoding.set(OFFalse);
      }
      if (cmd.findOption("--assume-implicit"))
      {
        dcmAcceptUnexpectedImplicitEncoding.set(OFTrue);
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-cp246"))
      {
        dcmEnableCP246Support.set(OFTrue);
      }
      if (cmd.findOption("--disable-cp246"))
      {
        dcmEnableCP246Support.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--retain-un"))
      {
        dcmEnableUnknownVRConversion.set(OFFalse);
      }
      if (cmd.findOption("--convert-un"))
      {
        dcmEnableUnknownVRConversion.set(OFTrue);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--maxlength-dict"))
      {
        dcmReadImplPrivAttribMaxLengthAsSQ.set(OFFalse);
      }
      if (cmd.findOption("--maxlength-seq"))
      {
        dcmReadImplPrivAttribMaxLengthAsSQ.set(OFTrue);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--ignore-parse-errors"))
      {
        dcmIgnoreParsingErrors.set(OFTrue);
      }
      if (cmd.findOption("--handle-parse-errors"))
      {
        dcmIgnoreParsingErrors.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-correction"))
      {
        dcmEnableAutomaticInputDataCorrection.set(OFTrue);
      }
      if (cmd.findOption("--disable-correction"))
      {
        dcmEnableAutomaticInputDataCorrection.set(OFFalse);
      }
      cmd.endOptionBlock();

#ifdef WITH_ZLIB
      cmd.beginOptionBlock();
      if (cmd.findOption("--bitstream-deflated"))
      {
        dcmZlibExpectRFC1950Encoding.set(OFFalse);
      }
      if (cmd.findOption("--bitstream-zlib"))
      {
        dcmZlibExpectRFC1950Encoding.set(OFTrue);
      }
      cmd.endOptionBlock();
#endif

      if (cmd.findOption("--max-read-length"))
      {
        app.checkValue(cmd.getValueAndCheckMinMax(maxReadLength, 4, 4194302));
        maxReadLength *= 1024; // convert kbytes to bytes
      }
      cmd.beginOptionBlock();
      if (cmd.findOption("--load-all")) loadIntoMemory = OFTrue;
      if (cmd.findOption("--load-short")) loadIntoMemory = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--print-all")) printFlags &= ~DCMTypes::PF_shortenLongTagValues;
      if (cmd.findOption("--print-short")) printFlags |= DCMTypes::PF_shortenLongTagValues;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--print-tree")) printFlags |= DCMTypes::PF_showTreeStructure;
      if (cmd.findOption("--print-indented")) printFlags &= ~DCMTypes::PF_showTreeStructure;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--print-filename"))
      {
        printFilename = OFTrue;
        printFileSearch = OFFalse;
      }
      if (cmd.findOption("--print-file-search"))
      {
        printFileSearch = OFTrue;
        printFilename = OFFalse;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--map-uid-names")) printFlags &= ~DCMTypes::PF_doNotMapUIDsToNames;
      if (cmd.findOption("--no-uid-names")) printFlags |= DCMTypes::PF_doNotMapUIDsToNames;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--quote-nonascii")) printFlags |= DCMTypes::PF_convertToMarkup;
      if (cmd.findOption("--print-nonascii")) printFlags &= ~DCMTypes::PF_convertToMarkup;
      cmd.endOptionBlock();

#ifdef ANSI_ESCAPE_CODES_AVAILABLE
      if (cmd.findOption("--print-color")) printFlags |= DCMTypes::PF_useANSIEscapeCodes;
#endif

      cmd.beginOptionBlock();
      if (cmd.findOption("--stop-on-error")) stopOnErrors = OFTrue;
      if (cmd.findOption("--ignore-errors")) stopOnErrors = OFFalse;
      cmd.endOptionBlock();

      if (cmd.findOption("--stop-after-elem"))
      {
        const char *tagName = NULL;
        app.checkValue(cmd.getValue(tagName));
        DcmTagKey key = parseTagKey(tagName);
        if (key != DCM_UndefinedTagKey)
          dcmStopParsingAfterElement.set(key);
        else
          app.printError("no valid key given for option --stop-after-elem");
      }

      if (cmd.findOption("--search", 0, OFCommandLine::FOM_FirstFromLeft))
      {
        const char *tagName = NULL;
        do
        {
          app.checkValue(cmd.getValue(tagName));
          if (!addPrintTagName(tagName)) return 1;
        } while (cmd.findOption("--search", 0, OFCommandLine::FOM_NextFromLeft));
      }

      if (printFileSearch)
        app.checkDependence("--print-file-search", "--search", printTagCount > 0);

      cmd.beginOptionBlock();
      if (cmd.findOption("--search-all"))
      {
        app.checkDependence("--search-all", "--search", printTagCount > 0);
        printAllInstances = OFTrue;
      }
      if (cmd.findOption("--search-first"))
      {
        app.checkDependence("--search-first", "--search", printTagCount > 0);
        printAllInstances = OFFalse;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--prepend"))
      {
        app.checkDependence("--prepend", "--search", printTagCount > 0);
        prependSequenceHierarchy = OFTrue;
      }
      if (cmd.findOption("--no-prepend"))
      {
        app.checkDependence("--no-prepend", "--search", printTagCount > 0);
        prependSequenceHierarchy = OFFalse;
      }
      cmd.endOptionBlock();
      app.checkConflict("--prepend", "--print-tree", prependSequenceHierarchy && (printFlags & DCMTypes::PF_showTreeStructure) > 0);

      if (cmd.findOption("--write-pixel"))
        app.checkValue(cmd.getValue(pixelDirectory));
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmdumpLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      OFLOG_WARN(dcmdumpLogger, "no data dictionary loaded, check environment variable: "
        << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* make sure the pixel data directory exists and is writable */
    if (pixelDirectory != NULL)
    {
      if (!OFStandard::dirExists(pixelDirectory))
      {
        OFLOG_WARN(dcmdumpLogger, "directory specified for --write-pixel does not exist, ignoring this option");
        pixelDirectory = NULL;
      }
      else if (!OFStandard::isWriteable(pixelDirectory))
      {
        OFLOG_WARN(dcmdumpLogger, "directory specified for --write-pixel is not writeable, ignoring this option");
        pixelDirectory = NULL;
      }
    }

    int errorCount = 0;

    /* create list of input files */
    const char *paramString = NULL;
    const int paramCount = cmd.getParamCount();
    OFList<OFString> inputFiles;
    /* iterate over all input filenames */
    for (int i = 1; i <= paramCount; i++)
    {
      cmd.getParam(i, paramString);
      /* search directory recursively (if required) */
      if (OFStandard::dirExists(paramString))
      {
        if (scanDir)
          OFStandard::searchDirectoryRecursively(paramString, inputFiles, scanPattern, "" /*dirPrefix*/, recurse);
        else
          OFLOG_WARN(dcmdumpLogger, "ignoring directory because option --scan-directories is not set: " << paramString);
      } else
        inputFiles.push_back(paramString);
    }
    /* check whether there are any input files at all */
    if (inputFiles.empty())
    {
      OFLOG_FATAL(dcmdumpLogger, "no input files to be dumped");
      return 1;
    }

    const size_t count = inputFiles.size();
    const char *current = NULL;
    OFListIterator(OFString) if_iter = inputFiles.begin();
    OFListIterator(OFString) if_last = inputFiles.end();
    /* iterate over all input filenames */
    while (if_iter != if_last)
    {
      current = (*if_iter++).c_str();
      if (printFilename)
      {
        /* a newline separates two consecutive "dumps" */
        if (++fileCounter > 1)
          COUT << OFendl;
        /* print header with filename */
        COUT << "# " << OFFIS_CONSOLE_APPLICATION << " (" << fileCounter << "/" << count << "): " << current << OFendl;
      }
      errorCount += dumpFile(COUT, current, readMode, xfer, printFlags, loadIntoMemory, stopOnErrors, pixelDirectory);
    }

    return errorCount;
}

static void printResult(STD_NAMESPACE ostream &out,
                        DcmStack &stack,
                        size_t printFlags,
                        const char*pixelFileName = NULL,
                        size_t *pixelCounter = NULL)
{
    unsigned long n = stack.card();
    if (n == 0) {
        return;
    }

    if (prependSequenceHierarchy) {
        if (printFlags & DCMTypes::PF_useANSIEscapeCodes)
            out << ANSI_ESCAPE_CODE_TAG;
        /* print the path leading up to the top stack elem */
        for (unsigned long i = n - 1; i >= 1; i--) {
            DcmObject *dobj = stack.elem(i);
            /* do not print if a DCM_Item as this is not
             * very helpful to distinguish instances.
             */
            if (dobj != NULL && dobj->getTag().getXTag() != DCM_Item) {
                out << dobj->getTag() << ".";
            }
        }
    }

    /* print the tag and its value */
    DcmObject *dobj = stack.top();
    dobj->print(out, printFlags, 1 /*level*/, pixelFileName, pixelCounter);
}

static int dumpFile(STD_NAMESPACE ostream &out,
                    const char *ifname,
                    const E_FileReadMode readMode,
                    const E_TransferSyntax xfer,
                    const size_t printFlags,
                    const OFBool loadIntoMemory,
                    const OFBool stopOnErrors,
                    const char *pixelDirectory)
{
    int result = 0;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        OFLOG_ERROR(dcmdumpLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
        return 1;
    }

    DcmFileFormat dfile;
    DcmObject *dset = &dfile;
    if (readMode == ERM_dataset) dset = dfile.getDataset();
    OFCondition cond = dfile.loadFile(ifname, xfer, EGL_noChange, maxReadLength, readMode);
    if (cond.bad())
    {
        OFLOG_ERROR(dcmdumpLogger, OFFIS_CONSOLE_APPLICATION << ": " << cond.text()
            << ": reading file: "<< ifname);
        result = 1;
        if (stopOnErrors) return result;
    }

    if (loadIntoMemory) dfile.loadAllDataIntoMemory();

    size_t pixelCounter = 0;
    const char *pixelFileName = NULL;
    OFString pixelFilenameStr;
    if (pixelDirectory != NULL)
    {
        /* create filename for pixel data */
        OFString fileName;
        OFStandard::getFilenameFromPath(fileName, ifname);
        OFStandard::combineDirAndFilename(pixelFilenameStr, pixelDirectory, fileName);
        pixelFileName = pixelFilenameStr.c_str();
    }

    /* dump complete file content */
    if (printTagCount == 0)
    {
        dset->print(out, printFlags, 0 /*level*/, pixelFileName, &pixelCounter);
    } else {
        OFBool firstTag = OFTrue;
        /* only print specified tags */
        for (int i = 0; i < printTagCount; i++)
        {
            unsigned int group = 0xffff;
            unsigned int elem = 0xffff;
            DcmTagKey searchKey;
            const char *tagName = printTagNames[i];
            if (printTagKeys[i])
                searchKey = *printTagKeys[i];
            else if (sscanf(tagName, "%x,%x", &group, &elem) == 2)
                searchKey.set(group, elem);
            else {
                OFLOG_FATAL(dcmdumpLogger, "Internal ERROR in File " << __FILE__ << ", Line "
                    << __LINE__ << OFendl << " -- Named tag inconsistency");
                abort();
            }

            DcmStack stack;
            if (dset->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal)
            {
                if (firstTag)
                {
                    if (!printFilename)
                    {
                        /* a newline separates two consecutive "dumps" */
                        if (++fileCounter > 1)
                            COUT << OFendl;
                    }
                    /* print header with filename */
                    if (printFileSearch)
                        COUT << "# " << OFFIS_CONSOLE_APPLICATION << " (" << fileCounter << "): " << ifname << OFendl;
                    firstTag = OFFalse;
                }
                printResult(out, stack, printFlags, pixelFileName, &pixelCounter);
                if (printAllInstances)
                {
                    while (dset->search(searchKey, stack, ESM_afterStackTop, OFTrue) == EC_Normal)
                      printResult(out, stack, printFlags, pixelFileName, &pixelCounter);
                }
            }
        }
    }
    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dcmdump.cc,v $
 * Revision 1.86  2010-10-29 13:54:22  joergr
 * Remove new --print-color option for Windows system since the ANSI escape
 * codes are not supported by the standard command shell.
 *
 * Revision 1.85  2010-10-29 10:59:34  joergr
 * Added new option for colored output of the textual dump.
 *
 * Revision 1.84  2010-10-14 13:13:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.83  2010-07-21 12:54:45  joergr
 * Made sure that no NULL pointer is passed to the OFString constructor. This
 * occurred when option --scan-directories was used without --scan-pattern.
 *
 * Revision 1.82  2010-03-23 15:17:24  joergr
 * Use printError() method for command line parsing errors only. After the
 * resource identifier has been printed to the log stream use "oflog" instead.
 *
 * Revision 1.81  2010-03-04 09:42:42  joergr
 * Fixed possible issue with read locks on global data dictionary.
 * Use return value of getTag() for stream output where possible.
 *
 * Revision 1.80  2009-11-13 13:20:23  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.79  2009-11-04 09:58:06  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.78  2009-06-04 16:50:18  joergr
 * Added new command line option that allows for ignoring the value of File Meta
 * Information Group Length (0002,0000).
 *
 * Revision 1.77  2009-04-24 12:20:41  joergr
 * Fixed minor inconsistencies regarding layout/formatting in syntax usage.
 *
 * Revision 1.76  2009-04-21 14:02:49  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.75  2009-03-18 13:57:16  joergr
 * Removed redundant call to OFString constructor.
 *
 * Revision 1.74  2009-03-13 10:13:36  joergr
 * Added check on pixel data directory (option --write-pixel), i.e. whether it
 * exists and whether it is writable. Added support for option --write-pixel
 * when --search is used to print selected tags only.
 *
 * Revision 1.73  2009-02-11 16:55:31  joergr
 * Renamed option --stop-at-elem to --stop-after-elem and fixed typo.
 *
 * Revision 1.72  2009-02-11 13:16:17  onken
 * Added global parser flag permitting to stop parsing after a specific
 * element was parsed on dataset level (useful for removing garbage at
 * end of file).
 *
 * Revision 1.71  2009-02-04 14:30:07  joergr
 * Fixed small issue with syntax usage layout.
 * Introduced new syntax usage output subsection "loading".
 *
 * Revision 1.70  2009-02-04 14:07:10  onken
 * Added command line options making use of the parser's error ignoring flag.
 *
 * Revision 1.69  2009-01-29 15:36:52  onken
 * *** empty log message ***
 *
 * Revision 1.68  2009-01-06 16:30:29  joergr
 * Made command line option --quiet visible by default (not only in experimental
 * mode).
 * Added new command line options for changing the output format (tree structure
 * vs. indented output).
 *
 * Revision 1.67  2009-01-05 15:30:15  joergr
 * Added command line options that allow for reading incorrectly encoded DICOM
 * datasets where particular data elements are encoded with a differing transfer
 * syntax (Implicit VR Little endian instead of Explicit VR encoding).
 *
 * Revision 1.66  2008-09-25 14:53:12  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.65  2008-09-25 11:16:32  joergr
 * Added method for printing the resource identifier of an application.
 *
 * Revision 1.64  2008-09-24 13:30:24  joergr
 * Added support for printing the expanded command line arguments to standard
 * output stream.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.63  2008-04-28 09:30:10  meichel
 * Implemented new command line option --quote-nonascii in dcmdump that
 * quotes non-ASCII and control characters as XML markup.
 *
 * Revision 1.62  2008-04-16 12:36:28  joergr
 * Changed order of command line evaluation for option --search (now: from left
 * to right).
 *
 * Revision 1.61  2008-02-26 16:54:12  joergr
 * Added new command line option that disables the mapping of well-known UID
 * numbers to their associated names (e.g. transfer syntax or SOP class).
 *
 * Revision 1.60  2007/10/24 17:13:33  joergr
 * Added new command line option which prints a header with the filename only
 * for those input files that contain one of the searched tags.
 * Fixed small layout and formatting issues in the dump output (missing
 * indentation and leading zeros).
 * Replaced sprintf() call by an appropriate use of the C++ stream manipulators.
 *
 * Revision 1.59  2007/10/01 16:52:38  joergr
 * Added support for searching directories recursively for DICOM files.
 * Enhanced experimental quiet mode.
 *
 * Revision 1.58  2007/03/09 14:58:31  joergr
 * Fixed wrong output of status variable after calling loadFile().
 *
 * Revision 1.57  2006/08/15 15:50:56  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.56  2006/07/27 13:52:42  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.55  2005/12/08 15:40:46  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.54  2005/12/07 16:42:49  onken
 * Changed default and minimum value of --max-read-length to 4 KB
 *
 * Revision 1.53  2005/12/02 09:16:17  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 * Added experimental support for a "quiet" mode.
 * Made description of option --max-read-length more consistent with the other
 * command line tools.
 *
 * Revision 1.52  2005/12/01 11:25:44  joergr
 * Removed superfluous local variable. Changed type of variable "maxReadLength".
 *
 * Revision 1.51  2005/11/17 11:26:11  onken
 * Option --max-read-length now uses OFCommandLine to check, whether option
 * value is in range
 *
 * Revision 1.50  2005/11/16 14:59:11  onken
 * *** empty log message ***
 *
 * Revision 1.49  2005/11/16 14:55:56  onken
 * Added "--max-read-length" option to dcmdump and dcm2xml to override
 * DCMTK-internal threshold (4096K) for long attribute values.
 *
 * Revision 1.48  2005/11/15 18:33:20  meichel
 * Added new command line option --convert-un that enables the re-conversion of
 *   defined length UN elements.
 *
 * Revision 1.47  2004/01/16 10:52:42  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.46  2002/12/13 13:44:29  meichel
 * Fixed problem in dcmdump, --ignore-errors now works correctly again.
 *
 * Revision 1.45  2002/12/06 12:05:44  joergr
 * Enhanced "print()" function by re-working the implementation and replacing
 * the boolean "showFullData" parameter by a more general integer flag.
 *
 * Revision 1.44  2002/11/27 12:07:16  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.43  2002/11/26 08:43:00  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.42  2002/09/23 17:52:02  joergr
 * Prepared code for future support of 'config.guess' host identifiers.
 *
 * Revision 1.41  2002/09/23 13:50:41  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used.
 *
 * Revision 1.40  2002/08/27 16:55:26  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.39  2002/08/20 12:19:57  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.38  2002/08/02 08:43:16  joergr
 * Fixed bug in dcmdump that was preventing the +p option from working.
 *
 * Revision 1.37  2002/07/08 14:44:54  meichel
 * Improved dcmdata behaviour when reading odd tag length. Depending on the
 *   global boolean flag dcmAcceptOddAttributeLength, the parser now either accepts
 *   odd length attributes or implements the old behaviour, i.e. assumes a real
 *   length larger by one.
 *
 * Revision 1.36  2002/04/16 13:38:53  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.35  2002/01/11 14:34:20  joergr
 * Added new option to dcmdump tool which prints a header with the filename.
 *
 * Revision 1.34  2001/09/28 14:18:45  joergr
 * Changed formatting.
 *
 * Revision 1.33  2001/09/25 17:20:59  meichel
 * Adapted dcmdata to class OFCondition
 *
 * Revision 1.32  2001/06/01 15:48:28  meichel
 * Updated copyright header
 *
 * Revision 1.31  2000/06/14 11:26:47  joergr
 * Corrected wrong filename creation for option +W.
 *
 * Revision 1.30  2000/05/03 14:19:05  meichel
 * Added new class GlobalDcmDataDictionary which implements read/write lock
 *   semantics for safe access to the DICOM dictionary from multiple threads
 *   in parallel. The global dcmDataDict now uses this class.
 *
 * Revision 1.29  2000/03/08 16:26:05  meichel
 * Updated copyright header.
 *
 * Revision 1.28  2000/03/06 18:09:37  joergr
 * Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
 *
 * Revision 1.27  2000/03/03 14:05:15  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.26  2000/02/23 15:11:31  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.25  2000/02/10 11:05:25  joergr
 * Modified help text for new command line option.
 *
 * Revision 1.23  2000/02/01 10:11:57  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.22  1999/04/27 17:50:50  joergr
 * Adapted console applications to new OFCommandLine and OFConsoleApplication
 * functionality.
 *
 * Revision 1.21  1999/04/26 16:38:25  joergr
 * Removed bug: empty parameters have always been interpreted as options.
 * Added support to check dependences between different options and report
 * error messages if necessary.
 *
 * Revision 1.20  1999/03/31 09:24:19  meichel
 * Updated copyright header in module dcmdata
 *
 * Revision 1.19  1999/03/29 10:14:12  meichel
 * Adapted command line options of dcmdata applications to new scheme.
 *
 * Revision 1.18  1999/03/22 16:12:16  meichel
 * Added -d <debuglevel> flag to dcmdump.
 *
 * Revision 1.17  1997/07/21 08:04:24  andreas
 * - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
 *   with one unique boolean type OFBool.
 * - With flag DEBUG dcmdump now tries to print the DICOM file even if
 *   an error in reading the file was detected.
 *
 * Revision 1.16  1997/07/03 15:09:38  andreas
 * - removed debugging functions Bdebug() and Edebug() since
 *   they write a static array and are not very useful at all.
 *   Cdebug and Vdebug are merged since they have the same semantics.
 *   The debugging functions in dcmdata changed their interfaces
 *   (see dcmdata/include/dcdebug.h)
 *
 * Revision 1.15  1997/05/29 15:52:50  meichel
 * Added constant for dcmtk release date in dcuid.h.
 * All dcmtk applications now contain a version string
 * which is displayed with the command line options ("usage" message)
 * and which can be queried in the binary with the "ident" command.
 *
 * Revision 1.14  1997/05/27 13:46:53  andreas
 * - Corrected usage string in dcmdump
 *
 * Revision 1.13  1997/05/22 16:53:32  andreas
 * - Changed default options and documentation for dcmdump.
 *
 * Revision 1.12  1997/05/22 13:26:23  hewett
 * Modified the test for presence of a data dictionary to use the
 * method DcmDataDictionary::isDictionaryLoaded().
 *
 * Revision 1.11  1997/05/20 07:57:11  andreas
 * - Removed obsolete applications file2ds and ds2file. The functionality of these
 *   applications is now peformed by dcmconv. Unified calling parameters
 *   are implemented in dump2dcm, dcmdump and dcmconv.
 *
 * Revision 1.10  1997/05/16 08:31:04  andreas
 * - Revised handling of GroupLength elements and support of
 *   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
 *   got additional enumeration values (for a description see dctypes.h).
 *   addGroupLength and removeGroupLength methods are replaced by
 *   computeGroupLengthAndPadding. To support Padding, the parameters of
 *   element and sequence write functions changed.
 *
 * Revision 1.9  1997/04/18 08:04:47  andreas
 * - Minor corrections: correct some warnings of the SUN-C++ Compiler
 *   concerning the assignments of wrong types and inline compiler
 *   errors
 *
 * Revision 1.8  1997/02/06 11:19:22  hewett
 * Update for CodeWarrior 11 on Macintrosh.  Now explicitly sets flags
 * for the SIOUX console.
 *
 * Revision 1.7  1996/09/24 16:13:50  hewett
 * Added preliminary support for the Macintosh environment (GUSI library).
 *
 * Revision 1.6  1996/09/18 16:34:16  hewett
 * Added optional code to the dcmdump program to restrict its
 * output to specified named tags.
 *
 * Revision 1.5  1996/08/05 08:43:36  andreas
 * new print routine with additional parameters:
 *  - print into files
 *  - fix output length for elements
 *
 * Revision 1.4  1996/03/12 15:11:38  hewett
 * Added call to prepareCmdLineArgs to enable command line arguments
 * in environments which do not provide them.
 *
 * Revision 1.3  1996/01/05 13:29:34  andreas
 * - new streaming facilities
 * - unique read/write methods for block and file transfer
 * - more cleanups
 *
 * Revision 1.2  1995/11/23 17:10:31  hewett
 * Updated for loadable data dictionary.
 *
 */
