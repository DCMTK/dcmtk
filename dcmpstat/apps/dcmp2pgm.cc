/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Authors: Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose
 *    sample application that reads a DICOM image and (optionally)
 *    a presentation state and creates a PGM bitmap using the settings
 *    of the presentation state. Non-grayscale transformations are
 *    ignored. If no presentation state is loaded, a default is created.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-05-03 14:27:26 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmp2pgm.cc,v $
 *  CVS/RCS Revision: $Revision: 1.21 $
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
#include "dvpstx.h"  /* for DVPSTextObject */
#include "dvpsgr.h"  /* for DVPSGraphicObject */
#include "dvpscu.h"  /* for DVPSCurve */
#include "dcmimage.h"
#include "cmdlnarg.h"
#include "ofcmdln.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmp2pgm"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

void dumpPresentationState(DVInterface& dvi);

#define SHORTCOL 2
#define LONGCOL 13

int main(int argc, char *argv[])
{
    OFString str;
    int    opt_debugMode    = 0;                       /* default: no debug */
    OFBool opt_dump_pstate  = OFFalse;                 /* default: do not dump presentation state */
    OFBool opt_dicom_mode   = OFFalse;                 /* default: create PGM, not DICOM SC */
    const char *opt_pstName = NULL;                    /* pstate read file name */
    const char *opt_imgName = NULL;                    /* image read file name */
    const char *opt_pgmName = NULL;                    /* pgm save file name */
    const char *opt_savName = NULL;                    /* pstate save file name */
    const char *opt_cfgName = NULL;                    /* config read file name */

    SetDebugLevel(( 0 ));
    DicomImageClass::setDebugLevel(DicomImageClass::DL_NoMessages);

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Read DICOM image and presentation state and render bitmap", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("dcmimage_in",              "input DICOM image");
    cmd.addParam("bitmap_out",               "output DICOM image or PGM bitmap", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:");
     cmd.addOption("--help",        "-h",    "print this help text and exit");
     cmd.addOption("--verbose",     "-v",    "verbose mode, dump presentation state contents");
     cmd.addOption("--debug",       "-d",    "debug mode, print debug information");

    cmd.addGroup("processing options:");
     cmd.addOption("--pstate",      "-p", 1, "[f]ilename: string",
                                             "process using presentation state file");
     cmd.addOption("--config",      "-c", 1, "[f]ilename: string",
                                             "process using settings from configuration file");
    cmd.addGroup("output format:");
     cmd.addOption("--pgm",         "-D",    "save image as PGM (default)");
     cmd.addOption("--dicom",       "+D",    "save image as DICOM secondary capture");

    cmd.addGroup("output options:");
     cmd.addOption("--save-pstate", "+S", 1, "[f]ilename: string",
                                             "save presentation state to file");
    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      cmd.getParam(1, opt_imgName);
      if (cmd.getParamCount() > 1)
        cmd.getParam(2, opt_pgmName);
      if (cmd.findOption("--verbose"))     opt_dump_pstate=OFTrue;
      if (cmd.findOption("--debug"))       opt_debugMode = 3;
      if (cmd.findOption("--pstate"))      app.checkValue(cmd.getValue(opt_pstName));
      if (cmd.findOption("--config"))      app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--pgm"))         opt_dicom_mode = OFFalse;
      if (cmd.findOption("--dicom"))       opt_dicom_mode = OFTrue;
      if (cmd.findOption("--save-pstate")) app.checkValue(cmd.getValue(opt_savName));
    }

    SetDebugLevel((opt_debugMode));
    DicomImageClass::setDebugLevel(opt_debugMode);

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        CERR << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
        return 10;
      }
    }
    DVInterface dvi(opt_cfgName);
    E_Condition status = EC_Normal;

    if (opt_pstName == NULL)
    {
        if (opt_debugMode > 0)
            CERR << "reading DICOM image file: " << opt_imgName << endl;
        status = dvi.loadImage(opt_imgName);
    } else {
        if (opt_debugMode > 0)
        {
            CERR << "reading DICOM pstate file: " << opt_pstName << endl;
            CERR << "reading DICOM image file: " << opt_imgName << endl;
        }
        status = dvi.loadPState(opt_pstName, opt_imgName);
    }

    if (status == EC_Normal)
    {
    	if (opt_dump_pstate) dumpPresentationState(dvi);
        if (opt_pgmName != NULL)
        {
            const void *pixelData = NULL;
            unsigned long width = 0;
            unsigned long height = 0;
            if (opt_debugMode > 0) CERR << "creating pixel data" << endl;
            if ((dvi.getCurrentPState().getPixelData(pixelData, width, height) == EC_Normal) && (pixelData != NULL))
            {
              if (opt_dicom_mode)
              {
                double pixelAspectRatio = dvi.getCurrentPState().getPrintBitmapPixelAspectRatio(); // works for rotated images
                if (opt_debugMode > 0) CERR << "writing DICOM SC file: " << opt_pgmName << endl;
                if (EC_Normal != dvi.saveDICOMImage(opt_pgmName, pixelData, width, height, pixelAspectRatio))
                {
                  CERR << "error during creation of DICOM file" << endl;
                }
              } else {
                FILE *outfile = fopen(opt_pgmName, "wb");
                if (outfile)
                {
                    if (opt_debugMode > 0)
                        CERR << "writing PGM file: " << opt_pgmName << endl;
                    fprintf(outfile, "P5\n%ld %ld 255\n", width, height);
                    fwrite(pixelData, (size_t)width, (size_t)height, outfile);
                    fclose(outfile);
                } else app.printError("Can't create output file.");
              }
            } else app.printError("Can't create output data.");
        }
        if (opt_savName != NULL)
        {
            if (opt_debugMode > 0)
                CERR << "writing pstate file: " << opt_savName << endl;
            if (dvi.savePState(opt_savName) != EC_Normal)
                app.printError("Can't write pstate file.");
        }
    } else
        app.printError("Can't open input file(s).");

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return (status != EC_Normal);
}


// ********************************************

void dumpPresentationState(DVInterface& dvi)
{
  DVPresentationState &ps = dvi.getCurrentPState();
  size_t i, j, max;
  const char *c;

  COUT << "DUMPING PRESENTATION STATE" << endl
       << "--------------------------" << endl << endl;

  c = ps.getPresentationLabel();
  COUT << "Presentation Label: "; if (c) COUT << c << endl; else COUT << "none" << endl;
  c = ps.getPresentationDescription();
  COUT << "Presentation Description: "; if (c) COUT << c << endl; else COUT << "none" << endl;
  c = ps.getPresentationCreatorsName();
  COUT << "Presentation Creator's Name: "; if (c) COUT << c << endl; else COUT << "none" << endl;

  COUT << "VOI transformation: ";
  if (ps.haveActiveVOIWindow())
  {
    double width=0.0, center=0.0;
    ps.getCurrentWindowWidth(width);
    ps.getCurrentWindowCenter(center);
    COUT << "window center=" << center << " width=" << width << " description=\"";
    c = ps.getCurrentVOIDescription();
    if (c) COUT << c << "\"" << endl; else COUT << "(none)\"" << endl;
  }
  else if (ps.haveActiveVOILUT())
  {
    COUT << "lut description=\"";
    c = ps.getCurrentVOIDescription();
    if (c) COUT << c << "\"" << endl; else COUT << "(none)\"" << endl;
  }
  else COUT << "none" << endl;

  COUT << "Rotation: ";
  switch (ps.getRotation())
  {
    case DVPSR_0_deg:
      COUT << "none";
      break;
    case DVPSR_90_deg:
      COUT << "90 degrees";
      break;
    case DVPSR_180_deg:
      COUT << "180 degrees";
      break;
    case DVPSR_270_deg:
      COUT << "270 degrees";
      break;
  }
  COUT << endl;
  COUT << "Flip: ";
  if (ps.getFlip()) COUT << "yes" << endl; else COUT << "no" << endl;

  Sint32 tlhcX=0;
  Sint32 tlhcY=0;
  Sint32 brhcX=0;
  Sint32 brhcY=0;
  COUT << "Displayed area:" << endl;

  DVPSPresentationSizeMode sizemode = ps.getDisplayedAreaPresentationSizeMode();
  double factor=1.0;
  switch (sizemode)
  {
    case DVPSD_scaleToFit:
      COUT << "  presentation size mode: SCALE TO FIT" << endl;
      break;
    case DVPSD_trueSize:
      COUT << "  presentation size mode: TRUE SIZE" << endl;
      break;
    case DVPSD_magnify:
      ps.getDisplayedAreaPresentationPixelMagnificationRatio(factor);
      COUT << "  presentation size mode: MAGNIFY factor=" << factor << endl;
      break;
  }
  ps.getDisplayedArea(tlhcX, tlhcY, brhcX, brhcY);
  COUT << "  displayed area TLHC=" << tlhcX << "\\" << tlhcY << " BRHC=" << brhcX << "\\" << brhcY << endl;

  double x, y;
  if (EC_Normal == ps.getDisplayedAreaPresentationPixelSpacing(x,y))
  {
    COUT << "  presentation pixel spacing: X=" << x << "mm Y=" << y << " mm" << endl;
  } else {
    COUT << "  presentation pixel aspect ratio: " << ps.getDisplayedAreaPresentationPixelAspectRatio() << endl;
  }

  COUT << "Rectangular shutter: ";
  if (ps.haveShutter(DVPSU_rectangular))
  {
    COUT << "LV=" << ps.getRectShutterLV()
         << " RV=" << ps.getRectShutterRV()
         << " UH=" << ps.getRectShutterUH()
         << " LH=" << ps.getRectShutterLH() << endl;

  } else COUT << "none" << endl;
  COUT << "Circular shutter: ";
  if (ps.haveShutter(DVPSU_circular))
  {
    COUT << "center=" << ps.getCenterOfCircularShutter_x()
         << "\\" << ps.getCenterOfCircularShutter_y()
         << " radius=" << ps.getRadiusOfCircularShutter() << endl;
  } else COUT << "none" << endl;
  COUT << "Polygonal shutter: ";
  if (ps.haveShutter(DVPSU_polygonal))
  {
     COUT << "points=" << ps.getNumberOfPolyShutterVertices() << " coordinates=";
     j = ps.getNumberOfPolyShutterVertices();
     Sint32 polyX, polyY;
     for (i=0; i<j; i++)
     {
     	if (EC_Normal == ps.getPolyShutterVertex(i, polyX, polyY))
     	{
     	  COUT << polyX << "\\" << polyY << ", ";
     	} else COUT << "???\\???,";
     }
     COUT << endl;
  } else COUT << "none" << endl;
  COUT << "Bitmap shutter: ";
  if (ps.haveShutter(DVPSU_bitmap))
  {
     COUT << "present" << endl;
  } else COUT << "none" << endl;
  COUT << "Shutter presentation value: 0x" << hex << ps.getShutterPresentationValue() << dec << endl;
  COUT << endl;

  ps.sortGraphicLayers();  // to order of display
  for (size_t layer=0; layer<ps.getNumberOfGraphicLayers(); layer++)
  {
    c = ps.getGraphicLayerName(layer);
    COUT << "Graphic Layer #" << layer+1 << " ["; if (c) COUT << c; else COUT << "(unnamed)";
    COUT << "]" << endl;
    c = ps.getGraphicLayerDescription(layer);
    COUT << "  Description: "; if (c) COUT << c << endl; else COUT << "none" << endl;
    COUT << "  Recomm. display value: ";
    if (ps.haveGraphicLayerRecommendedDisplayValue(layer))
    {
      Uint16 r, g, b;
      COUT << "gray ";
      if (EC_Normal == ps.getGraphicLayerRecommendedDisplayValueGray(layer, g))
      {
      	  COUT << "0x" << hex << g << dec << endl;
      } else COUT << "error" << endl;
      COUT << "color ";
      if (EC_Normal == ps.getGraphicLayerRecommendedDisplayValueRGB(layer, r, g, b))
      {
      	  COUT << "0x" << hex << r << "\\0x" << g << "\\0x" << b << dec << endl;
      } else COUT << "error" << endl;
    } else COUT << "none" << endl;

    // text objects
    max = ps.getNumberOfTextObjects(layer);
    COUT << "  Number of text objects: " << max << endl;
    DVPSTextObject *ptext = NULL;
    for (size_t textidx=0; textidx<max; textidx++)
    {
      ptext = ps.getTextObject(layer, textidx);
      if (ptext)
      {
      	// display contents of text object
        COUT << "      text " << textidx+1 << ": \"" << ptext->getText() << "\"" << endl;
        COUT << "        anchor point: ";
        if (ptext->haveAnchorPoint())
        {
          COUT << ptext->getAnchorPoint_x() << "\\" << ptext->getAnchorPoint_y() << " units=";
          if (ptext->getAnchorPointAnnotationUnits()==DVPSA_display) COUT << "display"; else COUT << "pixel";
          COUT << " visible=";
          if (ptext->anchorPointIsVisible()) COUT << "yes"; else COUT << "no";
          COUT << endl;
        } else COUT << "none" << endl;
        COUT << "        bounding box: ";
        if (ptext->haveBoundingBox())
        {
          COUT << "TLHC=";
          COUT << ptext->getBoundingBoxTLHC_x() << "\\" << ptext->getBoundingBoxTLHC_y()
               << " BRHC=" << ptext->getBoundingBoxBRHC_x() << "\\" << ptext->getBoundingBoxBRHC_y()
               << " units=";
          if (ptext->getBoundingBoxAnnotationUnits()==DVPSA_display) COUT << "display"; else COUT << "pixel";

          DVPSTextJustification justification = ptext->getBoundingBoxHorizontalJustification();
          COUT << " justification=";
          switch (justification)
          {
            case DVPSX_left:
              COUT << "left";
              break;
            case DVPSX_right:
              COUT << "right";
              break;
            case DVPSX_center:
              COUT << "center";
              break;
          }
          COUT << endl;
        } else COUT << "none" << endl;
      }
    }

    // graphic objects
    max = ps.getNumberOfGraphicObjects(layer);
    COUT << "  Number of graphic objects: " << max << endl;
    DVPSGraphicObject *pgraphic = NULL;
    for (size_t graphicidx=0; graphicidx<max; graphicidx++)
    {
      pgraphic = ps.getGraphicObject(layer, graphicidx);
      if (pgraphic)
      {
      	// display contents of graphic object
        COUT << "      graphic " << graphicidx+1 << ": points=" << pgraphic->getNumberOfPoints()
             << " type=";
        switch (pgraphic->getGraphicType())
        {
          case DVPST_polyline: COUT << "polyline filled="; break;
          case DVPST_interpolated: COUT << "interpolated filled="; break;
          case DVPST_circle: COUT << "circle filled="; break;
          case DVPST_ellipse: COUT << "ellipse filled="; break;
          case DVPST_point: COUT << "point filled="; break;
        }
        if (pgraphic->isFilled()) COUT << "yes units="; else COUT << "no units=";
        if (pgraphic->getAnnotationUnits()==DVPSA_display) COUT << "display"; else COUT << "pixel";
        COUT << endl << "        coordinates: ";
        j = pgraphic->getNumberOfPoints();
        Float32 fx=0.0, fy=0.0;
        for (i=0; i<j; i++)
        {
          if (EC_Normal==pgraphic->getPoint(i,fx,fy))
          {
            COUT << fx << "\\" << fy << ", ";
          } else COUT << "???\\???, ";
        }
        COUT << endl;
      }
    }

    // curve objects
    max = ps.getNumberOfCurves(layer);
    COUT << "  Number of activated curves: " << max << endl;
    DVPSCurve *pcurve = NULL;
    for (size_t curveidx=0; curveidx<max; curveidx++)
    {
      pcurve = ps.getCurve(layer, curveidx);
      if (pcurve)
      {
      	// display contents of curve
        COUT << "      curve " << curveidx+1 << ": points=" << pcurve->getNumberOfPoints()
             << " type=";
        switch (pcurve->getTypeOfData())
        {
          case DVPSL_roiCurve: COUT << "roi units="; break;
          case DVPSL_polylineCurve: COUT << "poly units="; break;
        }
        c = pcurve->getCurveAxisUnitsX();
        if (c && (strlen(c)>0)) COUT << c << "\\"; else COUT << "(none)\\";
        c = pcurve->getCurveAxisUnitsY();
        if (c && (strlen(c)>0)) COUT << c << endl; else COUT << "(none)" << endl;
        COUT << "        label=";
        c = pcurve->getCurveLabel();
        if (c && (strlen(c)>0)) COUT << c << " description="; else COUT << "(none) description=";
        c = pcurve->getCurveDescription();
        if (c && (strlen(c)>0)) COUT << c << endl; else COUT << "(none)" << endl;
        COUT << "        coordinates: ";
        j = pcurve->getNumberOfPoints();
        double dx=0.0, dy=0.0;
        for (i=0; i<j; i++)
        {
          if (EC_Normal==pcurve->getPoint(i,dx,dy))
          {
            COUT << dx << "\\" << dy << ", ";
          } else COUT << "???\\???, ";
        }
        COUT << endl;
      } else COUT << "      curve " << curveidx+1 << " not present in image." << endl;
    }

    // overlay objects
    const void *overlayData=NULL;
    unsigned int overlayWidth=0, overlayHeight=0, overlayLeft=0, overlayTop=0;
    OFBool overlayROI=OFFalse;
    Uint16 overlayTransp=0;
    char overlayfile[100];
    FILE *ofile=NULL;

    max = ps.getNumberOfActiveOverlays(layer);
    COUT << "  Number of activated overlays: " << max << endl;
    for (size_t ovlidx=0; ovlidx<max; ovlidx++)
    {
      COUT << "      overlay " << ovlidx+1 << ": group=0x" << hex
           << ps.getActiveOverlayGroup(layer, ovlidx) << dec << " label=\"";
      c=ps.getActiveOverlayLabel(layer, ovlidx);
      if (c) COUT << c; else COUT << "(none)";
      COUT << "\" description=\"";
      c=ps.getActiveOverlayDescription(layer, ovlidx);
      if (c) COUT << c; else COUT << "(none)";
      COUT << "\" type=";
      if (ps.activeOverlayIsROI(layer, ovlidx)) COUT << "ROI"; else COUT << "graphic";
      COUT << endl;

      /* get overlay data */
      if (EC_Normal == ps.getOverlayData(layer, ovlidx, overlayData, overlayWidth, overlayHeight,
          overlayLeft, overlayTop, overlayROI, overlayTransp))
      {
      	COUT << "        columns=" << overlayWidth << " rows=" << overlayHeight << " left="
      	<< overlayLeft << " top=" << overlayTop << endl;
      	sprintf(overlayfile, "ovl_%02d%02d.pgm", (int)layer+1, (int)ovlidx+1);
      	COUT << "        filename=\"" << overlayfile << "\"";

        ofile = fopen(overlayfile, "wb");
        if (ofile)
        {
           fprintf(ofile, "P5\n%d %d 255\n", overlayWidth, overlayHeight);
           fwrite(overlayData, overlayWidth, overlayHeight, ofile);
           fclose(ofile);
           COUT << " - written." << endl;
        } else COUT << " -write error-" << endl;
      } else {
      	COUT << "        unable to access overlay data!" << endl;
      }
    }
  }

  COUT << endl;

  max = ps.getNumberOfVOILUTsInImage();
  COUT << "VOI LUTs available in attached image: " << max << endl;
  for (size_t lutidx=0; lutidx<max; lutidx++)
  {
    COUT << "  LUT #" << lutidx+1 << ": description=";
    c=ps.getDescriptionOfVOILUTsInImage(lutidx);
    if (c) COUT << c << endl; else COUT << "(none)" << endl;
  }

  max = ps.getNumberOfVOIWindowsInImage();
  COUT << "VOI windows available in attached image: " << max << endl;
  for (size_t winidx=0; winidx<max; winidx++)
  {
    COUT << "  Window #" << winidx+1 << ": description=";
    c=ps.getDescriptionOfVOIWindowsInImage(winidx);
    if (c) COUT << c << endl; else COUT << "(none)" << endl;
  }

  max = ps.getNumberOfOverlaysInImage();
  COUT << "Overlays available (non-shadowed) in attached image: " << max << endl;
  for (size_t oidx=0; oidx<max; oidx++)
  {
    COUT << "  Overlay #" << oidx+1 << ": group=0x" << hex << ps.getOverlayInImageGroup(oidx) << dec << " label=\"";
    c=ps.getOverlayInImageLabel(oidx);
    if (c) COUT << c; else COUT << "(none)";
    COUT << "\" description=\"";
    c=ps.getOverlayInImageDescription(oidx);
    if (c) COUT << c; else COUT << "(none)";
    COUT << "\" type=";
    if (ps.overlayInImageIsROI(oidx)) COUT << "ROI"; else COUT << "graphic";
    COUT << endl;
  }
  COUT << endl;

  Uint32 numberOfPeers = dvi.getNumberOfTargets();
  COUT << "Communication peers (defined in config file): " << numberOfPeers << endl;
  for (Uint32 cpi=0; cpi<numberOfPeers; cpi++)
  {
    COUT << "  Peer " << cpi+1 << ": ID='" << dvi.getTargetID(cpi) << "' description='"
         << dvi.getTargetDescription(dvi.getTargetID(cpi)) << "'" << endl;
  }
  COUT << endl;

}


/*
 * CVS/RCS Log:
 * $Log: dcmp2pgm.cc,v $
 * Revision 1.21  2000-05-03 14:27:26  meichel
 * Updated dcmpstat apps for changes in dcmimgle.
 *
 * Revision 1.20  2000/03/08 16:28:41  meichel
 * Updated copyright header.
 *
 * Revision 1.19  2000/03/06 18:21:45  joergr
 * Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
 *
 * Revision 1.18  2000/03/03 14:13:25  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.17  1999/10/20 10:44:51  joergr
 * Replaced option --no-output by an optional output parameter (filename).
 * Minor corrections.
 *
 * Revision 1.16  1999/09/01 16:13:53  meichel
 * Fixed pixel aspect ratio computation in dcmp2pgm for rotated images.
 *
 * Revision 1.15  1999/07/27 15:41:32  meichel
 * Adapted dcmpstat tools to supplement 33 letter ballot changes.
 *
 * Revision 1.14  1999/05/03 14:16:37  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.13  1999/04/30 16:40:45  meichel
 * Minor code purifications to keep Sun CC 2.0.1 quiet
 *
 * Revision 1.12  1999/04/28 15:45:06  meichel
 * Cleaned up module dcmpstat apps, adapted to new command line class
 *   and added short documentation.
 *
 * Revision 1.11  1999/04/27 10:57:13  joergr
 * Added new entry to index file: Presentation Description.
 *
 * Revision 1.10  1999/03/22 09:04:57  joergr
 * Added parameter to get value of (transparent) background color for method
 * getOverlayData.
 *
 * Revision 1.9  1999/02/25 18:37:15  joergr
 * Changed formatting of some comments.
 *
 * Revision 1.8  1999/02/23 11:42:08  joergr
 * Added debug code (exclitly delete data dictionary).
 * Added layer number to filename when extracting overlay planes.
 *
 * Revision 1.7  1999/02/08 12:52:16  meichel
 * Removed dummy parameter from DVInterface constructor.
 *
 * Revision 1.6  1999/01/20 19:24:42  meichel
 * Implemented access methods for network communication
 *   related config file entries.
 *
 * Revision 1.5  1999/01/15 17:36:03  meichel
 * added configuration file facility (derived from dcmprint)
 *   and a sample config file.
 *
 * Revision 1.4  1999/01/14 17:50:55  meichel
 * added new command line option --dicom to test application
 *   dcmp2pgm. This demonstrates DVInterface::saveDICOMImage().
 *
 * Revision 1.3  1998/12/22 17:57:02  meichel
 * Implemented Presentation State interface for overlays,
 *   VOI LUTs, VOI windows, curves. Added test program that
 *   allows to add curve data to DICOM images.
 *
 * Revision 1.2  1998/12/14 16:10:24  meichel
 * Implemented Presentation State interface for graphic layers,
 *   text and graphic annotations, presentation LUTs.
 *
 * Revision 1.1  1998/11/27 14:50:19  meichel
 * Initial Release.
 *
 */
