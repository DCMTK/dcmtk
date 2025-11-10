/*
 *
 *  Copyright (C) 1996-2025, OFFIS e.V.
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
 *  Module:  dcmapps
 *
 *  Authors: Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose: Convert DICOM image to standard image format
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/dcuid.h"         /* for dcmtk version name */
#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */
#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */
#include "dcmtk/dcmimgle/diciefn.h"      /* for DiCIELABFunction */
#include "dcmtk/dcmimgle/digsdfn.h"      /* for DiGSDFunction */
#include "dcmtk/dcmjpeg/dipijpeg.h"      /* for dcmimage JPEG plugin */
#include "dcmtk/dcmjpeg/djdecode.h"      /* for dcmjpeg decoders */
#include "dcmtk/dcmjpls/dipijpls.h"      /* for dcmimage JPEG-LS plugin */
#include "dcmtk/dcmjpls/djdecode.h"      /* for dcmjpls decoders */
#include "dcmtk/oflog/oflog.h"           /* for the logger */
#include "dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */
#include "dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofstd.h"           /* for OFStandard */
#include "dcmtk/ofstd/ofstream.h"

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
#include "dcmtk/dcmjp2k/dipijp2k.h"      /* for dcmimage JPEG 2000 plugin */
#include "dcmtk/dcmjp2k/d2decode.h"      /* for JasPer based JPEG 2000 decoder */
#include "dcmtk/dcmjp2k/o2decode.h"      /* for OpenJPEG based JPEG 2000 decoder */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

#ifdef WITH_LIBTIFF
#include "dcmtk/dcmimage/dipitiff.h"     /* for dcmimage TIFF plugin */
#endif

#ifdef WITH_LIBPNG
#include "dcmtk/dcmimage/dipipng.h"      /* for dcmimage PNG plugin */
#endif

#ifdef WITH_ZLIB
#include <zlib.h>                        /* for zlibVersion() */
#endif

BEGIN_EXTERN_C
#include <fcntl.h>                       /*  for O_BINARY */
#ifdef HAVE_IO_H
#include <io.h>                          /* for setmode() on Windows */
#endif
END_EXTERN_C

#define OFFIS_OUTFILE_DESCRIPTION "output filename to be written (default: stdout)"

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
# define OFFIS_CONSOLE_APPLICATION "dcm2kimg"
#else /* BUILD_DCM2IMG_AS_DCM2KIMG */
# define OFFIS_CONSOLE_APPLICATION "dcm2img"
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

static OFLogger dcm2imgLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static const char *consoleDescription = "Convert DICOM image to standard image format";

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 4
#define LONGCOL 20

/* output file types */
enum E_FileType
{
    EFT_default,
    EFT_RawPNM,
    EFT_8bitPNM,
    EFT_16bitPNM,
    EFT_NbitPNM,
    EFT_BMP,
    EFT_8bitBMP,
    EFT_24bitBMP,
    EFT_32bitBMP,
    EFT_JPEG,
    EFT_TIFF,
    EFT_PNG,
    EFT_16bitPNG,
    EFT_JPLS
#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
  , EFT_JP2K_CS
#ifdef WITH_OPENJPEG
  , EFT_JP2K_FILE
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */
#ifdef PASTEL_COLOR_OUTPUT
   , EFT_PastelPNM
#endif
};


// ********************************************

static void dumpImageParameters(
  E_TransferSyntax xfer,
  DicomImage *di,
  DcmDataset *dataset,
  const char *ifname)
{
    if ((di == NULL) || (dataset == NULL) || (ifname == NULL)) return;

    /* dump image parameters */
    OFLOG_INFO(dcm2imgLogger, "dumping image parameters");

    double minVal = 0.0;
    double maxVal = 0.0;
    const char *colorModel;
    const char *SOPClassUID = NULL;
    const char *SOPInstanceUID = NULL;
    const char *SOPClassText = NULL;
    const char *XferText = DcmXfer(xfer).getXferName();

    int minmaxValid = di->getMinMaxValues(minVal, maxVal);
    colorModel = di->getString(di->getPhotometricInterpretation());
    if (colorModel == NULL)
        colorModel = "unknown";

    dataset->findAndGetString(DCM_SOPClassUID, SOPClassUID);
    dataset->findAndGetString(DCM_SOPInstanceUID, SOPInstanceUID);

    if (SOPInstanceUID == NULL)
        SOPInstanceUID = "not present";
    if (SOPClassUID == NULL)
        SOPClassText = "not present";
    else
        SOPClassText = dcmFindNameOfUID(SOPClassUID);
    if (SOPClassText == NULL)
        SOPClassText = SOPClassUID;

    char aspectRatio[30];
    OFStandard::ftoa(aspectRatio, sizeof(aspectRatio), di->getHeightWidthRatio(), OFStandard::ftoa_format_f, 0, 2);

    /* dump some general information */
    OFLOG_INFO(dcm2imgLogger, "  filename            : " << ifname << OFendl
        << "  transfer syntax     : " << XferText << OFendl
        << "  SOP class           : " << SOPClassText << OFendl
        << "  SOP instance UID    : " << SOPInstanceUID << OFendl
        << "  columns x rows      : " << di->getWidth() << " x " << di->getHeight() << OFendl
        << "  bits per sample     : " << di->getDepth() << OFendl
        << "  color model         : " << colorModel << OFendl
        << "  pixel aspect ratio  : " << aspectRatio << OFendl
        << "  number of frames    : " << di->getNumberOfFrames() << " (" << di->getFrameCount() << " processed)");
    if (di->getFrameTime() > 0)
        OFLOG_INFO(dcm2imgLogger, "  frame time (in ms)  : " << di->getFrameTime());

    /* dump VOI windows */
    unsigned long count;
    OFString explStr, funcStr;
    count = di->getWindowCount();
    switch (di->getVoiLutFunction())
    {
        case EFV_Default:
            funcStr = "<default>";
            break;
        case EFV_Linear:
            funcStr = "LINEAR";
            break;
        case EFV_Sigmoid:
            funcStr = "SIGMOID";
            break;
    }
    OFLOG_INFO(dcm2imgLogger, "  VOI LUT function    : " << funcStr);
    OFLOG_INFO(dcm2imgLogger, "  VOI windows in file : " << di->getWindowCount());
    unsigned long i;
    for (i = 0; i < count; i++)
    {
        if (di->getVoiWindowExplanation(i, explStr) == NULL)
            OFLOG_INFO(dcm2imgLogger, "  - <no explanation>");
        else
            OFLOG_INFO(dcm2imgLogger, "  - " << explStr);
    }

    /* dump VOI LUTs */
    count = di->getVoiLutCount();
    OFLOG_INFO(dcm2imgLogger, "  VOI LUTs in file    : " << count);
    for (i = 0; i < count; i++)
    {
        if (di->getVoiLutExplanation(i, explStr) == NULL)
            OFLOG_INFO(dcm2imgLogger, "  - <no explanation>");
        else
            OFLOG_INFO(dcm2imgLogger, "  - " << explStr);
    }

    /* dump presentation LUT shape */
    OFString shapeStr;
    switch (di->getPresentationLutShape())
    {
        case ESP_Default:
            shapeStr = "<default>";
            break;
        case ESP_Identity:
            shapeStr = "IDENTITY";
            break;
        case ESP_Inverse:
            shapeStr = "INVERSE";
            break;
        case ESP_LinOD:
            shapeStr = "LIN OD";
            break;
    }
    OFLOG_INFO(dcm2imgLogger, "  presentation shape  : " << shapeStr);

    /* dump overlays */
    OFLOG_INFO(dcm2imgLogger, "  overlays in file    : " << di->getOverlayCount());

    if (minmaxValid)
    {
      char minmaxText[30];
      OFStandard::ftoa(minmaxText, sizeof(minmaxText), maxVal, OFStandard::ftoa_format_f, 0, 0);
      OFLOG_INFO(dcm2imgLogger, "  maximum pixel value : " << minmaxText);
      OFStandard::ftoa(minmaxText, sizeof(minmaxText), minVal, OFStandard::ftoa_format_f, 0, 0);
      OFLOG_INFO(dcm2imgLogger, "  minimum pixel value : " << minmaxText);
    }
}

static DicomImage *convertToGrayscale(DicomImage *di)
{
    if (di == NULL) return NULL;
    OFLOG_INFO(dcm2imgLogger, "converting image to grayscale");
    DicomImage *newimage = di->createMonochromeImage();
    if (newimage == NULL)
    {
        OFLOG_FATAL(dcm2imgLogger, "Out of memory or cannot convert to monochrome image");
        return NULL;
    }
    else if (newimage->getStatus() != EIS_Normal)
    {
        OFLOG_FATAL(dcm2imgLogger, DicomImage::getString(newimage->getStatus()));
        return NULL;
    }
    else
    {
        delete di;
        return newimage;
    }
}

static void processOverlayParameters(
  DicomImage *di,
  int *overlayArray,
  EM_Overlay overlayMode,
  OFCmdFloat foregroundDensity,
  OFCmdFloat thresholdDensity)
{
    if ((di == NULL)||(overlayArray == NULL)) return;

    di->hideAllOverlays();
    for (unsigned int k = 0; k < 16; k++)
    {
        if (overlayArray[k])
        {
            if ((overlayArray[k] == 1) || (k < di->getOverlayCount()))
            {
                OFLOG_INFO(dcm2imgLogger, "activating overlay plane " << k + 1);
                if (overlayMode != EMO_Default)
                {
                    if (!di->showOverlay(k, overlayMode, foregroundDensity, thresholdDensity))
                        OFLOG_WARN(dcm2imgLogger, "cannot display overlay plane " << k + 1);
                } else {
                    if (!di->showOverlay(k)) /* use default values */
                        OFLOG_WARN(dcm2imgLogger, "cannot display overlay plane " << k + 1);
                }
            }
        }
    }
}


static int processVOIParameters(
  DicomImage *di,
  int windowType,
  OFCmdUnsignedInt windowParameter,
  OFCmdFloat windowCenter,
  OFCmdFloat windowWidth,
  EF_VoiLutFunction voiFunction,
  OFBool ignoreVoiLutDepth,
  OFCmdUnsignedInt roiLeft,
  OFCmdUnsignedInt roiTop,
  OFCmdUnsignedInt roiWidth,
  OFCmdUnsignedInt roiHeight)
{
    if (di == NULL) return 1;

    switch (windowType)
    {
        case 1: /* use the n-th VOI window from the image file */
            if ((windowParameter < 1) || (windowParameter > di->getWindowCount()))
            {
                OFLOG_FATAL(dcm2imgLogger, "cannot select VOI window " << windowParameter << ", only "
                    << di->getWindowCount() << " window(s) in file");
                return 1;
            }
            OFLOG_INFO(dcm2imgLogger, "activating VOI window " << windowParameter);
            if (!di->setWindow(windowParameter - 1))
                OFLOG_WARN(dcm2imgLogger, "cannot select VOI window " << windowParameter);
            break;
        case 2: /* use the n-th VOI look up table from the image file */
            if ((windowParameter < 1) || (windowParameter > di->getVoiLutCount()))
            {
                OFLOG_FATAL(dcm2imgLogger, "cannot select VOI LUT " << windowParameter << ", only "
                    << di->getVoiLutCount() << " LUT(s) in file");
                return 1;
            }
            OFLOG_INFO(dcm2imgLogger, "activating VOI LUT " << windowParameter);
            if (!di->setVoiLut(windowParameter - 1, ignoreVoiLutDepth ? ELM_IgnoreValue : ELM_UseValue))
                OFLOG_WARN(dcm2imgLogger, "cannot select VOI LUT " << windowParameter);
            break;
        case 3: /* Compute VOI window using min-max algorithm */
            OFLOG_INFO(dcm2imgLogger, "activating VOI window min-max algorithm");
            if (!di->setMinMaxWindow(0))
                OFLOG_WARN(dcm2imgLogger, "cannot compute min/max VOI window");
            break;
        case 4: /* Compute VOI window using Histogram algorithm, ignoring n percent */
            OFLOG_INFO(dcm2imgLogger, "activating VOI window histogram algorithm, ignoring " << windowParameter << "%");
            if (!di->setHistogramWindow(OFstatic_cast(double, windowParameter)/100.0))
                OFLOG_WARN(dcm2imgLogger, "cannot compute histogram VOI window");
            break;
        case 5: /* Compute VOI window using center and width */
            OFLOG_INFO(dcm2imgLogger, "activating VOI window center=" << windowCenter << ", width=" << windowWidth);
            if (!di->setWindow(windowCenter, windowWidth))
                OFLOG_WARN(dcm2imgLogger, "cannot set VOI window to specified values");
            break;
        case 6: /* Compute VOI window using min-max algorithm ignoring extremes */
            OFLOG_INFO(dcm2imgLogger, "activating VOI window min-max algorithm, ignoring extreme values");
            if (!di->setMinMaxWindow(1))
                OFLOG_WARN(dcm2imgLogger, "cannot compute min/max VOI window");
            break;
        case 7: /* Compute region of interest VOI window */
            OFLOG_INFO(dcm2imgLogger, "activating region of interest VOI window");
            if (!di->setRoiWindow(roiLeft, roiTop, roiWidth, roiHeight))
                OFLOG_WARN(dcm2imgLogger, "cannot compute region of interest VOI window");
            break;
        default: /* no VOI windowing */
            if (di->isMonochrome())
            {
                OFLOG_INFO(dcm2imgLogger, "disabling VOI window computation");
                if (!di->setNoVoiTransformation())
                    OFLOG_WARN(dcm2imgLogger, "cannot ignore VOI window");
            }
            break;
    }
    /* VOI LUT function */
    if (voiFunction != EFV_Default)
    {
        if (voiFunction == EFV_Linear)
            OFLOG_INFO(dcm2imgLogger, "setting VOI LUT function to LINEAR");
        else if (voiFunction == EFV_Sigmoid)
            OFLOG_INFO(dcm2imgLogger, "setting VOI LUT function to SIGMOID");
        if (!di->setVoiLutFunction(voiFunction))
            OFLOG_WARN(dcm2imgLogger, "cannot set VOI LUT function");
    }
    return 0;
}


static void processPLUTParameters(
  DicomImage *di,
  ES_PresentationLut presShape)
{
    if (di == NULL) return;
    if (presShape != ESP_Default)
    {
        if (presShape == ESP_Identity)
            OFLOG_INFO(dcm2imgLogger, "setting presentation LUT shape to IDENTITY");
        else if (presShape == ESP_Inverse)
            OFLOG_INFO(dcm2imgLogger, "setting presentation LUT shape to INVERSE");
        else if (presShape == ESP_LinOD)
            OFLOG_INFO(dcm2imgLogger, "setting presentation LUT shape to LIN OD");
        if (!di->setPresentationLutShape(presShape))
            OFLOG_WARN(dcm2imgLogger, "cannot set presentation LUT shape");
    }
}

static DicomImage *performClipping(
  DicomImage *di,
  int useClip,
  int scaleType,
  OFCmdSignedInt left,
  OFCmdSignedInt top,
  OFCmdUnsignedInt width,
  OFCmdUnsignedInt height)
{
    if (di == NULL) return NULL;

    if (useClip && (scaleType == 0))
    {
         OFLOG_INFO(dcm2imgLogger, "clipping image to (" << left << "," << top << "," << width
             << "," << height << ")");
         DicomImage *newimage = di->createClippedImage(left, top, width, height);
         if (newimage == NULL)
         {
             OFLOG_FATAL(dcm2imgLogger, "clipping to (" << left << "," << top << "," << width
                 << "," << height << ") failed");
             return NULL;
         } else if (newimage->getStatus() != EIS_Normal)
         {
             OFLOG_FATAL(dcm2imgLogger, DicomImage::getString(newimage->getStatus()));
             return NULL;
         }
         else
         {
             delete di;
             return newimage;
         }
    }
    return di;
}

static void performRotation(
  DicomImage *di,
  int rotateDegree)
{
    if (di == NULL) return;
    if (rotateDegree > 0)
    {
        OFLOG_INFO(dcm2imgLogger, "rotating image by " << rotateDegree << " degrees");
        if (!di->rotateImage(rotateDegree))
            OFLOG_WARN(dcm2imgLogger, "cannot rotate image");
    }
}

static void performFlip(
  DicomImage *di,
  int flipType)
{
    if (di == NULL) return;
    if (flipType > 0)
    {
        switch (flipType)
        {
            case 1:
                OFLOG_INFO(dcm2imgLogger, "flipping image horizontally");
                if (!di->flipImage(1, 0))
                    OFLOG_WARN(dcm2imgLogger, "cannot flip image");
                break;
            case 2:
                OFLOG_INFO(dcm2imgLogger, "flipping image vertically");
                if (!di->flipImage(0, 1))
                    OFLOG_WARN(dcm2imgLogger, "cannot flip image");
                break;
            case 3:
                OFLOG_INFO(dcm2imgLogger, "flipping image horizontally and vertically");
                if (!di->flipImage(1, 1))
                    OFLOG_WARN(dcm2imgLogger, "cannot flip image");
                break;
            default:
                break;
        }
    }
}

static DicomImage *performScaling(
  DicomImage *di,
  int useClip,
  int scaleType,
  OFCmdSignedInt left,
  OFCmdSignedInt top,
  OFCmdUnsignedInt width,
  OFCmdUnsignedInt height,
  OFCmdFloat scale_factor,
  OFCmdUnsignedInt scale_size,
  OFCmdUnsignedInt useInterpolation,
  int useAspectRatio)
{
    if (scaleType > 0)
    {
        DicomImage *newimage;
        if (useClip)
            OFLOG_INFO(dcm2imgLogger, "clipping image to (" << left << "," << top << "," << width << "," << height << ")");
        switch (scaleType)
        {
            case 1:
                OFLOG_INFO(dcm2imgLogger, "scaling image, X factor=" << scale_factor
                    << ", Interpolation=" << OFstatic_cast(int, useInterpolation)
                    << ", Aspect Ratio=" << (useAspectRatio ? "yes" : "no"));
                if (useClip)
                    newimage = di->createScaledImage(left, top, width, height, scale_factor, 0.0,
                        OFstatic_cast(int, useInterpolation), useAspectRatio);
                else
                    newimage = di->createScaledImage(scale_factor, 0.0, OFstatic_cast(int, useInterpolation),
                        useAspectRatio);
                break;
            case 2:
                OFLOG_INFO(dcm2imgLogger, "scaling image, Y factor=" << scale_factor
                    << ", Interpolation=" << OFstatic_cast(int, useInterpolation)
                    << ", Aspect Ratio=" << (useAspectRatio ? "yes" : "no"));
                if (useClip)
                    newimage = di->createScaledImage(left, top, width, height, 0.0, scale_factor,
                        OFstatic_cast(int, useInterpolation), useAspectRatio);
                else
                    newimage = di->createScaledImage(0.0, scale_factor, OFstatic_cast(int, useInterpolation),
                        useAspectRatio);
                break;
            case 3:
                OFLOG_INFO(dcm2imgLogger, "scaling image, X size=" << scale_size
                    << ", Interpolation=" << OFstatic_cast(int, useInterpolation)
                    << ", Aspect Ratio=" << (useAspectRatio ? "yes" : "no"));
                if (useClip)
                    newimage = di->createScaledImage(left, top, width, height, scale_size, 0,
                        OFstatic_cast(int, useInterpolation), useAspectRatio);
                else
                    newimage = di->createScaledImage(scale_size, 0, OFstatic_cast(int, useInterpolation),
                        useAspectRatio);
                break;
            case 4:
                OFLOG_INFO(dcm2imgLogger, "scaling image, Y size=" << scale_size
                    << ", Interpolation=" << OFstatic_cast(int, useInterpolation)
                    << ", Aspect Ratio=" << (useAspectRatio ? "yes" : "no"));
                if (useClip)
                    newimage = di->createScaledImage(left, top, width, height, 0, scale_size,
                        OFstatic_cast(int, useInterpolation), useAspectRatio);
                else
                    newimage = di->createScaledImage(0, scale_size, OFstatic_cast(int, useInterpolation),
                        useAspectRatio);
                break;
            default:
                OFLOG_INFO(dcm2imgLogger, "internal error: unknown scaling type");
                newimage = NULL;
                break;
        }
        if (newimage == NULL)
        {
            OFLOG_FATAL(dcm2imgLogger, "Out of memory or cannot scale image");
            return NULL;
        }
        else if (newimage->getStatus() != EIS_Normal)
        {
            OFLOG_FATAL(dcm2imgLogger, DicomImage::getString(newimage->getStatus()));
            delete newimage;
            return NULL;
        }
        else
        {
            delete di;
            return newimage;
        }
    }
    return di;
}

static const char *getDefaultExtension(
  DicomImage *di,
  E_FileType fileType)
{
    if (di == NULL) return "";

    const char *result = NULL;
    /* determine default file extension */
    switch (fileType)
    {
      case EFT_BMP:
      case EFT_8bitBMP:
      case EFT_24bitBMP:
      case EFT_32bitBMP:
        result = "bmp";
        break;
      case EFT_JPEG:
        result = "jpg";
        break;
      case EFT_JPLS:
        result = "jls";
        break;
#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
      case EFT_JP2K_CS:
        result = "jpc";
        break;
#ifdef WITH_OPENJPEG
      case EFT_JP2K_FILE:
        result = "jp2";
        break;
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */
      case EFT_TIFF:
        result = "tif";
        break;
      case EFT_PNG:
      case EFT_16bitPNG:
        result = "png";
        break;
      default:
        if (di->isMonochrome()) result = "pgm"; else result = "ppm";
        break;
    }
    return result;
}

// case insensitive string comparison
static OFBool ci_compare(const OFString& str1, const OFString& str2)
{
    size_t l = str1.length();
    if (l != str2.length()) return OFFalse;

    for (size_t i = 0; i < l; ++i) {
        if (tolower(str1[i]) != tolower(str2[i]))
            return OFFalse;
    }
    return OFTrue;
}

static E_FileType getFileTypeByExtension(const char *fname)
{
    // no filename, return 8-bit ASCII PNM as the default for stdout
    if (fname == NULL) return EFT_8bitPNM;

    // remove path from filename
    OFString filename(fname);
    size_t pos = filename.find_last_of('/');
    if (pos != OFString_npos) filename = filename.substr(pos + 1);
    pos = filename.find_last_of('\\');
    if (pos != OFString_npos) filename = filename.substr(pos + 1);

    // determine filename extension
    pos = filename.find_last_of('.');

    // no filename extension, return BMP as the default for files
    if (pos == OFString_npos)
    {
        OFLOG_WARN(dcm2imgLogger, "no filename extension specified, writing BMP file");
        return EFT_BMP;
    }

    // copy the extension into a separate string
    OFString extension = filename.substr(pos + 1);
    OFString ext;
    if (ci_compare(extension, OFString("bmp"))) return EFT_BMP;
    if (ci_compare(extension, OFString("jpg"))) return EFT_JPEG;
    if (ci_compare(extension, OFString("jpeg"))) return EFT_JPEG;
    if (ci_compare(extension, OFString("pnm"))) return EFT_RawPNM;
    if (ci_compare(extension, OFString("pgm"))) return EFT_RawPNM;
    if (ci_compare(extension, OFString("ppm"))) return EFT_RawPNM;
    if (ci_compare(extension, OFString("tif"))) return EFT_TIFF;
    if (ci_compare(extension, OFString("tiff"))) return EFT_TIFF;
    if (ci_compare(extension, OFString("png"))) return EFT_PNG;
    if (ci_compare(extension, OFString("jls"))) return EFT_JPLS;
    if (ci_compare(extension, OFString("jpls"))) return EFT_JPLS;
#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
    if (ci_compare(extension, OFString("jpc"))) return EFT_JP2K_CS;
    if (ci_compare(extension, OFString("j2c"))) return EFT_JP2K_CS;
#ifdef WITH_OPENJPEG
    if (ci_compare(extension, OFString("jp2"))) return EFT_JP2K_FILE;
    if (ci_compare(extension, OFString("j2k"))) return EFT_JP2K_FILE;
    if (ci_compare(extension, OFString("jpf"))) return EFT_JP2K_FILE;
    if (ci_compare(extension, OFString("jpg2"))) return EFT_JP2K_FILE;
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

    OFLOG_WARN(dcm2imgLogger, "unsupported filename extension '" << extension << "', writing BMP file");
    return EFT_BMP;
}

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, consoleDescription, rcsid);
    OFCommandLine cmd;

    int exitCode = 0;

    E_FileReadMode      opt_readMode = ERM_autoDetect;    /* default: fileformat or dataset */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */

    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
    OFBool              opt_ignoreVoiLutDepth = OFFalse;  /* default: do not ignore VOI LUT bit depth */
    OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_frameCount = 1;               /* default: one frame */
    OFBool              opt_useFrameNumber = OFFalse;     /* default: use frame counter */
    OFBool              opt_multiFrame = OFFalse;         /* default: no multiframes */
    int                 opt_convertToGrayscale = 0;       /* default: no conversion */
    int                 opt_changePolarity = 0;           /* default: normal polarity */
    int                 opt_useAspectRatio = 1;           /* default: use aspect ratio for scaling */
    OFCmdUnsignedInt    opt_useInterpolation = 1;         /* default: use interpolation method '1' for scaling */
    int                 opt_useClip = 0;                  /* default: don't clip */
    OFCmdSignedInt      opt_left = 0, opt_top = 0;        /* clip region (origin) */
    OFCmdUnsignedInt    opt_width = 0, opt_height = 0;    /* clip region (extension) */
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

    EF_VoiLutFunction   opt_voiFunction = EFV_Default;
    ES_PresentationLut  opt_presShape = ESP_Default;
    OFString            opt_displayFile;
    int                 opt_displayFunction = 0;          /* default: GSDF */
                        /* 0=GSDF, 1=CIELAB */
    OFCmdFloat          opt_ambientLight = -1;            /* default: not set */
    OFCmdFloat          opt_illumination = -1;            /* default: not set */
    OFCmdFloat          opt_minDensity = -1;              /* default: not set */
    OFCmdFloat          opt_maxDensity = -1;              /* default: not set */
    DiDisplayFunction::E_DeviceType deviceType = DiDisplayFunction::EDT_Monitor;

#ifdef WITH_LIBTIFF
    // TIFF parameters
    DiTIFFCompression   opt_tiffCompression = E_tiffLZWCompression;
    DiTIFFLZWPredictor  opt_lzwPredictor = E_tiffLZWPredictorDefault;
    OFCmdUnsignedInt    opt_rowsPerStrip = 0;
#endif

#ifdef WITH_LIBPNG
    // PNG parameters
    DiPNGInterlace      opt_interlace = E_pngInterlaceAdam7;
    DiPNGMetainfo       opt_metainfo  = E_pngFileMetainfo;
#endif

    // JPEG parameters
    OFCmdUnsignedInt    opt_quality = 90;                 /* default: 90% JPEG quality */
    E_SubSampling       opt_sampling = ESS_422;           /* default: 4:2:2 sub-sampling */
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
    OFBool              opt_predictor6WorkaroundEnable = OFFalse;
    OFBool              opt_cornellWorkaroundEnable = OFFalse;
    OFBool              opt_forceSingleFragmentPerFrame = OFFalse;
    OFBool              opt_preserveBitsStored = OFFalse;

    // JPEG-LS parameters
    OFBool              opt_true_lossless = OFFalse;
    OFBool              opt_force_8_bit = OFFalse;

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
    // JPEG 2000 parameters
    OFBool opt_lossy_jp2k = OFFalse;
    OFCmdFloat opt_compression_ratio = 0.2;
    OFCmdUnsignedInt opt_compressed_size = 0;
    OFCmdUnsignedInt opt_tile_size_x = 0;
    OFCmdUnsignedInt opt_tile_size_y = 0;
    OFCmdUnsignedInt opt_levels = 6;
    J2K_Progression opt_progression = EJ2O_progression_lrcp;
#ifdef WITH_OPENJPEG
    OFBool opt_use_openjpeg = OFTrue;
    OFCmdUnsignedInt openJPEGNumThreads = 0;
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

    // other parameters
    int                 opt_Overlay[16];
    int                 opt_O_used = 0;                   /* flag for +O parameter */
    EM_Overlay          opt_OverlayMode = EMO_Default;    /* default: Replace or ROI */

    OFCmdFloat          opt_foregroundDensity = 1.0;
    OFCmdFloat          opt_thresholdDensity  = 0.5;

    int                 opt_imageInfo = 0;                /* default: no info */
    int                 opt_suppressOutput = 0;           /* default: create output */
    E_FileType          opt_fileType = EFT_default;       /* default: auto */
    OFCmdUnsignedInt    opt_fileBits = 0;                 /* default: 0 */
    const char *        opt_ifname = NULL;
    const char *        opt_ofname = NULL;

    unsigned long i;
    for (i = 0; i < 16; i++)
        opt_Overlay[i] = 2;                               /* default: display all overlays if present */

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in", "DICOM input filename to be converted\n(\"-\" for stdin)");
    cmd.addParam("bitmap-out", OFFIS_OUTFILE_DESCRIPTION, OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("input options:");

     cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",          "+f",      "read file format or data set (default)");
      cmd.addOption("--read-file-only",     "+fo",     "read file format only");
      cmd.addOption("--read-dataset",       "-f",      "read data set without file meta information");

     cmd.addSubGroup("input transfer syntax:");
      cmd.addOption("--read-xfer-auto",     "-t=",     "use TS recognition (default)");
      cmd.addOption("--read-xfer-detect",   "-td",     "ignore TS specified in the file meta header");
      cmd.addOption("--read-xfer-little",   "-te",     "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",      "-tb",     "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit", "-ti",     "read with implicit VR little endian TS");

    cmd.addGroup("image processing options:");

     cmd.addSubGroup("frame selection:");
      cmd.addOption("--frame",              "+F",   1, "[n]umber: integer",
                                                       "select specified frame (default: 1)");
      cmd.addOption("--frame-range",        "+Fr",  2, "[n]umber [c]ount: integer",
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
      cmd.addOption("--recognize-aspect",   "+a",      "recognize pixel aspect ratio when scaling (def.)");
      cmd.addOption("--ignore-aspect",      "-a",      "ignore pixel aspect ratio when scaling");
      cmd.addOption("--interpolate",        "+i",   1, "[n]umber of algorithm: integer",
                                                       "use interpolation when scaling (1..4, def: 1)");
      cmd.addOption("--no-interpolation",   "-i",      "no interpolation when scaling");
      cmd.addOption("--no-scaling",         "-S",      "no scaling, ignore pixel aspect ratio (default)");
      cmd.addOption("--scale-x-factor",     "+Sxf", 1, "[f]actor: float",
                                                       "scale x axis by factor, auto-compute y axis");
      cmd.addOption("--scale-y-factor",     "+Syf", 1, "[f]actor: float",
                                                       "scale y axis by factor, auto-compute x axis");
      cmd.addOption("--scale-x-size",       "+Sxv", 1, "[n]umber: integer",
                                                       "scale x axis to n pixels, auto-compute y axis");
      cmd.addOption("--scale-y-size",       "+Syv", 1, "[n]umber: integer",
                                                       "scale y axis to n pixels, auto-compute x axis");

     cmd.addSubGroup("color space conversion (JPEG compressed images only):");
      cmd.addOption("--conv-photometric",   "+cp",     "convert if YCbCr photometric interpr. (default)");
      cmd.addOption("--conv-lossy",         "+cl",     "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-guess",         "+cg",     "convert to RGB if YCbCr is guessed by library");
      cmd.addOption("--conv-guess-lossy",   "+cgl",    "convert to RGB if lossy JPEG and YCbCr is\nguessed by the underlying JPEG library");
      cmd.addOption("--conv-always",        "+ca",     "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",         "+cn",     "never convert color space");

     cmd.addSubGroup("bits stored:");
      cmd.addOption("--bits-stored-fix",     "+bs",    "correct inconsistent bits stored value (default)");
      cmd.addOption("--bits-stored-keep",    "-bs",    "preserve inconsistent bits stored value");

     cmd.addSubGroup("workaround options for incorrect encodings (JPEG compressed images only):");
      cmd.addOption("--workaround-pred6",    "+w6",    "enable workaround for JPEG lossless images\nwith overflow in predictor 6");
      cmd.addOption("--workaround-incpl",    "+wi",    "enable workaround for incomplete JPEG data");
      cmd.addOption("--workaround-cornell",  "+wc",    "enable workaround for 16-bit JPEG lossless\nCornell images with Huffman table overflow");

     cmd.addSubGroup("modality LUT transformation:");
      cmd.addOption("--no-modality",        "-M",      "ignore stored modality LUT transformation");
      cmd.addOption("--use-modality",       "+M",      "use modality LUT transformation (default)");

     cmd.addSubGroup("VOI LUT transformation:");
      cmd.addOption("--no-windowing",       "-W",      "no VOI windowing (default)");
      cmd.addOption("--use-window",         "+Wi",  1, "[n]umber: integer",
                                                       "use the n-th VOI window from image file");
      cmd.addOption("--use-voi-lut",        "+Wl",  1, "[n]umber: integer",
                                                       "use the n-th VOI look up table from image file");
      cmd.addOption("--min-max-window",     "+Wm",     "compute VOI window using min-max algorithm");
      cmd.addOption("--min-max-window-n",   "+Wn",     "compute VOI window using min-max algorithm,\nignoring extreme values");
      cmd.addOption("--roi-min-max-window", "+Wr",  4, "[l]eft [t]op [w]idth [h]eight: integer",
                                                       "compute ROI window using min-max algorithm,\nregion of interest is specified by l,t,w,h");
      cmd.addOption("--histogram-window",   "+Wh",  1, "[n]umber: integer",
                                                       "compute VOI window using Histogram algorithm,\nignoring n percent");
      cmd.addOption("--set-window",         "+Ww",  2, "[c]enter [w]idth: float",
                                                       "compute VOI window using center c and width w");
      cmd.addOption("--linear-function",    "+Wfl",    "set VOI LUT function to LINEAR");
      cmd.addOption("--sigmoid-function",   "+Wfs",    "set VOI LUT function to SIGMOID");

     cmd.addSubGroup("presentation LUT transformation:");
      cmd.addOption("--identity-shape",     "+Pid",    "set presentation LUT shape to IDENTITY");
      cmd.addOption("--inverse-shape",      "+Piv",    "set presentation LUT shape to INVERSE");
      cmd.addOption("--lin-od-shape",       "+Pod",    "set presentation LUT shape to LIN OD");

     cmd.addSubGroup("overlay:");
      cmd.addOption("--no-overlays",        "-O",      "do not display overlays");
      cmd.addOption("--display-overlay",    "+O" ,  1, "[n]umber: integer",
                                                       "display overlay n (0..16, 0=all, default: +O 0)");
      cmd.addOption("--ovl-replace",        "+Omr",    "use overlay mode \"Replace\"\n(default for Graphic overlays)");
      cmd.addOption("--ovl-threshold",      "+Omt",    "use overlay mode \"Threshold Replace\"");
      cmd.addOption("--ovl-complement",     "+Omc",    "use overlay mode \"Complement\"");
      cmd.addOption("--ovl-invert",         "+Omv",    "use overlay mode \"Invert Bitmap\"");
      cmd.addOption("--ovl-roi",            "+Omi",    "use overlay mode \"Region of Interest\"\n(default for ROI overlays)");
      cmd.addOption("--set-foreground",     "+Osf", 1, "[d]ensity: float",
                                                       "set overlay foreground density (0..1, def: 1)");
      cmd.addOption("--set-threshold",      "+Ost", 1, "[d]ensity: float",
                                                       "set overlay threshold density (0..1, def: 0.5)");

     cmd.addSubGroup("display LUT transformation:");
      cmd.addOption("--monitor-file",       "+Dm",  1, "[f]ilename: string",
                                                       "calibrate output according to monitor\ncharacteristics defined in f");
      cmd.addOption("--printer-file",       "+Dp",  1, "[f]ilename: string",
                                                       "calibrate output according to printer\ncharacteristics defined in f");
      cmd.addOption("--ambient-light",      "+Da",  1, "[a]mbient light: float",
                                                       "ambient light value (cd/m^2, default: file f)");
      cmd.addOption("--illumination",       "+Di",  1, "[i]llumination: float",
                                                       "illumination value (cd/m^2, default: file f)");
      cmd.addOption("--min-density",        "+Dn",  1, "[m]inimum optical density: float",
                                                       "Dmin value (default: off, only with +Dp)");
      cmd.addOption("--max-density",        "+Dx",  1, "[m]aximum optical density: float",
                                                       "Dmax value (default: off, only with +Dp)");
      cmd.addOption("--gsd-function",       "+Dg",     "use GSDF for calibration (default for +Dm/+Dp)");
      cmd.addOption("--cielab-function",    "+Dc",     "use CIELAB function for calibration ");

     cmd.addSubGroup("compatibility:");
      cmd.addOption("--accept-acr-nema",    "+Ma",     "accept ACR-NEMA images without photometric\ninterpretation");
      cmd.addOption("--accept-palettes",    "+Mp",     "accept incorrect palette attribute tags\n(0028,111x) and (0028,121x)");
      cmd.addOption("--check-lut-depth",    "+Mc",     "check 3rd value of the LUT descriptor, compare\nwith expected bit depth based on LUT data");
      cmd.addOption("--ignore-mlut-depth",  "+Mm",     "ignore 3rd value of the modality LUT descriptor,\ndetermine bits per table entry automatically");
      cmd.addOption("--ignore-vlut-depth",  "+Mv",     "ignore 3rd value of the VOI LUT descriptor,\ndetermine bits per table entry automatically");

#ifdef WITH_LIBTIFF
     cmd.addSubGroup("TIFF format:");
      cmd.addOption("--compr-lzw",          "+Tl",     "LZW compression (default)");
      cmd.addOption("--compr-rle",          "+Tr",     "RLE compression");
      cmd.addOption("--compr-none",         "+Tn",     "uncompressed");
      cmd.addOption("--predictor-default",  "+Pd",     "no LZW predictor (default)");
      cmd.addOption("--predictor-none",     "+Pn",     "LZW predictor 1 (no prediction)");
      cmd.addOption("--predictor-horz",     "+Ph",     "LZW predictor 2 (horizontal differencing)");
      cmd.addOption("--rows-per-strip",     "+Rs",  1, "[r]ows: integer (default: 0)",
                                                       "rows per strip, default 8K per strip");
#endif

#ifdef WITH_LIBPNG
     cmd.addSubGroup("PNG format:");
      cmd.addOption("--interlace",          "+il",     "create interlaced file (default)");
      cmd.addOption("--nointerlace",        "-il",     "create non-interlaced file");
      cmd.addOption("--meta-file",          "+mf",     "create PNG file meta information (default)");
      cmd.addOption("--meta-none",          "-mf",     "no PNG file meta information");
#endif

     cmd.addSubGroup("JPEG format:");
      cmd.addOption("--compr-quality",      "+Jq",  1, "[q]uality: integer (0..100, default: 90)",
                                                       "quality value for compression (in percent)");
      cmd.addOption("--sample-444",         "+Js4",    "4:4:4 sampling (no subsampling)");
      cmd.addOption("--sample-422",         "+Js2",    "4:2:2 subsampling (horizontal subsampling of\nchroma components, default)");
      cmd.addOption("--sample-411",         "+Js1",    "4:1:1 subsampling (horizontal and vertical\nsubsampling of chroma components)");

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
     cmd.addSubGroup("JPEG-LS and JPEG 2000 format:");
#else
     cmd.addSubGroup("JPEG-LS format:");
#endif
      cmd.addOption("--rendered-lossless",  "+Trl",   "lossless compression of rendered image (default)");
      cmd.addOption("--true-lossless",      "+Tll",   "lossless compression of raw image");
      cmd.addOption("--force-8-bit",        "+f8",    "force output bit depth to 8 bits\n(not with --true-lossless)");

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
     cmd.addSubGroup("JPEG 2000 format:");
      cmd.addOption("--rendered-lossy",     "+Try",   "lossy compression of rendered image");
      cmd.addOption("--compression-ratio",   "-r",  1, "[r]atio: double, 0 < r < 1.0 (default: 0.2)",
                                                       "compress with compression ratio r");
      cmd.addOption("--compressed-size",     "-s",  1, "[s]ize: integer",
                                                       "compress each frame to approx. s bytes");
      cmd.addOption("--tile-size-x",                1, "[s]ize: integer (default: image width)",
                                                       "nominal tile width");
      cmd.addOption("--tile-size-y",                1, "[s]ize: integer (default: image height)",
                                                       "nominal tile height");
      cmd.addOption("--levels",              "-l",  1, "[l]evels: integer (default: 6)",
                                                       "number of wavelet resolution levels");
      cmd.addOption("--progression",         "-po", 1, "LRCP/RLCP/RPCL/PCRL/CPRL (default: LRCP)",
                                                       "progression order for layer, resolution,\ncomponent and position");
#ifdef WITH_OPENJPEG
     cmd.addSubGroup("JPEG 2000 codec:");
      cmd.addOption("--openjpeg",           "+co",    "use OpenJPEG codec (default)");

      cmd.addOption("--jasper",             "+cj",    "use JasPer codec");
      cmd.addOption("--threads",            "+t",  1, "[t]hreads: integer (default: 1)",
                                                      "use t threads in the decoder (not with --jasper)");
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

     cmd.addSubGroup("other transformations:");
      cmd.addOption("--grayscale",          "+G",      "convert color image to grayscale (monochrome)");
      cmd.addOption("--change-polarity",    "+P",      "change polarity (invert pixel output)");
      cmd.addOption("--clip-region",        "+C",   4, "[l]eft [t]op [w]idth [h]eight: integer",
                                                       "clip image region (l, t, w, h)");

    cmd.addGroup("output options:");
     cmd.addSubGroup("general:");
      cmd.addOption("--image-info",         "-im",     "print image details (requires verbose mode)");
      cmd.addOption("--no-output",          "-o",      "do not create any output (useful with -im)");
     cmd.addSubGroup("filename generation (only with --frame-range or --all-frames):");
      cmd.addOption("--use-frame-counter",  "+Fc",     "use 0-based counter for filenames (default)");
      cmd.addOption("--use-frame-number",   "+Fn",     "use absolute frame number for filenames");
     cmd.addSubGroup("image format:");
      cmd.addOption("--write-auto",         "+oa",     "determine file format from filename extension\n(default for files)");
      cmd.addOption("--write-raw-pnm",      "+op",     "write 8-bit binary PGM/PPM");
      cmd.addOption("--write-8-bit-pnm",    "+opb",    "write 8-bit ASCII PGM/PPM (default for stdout)");
      cmd.addOption("--write-16-bit-pnm",   "+opw",    "write 16-bit ASCII PGM/PPM");
      cmd.addOption("--write-n-bit-pnm",    "+opn", 1, "[n]umber: integer",
                                                       "write n-bit ASCII PGM/PPM (1..32)");
      cmd.addOption("--write-bmp",          "+ob",     "write 8-bit (monochrome) or 24-bit (color) BMP");
      cmd.addOption("--write-8-bit-bmp",    "+obp",    "write 8-bit palette BMP (monochrome only)");
      cmd.addOption("--write-24-bit-bmp",   "+obt",    "write 24-bit truecolor BMP");
      cmd.addOption("--write-32-bit-bmp",   "+obr",    "write 32-bit truecolor BMP");
#ifdef WITH_LIBTIFF
      cmd.addOption("--write-tiff",         "+ot",     "write 8-bit (monochrome) or 24-bit (color) TIFF");
#endif
#ifdef WITH_LIBPNG
      cmd.addOption("--write-png",          "+on",     "write 8-bit (monochrome) or 24-bit (color) PNG");
      cmd.addOption("--write-16-bit-png",   "+on2",    "write 16-bit (monochrome) or 48-bit (color) PNG");
#endif
      cmd.addOption("--write-jpeg",         "+oj",     "write 8-bit lossy JPEG (baseline)");
      cmd.addOption("--write-jpls",         "+ol",     "write JPEG-LS");

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
      cmd.addOption("--write-jp2k-cs",      "+ok",     "write JPEG 2000 code stream");
#ifdef WITH_OPENJPEG
      cmd.addOption("--write-jp2k",         "+o2",     "write JPEG 2000 file");
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

#ifdef PASTEL_COLOR_OUTPUT
      cmd.addOption("--write-pastel-pnm",   "+op",     "write 8-bit binary PPM with pastel colors\n(early experimental version)");
#endif

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
                COUT << "- " << DiJPEGPlugin::getLibraryVersionString() << OFendl;
                COUT << "- " << DJLSDecoderRegistration::getLibraryVersionString() << OFendl;
#ifdef WITH_LIBTIFF
                COUT << "- " << DiTIFFPlugin::getLibraryVersionString() << OFendl;
#endif
#ifdef WITH_LIBPNG
                COUT << "- " << DiPNGPlugin::getLibraryVersionString() << OFendl;
#endif
                return 0;
            }
        }

        /* command line parameters */

        cmd.getParam(1, opt_ifname);
        cmd.getParam(2, opt_ofname);

        /* general options */

        OFLog::configureFromCommandLine(cmd, app);

        /* input options: input file format */

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-file")) opt_readMode = ERM_autoDetect;
        if (cmd.findOption("--read-file-only")) opt_readMode = ERM_fileOnly;
        if (cmd.findOption("--read-dataset")) opt_readMode = ERM_dataset;
        cmd.endOptionBlock();

        /* input options: input transfer syntax */

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-xfer-auto"))
            opt_transferSyntax = EXS_Unknown;
        if (cmd.findOption("--read-xfer-detect"))
            dcmAutoDetectDatasetXfer.set(OFTrue);
        if (cmd.findOption("--read-xfer-little"))
        {
            app.checkDependence("--read-xfer-little", "--read-dataset", opt_readMode == ERM_dataset);
            opt_transferSyntax = EXS_LittleEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-big"))
        {
            app.checkDependence("--read-xfer-big", "--read-dataset", opt_readMode == ERM_dataset);
            opt_transferSyntax = EXS_BigEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-implicit"))
        {
            app.checkDependence("--read-xfer-implicit", "--read-dataset", opt_readMode == ERM_dataset);
            opt_transferSyntax = EXS_LittleEndianImplicit;
        }
        cmd.endOptionBlock();

        /* image processing options: compatibility options */

        if (cmd.findOption("--accept-acr-nema"))
            opt_compatibilityMode |= CIF_AcrNemaCompatibility;
        if (cmd.findOption("--accept-palettes"))
            opt_compatibilityMode |= CIF_WrongPaletteAttributeTags;
        if (cmd.findOption("--check-lut-depth"))
            opt_compatibilityMode |= CIF_CheckLutBitDepth;
        if (cmd.findOption("--ignore-mlut-depth"))
        {
            app.checkConflict("--ignore-mlut-depth", "--check-lut-depth", (opt_compatibilityMode & CIF_CheckLutBitDepth) > 0);
            opt_compatibilityMode |= CIF_IgnoreModalityLutBitDepth;
        }
        if (cmd.findOption("--ignore-vlut-depth"))
        {
            app.checkConflict("--ignore-vlut-depth", "--check-lut-depth", (opt_compatibilityMode & CIF_CheckLutBitDepth) > 0);
            opt_ignoreVoiLutDepth = OFTrue;
        }

        /* image processing options: frame selection */

        cmd.beginOptionBlock();
        if (cmd.findOption("--frame"))
            app.checkValue(cmd.getValueAndCheckMin(opt_frame, 1));
        if (cmd.findOption("--frame-range"))
        {
            app.checkValue(cmd.getValueAndCheckMin(opt_frame, 1));
            app.checkValue(cmd.getValueAndCheckMin(opt_frameCount, 1));
            opt_multiFrame = OFTrue;
        }
        if (cmd.findOption("--all-frames"))
        {
            opt_frameCount = 0;
            opt_multiFrame = OFTrue;
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
            app.checkValue(cmd.getValue(opt_width));
            app.checkValue(cmd.getValue(opt_height));
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

        /* image processing options: color space conversion */

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

        cmd.beginOptionBlock();
        if (cmd.findOption("--bits-stored-fix")) opt_preserveBitsStored = OFFalse;
        if (cmd.findOption("--bits-stored-keep")) opt_preserveBitsStored = OFTrue;
        cmd.beginOptionBlock();

        if (cmd.findOption("--workaround-pred6")) opt_predictor6WorkaroundEnable = OFTrue;
        if (cmd.findOption("--workaround-incpl")) opt_forceSingleFragmentPerFrame = OFTrue;
        if (cmd.findOption("--workaround-cornell")) opt_cornellWorkaroundEnable = OFTrue;

        /* image processing options: modality LUT transformation */

        cmd.beginOptionBlock();
        if (cmd.findOption("--no-modality"))
            opt_compatibilityMode |= CIF_IgnoreModalityTransformation;
        if (cmd.findOption("--use-modality"))
            opt_compatibilityMode &= ~CIF_IgnoreModalityTransformation;
        cmd.endOptionBlock();

        /* image processing options: VOI LUT transformation */

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
        cmd.beginOptionBlock();
        if (cmd.findOption("--linear-function"))
            opt_voiFunction = EFV_Linear;
        if (cmd.findOption("--sigmoid-function"))
            opt_voiFunction = EFV_Sigmoid;
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
        if (cmd.findOption("--min-density"))
        {
            app.checkDependence("--min-density", "--printer-file", deviceType == DiDisplayFunction::EDT_Printer);
            app.checkValue(cmd.getValueAndCheckMin(opt_minDensity, 0));
        }
        if (cmd.findOption("--max-density"))
        {
            app.checkDependence("--max-density", "--printer-file", deviceType == DiDisplayFunction::EDT_Printer);
            app.checkValue(cmd.getValueAndCheckMin(opt_maxDensity, (opt_minDensity < 0) ? 0.0 : opt_minDensity, OFFalse /*incl*/));
        }

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
                app.checkValue(cmd.getValueAndCheckMinMax(l, 0, 16));
                if (!opt_O_used)
                {
                    for (i = 0; i < 16; i++) opt_Overlay[i] = 0;
                    opt_O_used = 1;
                }
                if (l > 0)
                    opt_Overlay[l - 1] = 1;
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
            opt_OverlayMode = EMO_Replace;
        if (cmd.findOption("--ovl-threshold"))
            opt_OverlayMode = EMO_ThresholdReplace;
        if (cmd.findOption("--ovl-complement"))
            opt_OverlayMode = EMO_Complement;
        if (cmd.findOption("--ovl-invert"))
            opt_OverlayMode = EMO_InvertBitmap;
        if (cmd.findOption("--ovl-roi"))
            opt_OverlayMode = EMO_RegionOfInterest;
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

        /* image processing options: PNG options */

#ifdef WITH_LIBPNG
        cmd.beginOptionBlock();
        if (cmd.findOption("--interlace"))   opt_interlace = E_pngInterlaceAdam7;
        if (cmd.findOption("--nointerlace")) opt_interlace = E_pngInterlaceNone;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--meta-none"))    opt_metainfo = E_pngNoMetainfo;
        if (cmd.findOption("--meta-file"))    opt_metainfo = E_pngFileMetainfo;
        cmd.endOptionBlock();
#endif

        /* image processing options: JPEG options */

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

        /* JPEG-LS options */

        if (cmd.findOption("--force-8-bit"))
            opt_force_8_bit = OFTrue;

        cmd.beginOptionBlock();
        if (cmd.findOption("--rendered-lossless"))
            opt_true_lossless = OFFalse;
        if (cmd.findOption("--true-lossless"))
        {
            app.checkConflict("--force-8-bit", "--true-lossless", opt_force_8_bit);
            app.checkConflict("--grayscale", "--true-lossless", opt_convertToGrayscale);
            app.checkConflict("--change-polarity", "--true-lossless", opt_changePolarity);
            app.checkConflict("--clip-region", "--true-lossless", opt_useClip);
            app.checkConflict("--rotate-left", "--true-lossless", opt_rotateDegree == 270);
            app.checkConflict("--rotate-right", "--true-lossless", opt_rotateDegree == 90);
            app.checkConflict("--rotate-top-down", "--true-lossless", opt_rotateDegree == 180);
            app.checkConflict("--flip-horizontally", "--true-lossless", opt_flipType == 1);
            app.checkConflict("--flip-vertically", "--true-lossless", opt_flipType == 2);
            app.checkConflict("--flip-both-axes", "--true-lossless", opt_flipType == 3);
            app.checkConflict("--recognize-aspect", "--true-lossless", cmd.findOption("--recognize-aspect"));
            app.checkConflict("--ignore-aspect", "--true-lossless", cmd.findOption("--ignore-aspect"));
            app.checkConflict("--interpolate", "--true-lossless", cmd.findOption("--interpolate"));
            app.checkConflict("--scale-x-factor", "--true-lossless", opt_scaleType == 1);
            app.checkConflict("--scale-y-factor", "--true-lossless", opt_scaleType == 2);
            app.checkConflict("--scale-x-size", "--true-lossless", opt_scaleType == 3);
            app.checkConflict("--scale-y-size", "--true-lossless", opt_scaleType == 4);
            app.checkConflict("--use-window", "--true-lossless", opt_windowType == 1);
            app.checkConflict("--use-voi-lut", "--true-lossless", opt_windowType == 2);
            app.checkConflict("--min-max-window", "--true-lossless", opt_windowType == 3);
            app.checkConflict("--histogram-window", "--true-lossless", opt_windowType == 4);
            app.checkConflict("--set-window", "--true-lossless", opt_windowType == 5);
            app.checkConflict("--min-max-window-n", "--true-lossless", opt_windowType == 6);
            app.checkConflict("--roi-min-max-window", "--true-lossless", opt_windowType == 7);
            app.checkConflict("--use-window", "--true-lossless", opt_windowType == 1);
            app.checkConflict("--use-window", "--true-lossless", opt_windowType == 1);
            app.checkConflict("--use-window", "--true-lossless", opt_windowType == 1);
            app.checkConflict("--linear-function", "--true-lossless", opt_voiFunction == EFV_Linear);
            app.checkConflict("--sigmoid-function", "--true-lossless", opt_voiFunction == EFV_Sigmoid);
            app.checkConflict("--identity-shape", "--true-lossless", opt_presShape == ESP_Identity);
            app.checkConflict("--inverse-shape", "--true-lossless", opt_presShape == ESP_Inverse);
            app.checkConflict("--lin-od-shape", "--true-lossless", opt_presShape == ESP_LinOD);
            app.checkConflict("--monitor-file", "--true-lossless", deviceType == DiDisplayFunction::EDT_Monitor && opt_displayFile.length() > 0);
            app.checkConflict("--printer-file", "--true-lossless", deviceType == DiDisplayFunction::EDT_Printer && opt_displayFile.length() > 0);
            app.checkConflict("--ambient-light", "--true-lossless", opt_ambientLight != -1);
            app.checkConflict("--illumination", "--true-lossless", opt_illumination != -1);
            app.checkConflict("--min-density", "--true-lossless", opt_minDensity != -1);
            app.checkConflict("--max-density", "--true-lossless", opt_maxDensity != -1);
            app.checkConflict("--gsd-function", "--true-lossless", cmd.findOption("--gsd-function"));
            app.checkConflict("--cielab-function", "--true-lossless", opt_displayFunction == 1);
            app.checkConflict("--display-overlay", "--true-lossless", cmd.findOption("--display-overlay", 0, OFCommandLine::FOM_First));
            app.checkConflict("--ovl-replace", "--true-lossless", opt_OverlayMode == EMO_Replace);
            app.checkConflict("--ovl-threshold", "--true-lossless", opt_OverlayMode == EMO_ThresholdReplace);
            app.checkConflict("--ovl-complement", "--true-lossless", opt_OverlayMode == EMO_Complement);
            app.checkConflict("--ovl-invert", "--true-lossless", opt_OverlayMode == EMO_InvertBitmap);
            app.checkConflict("--ovl-roi", "--true-lossless", opt_OverlayMode == EMO_RegionOfInterest);
            app.checkConflict("--set-foreground", "--true-lossless", cmd.findOption("--set-foreground"));
            app.checkConflict("--set-threshold", "--true-lossless", cmd.findOption("--set-threshold"));
            opt_true_lossless = OFTrue;
        }
        cmd.endOptionBlock();

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
#ifdef WITH_OPENJPEG
        cmd.beginOptionBlock();
        if (cmd.findOption("--openjpeg")) opt_use_openjpeg = OFTrue;
        if (cmd.findOption("--jasper")) opt_use_openjpeg = OFFalse;
        cmd.endOptionBlock();

        if (cmd.findOption("--threads"))
        {
            app.checkConflict("--threads", "--jasper", !opt_use_openjpeg);
             // we limit the number of threads for decoding a single JPEG 2000 frame to 1024, which should be more than enough.
            app.checkValue(cmd.getValueAndCheckMinMax(openJPEGNumThreads, 1, 1024));
        }
#endif /* WITH_OPENJPEG */

        if (cmd.findOption("--rendered-lossy"))
        {
            app.checkConflict("--rendered-lossless", "--rendered-lossy", cmd.findOption("--rendered-lossless"));
            app.checkConflict("--true-lossless",     "--rendered-lossy", opt_true_lossless);
            opt_lossy_jp2k = OFTrue;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--compression-ratio"))
        {
          app.checkConflict("--compression-ratio", "--rendered-lossless", cmd.findOption("--rendered-lossless"));
          app.checkConflict("--compression-ratio", "--true-lossless", opt_true_lossless);
          app.checkValue(cmd.getValueAndCheckMin(opt_compression_ratio, 0.0, OFFalse));
          if (opt_compression_ratio >= 1.0) app.checkValue(OFCommandLine::VS_Overflow);
        }
        if (cmd.findOption("--compressed-size"))
        {
          app.checkConflict("--compressed-size", "--rendered-lossless", cmd.findOption("--rendered-lossless"));
          app.checkConflict("--compressed-size", "--true-lossless", opt_true_lossless);
          app.checkValue(cmd.getValueAndCheckMin(opt_compressed_size, 1));
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--tile-size-x"))
        {
            app.checkValue(cmd.getValue(opt_tile_size_x));
        }
        if (cmd.findOption("--tile-size-y"))
        {
            app.checkValue(cmd.getValue(opt_tile_size_y));
        }

        if (cmd.findOption("--levels"))
        {
            app.checkValue(cmd.getValueAndCheckMinMax(opt_levels, 1, 255));
        }
        if (cmd.findOption("--progression"))
        {
          OFCmdString progressionMode;
          app.checkValue(cmd.getValue(progressionMode));
          if ((progressionMode == "lrcp") || (progressionMode == "LRCP")) opt_progression = EJ2O_progression_lrcp;
          else if ((progressionMode == "rlcp") || (progressionMode == "RLCP")) opt_progression = EJ2O_progression_rlcp;
          else if ((progressionMode == "rpcl") || (progressionMode == "RPCL")) opt_progression = EJ2O_progression_rpcl;
          else if ((progressionMode == "pcrl") || (progressionMode == "PCRL")) opt_progression = EJ2O_progression_pcrl;
          else if ((progressionMode == "cprl") || (progressionMode == "CPRL")) opt_progression = EJ2O_progression_cprl;
          else
          {
            app.printError("known --progression modes are LRCP, RLCP, RPCL, PCRL or CPRL");
          }
        }

#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

        /* output options */

        if (cmd.findOption("--image-info"))
        {
            app.checkDependence("--image-info", "verbose mode", dcm2imgLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
            opt_imageInfo = 1;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--use-frame-counter"))
        {
            app.checkDependence("--use-frame-counter", "--frame-range or --all-frames", opt_multiFrame);
            opt_useFrameNumber = OFFalse;
        }
        if (cmd.findOption("--use-frame-number"))
        {
            app.checkDependence("--use-frame-number", "--frame-range or --all-frames", opt_multiFrame);
            opt_useFrameNumber = OFTrue;
        }
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--no-output"))
            opt_suppressOutput = 1;
        if (cmd.findOption("--write-raw-pnm"))
            opt_fileType = EFT_RawPNM;
        if (cmd.findOption("--write-8-bit-pnm"))
            opt_fileType = EFT_8bitPNM;
        if (cmd.findOption("--write-16-bit-pnm"))
            opt_fileType = EFT_16bitPNM;
        if (cmd.findOption("--write-n-bit-pnm"))
        {
            opt_fileType = EFT_NbitPNM;
            app.checkValue(cmd.getValueAndCheckMinMax(opt_fileBits, 1, 32));
        }
        if (cmd.findOption("--write-bmp"))
            opt_fileType = EFT_BMP;
        if (cmd.findOption("--write-8-bit-bmp"))
            opt_fileType = EFT_8bitBMP;
        if (cmd.findOption("--write-24-bit-bmp"))
            opt_fileType = EFT_24bitBMP;
        if (cmd.findOption("--write-32-bit-bmp"))
            opt_fileType = EFT_32bitBMP;
        if (cmd.findOption("--write-jpeg"))
            opt_fileType = EFT_JPEG;
        if (cmd.findOption("--write-jpls"))
            opt_fileType = EFT_JPLS;
#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
        if (cmd.findOption("--write-jp2k-cs"))
            opt_fileType = EFT_JP2K_CS;
#ifdef WITH_OPENJPEG
        if (cmd.findOption("--write-jp2k"))
        {
            app.checkConflict("--write-jp2k", "--jasper", !opt_use_openjpeg);
            opt_fileType = EFT_JP2K_FILE;
        }
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */
#ifdef WITH_LIBTIFF
        if (cmd.findOption("--write-tiff"))
            opt_fileType = EFT_TIFF;
#endif
#ifdef WITH_LIBPNG
        if (cmd.findOption("--write-png"))
            opt_fileType = EFT_PNG;
        if (cmd.findOption("--write-16-bit-png"))
            opt_fileType = EFT_16bitPNG;
#endif
#ifdef PASTEL_COLOR_OUTPUT
        if (cmd.findOption("--write-pastel-pnm"))
            opt_fileType = EFT_PastelPNM;
#endif
        if (cmd.findOption("--write-auto"))
            opt_fileType = EFT_default;
        cmd.endOptionBlock();
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcm2imgLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcm2imgLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    if (opt_ofname)
    {
        if (opt_suppressOutput)
            OFLOG_WARN(dcm2imgLogger, "ignoring parameter bitmap-out because of option --no-output");
        else if (opt_fileType == EFT_default)
            opt_fileType = getFileTypeByExtension(opt_ofname);
    }

    OFLOG_INFO(dcm2imgLogger, "reading DICOM file: " << opt_ifname);

    // register RLE decompression codec
    DcmRLEDecoderRegistration::registerCodecs();

    // register JPEG decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion, EUC_default,
        EPC_default, opt_predictor6WorkaroundEnable, opt_cornellWorkaroundEnable,
        opt_forceSingleFragmentPerFrame, opt_preserveBitsStored);

    // register JPEG-LS decompression codecs
    DJLSDecoderRegistration::registerCodecs();

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
#ifdef WITH_OPENJPEG
    if (opt_use_openjpeg)
    {
        // register global OpenJPEG decompression codecs
        O2DecoderRegistration::registerCodecs(EJ2UC_default, EJ2PC_restore, OFstatic_cast(int, openJPEGNumThreads), 0 /* decoder flags */, OFTrue, OFTrue, opt_preserveBitsStored);
    }
    else
    {
        // register global JasPer decompression codecs
        D2DecoderRegistration::registerCodecs(EJ2UC_default, EJ2PC_restore, OFTrue, OFTrue, opt_preserveBitsStored);
    }
#else /* WITH_OPENJPEG */
    // register global JasPer decompression codecs
    D2DecoderRegistration::registerCodecs(EJ2UC_default, EJ2PC_restore, OFTrue, OFTrue, opt_preserveBitsStored);
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

    DcmDataset *dataset = NULL;
    DicomImage *di = NULL;
    DiDisplayFunction *disp = NULL;
    E_TransferSyntax xfer = EXS_Unknown;

    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->loadFile(opt_ifname, opt_transferSyntax, EGL_withoutGL, DCM_MaxReadLength, opt_readMode);

    if (cond.bad())
    {
        OFLOG_FATAL(dcm2imgLogger, cond.text() << ": reading file: " << opt_ifname);
        delete dfile;
        exitCode = 1;
        goto cleanup;
    }

    OFLOG_INFO(dcm2imgLogger, "preparing pixel data");

    dataset = dfile->getDataset();
    xfer = dataset->getOriginalXfer();

    Sint32 frameCount;
    if (dataset->findAndGetSint32(DCM_NumberOfFrames, frameCount).bad())
        frameCount = 1;
    if ((opt_frameCount == 0) || ((opt_frame == 1) && (opt_frameCount == OFstatic_cast(Uint32, frameCount))))
    {
        // since we process all frames anyway, decompress the complete pixel data (if required)
        opt_compatibilityMode |= CIF_DecompressCompletePixelData;
    }
    if ((frameCount > 1) && !(opt_compatibilityMode & CIF_DecompressCompletePixelData))
    {
        // use partial read access to pixel data (only in case of multiple frames, but not for all frames)
        opt_compatibilityMode |= CIF_UsePartialAccessToPixelData;
    }

    di = new DicomImage(dfile, xfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);
    if (di == NULL)
    {
        OFLOG_FATAL(dcm2imgLogger, "Out of memory");
        exitCode = 1;
        goto cleanup;
    }

    if (di->getStatus() != EIS_Normal)
    {
        OFLOG_FATAL(dcm2imgLogger, DicomImage::getString(di->getStatus()));
        exitCode = 1;
        goto cleanup;
    }

    /* create & set display function */
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
            if (opt_minDensity >= 0)
                disp->setMinDensityValue(opt_minDensity);
            if (opt_maxDensity >= 0)
                disp->setMaxDensityValue(opt_maxDensity);
            if ((di != NULL) && (disp->isValid()))
            {
                OFLOG_INFO(dcm2imgLogger, "activating "
                    << ((opt_displayFunction == 1) ? "CIELAB" : "GSDF")
                    << " display function for "
                    << ((deviceType == DiDisplayFunction::EDT_Monitor) ? "softcopy" : "hardcopy")
                    << " devices");
                if (!di->setDisplayFunction(disp))
                    OFLOG_WARN(dcm2imgLogger, "cannot select display function");
            }
        }
    }

    if (opt_imageInfo)
    {
        dumpImageParameters(xfer, di, dataset, opt_ifname);
    }

    if (!opt_suppressOutput)
    {
        /* try to select frame */
        if (opt_frame != di->getFirstFrame() + 1)
        {
            OFLOG_FATAL(dcm2imgLogger, "cannot select frame " << opt_frame << ", invalid frame number");
            exitCode = 1;
            goto cleanup;
        }

        /* convert to grayscale if image is not monochrome */
        if ((opt_convertToGrayscale) && (!di->isMonochrome()))
        {
            di = convertToGrayscale(di);
            if (di == NULL)
            {
                exitCode = 1;
                goto cleanup;
            }
        }

        /* process overlay parameters */
        processOverlayParameters(di, opt_Overlay, opt_OverlayMode, opt_foregroundDensity, opt_thresholdDensity);

        /* process VOI parameters */
        int result = processVOIParameters(di, opt_windowType, opt_windowParameter,
            opt_windowCenter, opt_windowWidth, opt_voiFunction, opt_ignoreVoiLutDepth,
            opt_roiLeft, opt_roiTop, opt_roiWidth, opt_roiHeight);
        if (result)
        {
            exitCode = result;
            goto cleanup;
        }

        /* process presentation LUT parameters */
        processPLUTParameters(di, opt_presShape);

        /* change polarity */
        if (opt_changePolarity)
        {
            OFLOG_INFO(dcm2imgLogger, "setting polarity to REVERSE");
            if (!di->setPolarity(EPP_Reverse))
                OFLOG_WARN(dcm2imgLogger, "cannot set polarity");
        }

        /* perform clipping */
        di = performClipping(di, opt_useClip, opt_scaleType, opt_left,
             opt_top, opt_width, opt_height);
        if (di == NULL)
        {
            exitCode = 1;
            goto cleanup;
        }

        /* perform rotation */
        performRotation(di, opt_rotateDegree);

        /* perform flipping */
        performFlip(di, opt_flipType);

        /* perform scaling */
        di = performScaling(di, opt_useClip, opt_scaleType, opt_left, opt_top,
             opt_width, opt_height, opt_scale_factor, opt_scale_size,
             opt_useInterpolation, opt_useAspectRatio);
        if (di == NULL)
        {
            exitCode = 1;
            goto cleanup;
        }

        /* write selected frame(s) to file */

        FILE *ofile = NULL;
        OFString ofname;

        /* determine default file extension */
        const char *ofext = getDefaultExtension(di, opt_fileType);

        unsigned int fcount = OFstatic_cast(unsigned int, ((opt_frameCount > 0) && (opt_frameCount <= di->getFrameCount())) ? opt_frameCount : di->getFrameCount());
        if (fcount < opt_frameCount)
        {
            OFLOG_WARN(dcm2imgLogger, "cannot select " << opt_frameCount << " frames, limiting to "
                << fcount << " frames");
        }

        for (unsigned int frame = 0; frame < fcount; frame++)
        {
            if (opt_ofname)
            {
                /* output to file */
                if (opt_multiFrame)
                {
                    OFOStringStream stream;
                    /* generate output filename */
                    stream << opt_ofname << ".";
                    if (opt_useFrameNumber)
                        stream << "f" << (opt_frame + frame);
                    else
                        stream << frame;
                    stream << "." << ofext << OFStringStream_ends;
                    /* convert string stream into a character string */
                    OFSTRINGSTREAM_GETSTR(stream, buffer_str)
                    ofname.assign(buffer_str);
                    OFSTRINGSTREAM_FREESTR(buffer_str)
                }
                else
                {
                    ofname.assign(opt_ofname);
                }

                OFLOG_INFO(dcm2imgLogger, "writing frame " << (opt_frame + frame) << " to " << ofname);
                ofile = fopen(ofname.c_str(), "wb");
                if (ofile == NULL)
                {
                    OFLOG_FATAL(dcm2imgLogger, "cannot create file " << ofname);
                    exitCode = 1;
                    goto cleanup;
                }
            } else {
                /* output to stdout */
                ofile = stdout;
#ifdef _WIN32
                /* switch stdout to binary mode to prevent LF to CR/LF conversion */
                setmode(fileno(stdout), O_BINARY);
#endif
                OFLOG_INFO(dcm2imgLogger, "writing frame " << (opt_frame + frame) << " to stdout");
            }

            /* finally create output image file */

            switch (opt_fileType)
            {
                case EFT_RawPNM:
                    OFLOG_INFO(dcm2imgLogger, "writing 8-bit binary PGM/PPM");
                    result = di->writeRawPPM(ofile, 8, frame);
                    break;
                case EFT_8bitPNM:
                    OFLOG_INFO(dcm2imgLogger, "writing 8-bit ASCII PGM/PPM");
                    result = di->writePPM(ofile, 8, frame);
                    break;
                case EFT_16bitPNM:
                    OFLOG_INFO(dcm2imgLogger, "writing 16-bit ASCII PGM/PPM");
                    result = di->writePPM(ofile, 16, frame);
                    break;
                case EFT_NbitPNM:
                    OFLOG_INFO(dcm2imgLogger, "writing " << opt_fileBits << "-bit ASCII PGM/PPM");
                    result = di->writePPM(ofile, OFstatic_cast(int, opt_fileBits), frame);
                    break;
                case EFT_BMP:
                    OFLOG_INFO(dcm2imgLogger, (di->isMonochrome() ? "writing 8-bit monochrome BMP" : "writing 24-bit color BMP"));
                    result = di->writeBMP(ofile, 0, frame);
                    break;
                case EFT_8bitBMP:
                    OFLOG_INFO(dcm2imgLogger, "writing 8-bit palette BMP");
                    result = di->writeBMP(ofile, 8, frame);
                    break;
                case EFT_24bitBMP:
                    OFLOG_INFO(dcm2imgLogger, "writing 24-bit truecolor BMP");
                    result = di->writeBMP(ofile, 24, frame);
                    break;
                case EFT_32bitBMP:
                    OFLOG_INFO(dcm2imgLogger, "writing 32-bit truecolor BMP");
                    result = di->writeBMP(ofile, 32, frame);
                    break;
                case EFT_JPEG:
                    {
                        OFLOG_INFO(dcm2imgLogger, "writing 8-bit lossy JPEG (baseline)");
                        /* initialize JPEG plugin */
                        DiJPEGPlugin plugin;
                        plugin.setQuality(OFstatic_cast(unsigned int, opt_quality));
                        plugin.setSampling(opt_sampling);
                        result = di->writePluginFormat(&plugin, ofile, frame);
                    }
                    break;
                case EFT_JPLS:
                    {
                        OFLOG_INFO(dcm2imgLogger, (opt_true_lossless ? "writing JPEG-LS (raw)" : "writing JPEG-LS (rendered)"));
                        /* initialize JPEG-LS plugin */
                        DiJPLSPlugin plugin;
                        plugin.setTrueLosslessMode(opt_true_lossless);
                        plugin.setForce8BitMode(opt_force_8_bit);
                        result = di->writePluginFormat(&plugin, ofile, frame);
                    }
                    break;
#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
                case EFT_JP2K_CS:
#ifdef WITH_OPENJPEG
                case EFT_JP2K_FILE:
                        if (opt_fileType == EFT_JP2K_FILE) OFLOG_INFO(dcm2imgLogger, (opt_true_lossless ? "writing JPEG 2000 file (raw)" : "writing JPEG 2000 file (rendered)"));
                            else OFLOG_INFO(dcm2imgLogger, (opt_true_lossless ? "writing JPEG 2000 code stream (raw)" : "writing JPEG 2000 code stream (rendered)"));
#else /* WITH_OPENJPEG */
                        OFLOG_INFO(dcm2imgLogger, (opt_true_lossless ? "writing JPEG 2000 code stream (raw)" : "writing JPEG 2000 code stream (rendered)"));
#endif /* WITH_OPENJPEG */
                    {
                        /* initialize JPEG 2000 plugin */
                        DiJP2000Plugin plugin;
                        plugin.setLossyMode(opt_lossy_jp2k);
                        plugin.setTrueLosslessMode(opt_true_lossless);
                        plugin.setForce8BitMode(opt_force_8_bit);
                        plugin.setCompressionRatio(opt_compression_ratio);
                        plugin.setCompressedSize(opt_compressed_size);
                        plugin.setTileSizeX(opt_tile_size_x);
                        plugin.setTileSizeY(opt_tile_size_y);
                        plugin.setLevels(opt_levels);
                        plugin.setProgressionOrder(opt_progression);
#ifdef WITH_OPENJPEG
                        plugin.useFileFormat(opt_fileType == EFT_JP2K_FILE);
                        plugin.useOpenJPEG(opt_use_openjpeg);
#endif /* WITH_OPENJPEG */
                        result = di->writePluginFormat(&plugin, ofile, frame);
                    }
                    break;
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */
#ifdef WITH_LIBTIFF
                case EFT_TIFF:
                    {
                        OFLOG_INFO(dcm2imgLogger, (di->isMonochrome() ? "writing 8-bit monochrome TIFF" : "writing 24-bit color TIFF"));
                        /* initialize TIFF plugin */
                        DiTIFFPlugin tiffPlugin;
                        tiffPlugin.setCompressionType(opt_tiffCompression);
                        tiffPlugin.setLZWPredictor(opt_lzwPredictor);
                        tiffPlugin.setRowsPerStrip(OFstatic_cast(unsigned long, opt_rowsPerStrip));
                        result = di->writePluginFormat(&tiffPlugin, ofile, frame);
                    }
                    break;
#endif
#ifdef WITH_LIBPNG
                case EFT_PNG:
                case EFT_16bitPNG:
                    {
                        if (opt_fileType == EFT_PNG) OFLOG_INFO(dcm2imgLogger, (di->isMonochrome() ? "writing 8-bit monochrome PNG" : "writing 24-bit color PNG"));
                            else OFLOG_INFO(dcm2imgLogger, (di->isMonochrome() ? "writing 16-bit monochrome PNG" : "writing 48-bit color PNG"));
                        /* initialize PNG plugin */
                        DiPNGPlugin pngPlugin;
                        pngPlugin.setInterlaceType(opt_interlace);
                        pngPlugin.setMetainfoType(opt_metainfo);
                        if (opt_fileType == EFT_16bitPNG)
                            pngPlugin.setBitsPerSample(16);
                        result = di->writePluginFormat(&pngPlugin, ofile, frame);
                    }
                    break;
#endif
#ifdef PASTEL_COLOR_OUTPUT
                case EFT_PastelPNM:
                    OFLOG_INFO(dcm2imgLogger, "writing 8-bit binary PPM with pastel colors");
                    result = di->writePPM(ofile, MI_PastelColor, frame);
                    break;
#endif
                default:
                    if (opt_ofname)
                    {
                        OFLOG_INFO(dcm2imgLogger, (di->isMonochrome() ? "writing 8-bit monochrome BMP" : "writing 24-bit color BMP"));
                        result = di->writeBMP(ofile, 0, frame);
                    }
                    else /* stdout */
                    {
                        OFLOG_INFO(dcm2imgLogger, "writing 8-bit ASCII PGM/PPM to stdout");
                        result = di->writePPM(ofile, 8, frame);
                    }
                    break;
            }

            if (opt_ofname)
            {
                if (fclose(ofile))
                {
                    OFLOG_FATAL(dcm2imgLogger, "error while closing file, content may be incomplete");
                    exitCode = 1;
                    goto cleanup;
                }
            }
            if (!result)
            {
                OFLOG_FATAL(dcm2imgLogger, "cannot write frame");
                exitCode = 1;
                goto cleanup;
            }
        }
    }

    /* done, now cleanup. */
    OFLOG_INFO(dcm2imgLogger, "cleaning up memory");

  cleanup:

    delete di;
    delete disp;

    // deregister RLE decompression codec
    DcmRLEDecoderRegistration::cleanup();

    // deregister JPEG decompression codecs
    DJDecoderRegistration::cleanup();

    // deregister JPEG-LS decompression codecs
    DJLSDecoderRegistration::cleanup();

#ifdef BUILD_DCM2IMG_AS_DCM2KIMG
#ifdef WITH_OPENJPEG
    if (opt_use_openjpeg) O2DecoderRegistration::cleanup();
        else D2DecoderRegistration::cleanup();
#else /* WITH_OPENJPEG */
    D2DecoderRegistration::cleanup();
#endif /* WITH_OPENJPEG */
#endif /* BUILD_DCM2IMG_AS_DCM2KIMG */

    return exitCode;
}
