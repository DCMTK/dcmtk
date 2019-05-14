/*
 *
 *  Copyright (C) 1999-2019, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose
 *    sample application that reads multiple images and (optionally)
 *    presentation states and creates a print job consisting of
 *    stored print and hardcopy grayscale images.
 *    Non-grayscale transformations in the presentation state are ignored.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmpstat/dviface.h"
#include "dcmtk/dcmpstat/dvpssp.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/ofstd/oflist.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmpsprt"

static OFLogger dcmpsprtLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


static int addOverlay(const char *filename,
                      unsigned long xpos,
                      unsigned long ypos,
                      Uint16 *pixel,
                      unsigned long width,
                      unsigned long height,
                      unsigned int gray)
{
    if ((filename != NULL) && (pixel != NULL))
    {
#if defined(HAVE_IOS_NOCREATE) && !defined(HAVE_CXX11)
        STD_NAMESPACE ifstream input(filename, STD_NAMESPACE ios::in | STD_NAMESPACE ios::nocreate);
#else
        STD_NAMESPACE ifstream input(filename);
#endif
        if (input)
        {
            char c;
            unsigned int xsize, ysize;
            if (input.get(c) && (c == 'P') && input.get(c) && (c == '1'))
            {
                /* still need to add code for skipping comments in PBM file */
                input >> xsize;
                input >> ysize;
                if ((xpos + xsize <= width) && (ypos + ysize <= height))
                {
                    unsigned int value;
                    Uint16 *p = pixel + (ypos * width) + xpos;
                    for (unsigned long ys = 0; ys < ysize; ys++)
                    {
                        for (unsigned long xs = 0; xs < xsize; xs++)
                        {
                            while (input.get(c) && !isdigit(OFstatic_cast(unsigned char, c)));  // skip non-numeric chars
                            input.putback(c);
                            input >> value;
                            if (value)
                                *p = gray;
                            p++;
                        }
                        p += (width - xsize);
                    }
                    return 1;
                } else
                    OFLOG_ERROR(dcmpsprtLogger, "invalid position for overlay PBM file '" << filename);
            } else
                OFLOG_ERROR(dcmpsprtLogger, "overlay PBM file '" << filename << "' has no magic number P1");
        } else
            OFLOG_ERROR(dcmpsprtLogger, "can't open overlay PBM file '" << filename << "'");
    }
    return 0;
}


#define SHORTCOL 2
#define LONGCOL 21

int main(int argc, char *argv[])
{
    const char *              opt_printerID = NULL;             /* printer ID */
    const char *              opt_cfgName = NULL;               /* config read file name */
    DVPSFilmOrientation       opt_filmorientation = DVPSF_default;
    DVPSTrimMode              opt_trim = DVPSH_default;
    DVPSDecimateCropBehaviour opt_decimate = DVPSI_default;
    OFCmdUnsignedInt          opt_columns = 1;
    OFCmdUnsignedInt          opt_rows = 1;
    OFCmdUnsignedInt          opt_copies = 0;
    OFCmdUnsignedInt          opt_ovl_graylevel = 4095;
    const char *              opt_filmsize = NULL;
    const char *              opt_magnification = NULL;
    const char *              opt_smoothing = NULL;
    const char *              opt_configuration = NULL;
    const char *              opt_img_polarity = NULL;
    const char *              opt_img_request_size = NULL;
    const char *              opt_img_magnification = NULL;
    const char *              opt_img_smoothing = NULL;
    const char *              opt_img_configuration = NULL;
    const char *              opt_resolution = NULL;
    const char *              opt_border = NULL;
    const char *              opt_emptyimage = NULL;
    const char *              opt_maxdensity = NULL;
    const char *              opt_mindensity = NULL;
    const char *              opt_plutname = NULL;
    OFList<const char *>      opt_filenames;
    int                       opt_LUTshape = 0; // 0=use SCP default, 1=IDENTITY, 2=LIN OD.
    OFBool                    opt_inverse_plut = OFFalse;
    OFBool                    opt_spool = OFFalse;
    const char *              opt_mediumtype = NULL;
    const char *              opt_destination = NULL;
    const char *              opt_sessionlabel = NULL;
    const char *              opt_priority = NULL;
    const char *              opt_ownerID = NULL;

    OFBool                    opt_annotation = OFFalse;
    OFBool                    opt_annotationDatetime = OFTrue;
    OFBool                    opt_annotationPrinter = OFTrue;
    OFBool                    opt_annotationIllumination = OFTrue;
    const char *              opt_annotationString = NULL;

    OFCmdUnsignedInt          opt_illumination = (OFCmdUnsignedInt)-1;
    OFCmdUnsignedInt          opt_reflection = (OFCmdUnsignedInt)-1;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Read DICOM images and presentation states and render print job", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL + 2);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in", "DICOM image file(s) to be printed", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:");
     cmd.addOption("--help",              "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                     "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("processing options:");
     cmd.addOption("--pstate",            "+p",  1, "[p]state file: string",
                                                    "render the following image with pres. state p\n(this option can be specified multiple times)", OFCommandLine::AF_NoWarning);
     cmd.addOption("--config",            "-c",  1, "[f]ilename: string",
                                                    "process using settings from configuration file f");
     cmd.addOption("--printer",           "-p",  1, "[n]ame: string (default: 1st printer in cfg file)",
                                                    "select printer with identifier n from cfg file");

    cmd.addGroup("spooling options:");
     cmd.addOption("--spool",             "-s",     "spool print job to DICOM printer");
     cmd.addOption("--nospool",                     "do not spool print job to DICOM printer (default)");

    cmd.addGroup("film orientation options:");
     cmd.addOption("--portrait",                    "set portrait orientation");
     cmd.addOption("--landscape",                   "set landscape orientation");
     cmd.addOption("--default-orientation",         "use printer default (default)");

    cmd.addGroup("trim (border) options:");
     cmd.addOption("--trim",                        "set trim on");
     cmd.addOption("--no-trim",                     "set trim off");
     cmd.addOption("--default-trim",                "use printer default (default)");

    cmd.addGroup("requested decimate/crop behaviour options:");
     cmd.addOption("--request-decimate",            "request decimate");
     cmd.addOption("--request-crop",                "request crop");
     cmd.addOption("--request-fail",                "request failure");
     cmd.addOption("--default-request",             "use printer default (default)");

    cmd.addGroup("print presentation LUT options:");
     cmd.addOption("--default-plut",                "do not create presentation LUT (default)");
     cmd.addOption("--identity",                    "set IDENTITY presentation LUT shape");
     cmd.addOption("--lin-od",                      "set LIN OD presentation LUT shape");
     cmd.addOption("--plut",                     1, "[l]ut identifier: string",
                                                    "add LUT [l] to print job");
     cmd.addOption("--inverse-plut",                "render the inverse presentation LUT into the\nbitmap of the hardcopy grayscale image");
     cmd.addOption("--illumination",             1, "[v]alue: integer (0..65535)",
                                                    "set illumination to v (in cd/m^2)");
     cmd.addOption("--reflection",               1, "[v]alue: integer (0..65535)",
                                                    "set reflected ambient light to v (in cd/m^2)");

    cmd.addGroup("basic film session options (only with --spool):");
     cmd.addOption("--copies",                   1, "[v]alue: integer (1..100, default: 1)",
                                                    "set number of copies to v");
     cmd.addOption("--medium-type",              1, "[v]alue: string",
                                                    "set medium type to v");
     cmd.addOption("--destination",              1, "[v]alue: string",
                                                    "set film destination to v");
     cmd.addOption("--label",                    1, "[v]alue: string",
                                                    "set film session label to v");
     cmd.addOption("--priority",                 1, "[v]alue: string",
                                                    "set print priority to v");
     cmd.addOption("--owner",                    1, "[v]alue: string",
                                                    "set film session owner ID to v");

    cmd.addGroup("annotation options:");
     cmd.addOption("--no-annotation",               "do not create annotation (default)");
     cmd.addOption("--annotation",        "-a",  1, "[t]ext: string",
                                                    "create annotation with text t");
     cmd.addOption("--print-date",        "+pd",    "prepend date/time to annotation (default)");
     cmd.addOption("--print-no-date",     "-pd",    "do not prepend date/time to annotation");

     cmd.addOption("--print-name",        "+pn",    "prepend printer name to annotation (default)");
     cmd.addOption("--print-no-name",     "-pn",    "do not prepend printer name to annotation");

     cmd.addOption("--print-lighting",    "+pl",    "prepend illumination to annotation (default)");
     cmd.addOption("--print-no-lighting", "-pl",    "do not prepend illumination to annotation");

    cmd.addGroup("overlay options:");
     cmd.addOption("--overlay",           "+O" , 3, "[f]ilename: string, [x] [y]: integer",
                                                    "load overlay data from PBM file f and\ndisplay at position (x,y)");
     cmd.addOption("--ovl-graylevel",     "+Og", 1, "[v]alue: integer (0..4095)",
                                                    "use overlay gray level v (default: 4095 = white)");

    cmd.addGroup("other print options:");
     cmd.addOption("--layout",            "-l",  2, "[c]olumns [r]ows: integer (default: 1 1)",
                                                    "use 'STANDARD\\c,r' image display format");
     cmd.addOption("--filmsize",                 1, "[v]alue: string",
                                                    "set film size ID to v");
     cmd.addOption("--magnification",            1, "[v]alue: string",
                                                    "set magnification type to v");
     cmd.addOption("--smoothing",                1, "[v]alue: string",
                                                    "set smoothing type to v");
     cmd.addOption("--configinfo",               1, "[v]alue: string",
                                                    "set configuration information to v");
     cmd.addOption("--resolution",               1, "[v]alue: string",
                                                    "set requested resolution ID to v");
     cmd.addOption("--border",                   1, "[v]alue: string",
                                                    "set border density to v");
     cmd.addOption("--empty-image",              1, "[v]alue: string",
                                                    "set empty image density to v");
     cmd.addOption("--max-density",              1, "[v]alue: string",
                                                    "set max density to v");
     cmd.addOption("--min-density",              1, "[v]alue: string",
                                                    "set min density to v");
     cmd.addOption("--img-polarity",             1, "[v]alue: string",
                                                    "set image box polarity to v (NORMAL or REVERSE)");
     cmd.addOption("--img-request-size",         1, "[v]alue: string",
                                                    "set requested image size to v (width in mm)");
     cmd.addOption("--img-magnification",        1, "[v]alue: string",
                                                    "set image box magnification type to v");
     cmd.addOption("--img-smoothing",            1, "[v]alue: string",
                                                    "set image box smoothing type to v");
     cmd.addOption("--img-configinfo",           1, "[v]alue: string",
                                                    "set image box configuration information to v");

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
            COUT << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
            COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
            COUT << " none" << OFendl;
#endif
            return 0;
         }
      }

      /* options */
      OFLog::configureFromCommandLine(cmd, app);

      cmd.beginOptionBlock();
      if (cmd.findOption("--portrait"))  opt_filmorientation = DVPSF_portrait;
      if (cmd.findOption("--landscape")) opt_filmorientation = DVPSF_landscape;
      if (cmd.findOption("--default-orientation")) opt_filmorientation = DVPSF_default;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--trim"))         opt_trim = DVPSH_trim_on;
      if (cmd.findOption("--no-trim"))      opt_trim = DVPSH_trim_off;
      if (cmd.findOption("--default-trim")) opt_trim = DVPSH_default;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--request-decimate")) opt_decimate = DVPSI_decimate;
      if (cmd.findOption("--request-crop"))     opt_decimate = DVPSI_crop;
      if (cmd.findOption("--request-fail"))     opt_decimate = DVPSI_fail;
      if (cmd.findOption("--default-request"))  opt_decimate = DVPSI_default;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--default-plut")) opt_LUTshape = 0;
      if (cmd.findOption("--identity"))     opt_LUTshape = 1;
      if (cmd.findOption("--lin-od"))       opt_LUTshape = 2;
      if (cmd.findOption("--plut"))         app.checkValue(cmd.getValue(opt_plutname));
      cmd.endOptionBlock();
      if (cmd.findOption("--inverse-plut")) opt_inverse_plut = OFTrue;

      cmd.beginOptionBlock();
      if (cmd.findOption("--spool"))   opt_spool = OFTrue;
      if (cmd.findOption("--nospool")) opt_spool = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--no-annotation")) opt_annotation = OFFalse;
      if (cmd.findOption("--annotation"))
      {
        opt_annotation = OFTrue;
        app.checkValue(cmd.getValue(opt_annotationString));
      }
      cmd.endOptionBlock();

      cmd.findOption("--overlay", 0, OFCommandLine::FOM_First);      /* check at least once to avoid warnings */
      if (cmd.findOption("--ovl-graylevel"))
         app.checkValue(cmd.getValueAndCheckMinMax(opt_ovl_graylevel, 0, 4095));

      cmd.beginOptionBlock();
      if (cmd.findOption("--print-date"))    opt_annotationDatetime = OFTrue;
      if (cmd.findOption("--print-no-date")) opt_annotationDatetime = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--print-name"))    opt_annotationPrinter = OFTrue;
      if (cmd.findOption("--print-no-name")) opt_annotationPrinter = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--print-lighting"))    opt_annotationIllumination = OFTrue;
      if (cmd.findOption("--print-no-lighting")) opt_annotationIllumination = OFFalse;
      cmd.endOptionBlock();

      if (cmd.findOption("--filmsize"))          app.checkValue(cmd.getValue(opt_filmsize));
      if (cmd.findOption("--magnification"))     app.checkValue(cmd.getValue(opt_magnification));
      if (cmd.findOption("--smoothing"))         app.checkValue(cmd.getValue(opt_smoothing));
      if (cmd.findOption("--configinfo"))        app.checkValue(cmd.getValue(opt_configuration));
      if (cmd.findOption("--resolution"))        app.checkValue(cmd.getValue(opt_resolution));
      if (cmd.findOption("--border"))            app.checkValue(cmd.getValue(opt_border));
      if (cmd.findOption("--empty-image"))       app.checkValue(cmd.getValue(opt_emptyimage));
      if (cmd.findOption("--max-density"))       app.checkValue(cmd.getValue(opt_maxdensity));
      if (cmd.findOption("--min-density"))       app.checkValue(cmd.getValue(opt_mindensity));
      if (cmd.findOption("--config"))            app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--printer"))           app.checkValue(cmd.getValue(opt_printerID));
      if (cmd.findOption("--img-polarity"))      app.checkValue(cmd.getValue(opt_img_polarity));
      if (cmd.findOption("--img-request-size"))  app.checkValue(cmd.getValue(opt_img_request_size));
      if (cmd.findOption("--img-magnification")) app.checkValue(cmd.getValue(opt_img_magnification));
      if (cmd.findOption("--img-smoothing"))     app.checkValue(cmd.getValue(opt_img_smoothing));
      if (cmd.findOption("--img-configinfo"))    app.checkValue(cmd.getValue(opt_img_configuration));

      /* film session options */
      if (cmd.findOption("--medium-type"))
      {
        app.checkConflict("--medium-type", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_mediumtype));
      }
      if (cmd.findOption("--illumination"))
      {
        app.checkValue(cmd.getValueAndCheckMinMax(opt_illumination, 0, 65535));
      }
      if (cmd.findOption("--reflection"))
      {
        app.checkValue(cmd.getValueAndCheckMinMax(opt_reflection, 0, 65535));
      }

      if (cmd.findOption("--destination"))
      {
        app.checkConflict("--destination", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_destination));
      }
      if (cmd.findOption("--label"))
      {
        app.checkConflict("--label", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_sessionlabel));
      }
      if (cmd.findOption("--priority"))
      {
        app.checkConflict("--priority", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_priority));
      }
      if (cmd.findOption("--owner"))
      {
        app.checkConflict("--owner", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_ownerID));
      }
      if (cmd.findOption("--copies"))
      {
        app.checkConflict("--copies", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValueAndCheckMinMax(opt_copies, 1, 100));
      }

      if (cmd.findOption("--layout"))
      {
         app.checkValue(cmd.getValueAndCheckMin(opt_columns, 1));
         app.checkValue(cmd.getValueAndCheckMin(opt_rows, 1));
      }

      const char *imageFile = NULL;
      const char *pstateFile = NULL;
      int paramCount = cmd.getParamCount();
      for (int param = 1; param <= paramCount; param++)
      {
        cmd.getParam(param, imageFile);
        pstateFile = NULL;
        if (cmd.findOption("--pstate", -param)) app.checkValue(cmd.getValue(pstateFile));
        opt_filenames.push_back(imageFile);
        opt_filenames.push_back(pstateFile);
      }
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmpsprtLogger, rcsid << OFendl);

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        OFLOG_FATAL(dcmpsprtLogger, "can't open configuration file '" << opt_cfgName << "'");
        return 10;
      }
    }
    DVInterface dvi(opt_cfgName);

    if (opt_printerID && (EC_Normal != dvi.setCurrentPrinter(opt_printerID)))
      OFLOG_WARN(dcmpsprtLogger, "unable to select printer '" << opt_printerID << "', ignoring.");

    /* dump printer characteristics if requested */
    const char *currentPrinter = dvi.getCurrentPrinter();

    if ((opt_img_request_size) && (!dvi.getTargetPrinterSupportsRequestedImageSize(opt_printerID)))
      OFLOG_WARN(dcmpsprtLogger, "printer does not support requested image size");

    if (EC_Normal != dvi.getPrintHandler().setImageDisplayFormat(opt_columns, opt_rows))
      OFLOG_WARN(dcmpsprtLogger, "cannot set image display format to columns=" << opt_columns
           << ", rows=" << opt_rows << ", ignoring.");
    if ((opt_filmsize)&&(EC_Normal != dvi.getPrintHandler().setFilmSizeID(opt_filmsize)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film size ID to '" << opt_filmsize << "', ignoring.");
    if ((opt_magnification)&&(EC_Normal != dvi.getPrintHandler().setMagnificationType(opt_magnification)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set magnification type to '" << opt_magnification << "', ignoring.");
    if ((opt_smoothing)&&(EC_Normal != dvi.getPrintHandler().setSmoothingType(opt_smoothing)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set smoothing type to '" << opt_smoothing << "', ignoring.");
    if ((opt_configuration)&&(EC_Normal != dvi.getPrintHandler().setConfigurationInformation(opt_configuration)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set configuration information to '" << opt_configuration << "', ignoring.");
    if ((opt_resolution)&&(EC_Normal != dvi.getPrintHandler().setResolutionID(opt_resolution)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set requested resolution ID to '" << opt_resolution << "', ignoring.");
    if ((opt_border)&&(EC_Normal != dvi.getPrintHandler().setBorderDensity(opt_border)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set border density to '" << opt_border << "', ignoring.");
    if ((opt_emptyimage)&&(EC_Normal != dvi.getPrintHandler().setEmtpyImageDensity(opt_emptyimage)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set empty image density to '" << opt_emptyimage << "', ignoring.");
    if ((opt_maxdensity)&&(EC_Normal != dvi.getPrintHandler().setMaxDensity(opt_maxdensity)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set max density to '" << opt_maxdensity << "', ignoring.");
    if ((opt_mindensity)&&(EC_Normal != dvi.getPrintHandler().setMinDensity(opt_mindensity)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set min density to '" << opt_mindensity << "', ignoring.");
    if (EC_Normal != dvi.getPrintHandler().setFilmOrientation(opt_filmorientation))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film orientation, ignoring.");
    if (EC_Normal != dvi.getPrintHandler().setTrim(opt_trim))
      OFLOG_WARN(dcmpsprtLogger, "cannot set trim, ignoring.");
    if (EC_Normal != dvi.getPrintHandler().setRequestedDecimateCropBehaviour(opt_decimate))
      OFLOG_WARN(dcmpsprtLogger, "cannot set requested decimate/crop behaviour, ignoring.");
    if ((opt_illumination != (OFCmdUnsignedInt)-1)&&(EC_Normal != dvi.getPrintHandler().setPrintIllumination((Uint16)opt_illumination)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set illumination to '" << opt_illumination << "', ignoring.");
    if ((opt_reflection != (OFCmdUnsignedInt)-1)&&(EC_Normal != dvi.getPrintHandler().setPrintReflectedAmbientLight((Uint16)opt_reflection)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set reflected ambient light to '" << opt_reflection << "', ignoring.");

    if ((opt_copies > 0)&&(EC_Normal != dvi.setPrinterNumberOfCopies(opt_copies)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film session number of copies to '" << opt_copies << "', ignoring.");
    if ((opt_mediumtype)&&(EC_Normal != dvi.setPrinterMediumType(opt_mediumtype)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film session medium type to '" << opt_mediumtype << "', ignoring.");
    if ((opt_destination)&&(EC_Normal != dvi.setPrinterFilmDestination(opt_destination)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film destination to '" << opt_destination << "', ignoring.");
    if ((opt_sessionlabel)&&(EC_Normal != dvi.setPrinterFilmSessionLabel(opt_sessionlabel)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film session label to '" << opt_sessionlabel << "', ignoring.");
    if ((opt_priority)&&(EC_Normal != dvi.setPrinterPriority(opt_priority)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film session print priority to '" << opt_priority << "', ignoring.");
    if ((opt_ownerID)&&(EC_Normal != dvi.setPrinterOwnerID(opt_ownerID)))
      OFLOG_WARN(dcmpsprtLogger, "cannot set film session owner ID to '" << opt_ownerID << "', ignoring.");
    if ((opt_spool)&&(EC_Normal != dvi.startPrintSpooler()))
      OFLOG_WARN(dcmpsprtLogger, "unable to start print spooler, ignoring.");

    OFListIterator(const char *) first = opt_filenames.begin();
    OFListIterator(const char *) last = opt_filenames.end();
    const char *currentImage = NULL;
    const char *currentPState = NULL;
    OFCondition status = EC_Normal;
    void *pixelData = NULL;
    unsigned long width = 0;
    unsigned long height = 0;
    unsigned long bitmapSize = 0;
    double pixelAspectRatio;

    while ((EC_Normal == status) && (first != last))
    {
      currentImage = *first;
      ++first;
      if (first != last)
      {
        currentPState = *first;
        ++first;
        if (currentPState)
        {
          OFLOG_INFO(dcmpsprtLogger, "loading image file '" << currentImage << "' with presentation state '" << currentPState << "'");
          status = dvi.loadPState(currentPState, currentImage);
          if (EC_Normal != status)
          {
            OFLOG_FATAL(dcmpsprtLogger, "loading image file '" << currentImage << "' with presentation state '" << currentPState << "' failed.");
            return 10;
          }
        }
        else
        {
          OFLOG_INFO(dcmpsprtLogger, "loading image file '" << currentImage << "'");
          status = dvi.loadImage(currentImage);
          if (EC_Normal != status)
          {
            OFLOG_FATAL(dcmpsprtLogger, "loading image file '" << currentImage << "' failed.");
            return 10;
          }
        }

        if (opt_plutname)
        {
          if (EC_Normal != dvi.selectDisplayPresentationLUT(opt_plutname))
            OFLOG_WARN(dcmpsprtLogger, "cannot set requested presentation LUT '" << opt_plutname << "', ignoring.");
        } else {
          // in the case of a Presentation LUT Shape, we set the shape inside
          // the GSPS object to default (corresponding to IDENTITY for MONOCHROME2
          // and INVERSE for MONOCHROME1). This will leave our image data unaltered.
          // The LIN OD shape is only activated in the print handler, not the GSPS.
          if ((opt_LUTshape == 1) || (opt_LUTshape == 2))
          {
            if (dvi.getCurrentPState().setDefaultPresentationLUTShape().bad())
              OFLOG_WARN(dcmpsprtLogger, "cannot set presentation LUT shape, ignoring.");

            if (opt_LUTshape == 2)
            {
              if (dvi.getPrintHandler().setPresentationLUTShape(DVPSP_lin_od).bad())
                OFLOG_WARN(dcmpsprtLogger, "cannot set LIN OD presentation LUT shape, ignoring.");
            }
          }
        }

        // save grayscale hardcopy image.
        bitmapSize = dvi.getCurrentPState().getPrintBitmapSize();
        pixelData = new char[bitmapSize];
        if (pixelData)
        {
            if (EC_Normal != dvi.getCurrentPState().getPrintBitmapWidthHeight(width, height))
            {
              OFLOG_FATAL(dcmpsprtLogger, "can't determine bitmap size");
              return 10;
            }
            if (EC_Normal != dvi.getCurrentPState().getPrintBitmap(pixelData, bitmapSize, opt_inverse_plut))
            {
              OFLOG_FATAL(dcmpsprtLogger, "can't create print bitmap");
              return 10;
            }
            pixelAspectRatio = dvi.getCurrentPState().getPrintBitmapPixelAspectRatio();

            if (cmd.findOption("--overlay", 0, OFCommandLine::FOM_First))
            {
                do {
                    const char *fn = NULL;
                    OFCmdUnsignedInt x, y;
                    app.checkValue(cmd.getValue(fn));
                    app.checkValue(cmd.getValue(x));
                    app.checkValue(cmd.getValue(y));
                    if (fn != NULL)
                        addOverlay(fn, x, y, OFstatic_cast(Uint16 *, pixelData), width, height, OFstatic_cast(unsigned int, opt_ovl_graylevel));
                } while (cmd.findOption("--overlay", 0, OFCommandLine::FOM_Next));
            }

            OFLOG_INFO(dcmpsprtLogger, "writing DICOM grayscale hardcopy image to database.");
            if (EC_Normal != dvi.saveHardcopyGrayscaleImage(pixelData, width, height, pixelAspectRatio))
            {
              OFLOG_FATAL(dcmpsprtLogger, "error during creation of DICOM grayscale hardcopy image file");
              return 10;
            }
            delete[] OFstatic_cast(char *, pixelData);
        } else {
          OFLOG_FATAL(dcmpsprtLogger, "out of memory error: cannot allocate print bitmap");
          return 10;
        }
      } else {
        OFLOG_FATAL(dcmpsprtLogger, "internal error - odd number of filenames");
        return 10;
      }
    }

    // set annotations
    if (status == EC_Normal)
    {
      if (opt_annotation)
      {
        if (dvi.getTargetPrinterSupportsAnnotation(currentPrinter))
        {
          dvi.setActiveAnnotation(OFTrue);
          dvi.setPrependDateTime(opt_annotationDatetime);
          dvi.setPrependPrinterName(opt_annotationPrinter);
          dvi.setPrependLighting(opt_annotationIllumination);
          dvi.setAnnotationText(opt_annotationString);
        } else {
          OFLOG_WARN(dcmpsprtLogger, "printer '" << currentPrinter << "' does not support annotations, ignoring.");
          dvi.setActiveAnnotation(OFFalse);
        }
      } else {
        dvi.setActiveAnnotation(OFFalse);
      }
    }

    if (status == EC_Normal)
    {
      size_t numImages = dvi.getPrintHandler().getNumberOfImages();
      for (size_t i=0; i<numImages; i++)
      {
        if ((opt_img_polarity)&&(EC_Normal != dvi.getPrintHandler().setImagePolarity(i, opt_img_polarity)))
          OFLOG_WARN(dcmpsprtLogger, "cannot set polarity for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_polarity << "', ignoring.");
        if ((opt_img_request_size)&&(EC_Normal != dvi.getPrintHandler().setImageRequestedSize(i, opt_img_request_size)))
          OFLOG_WARN(dcmpsprtLogger, "cannot set requested size for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_request_size << "', ignoring.");
        if ((opt_img_magnification)&&(EC_Normal != dvi.getPrintHandler().setImageMagnificationType(i, opt_img_magnification)))
          OFLOG_WARN(dcmpsprtLogger, "cannot set magnification type for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_magnification << "', ignoring.");
        if ((opt_img_smoothing)&&(EC_Normal != dvi.getPrintHandler().setImageSmoothingType(i, opt_img_smoothing)))
          OFLOG_WARN(dcmpsprtLogger, "cannot set smoothing type for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_smoothing << "', ignoring.");
        if ((opt_img_configuration)&&(EC_Normal != dvi.getPrintHandler().setImageConfigurationInformation(i, opt_img_configuration)))
          OFLOG_WARN(dcmpsprtLogger, "cannot set configuration information for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_configuration << "', ignoring.");
      }
      if ((numImages > 0)&&(! opt_spool))
      {
        // no need to do this manually if we are spooling - spoolPrintJob() will do this anyway.
        OFLOG_WARN(dcmpsprtLogger, "writing DICOM stored print object to database.");
        if (EC_Normal != dvi.saveStoredPrint(dvi.getTargetPrinterSupportsRequestedImageSize(opt_printerID)))
        {
          OFLOG_ERROR(dcmpsprtLogger, "error during creation of DICOM stored print object");
        }
      }
    }

    if ((status == EC_Normal) && opt_spool)
    {
      if (currentPrinter)
      {
        OFLOG_INFO(dcmpsprtLogger, "spooling print job to printer '" << currentPrinter << "'");
        if (EC_Normal != dvi.spoolPrintJob())
          OFLOG_WARN(dcmpsprtLogger, "unable to spool print job to printer '" << currentPrinter << "', ignoring.");
      } else {
        OFLOG_WARN(dcmpsprtLogger, "no printer (undefined in config file?), cannot spool print job.");
      }
    }

    if ((opt_spool)&&(EC_Normal != dvi.terminatePrintSpooler()))
      OFLOG_WARN(dcmpsprtLogger, "unable to stop print spooler, ignoring.");

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return (status != EC_Normal);
}
