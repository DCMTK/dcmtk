/*
 *
 *  Copyright (C) 1998-2002, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    sample application that reads a DICOM image and creates
 *    a matching presentation state.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-23 18:26:08 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "dvpstat.h"
#include "dvpshlp.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"       /* for dcmtk version name */

#ifdef WITH_ZLIB
#include "zlib.h"        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmpsmk"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    int opt_debugMode = 0;

    // Variables for input parameters
    const char* opt_ifname = NULL;
    OFBool opt_iDataset = OFFalse;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    // Variables for output parameters
    const char* opt_ofname = NULL;
    OFBool verbosemode = OFFalse;
    E_TransferSyntax opt_oxfer = EXS_Unknown;
    OFBool opt_oDataset = OFFalse;                 // currently not available as command line option
    E_GrpLenEncoding oglenc = EGL_recalcGL;        // currently not available as command line option
    E_EncodingType oenctype = EET_ExplicitLength;  // currently not available as command line option
    E_PaddingEncoding opadenc = EPD_noChange;      // currently not available as command line option
    Uint32 padlen = 0;                             // currently not available as command line option
    Uint32 subPadlen = 0;                          // currently not available as command line option

    // Variables for processing parameters
    DVPSoverlayActivation overlayActivation      = DVPSO_copyOverlays;
    DVPSVOIActivation     voiActivation          = DVPSV_preferVOILUT;
    OFBool                curveActivation        = OFTrue;
    OFBool                shutterActivation      = OFTrue;
    OFBool                presentationActivation = OFTrue;
    DVPSGraphicLayering   layering               = DVPSG_twoLayers;

    const char *          opt_aetitle            = NULL;
    const char *          opt_filesetID          = NULL;
    const char *          opt_filesetUID         = NULL;

    SetDebugLevel((0));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Create DICOM grayscale softcopy presentation state", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",  "DICOM image file(s) to be read", OFCmdParam::PM_MultiMandatory);
    cmd.addParam("dcmfile-out", "DICOM presentation state file to be created");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                     "-h",        "print this help text and exit");
      cmd.addOption("--version",                               "print version information and exit", OFTrue /* exclusive */);
      cmd.addOption("--verbose",                  "-v",        "verbose mode, print processing details");
      cmd.addOption("--debug",                    "-d",        "debug mode, print debug information");

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
       cmd.addOption("--read-file",               "+f",        "read file format or data set (default)");
       cmd.addOption("--read-dataset",            "-f",        "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax (only with --read-dataset):", LONGCOL, SHORTCOL);
       cmd.addOption("--read-xfer-auto",          "-t=",       "use TS recognition (default)");
       cmd.addOption("--read-xfer-little",        "-te",       "read with explicit VR little endian TS");
       cmd.addOption("--read-xfer-big",           "-tb",       "read with explicit VR big endian TS");
       cmd.addOption("--read-xfer-implicit",      "-ti",       "read with implicit VR little endian TS");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("VOI transform handling:");
       cmd.addOption("--voi-lut",                 "+Vl",       "use first VOI LUT if present (default)");
       cmd.addOption("--voi-window",              "+Vw",       "use first window center/width if present");
       cmd.addOption("--voi-ignore",              "-V",        "ignore VOI LUT and window center/width");
      cmd.addSubGroup("curve handling:");
       cmd.addOption("--curve-activate",          "+c",        "activate curve data if present (default)");
       cmd.addOption("--curve-ignore",            "-c",        "ignore curve data");
      cmd.addSubGroup("overlay handling:");
       cmd.addOption("--overlay-copy",            "+oc",       "copy overlays if not embedded,\nactivate otherwise (default)");
       cmd.addOption("--overlay-activate",        "+oa",       "activate overlays");
       cmd.addOption("--overlay-ignore",          "-o",        "ignore overlays");
      cmd.addSubGroup("shutter handling:");
       cmd.addOption("--shutter-activate",        "+s",        "use shutter if present in image (default)");
       cmd.addOption("--shutter-ignore",          "-s",        "ignore shutter");
      cmd.addSubGroup("presentation LUT shape handling:");
       cmd.addOption("--plut-activate",           "+p",        "use presentation LUT shape if present (default)");
       cmd.addOption("--plut-ignore",             "-p",        "ignore presentation LUT shape");
      cmd.addSubGroup("layering:");
       cmd.addOption("--layer-single",            "+l1",       "all curves and overlays are in one layer");
       cmd.addOption("--layer-double",            "+l2",       "one layer for curves, one for overlays (default)");
       cmd.addOption("--layer-separate",          "+ls",       "separate layers for each curve and overlay");
      cmd.addSubGroup("location of referenced image:");
       cmd.addOption("--location-none",           "-lx",       "image reference without location (default)");
       cmd.addOption("--location-network",        "-ln",    1, "[a]etitle: string",
                                                               "image located at application entity a");
       cmd.addOption("--location-media",          "-lm",    2, "[f]ilesetID, fileset[UID]: string",
                                                               "image located on storage medium");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output transfer syntax:");
       cmd.addOption("--write-xfer-same",         "+t=",       "write with same TS as image file (default)");
       cmd.addOption("--write-xfer-little",       "+te",       "write with explicit VR little endian TS");
       cmd.addOption("--write-xfer-big",          "+tb",       "write with explicit VR big endian TS");
       cmd.addOption("--write-xfer-implicit",     "+ti",       "write with implicit VR little endian TS");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      /* check exclusive options first */
      if (cmd.getParamCount() == 0)
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
            CERR << endl << "External libraries used:";
#ifdef WITH_ZLIB
            CERR << endl << "- ZLIB, Version " << zlibVersion() << endl;
#else
            CERR << " none" << endl;
#endif
            return 0;
         }
      }

      /* command line parameters and options */
      cmd.getParam(1, opt_ifname);
      cmd.getParam(cmd.getParamCount(), opt_ofname);

      if (cmd.findOption("--verbose")) verbosemode=OFTrue;
      if (cmd.findOption("--debug")) opt_debugMode=3;

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file")) opt_iDataset = OFFalse;
      if (cmd.findOption("--read-dataset")) opt_iDataset = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-xfer-auto"))
      {
        if (! opt_iDataset) app.printError("--read-xfer-auto only allowed with --read-dataset");
        opt_ixfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-xfer-little"))
      {
        if (! opt_iDataset) app.printError("--read-xfer-little only allowed with --read-dataset");
        opt_ixfer = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-big"))
      {
        if (! opt_iDataset) app.printError("--read-xfer-big only allowed with --read-dataset");
        opt_ixfer = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-implicit"))
      {
        if (! opt_iDataset) app.printError("--read-xfer-implicit only allowed with --read-dataset");
        opt_ixfer = EXS_LittleEndianImplicit;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--voi-lut")) voiActivation = DVPSV_preferVOILUT;
      if (cmd.findOption("--voi-window")) voiActivation = DVPSV_preferVOIWindow;
      if (cmd.findOption("--voi-ignore")) voiActivation = DVPSV_ignoreVOI;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--curve-activate")) curveActivation = OFTrue;
      if (cmd.findOption("--curve-ignore")) curveActivation = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--overlay-copy")) overlayActivation = DVPSO_copyOverlays;
      if (cmd.findOption("--overlay-activate")) overlayActivation = DVPSO_referenceOverlays;
      if (cmd.findOption("--overlay-ignore")) overlayActivation = DVPSO_ignoreOverlays;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--shutter-activate")) shutterActivation = OFTrue;
      if (cmd.findOption("--shutter-ignore")) shutterActivation = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--plut-activate")) presentationActivation = OFTrue;
      if (cmd.findOption("--plut-ignore")) presentationActivation = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--layer-single")) layering = DVPSG_oneLayer;
      if (cmd.findOption("--layer-double")) layering = DVPSG_twoLayers;
      if (cmd.findOption("--layer-separate")) layering = DVPSG_separateLayers;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--location-none")) { /* nothing */ }
      if (cmd.findOption("--location-network")) app.checkValue(cmd.getValue(opt_aetitle));
      if (cmd.findOption("--location-media"))
      {
          app.checkValue(cmd.getValue(opt_filesetID));
          app.checkValue(cmd.getValue(opt_filesetUID));
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-same")) opt_oxfer = EXS_Unknown;
      if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      cmd.endOptionBlock();

    }

    // additional checks
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        CERR << "invalid input filename: <empty string>" << endl;
        return 1;
    }

    if ((opt_ofname == NULL) || (strlen(opt_ofname) == 0))
    {
        CERR << "invalid output filename: <empty string>" << endl;
        return 1;
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    // open input file
    DcmFileFormat fileformat;

    if (verbosemode)
        COUT << "read and interpret DICOM file "
             << opt_ifname << endl;


    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_iDataset);
    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": reading file: " <<  opt_ifname << endl;
        return 1;
    }

    DcmDataset *dataset = fileformat.getDataset();

    /* create presentation state */
    DVPresentationState state;
    if (verbosemode)
    {
        COUT << "creating presentation state object" << endl;
    }

    error = state.createFromImage(*dataset, overlayActivation, voiActivation,
      curveActivation, shutterActivation, presentationActivation, layering, opt_aetitle, opt_filesetID, opt_filesetUID);
    if (error != EC_Normal)
    {
        CERR << "Error: "
             << error.text()
             << ": creating presentation state from image file: " << opt_ifname << endl;
        return 1;
    }

    /* add additional image references to pstate */
    if (cmd.getParamCount() > 2)
    {
        if (verbosemode)
            COUT << "adding additonal image reference(s)" << endl;
        const int count = cmd.getParamCount();
        for (int i = 2; i < count; i++)
        {
            const char *fn = NULL;
            if (cmd.getParam(i, fn) == OFCommandLine::PVS_Normal)
            {
                DcmFileFormat *ff = NULL;
                if (DVPSHelper::loadFileFormat(fn, ff) == EC_Normal)
                {
                    if (ff)
                    {
                        DcmDataset *dset = ff->getDataset();
                        if (dset)
                            state.addImageReference(*dset);
                    }
                }
                delete ff;
            }
        }
    }

    DcmFileFormat fileformat2;
    DcmDataset *dataset2 = fileformat2.getDataset();

    error = state.write(*dataset2, OFTrue);
    if (error != EC_Normal)
    {
        CERR << "Error: "
             << error.text()
             << ": re-encoding presentation state : " <<  opt_ifname << endl;
        return 1;
    }

    if (opt_oxfer == EXS_Unknown)
    {
        if (verbosemode)
            COUT << "set output transfersyntax to input transfer syntax\n";
        opt_oxfer = dataset->getOriginalXfer();
    }

    if (verbosemode)
        COUT << "Check if new output transfer syntax is possible\n";

    DcmXfer oxferSyn(opt_oxfer);

    dataset2->chooseRepresentation(opt_oxfer, NULL);

    if (dataset2->canWriteXfer(opt_oxfer))
    {
        if (verbosemode)
           COUT << "Output transfer syntax " << oxferSyn.getXferName()
                << " can be written\n";
    } else {
        CERR << "No conversion to transfer syntax " << oxferSyn.getXferName()
             << " possible!\n";
        return 1;
    }

    if (verbosemode)
        COUT << "write converted DICOM file\n";

    error = fileformat2.saveFile(opt_ofname, opt_oxfer, oenctype, oglenc, opadenc, padlen, subPadlen, opt_oDataset);
    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": writing file: " <<  opt_ofname << endl;
        return 1;
    }

    if (verbosemode)
        COUT << "conversion successful\n";

    return 0;
}


/*
** CVS/RCS Log:
** $Log: dcmpsmk.cc,v $
** Revision 1.15  2002-09-23 18:26:08  joergr
** Added new command line option "--version" which prints the name and version
** number of external libraries used (incl. preparation for future support of
** 'config.guess' host identifiers).
**
** Revision 1.14  2002/08/20 12:21:53  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.13  2001/09/26 15:36:03  meichel
** Adapted dcmpstat to class OFCondition
**
** Revision 1.12  2001/06/01 15:50:08  meichel
** Updated copyright header
**
** Revision 1.11  2000/11/13 15:50:39  meichel
** Added dcmpstat support methods for creating image references
**   in SR documents.
**
** Revision 1.10  2000/06/02 12:49:51  joergr
** Added support for multiple image references.
**
** Revision 1.9  2000/03/08 16:28:42  meichel
** Updated copyright header.
**
** Revision 1.8  2000/03/06 18:21:46  joergr
** Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
**
** Revision 1.7  2000/03/03 14:13:26  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.6  2000/02/23 15:12:54  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.5  2000/02/01 11:54:35  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.4  1999/07/27 15:41:33  meichel
** Adapted dcmpstat tools to supplement 33 letter ballot changes.
**
** Revision 1.3  1999/04/28 15:45:07  meichel
** Cleaned up module dcmpstat apps, adapted to new command line class
**   and added short documentation.
**
** Revision 1.2  1999/02/17 10:05:03  meichel
** Removed dcmdata debug level from sample apps
**
** Revision 1.1  1998/11/27 14:50:20  meichel
** Initial Release.
**
*/

