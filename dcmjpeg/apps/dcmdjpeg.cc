/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Decompress DICOM file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:38 $
 *  CVS/RCS Revision: $Revision: 1.26 $
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
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmjpeg/djdecode.h"    /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"    /* for dcmimage JPEG plugin */

#ifdef WITH_ZLIB
#include <zlib.h>      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmdjpeg"

static OFLogger dcmdjpegLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************


#define SHORTCOL 4
#define LONGCOL 21

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
  E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;
  E_TransferSyntax opt_ixfer = EXS_Unknown;

  // JPEG parameters
  E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
  E_UIDCreation opt_uidcreation = EUC_default;
  E_PlanarConfiguration opt_planarconfig = EPC_default;
  OFBool opt_predictor6WorkaroundEnable = OFFalse;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Decode JPEG-compressed DICOM file", rcsid);
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
     cmd.addOption("--read-file",            "+f",     "read file format or data set (default)");
     cmd.addOption("--read-file-only",       "+fo",    "read file format only");
     cmd.addOption("--read-dataset",         "-f",     "read data set without file meta information");

  cmd.addGroup("processing options:");
    cmd.addSubGroup("color space conversion:");
      cmd.addOption("--conv-photometric",    "+cp",    "convert if YCbCr photometric interpr. (default)");
      cmd.addOption("--conv-lossy",          "+cl",    "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-guess",          "+cg",    "convert to RGB if YCbCr is guessed by library");
      cmd.addOption("--conv-guess-lossy",    "+cgl",   "convert to RGB if lossy JPEG and YCbCr is\nguessed by the underlying JPEG library");
      cmd.addOption("--conv-always",         "+ca",    "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",          "+cn",    "never convert color space");

    cmd.addSubGroup("planar configuration:");
      cmd.addOption("--planar-auto",         "+pa",    "automatically determine planar configuration\nfrom SOP class and color space (default)");
      cmd.addOption("--color-by-pixel",      "+px",    "always store color-by-pixel");
      cmd.addOption("--color-by-plane",      "+pl",    "always store color-by-plane");

    cmd.addSubGroup("SOP Instance UID:");
      cmd.addOption("--uid-default",         "+ud",    "keep same SOP Instance UID (default)");
      cmd.addOption("--uid-always",          "+ua",    "always assign new UID");

    cmd.addSubGroup("workaround options for incorrect JPEG encodings:");
      cmd.addOption("--workaround-pred6",    "+w6",    "enable workaround for JPEG lossless images\nwith overflow in predictor 6");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",     "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian (default)");
      cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
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
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",      "-p=",    "do not change padding\n(default if not --write-dataset)");
      cmd.addOption("--padding-off",         "-p",     "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",      "+p",  2, "[f]ile-pad [i]tem-pad: integer",
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
              COUT << "- " << DiJPEGPlugin::getLibraryVersionString() << OFendl;
              return 0;
          }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      /* options */

      OFLog::configureFromCommandLine(cmd, app);

      cmd.beginOptionBlock();
      if (cmd.findOption("--planar-auto")) opt_planarconfig = EPC_default;
      if (cmd.findOption("--color-by-pixel")) opt_planarconfig = EPC_colorByPixel;
      if (cmd.findOption("--color-by-plane")) opt_planarconfig = EPC_colorByPlane;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--conv-photometric")) opt_decompCSconversion = EDC_photometricInterpretation;
      if (cmd.findOption("--conv-lossy")) opt_decompCSconversion = EDC_lossyOnly;
      if (cmd.findOption("--conv-guess")) opt_decompCSconversion = EDC_guess;
      if (cmd.findOption("--conv-guess-lossy")) opt_decompCSconversion = EDC_guessLossyOnly;
      if (cmd.findOption("--conv-always")) opt_decompCSconversion = EDC_always;
      if (cmd.findOption("--conv-never")) opt_decompCSconversion = EDC_never;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-default")) opt_uidcreation = EUC_default;
      if (cmd.findOption("--uid-always")) opt_uidcreation = EUC_always;
      cmd.endOptionBlock();

      if (cmd.findOption("--workaround-pred6")) opt_predictor6WorkaroundEnable = OFTrue;

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

        // we don't know the real transfer syntax of the dataset, but this does
        // not matter. As long as the content of encapsulated pixel sequences is
        // some kind of JPEG bitstream supported by the underlying library, the
        // decompression will work. So we simply choose one of the lossless
        // transfer syntaxes, because these support all bit depths up to 16.
        opt_ixfer = EXS_JPEGProcess14TransferSyntax;
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
    OFLOG_DEBUG(dcmdjpegLogger, rcsid << OFendl);

    // register global decompression codecs
    DJDecoderRegistration::registerCodecs(
      opt_decompCSconversion,
      opt_uidcreation,
      opt_planarconfig,
      opt_predictor6WorkaroundEnable);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmdjpegLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcmdjpegLogger, "invalid filename: <empty string>");
        return 1;
    }

    OFCondition error = EC_Normal;

    DcmFileFormat fileformat;

    OFLOG_INFO(dcmdjpegLogger, "reading input file " << opt_ifname);

    error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
    if (error.bad())
    {
        OFLOG_FATAL(dcmdjpegLogger, error.text() << ": reading file: " <<  opt_ifname);
        return 1;
    }

    DcmDataset *dataset = fileformat.getDataset();

    OFLOG_INFO(dcmdjpegLogger, "decompressing file");

    DcmXfer opt_oxferSyn(opt_oxfer);
    DcmXfer original_xfer(dataset->getOriginalXfer());

    error = dataset->chooseRepresentation(opt_oxfer, NULL);
    if (error.bad())
    {
        OFLOG_FATAL(dcmdjpegLogger, error.text() << ": decompressing file: " <<  opt_ifname);
        if (error == EJ_UnsupportedColorConversion)
            OFLOG_FATAL(dcmdjpegLogger, "Try --conv-never to disable color space conversion");
        else if (error == EC_CannotChangeRepresentation)
            OFLOG_FATAL(dcmdjpegLogger, "Input transfer syntax " << original_xfer.getXferName() << " not supported");
        return 1;
    }

    if (! dataset->canWriteXfer(opt_oxfer))
    {
        OFLOG_FATAL(dcmdjpegLogger, "no conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible");
        return 1;
    }

    OFLOG_INFO(dcmdjpegLogger, "creating output file " << opt_ofname);

    // update file meta information with new SOP Instance UID
    if ((opt_uidcreation == EUC_always) && (opt_writeMode == EWM_fileformat))
        opt_writeMode = EWM_updateMeta;

    fileformat.loadAllDataIntoMemory();
    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc,
        opt_opadenc, (Uint32) opt_filepad, (Uint32) opt_itempad, opt_writeMode);
    if (error != EC_Normal)
    {
        OFLOG_FATAL(dcmdjpegLogger, error.text() << ": writing file: " <<  opt_ofname);
        return 1;
    }

    OFLOG_INFO(dcmdjpegLogger, "conversion successful");

    // deregister global decompression codecs
    DJDecoderRegistration::cleanup();

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmdjpeg.cc,v $
 * Revision 1.26  2010-10-14 13:13:38  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.25  2010-03-24 15:05:32  joergr
 * Added new options for the color space conversion during decompression based
 * on the color model that is "guessed" by the underlying JPEG library (IJG).
 *
 * Revision 1.24  2009-10-07 12:44:33  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.23  2009-08-21 09:37:16  joergr
 * Only update file meta information if required (use new file write mode).
 *
 * Revision 1.22  2009-08-21 09:33:03  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 * Use helper function checkDependence() and checkConflict() where appropriate.
 *
 * Revision 1.21  2009-08-05 10:30:00  joergr
 * Fixed various issues with syntax usage (e.g. layout and formatting).
 *
 * Revision 1.20  2009-04-21 14:07:14  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.19  2009-03-19 12:11:24  joergr
 * Added more explicit message in case input transfer syntax is not supported.
 *
 * Revision 1.18  2008-09-25 14:49:45  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.17  2008-09-25 13:58:28  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.16  2006/08/16 16:30:20  meichel
 * Updated all code in module dcmjpeg to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.15  2006/07/27 14:05:02  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.14  2006/03/29 15:58:52  meichel
 * Added support for decompressing images with 16 bits/pixel compressed with
 *   a faulty lossless JPEG encoder that produces integer overflows in predictor 6.
 *
 * Revision 1.13  2005/12/08 15:43:21  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.12  2005/12/02 09:41:40  joergr
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 *
 * Revision 1.11  2005/11/30 14:10:43  onken
 * Added hint concerning --convert-never option when color conversion fails
 *
 * Revision 1.10  2005/11/07 17:10:21  meichel
 * All tools that both read and write a DICOM file now call loadAllDataIntoMemory()
 *   to make sure they do not destroy a file when output = input.
 *
 * Revision 1.9  2005/05/26 14:35:07  meichel
 * Added option --read-dataset to dcmdjpeg that allows to decompress JPEG
 *   compressed DICOM objects that have been stored as dataset without meta-header.
 *   Such a thing should not exist since the transfer syntax cannot be reliably
 *   determined without meta-header, but unfortunately it does.
 *
 * Revision 1.8  2004/01/16 14:28:01  joergr
 * Updated copyright header.
 *
 * Revision 1.7  2002/11/27 15:39:56  meichel
 * Adapted module dcmjpeg to use of new header file ofstdinc.h
 *
 * Revision 1.6  2002/11/26 08:44:41  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.5  2002/09/23 18:14:07  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.4  2002/08/20 12:20:58  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.3  2002/07/10 12:26:02  meichel
 * Fixed memory leak in command line applications
 *
 * Revision 1.2  2001/11/19 15:13:22  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:56:10  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
