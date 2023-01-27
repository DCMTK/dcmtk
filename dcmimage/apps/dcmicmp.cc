/*
 *
 *  Copyright (C) 2018-2023, OFFIS e.V.
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
 *  Purpose: Compare DICOM images and compute difference metrics
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmimage/dcmicmph.h"     /* for DicomImageComparison */
#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcmtk/dcmdata/dcuid.h"         /* for dcmtk version name */
#include "dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */
#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */
#include "dcmtk/oflog/oflog.h"

#ifdef WITH_ZLIB
#include <zlib.h>                        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmicmp"

static OFLogger dcmicmpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static const char *consoleDescription = "Compare DICOM images and compute difference metrics";

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 4
#define LONGCOL 20

// processing errors (80-99)
#define EXITCODE_INITIALIZE_DIFF_IMAGE           80
#define EXITCODE_DISPLAY_PIPELINE                81
#define EXITCODE_IMAGE_COMPARISON                82
#define EXITCODE_LIMIT_EXCEEDED_MAX_ERROR        90
#define EXITCODE_LIMIT_EXCEEDED_MAE              91
#define EXITCODE_LIMIT_EXCEEDED_RMSE             92
#define EXITCODE_LIMIT_EXCEEDED_PSNR             93
#define EXITCODE_LIMIT_EXCEEDED_SNR              94

// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, consoleDescription, rcsid);
    OFCommandLine cmd;

    E_FileReadMode      opt_readMode = ERM_autoDetect;    /* default: fileformat or dataset */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */
    unsigned long       opt_compatibilityMode = 0;
    EW_WindowType       opt_windowType = EWT_none;        /* default: no windowing */
    OFCmdUnsignedInt    opt_windowParameter = 0;
    OFCmdFloat          opt_windowCenter = 0.0, opt_windowWidth = 0.0;
    OFBool              opt_sharedWindow = OFFalse;
    EF_VoiLutFunction   opt_voiFunction = EFV_Default;
    ES_PresentationLut  opt_presShape = ESP_Default;
    const char *        opt_ifname1 = NULL;
    const char *        opt_ifname2 = NULL;
    const char *        opt_ofname = NULL;

    OFBool              opt_checkError = OFFalse;
    OFBool              opt_checkMAE = OFFalse;
    OFBool              opt_checkRMSE = OFFalse;
    OFBool              opt_checkPSNR = OFFalse;
    OFBool              opt_checkSNR = OFFalse;
    OFCmdUnsignedInt    opt_checkErrorLimit = 0;
    OFCmdFloat          opt_checkMAELimit = 0.0;
    OFCmdFloat          opt_checkRMSELimit = 0.0;
    OFCmdFloat          opt_checkPSNRLimit = 0.0;
    OFCmdFloat          opt_checkSNRLimit = 0.0;
    OFCmdFloat          opt_amplify = 1.0;

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in-1",  "Reference DICOM image file for comparison");
    cmd.addParam("dcmfile-in-2",  "Test DICOM image file for comparison\n(\"-\" for stdin)");

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

     cmd.addSubGroup("modality LUT transformation:");
      cmd.addOption("--use-modality",       "+M",      "use modality LUT transformation (default)");
      cmd.addOption("--no-modality",        "-M",      "ignore stored modality LUT transformation");

     cmd.addSubGroup("VOI LUT transformation:");
      cmd.addOption("--no-windowing",       "-W",      "no VOI windowing (default)");
      cmd.addOption("--use-window",         "+Wi",  1, "[n]umber: integer",
                                                       "use the n-th VOI window from image file");
      cmd.addOption("--use-voi-lut",        "+Wl",  1, "[n]umber: integer",
                                                       "use the n-th VOI look up table from image file");
      cmd.addOption("--min-max-window",     "+Wm",     "compute VOI window using min-max algorithm\non both images separately");
      cmd.addOption("--min-max-window-n",   "+Wn",     "compute VOI window using min-max algorithm\non both images separately, ignoring extremes");
      cmd.addOption("--min-max-ref",        "+Wr",     "compute VOI window using min-max algorithm\nand use same window for the test image");
      cmd.addOption("--min-max-n-ref",      "+Wq",     "compute VOI window using min-max algorithm,\nignoring extreme values\nand use same window for the test image");
      cmd.addOption("--set-window",         "+Ww",  2, "[c]enter [w]idth: float",
                                                       "compute VOI window using center c and width w");
      cmd.addOption("--linear-function",    "+Wfl",    "set VOI LUT function to LINEAR");
      cmd.addOption("--sigmoid-function",   "+Wfs",    "set VOI LUT function to SIGMOID");

     cmd.addSubGroup("presentation LUT transformation:");
      cmd.addOption("--identity-shape",     "+Pid",    "set presentation LUT shape to IDENTITY");
      cmd.addOption("--inverse-shape",      "+Piv",    "set presentation LUT shape to INVERSE");
      cmd.addOption("--lin-od-shape",       "+Pod",    "set presentation LUT shape to LIN OD");

    cmd.addGroup("image comparison metrics options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--check-error",        "+ce",  1, "[l]imit: integer",
                                                       "check if max absolute error <= limit");
      cmd.addOption("--check-mae",          "+cm",  1, "[l]imit: float",
                                                       "check if mean absolute error <= limit");
      cmd.addOption("--check-rmse",         "+cr",  1, "[l]imit: float",
                                                       "check if root mean square error <= limit");
      cmd.addOption("--check-psnr",         "+cp",  1, "[l]imit: float",
                                                       "check if PSNR >= limit");
      cmd.addOption("--check-snr",          "+cs",  1, "[l]imit: float",
                                                       "check if SNR >= limit");

    cmd.addGroup("output options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--save-diff",          "+sd",  1, "[f]ilename: string",
                                                       "write secondary capture difference image");
      cmd.addOption("--amplify",            "+a",   1, "[f]actor: float",
                                                       "multiply diff image pixel values by f");

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
#else
                COUT << " none" << OFendl;
#endif
                return EXITCODE_NO_ERROR;
            }
        }
    }

    /* command line parameters */

    cmd.getParam(1, opt_ifname1);
    cmd.getParam(2, opt_ifname2);

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
        opt_windowType = EWT_none;
    if (cmd.findOption("--use-window"))
    {
        opt_windowType = EWT_window_from_file;
        app.checkValue(cmd.getValueAndCheckMin(opt_windowParameter, 1));
    }
    if (cmd.findOption("--use-voi-lut"))
    {
        opt_windowType = EWT_voi_lut_from_file;
        app.checkValue(cmd.getValueAndCheckMin(opt_windowParameter, 1));
    }
    if (cmd.findOption("--min-max-window"))
    {
        opt_windowType = EWT_window_minmax;
        opt_sharedWindow = OFFalse;
    }
    if (cmd.findOption("--min-max-window-n"))
    {
        opt_windowType = EWT_window_minmax_n;
        opt_sharedWindow = OFFalse;
    }

    if (cmd.findOption("--min-max-ref"))
    {
        opt_windowType = EWT_window_minmax;
        opt_sharedWindow = OFTrue;
    }
    if (cmd.findOption("--min-max-n-ref"))
    {
        opt_windowType = EWT_window_minmax_n;
        opt_sharedWindow = OFTrue;
    }

    if (cmd.findOption("--set-window"))
    {
        opt_windowType = EWT_window_center_width;
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

    /* image comparison metrics options */

    if (cmd.findOption("--check-error"))
    {
        opt_checkError = OFTrue;
        app.checkValue(cmd.getValueAndCheckMinMax(opt_checkErrorLimit, 0, 65535));
    }
    if (cmd.findOption("--check-mae"))
    {
        opt_checkMAE = OFTrue;
        app.checkValue(cmd.getValueAndCheckMin(opt_checkMAELimit, 0.0));
    }
    if (cmd.findOption("--check-rmse"))
    {
        opt_checkRMSE = OFTrue;
        app.checkValue(cmd.getValueAndCheckMin(opt_checkRMSELimit, 0.0));
    }
    if (cmd.findOption("--check-psnr"))
    {
        opt_checkPSNR = OFTrue;
        app.checkValue(cmd.getValueAndCheckMin(opt_checkPSNRLimit, 0.0));
    }
    if (cmd.findOption("--check-snr"))
    {
        opt_checkSNR = OFTrue;
        app.checkValue(cmd.getValueAndCheckMin(opt_checkSNRLimit, 0.0));
    }

    /* output options */

    if (cmd.findOption("--save-diff"))
    {
        app.checkValue(cmd.getValue(opt_ofname));
    }

    if (cmd.findOption("--amplify"))
    {
        app.checkDependence("--amplify", "--save-diff", (opt_ofname != NULL));
        app.checkValue(cmd.getValueAndCheckMinMax(opt_amplify, 1.0, 65535.0));
    }


    /* print resource identifier */
    OFLOG_DEBUG(dcmicmpLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmicmpLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    DicomImageComparison dicmp;
    OFCondition cond;

    if (opt_ofname)
    {
        // prepare the creation of a difference image file
        cond = dicmp.initializeDiffImage(opt_amplify);
        if (cond.bad())
        {
            OFLOG_FATAL(dcmicmpLogger, "Error while initializing difference image: " << cond.text());
            return EXITCODE_INITIALIZE_DIFF_IMAGE;
        }
    }

    // read the reference image
    cond = dicmp.readReferenceImage(opt_ifname1, opt_transferSyntax, opt_readMode, opt_compatibilityMode);
    if (cond.bad())
    {
        OFLOG_FATAL(dcmicmpLogger, "Error while loading reference image: " << cond.text());
        return EXITCODE_INVALID_INPUT_FILE;
    }

    // read the test image
    cond = dicmp.readTestImage(opt_ifname2, opt_transferSyntax, opt_readMode, opt_compatibilityMode);
    if (cond.bad())
    {
        OFLOG_FATAL(dcmicmpLogger, "Error while loading test image: " << cond.text());
        return EXITCODE_INVALID_INPUT_FILE;
    }

    // configure the display pipelines for both images with the same parameter set
    cond = dicmp.configureImages(opt_windowType, opt_sharedWindow, opt_windowParameter, opt_windowCenter, opt_windowWidth, opt_voiFunction, opt_presShape);
    if (cond.bad())
    {
        OFLOG_FATAL(dcmicmpLogger, "Error while configuring display pipeline: " << cond.text());
        return EXITCODE_DISPLAY_PIPELINE;
    }

    // compute image comparison metrics
    cond = dicmp.computeImageComparisonMetrics(opt_windowType);
    if (cond.bad())
    {
        OFLOG_FATAL(dcmicmpLogger, "Error while performing image comparison: " << cond.text());
        return EXITCODE_IMAGE_COMPARISON;
    }

    // print image comparison metrics
    COUT << "Max Absolute Error                     = " << dicmp.getMaxAbsoluteError() << "\n"
         << "Mean Absolute Error (MAE)              = " << dicmp.getMeanAbsoluteError() << "\n"
         << "Root Mean Square Error (RMSE)          = " << dicmp.getRootMeanSquareError() << "\n"
         << "Peak Signal to Noise Ratio (PSNR) [dB] = " << dicmp.getPeakSignalToNoiseRatio() << "\n"
         << "Signal to Noise Ratio (SNR) [dB]       = " << dicmp.getSignalToNoiseRatio() << OFendl;

    if (opt_ofname)
    {
        // save difference image
        cond = dicmp.saveDiffImage(opt_ofname);
        if (cond.bad())
        {
            OFLOG_FATAL(dcmicmpLogger, "Error while writing difference image: " << cond.text());
            return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
        }
    }

    // process --check options
    if (opt_checkError && (dicmp.getMaxAbsoluteError() > opt_checkErrorLimit))
    {
        OFLOG_FATAL(dcmicmpLogger, "Maximum absolute error " << dicmp.getMaxAbsoluteError() << " exceeds limit " << opt_checkErrorLimit);
        return EXITCODE_LIMIT_EXCEEDED_MAX_ERROR;
    }

    if (opt_checkMAE && (dicmp.getMeanAbsoluteError() > opt_checkMAELimit))
    {
        OFLOG_FATAL(dcmicmpLogger, "Mean absolute error " << dicmp.getMeanAbsoluteError() << " exceeds limit " << opt_checkMAELimit);
        return EXITCODE_LIMIT_EXCEEDED_MAE;
    }

    if (opt_checkRMSE && (dicmp.getRootMeanSquareError() > opt_checkRMSELimit))
    {
        OFLOG_FATAL(dcmicmpLogger, "Root mean square error (RMSE) " << dicmp.getRootMeanSquareError() << " exceeds limit " << opt_checkRMSELimit);
        return EXITCODE_LIMIT_EXCEEDED_RMSE;
    }

    if (opt_checkPSNR && (dicmp.getPeakSignalToNoiseRatio() < opt_checkPSNRLimit))
    {
        OFLOG_FATAL(dcmicmpLogger, "Peak signal to noise ratio (PSNR) " << dicmp.getPeakSignalToNoiseRatio() << " below limit " << opt_checkPSNRLimit);
        return EXITCODE_LIMIT_EXCEEDED_PSNR;
    }

    if (opt_checkSNR && (dicmp.getSignalToNoiseRatio() < opt_checkSNRLimit))
    {
        OFLOG_FATAL(dcmicmpLogger, "Signal to noise ratio (SNR) " << dicmp.getSignalToNoiseRatio() << " below limit " << opt_checkSNRLimit);
        return EXITCODE_LIMIT_EXCEEDED_SNR;
    }

    // done
    return EXITCODE_NO_ERROR;
}
