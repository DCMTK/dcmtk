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
 *  Update Date:      $Date: 1999-08-31 16:54:40 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmpsprt.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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

void dumpPresentationState(DVInterface& dvi);
void dumpPrinters(DVInterface& dvi);

#define SHORTCOL 2
#define LONGCOL 21

int main(int argc, char *argv[])
{
    OFCmdUnsignedInt          opt_debugMode      = 0;           /* default: no debug */
    OFBool                    opt_verbose        = OFFalse;     /* default: do not dump presentation state */
    const char *              opt_printerID = NULL;             /* printer ID */
    const char *              opt_cfgName = NULL;               /* config read file name */
    DVPSFilmOrientation       opt_filmorientation = DVPSF_default;
    DVPSTrimMode              opt_trim = DVPSH_default;
    DVPSDecimateCropBehaviour opt_decimate = DVPSI_default;
    OFCmdUnsignedInt          opt_columns = 1;
    OFCmdUnsignedInt          opt_rows = 1;
    const char *              opt_filmsize = NULL;     
    const char *              opt_magnification = NULL;   
    const char *              opt_smoothing = NULL;   
    const char *              opt_configuration = NULL;   
    const char *              opt_resolution = NULL;   
    OFList<const char *>      opt_filenames;
    OFBool                    opt_pstates_specified = OFFalse;
    OFBool                    opt_images_specified = OFFalse;
    OFBool                    opt_linearLUTshape = OFFalse;
    
    OFString str;
              
    SetDebugLevel(( 0 ));
    DicomImageClass::DebugLevel = DicomImageClass::DL_NoMessages;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Read DICOM images and presentation states and render print job", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL+2);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  
    cmd.addParam("printer_id",   "printer ID from config file");
      
    cmd.addGroup("general options:");
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, dump printer characteristics");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");

    cmd.addGroup("file options, can be specified multiple times:");
     cmd.addOption("--image",       "-i", 1, "[i]magefile: string",
                                             "add image [i] to print job");
     cmd.addOption("--pstate",      "-p", 2, "[i]magefile, [p]state-file: string",
                                             "render image [i] with presentation state [p]\nand add to print job");
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
    cmd.addGroup("processing options:");
     cmd.addOption("--config",      "-c", 1, "[f]ilename: string",
                                             "process using settings from configuration file");
    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      cmd.getParam(1, opt_printerID);
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
      if (cmd.findOption("--no-plut"))  opt_linearLUTshape = OFFalse;
      if (cmd.findOption("--identity"))  opt_linearLUTshape = OFTrue;
      cmd.endOptionBlock();

      if (cmd.findOption("--filmsize"))      app.checkValue(cmd.getValue(opt_filmsize));
      if (cmd.findOption("--magnification"))      app.checkValue(cmd.getValue(opt_magnification));
      if (cmd.findOption("--smoothing"))      app.checkValue(cmd.getValue(opt_smoothing));
      if (cmd.findOption("--configinfo"))      app.checkValue(cmd.getValue(opt_configuration));
      if (cmd.findOption("--resolution"))      app.checkValue(cmd.getValue(opt_resolution));
      if (cmd.findOption("--config"))      app.checkValue(cmd.getValue(opt_cfgName));

      if (cmd.findOption("--layout"))
      {
      	 app.checkValue(cmd.getValue(opt_columns, (OFCmdUnsignedInt)1));
      	 app.checkValue(cmd.getValue(opt_rows, (OFCmdUnsignedInt)1));
      }

      const char *aName;
      if (cmd.findOption("--pstate", 0, OFCommandLine::FOM_First))
      {
        do
        {
          app.checkValue(cmd.getValue(aName));
          opt_filenames.push_back(aName);
          app.checkValue(cmd.getValue(aName));
          opt_filenames.push_back(aName);       
          opt_pstates_specified = OFTrue;   
        } while (cmd.findOption("--pstate", 0, OFCommandLine::FOM_Next));
      }

      if (cmd.findOption("--image", 0, OFCommandLine::FOM_First))
      {
        do
        {
          app.checkValue(cmd.getValue(aName));
          opt_filenames.push_back(aName);
          opt_filenames.push_back(NULL);       
          opt_images_specified = OFTrue;   
        } while (cmd.findOption("--image", 0, OFCommandLine::FOM_Next));
      }
      if (opt_pstates_specified && opt_images_specified)
      {
        cerr << "warning: both images with and without presentation states have been specified" << endl
             << "on the command line. Images with presentation states will be processed first." << endl;
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

    dumpPrinters(dvi);
    

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
    if (EC_Normal != dvi.getPrintHandler().setFilmOrientation(opt_filmorientation))
      cerr << "warning: cannot set film orientation, ignoring." << endl;
    if (EC_Normal != dvi.getPrintHandler().setTrim(opt_trim))
      cerr << "warning: cannot set trim, ignoring." << endl;
    if (EC_Normal != dvi.getPrintHandler().setRequestedDecimateCropBehaviour(opt_decimate))
      cerr << "warning: cannot set requested decimate/crop behaviour, ignoring." << endl;
    if ((opt_linearLUTshape)&&(EC_Normal != dvi.getPrintHandler().setCurrentPresentationLUT(DVPSQ_identity)))
      cerr << "warning: cannot set IDENTITY presentation LUT shape, ignoring." << endl;

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
            if (EC_Normal != dvi.getCurrentPState().getPrintBitmapWidthHeight(width, height)) app.printError("can't determine bitmap size");
            if (EC_Normal != dvi.getCurrentPState().getPrintBitmap(pixelData, bitmapSize)) app.printError("can't create print bitmap");
            pixelAspectRatio = dvi.getCurrentPState().getDisplayedAreaPresentationPixelAspectRatio();
            
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
      if (opt_verbose) cerr << "writing DICOM stored print object to database." << endl;
      if (EC_Normal != dvi.saveStoredPrint())
      {
        cerr << "error during creation of DICOM stored print object" << endl;
      }
    }
    
#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return (status != EC_Normal);
}


// ********************************************


void dumpPrinters(DVInterface& dvi)
{
  Uint32 numPrinters = dvi.getNumberOfTargets(DVPSE_print);
  cout << "Number of known printers: " << numPrinters << endl;
  const char *c;
  OFString aString;
  
  for (Uint32 i=0; i<numPrinters; i++)
  {
    cout << endl << "Printer #" << i+1 << endl;
    const char *target = dvi.getTargetID(i, DVPSE_print);
    cout << "ID: " << target << endl;
    c = dvi.getTargetDescription(target);
    cout << "description: " << (c ? c : "(none)") << endl;
    c = dvi.getTargetHostname(target);
    cout << "hostname: " << (c ? c : "(none)") << endl;
    unsigned short port = dvi.getTargetPort(target);
    cout << "port: " << port << endl;
    c = dvi.getTargetAETitle(target);
    cout << "aetitle: " << (c ? c : "(none)") << endl;
    unsigned long pdu = dvi.getTargetMaxPDU(target);
    cout << "maxpdu: " << pdu << endl;
    cout << "implicit only: ";
    if (dvi.getTargetImplicitOnly(target)) cout << "yes" << endl; else cout << "no" << endl;
    cout << "disable new vr: ";
    if (dvi.getTargetDisableNewVRs(target)) cout << "yes" << endl; else cout << "no" << endl;
    cout << "supports presentation lut: ";
    if (dvi.getTargetPrinterSupportsPresentationLUT(target)) cout << "yes" << endl; else cout << "no" << endl;
    cout << "supports 12 bit transmission: ";
    if (dvi.getTargetPrinterSupports12BitTransmission(target)) cout << "yes" << endl; else cout << "no" << endl;
    cout << "supports requested image size: ";
    if (dvi.getTargetPrinterSupportsRequestedImageSize(target)) cout << "yes" << endl; else cout << "no" << endl;
    cout << "supports decimate/crop: ";
    if (dvi.getTargetPrinterSupportsDecimateCrop(target)) cout << "yes" << endl; else cout << "no" << endl;  
    cout << "supports trim: ";
    if (dvi.getTargetPrinterSupportsTrim(target)) cout << "yes" << endl; else cout << "no" << endl;  
    Uint32 maxcols = dvi.getTargetPrinterMaxDisplayFormatColumns(target);
    Uint32 maxrows = dvi.getTargetPrinterMaxDisplayFormatRows(target);
    cout << "max columns/rows: " << maxcols << "/" << maxrows << endl;

    Uint32 j, k;
    j = dvi.getTargetPrinterNumberOfFilmSizeIDs(target);
    cout << "film size ids: " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterFilmSizeID(target, k, aString);
      if (c==NULL) c="(none)";
      cout << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfMediumTypes(target);
    cout << "medium types: " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterMediumType(target, k, aString);
      if (c==NULL) c="(none)";
      cout << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfPrinterResolutionIDs(target);
    cout << "resolution ids: " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterResolutionID(target, k, aString);
      if (c==NULL) c="(none)";
      cout << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfMagnificationTypes(target);
    cout << "magnification types: " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterMagnificationType(target, k, aString);
      if (c==NULL) c="(none)";
      cout << "    [" << c << "]" << endl;
    }
    j = dvi.getTargetPrinterNumberOfSmoothingTypes(target);
    cout << "smoothing types: " << j << endl;
    for (k=0; k<j; k++)
    {
      c = dvi.getTargetPrinterSmoothingType(target, k, aString);
      if (c==NULL) c="(none)";
      cout << "    [" << c << "]" << endl;
    }
    c = dvi.getTargetPrinterConfigurationSetting(target);
    cout << "configuration information: " << (c ? c : "(none)") << endl;
  }
  return;  
}

/*
 * CVS/RCS Log:
 * $Log: dcmpsprt.cc,v $
 * Revision 1.1  1999-08-31 16:54:40  meichel
 * Added new sample application that allows to create simple print jobs.
 *
 *
 */
