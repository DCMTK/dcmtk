/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmimage
 *
 *  Authors: Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose: Convert DICOM Images to PPM or PGM using the dcmimage library.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-23 18:01:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/apps/dcm2pnm.cc,v $
 *  CVS/RCS Revision: $Revision: 1.66 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
END_EXTERN_C

#ifdef HAVE_GUSI_H
# include <GUSI.h>
#endif

#include "dctk.h"          /* for various dcmdata headers */
#include "dcutils.h"       /* for getSingleValue */
#include "dcdebug.h"       /* for SetDebugLevel */
#include "cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcuid.h"         /* for dcmtk version name */
#include "dcrledrg.h"      /* for DcmRLEDecoderRegistration */

#include "dcmimage.h"      /* for DicomImage */
#include "digsdfn.h"       /* for DiGSDFunction */
#include "diciefn.h"       /* for DiCIELABFunction */

#include "ofconapp.h"      /* for OFConsoleApplication */
#include "ofcmdln.h"       /* for OFCommandLine */

#include "diregist.h"      /* include to support color images */

#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
# include "djdecode.h"     /* for dcmjpeg decoders */
# include "dipijpeg.h"     /* for dcmimage JPEG plugin */
#endif

#ifdef WITH_LIBTIFF
# include "dipitiff.h"     /* for dcmimage TIFF plugin */
#endif

#ifdef WITH_ZLIB
# include "zlib.h"         /* for zlibVersion() */
#endif

#include "ofstream.h"

#define OFFIS_OUTFILE_DESCRIPTION "output filename to be written (default: stdout)"

#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
# define OFFIS_CONSOLE_APPLICATION "dcmj2pnm"
# ifdef WITH_LIBTIFF
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM, BMP, TIFF or JPEG"
# else
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM, BMP or JPEG"
# endif
#else
# define OFFIS_CONSOLE_APPLICATION "dcm2pnm"
# ifdef WITH_LIBTIFF
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM, BMP or TIFF"
# else
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM or BMP"
# endif
#endif

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 4
#define LONGCOL 20

#define OUTPUT CERR


// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;

    int                 opt_readAsDataset = 0;            /* default: fileformat or dataset */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */

    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
                                                          /* default: pixel data may detached if no longer needed */
    OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_frameCount = 1;               /* default: one frame */
    int                 opt_multiFrame = 0;               /* default: no multiframes */
    int                 opt_convertToGrayscale = 0;       /* default: color or grayscale */
    int                 opt_changePolarity = 0;           /* default: normal polarity */
    int                 opt_useAspectRatio = 1;           /* default: use aspect ratio for scaling */
    OFCmdUnsignedInt    opt_useInterpolation = 1;         /* default: use interpolation method '1' for scaling */
    int                 opt_useClip = 0;                  /* default: don't clip */
    OFCmdSignedInt      opt_left=0, opt_top=0;            /* clip region (origin) */
    OFCmdUnsignedInt    opt_width=0, opt_height=0;        /* clip region (extension) */
    int                 opt_rotateDegree = 0;             /* default: no rotation */
    int                 opt_flipType = 0;                 /* default: no flipping */
    int                 opt_scaleType = 0;                /* default: no scaling */
                        /* 1 = X-factor, 2 = Y-factor, 3=X-size, 4=Y-size */
    OFCmdFloat          opt_scale_factor = 1.0;
    OFCmdUnsignedInt    opt_scale_size = 1;
    int                 opt_windowType = 0;               /* default: no windowing */
                        /* 1=Wi, 2=Wl, 3=Wm, 4=Wh, 5=Ww, 6=Wn, 7=Wr */
    OFCmdUnsignedInt    opt_windowParameter = 0;
    OFCmdUnsignedInt    opt_roiLeft = 0, opt_roiTop = 0, opt_roiWidth = 0, opt_roiHeight = 0;
    OFCmdFloat          opt_windowCenter = 0.0, opt_windowWidth = 0.0;

    ES_PresentationLut  opt_presShape = ESP_Default;
    OFString            opt_displayFile;
    int                 opt_displayFunction = 0;          /* default: GSDF */
    OFCmdFloat          opt_ambientLight = -1;            /* default: not set */
    OFCmdFloat          opt_illumination = -1;            /* default: not set */
    DiDisplayFunction::E_DeviceType deviceType = DiDisplayFunction::EDT_Monitor;

#ifdef WITH_LIBTIFF
    // TIFF parameters
    DiTIFFCompression   opt_tiffCompression = E_tiffLZWCompression;
    DiTIFFLZWPredictor  opt_lzwPredictor = E_tiffLZWPredictorDefault;
    OFCmdUnsignedInt    opt_rowsPerStrip = 0;
#endif

#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
    // JPEG parameters
    OFCmdUnsignedInt    opt_quality = 90;                 /* default: 90% JPEG quality */
    E_SubSampling       opt_sampling = ESS_422;           /* default: 4:2:2 sub-sampling */
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
#endif

    int                 opt_Overlay[16];
    int                 opt_O_used = 0;                   /* flag for +O parameter */
    int                 opt_OverlayMode = 0;              /* default: Replace or ROI */
                        /* 1=replace, 2=threshold-replace, 3=complement, 4=ROI */

    OFCmdFloat          opt_foregroundDensity = 1.0;
    OFCmdFloat          opt_thresholdDensity  = 0.5;

    int                 opt_verboseMode = 1;              /* default: be more or less quiet */
    int                 opt_imageInfo = 0;                /* default: no info */
    int                 opt_debugMode = 0;                /* default: no debug */
    int                 opt_suppressOutput = 0;           /* default: create output */
    int                 opt_fileType = 0;                 /* default: 8-bit PGM/PPM */
                                                          /* (binary for file output and ASCII for stdout) */
                        /* 2=8-bit-ASCII, 3=16-bit-ASCII, 4=n-bit-ASCII, 5=pastel color */
    OFCmdUnsignedInt    opt_fileBits = 0;                 /* default: 0 */
    const char *        opt_ifname = NULL;
    const char *        opt_ofname = NULL;

    unsigned long i;
    for (i = 0; i < 16; i++)
        opt_Overlay[i] = 2;                               /* default: display all overlays if present */

    SetDebugLevel((0));
    DicomImageClass::setDebugLevel(DicomImageClass::DL_Warnings | DicomImageClass::DL_Errors);

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
    cmd.addParam("pnmfile-out", OFFIS_OUTFILE_DESCRIPTION, OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                "-h",      "print this help text and exit" /*, OFTrue is set implicitly */);
     cmd.addOption("--version",                        "print version information and exit", OFTrue /* exclusive */);
     cmd.addOption("--verbose",             "-v",      "verbose mode, print processing details");
     cmd.addOption("--quiet",               "-q",      "quiet mode, print no warnings and errors");
     cmd.addOption("--debug",               "-d",      "debug mode, print debug information");
     cmd.addOption("--image-info",          "-im",     "info mode, print image details");

    cmd.addGroup("input options:");

     cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",          "+f",      "read file format or data set (default)");
      cmd.addOption("--read-dataset",       "-f",      "read data set without file meta information");

     cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
      cmd.addOption("--read-xfer-auto",     "-t=",     "use TS recognition (default)");
      cmd.addOption("--read-xfer-little",   "-te",     "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",      "-tb",     "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit", "-ti",     "read with implicit VR little endian TS");

    cmd.addGroup("image processing options:");

     cmd.addSubGroup("frame selection:");
      cmd.addOption("--frame",              "+F",   1, "[n]umber : integer",
                                                        "select specified frame (default: 1)");
      cmd.addOption("--frame-range",        "+Fr",  2, "[n]umber [c]ount : integer",
                                                       "select c frames beginning with frame n");
      cmd.addOption("--all-frames",         "+Fa",     "select all frames");

     cmd.addSubGroup("rotation:");
      cmd.addOption("--rotate-left",        "+Rl",     "rotate image left (-90 degrees)");
      cmd.addOption("--rotate-right",       "+Rr",     "rotate image right (+90 degrees)");
      cmd.addOption("--rotate-top-down",    "+Rtd",    "rotate image top-down (180 degrees)");

     cmd.addSubGroup("flipping:");
      cmd.addOption("--flip-horizontally",  "+Lh",     "flip image horizontally");
      cmd.addOption("--flip-vertically",    "+Lv",     "flip image vertically");
      cmd.addOption("--flip-both-axes",     "+Lhv",    "flip image horizontally and vertically");

     cmd.addSubGroup("scaling:");
      cmd.addOption("--recognize-aspect",   "+a",      "recognize pixel aspect ratio (default)");
      cmd.addOption("--ignore-aspect",      "-a",      "ignore pixel aspect ratio when scaling");
      cmd.addOption("--interpolate",        "+i",   1, "[n]umber of algorithm : integer",
                                                       "use interpolation when scaling (1..2, def: 1)");
      cmd.addOption("--no-interpolation",   "-i",      "no interpolation when scaling");
      cmd.addOption("--no-scaling",         "-S",      "no scaling, ignore pixel aspect ratio (default)");
      cmd.addOption("--scale-x-factor",     "+Sxf", 1, "[f]actor : float",
                                                       "scale x axis by factor, auto-compute y axis");
      cmd.addOption("--scale-y-factor",     "+Syf", 1, "[f]actor : float",
                                                       "scale y axis by factor, auto-compute x axis");
      cmd.addOption("--scale-x-size",       "+Sxv", 1, "[n]umber : integer",
                                                       "scale x axis to n pixels, auto-compute y axis");
      cmd.addOption("--scale-y-size",       "+Syv", 1, "[n]umber : integer",
                                                       "scale y axis to n pixels, auto-compute x axis");
#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
     cmd.addSubGroup("color space conversion (compressed images only):");
      cmd.addOption("--conv-photometric",   "+cp",     "convert if YCbCr photom. interpr. (default)");
      cmd.addOption("--conv-lossy",         "+cl",     "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-always",        "+ca",     "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",         "+cn",     "never convert color space");
#endif

     cmd.addSubGroup("VOI windowing:");
      cmd.addOption("--no-windowing",       "-W",      "no VOI windowing (default)");
      cmd.addOption("--use-window",         "+Wi",  1, "[n]umber : integer",
                                                       "use the n-th VOI window from image file");
      cmd.addOption("--use-voi-lut",        "+Wl",  1, "[n]umber : integer",
                                                       "use the n-th VOI look up table from image file");
      cmd.addOption("--min-max-window",     "+Wm",     "compute VOI window using min-max algorithm");
      cmd.addOption("--min-max-window-n",   "+Wn",     "compute VOI window using min-max algorithm,\nignoring extreme values");
      cmd.addOption("--roi-min-max-window", "+Wr",  4, "[l]eft [t]op [w]idth [h]eight : integer",
                                                       "compute ROI window using min-max algorithm,\nregion of interest is specified by l,t,w,h");
      cmd.addOption("--histogram-window",   "+Wh",  1, "[n]umber: integer",
                                                       "compute VOI window using Histogram algorithm,\nignoring n percent");
      cmd.addOption("--set-window",         "+Ww",  2, "[c]enter [w]idth : float",
                                                       "compute VOI window using center c and width w");

     cmd.addSubGroup("presentation LUT transformation:");
      cmd.addOption("--identity-shape",     "+Pid",    "presentation LUT shape IDENTITY");
      cmd.addOption("--inverse-shape",      "+Piv",    "presentation LUT shape INVERSE");
      cmd.addOption("--lin-od-shape",       "+Pod",    "presentation LUT shape LIN OD");

     cmd.addSubGroup("overlay:");
      cmd.addOption("--no-overlays",        "-O",      "do not display overlays");
      cmd.addOption("--display-overlay",    "+O" ,  1, "[n]umber : integer",
                                                       "display overlay n (0..16, 0=all, default: +O 0)");
      cmd.addOption("--ovl-replace",        "+Omr",    "use overlay mode \"Replace\"\n(default for Graphic overlays)");
      cmd.addOption("--ovl-threshold",      "+Omt",    "use overlay mode \"Threshold-Replace\"");
      cmd.addOption("--ovl-complement",     "+Omc",    "use overlay mode \"Complement\"");
      cmd.addOption("--ovl-roi",            "+Omi",    "use overlay mode \"Region of Interest\"\n(default for ROI overlays)");
      cmd.addOption("--set-foreground",     "+Osf", 1, "[d]ensity : float",
                                                       "set overlay foreground density (0..1, def: 1)");
      cmd.addOption("--set-threshold",      "+Ost", 1, "[d]ensity : float",
                                                       "set overlay threshold density (0..1, def: 0.5)");

     cmd.addSubGroup("display LUT transformation:");
      cmd.addOption("--monitor-file",       "+Dm",  1, "[f]ilename : string",
                                                       "calibrate output according to monitor\ncharacteristics defined in f");
      cmd.addOption("--printer-file",       "+Dp",  1, "[f]ilename : string",
                                                       "calibrate output according to printer\ncharacteristics defined in f");
      cmd.addOption("--ambient-light",      "+Da",  1, "[a]mbient light : float",
                                                       "ambient light value (cd/m^2, default: file f)");
      cmd.addOption("--illumination",       "+Di",  1, "[i]llumination : float",
                                                       "illumination value (cd/m^2, default: file f)");
      cmd.addOption("--gsd-function",       "+Dg",     "use GSDF for calibration (default for +Dm/+Dp)");
      cmd.addOption("--cielab-function",    "+Dc",     "use CIELAB function for calibration ");

     cmd.addSubGroup("compatibility options:");
      cmd.addOption("--accept-acr-nema",    "+Ma",     "accept ACR-NEMA images without photometric\ninterpretation");
      cmd.addOption("--accept-palettes",    "+Mp",     "accept incorrect palette attribute tags\n(0028,111x) and (0028,121x)");

#ifdef WITH_LIBTIFF
     cmd.addSubGroup("TIFF options:");
      cmd.addOption("--compr-lzw",          "+Tl",     "LZW compression (default)");
      cmd.addOption("--compr-rle",          "+Tr",     "RLE compression");
      cmd.addOption("--compr-none",         "+Tn",     "uncompressed");
      cmd.addOption("--predictor-default",  "+Pd",     "no LZW predictor (default)");
      cmd.addOption("--predictor-none",     "+Pn",     "LZW predictor 1 (no prediction)");
      cmd.addOption("--predictor-horz",     "+Ph",     "LZW predictor 2 (horizontal differencing)");
      cmd.addOption("--rows-per-strip",     "+Rs",  1, "[r]ows : integer (default: 0)",
                                                       "rows per strip, default 8K per strip");
#endif

#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
     cmd.addSubGroup("JPEG options:");
      cmd.addOption("--compr-quality",      "+Jq",  1, "[q]uality : integer (0..100, default: 90)",
                                                       "quality value for compression (in percent)");
      cmd.addOption("--sample-444",         "+Js4",    "4:4:4 sampling (no subsampling)");
      cmd.addOption("--sample-422",         "+Js2",    "4:2:2 subsampling (horizontal subsampling of\nchroma components, default)");
      cmd.addOption("--sample-411",         "+Js1",    "4:1:1 subsampling (horizontal and vertical\nsubsampling of chroma components)");
#endif

     cmd.addSubGroup("other transformations:");
      cmd.addOption("--grayscale",          "+G",      "convert to grayscale if necessary");
      cmd.addOption("--change-polarity",    "+P",      "change polarity (invert pixel output)");
      cmd.addOption("--clip-region",        "+C",   4, "[l]eft [t]op [w]idth [h]eight : integer",
                                                       "clip image region (l, t, w, h)");

    cmd.addGroup("output options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--no-output",           "-o",      "do not create any output (useful with -im)");
     cmd.addOption("--write-raw-pnm",       "+op",     "write 8-bit binary PGM/PPM (default for files)");
     cmd.addOption("--write-8-bit-pnm",     "+opb",    "write 8-bit ASCII PGM/PPM (default for stdout)");
     cmd.addOption("--write-16-bit-pnm",    "+opw",    "write 16-bit ASCII PGM/PPM");
     cmd.addOption("--write-n-bit-pnm",     "+opn", 1, "[n]umber : integer",
                                                       "write n-bit ASCII PGM/PPM (1..32)");
     cmd.addOption("--write-bmp",           "+ob",     "write 8-bit (monochrome) or 24-bit (color) BMP");
     cmd.addOption("--write-8-bit-bmp",     "+obp",    "write 8-bit palette BMP (monochrome only)");
     cmd.addOption("--write-24-bit-bmp",    "+obt",    "write 24-bit truecolor BMP");
#ifdef WITH_LIBTIFF
     cmd.addOption("--write-tiff",          "+ot",     "write 8-bit (monochrome) or 24-bit (color) TIFF");
#endif
#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
     cmd.addOption("--write-jpeg",          "+oj",     "write 8-bit lossy JPEG (baseline)");
#endif
#ifdef PASTEL_COLOR_OUTPUT
     cmd.addOption("--write-pastel-pnm",    "+op",     "write 8-bit binary PPM with pastel colors\n(early experimental version)");
#endif

    if (app.parseCommandLine(cmd, argc, argv))
    {
        /* check exclusive options first */

        if (cmd.getParamCount() == 0)
        {
            if (cmd.findOption("--version"))
            {
                app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
                CERR << endl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(BUILD_DCM2PNM_AS_DCMJ2PNM) && !defined(WITH_LIBTIFF)
                CERR << " none" << endl;
#else
                CERR << endl;
#endif
#ifdef WITH_ZLIB
                CERR << "- ZLIB, Version " << zlibVersion() << endl;
#endif
#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
                CERR << "- " << DiJPEGPlugin::getLibraryVersionString() << endl;
#endif
#ifdef WITH_LIBTIFF
                CERR << "- " << DiTIFFPlugin::getLibraryVersionString() << endl;
#endif
                return 0;
            }
        }

        /* command line parameters */

        cmd.getParam(1, opt_ifname);
        cmd.getParam(2, opt_ofname);

        /* general options */

        cmd.beginOptionBlock();
        if (cmd.findOption("--verbose"))
            opt_verboseMode = 2;
        if (cmd.findOption("--quiet"))
        {
            opt_verboseMode = 0;
            app.setQuietMode();
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--debug"))
            opt_debugMode = 1;
        if (cmd.findOption("--image-info"))
            opt_imageInfo = 1;

        /* input options: input file format */

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-dataset"))
            opt_readAsDataset = 1;
        if (cmd.findOption("--read-file"))
            opt_readAsDataset = 0;
        cmd.endOptionBlock();

        /* input options: input transfer syntax */

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-xfer-auto"))
            opt_transferSyntax = EXS_Unknown;
        if (cmd.findOption("--read-xfer-implicit"))
            opt_transferSyntax = EXS_LittleEndianImplicit;
        if (cmd.findOption("--read-xfer-little"))
            opt_transferSyntax = EXS_LittleEndianExplicit;
        if (cmd.findOption("--read-xfer-big"))
            opt_transferSyntax = EXS_BigEndianExplicit;
        cmd.endOptionBlock();

        /* image processing options: compatibility options */

        if (cmd.findOption("--accept-acr-nema"))
            opt_compatibilityMode |= CIF_AcrNemaCompatibility;
        if (cmd.findOption("--accept-palettes"))
            opt_compatibilityMode |= CIF_WrongPaletteAttributeTags;

        /* image processing options: frame selection */

        cmd.beginOptionBlock();
        if (cmd.findOption("--frame"))
            app.checkValue(cmd.getValueAndCheckMin(opt_frame, 1));
        if (cmd.findOption("--frame-range"))
        {
            app.checkValue(cmd.getValueAndCheckMin(opt_frame, 1));
            app.checkValue(cmd.getValueAndCheckMin(opt_frameCount, 1));
            opt_multiFrame = 1;
        }
        if (cmd.findOption("--all-frames"))
        {
            opt_frameCount = 0;
            opt_multiFrame = 1;
        }
        cmd.endOptionBlock();

        /* image processing options: other transformations */

        if (cmd.findOption("--grayscale"))
            opt_convertToGrayscale = 1;
        if (cmd.findOption("--change-polarity"))
            opt_changePolarity = 1;

        if (cmd.findOption("--clip-region"))
        {
            app.checkValue(cmd.getValue(opt_left));
            app.checkValue(cmd.getValue(opt_top));
            app.checkValue(cmd.getValueAndCheckMin(opt_width, 1));
            app.checkValue(cmd.getValueAndCheckMin(opt_height, 1));
            opt_useClip = 1;
        }

        /* image processing options: rotation */

        cmd.beginOptionBlock();
        if (cmd.findOption("--rotate-left"))
            opt_rotateDegree = 270;
        if (cmd.findOption("--rotate-right"))
            opt_rotateDegree = 90;
        if (cmd.findOption("--rotate-top-down"))
            opt_rotateDegree = 180;
        cmd.endOptionBlock();

        /* image processing options: flipping */

        cmd.beginOptionBlock();
        if (cmd.findOption("--flip-horizontally"))
            opt_flipType = 1;
        if (cmd.findOption("--flip-vertically"))
            opt_flipType = 2;
        if (cmd.findOption("--flip-both-axes"))
            opt_flipType = 3;
        cmd.endOptionBlock();

        /* image processing options: scaling */

        cmd.beginOptionBlock();
        if (cmd.findOption("--recognize-aspect"))
            opt_useAspectRatio = 1;
        if (cmd.findOption("--ignore-aspect"))
            opt_useAspectRatio = 0;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--interpolate"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_useInterpolation, 1, 2));
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

        /* image processing options: color space conversion */

#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
        cmd.beginOptionBlock();
        if (cmd.findOption("--conv-photometric"))
            opt_decompCSconversion = EDC_photometricInterpretation;
        if (cmd.findOption("--conv-lossy"))
            opt_decompCSconversion = EDC_lossyOnly;
        if (cmd.findOption("--conv-always"))
            opt_decompCSconversion = EDC_always;
        if (cmd.findOption("--conv-never"))
            opt_decompCSconversion = EDC_never;
        cmd.endOptionBlock();
#endif

        /* image processing options: VOI windowing */

        cmd.beginOptionBlock();
        if (cmd.findOption("--no-windowing"))
            opt_windowType = 0;
        if (cmd.findOption("--use-window"))
        {
            opt_windowType = 1;
            app.checkValue(cmd.getValueAndCheckMin(opt_windowParameter, 1));
        }
        if (cmd.findOption("--use-voi-lut"))
        {
            opt_windowType = 2;
            app.checkValue(cmd.getValueAndCheckMin(opt_windowParameter, 1));
        }
        if (cmd.findOption("--min-max-window"))
            opt_windowType = 3;
        if (cmd.findOption("--min-max-window-n"))
            opt_windowType = 6;
        if (cmd.findOption("--roi-min-max-window"))
        {
            opt_windowType = 7;
            app.checkValue(cmd.getValue(opt_roiLeft));
            app.checkValue(cmd.getValue(opt_roiTop));
            app.checkValue(cmd.getValueAndCheckMin(opt_roiWidth, 1));
            app.checkValue(cmd.getValueAndCheckMin(opt_roiHeight, 1));
        }
        if (cmd.findOption("--histogram-window"))
        {
            opt_windowType = 4;
            app.checkValue(cmd.getValueAndCheckMinMax(opt_windowParameter, 0, 100));
        }
        if (cmd.findOption("--set-window"))
        {
            opt_windowType = 5;
            app.checkValue(cmd.getValue(opt_windowCenter));
            app.checkValue(cmd.getValueAndCheckMin(opt_windowWidth, 1.0));
        }
        cmd.endOptionBlock();

        /* image processing options: presentation LUT transformation */

        cmd.beginOptionBlock();
        if (cmd.findOption("--identity-shape"))
            opt_presShape = ESP_Identity;
        if (cmd.findOption("--inverse-shape"))
            opt_presShape = ESP_Inverse;
        if (cmd.findOption("--lin-od-shape"))
            opt_presShape = ESP_LinOD;
        cmd.endOptionBlock();

        /* image processing options: display LUT transformation */

        cmd.beginOptionBlock();
        if (cmd.findOption("--monitor-file"))
        {
            app.checkValue(cmd.getValue(opt_displayFile));
            deviceType = DiDisplayFunction::EDT_Monitor;
        }
        if (cmd.findOption("--printer-file"))
        {
            app.checkValue(cmd.getValue(opt_displayFile));
            deviceType = DiDisplayFunction::EDT_Printer;
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--ambient-light"))
            app.checkValue(cmd.getValueAndCheckMin(opt_ambientLight, 0));
        if (cmd.findOption("--illumination"))
            app.checkValue(cmd.getValueAndCheckMin(opt_illumination, 0));

        cmd.beginOptionBlock();
        if (cmd.findOption("--gsd-function"))
            opt_displayFunction = 0;
        if (cmd.findOption("--cielab-function"))
            opt_displayFunction = 1;
        cmd.endOptionBlock();

        /* image processing options: overlay */

        cmd.beginOptionBlock();
        if (cmd.findOption("--no-overlays"))
        {
            opt_O_used = 1;
            for (i = 0; i < 16; i++)
                opt_Overlay[i] = 0;
        }
        if (cmd.findOption("--display-overlay", 0, OFCommandLine::FOM_First))
        {
            do {
                unsigned long l;
                app.checkValue(cmd.getValueAndCheckMinMax(l, 1, 16));
                if (!opt_O_used)
                {
                    for (i = 0; i < 16; i++) opt_Overlay[i] = 0;
                    opt_O_used = 1;
                }
                if (l > 0)
                    opt_Overlay[l - 1]=1;
                else
                {
                    for (i = 0; i < 16; i++)
                        opt_Overlay[i] = 2;
                }
            } while (cmd.findOption("--display-overlay", 0, OFCommandLine::FOM_Next));
        }
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--ovl-replace"))
            opt_OverlayMode = 1;
        if (cmd.findOption("--ovl-threshold"))
            opt_OverlayMode = 2;
        if (cmd.findOption("--ovl-complement"))
            opt_OverlayMode = 3;
        if (cmd.findOption("--ovl-roi"))
            opt_OverlayMode = 4;
        cmd.endOptionBlock();

        if (cmd.findOption("--set-foreground"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_foregroundDensity, 0.0, 1.0));
        if (cmd.findOption("--set-threshold"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_thresholdDensity, 0.0, 1.0));

        /* image processing options: TIFF options */

#ifdef WITH_LIBTIFF
        cmd.beginOptionBlock();
        if (cmd.findOption("--compr-lzw")) opt_tiffCompression = E_tiffLZWCompression;
        if (cmd.findOption("--compr-rle")) opt_tiffCompression = E_tiffPackBitsCompression;
        if (cmd.findOption("--compr-none")) opt_tiffCompression = E_tiffNoCompression;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--predictor-default")) opt_lzwPredictor = E_tiffLZWPredictorDefault;
        if (cmd.findOption("--predictor-none")) opt_lzwPredictor = E_tiffLZWPredictorNoPrediction;
        if (cmd.findOption("--predictor-horz")) opt_lzwPredictor = E_tiffLZWPredictorHDifferencing;
        cmd.endOptionBlock();

        if (cmd.findOption("--rows-per-strip"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_rowsPerStrip, 0, 65535));
#endif

        /* image processing options: JPEG options */

#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
        if (cmd.findOption("--compr-quality"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_quality, 0, 100));
        cmd.beginOptionBlock();
        if (cmd.findOption("--sample-444"))
            opt_sampling = ESS_444;
        if (cmd.findOption("--sample-422"))
            opt_sampling = ESS_422;
        if (cmd.findOption("--sample-411"))
            opt_sampling = ESS_411;
        cmd.endOptionBlock();
#endif

        /* output options */

        cmd.beginOptionBlock();
        if (cmd.findOption("--no-output"))
            opt_suppressOutput = 1;
        if (cmd.findOption("--write-raw-pnm"))
            opt_fileType = 1;
        if (cmd.findOption("--write-8-bit-pnm"))
            opt_fileType = 2;
        if (cmd.findOption("--write-16-bit-pnm"))
            opt_fileType = 3;
        if (cmd.findOption("--write-n-bit-pnm"))
        {
            opt_fileType = 4;
            app.checkValue(cmd.getValueAndCheckMinMax(opt_fileBits, 1, 32));
        }
        if (cmd.findOption("--write-bmp"))
            opt_fileType = 5;
        if (cmd.findOption("--write-8-bit-bmp"))
            opt_fileType = 6;
        if (cmd.findOption("--write-24-bit-bmp"))
            opt_fileType = 7;
#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
        if (cmd.findOption("--write-jpeg"))
            opt_fileType = 8;
#endif
#ifdef WITH_LIBTIFF
        if (cmd.findOption("--write-tiff"))
            opt_fileType = 9;
#endif
#ifdef PASTEL_COLOR_OUTPUT
        if (cmd.findOption("--write-pastel-pnm"))
            opt_fileType = 99;
#endif
        cmd.endOptionBlock();
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFOStringStream oss;
        oss << "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        app.printWarning(tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
    }

//    SetDebugLevel(( (int)opt_debugMode ));
    if (opt_verboseMode < 1)
        DicomImageClass::setDebugLevel(0);
    else if (opt_debugMode > 0)
        DicomImageClass::setDebugLevel(DicomImageClass::getDebugLevel() | DicomImageClass::DL_DebugMessages);
    if (opt_verboseMode > 1)
    {
        DicomImageClass::setDebugLevel(DicomImageClass::getDebugLevel() | DicomImageClass::DL_Informationals);
        OUTPUT << "reading DICOM file: " << opt_ifname << endl;
    }

    // register RLE decompression codec
    DcmRLEDecoderRegistration::registerCodecs(OFFalse /*pCreateSOPInstanceUID*/, opt_debugMode);
#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
    // register JPEG decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion, EUC_default, EPC_default, opt_debugMode);
#endif

    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->loadFile(opt_ifname, opt_transferSyntax, EGL_withoutGL,
        DCM_MaxReadLength, opt_readAsDataset);

    if (cond.bad())
    {
        OFOStringStream oss;
        oss << cond.text() << ": reading file: " << opt_ifname << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        app.printError(tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
    }

    if (opt_verboseMode > 1)
        OUTPUT << "preparing pixel data." << endl;

    E_TransferSyntax xfer = dfile->getDataset()->getOriginalXfer();

    DicomImage *di = new DicomImage(dfile, xfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);
    if (di == NULL)
        app.printError("Out of memory");

    if (di->getStatus() != EIS_Normal)
        app.printError(DicomImage::getString(di->getStatus()));

    /* create & set display function */
    DiDisplayFunction *disp = NULL;
    if (!opt_displayFile.empty())
    {
        if (opt_displayFunction == 1)
            disp = new DiCIELABFunction(opt_displayFile.c_str(), deviceType);
        else
            disp = new DiGSDFunction(opt_displayFile.c_str(), deviceType);
        if (disp != NULL)
        {
            if (opt_ambientLight >= 0)
                disp->setAmbientLightValue(opt_ambientLight);
            if (opt_illumination >= 0)
                disp->setIlluminationValue(opt_illumination);
            if ((di != NULL) && (disp->isValid()))
            {
                if (opt_verboseMode > 1)
                {
                    OUTPUT << "activating "
                           << ((opt_displayFunction == 1) ? "CIELAB" : "GSDF")
                           << " display function for "
                           << ((deviceType == DiDisplayFunction::EDT_Monitor) ? "softcopy" : "hardcopy")
                           << " devices." << endl;
                }
                if (!di->setDisplayFunction(disp))
                    app.printWarning("cannot select display function");
            }
        }
    }

    if (opt_imageInfo)
    {

        /* dump image parameters */
        if (opt_verboseMode > 1)
            OUTPUT << "dumping image parameters." << endl;

        double minVal = 0.0;
        double maxVal = 0.0;
        const char *colorModel;
        char *SOPClassUID = NULL;
        char *SOPInstanceUID = NULL;
        const char *SOPClassText = NULL;
        const char *XferText = DcmXfer(xfer).getXferName();

        int minmaxValid = di->getMinMaxValues(minVal, maxVal);
        colorModel = di->getString(di->getPhotometricInterpretation());
        if (colorModel == NULL)
            colorModel = "unknown";

            getSingleValue(dfile->getDataset(), DCM_SOPClassUID, SOPClassUID);
            getSingleValue(dfile->getDataset(), DCM_SOPInstanceUID, SOPInstanceUID);

        if (SOPInstanceUID == NULL)
            SOPInstanceUID = (char *)"not present";
        if (SOPClassUID == NULL)
            SOPClassText = "not present";
        else
            SOPClassText = dcmFindNameOfUID(SOPClassUID);
        if (SOPClassText == NULL)
            SOPClassText = SOPClassUID;

        char aspectRatio[30];
        sprintf(aspectRatio, "%.2f", di->getHeightWidthRatio());

        CERR << "filename            : " << opt_ifname << endl
             << "transfer syntax     : " << XferText << endl
             << "SOP class           : " << SOPClassText << endl
             << "SOP instance UID    : " << SOPInstanceUID << endl << endl
             << "columns x rows      : " << di->getWidth() << " x " << di->getHeight() << endl
             << "bits per sample     : " << di->getDepth() << endl
             << "color model         : " << colorModel << endl;
        CERR << "pixel aspect ratio  : " << aspectRatio << endl
             << "number of frames    : " << di->getFrameCount() << endl << endl;

        /* dump VOI windows */
        unsigned long count;
        OFString explStr;
        count = di->getWindowCount();
        CERR << "VOI windows in file : " << di->getWindowCount() << endl;
        for (i = 0; i < count; i++)
        {
            CERR << " - ";
            if (di->getVoiWindowExplanation(i, explStr) == NULL)
                CERR << "<no explanation>";
            else
                CERR << explStr;
            CERR << endl;
        }

        /* dump VOI LUTs */
        count = di->getVoiLutCount();
        CERR << "VOI LUTs in file    : " << count << endl;
        for (i = 0; i < count; i++)
        {
            CERR << " - ";
            if (di->getVoiLutExplanation(i, explStr) == NULL)
                 CERR << "<no explanation>";
            else
                CERR << explStr;
            CERR << endl;
        }

        CERR << "Overlays in file    : " << di->getOverlayCount() << endl << endl;

        if (minmaxValid)
        {
          char minmaxText[30];
          sprintf(minmaxText, "%.0f", maxVal);
          CERR << "maximum pixel value : " << minmaxText << endl;
          sprintf(minmaxText, "%.0f", minVal);
          CERR << "minimum pixel value : " << minmaxText << endl;
        }
    }

    if (!opt_suppressOutput)
    {
        /* try to select frame */
        if (opt_frame != di->getFirstFrame() + 1)
        {
            OFOStringStream oss;
            oss << "cannot select frame no. " << opt_frame << ", invalid frame no." << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            app.printError(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
        }

        /* convert to grayscale if necessary */
        if ((opt_convertToGrayscale)  &&  (!di->isMonochrome()))
        {
             if (opt_verboseMode > 1)
                 OUTPUT << "converting image to grayscale." << endl;

             DicomImage *newimage = di->createMonochromeImage();
             if (newimage == NULL)
                 app.printError("Out of memory.\n");
             else if (newimage->getStatus() != EIS_Normal)
                 app.printError(DicomImage::getString(newimage->getStatus()));
             else
             {
                 delete di;
                 di = newimage;
             }
        }

        /* process overlay parameters */
        EM_Overlay overlayMode;
        switch (opt_OverlayMode)
        {
            case 2:
                overlayMode = EMO_ThresholdReplace;
                break;
            case 3:
                overlayMode = EMO_Complement;
                break;
            case 4:
                overlayMode = EMO_RegionOfInterest;
                break;
            case 1:
            default:
                overlayMode = EMO_Replace;
                break;

        }
        di->hideAllOverlays();
        for (unsigned int k = 0; k < 16; k++)
        {
            if (opt_Overlay[k])
            {
                if ((opt_Overlay[k] == 1) || (k < di->getOverlayCount()))
                {
                    if (opt_verboseMode > 1)
                        OUTPUT << "activating overlay plane " << k + 1 << endl;
                    if (opt_OverlayMode)
                    {
                        if (!di->showOverlay(k, overlayMode, opt_foregroundDensity, opt_thresholdDensity))
                        {
                            OFOStringStream oss;
                            oss << "cannot display overlay plane " << k + 1 << OFStringStream_ends;
                            OFSTRINGSTREAM_GETSTR(oss, tmpString)
                            app.printWarning(tmpString);
                            OFSTRINGSTREAM_FREESTR(tmpString)
                        }
                    } else {
                        if (!di->showOverlay(k)) /* use default values */
                        {
                            OFOStringStream oss;
                            oss << "cannot display overlay plane " << k + 1 << OFStringStream_ends;
                            OFSTRINGSTREAM_GETSTR(oss, tmpString)
                            app.printWarning(tmpString);
                            OFSTRINGSTREAM_FREESTR(tmpString)
                        }
                    }
                }
            }
        }

        /* process VOI parameters */
        switch (opt_windowType)
        {
            case 1: /* use the n-th VOI window from the image file */
                if ((opt_windowParameter < 1) || (opt_windowParameter > di->getWindowCount()))
                {
                    OFOStringStream oss;
                    oss << "cannot select VOI window no. " << opt_windowParameter << ", only "
                        << di->getWindowCount() << " window(s) in file." << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                    app.printError(tmpString);
                    OFSTRINGSTREAM_FREESTR(tmpString)
                }
                if (opt_verboseMode > 1)
                    OUTPUT << "activating VOI window " << opt_windowParameter << endl;
                if (!di->setWindow(opt_windowParameter - 1))
                {
                    OFOStringStream oss;
                    oss << "cannot select VOI window no. " << opt_windowParameter << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                    app.printWarning(tmpString);
                    OFSTRINGSTREAM_FREESTR(tmpString)
                }
                break;
            case 2: /* use the n-th VOI look up table from the image file */
                if ((opt_windowParameter < 1) || (opt_windowParameter > di->getVoiLutCount()))
                {
                    OFOStringStream oss;
                    oss << "cannot select VOI LUT no. " << opt_windowParameter << ", only "
                        << di->getVoiLutCount() << " LUT(s) in file." << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                    app.printError(tmpString);
                    OFSTRINGSTREAM_FREESTR(tmpString)
                }
                if (opt_verboseMode > 1)
                    OUTPUT << "activating VOI LUT " << opt_windowParameter << endl;
                if (!di->setVoiLut(opt_windowParameter - 1))
                {
                    OFOStringStream oss;
                    oss << "cannot select VOI LUT no. " << opt_windowParameter << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                    app.printWarning(tmpString);
                    OFSTRINGSTREAM_FREESTR(tmpString)
                }
                break;
            case 3: /* Compute VOI window using min-max algorithm */
                if (opt_verboseMode > 1)
                    OUTPUT << "activating VOI window min-max algorithm" << endl;
                if (!di->setMinMaxWindow(0))
                    app.printWarning("cannot compute min/max VOI window");
                break;
            case 4: /* Compute VOI window using Histogram algorithm, ignoring n percent */
                if (opt_verboseMode > 1)
                    OUTPUT << "activating VOI window histogram algorithm, ignoring " << opt_windowParameter << "%" << endl;
                if (!di->setHistogramWindow(((double)opt_windowParameter)/100.0))
                    app.printWarning("cannot compute histogram VOI window");
                break;
            case 5: /* Compute VOI window using center r and width s */
                if (opt_verboseMode > 1)
                    OUTPUT << "activating VOI window center=" << opt_windowCenter << ", width=" << opt_windowWidth << endl;
                if (!di->setWindow(opt_windowCenter, opt_windowWidth))
                {
                    OFOStringStream oss;
                    oss << "cannot set VOI window center=" << opt_windowCenter << " width="
                        << opt_windowWidth << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                    app.printError(tmpString);
                    OFSTRINGSTREAM_FREESTR(tmpString)
                }
                break;
            case 6: /* Compute VOI window using min-max algorithm ignoring extremes */
                if (opt_verboseMode > 1)
                    OUTPUT << "activating VOI window min-max algorithm, ignoring extreme values" << endl;
                if (!di->setMinMaxWindow(1))
                    app.printWarning("cannot compute min/max VOI window");
                break;
            case 7: /* Compute region of interest VOI window */
                if (opt_verboseMode > 1)
                    OUTPUT << "activating region of interest VOI window" << endl;
                if (!di->setRoiWindow(opt_roiLeft, opt_roiTop, opt_roiWidth, opt_roiHeight))
                    app.printWarning("cannot compute region of interest VOI window");
                break;
            default: /* no VOI windowing */
                if (di->isMonochrome())
                {
                    if (opt_verboseMode > 1)
                        OUTPUT << "disabling VOI window computation" << endl;
                    if (! di->setNoVoiTransformation())
                        app.printWarning("cannot ignore VOI window");
                }
                break;
        }

        /* process presentation LUT parameters */
        if (opt_presShape != ESP_Default)
        {
            if (opt_verboseMode > 1)
            {
                if (opt_presShape == ESP_Identity)
                    OUTPUT << "setting presentation LUT shape to IDENTITY" << endl;
                else if (opt_presShape == ESP_Inverse)
                    OUTPUT << "setting presentation LUT shape to INVERSE" << endl;
                else if (opt_presShape == ESP_LinOD)
                    OUTPUT << "setting presentation LUT shape to LIN OD" << endl;
            }
            di->setPresentationLutShape(opt_presShape);
        }

        /* change polarity */
        if (opt_changePolarity)
        {
            if (opt_verboseMode > 1)
                OUTPUT << "setting polarity to REVERSE" << endl;
            di->setPolarity(EPP_Reverse);
        }

        /* perform clipping */
        if (opt_useClip && (opt_scaleType == 0))
        {
             if (opt_verboseMode > 1)
                 OUTPUT << "clipping image to (" << opt_left << "," << opt_top << "," << opt_width << "," << opt_height << ")." << endl;
             DicomImage *newimage = di->createClippedImage(opt_left, opt_top, opt_width, opt_height);
             if (newimage==NULL)
             {
                 OFOStringStream oss;
                 oss << "clipping to (" << opt_left << "," << opt_top << "," << opt_width << ","
                     << opt_height << ") failed." << OFStringStream_ends;
                 OFSTRINGSTREAM_GETSTR(oss, tmpString)
                 app.printError(tmpString);
                 OFSTRINGSTREAM_FREESTR(tmpString)
             } else if (newimage->getStatus() != EIS_Normal)
                 app.printError(DicomImage::getString(newimage->getStatus()));
             else
             {
                 delete di;
                 di = newimage;
             }
        }

        /* perform rotation */
        if (opt_rotateDegree > 0)
        {
            if (opt_verboseMode > 1)
                OUTPUT << "rotating image by " << opt_rotateDegree << " degrees." << endl;
            di->rotateImage(opt_rotateDegree);
        }

        /* perform flipping */
        if (opt_flipType > 0)
        {
            if (opt_verboseMode > 1)
                OUTPUT << "flipping image";
            switch (opt_flipType)
            {
                case 1:
                    if (opt_verboseMode > 1)
                        OUTPUT << " horizontally." << endl;
                    di->flipImage(1, 0);
                    break;
                case 2:
                    if (opt_verboseMode > 1)
                        OUTPUT << " vertically." << endl;
                    di->flipImage(0, 1);
                    break;
                case 3:
                    if (opt_verboseMode > 1)
                        OUTPUT << " horizontally and vertically." << endl;
                    di->flipImage(1, 1);
                    break;
                default:
                    if (opt_verboseMode > 1)
                        OUTPUT << endl;
            }
        }

        /* perform scaling */
        if (opt_scaleType > 0)
        {
            DicomImage *newimage;
            if ((opt_verboseMode > 1) && opt_useClip)
                OUTPUT << "clipping image to (" << opt_left << "," << opt_top << "," << opt_width << "," << opt_height << ")." << endl;
            switch (opt_scaleType)
            {
                case 1:
                    if (opt_verboseMode > 1)
                        OUTPUT << "scaling image, X factor=" << opt_scale_factor << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_factor, 0.0,
                            (int)opt_useInterpolation, opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(opt_scale_factor, 0.0, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                case 2:
                    if (opt_verboseMode > 1)
                        OUTPUT << "scaling image, Y factor=" << opt_scale_factor << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0.0, opt_scale_factor,
                            (int)opt_useInterpolation, opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(0.0, opt_scale_factor, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                case 3:
                    if (opt_verboseMode > 1)
                        OUTPUT << "scaling image, X size=" << opt_scale_size << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_size, 0,
                            (int)opt_useInterpolation, opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(opt_scale_size, 0, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                case 4:
                    if (opt_verboseMode > 1)
                        OUTPUT << "scaling image, Y size=" << opt_scale_size << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0, opt_scale_size,
                            (int)opt_useInterpolation, opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(0, opt_scale_size, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                default:
                    if (opt_verboseMode > 1)
                        OUTPUT << "internal error: unknown scaling type" << endl;
                    newimage = NULL;
                    break;
            }
            if (newimage==NULL)
                app.printError("Out of memory.\n");
            else if (newimage->getStatus() != EIS_Normal)
                app.printError(DicomImage::getString(newimage->getStatus()));
            else
            {
                delete di;
                di = newimage;
            }
        }

        /* write selected frame(s) to file */

        int result = 0;
        FILE *ofile = NULL;
        char ofname[255];
        unsigned int fcount = (unsigned int)(((opt_frameCount > 0) && (opt_frameCount <= di->getFrameCount())) ? opt_frameCount : di->getFrameCount());
        const char *ofext = NULL;
        /* determine default file extension */
        switch (opt_fileType)
        {
          case 5:
          case 6:
          case 7:
            ofext = "bmp";
            break;
          case 8:
            ofext = "jpg";
            break;
          case 9:
            ofext = "tif";
            break;
          default:
            if (di->isMonochrome()) ofext = "pgm"; else ofext = "ppm";
            break;
        }

        if (fcount < opt_frameCount)
        {
            OFOStringStream oss;
            oss << "cannot select " << opt_frameCount << " frames, limiting to "
                << fcount << " frames" << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            app.printWarning(tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
        }

        for (unsigned int frame = 0; frame < fcount; frame++)
        {
            if (opt_ofname)
            {
                /* output to file */
                if (opt_multiFrame)
                    sprintf(ofname, "%s.%d.%s", opt_ofname, frame, ofext);
                else
                    strcpy(ofname, opt_ofname);
                if (opt_verboseMode > 1)
                     OUTPUT << "writing frame " << (opt_frame + frame) << " to " << ofname << endl;
                ofile = fopen(ofname, "wb");
                if (ofile == NULL)
                {
                    OFOStringStream oss;
                    oss << "cannot create file " << ofname << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(oss, tmpString)
                    app.printError(tmpString);
                    OFSTRINGSTREAM_FREESTR(tmpString)
                }
            } else {
                /* output to stdout */
                ofile = stdout;
                if (opt_verboseMode > 1)
                     OUTPUT << "writing frame " << (opt_frame + frame) << " to stdout" << endl;
            }

            /* finally create PPM/PGM or BMP file */

            switch (opt_fileType)
            {
                case 1:
                    result = di->writeRawPPM(ofile, 8, frame);
                case 2:
                    result = di->writePPM(ofile, 8, frame);
                    break;
                case 3:
                    result = di->writePPM(ofile, 16, frame);
                    break;
                case 4:
                    result = di->writePPM(ofile, (int)opt_fileBits, frame);
                    break;
                case 5:
                    result = di->writeBMP(ofile, 0, frame);
                    break;
                case 6:
                    result = di->writeBMP(ofile, 8, frame);
                    break;
                case 7:
                    result = di->writeBMP(ofile, 24, frame);
                    break;
#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
                case 8:
                    {
                        /* initialize JPEG plugin */
                        DiJPEGPlugin plugin;
                        plugin.setQuality((unsigned int) opt_quality);
                        plugin.setSampling(opt_sampling);
                        result = di->writePluginFormat(&plugin, ofile, frame);
                    }
                    break;
#endif
#ifdef WITH_LIBTIFF
                case 9:
                    {
                        /* initialize TIFF plugin */
                        DiTIFFPlugin tiffPlugin;
                        tiffPlugin.setCompressionType(opt_tiffCompression);
                        tiffPlugin.setLZWPredictor(opt_lzwPredictor);
                        tiffPlugin.setRowsPerStrip((unsigned long)opt_rowsPerStrip);
                        result = di->writePluginFormat(&tiffPlugin, ofile, frame);
                    }
                    break;
#endif
#ifdef PASTEL_COLOR_OUTPUT
                case 99:
                    result = di->writePPM(ofile, MI_PastelColor, frame);
                    break;
#endif
                default:
                    if (opt_ofname)
                        result = di->writeRawPPM(ofile, 8, frame);
                    else /* stdout */
                        result = di->writePPM(ofile, 8, frame);
                    break;
            }

            if (opt_ofname)
                fclose(ofile);

            if (!result)
                app.printError("cannot write frame");
        }
    }

    /* done, now cleanup. */
    if (opt_verboseMode > 1)
         OUTPUT << "cleaning up memory." << endl;
    delete di;
    delete disp;

    // deregister RLE decompression codec
    DcmRLEDecoderRegistration::cleanup();
#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
    // deregister JPEG decompression codecs
    DJDecoderRegistration::cleanup();
#endif

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcm2pnm.cc,v $
 * Revision 1.66  2002-09-23 18:01:18  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.65  2002/08/20 12:20:20  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.64  2002/07/05 10:41:08  joergr
 * Added support for new printer characteristics file.
 *
 * Revision 1.63  2002/06/26 16:33:11  joergr
 * Added new methods to get the explanation string of stored VOI windows and
 * LUTs (not only of the currently selected VOI transformation).
 * Added configuration flag that enables the DicomImage class to take the
 * responsibility of an external DICOM dataset (i.e. delete it on destruction).
 * Added support for RLE decompression.
 *
 * Revision 1.62  2002/05/24 10:00:14  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.61  2002/05/02 14:09:19  joergr
 * Added support for standard and non-standard string streams (which one is
 * supported is detected automatically via the configure mechanism).
 * Thanks again to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 * Revision 1.60  2002/04/16 13:54:28  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 * Revision 1.59  2002/04/11 12:44:47  joergr
 * Use the new loadFile() and saveFile() routines from the dcmdata library.
 *
 * Revision 1.58  2001/12/20 10:41:28  meichel
 * Fixed warnings reported by Sun CC 2.0.1
 *
 * Revision 1.57  2001/12/06 14:08:55  joergr
 * Changed description of new command line option "--write-tiff".
 *
 * Revision 1.56  2001/12/06 10:10:56  meichel
 * Removed references to tiffconf.h which does not exist on all installations
 *
 * Revision 1.55  2001/11/30 16:47:53  meichel
 * Added TIFF export option to dcm2pnm and dcmj2pnm
 *
 * Revision 1.54  2001/11/29 16:54:09  joergr
 * Added output of transfer syntax to "--image-info" option. Set default
 * quality for JPEG compression to 90% (now consistent with other dcmtk tools).
 *
 * Revision 1.53  2001/11/27 18:23:08  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 * Revision 1.52  2001/11/19 17:52:18  joergr
 * Added support for new 'verbose mode' in module dcmjpeg.
 *
 * Revision 1.51  2001/11/19 13:04:54  joergr
 * Moved dcmmkdir tool to dcmjpeg module.
 *
 * Revision 1.50  2001/11/09 16:35:20  joergr
 * Renamed some of the getValue/getParam methods to avoid ambiguities reported
 * by certain compilers.
 * Added support for Windows BMP file format.
 * Added support for the direct output of the converted PNM/PGM/BMP image to
 * the 'stdout' stream (not yet tested under Windows).
 *
 * Revision 1.49  2001/09/28 13:52:40  joergr
 * Added method setRoiWindow() which automatically calculates a min-max VOI
 * window for a specified rectangular region of the image.
 *
 * Revision 1.48  2001/09/26 16:08:44  meichel
 * Adapted dcmimage to class OFCondition
 *
 * Revision 1.47  2001/06/20 15:11:09  joergr
 * Enhanced multi-frame support for command line tool 'dcm2pnm': extract all
 * or a range of frames with one call.
 * Removed old dcmimage license information.
 *
 * Revision 1.46  2001/06/01 15:49:25  meichel
 * Updated copyright header
 *
 * Revision 1.45  2000/07/07 14:17:23  joergr
 * Added support for LIN OD presentation LUT shape.
 *
 * Revision 1.44  2000/06/07 14:46:08  joergr
 * Added new command line option to change the polarity.
 *
 * Revision 1.43  2000/04/28 12:35:59  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.42  2000/04/27 13:18:48  joergr
 * Adapted output method to new behaviour of dcmimgle library supporting the
 * specification of a start frame and the number of frames to be converted.
 *
 * Revision 1.41  2000/04/14 16:31:49  meichel
 * Adapted to changed parameter list for command line class
 *
 * Revision 1.40  2000/03/08 16:21:47  meichel
 * Updated copyright header.
 *
 * Revision 1.39  2000/03/07 15:29:39  joergr
 * Added type cast to make Sun CC 2.0.1 happy.
 *
 * Revision 1.38  2000/03/06 18:26:15  joergr
 * Replaced #ifdef statements (reported an error by Cygwin).
 *
 * Revision 1.37  2000/03/03 14:07:49  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.36  2000/02/02 11:00:58  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.35  1999/10/08 15:46:39  joergr
 * Corrected typo. Handled 'quiet' mode more restrictive.
 *
 * Revision 1.34  1999/09/17 13:40:54  joergr
 * Corrected typos and formatting.
 *
 * Revision 1.33  1999/09/13 17:28:30  joergr
 * Changed (almost) all output commands from C to C++ style (using string
 * streams). Advantage: C++ output routines are type safe and using the
 * same output streams (in this case 'CERR').
 * Introduced (more or less) consistent format for output messages.
 * Enhanced quiet mode (also warning and error messages are suppressed).
 * Corrected some typos in usage output / changed names of some options.
 *
 * Revision 1.32  1999/09/10 14:16:22  joergr
 * Added check whether display filename is specified or not to avoid
 * unnecessary warning messages.
 *
 * Revision 1.31  1999/09/10 09:41:17  joergr
 * Added support for CIELAB display function.
 *
 * Revision 1.30  1999/08/25 16:59:15  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.29  1999/07/23 13:14:44  joergr
 * Added new interpolation algorithm for scaling.
 * Added command line option '--quiet'.
 * Added support for frame selection.
 * Added support for 2..32 bit pnm/pgm export
 *
 * Revision 1.28  1999/05/31 12:59:07  joergr
 * Modified some command line options.
 *
 * Revision 1.27  1999/05/10 09:32:30  joergr
 * Moved dcm2pnm version definition from module dcmimgle to dcmimage.
 *
 * Revision 1.26  1999/04/28 14:37:25  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be display an a consumer monitor (use pastel colors).
 * Modified some command line options (not yet finished).
 * Adapted console application to new OFCommandline and OFConsoleApplication
 * features.
 *
 * Revision 1.25  1999/03/24 17:11:06  joergr
 * Changed optional integer parameter in method findOption to enum type.
 * Removed debug code.
 *
 * Revision 1.24  1999/02/11 15:33:10  joergr
 * Added testing routine for new isOutputValueUnused() method.
 *
 * Revision 1.23  1999/02/08 13:12:57  joergr
 * Moved output/checking functionality to new OFConsoleApplication class.
 *
 * Revision 1.22  1999/02/03 16:46:48  joergr
 * Added new option to select a display file (for calibration).
 *
 * Revision 1.21  1999/01/20 14:34:25  joergr
 * Added debug code to measure time of some routines.
 * Changed default value for compatibility flag.
 *
 * Revision 1.20  1998/12/22 13:16:27  joergr
 * Corrected spelling of option used for scaling without interpolation.
 * Use presentation LUT shape only when set explicitly.
 *
 * Revision 1.19  1998/12/16 16:06:12  joergr
 * Added (debug) code to test new explanation strings and export of overlay
 * planes.
 *
 * Revision 1.18  1998/12/14 17:05:02  joergr
 * Added support for presentation shapes.
 * Changed behaviour of debug and verbose mode.
 *
 * Revision 1.17  1998/11/30 15:40:51  joergr
 * Inserted newlines in the description of command line arguments to avoid
 * ugly line breaks.
 *
 * Revision 1.16  1998/11/27 13:27:32  joergr
 * Splitted module dcmimage into two parts.
 * Added registration class to allow easy combination of both modules
 * (for monochrome and color images).
 * Added support for new command line class and new dcmimage methods
 * (flipping, rotating etc.).
 *
 * Revision 1.15  1998/07/01 08:39:10  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.14  1998/06/25 12:31:43  joergr
 * Minor changes to syntax description of dcm2pnm.
 *
 * Revision 1.13  1998/06/25 08:48:15  joergr
 * Print 'maximum/minimum pixel value' (verbose mode) only for
 * monochrome images.
 * Added compatibility mode to support ACR-NEMA images and wrong
 * palette attribute tags.
 *
 * Revision 1.12  1998/05/11 15:00:04  joergr
 * Minor changes to some comments.
 *
 * Revision 1.11  1998/03/09 08:15:50  joergr
 * Made 'return' last statement in some non-void functions.
 *
 * Revision 1.10  1998/02/24 13:47:09  meichel
 * Added license info to usage string.
 *
 * Revision 1.9  1997/10/01 09:55:24  meichel
 * Introduced separate version number and date for dcmimage.
 *   OFFIS_DCMIMAGE_VERSION and OFFIS_DCMIMAGE_RELEASEDATE
 *   are declared in dcmimage.h.
 *
 * Revision 1.8  1997/09/18 08:12:58  meichel
 * Minor type conflicts (e.g. long passed as int) solved.
 *
 * Revision 1.7  1997/07/28 16:10:29  andreas
 * - Support for pixel representations (class DcmPixelData)
 *
 * Revision 1.6  1997/05/29 17:06:31  meichel
 * All dcmtk applications now contain a version string
 * which is displayed with the command line options ("usage" message)
 * and which can be queried in the binary with the "ident" command.
 *
 * Revision 1.5  1997/05/28 09:32:15  meichel
 * Changed dcm2pnm options for MinMax VOI window computation
 * to match functionality of the toolkit.
 * Default mode for overlays is now Replace for Graphic overlays
 * and ROI for ROI overlays. Updated documentation.
 *
 * Revision 1.4  1997/05/28 08:02:14  meichel
 * New method DicomImage::setWindow() allows to disable VOI windowing.
 * New method DiDocument::getVM().
 * Default mode for grayscale images changed from "setMinMaxWindow(0)"
 * to "no VOI windowing".
 * New class DiOverlayData introduced to solve a problem occuring when
 * images containing overlay planes were copied/scaled/clipped and the
 * originals deleted before the copy. Removed workaround in dcm2pnm for this bug.
 *
 * Revision 1.3  1997/05/22 13:27:14  hewett
 * Modified the test for presence of a data dictionary to use the
 * method DcmDataDictionary::isDictionaryLoaded().
 *
 * Revision 1.2  1997/05/16 08:33:02  andreas
 * - Revised handling of GroupLength elements and support of
 *   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
 *   got additional enumeration values (for a description see dctypes.h).
 *   addGroupLength and removeGroupLength methods are replaced by
 *   computeGroupLengthAndPadding. To support Padding, the parameters of
 *   element and sequence write functions changed.
 *
 * Revision 1.1  1997/05/13 13:49:42  meichel
 * Added new application dcm2pnm.
 * dcm2pnm allows to convert DICOM images to the widely used
 * PPM/PGM general purpose image format. dcm2pnm gives access to most
 * functionality offered by the dcmimage library.
 *
 *
 */
