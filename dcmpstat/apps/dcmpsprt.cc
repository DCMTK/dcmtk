/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-09-23 17:37:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmpsprt.cc,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dviface.h"
#include "dvpssp.h"
#include "dcmimage.h"
#include "cmdlnarg.h"
#include "ofcmdln.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */
#include "oflist.h"

#define OFFIS_CONSOLE_APPLICATION "dcmpsprt"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

void dumpPrinterCharacteristics(DVInterface& dvi, const char *target);

#define SHORTCOL 2
#define LONGCOL 21

int main(int argc, char *argv[])
{
    OFCmdUnsignedInt          opt_debugMode      = 0;           /* default: no debug */
    OFBool                    opt_verbose        = OFFalse;     /* default: do not dump presentation state */
    OFBool                    opt_dump           = OFFalse;
    const char *              opt_printerID = NULL;             /* printer ID */
    const char *              opt_cfgName = NULL;               /* config read file name */
    DVPSFilmOrientation       opt_filmorientation = DVPSF_default;
    DVPSTrimMode              opt_trim = DVPSH_default;
    DVPSDecimateCropBehaviour opt_decimate = DVPSI_default;
    OFCmdUnsignedInt          opt_columns = 1;
    OFCmdUnsignedInt          opt_rows = 1;
    OFCmdUnsignedInt          opt_copies = 0;
    const char *              opt_filmsize = NULL;     
    const char *              opt_magnification = NULL;   
    const char *              opt_smoothing = NULL;   
    const char *              opt_configuration = NULL;   
    const char *              opt_img_magnification = NULL;   
    const char *              opt_img_smoothing = NULL;   
    const char *              opt_img_configuration = NULL;   
    const char *              opt_resolution = NULL;   
    const char *              opt_border = NULL;   
    const char *              opt_emptyimage = NULL;       
    const char *              opt_plutname = NULL;     
    OFList<const char *>      opt_filenames;
    OFBool                    opt_linearLUTshape = OFFalse;
    OFBool                    opt_spool = OFFalse;
    const char *              opt_mediumtype = NULL;
    const char *              opt_destination     = NULL;
    const char *              opt_sessionlabel    = NULL;
    const char *              opt_priority        = NULL;
    const char *              opt_ownerID         = NULL;

    OFCmdUnsignedInt          opt_illumination = (OFCmdUnsignedInt)-1;
    OFCmdUnsignedInt          opt_reflection = (OFCmdUnsignedInt)-1;
        
    OFString str;
              
    SetDebugLevel(( 0 ));
    DicomImageClass::DebugLevel = DicomImageClass::DL_NoMessages;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Read DICOM images and presentation states and render print job", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL+2);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  
    cmd.addParam("imagefile_in",   "DICOM image file(s) to be printed", OFCmdParam::PM_MultiOptional);
      
    cmd.addGroup("general options:");
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print actions");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");

    cmd.addGroup("processing options:");
     cmd.addOption("--pstate",      "-p", 1, "[p]state-file: string",
                                             "render the following image with pres. state [p]");
     cmd.addOption("--config",      "-c", 1, "[f]ilename: string",
                                             "process using settings from configuration file");
     cmd.addOption("--printer",           1, "[n]ame: string (default: 1st printer in cfg file)",
                                             "select printer with identifier [n] from cfg file");
     cmd.addOption("--dump",                 "dump characteristics of selected printer");

    cmd.addGroup("spooling options:");
     cmd.addOption("--spool",       "-s",    "spool print job to DICOM printer");
     cmd.addOption("--nospool",              "do not spool print job to DICOM printer (default)");

    cmd.addGroup("film orientation options:");
     cmd.addOption("--portrait",             "set portrait orientation");
     cmd.addOption("--landscape",            "set landscape orientation");
     cmd.addOption("--default-orientation",  "use printer default (default)");

    cmd.addGroup("trim (border) options:");
     cmd.addOption("--trim",                 "set trim on");
     cmd.addOption("--no-trim",              "set trim off");
     cmd.addOption("--default-trim",         "use printer default (default)");

    cmd.addGroup("requested decimate/crop behaviour options:");
     cmd.addOption("--request-decimate",     "request decimate");
     cmd.addOption("--request-crop",         "request crop");
     cmd.addOption("--request-fail",         "request failure");
     cmd.addOption("--default-request",      "use printer default (default)");

    cmd.addGroup("print presentation LUT options:");
     cmd.addOption("--no-plut",              "do not use presentation LUT (default)");
     cmd.addOption("--identity",             "set IDENTITY presentation LUT shape");
     cmd.addOption("--plut",              1, "[l]ut identifier: string",
                                             "add LUT [l] to print job");

    cmd.addGroup("basic film session options (only with --spool):");
     cmd.addOption("--copies",            1, "[v]alue: integer (1..100, default: 1)",
                                             "set number of copies to [v]");
     cmd.addOption("--medium-type",       1, "[v]alue: string",
                                             "set medium type to [v]");
     cmd.addOption("--illumination",      1, "[v]alue: integer (0..65535)",
                                             "set illumination to [v] cd/m^2");
     cmd.addOption("--reflection",        1, "[v]alue: integer (0..65535)",
                                             "set reflected ambient light to [v] cd/m^2");
     cmd.addOption("--destination",       1, "[v]alue: string",
                                             "set film destination to [v]");
     cmd.addOption("--label",             1, "[v]alue: string",
                                             "set film session label to [v]");
     cmd.addOption("--priority",          1, "[v]alue: string",
                                             "set print priority to [v]");
     cmd.addOption("--owner",             1, "[v]alue: string",
                                             "set film session owner ID to [v]");

    cmd.addGroup("other print options:");
     cmd.addOption("--layout",      "-l", 2, "[c]olumns, [r]ows: integer (default: 1,1)",
                                             "use 'STANDARD\\c,r' image display format");
     cmd.addOption("--filmsize",          1, "[v]alue: string",
                                             "set film size ID to [v]");
     cmd.addOption("--magnification",     1, "[v]alue: string",
                                             "set magnification type to [v]");
     cmd.addOption("--smoothing",         1, "[v]alue: string",
                                             "set smoothing type to [v]");
     cmd.addOption("--configinfo",        1, "[v]alue: string",
                                             "set configuration information to [v]");
     cmd.addOption("--resolution",        1, "[v]alue: string",
                                             "set requested resolution ID to [v]");
     cmd.addOption("--border",            1, "[v]alue: string",
                                             "set border density to [v]");
     cmd.addOption("--empty-image",       1, "[v]alue: string",
                                             "set empty image density to [v]");
     cmd.addOption("--img-magnification", 1, "[v]alue: string",
                                             "set image box magnification type to [v]");
     cmd.addOption("--img-smoothing",     1, "[v]alue: string",
                                             "set image box smoothing type to [v]");
     cmd.addOption("--img-configinfo",    1, "[v]alue: string",
                                             "set image box configuration information to [v]");

    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      if (cmd.findOption("--verbose"))     opt_verbose=OFTrue;
      if (cmd.findOption("--debug"))       opt_debugMode = 3;

      cmd.beginOptionBlock();
      if (cmd.findOption("--portrait"))  opt_filmorientation = DVPSF_portrait;
      if (cmd.findOption("--landscape"))  opt_filmorientation = DVPSF_landscape;
      if (cmd.findOption("--default-orientation"))  opt_filmorientation = DVPSF_default;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--trim"))  opt_trim = DVPSH_trim_on;
      if (cmd.findOption("--no-trim"))  opt_trim = DVPSH_trim_off;
      if (cmd.findOption("--default-trim"))  opt_trim = DVPSH_default;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--request-decimate"))  opt_decimate = DVPSI_decimate;
      if (cmd.findOption("--request-crop"))  opt_decimate = DVPSI_crop;
      if (cmd.findOption("--request-fail"))  opt_decimate = DVPSI_fail;
      if (cmd.findOption("--default-request"))  opt_decimate = DVPSI_default;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--no-plut"))   opt_linearLUTshape = OFFalse;
      if (cmd.findOption("--identity"))  opt_linearLUTshape = OFTrue;
      if (cmd.findOption("--plut"))      app.checkValue(cmd.getValue(opt_plutname));
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--spool"))         opt_spool = OFTrue;
      if (cmd.findOption("--nospool"))       opt_spool = OFFalse;
      cmd.endOptionBlock();

      if (cmd.findOption("--filmsize"))      app.checkValue(cmd.getValue(opt_filmsize));
      if (cmd.findOption("--magnification")) app.checkValue(cmd.getValue(opt_magnification));
      if (cmd.findOption("--smoothing"))     app.checkValue(cmd.getValue(opt_smoothing));
      if (cmd.findOption("--configinfo"))    app.checkValue(cmd.getValue(opt_configuration));
      if (cmd.findOption("--resolution"))    app.checkValue(cmd.getValue(opt_resolution));
      if (cmd.findOption("--border"))        app.checkValue(cmd.getValue(opt_border));
      if (cmd.findOption("--empty-image"))   app.checkValue(cmd.getValue(opt_emptyimage));
      if (cmd.findOption("--config"))        app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--printer"))       app.checkValue(cmd.getValue(opt_printerID));
      if (cmd.findOption("--img-magnification")) app.checkValue(cmd.getValue(opt_img_magnification));
      if (cmd.findOption("--img-smoothing"))     app.checkValue(cmd.getValue(opt_img_smoothing));
      if (cmd.findOption("--img-configinfo"))    app.checkValue(cmd.getValue(opt_img_configuration));
      if (cmd.findOption("--dump"))          opt_dump = OFTrue;

      /* film session options */
      if (cmd.findOption("--medium-type"))
      {
        app.checkConflict("--medium-type", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_mediumtype));
      }
      if (cmd.findOption("--illumination"))
      {
        app.checkConflict("--illumination", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_illumination, (OFCmdUnsignedInt)0, (OFCmdUnsignedInt)65535));
      }
      if (cmd.findOption("--reflection"))
      {
        app.checkConflict("--reflection", "--nospool", (! opt_spool));
        app.checkValue(cmd.getValue(opt_reflection, (OFCmdUnsignedInt)0, (OFCmdUnsignedInt)65535));
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
        app.checkValue(cmd.getValue(opt_copies, (OFCmdUnsignedInt)1, (OFCmdUnsignedInt)100));
      }
      
      if (cmd.findOption("--pstate")) { /* prevent warning - this option is only checked if image filenames are really specified */ }

      if (cmd.findOption("--layout"))
      {
      	 app.checkValue(cmd.getValue(opt_columns, (OFCmdUnsignedInt)1));
      	 app.checkValue(cmd.getValue(opt_rows, (OFCmdUnsignedInt)1));
      }

      const char *imageFile=NULL;
      const char *pstateFile=NULL;
      int paramCount = cmd.getParamCount();
      for (int param=1; param<=paramCount; param++)
      {
        cmd.getParam(param, imageFile);
      	pstateFile = NULL;
        if (cmd.findOption("--pstate", -param)) app.checkValue(cmd.getValue(pstateFile));
        opt_filenames.push_back(imageFile);
        opt_filenames.push_back(pstateFile);
      }
    }
    
    SetDebugLevel(((int)opt_debugMode));
    DicomImageClass::DebugLevel = (int)opt_debugMode;

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        cerr << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
        return 10;
      }
    }
    DVInterface dvi(opt_cfgName);
    
    if (opt_printerID && (EC_Normal != dvi.setCurrentPrinter(opt_printerID)))
      cerr << "warning: unable to select printer '" << opt_printerID << "', ignoring." << endl;

    /* dump printer characteristics if requested */
    const char *currentPrinter = dvi.getCurrentPrinter();
    if (opt_dump) dumpPrinterCharacteristics(dvi, currentPrinter);

    if (EC_Normal != dvi.getPrintHandler().setImageDisplayFormat(opt_columns, opt_rows))
      cerr << "warning: cannot set image display format to columns=" << opt_columns
           << ", rows=" << opt_rows << ", ignoring." << endl;  
    if ((opt_filmsize)&&(EC_Normal != dvi.getPrintHandler().setFilmSizeID(opt_filmsize)))
      cerr << "warning: cannot set film size ID to '" << opt_filmsize << "', ignoring." << endl;
    if ((opt_magnification)&&(EC_Normal != dvi.getPrintHandler().setMagnificationType(opt_magnification)))
      cerr << "warning: cannot set magnification type to '" << opt_magnification << "', ignoring." << endl;
    if ((opt_smoothing)&&(EC_Normal != dvi.getPrintHandler().setSmoothingType(opt_smoothing)))
      cerr << "warning: cannot set smoothing type to '" << opt_smoothing << "', ignoring." << endl;
    if ((opt_configuration)&&(EC_Normal != dvi.getPrintHandler().setConfigurationInformation(opt_configuration)))
      cerr << "warning: cannot set configuration information to '" << opt_configuration << "', ignoring." << endl;
    if ((opt_resolution)&&(EC_Normal != dvi.getPrintHandler().setResolutionID(opt_resolution)))
      cerr << "warning: cannot set requested resolution ID to '" << opt_resolution << "', ignoring." << endl;
    if ((opt_border)&&(EC_Normal != dvi.getPrintHandler().setBorderDensity(opt_border)))
      cerr << "warning: cannot set border density to '" << opt_resolution << "', ignoring." << endl;
    if ((opt_emptyimage)&&(EC_Normal != dvi.getPrintHandler().setEmtpyImageDensity(opt_emptyimage)))
      cerr << "warning: cannot set empty image density to '" << opt_resolution << "', ignoring." << endl;
    if (EC_Normal != dvi.getPrintHandler().setFilmOrientation(opt_filmorientation))
      cerr << "warning: cannot set film orientation, ignoring." << endl;
    if (EC_Normal != dvi.getPrintHandler().setTrim(opt_trim))
      cerr << "warning: cannot set trim, ignoring." << endl;
    if (EC_Normal != dvi.getPrintHandler().setRequestedDecimateCropBehaviour(opt_decimate))
      cerr << "warning: cannot set requested decimate/crop behaviour, ignoring." << endl;

    if ((opt_illumination != (OFCmdUnsignedInt)-1)&&(EC_Normal != dvi.setPrintIllumination((Uint16)opt_illumination)))
      cerr << "warning: cannot set film session illumination to '" << opt_illumination << "', ignoring." << endl;
    if ((opt_reflection != (OFCmdUnsignedInt)-1)&&(EC_Normal != dvi.setPrintReflectedAmbientLight((Uint16)opt_reflection)))
      cerr << "warning: cannot set film session illumination to '" << opt_reflection << "', ignoring." << endl;
    if ((opt_copies > 0)&&(EC_Normal != dvi.setPrinterNumberOfCopies(opt_copies)))
      cerr << "warning: cannot set film session number of copies to '" << opt_copies << "', ignoring." << endl;
    if ((opt_mediumtype)&&(EC_Normal != dvi.setPrinterMediumType(opt_mediumtype)))
      cerr << "warning: cannot set film session medium type to '" << opt_mediumtype << "', ignoring." << endl;
    if ((opt_destination)&&(EC_Normal != dvi.setPrinterFilmDestination(opt_destination)))
      cerr << "warning: cannot set film destination to '" << opt_destination << "', ignoring." << endl;
    if ((opt_sessionlabel)&&(EC_Normal != dvi.setPrinterFilmSessionLabel(opt_sessionlabel)))
      cerr << "warning: cannot set film session label to '" << opt_sessionlabel << "', ignoring." << endl;
    if ((opt_priority)&&(EC_Normal != dvi.setPrinterPriority(opt_priority)))
      cerr << "warning: cannot set film session print priority to '" << opt_priority << "', ignoring." << endl;
    if ((opt_ownerID)&&(EC_Normal != dvi.setPrinterOwnerID(opt_ownerID)))
      cerr << "warning: cannot set film session owner ID to '" << opt_ownerID << "', ignoring." << endl;
      
    if (opt_plutname)
    {
      if (EC_Normal != dvi.selectPrintPresentationLUT(opt_plutname))
      cerr << "warning: cannot set requested presentation LUT '" << opt_plutname << "', ignoring." << endl;
    } else {
      if ((opt_linearLUTshape)&&(EC_Normal != dvi.getPrintHandler().setCurrentPresentationLUT(DVPSQ_identity)))
        cerr << "warning: cannot set IDENTITY presentation LUT shape, ignoring." << endl;
    }

    if ((opt_spool)&&(EC_Normal != dvi.startPrintSpooler()))
      cerr << "warning: unable to start print spooler, ignoring." << endl;
    
    OFListIterator(const char *) first = opt_filenames.begin();
    OFListIterator(const char *) last = opt_filenames.end();
    const char *currentImage = NULL;
    const char *currentPState = NULL;
    E_Condition status = EC_Normal;
    void *pixelData = NULL;
    unsigned long width = 0;
    unsigned long height = 0;
    unsigned long bitmapSize = 0;
    double pixelAspectRatio;
        
    while ((EC_Normal == status)&&(first != last))
    {
      currentImage = *first;
      ++first;
      if (first != last)
      {
      	currentPState = *first;
      	++first;
        if (currentPState)
        {
          if (opt_verbose) cerr << "loading image file '" << currentImage << "' with presentation state '" << currentPState << "'" << endl;
          status = dvi.loadPState(currentPState, currentImage);
          if (EC_Normal != status) 
          {
            cerr << "error: loading image file '" << currentImage << "' with presentation state '" << currentPState << "' failed." << endl;
            return 10;
          }
        }
        else 
        {
          if (opt_verbose) cerr << "loading image file '" << currentImage << "'" << endl;
          status = dvi.loadImage(currentImage);
          if (EC_Normal != status) 
          {
            cerr << "error: loading image file '" << currentImage << "' failed." << endl;
            return 10;
          }
        }

        // save grayscale hardcopy image.
        bitmapSize = dvi.getCurrentPState().getPrintBitmapSize();
        pixelData = new char[bitmapSize];
        if (pixelData)
        {
            if (EC_Normal != dvi.getCurrentPState().getPrintBitmapWidthHeight(width, height))
            {
              cerr << "error: can't determine bitmap size" << endl;
              return 10;
            }
            if (EC_Normal != dvi.getCurrentPState().getPrintBitmap(pixelData, bitmapSize))
            {
              cerr << "error: can't create print bitmap" << endl;
              return 10;
            }
            pixelAspectRatio = dvi.getCurrentPState().getPrintBitmapPixelAspectRatio();
            
            if (opt_verbose) cerr << "writing DICOM grayscale hardcopy image to database." << endl;
            if (EC_Normal != dvi.saveGrayscaleHardcopyImage(pixelData, width, height, pixelAspectRatio))
            {
              cerr << "error during creation of DICOM grayscale hardcopy image file" << endl;
              return 10;
            }
            delete[] pixelData;
        } else {
          cerr << "out of memory error: cannot allocate print bitmap" << endl;
          return 10;
        }
      } else {
        cerr << "internal error - odd number of filenames" << endl;
        return 10;
      }
    }

    if (status == EC_Normal)
    {
      size_t numImages = dvi.getPrintHandler().getNumberOfImages();
      for (size_t i=0; i<numImages; i++)
      {
        if ((opt_img_magnification)&&(EC_Normal != dvi.getPrintHandler().setImageMagnificationType(i, opt_img_magnification)))
          cerr << "warning: cannot set magnification type for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_magnification << "', ignoring." << endl;
        if ((opt_img_smoothing)&&(EC_Normal != dvi.getPrintHandler().setImageSmoothingType(i, opt_img_smoothing)))
          cerr << "warning: cannot set smoothing type for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_smoothing << "', ignoring." << endl;
        if ((opt_img_configuration)&&(EC_Normal != dvi.getPrintHandler().setImageConfigurationInformation(i, opt_img_configuration)))
          cerr << "warning: cannot set configuration information for image #" << i+1 << " (of " << numImages << ") to '" << opt_img_configuration << "', ignoring." << endl;
      }
      if ((numImages > 0)&&(! opt_spool))
      {
      	// no need to do this manually if we are spooling - spoolPrintJob() will do this anyway.
        if (opt_verbose) cerr << "writing DICOM stored print object to database." << endl;
        if (EC_Normal != dvi.saveStoredPrint(dvi.getTargetPrinterSupportsRequestedImageSize(opt_printerID)))
        {
          cerr << "error during creation of DICOM stored print object" << endl;
        }
      }
    }

    if ((status == EC_Normal) && opt_spool)
    {
      if (currentPrinter)
      {
        if (opt_verbose) cerr << "spooling print job to printer '" << currentPrinter << "'" << endl;
        if (EC_Normal != dvi.spoolPrintJob())
          cerr << "warning: unable to spool print job to printer '" << currentPrinter << "', ignoring." << endl;
      } else {
          cerr << "warning: no printer (undefined in config file?), cannot spool print job." << endl;
      }
    }

    if ((opt_spool)&&(EC_Normal != dvi.terminatePrintSpooler()))
      cerr << "warning: unable to stop print spooler, ignoring." << endl;
    
#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return (status != EC_Normal);
}


// ********************************************


void dumpPrinterCharacteristics(DVInterface& dvi, const char *target)
{
    const char *c;
    OFString aString;
    
    cerr << "========== Dump of Printer Characteristics ==========" << endl;
    cerr << "Printer ID                    : " << target << endl;
    c = dvi.getTargetDescription(target);
    cerr << "description                   : " << (c ? c : "(none)") << endl;
    c = dvi.getTargetHostname(target);
    cerr << "hostname                      : " << (c ? c : "(none)") << endl;
    unsigned short port = dvi.getTargetPort(target);
    cerr << "port                          : " << port << endl;
    c = dvi.getTargetAETitle(target);
    cerr << "application entity title      : " << (c ? c : "(none)") << endl;
    unsigned long pdu = dvi.getTargetMaxPDU(target);
    cerr << "max PDU size                  : " << pdu << endl;
    cerr << "implicit only                 : ";
    if (dvi.getTargetImplicitOnly(target)) cerr << "yes" << endl; else cerr << "no" << endl;
    cerr << "disable new vr                : ";
    if (dvi.getTargetDisableNewVRs(target)) cerr << "yes" << endl; else cerr << "no" << endl;
    cerr << "supports presentation lut     : ";
    if (dvi.getTargetPrinterSupportsPresentationLUT(target)) cerr << "yes" << endl; else cerr << "no" << endl;
    cerr << "supports 12 bit transmission  : ";
    if (dvi.getTargetPrinterSupports12BitTransmission(target)) cerr << "yes" << endl; else cerr << "no" << endl;
    cerr << "supports requested image size : ";
    if (dvi.getTargetPrinterSupportsRequestedImageSize(target)) cerr << "yes" << endl; else cerr << "no" << endl;
    cerr << "supports decimate/crop        : ";
    if (dvi.getTargetPrinterSupportsDecimateCrop(target)) cerr << "yes" << endl; else cerr << "no" << endl;  
    cerr << "supports trim                 : ";
    if (dvi.getTargetPrinterSupportsTrim(target)) cerr << "yes" << endl; else cerr << "no" << endl;  
    Uint32 maxcols = dvi.getTargetPrinterMaxDisplayFormatColumns(target);
    Uint32 maxrows = dvi.getTargetPrinterMaxDisplayFormatRows(target);
    cerr << "max columns/rows              : ";
    if (maxcols == (unsigned long)-1) cerr << "unlimited/"; else cerr << maxcols << "/";
    if (maxrows == (unsigned long)-1) cerr << "unlimited" << endl; else cerr << maxrows << endl;

    Uint32 j, k;
    j = dvi.getTargetPrinterNumberOfFilmSizeIDs(target);
    cerr << "film size IDs                 : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterFilmSizeID(target, k, aString);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfMediumTypes(target);
    cerr << "medium types                  : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterMediumType(target, k, aString);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfPrinterResolutionIDs(target);
    cerr << "resolution IDs                : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterResolutionID(target, k, aString);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfMagnificationTypes(target);
    cerr << "magnification types           : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterMagnificationType(target, k, aString);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfSmoothingTypes(target);
    cerr << "smoothing types               : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterSmoothingType(target, k, aString);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfConfigurationSettings(target);
    cerr << "configuration information     : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterConfigurationSetting(target, k);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfBorderDensities(target);
    cerr << "border density                : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterBorderDensity(target, k, aString);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfEmptyImageDensities(target);
    cerr << "empty image density           : " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterEmptyImageDensity(target, k, aString);
      if (c==NULL) c="(none)";
      cerr << "    [" << c << "]" << endl;
    }

    cerr << "=====================================================" << endl << endl;
    return;  
}

/*
 * CVS/RCS Log:
 * $Log: dcmpsprt.cc,v $
 * Revision 1.7  1999-09-23 17:37:09  meichel
 * Added support for Basic Film Session options to dcmpstat print code.
 *
 * Revision 1.6  1999/09/15 17:42:56  meichel
 * Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *   and dcmpsprt applications.
 *
 * Revision 1.5  1999/09/14 18:12:29  meichel
 * Removed unneeded debug output from dcmpsprt
 *
 * Revision 1.4  1999/09/13 15:18:45  meichel
 * Adapted dcmpsprt to print API enhancements
 *
 * Revision 1.3  1999/09/08 16:49:22  meichel
 * Added print API method declarations
 *
 * Revision 1.2  1999/09/01 16:14:11  meichel
 * Completed printer characteristics dump routine
 *
 * Revision 1.1  1999/08/31 16:54:40  meichel
 * Added new sample application that allows to create simple print jobs.
 *
 *
 */
