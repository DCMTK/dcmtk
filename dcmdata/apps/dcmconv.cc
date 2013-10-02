/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Convert dicom file encoding
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"    /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */

#ifdef WITH_ZLIB
#include <zlib.h>                      /* for zlibVersion() */
#endif
#ifdef WITH_LIBICONV
#include "dcmtk/ofstd/ofchrenc.h"      /* for OFCharacterEncoding */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmconv"

static OFLogger dcmconvLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************


#define SHORTCOL 3
#define LONGCOL 21

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
    if (dicent == NULL)
    {
      OFLOG_ERROR(dcmconvLogger, "unrecognised tag name: '" << tagName << "'");
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

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  E_FileReadMode opt_readMode = ERM_autoDetect;
  E_FileWriteMode opt_writeMode = EWM_fileformat;
  E_TransferSyntax opt_ixfer = EXS_Unknown;
  E_TransferSyntax opt_oxfer = EXS_Unknown;
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;
#ifdef WITH_ZLIB
  OFCmdUnsignedInt opt_compressionLevel = 0;
#endif
#ifdef WITH_LIBICONV
  const char *opt_convertToCharset = NULL;
  OFBool opt_transliterate = OFFalse;
  OFBool opt_discardIllegal = OFFalse;
#endif
  OFBool opt_noInvalidGroups = OFFalse;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Convert DICOM file encoding", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                  "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
    OFLog::addOptions(cmd);

  cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",           "+f",     "read file format or data set (default)");
      cmd.addOption("--read-file-only",      "+fo",    "read file format only");
      cmd.addOption("--read-dataset",        "-f",     "read data set without file meta information");
    cmd.addSubGroup("input transfer syntax:", LONGCOL, SHORTCOL);
      cmd.addOption("--read-xfer-auto",      "-t=",    "use TS recognition (default)");
      cmd.addOption("--read-xfer-detect",    "-td",    "ignore TS specified in the file meta header");
      cmd.addOption("--read-xfer-little",    "-te",    "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",       "-tb",    "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit",  "-ti",    "read with implicit VR little endian TS");
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

  cmd.addGroup("processing options:");
#ifdef WITH_LIBICONV
    cmd.addSubGroup("specific character set:");
      cmd.addOption("--convert-to-utf8",     "+U8",    "convert all element values that are affected\nby Specific Character Set (0008,0005) to UTF-8");
      cmd.addOption("--convert-to-latin1",   "+L1",    "convert affected element values to ISO 8859-1");
      cmd.addOption("--convert-to-ascii",    "+A7",    "convert affected element values to 7-bit ASCII");
      cmd.addOption("--convert-to-charset",  "+C",  1, "[c]harset: string",
                                                       "convert affected element values to the character\nset specified by the DICOM defined term c");
      cmd.addOption("--transliterate",       "-Ct",    "try to approximate characters that cannot be\nrepresented through similar looking characters");
      cmd.addOption("--discard-illegal",     "-Cd",    "discard characters that cannot be represented\nin destination character set");
#endif
    cmd.addSubGroup("other processing options:");
      cmd.addOption("--no-invalid-groups",   "-ig",    "remove elements with invalid group number");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",     "write file format (default)");
      cmd.addOption("--write-new-meta-info", "+Fm",    "write file format with new meta information");
      cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-same",     "+t=",    "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
      cmd.addOption("--write-xfer-deflated", "+td",    "write with deflated explicit VR little endian TS");
#endif
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",       "+u",     "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",      "-u",     "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc", "+g=",    "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create", "+g",     "always write with group length elements");
      cmd.addOption("--group-length-remove", "-g",     "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",     "+e",     "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",    "-e",     "write with undefined lengths");
      cmd.addOption("--write-oversized",     "+eo",    "write oversized explicit length sequences\nand items with undefined length (default)");
      cmd.addOption("--abort-oversized",     "-eo",    "abort on oversized explicit sequences/items");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",      "-p=",    "do not change padding\n(default if not --write-dataset)");
      cmd.addOption("--padding-off",         "-p",     "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",      "+p",  2, "[f]ile-pad [i]tem-pad: integer",
                                                       "align file on multiple of f bytes\nand items on multiple of i bytes");
#ifdef WITH_ZLIB
    cmd.addSubGroup("deflate compression level (only with --write-xfer-deflated):");
      cmd.addOption("--compression-level",   "+cl", 1, "[l]evel: integer (default: 6)",
                                                       "0=uncompressed, 1=fastest, 9=best compression");
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

      /* command line parameters */
      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

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
      if (cmd.findOption("--ignore-parse-errors"))
      {
        dcmIgnoreParsingErrors.set(OFTrue);
      }
      if (cmd.findOption("--handle-parse-errors"))
      {
        dcmIgnoreParsingErrors.set(OFFalse);
      }
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
#ifdef WITH_LIBICONV
      cmd.beginOptionBlock();
      if (cmd.findOption("--convert-to-utf8")) opt_convertToCharset = "ISO_IR 192";
      if (cmd.findOption("--convert-to-latin1")) opt_convertToCharset = "ISO_IR 100";
      if (cmd.findOption("--convert-to-ascii")) opt_convertToCharset = "";
      if (cmd.findOption("--convert-to-charset")) app.checkValue(cmd.getValue(opt_convertToCharset));
      cmd.endOptionBlock();
      if (cmd.findOption("--transliterate"))
      {
        app.checkDependence("--transliterate", "one of the --convert-to-xxx options", opt_convertToCharset != NULL);
        opt_transliterate = OFTrue;
      }
      if (cmd.findOption("--discard-illegal"))
      {
        app.checkDependence("--discard-illegal", "one of the --convert-to-xxx options", opt_convertToCharset != NULL);
        opt_discardIllegal = OFTrue;
      }
#endif
      if (cmd.findOption("--no-invalid-groups")) opt_noInvalidGroups = OFTrue;

      /* output options */
      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_writeMode = EWM_fileformat;
      if (cmd.findOption("--write-new-meta-info")) opt_writeMode = EWM_createNewMeta;
      if (cmd.findOption("--write-dataset")) opt_writeMode = EWM_dataset;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-same")) opt_oxfer = EXS_Unknown;
      if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
      if (cmd.findOption("--write-xfer-deflated")) opt_oxfer = EXS_DeflatedLittleEndianExplicit;
#endif
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
        dcmEnableOtherFloatStringVRGeneration.set(OFTrue);
        dcmEnableOtherDoubleStringVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
        dcmEnableOtherFloatStringVRGeneration.set(OFFalse);
        dcmEnableOtherDoubleStringVRGeneration.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-recalc")) opt_oglenc = EGL_recalcGL;
      if (cmd.findOption("--group-length-create")) opt_oglenc = EGL_withGL;
      if (cmd.findOption("--group-length-remove")) opt_oglenc = EGL_withoutGL;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit")) opt_oenctype = EET_ExplicitLength;
      if (cmd.findOption("--length-undefined")) opt_oenctype = EET_UndefinedLength;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-oversized")) dcmWriteOversizedSeqsAndItemsUndefined.set(OFTrue);
      if (cmd.findOption("--abort-oversized")) dcmWriteOversizedSeqsAndItemsUndefined.set(OFFalse);
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--padding-retain"))
      {
        app.checkConflict("--padding-retain", "--write-dataset", opt_writeMode == EWM_dataset);
        opt_opadenc = EPD_noChange;
      }
      if (cmd.findOption("--padding-off")) opt_opadenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
        app.checkConflict("--padding-create", "--write-dataset", opt_writeMode == EWM_dataset);
        app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
        app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
        opt_opadenc = EPD_withPadding;
      }
      cmd.endOptionBlock();

#ifdef WITH_ZLIB
      if (cmd.findOption("--compression-level"))
      {
        app.checkDependence("--compression-level", "--write-xfer-deflated", opt_oxfer == EXS_DeflatedLittleEndianExplicit);
        app.checkValue(cmd.getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
        dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
      }
#endif
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmconvLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmconvLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* open input file */
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcmconvLogger, "invalid filename: <empty string>");
        return 1;
    }

    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();

    OFLOG_INFO(dcmconvLogger, "open input file " << opt_ifname);

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcmconvLogger, error.text() << ": reading file: " <<  opt_ifname);
        return 1;
    }

    OFLOG_INFO(dcmconvLogger, "load all data into memory");
    /* make sure that pixel data is loaded before output file is created */
    dataset->loadAllDataIntoMemory();

    if (opt_noInvalidGroups)
    {
        OFLOG_INFO(dcmconvLogger, "remove all elements with an invalid group number");
        fileformat.removeInvalidGroups();
    }
#ifdef WITH_LIBICONV
    if (opt_convertToCharset != NULL)
    {
        OFString toCharset(opt_convertToCharset);
        /* convert the complete dataset to a new character encoding */
        OFLOG_INFO(dcmconvLogger, "converting all element values that are affected by "
            << "Specific Character Set (0008,0005) to '" << opt_convertToCharset << "'"
            << (toCharset.empty() ? " (ASCII)" : ""));
        error = fileformat.convertCharacterSet(toCharset, opt_transliterate, opt_discardIllegal);
        if (error.bad())
        {
            OFLOG_FATAL(dcmconvLogger, error.text() << ": processing file: " << opt_ifname);
            return 1;
        }
    }
#endif

    if (opt_oxfer == EXS_Unknown)
    {
        OFLOG_INFO(dcmconvLogger, "set output transfer syntax to input transfer syntax");
        opt_oxfer = dataset->getOriginalXfer();
    }

    OFLOG_INFO(dcmconvLogger, "check if new output transfer syntax is possible");

    DcmXfer opt_oxferSyn(opt_oxfer);

    dataset->chooseRepresentation(opt_oxfer, NULL);

    if (dataset->canWriteXfer(opt_oxfer))
    {
        OFLOG_INFO(dcmconvLogger, "output transfer syntax " << opt_oxferSyn.getXferName() << " can be written");
    } else {
        OFLOG_FATAL(dcmconvLogger, "no conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
        return 1;
    }

    OFLOG_INFO(dcmconvLogger, "create output file " << opt_ofname);

    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcmconvLogger, error.text() << ": writing file: " <<  opt_ofname);
        return 1;
    }

    OFLOG_INFO(dcmconvLogger, "conversion successful");

    return 0;
}
