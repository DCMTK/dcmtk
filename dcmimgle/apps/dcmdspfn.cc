/*
 *
 *  Copyright (C) 1996-2003, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: export display curves to a text file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-02-12 11:34:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/apps/dcmdspfn.cc,v $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "cmdlnarg.h"

#include "ofconapp.h"
#include "ofcmdln.h"

#include "digsdfn.h"
#include "diciefn.h"

#define OFFIS_CONSOLE_APPLICATION "dcmdspfn"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 3
#define LONGCOL  15

#define OUTPUT CERR


// ********************************************


int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Export standard display curves to a text file", rcsid);
    OFCommandLine cmd;

    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
    int opt_verboseMode = 1;
    int opt_debugMode = 0;
    int opt_outputMode = 0;
    OFCmdUnsignedInt opt_ddlCount = 256;
    OFCmdSignedInt opt_polyOrder = -1;
    OFCmdFloat opt_ambLight = -1;
    OFCmdFloat opt_illum = -1;
    OFCmdFloat opt_Dmin = -1;
    OFCmdFloat opt_Dmax = -1;
    OFCmdFloat opt_minVal = 0;
    OFCmdFloat opt_maxVal = 0;
    DiDisplayFunction::E_DeviceType deviceType = DiDisplayFunction::EDT_Monitor;

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addGroup("general options:");
     cmd.addOption("--help",          "-h",     "print this help text and exit");
     cmd.addOption("--verbose",       "-v",     "verbose mode, print processing details");
     cmd.addOption("--quiet",         "-q",     "quiet mode, print no warnings and errors");
     cmd.addOption("--debug",         "-d",     "debug mode, print debug information");

    cmd.addGroup("input options: (mutually exclusive)");
     cmd.addOption("--monitor-file",  "+Im", 1, "[f]ilename : string",
                                                "text file describing the monitor characteristics");
     cmd.addOption("--camera-file",   "+Ic", 1, "[f]ilename : string",
                                                "text file describing the camera characteristics");
     cmd.addOption("--printer-file",  "+Ip", 1, "[f]ilename : string",
                                                "text file describing the printer characteristics");
     cmd.addOption("--scanner-file",  "+Is", 1, "[f]ilename : string",
                                                "text file describing the scanner characteristics");
     cmd.addOption("--lum-range",     "+Il", 2, "[m]in max : float",
                                                "minimum and maximum luminance (cd/m^2)");
     cmd.addOption("--od-range",      "+Io", 2, "[m]in max : float",
                                                "minimum and maximum optical density (OD),\nautomatically converted to luminance");

    cmd.addGroup("creation options:");
     cmd.addOption("--ambient-light", "+Ca", 1, "[a]mbient light : float",
                                                "ambient light value (cd/m^2, default: file f)");
     cmd.addOption("--illumination",  "+Ci", 1, "[i]llumination : float",
                                                "illumination value (cd/m^2, default: file f)");
     cmd.addOption("--min-density",   "+Dn", 1, "[m]inimum optical density : float",
                                                "Dmin value (default: off, only with +Ip and +Io)");
     cmd.addOption("--max-density",   "+Dx", 1, "[m]aximum optical density : float",
                                                "Dmax value (default: off, only with +Ip and +Io)");
     cmd.addOption("--ddl-count",     "+Cd", 1, "[n]umber of DDLs : integer",
                                                "number of Device Driving Levels\n(default: 256, only with --lum/od-range)");
     cmd.addOption("--curve-fitting", "+Cf", 1, "[n]umber : integer",
                                                "use polynomial curve fitting algorithm with order n\n(0..99, default: file setting or cubic spline)");

    cmd.addGroup("output options:");
     cmd.addOption("--gsdf",          "+Og", 1, "[f]ilename : string",
                                                "write GSDF curve data to file f");
     cmd.addOption("--cielab",        "+Oc", 1, "[f]ilename : string",
                                                "write CIELAB curve data to file f");

    if (app.parseCommandLine(cmd, argc, argv))
    {
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

        if (cmd.findOption("--ambient-light"))
            app.checkValue(cmd.getValueAndCheckMin(opt_ambLight, 0));
        if (cmd.findOption("--illumination"))
            app.checkValue(cmd.getValueAndCheckMin(opt_illum, 0));
        if (cmd.findOption("--ddl-count"))
        {
            if (opt_ifname != NULL)
            {
                app.checkConflict("--ddl-count", "--monitor-file", OFTrue);
                app.checkConflict("--ddl-count", "--printer-file", OFTrue);
            } else
                app.checkValue(cmd.getValueAndCheckMinMax(opt_ddlCount, 2, 65536));
        }
        if (cmd.findOption("--curve-fitting"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_polyOrder, 0, 99));

        cmd.beginOptionBlock();
        if (cmd.findOption("--monitor-file"))
        {
            app.checkValue(cmd.getValue(opt_ifname));
            deviceType = DiDisplayFunction::EDT_Monitor;
        }
        if (cmd.findOption("--camera-file"))
        {
            app.checkValue(cmd.getValue(opt_ifname));
            deviceType = DiDisplayFunction::EDT_Camera;
        }
        if (cmd.findOption("--printer-file"))
        {
            app.checkValue(cmd.getValue(opt_ifname));
            deviceType = DiDisplayFunction::EDT_Printer;
        }
        if (cmd.findOption("--scanner-file"))
        {
            app.checkValue(cmd.getValue(opt_ifname));
            deviceType = DiDisplayFunction::EDT_Scanner;
        }
        if (cmd.findOption("--lum-range"))
        {
            app.checkValue(cmd.getValueAndCheckMin(opt_minVal, 0));
            app.checkValue(cmd.getValueAndCheckMin(opt_maxVal, opt_minVal, OFFalse));
            deviceType = DiDisplayFunction::EDT_Monitor;
        }
        if (cmd.findOption("--od-range"))
        {
            app.checkDependence("--od-range", "--illumination", opt_illum >= 0);
            OFCmdFloat minVal = 0;
            OFCmdFloat maxVal = 0;
            app.checkValue(cmd.getValueAndCheckMin(minVal, 0));
            app.checkValue(cmd.getValueAndCheckMin(maxVal, minVal, OFFalse));
            deviceType = DiDisplayFunction::EDT_Printer;
            /* convert given optical density to luminance */
            opt_minVal = DiDisplayFunction::convertODtoLum(maxVal, 0 /*ambLight*/, opt_illum);
            opt_maxVal = DiDisplayFunction::convertODtoLum(minVal, 0 /*ambLight*/, opt_illum);
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--min-density"))
        {
            app.checkDependence("--min-density", "--printer-file or --od-range", deviceType == DiDisplayFunction::EDT_Printer);
            app.checkValue(cmd.getValueAndCheckMin(opt_Dmin, 0));
        }
        if (cmd.findOption("--max-density"))
        {
            app.checkDependence("--max-density", "--printer-file or --od-range", deviceType == DiDisplayFunction::EDT_Printer);
            app.checkValue(cmd.getValueAndCheckMin(opt_Dmax, (opt_Dmin < 0) ? 0.0 : opt_Dmin, OFFalse /*incl*/));
        }

        if (cmd.findOption("--gsdf"))
            opt_outputMode++;
        if (cmd.findOption("--cielab"))
            opt_outputMode++;
    }

    /* init verbose and debug mode */
    if (opt_verboseMode == 0)
        DicomImageClass::setDebugLevel(0);
    else {
        DicomImageClass::setDebugLevel(DicomImageClass::DL_Errors |
                                       DicomImageClass::DL_Warnings);
        if (opt_verboseMode == 2)
            DicomImageClass::setDebugLevel(DicomImageClass::getDebugLevel() |
                                           DicomImageClass::DL_Informationals);
        if (opt_debugMode > 0)
            DicomImageClass::setDebugLevel(DicomImageClass::getDebugLevel() |
                                           DicomImageClass::DL_DebugMessages);
    }

    /* print syntax usage */
    if (cmd.getArgCount() == 0)
        app.printUsage();

    if (opt_outputMode > 0)
    {
        if ((opt_verboseMode > 1) && (opt_ifname != NULL))
            OUTPUT << "reading LUT file: " << opt_ifname << endl;

        /* Grayscale Standard Display Function */
        if (cmd.findOption("--gsdf"))
        {
            if (opt_verboseMode > 1)
                OUTPUT << "creating GSDF display curve ..." << endl;
            app.checkValue(cmd.getValue(opt_ofname));
            DiGSDFunction *disp = NULL;
            if (opt_ifname != NULL)
                disp = new DiGSDFunction(opt_ifname, deviceType, (signed int)opt_polyOrder);
            else
                disp = new DiGSDFunction(opt_minVal, opt_maxVal, opt_ddlCount, DiDisplayFunction::EDT_Monitor,
                                        (signed int)opt_polyOrder);
            if ((disp != NULL) && disp->isValid())
            {
                if (opt_ambLight >= 0)
                {
                    if (opt_verboseMode > 1)
                        OUTPUT << "setting ambient light value ..." << endl;
                    disp->setAmbientLightValue(opt_ambLight);
                }
                if (opt_illum >= 0)
                {
                    if (opt_verboseMode > 1)
                        OUTPUT << "setting illumination value ..." << endl;
                    disp->setIlluminationValue(opt_illum);
                }
                /* Dmin/max only suppoted for printers */
                if (disp->getDeviceType() == DiDisplayFunction::EDT_Printer)
                {
                    if (opt_Dmin >= 0)
                    {
                        if (opt_verboseMode > 1)
                            OUTPUT << "setting minimum optical density value ..." << endl;
                        disp->setMinDensityValue(opt_Dmin);
                    }
                    if (opt_Dmax >= 0)
                    {
                        if (opt_verboseMode > 1)
                            OUTPUT << "setting maximum optical density value ..." << endl;
                        disp->setMaxDensityValue(opt_Dmax);
                    }
                }
                if (opt_verboseMode > 1)
                    OUTPUT << "writing output file: " << opt_ofname << endl;
                if (!disp->writeCurveData(opt_ofname, opt_ifname != NULL))
                    app.printError("can't write output file");
            } else
                app.printError("can't create display curve");
            delete disp;
        }

        /* CIELAB display function */
        if (cmd.findOption("--cielab"))
        {
            if (opt_verboseMode > 1)
                OUTPUT << "creating CIELAB display curve ..." << endl;
            app.checkValue(cmd.getValue(opt_ofname));
            DiCIELABFunction *disp = NULL;
            if (opt_ifname != NULL)
                disp = new DiCIELABFunction(opt_ifname, deviceType, (signed int)opt_polyOrder);
            else
                disp = new DiCIELABFunction(opt_minVal, opt_maxVal, opt_ddlCount, DiDisplayFunction::EDT_Monitor,
                                           (signed int)opt_polyOrder);
            if ((disp != NULL) && disp->isValid())
            {
                if (opt_ambLight >= 0)
                {
                    if (opt_verboseMode > 1)
                        OUTPUT << "setting ambient light value ..." << endl;
                    disp->setAmbientLightValue(opt_ambLight);
                }
                /* Dmin/max only suppoted for printers */
                if (disp->getDeviceType() == DiDisplayFunction::EDT_Printer)
                {
                    if (opt_Dmin >= 0)
                    {
                        if (opt_verboseMode > 1)
                            OUTPUT << "setting minimum optical density value ..." << endl;
                        disp->setMinDensityValue(opt_Dmin);
                    }
                    if (opt_Dmax >= 0)
                    {
                        if (opt_verboseMode > 1)
                            OUTPUT << "setting maximum optical density value ..." << endl;
                        disp->setMaxDensityValue(opt_Dmax);
                    }
                }
                if (opt_verboseMode > 1)
                    OUTPUT << "writing output file: " << opt_ofname << endl;
                if (!disp->writeCurveData(opt_ofname, opt_ifname != NULL))
                    app.printError("can't write output file");
            } else
                app.printError("can't create display curve");
        }
    } else {
        if (opt_verboseMode > 0)
            OUTPUT << "nothing to do, no output file specified" << endl;
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dcmdspfn.cc,v $
 * Revision 1.16  2003-02-12 11:34:35  joergr
 * Added Dmin/max support to CIELAB calibration routines.
 *
 * Revision 1.15  2003/02/11 16:38:50  joergr
 * Removed debug/test code.
 *
 * Revision 1.14  2003/02/11 16:31:56  joergr
 * Added two new functions to determine the luminance/OD value of a particular
 * DDL according to the device's characteristic curve and vice versa.
 *
 * Revision 1.13  2003/02/11 10:00:34  joergr
 * Added support for Dmin/max to calibration routines (required for printers).
 *
 * Revision 1.12  2002/07/18 12:23:11  joergr
 * Added support for hardcopy and softcopy input devices (camera and scanner).
 *
 * Revision 1.11  2002/07/05 10:35:57  joergr
 * Modified description of command line options.
 *
 * Revision 1.10  2002/07/03 13:49:54  joergr
 * Fixed inconsistencies regarding the handling of ambient light.
 *
 * Revision 1.9  2002/07/02 16:21:52  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.8  2001/11/09 16:22:29  joergr
 * Renamed some of the getValue/getParam methods to avoid ambiguities reported
 * by certain compilers.
 *
 * Revision 1.7  2001/06/01 15:49:38  meichel
 * Updated copyright header
 *
 * Revision 1.6  2000/04/28 12:31:32  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.5  2000/04/14 16:31:24  meichel
 * Adapted to changed parameter list for command line class
 *
 * Revision 1.4  2000/03/08 16:24:10  meichel
 * Updated copyright header.
 *
 * Revision 1.3  2000/03/03 14:09:08  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.2  1999/10/18 15:04:16  joergr
 * Enhanced command line tool dcmdspfn (add new options).
 *
 * Revision 1.1  1999/10/08 15:40:08  joergr
 * Merged 'dcmgsdf' and 'dccielab' into one application.
 *
 *
 *
 */
