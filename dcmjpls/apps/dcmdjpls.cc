/*
 *
 *  Copyright (C) 2007-2020, OFFIS e.V.
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
 *  Module:  dcmdjpls
 *
 *  Author:  Martin Willkomm
 *
 *  Purpose: Decompress DICOM file with JPEG-LS transfer syntax
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmimage/diregist.h"  /* include to support color images */
#include "dcmtk/dcmjpls/djlsutil.h"   /* for dcmjpgls typedefs */
#include "dcmtk/dcmjpls/djdecode.h"   /* for JPEG-LS decoder */

#ifdef WITH_ZLIB
#include <zlib.h>      /* for zlibVersion() */
#endif

#ifdef USE_LICENSE_FILE
#include "oflice.h"
#endif

#ifndef OFFIS_CONSOLE_APPLICATION
#define OFFIS_CONSOLE_APPLICATION "dcmdjpls"
#endif

static OFLogger dcmdjplsLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************


#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{
  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;
  E_FileReadMode opt_readMode = ERM_autoDetect;
  E_FileWriteMode opt_writeMode = EWM_createNewMeta;
  E_TransferSyntax opt_ixfer = EXS_Unknown;
  OFBool opt_forceSingleFragmentPerFrame = OFFalse;

  // parameter
  JLS_UIDCreation opt_uidcreation = EJLSUC_default;
  JLS_PlanarConfiguration opt_planarconfig = EJLSPC_restore;
  OFBool opt_ignoreOffsetTable = OFFalse;

#ifdef USE_LICENSE_FILE
LICENSE_FILE_DECLARATIONS
#endif

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Decode JPEG-LS compressed DICOM file", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                      "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                             "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

#ifdef USE_LICENSE_FILE
LICENSE_FILE_DECLARE_COMMAND_LINE_OPTIONS
#endif

   cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
     cmd.addOption("--read-file",               "+f",     "read file format or data set (default)");
     cmd.addOption("--read-file-only",          "+fo",    "read file format only");
     cmd.addOption("--read-dataset",            "-f",     "read data set without file meta information");

  cmd.addGroup("processing options:");
    cmd.addSubGroup("planar configuration:");
      cmd.addOption("--planar-restore",         "+pr",    "restore original planar configuration (default)");
      cmd.addOption("--planar-auto",            "+pa",    "automatically determine planar configuration\nfrom SOP class and color space");
      cmd.addOption("--color-by-pixel",         "+px",    "always store color-by-pixel");
      cmd.addOption("--color-by-plane",         "+pl",    "always store color-by-plane");
    cmd.addSubGroup("SOP Instance UID:");
      cmd.addOption("--uid-default",            "+ud",    "keep same SOP Instance UID (default)");
      cmd.addOption("--uid-always",             "+ua",    "always assign new UID");
    cmd.addSubGroup("workaround options for incorrect JPEG-LS encodings:");
      cmd.addOption("--workaround-incpl",       "+wi",    "enable workaround for incomplete JPEG-LS data");
    cmd.addSubGroup("other processing options:");
      cmd.addOption("--ignore-offsettable",     "+io",    "ignore offset table when decompressing");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",             "+F",     "write file format (default)");
      cmd.addOption("--write-dataset",          "-F",     "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-little",      "+te",    "write with explicit VR little endian (default)");
      cmd.addOption("--write-xfer-big",         "+tb",    "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",    "+ti",    "write with implicit VR little endian TS");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",          "+u",     "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",     "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc",    "+g=",    "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create",    "+g",     "always write with group length elements");
      cmd.addOption("--group-length-remove",    "-g",     "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",        "+e",     "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",       "-e",     "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",         "-p=",    "do not change padding (default)");
      cmd.addOption("--padding-off",            "-p",     "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",         "+p",  2, "[f]ile-pad [i]tem-pad: integer",
                                                          "align file on multiple of f bytes\nand items on multiple of i bytes");

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
          COUT << OFendl << "External libraries used:" << OFendl;
#ifdef WITH_ZLIB
          COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
          COUT << "- " << DJLSDecoderRegistration::getLibraryVersionString() << OFendl;
          return 0;
        }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      /* options */

      OFLog::configureFromCommandLine(cmd, app);

#ifdef USE_LICENSE_FILE
LICENSE_FILE_EVALUATE_COMMAND_LINE_OPTIONS
#endif

      cmd.beginOptionBlock();
      if (cmd.findOption("--planar-restore")) opt_planarconfig = EJLSPC_restore;
      if (cmd.findOption("--planar-auto")) opt_planarconfig = EJLSPC_auto;
      if (cmd.findOption("--color-by-pixel")) opt_planarconfig = EJLSPC_colorByPixel;
      if (cmd.findOption("--color-by-plane")) opt_planarconfig = EJLSPC_colorByPlane;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-default")) opt_uidcreation = EJLSUC_default;
      if (cmd.findOption("--uid-always")) opt_uidcreation = EJLSUC_always;
      cmd.endOptionBlock();

      if (cmd.findOption("--workaround-incpl")) opt_forceSingleFragmentPerFrame = OFTrue;
      if (cmd.findOption("--ignore-offsettable")) opt_ignoreOffsetTable = OFTrue;

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file"))
      {
        opt_readMode = ERM_autoDetect;
        opt_ixfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-file-only"))
      {
        opt_readMode = ERM_fileOnly;
        opt_ixfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-dataset"))
      {
        opt_readMode = ERM_dataset;

        // This transfer syntax works as long as the content of encapsulated pixel
        // sequences is some kind of JPEG-LS bitstream. hmm?
        opt_ixfer = EXS_JPEGLSLossless;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_writeMode = EWM_createNewMeta;
      if (cmd.findOption("--write-dataset")) opt_writeMode = EWM_dataset;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr")) dcmEnableGenerationOfNewVRs();
      if (cmd.findOption("--disable-new-vr")) dcmDisableGenerationOfNewVRs();
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
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmdjplsLogger, rcsid << OFendl);

    // register global decompression codecs
    DJLSDecoderRegistration::registerCodecs(
        opt_uidcreation,
        opt_planarconfig,
        opt_ignoreOffsetTable,
        opt_forceSingleFragmentPerFrame);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      OFLOG_WARN(dcmdjplsLogger, "no data dictionary loaded, "
           << "check environment variable: "
           << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open input file
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
      OFLOG_FATAL(dcmdjplsLogger, "invalid filename: <empty string>");
      return 1;
    }

    DcmFileFormat fileformat;

    OFLOG_INFO(dcmdjplsLogger, "reading input file " << opt_ifname);

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
    if (error.bad())
    {
      OFLOG_FATAL(dcmdjplsLogger, error.text() << ": reading file: " <<  opt_ifname);
      return 1;
    }

    DcmDataset *dataset = fileformat.getDataset();

    OFLOG_INFO(dcmdjplsLogger, "decompressing file");

    DcmXfer opt_oxferSyn(opt_oxfer);
    DcmXfer original_xfer(dataset->getOriginalXfer());

    error = dataset->chooseRepresentation(opt_oxfer, NULL);
    if (error.bad())
    {
      OFLOG_FATAL(dcmdjplsLogger, error.text() << ": decompressing file: " <<  opt_ifname);
      if (error == EC_CannotChangeRepresentation)
        OFLOG_FATAL(dcmdjplsLogger, "Input transfer syntax " << original_xfer.getXferName() << " not supported");
      return 1;
    }

    if (! dataset->canWriteXfer(opt_oxfer))
    {
      OFLOG_FATAL(dcmdjplsLogger, "no conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible");
      return 1;
    }

    OFLOG_INFO(dcmdjplsLogger, "creating output file " << opt_ofname);

    fileformat.loadAllDataIntoMemory();
    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
      OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);

    if (error != EC_Normal)
    {
      OFLOG_FATAL(dcmdjplsLogger, error.text() << ": writing file: " <<  opt_ofname);
      return 1;
    }

    OFLOG_INFO(dcmdjplsLogger, "conversion successful");

    // deregister global decompression codecs
    DJLSDecoderRegistration::cleanup();

    return 0;
}
