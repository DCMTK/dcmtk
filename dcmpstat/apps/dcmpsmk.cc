/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    sample application that reads a DICOM image and creates
 *    a matching presentation state.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmpstat/dcmpstat.h"
#include "dcmtk/dcmpstat/dvpshlp.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmpsmk"

static OFLogger dcmpsmkLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{

#ifdef WITH_TCPWRAPPER
    // this code makes sure that the linker cannot optimize away
    // the DUL part of the network module where the external flags
    // for libwrap are defined. Needed on OpenBSD.
    dcmTCPWrapperDaemonName.set(NULL);
#endif

    // Variables for input parameters
    const char* opt_ifname = NULL;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    // Variables for output parameters
    const char* opt_ofname = NULL;
    E_TransferSyntax opt_oxfer = EXS_Unknown;
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

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Create DICOM grayscale softcopy presentation state", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",  "DICOM image file(s) to be read", OFCmdParam::PM_MultiMandatory);
    cmd.addParam("dcmfile-out", "DICOM presentation state file to be created");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                 "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
       cmd.addOption("--read-file",           "+f",     "read file format or data set (default)");
       cmd.addOption("--read-file-only",      "+fo",    "read file format only");
       cmd.addOption("--read-dataset",        "-f",     "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:", LONGCOL, SHORTCOL);
       cmd.addOption("--read-xfer-auto",      "-t=",    "use TS recognition (default)");
       cmd.addOption("--read-xfer-detect",    "-td",    "ignore TS specified in the file meta header");
       cmd.addOption("--read-xfer-little",    "-te",    "read with explicit VR little endian TS");
       cmd.addOption("--read-xfer-big",       "-tb",    "read with explicit VR big endian TS");
       cmd.addOption("--read-xfer-implicit",  "-ti",    "read with implicit VR little endian TS");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("VOI transform handling:");
       cmd.addOption("--voi-lut",             "+Vl",    "use first VOI LUT if present (default)");
       cmd.addOption("--voi-window",          "+Vw",    "use first window center/width if present");
       cmd.addOption("--voi-ignore",          "-V",     "ignore VOI LUT and window center/width");
      cmd.addSubGroup("curve handling:");
       cmd.addOption("--curve-activate",      "+c",     "activate curve data if present (default)");
       cmd.addOption("--curve-ignore",        "-c",     "ignore curve data");
      cmd.addSubGroup("overlay handling:");
       cmd.addOption("--overlay-copy",        "+oc",    "copy overlays if not embedded,\nactivate otherwise (default)");
       cmd.addOption("--overlay-activate",    "+oa",    "activate overlays");
       cmd.addOption("--overlay-ignore",      "-o",     "ignore overlays");
      cmd.addSubGroup("shutter handling:");
       cmd.addOption("--shutter-activate",    "+s",     "use shutter if present in image (default)");
       cmd.addOption("--shutter-ignore",      "-s",     "ignore shutter");
      cmd.addSubGroup("presentation LUT shape handling:");
       cmd.addOption("--plut-activate",       "+p",     "use presentation LUT shape if present (default)");
       cmd.addOption("--plut-ignore",         "-p",     "ignore presentation LUT shape");
      cmd.addSubGroup("layering:");
       cmd.addOption("--layer-single",        "+l1",    "all curves and overlays are in one layer");
       cmd.addOption("--layer-double",        "+l2",    "one layer for curves, one for overlays (default)");
       cmd.addOption("--layer-separate",      "+ls",    "separate layers for each curve and overlay");
      cmd.addSubGroup("location of referenced image:");
       cmd.addOption("--location-none",       "-lx",    "image reference without location (default)");
       cmd.addOption("--location-network",    "-ln", 1, "[a]etitle: string",
                                                        "image located at application entity a");
       cmd.addOption("--location-media",      "-lm", 2, "[f]ilesetID, fileset[UID]: string",
                                                        "image located on storage medium");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output transfer syntax:");
       cmd.addOption("--write-xfer-same",     "+t=",    "write with same TS as image file (default)");
       cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian TS");
       cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
       cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");

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
#ifdef WITH_ZLIB
            COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
            COUT << " none" << OFendl;
#endif
            return 0;
         }
      }

      /* command line parameters and options */
      cmd.getParam(1, opt_ifname);
      cmd.getParam(cmd.getParamCount(), opt_ofname);

      OFLog::configureFromCommandLine(cmd, app);

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file")) opt_readMode = ERM_autoDetect;
      if (cmd.findOption("--read-file-only")) opt_readMode = ERM_fileOnly;
      if (cmd.findOption("--read-dataset")) opt_readMode = ERM_dataset;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-xfer-auto"))
          opt_ixfer = EXS_Unknown;
      if (cmd.findOption("--read-xfer-detect"))
          dcmAutoDetectDatasetXfer.set(OFTrue);
      if (cmd.findOption("--read-xfer-little"))
      {
          app.checkDependence("--read-xfer-little", "--read-dataset", opt_readMode == ERM_dataset);
          opt_ixfer = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-big"))
      {
          app.checkDependence("--read-xfer-big", "--read-dataset", opt_readMode == ERM_dataset);
          opt_ixfer = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-implicit"))
      {
          app.checkDependence("--read-xfer-implicit", "--read-dataset", opt_readMode == ERM_dataset);
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

    /* print resource identifier */
    OFLOG_DEBUG(dcmpsmkLogger, rcsid << OFendl);

    // additional checks
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcmpsmkLogger, "invalid input filename: <empty string>");
        return 1;
    }

    if ((opt_ofname == NULL) || (strlen(opt_ofname) == 0))
    {
        OFLOG_FATAL(dcmpsmkLogger, "invalid output filename: <empty string>");
        return 1;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmpsmkLogger, "no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open input file
    DcmFileFormat fileformat;

    OFLOG_INFO(dcmpsmkLogger, "read and interpret DICOM file " << opt_ifname);

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
    if (error.bad())
    {
        OFLOG_FATAL(dcmpsmkLogger, error.text() << ": reading file: " << opt_ifname);
        return 1;
    }

    DcmDataset *dataset = fileformat.getDataset();

    /* create presentation state */
    DcmPresentationState state;
    OFLOG_INFO(dcmpsmkLogger, "creating presentation state object");

    error = state.createFromImage(*dataset, overlayActivation, voiActivation,
      curveActivation, shutterActivation, presentationActivation, layering, opt_aetitle, opt_filesetID, opt_filesetUID);
    if (error != EC_Normal)
    {
        OFLOG_FATAL(dcmpsmkLogger, error.text() << ": creating presentation state from image file: " << opt_ifname);
        return 1;
    }

    /* add additional image references to pstate */
    if (cmd.getParamCount() > 2)
    {
        OFLOG_INFO(dcmpsmkLogger, "adding additonal image reference(s)");
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
        OFLOG_FATAL(dcmpsmkLogger, error.text() << ": re-encoding presentation state : " << opt_ifname);
        return 1;
    }

    if (opt_oxfer == EXS_Unknown)
    {
        OFLOG_INFO(dcmpsmkLogger, "set output transfersyntax to input transfer syntax");
        opt_oxfer = dataset->getOriginalXfer();
    }

    OFLOG_INFO(dcmpsmkLogger, "Check if new output transfer syntax is possible");

    DcmXfer oxferSyn(opt_oxfer);

    dataset2->chooseRepresentation(opt_oxfer, NULL);

    if (dataset2->canWriteXfer(opt_oxfer))
    {
        OFLOG_INFO(dcmpsmkLogger, "Output transfer syntax " << oxferSyn.getXferName()
                << " can be written");
    } else {
        OFLOG_FATAL(dcmpsmkLogger, "No conversion to transfer syntax " << oxferSyn.getXferName()
             << " possible!");
        return 1;
    }

    OFLOG_INFO(dcmpsmkLogger, "write converted DICOM file");

    error = fileformat2.saveFile(opt_ofname, opt_oxfer, oenctype, oglenc, opadenc, padlen, subPadlen);
    if (error.bad())
    {
        OFLOG_FATAL(dcmpsmkLogger, error.text() << ": writing file: " <<  opt_ofname);
        return 1;
    }

    OFLOG_INFO(dcmpsmkLogger, "conversion successful");

    return 0;
}
