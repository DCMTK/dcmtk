/*
 *
 *  Copyright (C) 2007-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:40 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;
  E_FileReadMode opt_readMode = ERM_autoDetect;
  E_FileWriteMode opt_writeMode = EWM_fileformat;
  E_TransferSyntax opt_ixfer = EXS_Unknown;

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
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
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
          COUT << "- " << "CharLS, Revision 55020 (modified)" << OFendl;
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
      if (cmd.findOption("--write-file")) opt_writeMode = EWM_fileformat;
      if (cmd.findOption("--write-dataset")) opt_writeMode = EWM_dataset;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
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
    DJLSDecoderRegistration::registerCodecs(opt_uidcreation, opt_planarconfig, opt_ignoreOffsetTable);

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

    // update file meta information with new SOP Instance UID
    if (opt_uidcreation && (opt_writeMode == EWM_fileformat))
        opt_writeMode = EWM_updateMeta;

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


/*
 * CVS/RCS Log:
 * $Log: dcmdjpls.cc,v $
 * Revision 1.8  2010-10-14 13:13:40  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2010-10-05 08:25:40  uli
 * Update dcmjpls to newest CharLS snapshot.
 *
 * Revision 1.6  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.5  2009-09-04 13:37:00  meichel
 * Updated libcharls in module dcmjpls to CharLS revision 27770.
 *
 * Revision 1.4  2009-08-21 10:04:45  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 * Use helper function checkConflict() where appropriate.
 *
 * Revision 1.3  2009-08-20 14:45:06  meichel
 * Updated libcharls in module dcmjpls to CharLS revision 26807.
 *
 * Revision 1.2  2009-08-05 10:24:54  joergr
 * Made syntax usage more consistent with other DCMTK compression tools.
 * Fixed wrong reference to JPEG-LS implementation and added revision number.
 *
 * Revision 1.1  2009-07-29 14:46:46  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.10  2009-03-19 12:15:25  joergr
 * Fixed wrong CVS log message.
 *
 * Revision 1.9  2009-03-19 12:14:22  joergr
 * Added more explicit message in case input transfer syntax is not supported.
 *
 * Revision 1.8  2008-09-25 15:38:27  joergr
 * Fixed outdated comment.
 *
 * Revision 1.7  2008-09-25 14:23:11  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.6  2008-09-25 13:47:29  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.5  2007/06/15 14:35:45  meichel
 * Renamed CMake project and include directory from dcmjpgls to dcmjpls
 *
 * Revision 1.4  2007/06/15 10:39:15  meichel
 * Completed implementation of decoder, which now correctly processes all
 *   of the NEMA JPEG-LS sample images, including fragmented frames.
 *
 * Revision 1.3  2007/06/13 16:41:07  meichel
 * Code clean-up and removal of dead code
 *
 * Revision 1.2  2007/06/13 16:22:54  joergr
 * Fixed a couple of inconsistencies.
 *
 *
 */
