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
 *  Update Date:      $Date: 1998-12-22 13:16:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/apps/dcm2pnm.cc,v $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcutils.h"
#include "dcdebug.h"
#include "cmdlnarg.h"
#include "dcmimage.h"
#include "dcuid.h"      /* for dcmtk version name */

#include "ofcmdln.h"

//#include "diregist.h"   /* include to use color images */

#undef  USE_LICENSE
#define LICENSE_TYPE       ""
#define LICENSE_CONTRACTOR ""
#define LICENSE_EXPIRY     ""

static char rcsid[] = "$dcmtk: dcm2pnm v"
  OFFIS_DCMIMAGE_VERSION " " OFFIS_DCMIMAGE_RELEASEDATE 
  " using dcmtk v" OFFIS_DCMTK_VERSION " $"
#ifdef USE_LICENSE
  "\n$dcmtk: " LICENSE_TYPE " for '" LICENSE_CONTRACTOR "' $\n"
  "$dcmtk: " LICENSE_EXPIRY " $"
#endif
  ;

// ********************************************


static void
printHeader()
{
    cerr << rcsid << endl << endl;
    cerr << "dcm2pnm: Convert DICOM file to PGM or PPM" << endl;
}


static void
printUsage(const OFCommandLine &cmd)
{
    OFString str;
    cmd.getOptionString(str);
    printHeader();
    cerr << "usage: dcm2pnm [options] dcmfile-in [pnmfile-out]" << endl;
    cerr << "options are:" << endl << endl;
    cerr << str << endl;
    exit(0);
}

static void
printError(const OFString &str)
{
    printHeader();
    cerr << "error: " << str << endl;
    exit(1);
}

static void
checkValue(OFCommandLine &cmd,
           const OFCommandLine::E_ValueStatus status)
{
    OFString str;
    if (status != OFCommandLine::VS_Normal)
    {
        cmd.getStatusString(status, str);
        printError(str);
    }
}

// ********************************************

int main(int argc, char *argv[])
{
    OFCommandLine cmd;
    OFString str;

    int                 opt_readAsDataset = 0;            /* default: fileformat or dataset */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */

    unsigned long       opt_compatibilityMode = 0;        /* default: no compatibility option */
    OFCmdUnsignedInt    opt_Frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_FrameCount = 1;               /* default: one frame */
    int                 opt_MultiFrame = 0;               /* default: no multiframes */
    int                 opt_ConvertToGrayscale = 0;       /* default: color or grayscale */
    int                 opt_useAspectRatio = 1;           /* default: use aspect ratio for scaling */
    int                 opt_useInterpolation = 1;         /* default: use interpolation for scaling */
    int                 opt_useClip = 0;                  /* default: don't clip */
    OFCmdUnsignedInt    opt_left=0, opt_top=0, opt_width=0, opt_height=0; /* clip region */
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

    int                 opt_Overlay[16];
    int                 opt_O_used = 0;                   /* flag for +O parameter */
    int                 opt_OverlayMode = 0;              /* default: Replace or ROI */
                        /* 1=replace, 2=threshold-replace, 3=complement, 4=ROI */

    OFCmdFloat          opt_foregroundDensity = 1.0;
    OFCmdFloat          opt_thresholdDensity  = 0.5;

    int                 opt_verboseMode = 0;              /* default: be quiet */
    int                 opt_imageInfo = 0;                /* default: no info */
    OFCmdUnsignedInt    opt_debugMode   = 0;              /* default: no debug */
    int                 opt_suppressOutput = 0;           /* default: create output */
    int                 opt_fileType = 1;                 /* default: 8-bit binary PGM/PPM */
                        /* 2=8-bit-ASCII, 3=16-bit-ASCII */  
    const char *        opt_ifname = NULL;
    const char *        opt_ofname = NULL;
    
    int i;
    for (i = 0; i < 16; i++) opt_Overlay[i] = 2;       /* default: display all overlays if present */

    SetDebugLevel(( 0 ));
    DicomImageClass::DebugLevel = DicomImageClass::DL_NoMessages;
  
    prepareCmdLineArgs(argc, argv, "dcm2pnm");
      
    cmd.addGroup("options:");
    cmd.addOption("--help", "print this help screen");

    cmd.addGroup("input options:");
     cmd.addOption("--read-as-dataset", "+D", "read as a dataset");
     cmd.addOption("--auto-read-mode",  "-D", "read as a fileformat or dataset (default)");

    cmd.addGroup("input transfer syntax: use only after +D");
     cmd.addOption("--transfer-syntax-recognition", "+t=", "use transfer syntax recognition\n(default)");
     cmd.addOption("--little-endian-implicit",      "+ti", "read with little-endian implicit\ntransfer syntax");
     cmd.addOption("--little-endian-explicit",      "+te", "read with little-endian explicit\ntransfer syntax");
     cmd.addOption("--big-endian-explicit",         "+tb", "read with big-endian explicit\ntransfer syntax");

    cmd.addGroup("compatibility options:");
     cmd.addOption("--accept-acr-nema",           "+Ma", "accept ACR-NEMA images without\nphotometric interpretation");
     cmd.addOption("--accept-incorrect-palettes", "+Mp", "accept incorrect palette attribute\ntags (0028,111x) and (0028,121x)");

    cmd.addGroup("image processing options:");
     cmd.addOption("--frame",               "+F",   1, "[n]umber : integer",
                                                       "select specified frame (default: 1)");
     cmd.addOption("--frame-range",         "+Fr",  2, "[n]umber [c]ount : integer",
                                                       "select c frames beginning with n");
     cmd.addOption("--all-frames",          "+Fa",     "select all frames");
     cmd.addOption("--grayscale",           "+G",      "convert to grayscale if necessary");
     cmd.addOption("--clip-region",         "+C",   4, "[l]eft [t]op [w]idth [h]eight : integer",
                                                       "clip image region (l, t, w, h)");
     cmd.addOption("--rotate-left",         "+Rl",     "rotate image left (-90 degrees)");
     cmd.addOption("--rotate-right",        "+Rr",     "rotate image left (+90 degrees)");
     cmd.addOption("--rotate-top-down",     "+Rtd",    "rotate image left (180 degrees)");
     cmd.addOption("--flip-horizontally",   "+Ph",     "flip image horizontally");
     cmd.addOption("--flip-vertically",     "+Pv",     "flip image vertically");
     cmd.addOption("--flip-both-axes",      "+Phv",    "flip image horizontally and vertically");
     cmd.addOption("--pixel-aspect-ratio",  "+a",      "recognize pixel aspect ratio when scaling\n(default)");
     cmd.addOption("--ignore-pixel-aspect", "-a",      "ignore pixel aspect ratio when scaling");
     cmd.addOption("--interpolate",         "+i",      "use bilinear interpolation when scaling\n(default)");
     cmd.addOption("--no-interpolation",    "-i",      "no interpolation when scaling");
     cmd.addOption("--no-scaling",          "-S",      "no scaling, ignore pixel aspect ratio\n(default)");
     cmd.addOption("--scale-x-factor",      "+Sxf", 1, "[f]actor : float",
                                                       "scale x axis by factor, compute y axis\nautomatically");
     cmd.addOption("--scale-y-factor",      "+Syf", 1, "[f]actor : float",
                                                       "scale y axis by factor, compute x axis\nautomatically");
     cmd.addOption("--scale-x-size",        "+Sxv", 1, "[n]umber : integer",
                                                       "scale x axis to n pixels, compute y axis\nautomatically");
     cmd.addOption("--scale-y-size",        "+Syv", 1, "[n]umber : integer",
                                                       "scale y axis to n pixels, compute x axis\nautomatically");

    cmd.addGroup("VOI windowing options:");
     cmd.addOption("--no-windowing",     "-W",     "no VOI windowing (default)");
     cmd.addOption("--use-window",       "+Wi", 1, "[n]umber : integer",
                                                   "use the n-th VOI window from the image file");
     cmd.addOption("--use-voi-lut",      "+Wl", 1, "[n]umber : integer",
                                                   "use the n-th VOI look up table from the image file");
     cmd.addOption("--min-max-window",   "+Wm",    "Compute VOI window using min-max algorithm");
     cmd.addOption("--min-max-window-n", "+Wn",    "Compute VOI window using min-max algorithm,\nignoring extreme values");
     cmd.addOption("--histogram-window", "+Wh", 1, "[n]umber: integer",
                                                   "Compute VOI window using Histogram algorithm,\nignoring n percent");
     cmd.addOption("--set-window",       "+Ww", 2, "[c]enter [w]idth : float",
                                                   "Compute VOI window using center c and width w");

    cmd.addGroup("presentation LUT transformation options:");
     cmd.addOption("--identity-shape",             "Presentation LUT shape IDENTITY (default)");
     cmd.addOption("--inverse-shape",              "Presentation LUT shape INVERSE");

    cmd.addGroup("overlay options:");
     cmd.addOption("--no-overlays",               "-O",     "do not display overlays");
     cmd.addOption("--display-overlay",           "+O" , 1, "[n]umber : integer",
                                                            "display overlay n\n(0..16, 0=all, default: +O 0)");
     cmd.addOption("--overlay-replace",           "+mr",    "use overlay mode \"Replace\"\n(default for Graphic overlays)");
     cmd.addOption("--overlay-threshold-replace", "+mt",    "use overlay mode \"Threshold-Replace\"");
     cmd.addOption("--overlay-complement",        "+mc",    "use overlay mode \"Complement\"");
     cmd.addOption("--overlay-roi",               "+mi",    "use overlay mode \"Region of Interest\"\n(default for ROI overlays)");
     cmd.addOption("--overlay-foreground",        "+of", 1, "[d]ensity : float",
                                                            "set overlay foreground density to d\n(0..1, default: 1)\n"
                                                            "must be used together with +mr, +mt,\n+mc or +mi.");
     cmd.addOption("--overlay-threshold",         "+ot", 1, "[d]ensity : float",
                                                            "set overlay threshold density to d\n(0..1, default: 0.5)\n"
                                                            "must be used together with +mr, +mt,\n+mc or +mi.");

    cmd.addGroup("output options:");
     cmd.addOption("--verbose",          "+V",    "verbose mode, print processing details");
     cmd.addOption("--image-info",       "+I",    "info mode, print image details");
     cmd.addOption("--debug-level",      "+d", 1, "[n]umber : integer",
                                                  "set debug level to n (0..9, default: 0)");
     cmd.addOption("--no-output",        "-f",    "do not create any output (useful with +V)");
     cmd.addOption("--write-raw-pnm",    "+fb",   "write 8-bit binary PGM/PPM (default)");
     cmd.addOption("--write-8-bit-pnm",  "+fa",   "write 8-bit ASCII PGM/PPM");
     cmd.addOption("--write-16-bit-pnm", "+fA",   "write 16-bit ASCII PGM/PPM");

    switch (cmd.parseLine(argc, argv))    
    {
        case OFCommandLine::PS_NoArguments:
            printUsage(cmd);
            break;
        case OFCommandLine::PS_UnknownOption:
            cmd.getStatusString(OFCommandLine::PS_UnknownOption, str);
            printError(str);
            break;
        case OFCommandLine::PS_MissingValue:
            cmd.getStatusString(OFCommandLine::PS_MissingValue, str);
            printError(str);
            break;
        case OFCommandLine::PS_Normal:
            if ((cmd.getArgCount() == 1) && cmd.findOption("--help"))
                printUsage(cmd);
            else if (cmd.getParamCount() == 0)
                printError("Missing input file");
            else if ((cmd.getParamCount() == 1) && (!cmd.findOption("--no-output")))
                printError("Missing output file");
            else if (cmd.getParamCount() > 2)
                printError("Too many arguments");
            else 
            {
                cmd.getParam(1, opt_ifname);
                cmd.getParam(2, opt_ofname);

                cmd.beginOptionBlock();
                if (cmd.findOption("--read-as-dataset"))
                    opt_readAsDataset = 1;
                if (cmd.findOption("--auto-read-mode"))
                    opt_readAsDataset = 0;
                cmd.endOptionBlock();

                cmd.beginOptionBlock();
                if (cmd.findOption("--transfer-syntax-recognition"))
                    opt_transferSyntax = EXS_Unknown;
                if (cmd.findOption("--little-endian-implicit"))
                    opt_transferSyntax = EXS_LittleEndianImplicit;
                if (cmd.findOption("--little-endian-explicit"))
                    opt_transferSyntax = EXS_LittleEndianExplicit;
                if (cmd.findOption("--big-endian-explicit"))
                    opt_transferSyntax = EXS_BigEndianExplicit;
                cmd.endOptionBlock();

                if (cmd.findOption("--accept-acr-nema"))
                    opt_compatibilityMode = CIF_AcrNemaCompatibility;
                if (cmd.findOption("--accept-incorrect-palettes"))
                    opt_compatibilityMode = CIF_WrongPaletteAttributeTags;

                cmd.beginOptionBlock();
                if (cmd.findOption("--frame"))
                    checkValue(cmd, cmd.getValue(opt_Frame, 1));
                if (cmd.findOption("--frame-range"))
                {
                    checkValue(cmd, cmd.getValue(opt_Frame, 1));
                    checkValue(cmd, cmd.getValue(opt_FrameCount, 1));
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
                    checkValue(cmd, cmd.getValue(opt_left));
                    checkValue(cmd, cmd.getValue(opt_top));
                    checkValue(cmd, cmd.getValue(opt_width, 1));
                    checkValue(cmd, cmd.getValue(opt_height,1));
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
                if (cmd.findOption("--pixel-aspect-ratio"))
                    opt_useAspectRatio = 1;
                if (cmd.findOption("--ignore-pixel-aspect"))
                    opt_useAspectRatio = 0;
                cmd.endOptionBlock();

                cmd.beginOptionBlock();
                if (cmd.findOption("--interpolate"))
                    opt_useInterpolation = 1;
                if (cmd.findOption("--no-interpolation"))
                    opt_useInterpolation = 0;
                cmd.endOptionBlock();

                cmd.beginOptionBlock();
                if (cmd.findOption("--no-scaling"))
                    opt_scaleType = 0;
                if (cmd.findOption("--scale-x-factor"))
                {
                    opt_scaleType = 1;
                    checkValue(cmd, cmd.getValue(opt_scale_factor, 0.0, OFFalse));
                }
                if (cmd.findOption("--scale-y-factor"))
                {
                    opt_scaleType = 2;
                    checkValue(cmd, cmd.getValue(opt_scale_factor, 0.0, OFFalse));
                }
                if (cmd.findOption("--scale-x-size"))
                {
                    opt_scaleType = 3;
                    checkValue(cmd, cmd.getValue(opt_scale_size, 1));
                }
                if (cmd.findOption("--scale-y-size"))
                {
                    opt_scaleType = 4;
                    checkValue(cmd, cmd.getValue(opt_scale_size, 1));
                }
                cmd.endOptionBlock();

                cmd.beginOptionBlock();
                if (cmd.findOption("--no-windowing"))
                    opt_windowType = 0;
                if (cmd.findOption("--use-window"))
                {
                    opt_windowType = 1;
                    checkValue(cmd, cmd.getValue(opt_windowParameter, 1));
                }
                if (cmd.findOption("--use-voi-lut"))
                {
                    opt_windowType = 2;
                    checkValue(cmd, cmd.getValue(opt_windowParameter, 1));
                }
                if (cmd.findOption("--min-max-window"))
                    opt_windowType = 3;
                if (cmd.findOption("--min-max-window-n"))
                    opt_windowType = 6;
                if (cmd.findOption("--histogram-window"))
                {
                    opt_windowType = 4;
                    checkValue(cmd, cmd.getValue(opt_windowParameter, 0, 100));
                }
                if (cmd.findOption("--set-window"))
                {
                    opt_windowType = 5;
                    checkValue(cmd, cmd.getValue(opt_windowCenter));
                    checkValue(cmd, cmd.getValue(opt_windowWidth, 0.0, OFFalse));
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
                
                cmd.beginOptionBlock();
                if (cmd.findOption("--no-overlays"))
                {
                    opt_O_used = 1;
                    for (i = 0; i < 16; i++) opt_Overlay[i] = 0;
                }
                if (cmd.findOption("--display-overlay", 0, 1))
                {
                    do {
                        unsigned long l;
                        checkValue(cmd, cmd.getValue(l, 1, 16));
// cout << "overlay: " << l << endl;
                        if (!opt_O_used)
                        {
                            for (i = 0; i < 16; i++) opt_Overlay[i] = 0; 
                            opt_O_used = 1;
                        }
                        if (l > 0)
                            opt_Overlay[l - 1]=1;
                        else
                            for (i = 0; i < 16; i++) opt_Overlay[i] = 2; 
                    } while (cmd.findOption("--display-overlay", 0, 2));
                }
                cmd.endOptionBlock();

                cmd.beginOptionBlock();
                if (cmd.findOption("--overlay-replace"))
                    opt_OverlayMode = 1;
                if (cmd.findOption("--overlay-threshold-replace"))
                    opt_OverlayMode = 2;
                if (cmd.findOption("--overlay-complement"))
                    opt_OverlayMode = 3;
                if (cmd.findOption("--overlay-roi"))
                    opt_OverlayMode = 4;
                cmd.endOptionBlock();

                if (cmd.findOption("--overlay-foreground"))
                    checkValue(cmd, cmd.getValue(opt_foregroundDensity, 0.0, 1.0));
                if (cmd.findOption("--overlay-threshold"))
                    checkValue(cmd, cmd.getValue(opt_thresholdDensity, 0.0, 1.0));

                if (cmd.findOption("--verbose"))
                    opt_verboseMode = 1;
                if (cmd.findOption("--image-info"))
                    opt_imageInfo = 1;
                if (cmd.findOption("--debug-level"))
                    checkValue(cmd, cmd.getValue(opt_debugMode, 0, 9));

                cmd.beginOptionBlock();
                if (cmd.findOption("--no-output"))
                    opt_suppressOutput = 1;
                if (cmd.findOption("--write-raw-pnm"))
                    opt_fileType = 1;
                if (cmd.findOption("--write-8-bit-pnm"))
                    opt_fileType = 2;
                if (cmd.findOption("--write-16-bit-pnm"))
                    opt_fileType = 3;
                cmd.endOptionBlock();
            }
    }


    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
        fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n", DCM_DICT_ENVIRONMENT_VARIABLE);
        
    SetDebugLevel(( (int)opt_debugMode ));
    DicomImageClass::DebugLevel = opt_debugMode;

    if (opt_verboseMode > 0)
        fprintf(stderr, "reading DICOM file: %s\n", opt_ifname);

    DcmFileStream myin(opt_ifname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal )
    {
        fprintf(stderr, "dcm2pnm: cannot open DICOM file: %s\n", opt_ifname);
        return 1;
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
        fprintf(stderr, "dcm2pnm: error: %s: reading file: %s\n", dcmErrorConditionToString(dfile->error()), opt_ifname);
        return 1;
    }
    
    if (opt_verboseMode > 0)
        fprintf(stderr, "preparing pixel data.\n");

    E_TransferSyntax xfer;
    if (opt_readAsDataset)
        xfer = ((DcmDataset *)dfile)->getOriginalXfer();
    else
        xfer = ((DcmFileFormat *)dfile)->getDataset()->getOriginalXfer();

    DicomImage *di = new DicomImage(dfile, xfer, opt_compatibilityMode); /* warning: dfile is Dataset or Fileformat! */
//    DicomImage *di = new DicomImage(dfile, xfer, opt_compatibilityMode, opt_Frame - 1, opt_FrameCount);
    if (di == NULL)
        printError("Out of memory");

    if (di->getStatus() != EIS_Normal)
        printError(DicomImage::getString(di->getStatus()));

    if (opt_imageInfo)
    {
        
        /* dump image parameters */
        if (opt_verboseMode)
            fprintf(stderr, "dumping image parameters.\n");

        double minVal=0.0;
        double maxVal=0.0;
        const char *colorModel;
        char *SOPClassUID=NULL;
        char *SOPInstanceUID=NULL;
        const char *SOPClassText=NULL;
        
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
            SOPClassText="not present";
        else
            SOPClassText=dcmFindNameOfUID(SOPClassUID);
        if (SOPClassText==NULL)
            SOPClassText=SOPClassUID;
                                
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
        fprintf(stderr,"dcm2pnm: error: cannot select frame no. %lu, only %lu frame(s) in file.\n",
            opt_Frame, di->getFrameCount());
        return 1;
    }
    
    /* convert to grayscale if necessary */
    if ((opt_ConvertToGrayscale)&&(! di->isMonochrome()))
    { 
         if (opt_verboseMode)
             fprintf(stderr, "converting image to grayscale.\n");

         DicomImage *newimage = di->createMonochromeImage();
         if (newimage==NULL)
             printError("Out of memory.\n");
         else if (newimage->getStatus() != EIS_Normal)
             printError(DicomImage::getString(newimage->getStatus()));
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
            overlayMode=EMO_ThresholdReplace;
            break;
        case 3:
            overlayMode=EMO_Complement;
            break;
        case 4:
            overlayMode=EMO_RegionOfInterest;
            break;
        case 1:
        default:
            overlayMode=EMO_Replace;
            break;
            
    }
    di->hideAllOverlays();
    for (unsigned int k=0; k<16; k++)
    {
        if (opt_Overlay[k])
        {
            if ((opt_Overlay[k]==1)||(k < di->getOverlayCount()))
            {
                if (opt_verboseMode)
                    fprintf(stderr, "activating overlay plane %u\n", k+1);
                if (opt_OverlayMode)
                {
                    if (! di->showOverlay(k, overlayMode, opt_foregroundDensity, opt_thresholdDensity))
                        fprintf(stderr,"dcm2pnm: warning: cannot display overlay plane %u\n",k+1);
                } else {
                    if (! di->showOverlay(k)) /* use default values */
                        fprintf(stderr,"dcm2pnm: warning: cannot display overlay plane %u\n",k+1);
                }
            }
        }
    }
/*
 unsigned int us[4];
 const Uint8 *buf = di->getOverlayData(0x6002, us[0], us[1], us[2], us[3]);
 if (buf != NULL)
 {
     cout << us[0] << " " << us[1] << " " << us[2] << " " << us[3] << endl;
     FILE *outfile = fopen("/home/joergr/tmp/overlay.pgm", "wb");
     if (outfile)
     {
         fprintf(outfile, "P5\n%d %d 255\n", us[0], us[1]);
         fwrite(buf, us[0], us[1], outfile);
         fclose(outfile);
     } else
         cerr << "can't create file !" << endl;
 }
*/
    /* process VOI parameters */
    switch (opt_windowType)    
    {
        case 1: /* use the n-th VOI window from the image file */
            if ((opt_windowParameter<1)||(opt_windowParameter>di->getWindowCount()))
            {
                fprintf(stderr,"dcm2pnm: error: cannot select VOI window no. %lu, only %lu window(s) in file.\n",
                    opt_windowParameter, di->getWindowCount());
                return 1;
            }
            if (opt_verboseMode)
                fprintf(stderr, "activating VOI window %lu\n", opt_windowParameter);
            if (! di->setWindow(opt_windowParameter-1))
                fprintf(stderr,"dcm2pnm: warning: cannot select VOI window no. %lu\n", opt_windowParameter);
            break;
        case 2: /* use the n-th VOI look up table from the image file */
            if ((opt_windowParameter<1)||(opt_windowParameter>di->getVoiLutCount()))
            {
                fprintf(stderr,"dcm2pnm: error: cannot select VOI LUT no. %lu, only %lu LUT(s) in file.\n",
                    opt_windowParameter, di->getVoiLutCount());
                return 1;
            }
            if (opt_verboseMode)
                fprintf(stderr, "activating VOI LUT %lu\n", opt_windowParameter);
            if (! di->setVoiLut(opt_windowParameter-1))
                fprintf(stderr,"dcm2pnm: warning: cannot select VOI LUT no. %lu\n", opt_windowParameter);
            break;
        case 3: /* Compute VOI window using min-max algorithm */
            if (opt_verboseMode)
                fprintf(stderr, "activating VOI window min-max algorithm\n");
            if (! di->setMinMaxWindow(0))
                fprintf(stderr,"dcm2pnm: warning: cannot compute min/max VOI window\n");
            break;         
        case 4: /* Compute VOI window using Histogram algorithm, ignoring n percent */
            if (opt_verboseMode)
                fprintf(stderr, "activating VOI window histogram algorithm, ignoring %lu%%\n", opt_windowParameter);
            if (! di->setHistogramWindow(((double)opt_windowParameter)/100.0))
                fprintf(stderr,"dcm2pnm: warning: cannot compute histogram VOI window\n");
            break;
        case 5: /* Compute VOI window using center r and width s */
            if (opt_verboseMode)
                fprintf(stderr, "activating VOI window center=%f, width=%f\n", opt_windowCenter, opt_windowWidth);
            if (! di->setWindow(opt_windowCenter, opt_windowWidth))
                fprintf(stderr,"dcm2pnm: warning: cannot set VOI window center=%f width=%f\n",opt_windowCenter, opt_windowWidth);
            break;
        case 6: /* Compute VOI window using min-max algorithm ignoring extremes */
            if (opt_verboseMode)
                fprintf(stderr, "activating VOI window min-max algorithm, ignoring extreme values\n");
            if (! di->setMinMaxWindow(1))
                fprintf(stderr,"dcm2pnm: warning: cannot compute min/max VOI window\n");
            break;         
        default: /* no VOI windowing */
            if (di->isMonochrome())
            {
                if (opt_verboseMode)
                    fprintf(stderr, "disabling VOI window computation\n");
                if (! di->setNoVoiTransformation()) 
                    fprintf(stderr,"dcm2pnm: warning: cannot ignore VOI window\n");
            }
            break;
    }
/*
 cout << "VOI: " << di->getVoiTransformationExplanation() << endl;
*/        
    /* process presentation LUT parameters */
    if (opt_usePresShape)
    {
        if (opt_verboseMode)
        {
            switch (opt_presShape)
            {
                case ESP_Inverse:
                    fprintf(stderr, "setting presentation LUT shape to INVERSE\n");
                    break;
                default:
                    fprintf(stderr, "setting presentation LUT shape to IDENTITY\n");
            }
        }
        di->setPresentationLutShape(opt_presShape);
    }

    /* perform clipping */
    if (opt_useClip && (opt_scaleType == 0))
    { 
         if (opt_verboseMode)
             fprintf(stderr, "clipping image to (%lu,%lu,%lu,%lu).\n", opt_left, opt_top, opt_width, opt_height);
         DicomImage *newimage = di->createClippedImage(opt_left, opt_top, opt_width, opt_height);
         if (newimage==NULL)
         {
             fprintf(stderr, "dcm2pnm: error: clipping to (%lu,%lu,%lu,%lu) failed.\n", 
                 opt_left, opt_top, opt_width, opt_height);
             return 1;
         } else if (newimage->getStatus() != EIS_Normal)
             printError(DicomImage::getString(newimage->getStatus()));
         else
         {
             delete di;
             di = newimage;
         }
    }

    
    /* perform rotation */
    if (opt_rotateDegree > 0)
    {
        if (opt_verboseMode)
            fprintf(stderr, "rotating image by %i degrees.\n", opt_rotateDegree);

        di->rotateImage(opt_rotateDegree);
/*
        DicomImage *newimage = di->createRotatedImage(opt_rotateDegree);
        if (newimage != NULL)
        {
            delete di;
            di = newimage;
        }
*/
    }


    /* perform flipping */
    if (opt_flipType > 0)
    {
        if (opt_verboseMode)
            fprintf(stderr, "flipping image");
        switch (opt_flipType)
        {
            case 1:
                if (opt_verboseMode)
                    fprintf(stderr, " horizontally.\n");
                di->flipImage(1, 0);
                break;
            case 2:
                if (opt_verboseMode)
                    fprintf(stderr, " vertically.\n");
                di->flipImage(0, 1);
                break;
            case 3:
                if (opt_verboseMode)
                    fprintf(stderr, " horizontally and vertically.\n");
                di->flipImage(1, 1);
                break;
            default:
                if (opt_verboseMode)
                    fprintf(stderr, "\n");
        }
    }


    /* perform scaling */
    if (opt_scaleType > 0)
    {
        DicomImage *newimage;
        if ((opt_verboseMode) && opt_useClip)
            fprintf(stderr, "clipping image to (%lu,%lu,%lu,%lu).\n", opt_left, opt_top, opt_width, opt_height);
        switch (opt_scaleType)
        {
            case 1:
                if (opt_verboseMode)
                    fprintf(stderr, "scaling image, X factor=%f, Interpolation=%s, Aspect Ratio=%s\n",
                        opt_scale_factor, (opt_useInterpolation ? "yes" : "no"), (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_factor, 0.0,
                        opt_useInterpolation, opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(opt_scale_factor, 0.0, opt_useInterpolation, opt_useAspectRatio);
                break;
            case 2:
                if (opt_verboseMode)
                    fprintf(stderr, "scaling image, Y factor=%f, Interpolation=%s, Aspect Ratio=%s\n",
                        opt_scale_factor, (opt_useInterpolation ? "yes" : "no"), (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0.0, opt_scale_factor,
                        opt_useInterpolation, opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(0.0, opt_scale_factor, opt_useInterpolation, opt_useAspectRatio);
                break;
            case 3:
                if (opt_verboseMode)
                    fprintf(stderr, "scaling image, X size=%lu, Interpolation=%s, Aspect Ratio=%s\n",
                        opt_scale_size, (opt_useInterpolation ? "yes" : "no"), (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_size, 0,
                        opt_useInterpolation, opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(opt_scale_size, 0, opt_useInterpolation, opt_useAspectRatio);
                break;
            case 4:
                if (opt_verboseMode)
                    fprintf(stderr, "scaling image, Y size=%lu, Interpolation=%s, Aspect Ratio=%s\n",
                        opt_scale_size, (opt_useInterpolation ? "yes" : "no"), (opt_useAspectRatio ? "yes" : "no"));
                if (opt_useClip)
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0, opt_scale_size,
                        opt_useInterpolation, opt_useAspectRatio);
                else
                    newimage = di->createScaledImage(0, opt_scale_size, opt_useInterpolation, opt_useAspectRatio);
                break;
            default:
                if (opt_verboseMode)
                    fprintf(stderr, "internal error: unknown scaling type\n");
                newimage = NULL;
                break;
        }
        if (newimage==NULL)
            printError("Out of memory.\n");
        else if (newimage->getStatus() != EIS_Normal)
            printError(DicomImage::getString(newimage->getStatus()));
        else
        {
             delete di; 
             di = newimage;
        }
    }

    
    if (opt_verboseMode)
         fprintf(stderr, "writing PPM/PGM file to %s\n",((opt_ofname)? opt_ofname : "stderr"));
    FILE *ofile = stdout;
    if (opt_ofname)
    {
        ofile = fopen(opt_ofname, "wb");
        if (ofile==NULL)     
        {
            fprintf(stderr, "dcm2pnm: error: cannot create file %s\n",opt_ofname);
            return 1;
        }
    }

    /* finally create PPM/PGM file */
/*
    for (i = 0; i < (int)di->getFrameCount(); i++)
        cout << "Frame: " << i << endl;
*/
    switch (opt_fileType)
    {
        case 2:
            di->writePPM(ofile, 8);
            break;
        case 3:
            di->writePPM(ofile, 16);
            break;
        case 1:
        default:
            di->writeRawPPM(ofile, 8);
            break;
    }

    if (opt_ofname) fclose(ofile);

    /* done, now cleanup. */
    if (opt_verboseMode)
         fprintf(stderr, "cleaning up memory.\n");
    delete di;

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcm2pnm.cc,v $
 * Revision 1.20  1998-12-22 13:16:27  joergr
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
