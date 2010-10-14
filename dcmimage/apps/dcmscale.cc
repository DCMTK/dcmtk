/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: Scale DICOM images
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:34 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

#include "dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */

#include "dcmtk/oflog/oflog.h"           /* for OFLogger */

#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */
#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */

#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
#include "dcmtk/dcmjpeg/djdecode.h"      /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"      /* for dcmimage JPEG plugin */
#endif

#ifdef WITH_ZLIB
#include <zlib.h>          /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_DESCRIPTION "Scale DICOM images"

#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
#define OFFIS_CONSOLE_APPLICATION "dcmjscal"
#else
#define OFFIS_CONSOLE_APPLICATION "dcmscale"
#endif

static OFLogger dcmscaleLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 4
#define LONGCOL 21


// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;

    OFBool opt_uidCreation = OFTrue;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_FileWriteMode opt_writeMode = EWM_fileformat;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    E_TransferSyntax opt_oxfer = EXS_Unknown;
    E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
    E_EncodingType opt_oenctype = EET_ExplicitLength;
    E_PaddingEncoding opt_opadenc = EPD_noChange;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;

#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
    // JPEG parameters, currently not used
# if 0
    OFCmdUnsignedInt opt_quality = 90;                 /* default: 90% JPEG quality */
    E_SubSampling opt_sampling = ESS_422;              /* default: 4:2:2 sub-sampling */
# endif
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
#endif

    int opt_useAspectRatio = 1;                        /* default: use aspect ratio for scaling */
    OFCmdUnsignedInt opt_useInterpolation = 1;         /* default: use interpolation method '1' for scaling */
    int opt_scaleType = 0;                             /* default: no scaling */
                                                       /* 1 = X-factor, 2 = Y-factor, 3=X-size, 4=Y-size */
    OFCmdFloat opt_scale_factor = 1.0;
    OFCmdUnsignedInt opt_scale_size = 1;

    OFBool           opt_useClip = OFFalse;            /* default: don't clip */
    OFCmdSignedInt   opt_left = 0, opt_top = 0;        /* clip region (origin) */
    OFCmdUnsignedInt opt_width = 0, opt_height = 0;    /* clip region (extension) */

    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addParam("dcmfile-in",  "DICOM input filename to be scaled");
    cmd.addParam("dcmfile-out", "DICOM output filename to be written");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                "-h",       "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
     cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",          "+f",       "read file format or data set (default)");
      cmd.addOption("--read-file-only",     "+fo",      "read file format only");
      cmd.addOption("--read-dataset",       "-f",       "read data set without file meta information");
     cmd.addSubGroup("input transfer syntax:");
      cmd.addOption("--read-xfer-auto",     "-t=",      "use TS recognition (default)");
      cmd.addOption("--read-xfer-detect",   "-td",      "ignore TS specified in the file meta header");
      cmd.addOption("--read-xfer-little",   "-te",      "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",      "-tb",      "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit", "-ti",      "read with implicit VR little endian TS");

    cmd.addGroup("image processing and encoding options:");
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
     cmd.addSubGroup("color space conversion options (compressed images only):");
      cmd.addOption("--conv-photometric",   "+cp",      "convert if YCbCr photometric interpr. (default)");
      cmd.addOption("--conv-lossy",         "+cl",      "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-guess",         "+cg",      "convert to RGB if YCbCr is guessed by library");
      cmd.addOption("--conv-guess-lossy",   "+cgl",     "convert to RGB if lossy JPEG and YCbCr is\nguessed by the underlying JPEG library");
      cmd.addOption("--conv-always",        "+ca",      "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",         "+cn",      "never convert color space");
#endif
     cmd.addSubGroup("scaling:");
      cmd.addOption("--recognize-aspect",    "+a",      "recognize pixel aspect ratio (default)");
      cmd.addOption("--ignore-aspect",       "-a",      "ignore pixel aspect ratio when scaling");
      cmd.addOption("--interpolate",         "+i",   1, "[n]umber of algorithm: integer",
                                                        "use interpolation when scaling (1..4, def: 1)");
      cmd.addOption("--no-interpolation",    "-i",      "no interpolation when scaling");
      cmd.addOption("--no-scaling",          "-S",      "no scaling, ignore pixel aspect ratio (default)");
      cmd.addOption("--scale-x-factor",      "+Sxf", 1, "[f]actor: float",
                                                        "scale x axis by factor, auto-compute y axis");
      cmd.addOption("--scale-y-factor",      "+Syf", 1, "[f]actor: float",
                                                        "scale y axis by factor, auto-compute x axis");
      cmd.addOption("--scale-x-size",        "+Sxv", 1, "[n]umber: integer",
                                                        "scale x axis to n pixels, auto-compute y axis");
      cmd.addOption("--scale-y-size",        "+Syv", 1, "[n]umber: integer",
                                                        "scale y axis to n pixels, auto-compute x axis");
     cmd.addSubGroup("other transformations:");
      cmd.addOption("--clip-region",         "+C",   4, "[l]eft [t]op [w]idth [h]eight: integer",
                                                        "clip rectangular image region (l, t, w, h)");
     cmd.addSubGroup("SOP Instance UID:");
      cmd.addOption("--uid-always",          "+ua",     "always assign new SOP Instance UID (default)");
      cmd.addOption("--uid-never",           "+un",     "never assign new SOP Instance UID");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",      "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",      "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-same",     "+t=",     "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",   "+te",     "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",      "+tb",     "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit", "+ti",     "write with implicit VR little endian TS");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",       "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",      "-u",      "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc", "+g=",     "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create", "+g",      "always write with group length elements");
      cmd.addOption("--group-length-remove", "-g",      "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",     "+e",      "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",    "-e",      "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",      "-p=",     "do not change padding\n(default if not --write-dataset)");
      cmd.addOption("--padding-off",         "-p",      "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",      "+p",   2, "[f]ile-pad [i]tem-pad: integer",
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
#if !defined(WITH_ZLIB) && !defined(BUILD_DCMSCALE_AS_DCMJSCAL)
              COUT << " none" << OFendl;
#else
              COUT << OFendl;
#endif
#ifdef WITH_ZLIB
              COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
              COUT << "- " << DiJPEGPlugin::getLibraryVersionString() << OFendl;
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

      /* image processing options: color space conversion */

#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
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

      /* image processing options: scaling */

      cmd.beginOptionBlock();
      if (cmd.findOption("--recognize-aspect"))
          opt_useAspectRatio = 1;
      if (cmd.findOption("--ignore-aspect"))
          opt_useAspectRatio = 0;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--interpolate"))
          app.checkValue(cmd.getValueAndCheckMinMax(opt_useInterpolation, 1, 4));
      if (cmd.findOption("--no-interpolation"))
          opt_useInterpolation = 0;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--no-scaling"))
          opt_scaleType = 0;
      if (cmd.findOption("--scale-x-factor"))
      {
          opt_scaleType = 1;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_factor, 0.0, OFFalse));
      }
      if (cmd.findOption("--scale-y-factor"))
      {
          opt_scaleType = 2;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_factor, 0.0, OFFalse));
      }
      if (cmd.findOption("--scale-x-size"))
      {
          opt_scaleType = 3;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_size, 1));
      }
      if (cmd.findOption("--scale-y-size"))
      {
          opt_scaleType = 4;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_size, 1));
      }
      cmd.endOptionBlock();

      /* image processing options: other transformations */

      if (cmd.findOption("--clip-region"))
      {
          app.checkValue(cmd.getValue(opt_left));
          app.checkValue(cmd.getValue(opt_top));
          app.checkValue(cmd.getValue(opt_width));
          app.checkValue(cmd.getValue(opt_height));
          opt_useClip = OFTrue;
      }

      /* image processing options: SOP Instance UID options */

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-always")) opt_uidCreation = OFTrue;
      if (cmd.findOption("--uid-never")) opt_uidCreation = OFFalse;
      cmd.endOptionBlock();

      /* output options */

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
    OFLOG_DEBUG(dcmscaleLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmscaleLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // register RLE decompression codec
    DcmRLEDecoderRegistration::registerCodecs();
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
    // register global decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion);
#endif

    // ======================================================================
    // read input file

    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcmscaleLogger, "invalid input filename: <empty string>");
        return 1;
    }

    /* no clipping/scaling */
    if (!opt_scaleType && !opt_useClip)
    {
        OFLOG_FATAL(dcmscaleLogger, "nothing to do");
        return 1;
    }

    OFLOG_INFO(dcmscaleLogger, "open input file " << opt_ifname);

    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
    if (error.bad())
    {
        OFLOG_FATAL(dcmscaleLogger, error.text() << ": reading file: " <<  opt_ifname);
        return 1;
    }

    OFLOG_INFO(dcmscaleLogger, "load all data into memory");

    /* make sure that pixel data is loaded before output file is created */
    dataset->loadAllDataIntoMemory();

    // select uncompressed output transfer syntax.
    // this will implicitly decompress the image if necessary.

    if (opt_oxfer == EXS_Unknown)
    {
        OFLOG_INFO(dcmscaleLogger, "set output transfer syntax to input transfer syntax");
        opt_oxfer = dataset->getOriginalXfer();
    }

    OFLOG_INFO(dcmscaleLogger, "check if new output transfer syntax is possible");

    DcmXfer opt_oxferSyn(opt_oxfer);
    dataset->chooseRepresentation(opt_oxfer, NULL);

    if (dataset->canWriteXfer(opt_oxfer))
    {
        OFLOG_INFO(dcmscaleLogger, "output transfer syntax " << opt_oxferSyn.getXferName()
            << " can be written");
    } else {
        OFLOG_FATAL(dcmscaleLogger, "no conversion to transfer syntax " << opt_oxferSyn.getXferName()
            << " possible");
        return 1;
    }

    // ======================================================================
    // image processing starts here

    OFLOG_INFO(dcmscaleLogger, "preparing pixel data");

    const unsigned long flags = (opt_scaleType > 0) ? CIF_MayDetachPixelData : 0;
    // create DicomImage object
    DicomImage *di = new DicomImage(dataset, opt_oxfer, flags);
    if (di == NULL)
    {
        OFLOG_FATAL(dcmscaleLogger, "memory exhausted");
        return 1;
    }
    if (di->getStatus() != EIS_Normal)
    {
        OFLOG_FATAL(dcmscaleLogger, DicomImage::getString(di->getStatus()));
        return 1;
    }

    DicomImage *newimage = NULL;
    OFString derivationDescription;

    if (opt_useClip)
        OFLOG_INFO(dcmscaleLogger, "clipping image to (" << opt_left << "," << opt_top
            << "," << opt_width << "," << opt_height << ")");
    // perform clipping (without scaling)
    if (opt_scaleType <= 0)
    {
        if (opt_useClip)
        {
            newimage = di->createClippedImage(opt_left, opt_top, opt_width, opt_height);
            derivationDescription = "Clipped rectangular image region";
        }
    }
    // perform scaling (and possibly clipping)
    else if (opt_scaleType <= 4)
    {
        switch (opt_scaleType)
        {
            case 1:
                OFLOG_INFO(dcmscaleLogger, "scaling image, X factor=" << opt_scale_factor
                    << ", Interpolation=" << OFstatic_cast(int, opt_useInterpolation)
                    << ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_factor, 0.0,
                        OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(opt_scale_factor, 0.0, OFstatic_cast(int, opt_useInterpolation),
                        opt_useAspectRatio);
                break;
            case 2:
                OFLOG_INFO(dcmscaleLogger, "scaling image, Y factor=" << opt_scale_factor
                    << ", Interpolation=" << OFstatic_cast(int, opt_useInterpolation)
                    << ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0.0, opt_scale_factor,
                        OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(0.0, opt_scale_factor, OFstatic_cast(int, opt_useInterpolation),
                        opt_useAspectRatio);
                break;
            case 3:
                OFLOG_INFO(dcmscaleLogger, "scaling image, X size=" << opt_scale_size
                    << ", Interpolation=" << OFstatic_cast(int, opt_useInterpolation)
                    << ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_size, 0,
                        OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(opt_scale_size, 0, OFstatic_cast(int, opt_useInterpolation),
                        opt_useAspectRatio);
                break;
            case 4:
                OFLOG_INFO(dcmscaleLogger, "scaling image, Y size=" << opt_scale_size
                    << ", Interpolation=" << OFstatic_cast(int, opt_useInterpolation)
                    << ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0, opt_scale_size,
                        OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(0, opt_scale_size, OFstatic_cast(int, opt_useInterpolation),
                        opt_useAspectRatio);
                break;
            default:
                break;
        }
        if (opt_useClip)
            derivationDescription = "Scaled rectangular image region";
        else
            derivationDescription = "Scaled image";
    }
    if (opt_scaleType > 4)
        OFLOG_ERROR(dcmscaleLogger, "internal error: unknown scaling type");
    else if (newimage == NULL)
    {
        OFLOG_FATAL(dcmscaleLogger, "cannot create new image");
        return 1;
    }
    else if (newimage->getStatus() != EIS_Normal)
    {
        OFLOG_FATAL(dcmscaleLogger, DicomImage::getString(newimage->getStatus()));
        return 1;
    }
    /* write scaled image to dataset (update attributes of Image Pixel Module) */
    else if (!newimage->writeImageToDataset(*dataset))
    {
        OFLOG_FATAL(dcmscaleLogger, "cannot write new image to dataset");
        return 1;
    }
    delete newimage;

    /* cleanup original image */
    delete di;

    // ======================================================================
    // update some header attributes

    // update Derivation Description
    if (!derivationDescription.empty())
    {
        const char *oldDerivation = NULL;
        if (dataset->findAndGetString(DCM_DerivationDescription, oldDerivation).good())
        {
             // append old Derivation Description, if any
            derivationDescription += " [";
            derivationDescription += oldDerivation;
            derivationDescription += "]";
            if (derivationDescription.length() > 1024)
            {
                // ST is limited to 1024 characters, cut off tail
                derivationDescription.erase(1020);
                derivationDescription += "...]";
            }
        }
        dataset->putAndInsertString(DCM_DerivationDescription, derivationDescription.c_str());
    }

    // update Image Type
    OFString imageType = "DERIVED";
    const char *oldImageType = NULL;
    if (dataset->findAndGetString(DCM_ImageType, oldImageType).good())
    {
        // append old image type information beginning with second entry
        const char *pos = strchr(oldImageType, '\\');
        if (pos != NULL)
            imageType += pos;
    }
    dataset->putAndInsertString(DCM_ImageType, imageType.c_str());

    // update SOP Instance UID
    if (opt_uidCreation)
    {
        // add reference to source image
        DcmItem *ditem = NULL;
        const char *sopClassUID = NULL;
        const char *sopInstanceUID = NULL;
        if (dataset->findAndGetString(DCM_SOPClassUID, sopClassUID).good() &&
            dataset->findAndGetString(DCM_SOPInstanceUID, sopInstanceUID).good())
        {
            dataset->findAndDeleteElement(DCM_SourceImageSequence);
            if (dataset->findOrCreateSequenceItem(DCM_SourceImageSequence, ditem).good())
            {
                ditem->putAndInsertString(DCM_SOPClassUID, sopClassUID);
                ditem->putAndInsertString(DCM_SOPInstanceUID, sopInstanceUID);
            }
        }
        // create new SOP instance UID
        char new_uid[100];
        dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(new_uid));
        // force meta-header to refresh SOP Instance UID
        if (opt_writeMode == EWM_fileformat)
            opt_writeMode = EWM_updateMeta;
    }

    // ======================================================================
    // write back output file

    OFLOG_INFO(dcmscaleLogger, "create output file " << opt_ofname);

    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcmscaleLogger, error.text() << ": writing file: " <<  opt_ofname);
        return 1;
    }

    OFLOG_INFO(dcmscaleLogger, "conversion successful");

    // deregister RLE decompression codec
    DcmRLEDecoderRegistration::cleanup();
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
    // deregister global decompression codecs
    DJDecoderRegistration::cleanup();
#endif

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmscale.cc,v $
 * Revision 1.26  2010-10-14 13:13:34  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.25  2010-03-24 15:07:25  joergr
 * Added missing command line options for the color space conversion.
 *
 * Revision 1.24  2009-10-14 10:26:37  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.23  2009-10-13 14:08:33  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.22  2009-08-21 09:28:12  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 *
 * Revision 1.21  2009-04-21 14:04:12  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.20  2009-04-03 11:45:02  joergr
 * Added check whether neither scaling nor clipping is required.
 *
 * Revision 1.19  2008-09-25 14:43:22  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.18  2008-09-25 12:47:58  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.17  2008-05-20 09:58:22  joergr
 * Added new bilinear and bicubic scaling algorithms for image magnification.
 * Allow width and height of the clipping area to be 0 (compute automatically).
 *
 * Revision 1.16  2006/08/15 16:35:00  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.15  2006/07/27 13:59:24  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Option "--help" is no longer an exclusive option
 * by default.
 *
 * Revision 1.14  2005/12/15 17:42:10  joergr
 * Changed type of local variable, reported by Sun CC 2.0.1 on Solaris.
 *
 * Revision 1.13  2005/12/08 15:42:18  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.12  2005/12/02 09:31:17  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 *
 * Revision 1.11  2005/07/26 18:29:01  joergr
 * Added new command line option that allows to clip a rectangular image region
 * (combination with scaling not yet fully implemented in corresponding classes).
 * Update ImageType, add DerivationDescription and SourceImageSequence.
 * Cleaned up use of CERR and COUT.
 *
 * Revision 1.10  2005/03/22 13:54:10  joergr
 * Minor code corrections, e.g. write pixel data if no scaling factor is given.
 *
 * Revision 1.9  2003/12/11 15:39:50  joergr
 * Made usage output consistent with other tools.
 *
 * Revision 1.8  2003/12/05 10:50:52  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.7  2002/12/13 13:44:43  meichel
 * Activated file padding options
 *
 * Revision 1.6  2002/11/27 14:16:53  meichel
 * Adapted module dcmimage to use of new header file ofstdinc.h
 *
 * Revision 1.5  2002/11/26 08:44:57  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.4  2002/09/23 18:01:19  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.3  2002/08/21 09:54:07  meichel
 * Fixed argument lists for loadFile and saveFile
 *
 * Revision 1.2  2002/08/20 12:20:21  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.1  2002/08/02 15:14:16  joergr
 * Added new command line program which allows to scale DICOM images.
 *
 */
