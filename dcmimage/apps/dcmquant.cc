/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Convert DICOM color images palette color
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-01-25 13:32:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/apps/dcmquant.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
// #include "dcutils.h"       /* for getSingleValue */
#include "dcdebug.h"       /* for SetDebugLevel */
#include "cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcuid.h"         /* for dcmtk version name */
#include "dcmimage.h"      /* for DicomImage */

#include "ofconapp.h"      /* for OFConsoleApplication */
#include "ofcmdln.h"       /* for OFCommandLine */

#include "diregist.h"      /* include to support color images */
#include "diquant.h"       /* for DcmQuant */

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
#include "djdecode.h"     /* for dcmjpeg decoders */
#endif

#define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM color images to palette color"

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
#define OFFIS_CONSOLE_APPLICATION "dcmjquan"
#else
#define OFFIS_CONSOLE_APPLICATION "dcmquant"
#endif

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 4
#define LONGCOL 20


// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;

    OFBool opt_verbose = OFFalse;
    OFBool opt_iDataset = OFFalse;
    OFBool opt_oDataset = OFFalse;
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    E_TransferSyntax opt_oxfer = EXS_Unknown;
    E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
    E_EncodingType opt_oenctype = EET_ExplicitLength;
    E_PaddingEncoding opt_opadenc = EPD_noChange;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;

    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData;
                                                          /* default: pixel data may detached if no longer needed */
    OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_frameCount = 0;               /* default: all frames */

    OFBool              opt_palette_ow = OFFalse;
    OFBool              opt_entries_word = OFFalse;
    OFBool              opt_palette_fs = OFFalse;
    OFCmdUnsignedInt    opt_palette_col = 256;

    DcmLargestDimensionType opt_largeType = DcmLargestDimensionType_default;
    DcmRepresentativeColorType opt_repType = DcmRepresentativeColorType_default;

    OFBool              opt_secondarycapture = OFFalse;
    OFBool              opt_uidcreation = OFTrue;


#ifdef BUILD_WITH_DCMJPEG_SUPPORT
    // JPEG parameters
    OFCmdUnsignedInt    opt_quality = 90;                 /* default: 90% JPEG quality */
    E_SubSampling       opt_sampling = ESS_422;           /* default: 4:2:2 sub-sampling */
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
#endif

    const char *        opt_ifname = NULL;
    const char *        opt_ofname = NULL;

    SetDebugLevel((0));
    DicomImageClass::setDebugLevel(DicomImageClass::DL_Warnings | DicomImageClass::DL_Errors);

    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);

    cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
    cmd.addParam("dcmfile-out", "DICOM output filename to be written");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                "-h",      "print this help text and exit");
     cmd.addOption("--verbose",             "-v",      "verbose mode, print processing details");

    cmd.addGroup("input options:");

     cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",          "+f",      "read file format or data set (default)");
      cmd.addOption("--read-dataset",       "-f",      "read data set without file meta information");

     cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
      cmd.addOption("--read-xfer-auto",     "-t=",     "use TS recognition (default)");
      cmd.addOption("--read-xfer-little",   "-te",     "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",      "-tb",     "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit", "-ti",     "read with implicit VR little endian TS");

    cmd.addGroup("image processing and encoding options:");

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
     cmd.addSubGroup("color space conversion options (compressed images only):");
      cmd.addOption("--conv-photometric",   "+cp",     "convert if YCbCr photom. interpr. (default)");
      cmd.addOption("--conv-lossy",         "+cl",     "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-always",        "+ca",     "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",         "+cn",     "never convert color space");
#endif

     cmd.addSubGroup("compatibility options:");
      cmd.addOption("--accept-palettes",    "+Mp",     "accept incorrect palette attribute tags\n(0028,111x) and (0028,121x)");

     cmd.addSubGroup("median cut dimension selection options:");      
      cmd.addOption("--mc-dimension-rgb",   "+Dr",     "max dimension from RGB range (default)");
      cmd.addOption("--mc-dimension-lum",   "+Dl",     "max dimension from luminance");

     cmd.addSubGroup("median cut representative color selection options:");      
      cmd.addOption("--mc-color-avgbox",   "+Cb",     "average colors in box (default)");
      cmd.addOption("--mc-color-avgpixel", "+Cp",     "average pixels in box");
      cmd.addOption("--mc-color-center",   "+Cc",     "select center of box");
      
     cmd.addSubGroup("color palette creation options:");      
      cmd.addOption("--write-ow",           "+pw",     "write Palette LUT as OW instead of US");
      cmd.addOption("--lut-entries-word",   "+pe",     "write Palette LUT with 16-bit entries");
      cmd.addOption("--floyd-steinberg",    "+pf",     "use Floyd-Steinberg error diffusion");
      cmd.addOption("--colors",             "+pc",  1, "number of colors: 2..65536 (default 256)", 
                                                       "number of colors to quantize to");

    cmd.addSubGroup("SOP Class UID options:");
     cmd.addOption("--class-default",      "+cd",     "keep SOP Class UID (default)");
     cmd.addOption("--class-sc",           "+cs",     "convert to Secondary Capture Image\n(implies --uid-always)");

    cmd.addSubGroup("SOP Instance UID options:");
     cmd.addOption("--uid-always",         "+ua",     "always assign new UID (default)");
     cmd.addOption("--uid-never",          "+un",     "never assign new UID");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",             "+F",        "write file format (default)");
      cmd.addOption("--write-dataset",          "-F",        "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-same",        "+t=",       "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",      "+te",       "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",         "+tb",       "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",    "+ti",       "write with implicit VR little endian TS");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc",    "+g=",       "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create",    "+g",        "always write with group length elements");
      cmd.addOption("--group-length-remove",    "-g",        "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",        "+e",        "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",       "-e",        "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",         "-p=",       "do not change padding\n(default if not --write-dataset)");
      cmd.addOption("--padding-off",            "-p",        "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",         "+p",    2,  "[f]ile-pad [i]tem-pad: integer",
                                                             "align file on multiple of f bytes\nand items on multiple of i bytes");

    if (app.parseCommandLine(cmd, argc, argv))
    {
      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      if (cmd.findOption("--verbose")) opt_verbose = OFTrue;

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

      if (cmd.findOption("--accept-palettes"))
          opt_compatibilityMode |= CIF_WrongPaletteAttributeTags;

      cmd.beginOptionBlock();
      if (cmd.findOption("--frame"))
      {
          app.checkValue(cmd.getValueAndCheckMin(opt_frame, 1));
          opt_frameCount = 1;
      }
      if (cmd.findOption("--all-frames"))
      {
          opt_frame = 1;
          opt_frameCount = 0;
      }
      cmd.endOptionBlock();

      if (cmd.findOption("--write-ow")) opt_palette_ow = OFTrue;
      if (cmd.findOption("--lut-entries-word")) opt_entries_word = OFTrue;
      if (cmd.findOption("--floyd-steinberg")) opt_palette_fs = OFTrue;        
      if (cmd.findOption("--colors")) cmd.getValueAndCheckMinMax(opt_palette_col, 2, 65536);

      cmd.beginOptionBlock();
      if (cmd.findOption("--mc-dimension-rgb")) opt_largeType = DcmLargestDimensionType_default;
      if (cmd.findOption("--mc-dimension-lum")) opt_largeType = DcmLargestDimensionType_luminance;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--mc-color-avgbox")) opt_repType = DcmRepresentativeColorType_default;
      if (cmd.findOption("--mc-color-avgpixel")) opt_repType = DcmRepresentativeColorType_averagePixels;
      if (cmd.findOption("--mc-color-center")) opt_repType = DcmRepresentativeColorType_centerOfBox;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--class-default")) opt_secondarycapture = OFFalse;
      if (cmd.findOption("--class-sc")) opt_secondarycapture = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-always")) opt_uidcreation = OFTrue;
      if (cmd.findOption("--uid-never")) opt_uidcreation = OFFalse;
      cmd.endOptionBlock();

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
          if (opt_oDataset) app.printError("--padding-retain not allowed with --write-dataset");
          opt_opadenc = EPD_noChange;
      }
      if (cmd.findOption("--padding-off")) opt_opadenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
          if (opt_oDataset) app.printError("--padding-create not allowed with --write-dataset");
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

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
    // register global decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion, EUC_default, EPC_default, OFFalse);
#endif

    // ======================================================================
    // read input file

    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        CERR << "Error: invalid filename: <empty string>" << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "open input file " << opt_ifname << endl;

    DcmFileStream inf(opt_ifname, DCM_ReadMode);
    if ( inf.Fail() )
    {
        CERR << "Error: cannot open file: " << opt_ifname << endl;
        return 1;
    }

    DcmFileFormat *fileformat = NULL;
    DcmDataset *dataset = NULL;
    OFCondition error = EC_Normal;

    if (opt_iDataset)
    {
        dataset = new DcmDataset;
        if (!dataset)
        {
            CERR << "Error: memory exhausted" << endl;
            return 1;
        }
        if (opt_verbose)
            COUT << "read and interpret DICOM dataset " << opt_ifname << endl;
        dataset->transferInit();
        error = dataset -> read(inf, opt_ixfer, EGL_noChange);
        dataset->transferEnd();
    }
    else
    {
        fileformat = new DcmFileFormat;
        if (!fileformat)
        {
            CERR << "Error: memory exhausted" << endl;
            return 1;
        }
        if (opt_verbose)
            COUT << "read and interpret DICOM file with metaheader "
                 << opt_ifname << endl;
        fileformat->transferInit();
        error = fileformat -> read(inf, opt_ixfer, EGL_noChange);
        fileformat->transferEnd();
    }

    if (error.bad())
    {
        CERR << "Error: " << error.text()
             << ": reading file: " <<  opt_ifname << endl;
        return 1;
    }

    if (fileformat)
    {
        if (opt_oDataset && opt_verbose)
            COUT << "get dataset of DICOM file with metaheader" << endl;
        dataset = fileformat -> getDataset();
    }

    if (opt_verbose)
        COUT << "load all data into memory" << endl;

    /* make sure that pixel data is loaded before output file is created */
    dataset->loadAllDataIntoMemory();

    if (!fileformat && !opt_oDataset)
    {
        if (opt_verbose)
            COUT << "create new Metaheader for dataset" << endl;
        fileformat = new DcmFileFormat(dataset);
    }

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
    DicomImage di(dataset, opt_oxfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);
    if (di.getStatus() != EIS_Normal) app.printError(DicomImage::getString(di.getStatus()));

    if (di.isMonochrome())
    {
      app.printError("cannot convert monochrome image to palette color");
    }

    OFString derivationDescription;

    // create palette color image
    error = DcmQuant::createPaletteColorImage(
      di, *dataset, opt_palette_ow, opt_entries_word, opt_palette_fs, opt_palette_col,
      derivationDescription, opt_verbose, opt_largeType, opt_repType);

    // update image type
    if (error.good()) error = DcmQuant::updateImageType(dataset);
    
    // update derivation description
    if (error.good()) error = DcmQuant::updateDerivationDescription(dataset, derivationDescription.c_str());
    
    // create new SOP instance UID
    if (error.good() && (opt_secondarycapture || opt_uidcreation)) error = DcmQuant::newInstance(dataset);

    // convert to Secondary Capture if requested by user.  
    // This method creates a new SOP class UID, so it should be executed
    // after the call to newInstance() which creates a Source Image Sequence.
    if (error.good() && opt_secondarycapture) error = DcmQuant::convertToSecondaryCapture(dataset);

    if (error.bad())
    {
        CERR << "Error: " << error.text()
             << ": converting image: " <<  opt_ifname << endl;
        return 1;
    }

    // ======================================================================
    // write back output file

    if (opt_verbose)
        COUT << "create output file " << opt_ofname << endl;

    DcmFileStream outf( opt_ofname, DCM_WriteMode );
    if ( outf.Fail() )
    {
        CERR << "Error: cannot create file: " << opt_ofname << endl;
        return 1;
    }

    if (opt_oDataset)
    {
        if (opt_verbose)
            COUT << "write converted DICOM dataset" << endl;

        dataset->transferInit();
        error = dataset->write(outf, opt_oxfer, opt_oenctype, opt_oglenc, EPD_withoutPadding);
        dataset->transferEnd();
    } else {
        if (opt_verbose)
            COUT << "write converted DICOM file with metaheader" << endl;

        fileformat->transferInit();
        error = fileformat->write(outf, opt_oxfer, opt_oenctype, opt_oglenc,
                  opt_opadenc, (Uint32) opt_filepad, (Uint32) opt_itempad);
        fileformat->transferEnd();
    }

    if (error.bad())
    {
        CERR << "Error: " << error.text()
             << ": writing file: " <<  opt_ofname << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "conversion successful" << endl;

#ifdef BUILD_WITH_DCMJPEG_SUPPORT
    // deregister global decompression codecs
    DJDecoderRegistration::cleanup();
#endif

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmquant.cc,v $
 * Revision 1.1  2002-01-25 13:32:01  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
