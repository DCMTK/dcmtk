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
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: test ...
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmp2pgm.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "cmdlnarg.h"
#include "ofcmdln.h"

#include "dviface.h"
#include "dvpstx.h"  /* for DVPSTextObject */
#include "dvpsgr.h"  /* for DVPSGraphicObject */
#include "dcmimage.h"


// ********************************************


static void
printHeader()
{
    cerr << "dcmp2pgm: Convert DICOM file to PGM or PPM" << endl;
}


static void
printUsage(const OFCommandLine &cmd)
{
    OFString str;
    cmd.getOptionString(str);
    printHeader();
    cerr << "usage: dcmp2pgm [options] [dcmpst-in] dcmimg-in [pgmfile-out]" << endl;
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

void dumpAnnotations(DVInterface& dvi)
{
  DVPresentationState &ps = dvi.getCurrentPState();
  size_t i, j;
  const char *c;
    
  cout << "DUMPING PRESENTATION STATE ANNOTATIONS" << endl
       << "--------------------------------------" << endl << endl;

  c = ps.getPresentationLabel();
  cout << "Presentation Label: "; if (c) cout << c << endl; else cout << "none" << endl;
  c = ps.getPresentationDescription();
  cout << "Presentation Description: "; if (c) cout << c << endl; else cout << "none" << endl;
  c = ps.getPresentationCreatorsName();
  cout << "Presentation Creator's Name: "; if (c) cout << c << endl; else cout << "none" << endl;

  cout << "Rotation: ";
  switch (ps.getRotation())
  {
    case DVPSR_0_deg:
      cout << "none";
      break;
    case DVPSR_90_deg:
      cout << "90 degrees";
      break;
    case DVPSR_180_deg:
      cout << "180 degrees";
      break;
    case DVPSR_270_deg:
      cout << "270 degrees";
      break;
  }
  cout << endl;
  cout << "Flip: ";
  if (ps.getFlip()) cout << "yes" << endl; else cout << "no" << endl;
  cout << "Displayed area: TLHC=" << ps.getDisplayedAreaTLHC_x() << "\\"
       << ps.getDisplayedAreaTLHC_y() << " BRHC=" << ps.getDisplayedAreaBRHC_x()
       << "\\" << ps.getDisplayedAreaBRHC_y() << endl;
  
  cout << "Rectangular shutter: ";
  if (ps.haveShutter(DVPSU_rectangular))
  {
    cout << "LV=" << ps.getRectShutterLV()
         << " RV=" << ps.getRectShutterRV()
         << " UH=" << ps.getRectShutterUH()
         << " LH=" << ps.getRectShutterLH() << endl;
   
  } else cout << "none" << endl;
  cout << "Circular shutter: ";
  if (ps.haveShutter(DVPSU_circular))
  {
    cout << "center=" << ps.getCenterOfCircularShutter_x()
         << "\\" << ps.getCenterOfCircularShutter_y()
         << " radius=" << ps.getRadiusOfCircularShutter() << endl;  
  } else cout << "none" << endl;
  cout << "Polygonal shutter: ";
  if (ps.haveShutter(DVPSU_polygonal))
  {
     cout << "points=" << ps.getNumberOfPolyShutterVertices() << " coordinates=";
     j = ps.getNumberOfPolyShutterVertices();
     Sint32 polyX, polyY;
     for (i=0; i<j; i++) 
     {
     	if (EC_Normal == ps.getPolyShutterVertex(i, polyX, polyY))
     	{
     	  cout << polyX << "\\" << polyY << ", ";
     	} else cout << "???\\???,";
     }
     cout << endl;
  } else cout << "none" << endl;
  cout << "Bitmap shutter: ";
  if (ps.haveShutter(DVPSU_bitmap))
  {
     cout << "group " << hex << ps.getBitmapShutterGroup() << dec << endl;
  } else cout << "none" << endl;
  cout << "Shutter presentation value: 0x" << hex << ps.getShutterPresentationValue() << dec << endl;
  cout << endl;
  
  ps.sortGraphicLayers();  // to order of display
  for (size_t layer=0; layer<ps.getNumberOfGraphicLayers(); layer++)
  {
    c = ps.getGraphicLayerName(layer);
    cout << "Graphic Layer #" << layer+1 << " ["; if (c) cout << c; else cout << "(unnamed)";
    cout << "]" << endl;
    c = ps.getGraphicLayerDescription(layer);
    cout << "  Description: "; if (c) cout << c << endl; else cout << "none" << endl;
    cout << "  Recomm. display value: ";
    if (ps.haveGraphicLayerRecommendedDisplayValue(layer))
    {
      Uint16 r, g, b;
      if (ps.isGrayGraphicLayerRecommendedDisplayValue(layer))
      {
      	cout << "gray ";
      	if (EC_Normal == ps.getGraphicLayerRecommendedDisplayValueGray(layer, g))
      	{
      	  cout << "0x" << hex << g << dec << endl;
      	} else cout << "error" << endl;
      } else {
      	cout << "color ";
      	if (EC_Normal == ps.getGraphicLayerRecommendedDisplayValueRGB(layer, r, g, b))
      	{
      	  cout << "0x" << hex << r << "\\0x" << g << "\\0x" << b << dec << endl;
      	} else cout << "error" << endl;
      }
    } else cout << "none" << endl;

    // text objects
    size_t max = ps.getNumberOfTextObjects(layer);
    cout << "  Number of text objects:" << max << endl;
    DVPSTextObject *ptext = NULL;
    for (size_t textidx=0; textidx<max; textidx++)
    {
      ptext = ps.getTextObject(layer, textidx);
      if (ptext)
      {
      	// display contents of text object
        cout << "      text " << textidx+1 << ": \"" << ptext->getText() << "\"" << endl;
        cout << "        anchor point: ";
        if (ptext->haveAnchorPoint())
        {
          cout << ptext->getAnchorPoint_x() << "\\" << ptext->getAnchorPoint_y() << " units=";
          if (ptext->getAnchorPointAnnotationUnits()==DVPSA_display) cout << "display"; else cout << "pixel";
          cout << " visible=";
          if (ptext->anchorPointIsVisible()) cout << "yes"; else cout << "no";
          cout << endl;
        } else cout << "none" << endl;
        cout << "        bounding box: ";
        if (ptext->haveBoundingBox())
        {
          cout << "TLHC=";
          cout << ptext->getBoundingBoxTLHC_x() << "\\" << ptext->getBoundingBoxTLHC_y()
               << " BRHC=" << ptext->getBoundingBoxBRHC_x() << "\\" << ptext->getBoundingBoxBRHC_y()
               << " units=";
          if (ptext->getBoundingBoxAnnotationUnits()==DVPSA_display) cout << "display"; else cout << "pixel";
          cout << endl;
        } else cout << "none" << endl;
        cout << "        specific character set: ";
        switch (ptext->getCharset(ps.getCharset())) 
        {
          case DVPSC_ascii: cout << "ASCII 7-bit" << endl; break;
          case DVPSC_latin1: cout << "Latin 1" << endl; break;
          case DVPSC_latin2: cout << "Latin 2" << endl; break;
          case DVPSC_latin3: cout << "Latin 3" << endl; break;
          case DVPSC_latin4: cout << "Latin 4" << endl; break;
          case DVPSC_latin5: cout << "Latin 5" << endl; break;
          case DVPSC_cyrillic: cout << "Cyrillic" << endl; break;
          case DVPSC_arabic: cout << "Arabic" << endl; break;
          case DVPSC_greek: cout << "Greek" << endl; break;
          case DVPSC_hebrew: cout << "Hebrew" << endl; break;
          case DVPSC_japanese: cout << "Japanese" << endl; break;
          default: cout << "Unknown: " << ptext->getCharsetString() << endl; break;
        }
      }
    }

    // graphic objects
    max = ps.getNumberOfGraphicObjects(layer);
    cout << "  Number of graphic objects:" << max << endl;
    DVPSGraphicObject *pgraphic = NULL;
    for (size_t graphicidx=0; graphicidx<max; graphicidx++)
    {
      pgraphic = ps.getGraphicObject(layer, graphicidx);
      if (pgraphic)
      {
      	// display contents of graphic object
        cout << "      graphic " << graphicidx+1 << ": points=" << pgraphic->getNumberOfPoints() 
             << " type=";
        switch (pgraphic->getGraphicType())
        {
          case DVPST_polyline: cout << "polyline filled="; break;
          case DVPST_interpolated: cout << "interpolated filled="; break;
          case DVPST_circle: cout << "circle filled="; break;
          case DVPST_ellipse: cout << "ellipse filled="; break;
        }
        if (pgraphic->isFilled()) cout << "yes units="; else cout << "no units=";
        if (pgraphic->getAnnotationUnits()==DVPSA_display) cout << "display"; else cout << "pixel";
        cout << endl << "        coordinates: ";
        j = pgraphic->getNumberOfPoints();
        Float32 fx=0.0, fy=0.0;
        for (i=0; i<j; i++)
        {
          if (EC_Normal==pgraphic->getPoint(i,fx,fy))
          {
            cout << fx << "\\" << fy << ", ";
          } else cout << "???\\???, ";
        }
        cout << endl;
      }
    }
    
    // curve objects
    
    // overlay objects

  }


 
  
}

// ********************************************

int main(int argc, char *argv[])
{
    OFCommandLine cmd;
    OFString str;

    OFCmdUnsignedInt opt_debugMode      = 0;           /* default: no debug */
    int              opt_suppressOutput = 0;           /* default: create output */
    int              opt_annotation     = 0;           /* default: do not dump annotations */
    const char *opt_pstName = NULL;                    /* pstate file name */
    const char *opt_imgName = NULL;                    /* image file name */
    const char *opt_pgmName = NULL;                    /* pgm file name */
    const char *opt_savName = NULL;                    /* save file name */
    
    SetDebugLevel(( 0 ));
    DicomImageClass::DebugLevel = DicomImageClass::DL_NoMessages;
  
    prepareCmdLineArgs(argc, argv, "dcmp2pgm");
      
    cmd.addGroup("options:");
     cmd.addOption("--help", "print this help screen");

    cmd.addGroup("output options:");
     cmd.addOption("--verbose",     "+V",    "verbose mode, print image details");
     cmd.addOption("--debug-level", "+d", 1, "[n]umber : integer",
                                             "set debug level to n (0..9, default: 0)");
     cmd.addOption("--no-output",   "-f",    "do not create any output (useful with +V)");
     cmd.addOption("--save-pstate", "+S", 1, "[f]ilename",
                                             "save presentation state to file");
     cmd.addOption("--annotation",  "-a",    "dump annotations from presentation state");
     
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
            else if (cmd.getParamCount() > 3)
                printError("Too many arguments");
            else 
            {
                if (cmd.getParamCount() <= 2)
                {
                    cmd.getParam(1, opt_imgName);
                    cmd.getParam(2, opt_pgmName);
                }
                else if (cmd.getParamCount() == 3)
                {
                    cmd.getParam(1, opt_pstName);
                    cmd.getParam(2, opt_imgName);
                    cmd.getParam(3, opt_pgmName);
                }

                if (cmd.findOption("--debug-level"))
                    checkValue(cmd, cmd.getValue(opt_debugMode, 0, 9));
                if (cmd.findOption("--no-output"))
                    opt_suppressOutput = 1;
                if (cmd.findOption("--save-pstate"))
                    checkValue(cmd, cmd.getValue(opt_savName));
                if (cmd.findOption("--annotation"))
                    opt_annotation = 1;
            }
    }

    SetDebugLevel(((int)opt_debugMode));
    DicomImageClass::DebugLevel = opt_debugMode;

    DVInterface dvi;
    E_Condition status = EC_Normal;

    if (opt_pstName == NULL)
    {
        if (opt_debugMode > 0)
            cerr << "reading DICOM image file: " << opt_imgName << endl;
        status = dvi.loadImage(opt_imgName);
    } else {
        if (opt_debugMode > 0)
        {
            cerr << "reading DICOM pstate file: " << opt_pstName << endl;
            cerr << "reading DICOM image file: " << opt_imgName << endl;
        }
        status = dvi.loadPState(opt_pstName, opt_imgName);
    }

    if (status == EC_Normal)
    {
    	if (opt_annotation) dumpAnnotations(dvi);
        if (!opt_suppressOutput)
        {
            const void *pixelData = NULL;
            unsigned long width = 0;
            unsigned long height = 0;
            if (opt_debugMode > 0)
                cerr << "creating pixel data" << endl;
            if ((dvi.getCurrentPState().getPixelData(pixelData, width, height) == EC_Normal) && (pixelData != NULL))
            {
                FILE *outfile = fopen(opt_pgmName, "wb");
                if (outfile)
                {
                    if (opt_debugMode > 0)
                        cerr << "writing PGM file: " << opt_pgmName << endl;
                    fprintf(outfile, "P5\n%ld %ld 255\n", width, height);
                    fwrite(pixelData, width, height, outfile);
                    fclose(outfile);
                } else
                    printError("Can't create output file.");
            } else
                printError("Can't create output data.");
        }
        if (opt_savName != NULL)
        {
            if (opt_debugMode > 0)
                cerr << "writing pstate file: " << opt_savName << endl;
            if (dvi.savePState(opt_savName) != EC_Normal)
                printError("Can't write pstate file.");
        }
    } else
        printError("Can't open input file(s).");
    
    return (status != EC_Normal);
}


/*
** CVS/RCS Log:
** $Log: dcmp2pgm.cc,v $
** Revision 1.2  1998-12-14 16:10:24  meichel
** Implemented Presentation State interface for graphic layers,
**   text and graphic annotations, presentation LUTs.
**
** Revision 1.1  1998/11/27 14:50:19  meichel
** Initial Release.
**
**
**
*/
