/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: export display curves to a text file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:36 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"

#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofcmdln.h"

#include "dcmtk/dcmimgle/digsdfn.h"
#include "dcmtk/dcmimgle/diciefn.h"

#define OFFIS_CONSOLE_APPLICATION "dcmdspfn"

static OFLogger dcmdspfnLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 3
#define LONGCOL  15

// ********************************************


int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Export standard display curves to a text file", rcsid);
    OFCommandLine cmd;

    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
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
     cmd.addOption("--help",          "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                 "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("input options: (mutually exclusive)");
     cmd.addOption("--monitor-file",  "+Im", 1, "[f]ilename: string",
                                                "text file describing the monitor characteristics");
     cmd.addOption("--camera-file",   "+Ic", 1, "[f]ilename: string",
                                                "text file describing the camera characteristics");
     cmd.addOption("--printer-file",  "+Ip", 1, "[f]ilename: string",
                                                "text file describing the printer characteristics");
     cmd.addOption("--scanner-file",  "+Is", 1, "[f]ilename: string",
                                                "text file describing the scanner characteristics");
     cmd.addOption("--lum-range",     "+Il", 2, "[m]in max: float",
                                                "minimum and maximum luminance (cd/m^2)");
     cmd.addOption("--od-range",      "+Io", 2, "[m]in max: float",
                                                "minimum and maximum optical density (OD),\nautomatically converted to luminance");

    cmd.addGroup("creation options:");
     cmd.addOption("--ambient-light", "+Ca", 1, "[a]mbient light: float",
                                                "ambient light value (cd/m^2, default: file f)");
     cmd.addOption("--illumination",  "+Ci", 1, "[i]llumination: float",
                                                "illumination value (cd/m^2, default: file f)");
     cmd.addOption("--min-density",   "+Dn", 1, "[m]inimum optical density: float",
                                                "Dmin value (default: off, only with +Ip and +Io)");
     cmd.addOption("--max-density",   "+Dx", 1, "[m]aximum optical density: float",
                                                "Dmax value (default: off, only with +Ip and +Io)");
     cmd.addOption("--ddl-count",     "+Cd", 1, "[n]umber of DDLs: integer",
                                                "number of Device Driving Levels\n(default: 256, only with --lum/od-range)");
     cmd.addOption("--curve-fitting", "+Cf", 1, "[n]umber : integer",
                                                "use polynomial curve fitting algorithm with order n\n(0..99, default: file setting or cubic spline)");

    cmd.addGroup("output options:");
     cmd.addOption("--gsdf",          "+Og", 1, "[f]ilename: string",
                                                "write GSDF curve data to file f");
     cmd.addOption("--cielab",        "+Oc", 1, "[f]ilename: string",
                                                "write CIELAB curve data to file f");

    if (app.parseCommandLine(cmd, argc, argv))
    {
        /* print help text and exit */
        if (cmd.getArgCount() == 0)
            app.printUsage();

        /* check exclusive options */
        if (cmd.hasExclusiveOption())
        {
            if (cmd.findOption("--version"))
            {
                app.printHeader(OFTrue /*print host identifier*/);
                COUT << OFendl << "External libraries used: none" << OFendl;
                return 0;
           }
        }

        OFLog::configureFromCommandLine(cmd, app);

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
            ++opt_outputMode;
        if (cmd.findOption("--cielab"))
            ++opt_outputMode;
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmdspfnLogger, rcsid << OFendl);

    if (opt_outputMode > 0)
    {
        if (opt_ifname != NULL)
            OFLOG_INFO(dcmdspfnLogger, "reading LUT file: " << opt_ifname);

        /* Grayscale Standard Display Function */
        if (cmd.findOption("--gsdf"))
        {
            OFLOG_INFO(dcmdspfnLogger, "creating GSDF display curve ...");
            app.checkValue(cmd.getValue(opt_ofname));
            DiGSDFunction *disp = NULL;
            if (opt_ifname != NULL)
                disp = new DiGSDFunction(opt_ifname, deviceType, OFstatic_cast(signed int, opt_polyOrder));
            else
                disp = new DiGSDFunction(opt_minVal, opt_maxVal, opt_ddlCount, DiDisplayFunction::EDT_Monitor,
                                        OFstatic_cast(signed int, opt_polyOrder));
            if ((disp != NULL) && disp->isValid())
            {
                if (opt_ambLight >= 0)
                {
                    OFLOG_INFO(dcmdspfnLogger, "setting ambient light value ...");
                    disp->setAmbientLightValue(opt_ambLight);
                }
                if (opt_illum >= 0)
                {
                    OFLOG_INFO(dcmdspfnLogger, "setting illumination value ...");
                    disp->setIlluminationValue(opt_illum);
                }
                /* Dmin/max only suppoted for printers */
                if (disp->getDeviceType() == DiDisplayFunction::EDT_Printer)
                {
                    if (opt_Dmin >= 0)
                    {
                        OFLOG_INFO(dcmdspfnLogger, "setting minimum optical density value ...");
                        disp->setMinDensityValue(opt_Dmin);
                    }
                    if (opt_Dmax >= 0)
                    {
                        OFLOG_INFO(dcmdspfnLogger, "setting maximum optical density value ...");
                        disp->setMaxDensityValue(opt_Dmax);
                    }
                }
                OFLOG_INFO(dcmdspfnLogger, "writing output file: " << opt_ofname);
                if (!disp->writeCurveData(opt_ofname, opt_ifname != NULL))
                {
                    OFLOG_FATAL(dcmdspfnLogger, "can't write output file");
                    return 1;
                }
            } else {
                OFLOG_FATAL(dcmdspfnLogger, "can't create display curve");
                return 1;
            }
            delete disp;
        }

        /* CIELAB display function */
        if (cmd.findOption("--cielab"))
        {
            OFLOG_INFO(dcmdspfnLogger, "creating CIELAB display curve ...");
            app.checkValue(cmd.getValue(opt_ofname));
            DiCIELABFunction *disp = NULL;
            if (opt_ifname != NULL)
                disp = new DiCIELABFunction(opt_ifname, deviceType, OFstatic_cast(signed int, opt_polyOrder));
            else
                disp = new DiCIELABFunction(opt_minVal, opt_maxVal, opt_ddlCount, DiDisplayFunction::EDT_Monitor,
                                            OFstatic_cast(signed int, opt_polyOrder));
            if ((disp != NULL) && disp->isValid())
            {
                if (opt_ambLight >= 0)
                {
                    OFLOG_INFO(dcmdspfnLogger, "setting ambient light value ...");
                    disp->setAmbientLightValue(opt_ambLight);
                }
                /* Dmin/max only suppoted for printers */
                if (disp->getDeviceType() == DiDisplayFunction::EDT_Printer)
                {
                    if (opt_Dmin >= 0)
                    {
                        OFLOG_INFO(dcmdspfnLogger, "setting minimum optical density value ...");
                        disp->setMinDensityValue(opt_Dmin);
                    }
                    if (opt_Dmax >= 0)
                    {
                        OFLOG_INFO(dcmdspfnLogger, "setting maximum optical density value ...");
                        disp->setMaxDensityValue(opt_Dmax);
                    }
                }
                OFLOG_INFO(dcmdspfnLogger, "writing output file: " << opt_ofname);
                if (!disp->writeCurveData(opt_ofname, opt_ifname != NULL))
                {
                    OFLOG_FATAL(dcmdspfnLogger, "can't write output file");
                    return 1;
                }
            } else {
                OFLOG_FATAL(dcmdspfnLogger, "can't create display curve");
                return 1;
            }
        }
    } else {
        OFLOG_WARN(dcmdspfnLogger, "nothing to do, no output file specified");
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dcmdspfn.cc,v $
 * Revision 1.26  2010-10-14 13:13:36  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.25  2009-10-28 09:53:39  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.24  2009-04-21 14:05:12  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.23  2008-09-25 14:47:30  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.22  2008-09-25 12:44:39  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.21  2006/08/15 16:30:09  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.20  2006/07/27 14:01:53  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Option "--help" is no longer an exclusive option
 * by default.
 *
 * Revision 1.19  2005/12/08 15:42:39  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.18  2003/12/23 15:40:13  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.17  2003/12/08 19:23:33  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Updated CVS header.
 *
 * Revision 1.16  2003/02/12 11:34:35  joergr
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
