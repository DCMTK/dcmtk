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
 *  Update Date:      $Date: 1998-11-27 14:50:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmp2pgm.cc,v $
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

#include "cmdlnarg.h"
#include "ofcmdln.h"

#include "dviface.h"
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

// ********************************************

int main(int argc, char *argv[])
{
    OFCommandLine cmd;
    OFString str;

    OFCmdUnsignedInt opt_debugMode      = 0;           /* default: no debug */
    int              opt_suppressOutput = 0;           /* default: create output */

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
** Revision 1.1  1998-11-27 14:50:19  meichel
** Initial Release.
**
**
**
*/
