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
 *  Purpose: Presentation State Viewer - Network Send Component (Store SCU)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-01-20 19:26:17 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/Attic/runsend.cc,v $
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

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>    /* for O_RDONLY */
#endif
END_EXTERN_C

#include "cmdlnarg.h"
#include "ofcmdln.h"
#include "dviface.h"
#include "ofbmanip.h" /* for OFBitmanipTemplate */
#include "dcuid.h"    /* for dcmtk version name */
#include "diutil.h"

static char rcsid[] = "$dcmtk: runsend v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default Max PDU size to be used when no different value is defined in the configuration file */
#define DEFAULT_MAXPDU 16384

static void
printHeader()
{
    cerr << "runsend: parallel network send for presentation state viewer" << endl;
}


static void
printUsage(const OFCommandLine &cmd)
{
    OFString str;
    cmd.getOptionString(str);
    printHeader();
    cerr << "usage: runsend [options] config-file target study [series] [instance]" << endl;
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


// ********************************************

int main(int argc, char *argv[])
{
                
    OFCommandLine cmd;
    OFString str;

    const char *opt_cfgName     = NULL;                /* config file name */
    const char *opt_target      = NULL;                /* send target name */
    const char *opt_studyUID    = NULL;                /* study instance UID */
    const char *opt_seriesUID   = NULL;                /* series instance UID */
    const char *opt_instanceUID = NULL;                /* instance instance UID */

    SetDebugLevel(( 0 ));
  
    prepareCmdLineArgs(argc, argv, "runsend");
      
    cmd.addGroup("options:");
     cmd.addOption("--help",        "-h",    "print this help screen");
     
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
                printError("Missing configuration file name");
            else if (cmd.getParamCount() == 1)
                printError("Missing send target");
            else if (cmd.getParamCount() == 2)
                printError("Missing study instance UID");
            else if (cmd.getParamCount() > 5)
                printError("Too many arguments");
            else 
            {
                cmd.getParam(1, opt_cfgName);
                cmd.getParam(2, opt_target);
                cmd.getParam(3, opt_studyUID);
                if (cmd.getParamCount() >= 4) cmd.getParam(4, opt_seriesUID);
                if (cmd.getParamCount() >= 5) cmd.getParam(5, opt_instanceUID);
            }
    }

    
    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        cerr << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
        return 10;
      }
    } else {
        cerr << "error: missing configuration file name" << endl;
        return 10;
    }
    
    DVInterface dvi(0, opt_cfgName);

    E_Condition result = dvi.sendIOD(opt_target, opt_studyUID, opt_seriesUID, opt_instanceUID);

    if (result != EC_Normal) cerr << "error: unable to create send process" << endl;
    return 0;    
}


/*
 * CVS/RCS Log:
 * $Log: runsend.cc,v $
 * Revision 1.1  1999-01-20 19:26:17  meichel
 * Implemented DICOM network send application "dcmpssnd" which sends studies/
 *   series/images contained in the local database to a remote DICOM
 *   communication peer.
 *
 *
 */
