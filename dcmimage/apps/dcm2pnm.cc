/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Update Date:      $Date: 1999-10-08 15:46:39 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/apps/dcm2pnm.cc,v $
 *  CVS/RCS Revision: $Revision: 1.35 $
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
 #include <GUSI.h>
#endif

#include "dctk.h"          /* for various dcmdata headers */
#include "dcutils.h"       /* for getSingleValue */
#include "dcdebug.h"       /* for SetDebugLevel */
#include "cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcuid.h"         /* for dcmtk version name */

#include "dcmimage.h"      /* for DicomImage */
#include "digsdfn.h"       /* for DiGSDFunction */
#include "diciefn.h"       /* for DiCIELABFunction */

#include "ofconapp.h"      /* for OFConsoleApplication */
#include "ofcmdln.h"       /* for OFCommandLine */

#include "diregist.h"      /* include to support color images */

#ifdef _WIN32
 #include <strstrea.h>     /* for ostrstream */
#else
 #include <strstream.h>    /* for ostrstream */
#endif

#undef  USE_LICENSE
#define LICENSE_TYPE       ""
#define LICENSE_CONTRACTOR ""
#define LICENSE_EXPIRY     ""

#define OFFIS_CONSOLE_APPLICATION "dcm2pnm"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
#ifdef OFFIS_DCMIMAGE_VERSION
  OFFIS_DCMIMAGE_VERSION " " OFFIS_DCMIMAGE_RELEASEDATE " using dcmtk v" OFFIS_DCMTK_VERSION " $"
#else
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $"
#endif
#ifdef USE_LICENSE
  "\n$dcmtk: " LICENSE_TYPE " for '" LICENSE_CONTRACTOR "' $\n"
  "$dcmtk: " LICENSE_EXPIRY " $"
#endif
  ;

#define SHORTCOL 4
#define LONGCOL 20

#define OUTPUT cerr


// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Convert DICOM images to PGM or PNM", rcsid, &OUTPUT);
    OFCommandLine cmd;

    char buf[1024];
    ostrstream oss(buf, 1024);

    int                 opt_readAsDataset = 0;            /* default: fileformat or dataset */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */

    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData;
                                                          /* default: pixel data may detached if no longer needed */
    OFCmdUnsignedInt    opt_Frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_FrameCount = 1;               /* default: one frame */
    int                 opt_MultiFrame = 0;               /* default: no multiframes */
    int                 opt_ConvertToGrayscale = 0;       /* default: color or grayscale */
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
                        /* 1=Wi, 2=Wl, 3=Wm, 4=Wh, 5=Ww, 6=Wn */
    OFCmdUnsignedInt    opt_windowParameter = 0;
    OFCmdFloat          opt_windowCenter=0.0, opt_windowWidth=0.0;

    int                 opt_usePresShape = 0;
    ES_PresentationLut  opt_presShape = ESP_Identity;
    OFString            opt_displayFile;
    int                 opt_displayFunction = 0;          /* default: GSDF */

    int                 opt_Overlay[16];
    int                 opt_O_used = 0;                   /* flag for +O parameter */
    int                 opt_OverlayMode = 0;              /* default: Replace or ROI */
                        /* 1=replace, 2=threshold-replace, 3=complement, 4=ROI */

    OFCmdFloat          opt_foregroundDensity = 1.0;
    OFCmdFloat          opt_thresholdDensity  = 0.5;

    int                 opt_verboseMode = 1;              /* default: be more or less quiet */
    int                 opt_imageInfo = 0;                /* default: no info */
    int                 opt_debugMode   = 0;              /* default: no debug */
    int                 opt_suppressOutput = 0;           /* default: create output */
    int                 opt_fileType = 1;                 /* default: 8-bit binary PGM/PPM */
                        /* 2=8-bit-ASCII, 3=16-bit-ASCII, 4=n-bit-ASCII, 5=pastel color */
    OFCmdUnsignedInt    opt_fileBits = 0;                 /* default: 0 */
    const char *        opt_ifname = NULL;
    const char *        opt_ofname = NULL;

    int i;
    for (i = 0; i < 16; i++)
        opt_Overlay[i] = 2;                               /* default: display all overlays if present */

    SetDebugLevel((0));
    DicomImageClass::DebugLevel = DicomImageClass::DL_Warnings | DicomImageClass::DL_Errors;

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
    cmd.addParam("dcmfile-out", "PGM/PNM output file name to be written", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                "-h",      "print this help text and exit");
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
                                                       "select c frames beginning with n");
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
      cmd.addOption("--recognize-aspect",   "+a",      "recognize pixel aspect ratio when scaling\n(default)");
      cmd.addOption("--ignore-aspect",      "-a",      "ignore pixel aspect ratio when scaling");
      cmd.addOption("--interpolate",        "+i",   1, "[n]umber of algorithm : integer",
                                                       "use interpolation when scaling (1..2, def: 1)");
      cmd.addOption("--no-interpolation",   "-i",      "no interpolation when scaling");
      cmd.addOption("--no-scaling",         "-S",      "no scaling, ignore pixel aspect ratio (def.)");
      cmd.addOption("--scale-x-factor",     "+Sxf", 1, "[f]actor : float",
                                                       "scale x axis by factor, auto-compute y axis");
      cmd.addOption("--scale-y-factor",     "+Syf", 1, "[f]actor : float",
                                                       "scale y axis by factor, auto-compute x axis");
      cmd.addOption("--scale-x-size",       "+Sxv", 1, "[n]umber : integer",
                                                       "scale x axis to n pixels, auto-compute y axis");
      cmd.addOption("--scale-y-size",       "+Syv", 1, "[n]umber : integer",
                                                       "scale y axis to n pixels, auto-compute x axis");

     cmd.addSubGroup("VOI windowing options:");
      cmd.addOption("--no-windowing",       "-W",      "no VOI windowing (default)");
      cmd.addOption("--use-window",         "+Wi",  1, "[n]umber : integer",
                                                       "use the n-th VOI window from image file");
      cmd.addOption("--use-voi-lut",        "+Wl",  1, "[n]umber : integer",
                                                       "use the n-th VOI look up table from image file");
      cmd.addOption("--min-max-window",     "+Wm",     "compute VOI window using min-max algorithm");
      cmd.addOption("--min-max-window-n",   "+Wn",     "compute VOI window using min-max algorithm,\nignoring extreme values");
      cmd.addOption("--histogram-window",   "+Wh",  1, "[n]umber: integer",
                                                       "compute VOI window using Histogram algorithm,\nignoring n percent");
      cmd.addOption("--set-window",         "+Ww",  2, "[c]enter [w]idth : float",
                                                       "compute VOI window using center c and width w");

     cmd.addSubGroup("presentation LUT transformation options:");
      cmd.addOption("--identity-shape",     "+Pid",    "presentation LUT shape IDENTITY (default)");
      cmd.addOption("--inverse-shape",      "+Piv",    "presentation LUT shape INVERSE");

     cmd.addSubGroup("overlay options:");
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

     cmd.addSubGroup("display LUT transformation options:");
      cmd.addOption("--display-file",       "+D",   1, "[f]ilename : string",
                                                       "calibrate output according to monitor\ncharacteristics defined in f");
      cmd.addOption("--gsd-function",       "+Dg",     "use GSDF for calibration (def. if +D present)");
      cmd.addOption("--cielab-function",    "+Dc",     "use CIELAB function for calibration ");

     cmd.addSubGroup("compatibility options:");
      cmd.addOption("--accept-acr-nema",    "+Ma",     "accept ACR-NEMA images without photometric\ninterpretation");
      cmd.addOption("--accept-palettes",    "+Mp",     "accept incorrect palette attribute tags\n(0028,111x) and (0028,121x)");

     cmd.addSubGroup("other transformations:");
      cmd.addOption("--grayscale",          "+G",      "convert to grayscale if necessary");
      cmd.addOption("--clip-region",        "+C",   4, "[l]eft [t]op [w]idth [h]eight : integer",
                                                       "clip image region (l, t, w, h)");

    cmd.addGroup("output options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--no-output",           "-o",      "do not create any output (useful with -im)");
     cmd.addOption("--write-raw-pnm",       "+ob",     "write 8-bit binary PGM/PPM (default)");
     cmd.addOption("--write-8-bit-pnm",     "+oa",     "write 8-bit ASCII PGM/PPM");
     cmd.addOption("--write-16-bit-pnm",    "+oA",     "write 16-bit ASCII PGM/PPM");
     cmd.addOption("--write-n-bit-pnm",     "+oN",  1, "[n]umber : integer",
                                                       "write n-bit ASCII PGM/PPM (1..32)");
#ifdef PASTEL_COLOR_OUTPUT
     cmd.addOption("--write-pastel-pnm",    "+op",     "write 8-bit binary PPM with pastel colors\n(early experimental version)");
#endif

    if (app.parseCommandLine(cmd, argc, argv))
    {
        if ((cmd.getParamCount() == 1) && (!cmd.findOption("--no-output")))
            app.printError("Missing output file");
        else
        {
            cmd.getParam(1, opt_ifname);
            cmd.getParam(2, opt_ofname);

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

            cmd.beginOptionBlock();
            if (cmd.findOption("--read-dataset"))
                opt_readAsDataset = 1;
            if (cmd.findOption("--read-file"))
                opt_readAsDataset = 0;
            cmd.endOptionBlock();

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

            if (cmd.findOption("--accept-acr-nema"))
                opt_compatibilityMode |= CIF_AcrNemaCompatibility;
            if (cmd.findOption("--accept-palettes"))
                opt_compatibilityMode |= CIF_WrongPaletteAttributeTags;

            cmd.beginOptionBlock();
            if (cmd.findOption("--frame"))
                app.checkValue(cmd.getValue(opt_Frame, 1));
            if (cmd.findOption("--frame-range"))
            {
                app.checkValue(cmd.getValue(opt_Frame, 1));
                app.checkValue(cmd.getValue(opt_FrameCount, 1));
                opt_MultiFrame = 1;
            }
            if (cmd.findOption("--all-frames"))
            {
                opt_FrameCount = 0;
                opt_MultiFrame = 1;
            }
            cmd.endOptionBlock();

            if (cmd.findOption("--grayscale"))
                opt_ConvertToGrayscale = 1;

            if (cmd.findOption("--clip-region"))
            {
                app.checkValue(cmd.getValue(opt_left));
                app.checkValue(cmd.getValue(opt_top));
                app.checkValue(cmd.getValue(opt_width, 1));
                app.checkValue(cmd.getValue(opt_height, 1));
                opt_useClip = 1;
            }

            cmd.beginOptionBlock();
            if (cmd.findOption("--rotate-left"))
                opt_rotateDegree = 270;
            if (cmd.findOption("--rotate-right"))
                opt_rotateDegree = 90;
            if (cmd.findOption("--rotate-top-down"))
                opt_rotateDegree = 180;
            cmd.endOptionBlock();

            cmd.beginOptionBlock();
            if (cmd.findOption("--flip-horizontally"))
                opt_flipType = 1;
            if (cmd.findOption("--flip-vertically"))
                opt_flipType = 2;
            if (cmd.findOption("--flip-both-axes"))
                opt_flipType = 3;
            cmd.endOptionBlock();

            cmd.beginOptionBlock();
            if (cmd.findOption("--recognize-aspect"))
                opt_useAspectRatio = 1;
            if (cmd.findOption("--ignore-aspect"))
                opt_useAspectRatio = 0;
            cmd.endOptionBlock();

            cmd.beginOptionBlock();
            if (cmd.findOption("--interpolate"))
                app.checkValue(cmd.getValue(opt_useInterpolation, 1, 2));
            if (cmd.findOption("--no-interpolation"))
                opt_useInterpolation = 0;
            cmd.endOptionBlock();

            cmd.beginOptionBlock();
            if (cmd.findOption("--no-scaling"))
                opt_scaleType = 0;
            if (cmd.findOption("--scale-x-factor"))
            {
                opt_scaleType = 1;
                app.checkValue(cmd.getValue(opt_scale_factor, 0.0, OFFalse));
            }
            if (cmd.findOption("--scale-y-factor"))
            {
                opt_scaleType = 2;
                app.checkValue(cmd.getValue(opt_scale_factor, 0.0, OFFalse));
            }
            if (cmd.findOption("--scale-x-size"))
            {
                opt_scaleType = 3;
                app.checkValue(cmd.getValue(opt_scale_size, 1));
            }
            if (cmd.findOption("--scale-y-size"))
            {
                opt_scaleType = 4;
                app.checkValue(cmd.getValue(opt_scale_size, 1));
            }
            cmd.endOptionBlock();

            cmd.beginOptionBlock();
            if (cmd.findOption("--no-windowing"))
                opt_windowType = 0;
            if (cmd.findOption("--use-window"))
            {
                opt_windowType = 1;
                app.checkValue(cmd.getValue(opt_windowParameter, 1));
            }
            if (cmd.findOption("--use-voi-lut"))
            {
                opt_windowType = 2;
                app.checkValue(cmd.getValue(opt_windowParameter, 1));
            }
            if (cmd.findOption("--min-max-window"))
                opt_windowType = 3;
            if (cmd.findOption("--min-max-window-n"))
                opt_windowType = 6;
            if (cmd.findOption("--histogram-window"))
            {
                opt_windowType = 4;
                app.checkValue(cmd.getValue(opt_windowParameter, 0, 100));
            }
            if (cmd.findOption("--set-window"))
            {
                opt_windowType = 5;
                app.checkValue(cmd.getValue(opt_windowCenter));
                app.checkValue(cmd.getValue(opt_windowWidth, 1.0));
            }
            cmd.endOptionBlock();

            cmd.beginOptionBlock();
            if (cmd.findOption("--identity-shape"))
            {
                opt_usePresShape = 1;
                opt_presShape = ESP_Identity;
            }
            if (cmd.findOption("--inverse-shape"))
            {
                opt_usePresShape = 1;
                opt_presShape = ESP_Inverse;
            }
            cmd.endOptionBlock();

            if (cmd.findOption("--display-file"))
                app.checkValue(cmd.getValue(opt_displayFile));
            cmd.beginOptionBlock();
            if (cmd.findOption("--gsd-function"))
                opt_displayFunction = 0;
            if (cmd.findOption("--cielab-function"))
                opt_displayFunction = 1;
            cmd.endOptionBlock();

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
                    app.checkValue(cmd.getValue(l, 1, 16));
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
                app.checkValue(cmd.getValue(opt_foregroundDensity, 0.0, 1.0));
            if (cmd.findOption("--set-threshold"))
                app.checkValue(cmd.getValue(opt_thresholdDensity, 0.0, 1.0));

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
                app.checkValue(cmd.getValue(opt_fileBits, 1, 32));
            }
#ifdef PASTEL_COLOR_OUTPUT
            if (cmd.findOption("--write-pastel-pnm"))
                opt_fileType = 5;
#endif
            cmd.endOptionBlock();
        }
    }


    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        oss << "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << ends;
        app.printWarning(oss.str());
    }

//    SetDebugLevel(( (int)opt_debugMode ));
    if (opt_verboseMode < 1)
        DicomImageClass::DebugLevel = 0;
    else if (opt_debugMode > 0)
        DicomImageClass::DebugLevel |= DicomImageClass::DL_DebugMessages;
    if (opt_verboseMode > 1)
    {
        DicomImageClass::DebugLevel |= DicomImageClass::DL_Informationals;
        OUTPUT << "reading DICOM file: " << opt_ifname << endl;
    }

    DcmFileStream myin(opt_ifname, DCM_ReadMode);
    if (myin.GetError() != EC_Normal)
    {
        oss << "cannot open DICOM file: " << opt_ifname << ends;
        app.printError(oss.str());
    }

    DcmObject * dfile = NULL;
    if (opt_readAsDataset)
        dfile = new DcmDataset();
    else
        dfile = new DcmFileFormat();

    dfile->transferInit();
    dfile->read(myin, opt_transferSyntax, EGL_withoutGL);
    dfile->transferEnd();

    if (dfile->error() != EC_Normal)
    {
        oss << dcmErrorConditionToString(dfile->error()) << ": reading file: " << opt_ifname << ends;
        app.printError(oss.str());
    }

    if (opt_verboseMode > 1)
        OUTPUT << "preparing pixel data." << endl;

    E_TransferSyntax xfer;
    if (opt_readAsDataset)
        xfer = ((DcmDataset *)dfile)->getOriginalXfer();
    else
        xfer = ((DcmFileFormat *)dfile)->getDataset()->getOriginalXfer();
    DicomImage *di = new DicomImage(dfile, xfer, opt_compatibilityMode); /* warning: dfile is Dataset or Fileformat! */
    if (di == NULL)
        app.printError("Out of memory");

    if (di->getStatus() != EIS_Normal)
        app.printError(DicomImage::getString(di->getStatus()));

    /* create & set display function */
    DiDisplayFunction *disp = NULL;
    if (!opt_displayFile.empty())
    {
        if (opt_displayFunction == 1)
            disp = new DiCIELABFunction(opt_displayFile.c_str());
        else
            disp = new DiGSDFunction(opt_displayFile.c_str());
        if ((di != NULL) && (disp != NULL) && (disp->isValid()))
            di->setDisplayFunction(disp);
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

        int minmaxValid = di->getMinMaxValues(minVal, maxVal);
        colorModel = di->getString(di->getPhotometricInterpretation());
        if (colorModel == NULL)
            colorModel = "unknown";

        if (opt_readAsDataset)
        {
            getSingleValue(dfile, DCM_SOPClassUID, SOPClassUID);
            getSingleValue(dfile, DCM_SOPInstanceUID, SOPInstanceUID);
        } else {
            getSingleValue(((DcmFileFormat *)dfile)->getDataset(), DCM_SOPClassUID, SOPClassUID);
            getSingleValue(((DcmFileFormat *)dfile)->getDataset(), DCM_SOPInstanceUID, SOPInstanceUID);
        }
        if (SOPInstanceUID == NULL)
            SOPInstanceUID = (char *)"not present";
        if (SOPClassUID == NULL)
            SOPClassText = "not present";
        else
            SOPClassText = dcmFindNameOfUID(SOPClassUID);
        if (SOPClassText == NULL)
            SOPClassText = SOPClassUID;

        fprintf(stderr,
            "filename            : %s\n"
            "SOP class           : %s\n"
            "SOP instance UID    : %s\n\n"
            "columns x rows      : %lu x %lu\n"
            "bits per sample     : %u\n"
            "color model         : %s\n"
            "pixel aspect ratio  : %.2f\n"
            "number of frames    : %lu\n\n"
            "VOI windows in file : %lu\n"
            "VOI LUTs in file    : %lu\n"
            "Overlays in file    : %u\n\n",
                opt_ifname,
                SOPClassText,
                SOPInstanceUID,
                di->getWidth(), di->getHeight(),
                di->getDepth(),
                colorModel,
                di->getHeightWidthRatio(),
                di->getFrameCount(),
                di->getWindowCount(),
                di->getVoiLutCount(),
                di->getOverlayCount()
        );
        if (minmaxValid)
        {
            fprintf(stderr,
                "maximum pixel value : %.0f\n"
                "minimum pixel value : %.0f\n\n",
                    maxVal,
                    minVal
            );
        }
    }

    if (opt_suppressOutput) return 0;

    /* select frame */
    if (opt_Frame > di->getFrameCount())
    {
        oss << "cannot select frame no. " << opt_Frame << ", only " << di->getFrameCount() << " frame(s) in file." << ends;
        app.printError(oss.str());
    }

    /* convert to grayscale if necessary */
    if ((opt_ConvertToGrayscale)  &&  (!di->isMonochrome()))
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
                        oss << "cannot display overlay plane " << k + 1 << ends;
                        app.printWarning(oss.str());
                    }
                } else {
                    if (!di->showOverlay(k)) /* use default values */
                    {
                        oss << "cannot display overlay plane " << k + 1 << ends;
                        app.printWarning(oss.str());
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
                oss << "cannot select VOI window no. " << opt_windowParameter << ", only " << di->getWindowCount() <<
                    " window(s) in file." << ends;
                app.printError(oss.str());
            }
            if (opt_verboseMode > 1)
                OUTPUT << "activating VOI window " << opt_windowParameter << endl;
            if (!di->setWindow(opt_windowParameter - 1))
            {
                oss << "cannot select VOI window no. " << opt_windowParameter << ends;
                app.printWarning(oss.str());
            }
            break;
        case 2: /* use the n-th VOI look up table from the image file */
            if ((opt_windowParameter < 1) || (opt_windowParameter > di->getVoiLutCount()))
            {
                oss << "cannot select VOI LUT no. " << opt_windowParameter << ", only " << di->getVoiLutCount() <<
                    " LUT(s) in file." << ends;
                app.printError(oss.str());
            }
            if (opt_verboseMode > 1)
                OUTPUT << "activating VOI LUT " << opt_windowParameter << endl;
            if (!di->setVoiLut(opt_windowParameter - 1))
            {
                oss << "cannot select VOI LUT no. " << opt_windowParameter << ends;
                app.printWarning(oss.str());
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
                oss << "cannot set VOI window center=" << opt_windowCenter << " width=" << opt_windowWidth << ends;
                app.printWarning(oss.str());
            }
            break;
        case 6: /* Compute VOI window using min-max algorithm ignoring extremes */
            if (opt_verboseMode > 1)
                OUTPUT << "activating VOI window min-max algorithm, ignoring extreme values" << endl;
            if (!di->setMinMaxWindow(1))
                app.printWarning("cannot compute min/max VOI window");
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
    if (opt_usePresShape)
    {
        if (opt_verboseMode > 1)
        {
            switch (opt_presShape)
            {
                case ESP_Inverse:
                    OUTPUT << "setting presentation LUT shape to INVERSE" << endl;
                    break;
                default:
                    OUTPUT << "setting presentation LUT shape to IDENTITY" << endl;
            }
        }
        di->setPresentationLutShape(opt_presShape);
    }

    /* perform clipping */
    if (opt_useClip && (opt_scaleType == 0))
    {
         if (opt_verboseMode > 1)
             OUTPUT << "clipping image to (" << opt_left << "," << opt_top << "," << opt_width << "," << opt_height << ")." << endl;
         DicomImage *newimage = di->createClippedImage(opt_left, opt_top, opt_width, opt_height);
         if (newimage==NULL)
         {
             oss << "clipping to (" << opt_left << "," << opt_top << "," << opt_width << "," << opt_height << ") failed." << ends;
             app.printError(oss.str());
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

    if (opt_verboseMode > 1)
         OUTPUT << "writing PPM/PGM file to " << ((opt_ofname)? opt_ofname : "stderr") << endl;
    FILE *ofile = stdout;
    if (opt_ofname)
    {
        ofile = fopen(opt_ofname, "wb");
        if (ofile==NULL)
        {
            oss << "cannot create file " << opt_ofname << ends;
            app.printError(oss.str());
        }
    }

    /* finally create PPM/PGM file */

    switch (opt_fileType)
    {
        case 2:
            di->writePPM(ofile, 8, opt_Frame - 1);
            break;
        case 3:
            di->writePPM(ofile, 16, opt_Frame - 1);
            break;
        case 4:
            di->writePPM(ofile, opt_fileBits, opt_Frame - 1);
            break;
#ifdef PASTEL_COLOR_OUTPUT
        case 5:
            di->writePPM(ofile, MI_PastelColor, opt_Frame - 1);
            break;
#endif
        case 1:
        default:
            di->writeRawPPM(ofile, 8, opt_Frame - 1);
            break;
    }

    if (opt_ofname)
        fclose(ofile);

    /* done, now cleanup. */
    if (opt_verboseMode > 1)
         OUTPUT << "cleaning up memory." << endl;
    delete di;
    delete disp;

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcm2pnm.cc,v $
 * Revision 1.35  1999-10-08 15:46:39  joergr
 * Corrected typo. Handled 'quiet' mode more restrictive.
 *
 * Revision 1.34  1999/09/17 13:40:54  joergr
 * Corrected typos and formatting.
 *
 * Revision 1.33  1999/09/13 17:28:30  joergr
 * Changed (almost) all output commands from C to C++ style (using string
 * streams). Advantage: C++ output routines are type safe and using the
 * same output streams (in this case 'cerr').
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
