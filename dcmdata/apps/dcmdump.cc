/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
#include "dcmtk/ofstd/ofchrenc.h"     /* for OFCharacterEncoding */
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

static int dumpFile(STD_NAMESPACE ostream &out,
                    const OFFilename &ifname,
                    const E_FileReadMode readMode,
                    const E_TransferSyntax xfer,
                    const size_t printFlags,
                    const OFBool loadIntoMemory,
                    const OFBool stopOnErrors,
                    const OFBool convertToUTF8,
                    const DcmTagKey &stopParsingAtElement,
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
        return DcmTagKey(OFstatic_cast(Uint16, group),OFstatic_cast(Uint16, elem));
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


/* main program */

#define SHORTCOL 3
#define LONGCOL 21

// this macro either expands to main() or wmain()
DCMTK_MAIN_FUNCTION
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
    OFBool convertToUTF8 = OFFalse;
    DcmTagKey stopParsingBeforeElement = DCM_UndefinedTagKey;

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
      cmd.addSubGroup("long tag values:");
        cmd.addOption("--load-all",            "+M",     "load very long tag values (default)");
        cmd.addOption("--load-short",          "-M",     "do not load very long values (e.g. pixel data)");
        cmd.addOption("--max-read-length",     "+R",  1, "[k]bytes: integer (4..4194302, default: 4)",
                                                         "set threshold for long values to k kbytes");
      cmd.addSubGroup("parsing of file meta information:");
        cmd.addOption("--use-meta-length",     "+ml",    "use file meta information group length (default)");
        cmd.addOption("--ignore-meta-length",  "-ml",    "ignore file meta information group length");
      cmd.addSubGroup("parsing of odd-length attributes:");
        cmd.addOption("--accept-odd-length",   "+ao",    "accept odd length attributes (default)");
        cmd.addOption("--assume-even-length",  "+ae",    "assume real length is one byte larger");
      cmd.addSubGroup("handling of explicit VR:");
        cmd.addOption("--use-explicit-vr",     "+ev",    "use explicit VR from dataset (default)");
        cmd.addOption("--ignore-explicit-vr",  "-ev",    "ignore explicit VR (prefer data dictionary)");
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
      cmd.addSubGroup("handling of wrong delimitation items:");
        cmd.addOption("--use-delim-items",     "-rd",    "use delimitation items from dataset (default)");
        cmd.addOption("--replace-wrong-delim", "+rd",    "replace wrong sequence/item delimitation items");
      cmd.addSubGroup("handling of illegal undefined length OB/OW elements:");
        cmd.addOption("--illegal-obow-rej",    "-oi",    "reject dataset with illegal element (default)");
        cmd.addOption("--illegal-obow-conv",   "+oi",    "convert undefined length OB/OW element to SQ");
      cmd.addSubGroup("handling of VOI LUT Sequence with OW VR and explicit length:");
        cmd.addOption("--illegal-voi-rej",     "-vi",    "reject dataset with illegal VOI LUT (default)");
        cmd.addOption("--illegal-voi-conv",    "+vi",    "convert illegal VOI LUT to SQ");
      cmd.addSubGroup("handling of explicit length pixel data for encaps. transfer syntaxes:");
        cmd.addOption("--abort-expl-pixdata",  "-pe",    "abort on explicit length pixel data (default)");
        cmd.addOption("--use-expl-pixdata",    "+pe",    "use explicit length pixel data");
      cmd.addSubGroup("general handling of parser errors: ");
        cmd.addOption("--ignore-parse-errors", "+Ep",    "try to recover from parse errors");
        cmd.addOption("--handle-parse-errors", "-Ep",    "handle parse errors and stop parsing (default)");
      cmd.addSubGroup("other parsing options:");
        cmd.addOption("--stop-after-elem",     "+st", 1, "[t]ag: \"gggg,eeee\" or dictionary name",
                                                         "stop parsing after element specified by t");
        cmd.addOption("--stop-before-elem",    "+sb", 1, "[t]ag: \"gggg,eeee\" or dictionary name",
                                                         "stop parsing before element specified by t");
      cmd.addSubGroup("automatic data correction:");
        cmd.addOption("--enable-correction",   "+dc",    "enable automatic data correction (default)");
        cmd.addOption("--disable-correction",  "-dc",    "disable automatic data correction");
#ifdef WITH_ZLIB
      cmd.addSubGroup("bitstream format of deflated input:");
        cmd.addOption("--bitstream-deflated",  "+bd",    "expect deflated bitstream (default)");
        cmd.addOption("--bitstream-zlib",      "+bz",    "expect deflated zlib bitstream");
#endif

#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    cmd.addGroup("processing options:");
      cmd.addSubGroup("specific character set:");
        cmd.addOption("--convert-to-utf8",     "+U8",    "convert all element values that are affected\nby Specific Character Set (0008,0005) to UTF-8");
#endif

    cmd.addGroup("output options:");
      cmd.addSubGroup("printing:");
        cmd.addOption("--print-all",           "+L",     "print long tag values completely");
        cmd.addOption("--print-short",         "-L",     "print long tag values shortened (default)");
        cmd.addOption("--print-tree",          "+T",     "print hierarchical structure as a simple tree");
        cmd.addOption("--print-indented",      "-T",     "print hierarchical structure indented (default)");
        cmd.addOption("--print-filename",      "+F",     "print header with filename for each input file");
        cmd.addOption("--print-file-search",   "+Fs",    "print header with filename only for those input\nfiles that contain one of the searched tags");
      cmd.addSubGroup("mapping:");
        cmd.addOption("--map-uid-names",       "+Un",    "map well-known UID numbers to names (default)");
        cmd.addOption("--no-uid-names",        "-Un",    "do not map well-known UID numbers to names");
      cmd.addSubGroup("quoting:");
        cmd.addOption("--quote-nonascii",      "+Qn",    "quote non-ASCII and control chars as XML markup");
        cmd.addOption("--quote-as-octal",      "+Qo",    "quote non-ASCII and control ch. as octal numbers");
        cmd.addOption("--print-nonascii",      "-Qn",    "print non-ASCII and control chars (default)");
#ifdef ANSI_ESCAPE_CODES_AVAILABLE
      cmd.addSubGroup("color:");
        cmd.addOption("--print-color",         "+C",     "use ANSI escape codes for colored output");
        cmd.addOption("--no-color",            "-C",     "do not use any ANSI escape codes (default)");
#endif

      cmd.addSubGroup("error handling:");
        cmd.addOption("--stop-on-error",       "-E",     "do not print if file is damaged (default)");
        cmd.addOption("--ignore-errors",       "+E",     "attempt to print even if file is damaged");

      cmd.addSubGroup("searching:");
        cmd.addOption("--search",              "+P",  1, "[t]ag: \"gggg,eeee\" or dictionary name",
                                                         "print the textual dump of tag t\nthis option can be specified multiple times\n(default: the complete file is printed)");
        cmd.addOption("--search-all",          "+s",     "print all instances of searched tags (default)");
        cmd.addOption("--search-first",        "-s",     "only print first instance of searched tags");
        cmd.addOption("--prepend",             "+p",     "prepend sequence hierarchy to printed tag,\ndenoted by: (gggg,eeee).(gggg,eeee).*\n(only when used with --search)");
        cmd.addOption("--no-prepend",          "-p",     "do not prepend hierarchy to tag (default)");

      cmd.addSubGroup("writing:");
        cmd.addOption("--write-pixel",         "+W",  1, "[d]irectory: string",
                                                         "write pixel data to a .raw file stored in d\n(little endian, filename created automatically)");

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
      if (cmd.findOption("--use-explicit-vr"))
      {
        dcmPreferVRFromDataDictionary.set(OFFalse);
      }
      if (cmd.findOption("--ignore-explicit-vr"))
      {
        dcmPreferVRFromDataDictionary.set(OFTrue);
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
      if (cmd.findOption("--use-delim-items"))
      {
        dcmReplaceWrongDelimitationItem.set(OFFalse);
      }
      if (cmd.findOption("--replace-wrong-delim"))
      {
        dcmReplaceWrongDelimitationItem.set(OFTrue);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--illegal-obow-rej"))
      {
        dcmConvertUndefinedLengthOBOWtoSQ.set(OFFalse);
      }
      if (cmd.findOption("--illegal-obow-conv"))
      {
        dcmConvertUndefinedLengthOBOWtoSQ.set(OFTrue);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--illegal-voi-rej"))
      {
        dcmConvertVOILUTSequenceOWtoSQ.set(OFFalse);
      }
      if (cmd.findOption("--illegal-voi-conv"))
      {
        dcmConvertVOILUTSequenceOWtoSQ.set(OFTrue);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--abort-expl-pixdata"))
      {
        dcmUseExplLengthPixDataForEncTS.set(OFFalse);
      }
      if (cmd.findOption("--use-expl-pixdata"))
      {
        dcmUseExplLengthPixDataForEncTS.set(OFTrue);
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

      /* processing options */
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
      if (cmd.findOption("--convert-to-utf8")) convertToUTF8 = OFTrue;
#endif

      /* output options */
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
      if (cmd.findOption("--quote-nonascii"))
        printFlags = (printFlags & ~DCMTypes::PF_convertToOctalNumbers) | DCMTypes::PF_convertToMarkup;
      if (cmd.findOption("--quote-as-octal"))
        printFlags = (printFlags & ~DCMTypes::PF_convertToMarkup) | DCMTypes::PF_convertToOctalNumbers;
      if (cmd.findOption("--print-nonascii"))
        printFlags = printFlags & ~(DCMTypes::PF_convertToMarkup | DCMTypes::PF_convertToOctalNumbers);
      cmd.endOptionBlock();

#ifdef ANSI_ESCAPE_CODES_AVAILABLE
      cmd.beginOptionBlock();
      if (cmd.findOption("--print-color")) printFlags |= DCMTypes::PF_useANSIEscapeCodes;
      if (cmd.findOption("--no-color")) printFlags &= ~DCMTypes::PF_useANSIEscapeCodes;
      cmd.endOptionBlock();
#endif

      cmd.beginOptionBlock();
      if (cmd.findOption("--stop-on-error")) stopOnErrors = OFTrue;
      if (cmd.findOption("--ignore-errors")) stopOnErrors = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
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
      if (cmd.findOption("--stop-before-elem"))
      {
        const char *tagName = NULL;
        app.checkValue(cmd.getValue(tagName));
        DcmTagKey key = parseTagKey(tagName);
        if (key != DCM_UndefinedTagKey)
          stopParsingBeforeElement = key;
        else
          app.printError("no valid key given for option --stop-before-elem");
      }
      cmd.endOptionBlock();

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
    OFFilename paramValue;
    OFList<OFFilename> inputFiles;
    const int paramCount = cmd.getParamCount();
    /* iterate over all input filenames */
    for (int i = 1; i <= paramCount; i++)
    {
      cmd.getParam(i, paramValue);
      /* search directory recursively (if required) */
      if (OFStandard::dirExists(paramValue))
      {
        if (scanDir)
          OFStandard::searchDirectoryRecursively(paramValue, inputFiles, scanPattern, "" /*dirPrefix*/, recurse);
        else
          OFLOG_WARN(dcmdumpLogger, "ignoring directory because option --scan-directories is not set: " << paramValue);
      } else
        inputFiles.push_back(paramValue);
    }
    /* check whether there are any input files at all */
    if (inputFiles.empty())
    {
      OFLOG_FATAL(dcmdumpLogger, "no input files to be dumped");
      return 1;
    }

    const size_t count = inputFiles.size();
    OFFilename current;
    OFListIterator(OFFilename) if_iter = inputFiles.begin();
    OFListIterator(OFFilename) if_last = inputFiles.end();
    /* iterate over all input filenames */
    while (if_iter != if_last)
    {
      current = (*if_iter++);
      if (printFilename)
      {
        /* a newline separates two consecutive "dumps" */
        if (++fileCounter > 1)
          COUT << OFendl;
        /* print header with filename */
        COUT << "# " << OFFIS_CONSOLE_APPLICATION << " (" << fileCounter << "/" << count << "): " << current << OFendl;
      }
      errorCount += dumpFile(COUT, current, readMode, xfer, printFlags, loadIntoMemory, stopOnErrors, convertToUTF8, stopParsingBeforeElement, pixelDirectory);
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
            out << DCMDATA_ANSI_ESCAPE_CODE_TAG;
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
                    const OFFilename &ifname,
                    const E_FileReadMode readMode,
                    const E_TransferSyntax xfer,
                    const size_t printFlags,
                    const OFBool loadIntoMemory,
                    const OFBool stopOnErrors,
                    const OFBool convertToUTF8,
                    const DcmTagKey &stopParsingAtElement,
                    const char *pixelDirectory)
{
    int result = 0;

    if (ifname.isEmpty())
    {
        OFLOG_ERROR(dcmdumpLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
        return 1;
    }

    DcmFileFormat dfile;
    DcmObject *dset = &dfile;
    if (readMode == ERM_dataset) dset = dfile.getDataset();
    OFCondition cond;

    if (stopParsingAtElement == DCM_UndefinedTagKey)
    {
        cond = dfile.loadFile(ifname, xfer, EGL_noChange, OFstatic_cast(Uint32, maxReadLength), readMode);
    }
    else
    {
        // instead of using loadFile(), we call loadFileUntilTag().
        cond = dfile.loadFileUntilTag(ifname, xfer, EGL_noChange, OFstatic_cast(Uint32, maxReadLength), readMode, stopParsingAtElement);
    }
    if (cond.bad())
    {
        OFLOG_ERROR(dcmdumpLogger, OFFIS_CONSOLE_APPLICATION << ": " << cond.text()
            << ": reading file: "<< ifname);
        result = 1;
        if (stopOnErrors) return result;
    }

    if (loadIntoMemory) dfile.loadAllDataIntoMemory();

#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    if (convertToUTF8)
    {
        OFLOG_INFO(dcmdumpLogger, "converting all element values that are affected by Specific Character Set (0008,0005) to UTF-8");
        cond = dfile.convertToUTF8();
        if (cond.bad())
        {
            OFLOG_FATAL(dcmdumpLogger, cond.text() << ": converting file to UTF-8: " << ifname);
            result = 1;
            if (stopOnErrors) return result;
        }
    }
#else
    // avoid compiler warning on unused variable
    (void)convertToUTF8;
#endif

    size_t pixelCounter = 0;
    const char *pixelFileName = NULL;
    OFFilename pixelFilenameStr;
    if (pixelDirectory != NULL)
    {
        /* create filename for pixel data */
        OFFilename fileName;
        OFStandard::getFilenameFromPath(fileName, ifname);
        OFStandard::combineDirAndFilename(pixelFilenameStr, pixelDirectory, fileName);
        pixelFileName = pixelFilenameStr.getCharPointer();
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
                searchKey.set(OFstatic_cast(Uint16, group), OFstatic_cast(Uint16, elem));
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
