/*
 *
 *  Copyright (C) 1998-2017, OFFIS e.V.
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
 *  Authors: Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose
 *    sample application that reads a DICOM image and (optionally)
 *    a presentation state and creates a PGM bitmap using the settings
 *    of the presentation state. Non-grayscale transformations are
 *    ignored. If no presentation state is loaded, a default is created.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmpstat/dviface.h"
#include "dcmtk/dcmpstat/dvpstx.h"    /* for DVPSTextObject */
#include "dcmtk/dcmpstat/dvpsgr.h"    /* for DVPSGraphicObject */
#include "dcmtk/dcmpstat/dvpscu.h"    /* for DVPSCurve */
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmp2pgm"

static OFLogger dcmp2pgmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


static void dumpPresentationState(DVPresentationState &ps)
{
  size_t i, j, max;
  const char *c;

  OFOStringStream oss;

  oss << "DUMPING PRESENTATION STATE" << OFendl
       << "--------------------------" << OFendl << OFendl;

  c = ps.getPresentationLabel();
  oss << "Presentation Label: "; if (c) oss << c << OFendl; else oss << "none" << OFendl;
  c = ps.getPresentationDescription();
  oss << "Presentation Description: "; if (c) oss << c << OFendl; else oss << "none" << OFendl;
  c = ps.getPresentationCreatorsName();
  oss << "Presentation Creator's Name: "; if (c) oss << c << OFendl; else oss << "none" << OFendl;

  oss << "VOI transformation: ";
  if (ps.haveActiveVOIWindow())
  {
    double width=0.0, center=0.0;
    ps.getCurrentWindowWidth(width);
    ps.getCurrentWindowCenter(center);
    oss << "window center=" << center << " width=" << width << " description=\"";
    c = ps.getCurrentVOIDescription();
    if (c) oss << c << "\"" << OFendl; else oss << "(none)\"" << OFendl;
  }
  else if (ps.haveActiveVOILUT())
  {
    oss << "lut description=\"";
    c = ps.getCurrentVOIDescription();
    if (c) oss << c << "\"" << OFendl; else oss << "(none)\"" << OFendl;
  }
  else oss << "none" << OFendl;

  oss << "Rotation: ";
  switch (ps.getRotation())
  {
    case DVPSR_0_deg:
      oss << "none";
      break;
    case DVPSR_90_deg:
      oss << "90 degrees";
      break;
    case DVPSR_180_deg:
      oss << "180 degrees";
      break;
    case DVPSR_270_deg:
      oss << "270 degrees";
      break;
  }
  oss << OFendl;
  oss << "Flip: ";
  if (ps.getFlip()) oss << "yes" << OFendl; else oss << "no" << OFendl;

  Sint32 tlhcX=0;
  Sint32 tlhcY=0;
  Sint32 brhcX=0;
  Sint32 brhcY=0;
  oss << "Displayed area:" << OFendl;

  DVPSPresentationSizeMode sizemode = ps.getDisplayedAreaPresentationSizeMode();
  double factor=1.0;
  switch (sizemode)
  {
    case DVPSD_scaleToFit:
      oss << "  presentation size mode: SCALE TO FIT" << OFendl;
      break;
    case DVPSD_trueSize:
      oss << "  presentation size mode: TRUE SIZE" << OFendl;
      break;
    case DVPSD_magnify:
      ps.getDisplayedAreaPresentationPixelMagnificationRatio(factor);
      oss << "  presentation size mode: MAGNIFY factor=" << factor << OFendl;
      break;
  }
  ps.getStandardDisplayedArea(tlhcX, tlhcY, brhcX, brhcY);
  oss << "  displayed area TLHC=" << tlhcX << "\\" << tlhcY << " BRHC=" << brhcX << "\\" << brhcY << OFendl;

  double x, y;
  if (EC_Normal == ps.getDisplayedAreaPresentationPixelSpacing(x,y))
  {
    oss << "  presentation pixel spacing: X=" << x << "mm Y=" << y << " mm" << OFendl;
  } else {
    oss << "  presentation pixel aspect ratio: " << ps.getDisplayedAreaPresentationPixelAspectRatio() << OFendl;
  }

  oss << "Rectangular shutter: ";
  if (ps.haveShutter(DVPSU_rectangular))
  {
    oss << "LV=" << ps.getRectShutterLV()
         << " RV=" << ps.getRectShutterRV()
         << " UH=" << ps.getRectShutterUH()
         << " LH=" << ps.getRectShutterLH() << OFendl;

  } else oss << "none" << OFendl;
  oss << "Circular shutter: ";
  if (ps.haveShutter(DVPSU_circular))
  {
    oss << "center=" << ps.getCenterOfCircularShutter_x()
         << "\\" << ps.getCenterOfCircularShutter_y()
         << " radius=" << ps.getRadiusOfCircularShutter() << OFendl;
  } else oss << "none" << OFendl;
  oss << "Polygonal shutter: ";
  if (ps.haveShutter(DVPSU_polygonal))
  {
     oss << "points=" << ps.getNumberOfPolyShutterVertices() << " coordinates=";
     j = ps.getNumberOfPolyShutterVertices();
     Sint32 polyX, polyY;
     for (i=0; i<j; i++)
     {
        if (EC_Normal == ps.getPolyShutterVertex(i, polyX, polyY))
        {
          oss << polyX << "\\" << polyY << ", ";
        } else oss << "???\\???,";
     }
     oss << OFendl;
  } else oss << "none" << OFendl;
  oss << "Bitmap shutter: ";
  if (ps.haveShutter(DVPSU_bitmap))
  {
     oss << "present" << OFendl;
  } else oss << "none" << OFendl;
  oss << "Shutter presentation value: 0x" << STD_NAMESPACE hex << ps.getShutterPresentationValue() << STD_NAMESPACE dec << OFendl;
  oss << OFendl;

  ps.sortGraphicLayers();  // to order of display
  for (size_t layer=0; layer<ps.getNumberOfGraphicLayers(); layer++)
  {
    c = ps.getGraphicLayerName(layer);
    oss << "Graphic Layer #" << layer+1 << " ["; if (c) oss << c; else oss << "(unnamed)";
    oss << "]" << OFendl;
    c = ps.getGraphicLayerDescription(layer);
    oss << "  Description: "; if (c) oss << c << OFendl; else oss << "none" << OFendl;
    oss << "  Recomm. display value: ";
    if (ps.haveGraphicLayerRecommendedDisplayValue(layer))
    {
      Uint16 r, g, b;
      oss << "gray ";
      if (EC_Normal == ps.getGraphicLayerRecommendedDisplayValueGray(layer, g))
      {
        oss << "0x" << STD_NAMESPACE hex << g << STD_NAMESPACE dec << OFendl;
      } else oss << "error" << OFendl;
      oss << "color ";
      if (EC_Normal == ps.getGraphicLayerRecommendedDisplayValueRGB(layer, r, g, b))
      {
        oss << "0x" << STD_NAMESPACE hex << r << "\\0x" << g << "\\0x" << b << STD_NAMESPACE dec << OFendl;
      } else oss << "error" << OFendl;
    } else oss << "none" << OFendl;

    // text objects
    max = ps.getNumberOfTextObjects(layer);
    oss << "  Number of text objects: " << max << OFendl;
    DVPSTextObject *ptext = NULL;
    for (size_t textidx=0; textidx<max; textidx++)
    {
      ptext = ps.getTextObject(layer, textidx);
      if (ptext)
      {
        // display contents of text object
        oss << "      text " << textidx+1 << ": \"" << ptext->getText() << "\"" << OFendl;
        oss << "        anchor point: ";
        if (ptext->haveAnchorPoint())
        {
          oss << ptext->getAnchorPoint_x() << "\\" << ptext->getAnchorPoint_y() << " units=";
          if (ptext->getAnchorPointAnnotationUnits()==DVPSA_display) oss << "display"; else oss << "pixel";
          oss << " visible=";
          if (ptext->anchorPointIsVisible()) oss << "yes"; else oss << "no";
          oss << OFendl;
        } else oss << "none" << OFendl;
        oss << "        bounding box: ";
        if (ptext->haveBoundingBox())
        {
          oss << "TLHC=";
          oss << ptext->getBoundingBoxTLHC_x() << "\\" << ptext->getBoundingBoxTLHC_y()
               << " BRHC=" << ptext->getBoundingBoxBRHC_x() << "\\" << ptext->getBoundingBoxBRHC_y()
               << " units=";
          if (ptext->getBoundingBoxAnnotationUnits()==DVPSA_display) oss << "display"; else oss << "pixel";

          DVPSTextJustification justification = ptext->getBoundingBoxHorizontalJustification();
          oss << " justification=";
          switch (justification)
          {
            case DVPSX_left:
              oss << "left";
              break;
            case DVPSX_right:
              oss << "right";
              break;
            case DVPSX_center:
              oss << "center";
              break;
          }
          oss << OFendl;
        } else oss << "none" << OFendl;
      }
    }

    // graphic objects
    max = ps.getNumberOfGraphicObjects(layer);
    oss << "  Number of graphic objects: " << max << OFendl;
    DVPSGraphicObject *pgraphic = NULL;
    for (size_t graphicidx=0; graphicidx<max; graphicidx++)
    {
      pgraphic = ps.getGraphicObject(layer, graphicidx);
      if (pgraphic)
      {
        // display contents of graphic object
        oss << "      graphic " << graphicidx+1 << ": points=" << pgraphic->getNumberOfPoints()
             << " type=";
        switch (pgraphic->getGraphicType())
        {
          case DVPST_polyline: oss << "polyline filled="; break;
          case DVPST_interpolated: oss << "interpolated filled="; break;
          case DVPST_circle: oss << "circle filled="; break;
          case DVPST_ellipse: oss << "ellipse filled="; break;
          case DVPST_point: oss << "point filled="; break;
        }
        if (pgraphic->isFilled()) oss << "yes units="; else oss << "no units=";
        if (pgraphic->getAnnotationUnits()==DVPSA_display) oss << "display"; else oss << "pixel";
        oss << OFendl << "        coordinates: ";
        j = pgraphic->getNumberOfPoints();
        Float32 fx=0.0, fy=0.0;
        for (i=0; i<j; i++)
        {
          if (EC_Normal==pgraphic->getPoint(i,fx,fy))
          {
            oss << fx << "\\" << fy << ", ";
          } else oss << "???\\???, ";
        }
        oss << OFendl;
      }
    }

    // curve objects
    max = ps.getNumberOfCurves(layer);
    oss << "  Number of activated curves: " << max << OFendl;
    DVPSCurve *pcurve = NULL;
    for (size_t curveidx=0; curveidx<max; curveidx++)
    {
      pcurve = ps.getCurve(layer, curveidx);
      if (pcurve)
      {
        // display contents of curve
        oss << "      curve " << curveidx+1 << ": points=" << pcurve->getNumberOfPoints()
            << " type=";
        switch (pcurve->getTypeOfData())
        {
          case DVPSL_roiCurve: oss << "roi units="; break;
          case DVPSL_polylineCurve: oss << "poly units="; break;
        }
        c = pcurve->getCurveAxisUnitsX();
        if (c && (strlen(c)>0)) oss << c << "\\"; else oss << "(none)\\";
        c = pcurve->getCurveAxisUnitsY();
        if (c && (strlen(c)>0)) oss << c << OFendl; else oss << "(none)" << OFendl;
        oss << "        label=";
        c = pcurve->getCurveLabel();
        if (c && (strlen(c)>0)) oss << c << " description="; else oss << "(none) description=";
        c = pcurve->getCurveDescription();
        if (c && (strlen(c)>0)) oss << c << OFendl; else oss << "(none)" << OFendl;
        oss << "        coordinates: ";
        j = pcurve->getNumberOfPoints();
        double dx=0.0, dy=0.0;
        for (i=0; i<j; i++)
        {
          if (EC_Normal==pcurve->getPoint(i,dx,dy))
          {
            oss << dx << "\\" << dy << ", ";
          } else oss << "???\\???, ";
        }
        oss << OFendl;
      } else oss << "      curve " << curveidx+1 << " not present in image." << OFendl;
    }

    // overlay objects
    const void *overlayData=NULL;
    unsigned int overlayWidth=0, overlayHeight=0, overlayLeft=0, overlayTop=0;
    OFBool overlayROI=OFFalse;
    Uint16 overlayTransp=0;
    char overlayfile[100];
    FILE *ofile=NULL;

    max = ps.getNumberOfActiveOverlays(layer);
    oss << "  Number of activated overlays: " << max << OFendl;
    for (size_t ovlidx=0; ovlidx<max; ovlidx++)
    {
      oss << "      overlay " << ovlidx+1 << ": group=0x" << STD_NAMESPACE hex
           << ps.getActiveOverlayGroup(layer, ovlidx) << STD_NAMESPACE dec << " label=\"";
      c=ps.getActiveOverlayLabel(layer, ovlidx);
      if (c) oss << c; else oss << "(none)";
      oss << "\" description=\"";
      c=ps.getActiveOverlayDescription(layer, ovlidx);
      if (c) oss << c; else oss << "(none)";
      oss << "\" type=";
      if (ps.activeOverlayIsROI(layer, ovlidx)) oss << "ROI"; else oss << "graphic";
      oss << OFendl;

      /* get overlay data */
      if (EC_Normal == ps.getOverlayData(layer, ovlidx, overlayData, overlayWidth, overlayHeight,
          overlayLeft, overlayTop, overlayROI, overlayTransp))
      {
        oss << "        columns=" << overlayWidth << " rows=" << overlayHeight << " left="
            << overlayLeft << " top=" << overlayTop << OFendl;
        sprintf(overlayfile, "ovl_%02d%02d.pgm", (int)layer+1, (int)ovlidx+1);
        oss << "        filename=\"" << overlayfile << "\"";

        ofile = fopen(overlayfile, "wb");
        if (ofile)
        {
           fprintf(ofile, "P5\n%d %d 255\n", overlayWidth, overlayHeight);
           fwrite(overlayData, overlayWidth, overlayHeight, ofile);
           fclose(ofile);
           oss << " - written." << OFendl;
        } else oss << " -write error-" << OFendl;
      } else {
        oss << "        unable to access overlay data!" << OFendl;
      }
    }
  }

  oss << OFendl;

  max = ps.getNumberOfVOILUTsInImage();
  oss << "VOI LUTs available in attached image: " << max << OFendl;
  for (size_t lutidx=0; lutidx<max; lutidx++)
  {
    oss << "  LUT #" << lutidx+1 << ": description=";
    c=ps.getDescriptionOfVOILUTsInImage(lutidx);
    if (c) oss << c << OFendl; else oss << "(none)" << OFendl;
  }

  max = ps.getNumberOfVOIWindowsInImage();
  oss << "VOI windows available in attached image: " << max << OFendl;
  for (size_t winidx=0; winidx<max; winidx++)
  {
    oss << "  Window #" << winidx+1 << ": description=";
    c=ps.getDescriptionOfVOIWindowsInImage(winidx);
    if (c) oss << c << OFendl; else oss << "(none)" << OFendl;
  }

  max = ps.getNumberOfOverlaysInImage();
  oss << "Overlays available (non-shadowed) in attached image: " << max << OFendl;
  for (size_t oidx=0; oidx<max; oidx++)
  {
    oss << "  Overlay #" << oidx+1 << ": group=0x" << STD_NAMESPACE hex << ps.getOverlayInImageGroup(oidx) << STD_NAMESPACE dec << " label=\"";
    c=ps.getOverlayInImageLabel(oidx);
    if (c) oss << c; else oss << "(none)";
    oss << "\" description=\"";
    c=ps.getOverlayInImageDescription(oidx);
    if (c) oss << c; else oss << "(none)";
    oss << "\" type=";
    if (ps.overlayInImageIsROI(oidx)) oss << "ROI"; else oss << "graphic";
    oss << OFendl;
  }

  oss << OFStringStream_ends;
  OFSTRINGSTREAM_GETSTR(oss, res)
  OFLOG_INFO(dcmp2pgmLogger, res);
  OFSTRINGSTREAM_FREESTR(res)
}


#define SHORTCOL 3
#define LONGCOL 13

int main(int argc, char *argv[])
{
    OFBool opt_dump_pstate     = OFFalse;                 /* default: do not dump presentation state */
    OFBool opt_dicom_mode      = OFFalse;                 /* default: create PGM, not DICOM SC */
    OFCmdUnsignedInt opt_frame = 1;                       /* default: first frame */
    const char *opt_pstName    = NULL;                    /* pstate read file name */
    const char *opt_imgName    = NULL;                    /* image read file name */
    const char *opt_pgmName    = NULL;                    /* pgm save file name */
    const char *opt_savName    = NULL;                    /* pstate save file name */
    const char *opt_cfgName    = NULL;                    /* config read file name */

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Read DICOM image and presentation state and render bitmap", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("dcmfile-in", "input DICOM image");
    cmd.addParam("bitmap-out", "output DICOM image or PGM bitmap", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:");
     cmd.addOption("--help",        "-h",    "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",              "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("processing options:");
     cmd.addOption("--pstate",      "-p", 1, "[f]ilename: string",
                                             "process using presentation state file");
     cmd.addOption("--config",      "-c", 1, "[f]ilename: string",
                                             "process using settings from configuration file");
     cmd.addOption("--frame",       "-f", 1, "[f]rame: integer",
                                             "process using image frame f (default: 1)");

    cmd.addGroup("output format:");
     cmd.addOption("--pgm",         "-D",    "save image as PGM (default)");
     cmd.addOption("--dicom",       "+D",    "save image as DICOM secondary capture");

    cmd.addGroup("output options:");
     cmd.addOption("--save-pstate", "+S", 1, "[f]ilename: string",
                                             "save presentation state to file");
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

      /* command line parameters and options */
      cmd.getParam(1, opt_imgName);
      if (cmd.getParamCount() > 1)
        cmd.getParam(2, opt_pgmName);

      OFLog::configureFromCommandLine(cmd, app);

      opt_dump_pstate = dcmp2pgmLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL);

      if (cmd.findOption("--pstate"))      app.checkValue(cmd.getValue(opt_pstName));
      if (cmd.findOption("--config"))      app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--frame"))       app.checkValue(cmd.getValueAndCheckMin(opt_frame, 1));
      if (cmd.findOption("--pgm"))         opt_dicom_mode = OFFalse;
      if (cmd.findOption("--dicom"))       opt_dicom_mode = OFTrue;
      if (cmd.findOption("--save-pstate")) app.checkValue(cmd.getValue(opt_savName));
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmp2pgmLogger, rcsid << OFendl);

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        OFLOG_FATAL(dcmp2pgmLogger, "can't open configuration file '" << opt_cfgName << "'");
        return 10;
      }
    }
    DVInterface dvi(opt_cfgName);
    OFCondition status = EC_Normal;

    if (opt_pstName == NULL)
    {
        OFLOG_DEBUG(dcmp2pgmLogger, "reading DICOM image file: " << opt_imgName);
        status = dvi.loadImage(opt_imgName);
    } else {
        OFLOG_DEBUG(dcmp2pgmLogger, "reading DICOM pstate file: " << opt_pstName);
        OFLOG_DEBUG(dcmp2pgmLogger, "reading DICOM image file: " << opt_imgName);
        status = dvi.loadPState(opt_pstName, opt_imgName);
    }

    if (status == EC_Normal)
    {
        if (opt_dump_pstate) dumpPresentationState(dvi.getCurrentPState());
        if (opt_pgmName != NULL)
        {
            const void *pixelData = NULL;
            unsigned long width = 0;
            unsigned long height = 0;
            OFLOG_DEBUG(dcmp2pgmLogger, "creating pixel data");
            if ((opt_frame > 0) && (dvi.getCurrentPState().selectImageFrameNumber(opt_frame) != EC_Normal))
               OFLOG_ERROR(dcmp2pgmLogger, "cannot select frame " << opt_frame);
            if ((dvi.getCurrentPState().getPixelData(pixelData, width, height) == EC_Normal) && (pixelData != NULL))
            {
              if (opt_dicom_mode)
              {
                double pixelAspectRatio = dvi.getCurrentPState().getPrintBitmapPixelAspectRatio(); // works for rotated images
                OFLOG_DEBUG(dcmp2pgmLogger, "writing DICOM SC file: " << opt_pgmName);
                if (EC_Normal != dvi.saveDICOMImage(opt_pgmName, pixelData, width, height, pixelAspectRatio))
                {
                  OFLOG_ERROR(dcmp2pgmLogger, "error during creation of DICOM file");
                }
              } else {
                FILE *outfile = fopen(opt_pgmName, "wb");
                if (outfile)
                {
                    OFLOG_DEBUG(dcmp2pgmLogger, "writing PGM file: " << opt_pgmName);
                    fprintf(outfile, "P5\n%ld %ld 255\n", width, height);
                    fwrite(pixelData, (size_t)width, (size_t)height, outfile);
                    fclose(outfile);
                } else {
                    OFLOG_FATAL(dcmp2pgmLogger, "Can't create output file.");
                    return 10;
                }
              }
            } else {
                OFLOG_FATAL(dcmp2pgmLogger, "Can't create output data.");
                return 10;
            }
        }
        if (opt_savName != NULL)
        {
            OFLOG_DEBUG(dcmp2pgmLogger, "writing pstate file: " << opt_savName);
            if (dvi.savePState(opt_savName, OFFalse) != EC_Normal)
            {
                OFLOG_FATAL(dcmp2pgmLogger, "Can't write pstate file.");
                return 10;
            }
        }
    } else {
        OFLOG_FATAL(dcmp2pgmLogger, "Can't open input file(s).");
        return 10;
    }

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return (status != EC_Normal);
}
