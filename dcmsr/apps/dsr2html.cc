/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Renders the contents of a dicom structured reporting file in
 *           HTML format
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:46:21 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmsr/apps/dsr2html.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoc.h"

#include "dcdebug.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#include <iostream.h>
#include <fstream.h>


#define OFFIS_CONSOLE_APPLICATION "dsr2html"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static E_Condition renderFile(ostream &out,
                              const char *ifname,
                              const OFBool isDataset,
                              const E_TransferSyntax xfer,
                              const size_t renderFlags,
                              const OFBool debugMode)
{
    E_Condition result = EC_Normal;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>" << endl;
        return EC_IllegalCall;
    }

    DcmFileStream myin(ifname, DCM_ReadMode);
    if (myin.GetError() != EC_Normal)
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": cannot open file: " << ifname << endl;
        return EC_IllegalCall;
    }

    DcmObject *dfile = NULL;
    if (isDataset)
        dfile = new DcmDataset();
    else
        dfile = new DcmFileFormat();

    if (dfile != NULL)
    {
        dfile->transferInit();
        dfile->read(myin, xfer, EGL_noChange);
        dfile->transferEnd();

        if (dfile->error() != EC_Normal)
        {
            CERR << OFFIS_CONSOLE_APPLICATION << ": error: " << dcmErrorConditionToString(dfile->error())
                 << ": reading file: "<< ifname << endl;
            result = EC_IllegalCall;
        }
    } else
        result = EC_MemoryExhausted;

    if (result == EC_Normal)
    {
        result = EC_IllegalCall;
        DcmDataset *dset = (isDataset) ? (DcmDataset*)dfile : ((DcmFileFormat *)dfile)->getDataset();
        if (dset != NULL)
        {
            DSRDocument *dsrdoc = new DSRDocument();
            if (dsrdoc != NULL)
            {
                if (debugMode)
                    dsrdoc->setLogStream(&ofConsole);
                result = dsrdoc->read(*dset);
                if (result == EC_Normal)
                    result = dsrdoc->renderHTML(out, renderFlags);
                else
                    CERR << OFFIS_CONSOLE_APPLICATION << ": error: parsing SR file: "<< ifname << endl;
            }
            delete dsrdoc;
        }
    }
    delete dfile;

    return result;
}


#define SHORTCOL 3
#define LONGCOL 21


int main(int argc, char *argv[])
{
    int opt_debugMode = 0;
    size_t opt_renderFlags = 0;
    OFBool isDataset = OFFalse;
    E_TransferSyntax xfer = EXS_Unknown;

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Render DICOM SR file and data set to HTML", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dsrfile-in",   "DICOM SR input filename to be rendered", OFCmdParam::PM_Mandatory);
    cmd.addParam("htmlfile-out", "HTML output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",        "print this help text and exit");
      cmd.addOption("--debug",                 "-d",        "debug mode, print debug information");

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",           "+f",        "read file format or data set (default)");
        cmd.addOption("--read-dataset",        "-f",        "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
        cmd.addOption("--read-xfer-auto",      "-t=",       "use TS recognition (default)");
        cmd.addOption("--read-xfer-little",    "-te",       "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",       "-tb",       "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",  "-ti",       "read with implicit VR little endian TS");

    cmd.addGroup("output options:");
      cmd.addSubGroup("rendering:");
        cmd.addOption("--expand-inline",       "+Ri",        "expand short content items inline (default)");
        cmd.addOption("--never-expand-inline", "-Ri",        "never expand content items inline");
        cmd.addOption("--render-all-codes",    "+Rc",        "render all codes (incl. concept name codes)");
        cmd.addOption("--render-full-data",    "+Rd",        "render full data of the content items");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
        if (cmd.findOption("--debug"))
            opt_debugMode = 5;

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-file"))
            isDataset = OFFalse;
        if (cmd.findOption("--read-dataset"))
            isDataset = OFTrue;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-xfer-auto"))
        {
            app.checkDependence("--read-xfer-auto", "--read-dataset", isDataset);
            xfer = EXS_Unknown;
        }
        if (cmd.findOption("--read-xfer-little"))
        {
            app.checkDependence("--read-xfer-little", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-big"))
        {
            app.checkDependence("--read-xfer-big", "--read-dataset", isDataset);
            xfer = EXS_BigEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-implicit"))
        {
            app.checkDependence("--read-xfer-implicit", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianImplicit;
        }
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--expand-inline"))
            /* default */;
        if (cmd.findOption("--never-expand-inline"))
            opt_renderFlags |= DSRTypes::HF_neverExpandChildsInline;
        cmd.endOptionBlock();

        if (cmd.findOption("--render-all-codes"))
            opt_renderFlags |= DSRTypes::HF_renderAllCodes;

        if (cmd.findOption("--render-full-data"))
            opt_renderFlags |= DSRTypes::HF_renderFullData;
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    int result = 0;
    const char *ifname = NULL;
    cmd.getParam(1, ifname);
    if (cmd.getParamCount() == 2)
    {
        const char *ofname = NULL;
        cmd.getParam(2, ofname);
        ofstream stream(ofname);
        if (stream.good())
        {
            if (renderFile(stream, ifname, isDataset, xfer, opt_renderFlags, opt_debugMode != 0) != EC_Normal)
                result = 2;
        } else
            result = 1;
    } else {
        if (renderFile(COUT, ifname, isDataset, xfer, opt_renderFlags, opt_debugMode != 0) != EC_Normal)
            result = 3;
    }

    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dsr2html.cc,v $
 * Revision 1.1  2000-10-13 07:46:21  joergr
 * Added new module 'dcmsr' providing access to DICOM structured reporting
 * documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
