/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Module:  dcmimage
 *
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: Scale DICOM images
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-23 18:01:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/apps/dcmscale.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
END_EXTERN_C

#ifdef HAVE_GUSI_H
# include <GUSI.h>
#endif

#include "dctk.h"          /* for various dcmdata headers */
#include "dcdebug.h"       /* for SetDebugLevel */
#include "cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcuid.h"         /* for dcmtk version name */

#include "ofconapp.h"      /* for OFConsoleApplication */
#include "ofcmdln.h"       /* for OFCommandLine */

#include "dcmimage.h"      /* for DicomImage */
#include "diregist.h"      /* include to support color images */
#include "dcrledrg.h"      /* for DcmRLEDecoderRegistration */

#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
#include "djdecode.h"      /* for dcmjpeg decoders */
#include "dipijpeg.h"      /* for dcmimage JPEG plugin */
#endif

#ifdef WITH_ZLIB
#include "zlib.h"          /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_DESCRIPTION "Scale DICOM images"

#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
#define OFFIS_CONSOLE_APPLICATION "dcmjscal"
#else
#define OFFIS_CONSOLE_APPLICATION "dcmscale"
#endif

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 4
#define LONGCOL 21


// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;

    OFBool opt_debug = OFFalse;
    OFBool opt_verbose = OFFalse;
    OFBool opt_iDataset = OFFalse;
    OFBool opt_oDataset = OFFalse;
    OFBool opt_uidcreation = OFTrue;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    E_TransferSyntax opt_oxfer = EXS_Unknown;
    E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
    E_EncodingType opt_oenctype = EET_ExplicitLength;
    E_PaddingEncoding opt_opadenc = EPD_noChange;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;

#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
    // JPEG parameters, currently not used
# if 0
    OFCmdUnsignedInt opt_quality = 90;                 /* default: 90% JPEG quality */
    E_SubSampling opt_sampling = ESS_422;              /* default: 4:2:2 sub-sampling */
# endif
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
#endif

    int opt_useAspectRatio = 1;                        /* default: use aspect ratio for scaling */
    OFCmdUnsignedInt opt_useInterpolation = 1;         /* default: use interpolation method '1' for scaling */
    int opt_scaleType = 0;                             /* default: no scaling */
                                                       /* 1 = X-factor, 2 = Y-factor, 3=X-size, 4=Y-size */
    OFCmdFloat opt_scale_factor = 1.0;
    OFCmdUnsignedInt opt_scale_size = 1;

    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;

    SetDebugLevel((0));
    DicomImageClass::setDebugLevel(DicomImageClass::DL_Warnings | DicomImageClass::DL_Errors);

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addParam("dcmfile-in",  "DICOM input filename to be scaled");
    cmd.addParam("dcmfile-out", "DICOM output filename to be written");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                "-h",         "print this help text and exit" /*, OFTrue is set implicitly */);
     cmd.addOption("--version",                           "print version information and exit", OFTrue /* exclusive */);
     cmd.addOption("--debug",               "-d",         "debug mode, print debug information");
     cmd.addOption("--verbose",             "-v",         "verbose mode, print processing details");

    cmd.addGroup("input options:");

     cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",          "+f",         "read file format or data set (default)");
      cmd.addOption("--read-dataset",       "-f",         "read data set without file meta information");

     cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
      cmd.addOption("--read-xfer-auto",     "-t=",        "use TS recognition (default)");
      cmd.addOption("--read-xfer-little",   "-te",        "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",      "-tb",        "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit", "-ti",        "read with implicit VR little endian TS");

    cmd.addGroup("image processing and encoding options:");
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
     cmd.addSubGroup("color space conversion options (compressed images only):");
      cmd.addOption("--conv-photometric",   "+cp",        "convert if YCbCr photom. interpr. (default)");
      cmd.addOption("--conv-lossy",         "+cl",        "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-always",        "+ca",        "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",         "+cn",        "never convert color space");
#endif
     cmd.addSubGroup("scaling:");
      cmd.addOption("--recognize-aspect",    "+a",        "recognize pixel aspect ratio (default)");
      cmd.addOption("--ignore-aspect",       "-a",        "ignore pixel aspect ratio when scaling");
      cmd.addOption("--interpolate",         "+i",     1, "[n]umber of algorithm : integer",
                                                          "use interpolation when scaling (1..2, def: 1)");
      cmd.addOption("--no-interpolation",    "-i",        "no interpolation when scaling");
      cmd.addOption("--no-scaling",          "-S",        "no scaling, ignore pixel aspect ratio (default)");
      cmd.addOption("--scale-x-factor",      "+Sxf",   1, "[f]actor : float",
                                                          "scale x axis by factor, auto-compute y axis");
      cmd.addOption("--scale-y-factor",      "+Syf",   1, "[f]actor : float",
                                                          "scale y axis by factor, auto-compute x axis");
      cmd.addOption("--scale-x-size",        "+Sxv",   1, "[n]umber : integer",
                                                          "scale x axis to n pixels, auto-compute y axis");
      cmd.addOption("--scale-y-size",        "+Syv",   1, "[n]umber : integer",
                                                          "scale y axis to n pixels, auto-compute x axis");
     cmd.addSubGroup("SOP Instance UID options:");
      cmd.addOption("--uid-always",          "+ua",       "always assign new SOP Instance UID (default)");
      cmd.addOption("--uid-never",           "+un",       "never assign new SOP Instance UID");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",        "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",        "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-same",     "+t=",       "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",   "+te",       "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",      "+tb",       "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit", "+ti",       "write with implicit VR little endian TS");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",       "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",      "-u",        "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc", "+g=",       "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create", "+g",        "always write with group length elements");
      cmd.addOption("--group-length-remove", "-g",        "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",     "+e",        "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",    "-e",        "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",      "-p=",       "do not change padding\n(default if not --write-dataset)");
      cmd.addOption("--padding-off",         "-p",        "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",      "+p",    2,  "[f]ile-pad [i]tem-pad: integer",
                                                          "align file on multiple of f bytes\nand items on multiple of i bytes");

    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */

      if (cmd.getParamCount() == 0)
      {
          if (cmd.findOption("--version"))
          {
              app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
              CERR << endl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(BUILD_DCMSCALE_AS_DCMJSCAL)
              CERR << " none" << endl;
#else
              CERR << endl;
#endif
#ifdef WITH_ZLIB
              CERR << "- ZLIB, Version " << zlibVersion() << endl;
#endif
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
              CERR << "- " << DiJPEGPlugin::getLibraryVersionString() << endl;
#endif
              return 0;
          }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      /* general options */

      if (cmd.findOption("--debug"))
          opt_debug = OFTrue;
      if (cmd.findOption("--verbose"))
          opt_verbose = OFTrue;

      /* input options */

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file")) opt_iDataset = OFFalse;
      if (cmd.findOption("--read-dataset")) opt_iDataset = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-xfer-auto"))
      {
          app.checkDependence("--read-xfer-auto", "--read-dataset", opt_iDataset);
          opt_ixfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-xfer-little"))
      {
          app.checkDependence("--read-xfer-little", "--read-dataset", opt_iDataset);
          opt_ixfer = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-big"))
      {
          app.checkDependence("--read-xfer-big", "--read-dataset", opt_iDataset);
          opt_ixfer = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-implicit"))
      {
          app.checkDependence("--read-xfer-implicit", "--read-dataset", opt_iDataset);
          opt_ixfer = EXS_LittleEndianImplicit;
      }
      cmd.endOptionBlock();

      /* image processing options: scaling */

      cmd.beginOptionBlock();
      if (cmd.findOption("--recognize-aspect"))
          opt_useAspectRatio = 1;
      if (cmd.findOption("--ignore-aspect"))
          opt_useAspectRatio = 0;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--interpolate"))
          app.checkValue(cmd.getValueAndCheckMinMax(opt_useInterpolation, 1, 2));
      if (cmd.findOption("--no-interpolation"))
          opt_useInterpolation = 0;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--no-scaling"))
          opt_scaleType = 0;
      if (cmd.findOption("--scale-x-factor"))
      {
          opt_scaleType = 1;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_factor, 0.0, OFFalse));
      }
      if (cmd.findOption("--scale-y-factor"))
      {
          opt_scaleType = 2;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_factor, 0.0, OFFalse));
      }
      if (cmd.findOption("--scale-x-size"))
      {
          opt_scaleType = 3;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_size, 1));
      }
      if (cmd.findOption("--scale-y-size"))
      {
          opt_scaleType = 4;
          app.checkValue(cmd.getValueAndCheckMin(opt_scale_size, 1));
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-always")) opt_uidcreation = OFTrue;
      if (cmd.findOption("--uid-never")) opt_uidcreation = OFFalse;
      cmd.endOptionBlock();

      /* output options */

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_oDataset = OFFalse;
      if (cmd.findOption("--write-dataset")) opt_oDataset = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-same")) opt_oxfer = EXS_Unknown;
      if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
          dcmEnableUnknownVRGeneration.set(OFTrue);
          dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
          dcmEnableUnknownVRGeneration.set(OFFalse);
          dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-recalc")) opt_oglenc = EGL_recalcGL;
      if (cmd.findOption("--group-length-create")) opt_oglenc = EGL_withGL;
      if (cmd.findOption("--group-length-remove")) opt_oglenc = EGL_withoutGL;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit")) opt_oenctype = EET_ExplicitLength;
      if (cmd.findOption("--length-undefined")) opt_oenctype = EET_UndefinedLength;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--padding-retain"))
      {
          app.checkConflict("--padding-retain", "--write-dataset", opt_oDataset);
          opt_opadenc = EPD_noChange;
      }
      if (cmd.findOption("--padding-off")) opt_opadenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
          app.checkConflict("--padding-create", "--write-dataset", opt_oDataset);
          app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
          app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
          opt_opadenc = EPD_withPadding;
      }
      cmd.endOptionBlock();
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    // register RLE decompression codec
    DcmRLEDecoderRegistration::registerCodecs(OFFalse /*pCreateSOPInstanceUID*/, opt_debug);
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
    // register global decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion, EUC_default, EPC_default, opt_debug);
#endif

    if (opt_debug)
        DicomImageClass::setDebugLevel(DicomImageClass::getDebugLevel() | DicomImageClass::DL_DebugMessages);
    if (opt_verbose)
        DicomImageClass::setDebugLevel(DicomImageClass::getDebugLevel() | DicomImageClass::DL_Informationals);

    // ======================================================================
    // read input file

    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        CERR << "Error: invalid filename: <empty string>" << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "open input file " << opt_ifname << endl;

    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_iDataset);
    if (error.bad())
    {
        CERR << "Error: " << error.text()
             << ": reading file: " <<  opt_ifname << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "load all data into memory" << endl;

    /* make sure that pixel data is loaded before output file is created */
    dataset->loadAllDataIntoMemory();

    // select uncompressed output transfer syntax.
    // this will implicitly decompress the image if necessary.

    if (opt_oxfer == EXS_Unknown)
    {
        if (opt_verbose)
            COUT << "set output transfer syntax to input transfer syntax" << endl;
        opt_oxfer = dataset->getOriginalXfer();
    }

    if (opt_verbose)
        COUT << "check if new output transfer syntax is possible" << endl;

    DcmXfer opt_oxferSyn(opt_oxfer);
    dataset->chooseRepresentation(opt_oxfer, NULL);

    if (dataset->canWriteXfer(opt_oxfer))
    {
        if (opt_verbose)
            COUT << "output transfer syntax " << opt_oxferSyn.getXferName()
                 << " can be written" << endl;
    } else {
        CERR << "Error: no conversion to transfer syntax " << opt_oxferSyn.getXferName()
             << " possible!" << endl;
        return 1;
    }

    // ======================================================================
    // image processing starts here

    if (opt_verbose)
        CERR << "preparing pixel data." << endl;

    // create DicomImage object
    DicomImage *di = new DicomImage(dataset, opt_oxfer, CIF_MayDetachPixelData);
    if (!di)
        app.printError("memory exhausted");
    if (di->getStatus() != EIS_Normal)
        app.printError(DicomImage::getString(di->getStatus()));

    // create new SOP instance UID
    if (error.good() && opt_uidcreation)
    {
        char new_uid[100];
        error = dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(new_uid));
    }

    if (error.good())
    {
        // perform scaling
        if (opt_scaleType > 0)
        {
            DicomImage *newimage;
            switch (opt_scaleType)
            {
                case 1:
                    if (opt_verbose)
                        CERR << "scaling image, X factor=" << opt_scale_factor << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    newimage = di->createScaledImage(opt_scale_factor, 0.0, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                case 2:
                    if (opt_verbose)
                        CERR << "scaling image, Y factor=" << opt_scale_factor << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    newimage = di->createScaledImage(0.0, opt_scale_factor, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                case 3:
                    if (opt_verbose)
                        CERR << "scaling image, X size=" << opt_scale_size << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    newimage = di->createScaledImage(opt_scale_size, 0, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                case 4:
                    if (opt_verbose)
                        CERR << "scaling image, Y size=" << opt_scale_size << ", Interpolation=" << (int)opt_useInterpolation <<
                            ", Aspect Ratio=" << (opt_useAspectRatio ? "yes" : "no") << endl;
                    newimage = di->createScaledImage(0, opt_scale_size, (int)opt_useInterpolation, opt_useAspectRatio);
                    break;
                default:
                    if (opt_verbose)
                        CERR << "internal error: unknown scaling type" << endl;
                    newimage = NULL;
                    break;
            }
            if (newimage==NULL)
                app.printError("memory exhausted");
            else if (newimage->getStatus() != EIS_Normal)
                app.printError(DicomImage::getString(newimage->getStatus()));
            else
            {
                /* write scaled image to dataset (update attributes of Image Pixel Module) */
                newimage->writeImageToDataset(*dataset);
                delete newimage;
            }
        }
    } else {    /* error.bad() */
        CERR << "Error: " << error.text()
             << ": converting image: " <<  opt_ifname << endl;
        return 1;
    }

    /* cleanup original image */
    delete di;
/*
    OFString derivationDescription = "...";

    // update image type
    if (error.good()) error = DcmQuant::updateImageType(dataset);

    // update derivation description
    if (error.good()) error = DcmQuant::updateDerivationDescription(dataset, derivationDescription.c_str());

    // create new SOP instance UID and add original instance to the source image sequence
    if (error.good() && opt_uidcreation) error = DcmQuant::newInstance(dataset);
*/
    // ======================================================================
    // write back output file

    // force meta-header to refresh SOP Instance UID
    DcmItem *metaInfo = fileformat.getMetaInfo();
    if (metaInfo)
        delete metaInfo->remove(DCM_MediaStorageSOPInstanceUID);

    if (opt_verbose)
        COUT << "create output file " << opt_ofname << endl;

    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, EPD_withoutPadding, 0, 0, opt_oDataset);
    if (error != EC_Normal)
    {
        CERR << "Error: "
             << error.text()
             << ": writing file: " <<  opt_ofname << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "conversion successful" << endl;

    // deregister RLE decompression codec
    DcmRLEDecoderRegistration::cleanup();
#ifdef BUILD_DCMSCALE_AS_DCMJSCAL
    // deregister global decompression codecs
    DJDecoderRegistration::cleanup();
#endif

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmscale.cc,v $
 * Revision 1.4  2002-09-23 18:01:19  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.3  2002/08/21 09:54:07  meichel
 * Fixed argument lists for loadFile and saveFile
 *
 * Revision 1.2  2002/08/20 12:20:21  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.1  2002/08/02 15:14:16  joergr
 * Added new command line program which allows to scale DICOM images.
 *
 *
 *
 */
