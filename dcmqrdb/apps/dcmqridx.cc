/*
 *
 *  Copyright (C) 1993-2009, OFFIS
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
 *  Module:  dcmqrdb
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: This test program registers image files in the image database.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-09-04 14:38:02 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmqrdb/dcmqrdbs.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdebug.h"
#include "dcmtk/dcmnet/dcompat.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmnet/dul.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmqridx"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


#define SHORTCOL 2
#define LONGCOL  11


int main (int argc, char *argv[])
{
    char sclass [120] ;
    char sinst  [120] ;
#ifdef DEBUG
    char fname  [120] ;
#endif
    DcmQueryRetrieveDatabaseStatus status;

    const char *opt_storageArea = NULL;
    OFBool opt_debug = OFFalse;
    OFBool opt_verbose = OFFalse;
    OFBool opt_print = OFFalse;
    OFBool opt_isNewFlag = OFTrue;

    SetDebugLevel(( 0 ));

#ifdef WITH_TCPWRAPPER
    // this code makes sure that the linker cannot optimize away
    // the DUL part of the network module where the external flags
    // for libwrap are defined. Needed on OpenBSD.
    dcmTCPWrapperDaemonName.set(NULL);
#endif

    OFCommandLine cmd;
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Register a DICOM image file in an image database index file", rcsid);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("index-out",  "storage area for the index file (directory)");
    cmd.addParam("dcmfile-in", "DICOM image file to be registered in the index file", OFCmdParam::PM_MultiOptional);

    cmd.addGroup("options:", LONGCOL, SHORTCOL);
     cmd.addOption("--help",      "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",         "print version information and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--arguments",       "print expanded command line arguments");
     cmd.addOption("--verbose",   "-v", "verbose mode, print processing details");
     cmd.addOption("--debug",     "-d", "debug mode, print debug information");
     cmd.addOption("--print",     "-p", "list contents of database index file");
     cmd.addOption("--not-new",   "-n", "set instance reviewed status to 'not new'");

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
        /* check whether to print the command line arguments */
        if (cmd.findOption("--arguments"))
            app.printArguments();

        /* check exclusive options first */
        if (cmd.hasExclusiveOption())
        {
            if (cmd.findOption("--version"))
            {
                app.printHeader(OFTrue /*print host identifier*/);
                COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_TCPWRAPPER)
                COUT << " none" << OFendl;
#else
                COUT << OFendl;
#endif
#ifdef WITH_ZLIB
                COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_TCPWRAPPER
                COUT << "- LIBWRAP" << OFendl;
#endif
                return 0;
            }
        }

        /* command line parameters and options */
        cmd.getParam(1, opt_storageArea);

        if (cmd.findOption("--verbose"))
            opt_verbose = OFTrue;
        if (cmd.findOption("--debug"))
        {
            opt_debug = OFTrue;
            SetDebugLevel(3);
            app.printIdentifier();
        }
        if (cmd.findOption("--print"))
            opt_print = OFTrue;

        if (cmd.findOption("--not-new"))
            opt_isNewFlag = OFFalse;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
        fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n", DCM_DICT_ENVIRONMENT_VARIABLE);

    OFCondition cond;
    DcmQueryRetrieveIndexDatabaseHandle hdl(opt_storageArea, DB_UpperMaxStudies, DB_UpperMaxBytesPerStudy, cond);
    if (cond.good())
    {
        hdl.setDebugLevel(opt_debug ? 3 : 0);
        hdl.enableQuotaSystem(OFFalse); /* disable deletion of images */
        int paramCount = cmd.getParamCount();
        for (int param = 2; param <= paramCount; param++)
        {
            const char *opt_imageFile = NULL;
            cmd.getParam(param, opt_imageFile);
            if (access(opt_imageFile, R_OK) < 0)
                fprintf(stderr, "cannot access: %s\n", opt_imageFile);
            else
            {
                if (opt_verbose)
                    printf("registering: %s\n", opt_imageFile);
                if (DU_findSOPClassAndInstanceInFile(opt_imageFile, sclass, sinst))
                {
#ifdef DEBUG
                    if (hdl.getDebugLevel() > 0)
                    {
                        /*** Test what filename is recommended by DB_Module **/
                        hdl.makeNewStoreFileName (sclass, sinst, fname) ;
                        printf("DB_Module recommends %s for filename\n", fname) ;
                    }
#endif
                    hdl.storeRequest(sclass, sinst, opt_imageFile, &status, opt_isNewFlag) ;
                } else
                    fprintf(stderr, "%s: cannot load dicom file: %s\n", OFFIS_CONSOLE_APPLICATION, opt_imageFile);
            }
        }
        if (opt_print)
        {
            printf("-- DB Index File --\n");
            hdl.printIndexFile((char *)opt_storageArea);
        }
        return 0;
    }

    return 1;
}


/*
 * CVS Log
 * $Log: dcmqridx.cc,v $
 * Revision 1.11  2009-09-04 14:38:02  joergr
 * Output all --version information to COUT (and not to CERR).
 *
 * Revision 1.10  2009-08-21 09:49:41  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.9  2008-09-25 15:34:37  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.8  2006/10/27 09:16:59  joergr
 * Fixed wrong name of the command line tool "dcmqridx".
 *
 * Revision 1.7  2006/08/15 16:09:33  meichel
 * Updated the code in module dcmqrdb to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.6  2006/07/27 14:45:18  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 * Made naming conventions for command line parameters more consistent, e.g.
 * used "dcmfile-in", "dcmfile-out" and "bitmap-out".
 *
 * Revision 1.5  2006/01/17 18:36:35  joergr
 * Fixed syntax errors in debug code.
 *
 * Revision 1.4  2005/12/14 17:43:42  meichel
 * Adapted code for compilation with TCP wrappers to NetBSD
 *
 * Revision 1.3  2005/12/14 13:01:01  meichel
 * Added code needed for compilation with TCP wrappers on OpenBSD
 *
 * Revision 1.2  2005/12/08 15:47:01  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2005/03/30 13:34:44  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
