/*
 *
 *  Copyright (C) 2001-2013, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Convert DICOM color images palette color
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcmtk/dcmdata/dcuid.h"         /* for dcmtk version name */
#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */

#include "dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */

#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */
#include "dcmtk/dcmimage/diquant.h"      /* for DcmQuant */
#include "dcmtk/dcmdata/dccodec.h"       /* for DcmCodec */

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
#include "dcmtk/dcmjpeg/djdecode.h"      /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"      /* for dcmimage JPEG plugin */
#endif

#ifdef WITH_ZLIB
#include <zlib.h>          /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM color images to palette color"

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
#define OFFIS_CONSOLE_APPLICATION "dcmjquan"
#else
#define OFFIS_CONSOLE_APPLICATION "dcmquant"
#endif

static OFLogger dcmquantLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 3
#define LONGCOL 21


// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;

    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_FileWriteMode opt_writeMode = EWM_fileformat;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    E_TransferSyntax opt_oxfer = EXS_Unknown;
    E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
    E_EncodingType opt_oenctype = EET_ExplicitLength;
    E_PaddingEncoding opt_opadenc = EPD_noChange;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;

    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData;
                                                          /* default: pixel data may detached if no longer needed */
    OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_frameCount = 0;               /* default: all frames */

    OFBool              opt_palette_ow = OFFalse;
    OFBool              opt_entries_word = OFFalse;
    OFBool              opt_palette_fs = OFFalse;
    OFCmdUnsignedInt    opt_palette_col = 256;

    DcmLargestDimensionType opt_largeType = DcmLargestDimensionType_default;
    DcmRepresentativeColorType opt_repType = DcmRepresentativeColorType_default;

    OFBool              opt_secondarycapture = OFFalse;
    OFBool              opt_uidcreation = OFTrue;

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
    // JPEG parameters, currently not used
# if 0
    OFCmdUnsignedInt    opt_quality = 90;                 /* default: 90% JPEG quality */
    E_SubSampling       opt_sampling = ESS_422;           /* default: 4:2:2 sub-sampling */
# endif
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
#endif

    const char *        opt_ifname = NULL;
    const char *        opt_ofname = NULL;

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
    cmd.addParam("dcmfile-out", "DICOM output filename to be written");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                 "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
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

    cmd.addGroup("image processing and encoding options:");
     cmd.addSubGroup("frame selection:");
      cmd.addOption("--frame",               "+fr", 1, "[n]umber: integer",
                                                       "select specified frame");
      cmd.addOption("--all-frames",          "+fa",    "select all frames (default)");

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
     cmd.addSubGroup("color space conversion options (compressed images only):");
      cmd.addOption("--conv-photometric",    "+cp",    "convert if YCbCr photometric interpr. (default)");
      cmd.addOption("--conv-lossy",          "+cl",    "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-guess",          "+cg",    "convert to RGB if YCbCr is guessed by library");
      cmd.addOption("--conv-guess-lossy",    "+cgl",   "convert to RGB if lossy JPEG and YCbCr is\nguessed by the underlying JPEG library");
      cmd.addOption("--conv-always",         "+ca",    "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",          "+cn",    "never convert color space");
#endif

     cmd.addSubGroup("compatibility:");
      cmd.addOption("--accept-palettes",     "+Mp",    "accept incorrect palette attribute tags\n(0028,111x) and (0028,121x)");

     cmd.addSubGroup("median cut dimension selection:");
      cmd.addOption("--mc-dimension-rgb",    "+Dr",    "max dimension from RGB range (default)");
      cmd.addOption("--mc-dimension-lum",    "+Dl",    "max dimension from luminance");

     cmd.addSubGroup("median cut representative color selection:");
      cmd.addOption("--mc-color-avgbox",     "+Cb",    "average colors in box (default)");
      cmd.addOption("--mc-color-avgpixel",   "+Cp",    "average pixels in box");
      cmd.addOption("--mc-color-center",     "+Cc",    "select center of box");

     cmd.addSubGroup("color palette creation:");
      cmd.addOption("--write-ow",            "+pw",    "write Palette LUT as OW instead of US");
      cmd.addOption("--lut-entries-word",    "+pe",    "write Palette LUT with 16-bit entries");
      cmd.addOption("--floyd-steinberg",     "+pf",    "use Floyd-Steinberg error diffusion");
      cmd.addOption("--colors",              "+pc", 1, "number of colors: 2..65536 (default 256)",
                                                       "number of colors to quantize to");

     cmd.addSubGroup("SOP Class UID:");
      cmd.addOption("--class-default",       "+cd",    "keep SOP Class UID (default)");
      cmd.addOption("--class-sc",            "+cs",    "convert to Secondary Capture Image\n(implies --uid-always)");

     cmd.addSubGroup("SOP Instance UID:");
      cmd.addOption("--uid-always",          "+ua",    "always assign new UID (default)");
      cmd.addOption("--uid-never",           "+un",    "never assign new UID");

    cmd.addGroup("output options:");
     cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",     "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
     cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-same",     "+t=",    "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian TS");
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

    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
          if (cmd.findOption("--version"))
          {
              app.printHeader(OFTrue /*print host identifier*/);
              COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(BUILD_WITH_DCMJPEG_SUPPORT)
              COUT << " none" << OFendl;
#else
              COUT << OFendl;
#endif
#ifdef WITH_ZLIB
              COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef BUILD_WITH_DCMJPEG_SUPPORT
              COUT << "- " << DiJPEGPlugin::getLibraryVersionString() << OFendl;
#endif
              return 0;
          }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

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

      if (cmd.findOption("--accept-palettes"))
          opt_compatibilityMode |= CIF_WrongPaletteAttributeTags;

      cmd.beginOptionBlock();
      if (cmd.findOption("--frame"))
      {
          app.checkValue(cmd.getValueAndCheckMin(opt_frame, 1));
          opt_frameCount = 1;
      }
      if (cmd.findOption("--all-frames"))
      {
          opt_frame = 1;
          opt_frameCount = 0;
      }
      cmd.endOptionBlock();

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
      cmd.beginOptionBlock();
      if (cmd.findOption("--conv-photometric"))
          opt_decompCSconversion = EDC_photometricInterpretation;
      if (cmd.findOption("--conv-lossy"))
          opt_decompCSconversion = EDC_lossyOnly;
      if (cmd.findOption("--conv-guess"))
          opt_decompCSconversion = EDC_guess;
      if (cmd.findOption("--conv-guess-lossy"))
          opt_decompCSconversion = EDC_guessLossyOnly;
      if (cmd.findOption("--conv-always"))
          opt_decompCSconversion = EDC_always;
      if (cmd.findOption("--conv-never"))
          opt_decompCSconversion = EDC_never;
      cmd.endOptionBlock();
#endif

      if (cmd.findOption("--write-ow")) opt_palette_ow = OFTrue;
      if (cmd.findOption("--lut-entries-word")) opt_entries_word = OFTrue;
      if (cmd.findOption("--floyd-steinberg")) opt_palette_fs = OFTrue;
      if (cmd.findOption("--colors")) cmd.getValueAndCheckMinMax(opt_palette_col, 2, 65536);

      cmd.beginOptionBlock();
      if (cmd.findOption("--mc-dimension-rgb")) opt_largeType = DcmLargestDimensionType_default;
      if (cmd.findOption("--mc-dimension-lum")) opt_largeType = DcmLargestDimensionType_luminance;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--mc-color-avgbox")) opt_repType = DcmRepresentativeColorType_default;
      if (cmd.findOption("--mc-color-avgpixel")) opt_repType = DcmRepresentativeColorType_averagePixels;
      if (cmd.findOption("--mc-color-center")) opt_repType = DcmRepresentativeColorType_centerOfBox;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--class-default")) opt_secondarycapture = OFFalse;
      if (cmd.findOption("--class-sc")) opt_secondarycapture = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-always")) opt_uidcreation = OFTrue;
      if (cmd.findOption("--uid-never")) opt_uidcreation = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_writeMode = EWM_fileformat;
      if (cmd.findOption("--write-dataset")) opt_writeMode = EWM_dataset;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-same")) opt_oxfer = EXS_Unknown;
      if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
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
    OFLOG_DEBUG(dcmquantLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmquantLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
    // register global decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion);
#endif

    // ======================================================================
    // read input file

    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcmquantLogger, "invalid filename: <empty string>");
        return 1;
    }

    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
    if (error.bad())
    {
        OFLOG_FATAL(dcmquantLogger, error.text() << ": reading file: " <<  opt_ifname);
        return 1;
    }

    OFLOG_INFO(dcmquantLogger, "load all data into memory");

    /* make sure that pixel data is loaded before output file is created */
    dataset->loadAllDataIntoMemory();

    // select uncompressed output transfer syntax.
    // this will implicitly decompress the image if necessary.

    if (opt_oxfer == EXS_Unknown)
    {
        OFLOG_INFO(dcmquantLogger, "set output transfer syntax to input transfer syntax");
        opt_oxfer = dataset->getOriginalXfer();
    }

    OFLOG_INFO(dcmquantLogger, "check if new output transfer syntax is possible");

    DcmXfer opt_oxferSyn(opt_oxfer);
    dataset->chooseRepresentation(opt_oxfer, NULL);

    if (dataset->canWriteXfer(opt_oxfer))
    {
        OFLOG_INFO(dcmquantLogger, "output transfer syntax " << opt_oxferSyn.getXferName()
            << " can be written");
    } else {
        OFLOG_FATAL(dcmquantLogger, "no conversion to transfer syntax " << opt_oxferSyn.getXferName()
            << " possible");
        return 1;
    }

    // ======================================================================
    // image processing starts here

    OFLOG_INFO(dcmquantLogger, "preparing pixel data.");

    // create DicomImage object
    DicomImage di(dataset, opt_oxfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);
    if (di.getStatus() != EIS_Normal)
    {
        OFLOG_FATAL(dcmquantLogger, DicomImage::getString(di.getStatus()));
        return 1;
    }

    if (di.isMonochrome())
    {
      OFLOG_FATAL(dcmquantLogger, "cannot convert monochrome image to palette color");
      return 1;
    }

    OFString derivationDescription;

    // create palette color image
    error = DcmQuant::createPaletteColorImage(
      di, *dataset, opt_palette_ow, opt_entries_word, opt_palette_fs, opt_palette_col,
      derivationDescription, opt_largeType, opt_repType);

    // update image type
    if (error.good()) error = DcmCodec::updateImageType(dataset);

    // update derivation description
    if (error.good()) error = DcmQuant::updateDerivationDescription(dataset, derivationDescription.c_str());

    // create new SOP instance UID
    if (error.good() && (opt_secondarycapture || opt_uidcreation))
      error = DcmCodec::newInstance(dataset, "DCM", "121320", "Uncompressed predecessor");

    // convert to Secondary Capture if requested by user.
    // This method creates a new SOP class UID, so it should be executed
    // after the call to newInstance() which creates a Source Image Sequence.
    if (error.good() && opt_secondarycapture) error = DcmCodec::convertToSecondaryCapture(dataset);

    if (error.bad())
    {
        OFLOG_FATAL(dcmquantLogger, error.text() << ": converting image: " <<  opt_ifname);
        return 1;
    }

    // ======================================================================
    // write back output file

    OFLOG_INFO(dcmquantLogger, "write converted DICOM file");

    // update file meta information with new SOP Instance UID
    if ((opt_uidcreation || opt_secondarycapture) && (opt_writeMode == EWM_fileformat))
        opt_writeMode = EWM_updateMeta;

    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcmquantLogger, error.text() << ": writing file: " <<  opt_ofname);
        return 1;
    }

    OFLOG_INFO(dcmquantLogger, "conversion successful");

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
    // deregister global decompression codecs
    DJDecoderRegistration::cleanup();
#endif

    return 0;
}
