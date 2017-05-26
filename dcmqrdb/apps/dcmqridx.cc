/*
 *
 *  Copyright (C) 1993-2017, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: This test program registers image files in the image database.
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmnet/dcompat.h"
#include "dcmtk/dcmqrdb/dcmqrdbs.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmqridx"

static OFLogger dcmqridxLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


#define SHORTCOL 3
#define LONGCOL  12


int main (int argc, char *argv[])
{
    char sclass [120] ;
    char sinst  [120] ;
#ifdef DEBUG
    char fname  [120] ;
#endif
    DcmQueryRetrieveDatabaseStatus status;

    const char *opt_storageArea = NULL;
    OFBool opt_print = OFFalse;
    OFBool opt_isNewFlag = OFTrue;

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
     cmd.addOption("--help",    "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",       "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);
     cmd.addOption("--print",   "-p", "list contents of database index file");
     cmd.addOption("--not-new", "-n", "set instance reviewed status to 'not new'");

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

        OFLog::configureFromCommandLine(cmd, app);

        if (cmd.findOption("--print"))
            opt_print = OFTrue;

        if (cmd.findOption("--not-new"))
            opt_isNewFlag = OFFalse;
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmqridxLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmqridxLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    OFCondition cond;
    DcmQueryRetrieveIndexDatabaseHandle hdl(opt_storageArea, DB_UpperMaxStudies, DB_UpperMaxBytesPerStudy, cond);
    if (cond.good())
    {
        hdl.enableQuotaSystem(OFFalse); /* disable deletion of images */
        int paramCount = cmd.getParamCount();
        for (int param = 2; param <= paramCount; param++)
        {
            const char *opt_imageFile = NULL;
            cmd.getParam(param, opt_imageFile);
            if (access(opt_imageFile, R_OK) < 0)
                OFLOG_ERROR(dcmqridxLogger, "cannot access: " << opt_imageFile);
            else
            {
                OFLOG_INFO(dcmqridxLogger, "registering: " << opt_imageFile);
                if (DU_findSOPClassAndInstanceInFile(opt_imageFile, sclass, sinst))
                {
#ifdef DEBUG
                    /*** Test what filename is recommended by DB_Module **/
                    hdl.makeNewStoreFileName (sclass, sinst, fname) ;
                    OFLOG_DEBUG(dcmqridxLogger, "DB_Module recommends " << fname << " for filename");
#endif
                    hdl.storeRequest(sclass, sinst, opt_imageFile, &status, opt_isNewFlag) ;
                } else
                    OFLOG_ERROR(dcmqridxLogger, "cannot load dicom file: " << opt_imageFile);
            }
        }
        if (opt_print)
        {
            COUT << "-- DB Index File --" << OFendl;
            hdl.printIndexFile(OFconst_cast(char *, opt_storageArea));
        }
        return 0;
    }

    return 1;
}
