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
 *  Update Date:      $Date: 1999-10-08 15:40:08 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/apps/dcmdspfn.cc,v $
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
#define LONGCOL  9

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

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addParam("lutfile-in",               "text file describing the monitor characteristic");

    cmd.addGroup("general options:");
     cmd.addOption("--help",       "-h",     "print this help text and exit");
     cmd.addOption("--verbose",    "-v",     "verbose mode, print processing details");
     cmd.addOption("--quiet",      "-q",     "quiet mode, print no warnings and errors");
     cmd.addOption("--debug",      "-d",     "debug mode, print debug information");

    cmd.addGroup("output options:");
     cmd.addOption("--gsdf",       "-Og", 1, "[f]ilename : string",
                                             "write GSDF curve data to file f");
     cmd.addOption("--cielab",     "-Oc", 1, "[f]ilename : string",
                                             "write CIELAB curve data to file f");

    if (app.parseCommandLine(cmd, argc, argv))
    {
        cmd.getParam(1, opt_ifname);

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
        if (opt_verboseMode > 1)
            OUTPUT << "reading LUT file: " << opt_ifname << endl;

        if (cmd.findOption("--gsdf"))
        {
            app.checkValue(cmd.getValue(opt_ofname));
            DiGSDFunction disp(opt_ifname);
            if (disp.isValid())
            {
                if (opt_verboseMode > 1)
                    OUTPUT << "writing GSDF file: " << opt_ofname << endl;
                if (!disp.writeCurveData(opt_ofname))
                    app.printError("can't write output file");
            } else
                app.printError("invalid input file");
        }

        if (cmd.findOption("--cielab"))
        {
            app.checkValue(cmd.getValue(opt_ofname));
            DiCIELABFunction disp(opt_ifname);
            if (disp.isValid())
            {
                if (opt_verboseMode > 1)
                    OUTPUT << "writing CIELAB file: " << opt_ofname << endl;
                if (!disp.writeCurveData(opt_ofname))
                    app.printError("can't write output file");
            } else
                app.printError("invalid input file");    
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
 * Revision 1.1  1999-10-08 15:40:08  joergr
 * Merged 'dcmgsdf' and 'dccielab' into one application.
 *
 *
 *
 */
