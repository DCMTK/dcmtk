/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Update Date:      $Date: 1999-10-18 15:04:16 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/apps/dcmdspfn.cc,v $
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
#define LONGCOL  11

#define OUTPUT cerr


// ********************************************


int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Export standard display curves to a text file", rcsid, &OUTPUT);
    OFCommandLine cmd;

    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
    int opt_verboseMode = 1;
    int opt_debugMode = 0;
    int opt_outputMode = 0;
    OFCmdUnsignedInt opt_ddlCount = 256;
    OFCmdFloat opt_ambLight = 0;
    OFCmdFloat opt_minLum = 0;
    OFCmdFloat opt_maxLum = 0;

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addGroup("general options:");
     cmd.addOption("--help",       "-h",     "print this help text and exit");
     cmd.addOption("--verbose",    "-v",     "verbose mode, print processing details");
     cmd.addOption("--quiet",      "-q",     "quiet mode, print no warnings and errors");
     cmd.addOption("--debug",      "-d",     "debug mode, print debug information");

    cmd.addGroup("input options:");
     cmd.addOption("--lut-file",   "+If", 1, "[f]ilename : string",
                                             "text file describing the monitor characteristic");
     cmd.addOption("--lum-range",  "+Il", 2, "[m]in max : float",
                                             "minimum and maximum luminance (cd/m^2)");

    cmd.addGroup("creation options:");
     cmd.addOption("--amb-light",  "+Ca", 1, "[a]mbient light : float",
                                             "ambient light value (cd/m^2, default: 0)");
     cmd.addOption("--ddl-count",  "+Cd", 1, "[n]umber of DDLs : integer",
                                             "number of Device Driving Levels (def.: 256, only with +Il)");

    cmd.addGroup("output options:");
     cmd.addOption("--gsdf",       "+Og", 1, "[f]ilename : string",
                                             "write GSDF curve data to file f");
     cmd.addOption("--cielab",     "+Oc", 1, "[f]ilename : string",
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

        cmd.beginOptionBlock();
        if (cmd.findOption("--lut-file"))
            app.checkValue(cmd.getValue(opt_ifname));
        if (cmd.findOption("--lum-range"))
        {
            app.checkValue(cmd.getValue(opt_minLum, 0));
            app.checkValue(cmd.getValue(opt_maxLum, opt_minLum, OFFalse));
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--amb-light"))
            app.checkValue(cmd.getValue(opt_ambLight, 0));
        if (cmd.findOption("--ddl-count"))
        {
            if (opt_ifname != NULL)
                app.checkConflict("--ddl-count", "--lut-file", OFTrue);
            else
                app.checkValue(cmd.getValue(opt_ddlCount, (OFCmdUnsignedInt)2, (OFCmdUnsignedInt)65536));
        }
        
        if (cmd.findOption("--gsdf"))
            opt_outputMode++;
        if (cmd.findOption("--cielab"))
            opt_outputMode++;
    }

    if (opt_verboseMode < 1)
        DicomImageClass::DebugLevel = 0;
    else if (opt_debugMode > 0)
        DicomImageClass::DebugLevel |= DicomImageClass::DL_DebugMessages | DicomImageClass::DL_Informationals;

    if (opt_outputMode > 0)
    {
        if ((opt_verboseMode > 1) && (opt_ifname != NULL))
            OUTPUT << "reading LUT file: " << opt_ifname << endl;

        if (cmd.findOption("--gsdf"))
        {
            if (opt_verboseMode > 1)
                OUTPUT << "creating GSDF display curve ..." << endl;
            app.checkValue(cmd.getValue(opt_ofname));
            DiGSDFunction *disp = NULL;
            if (opt_ifname != NULL)
                disp = new DiGSDFunction(opt_ifname);
            else
                disp = new DiGSDFunction(opt_minLum, opt_maxLum, opt_ddlCount);
            if ((disp != NULL) && disp->isValid())
            {                
                if (opt_ambLight > 0)
                {
                    if (opt_verboseMode > 1)
                        OUTPUT << "setting ambient light value ..." << endl;
                    disp->setAmbientLightValue(opt_ambLight);
                }
                if (opt_verboseMode > 1)
                    OUTPUT << "writing output file: " << opt_ofname << endl;
                if (!disp->writeCurveData(opt_ofname, opt_ifname != NULL))
                    app.printError("can't write output file");
            } else
                app.printError("can't create display curve");
            delete disp;
        }

        if (cmd.findOption("--cielab"))
        {
            if (opt_verboseMode > 1)
                OUTPUT << "creating CIELAB display curve ..." << endl;
            app.checkValue(cmd.getValue(opt_ofname));
            DiCIELABFunction *disp = NULL;
            if (opt_ifname != NULL)
                disp = new DiCIELABFunction(opt_ifname);
            else
                disp = new DiCIELABFunction(opt_minLum, opt_maxLum, opt_ddlCount);
            if ((disp != NULL) && disp->isValid())
            {
                if (opt_ambLight > 0)
                {
                    if (opt_verboseMode > 1)
                        OUTPUT << "setting ambient light value ..." << endl;
                    disp->setAmbientLightValue(opt_ambLight);
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
 * Revision 1.2  1999-10-18 15:04:16  joergr
 * Enhanced command line tool dcmdspfn (add new options).
 *
 * Revision 1.1  1999/10/08 15:40:08  joergr
 * Merged 'dcmgsdf' and 'dccielab' into one application.
 *
 *
 *
 */
