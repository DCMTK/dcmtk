/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Compress DICOM file
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-11-26 08:44:41 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/apps/dcmcjpeg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
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
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"     /* for dcmtk version name */
#include "djdecode.h"  /* for dcmjpeg decoders */
#include "djencode.h"  /* for dcmjpeg encoders */
#include "djrplol.h"   /* for DJ_RPLossless */
#include "djrploss.h"  /* for DJ_RPLossy */
#include "dipijpeg.h"  /* for dcmimage JPEG plugin */
#include "diregist.h"  /* include to support color images */

#ifdef WITH_ZLIB
#include <zlib.h>      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmcjpeg"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************


#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  SetDebugLevel(( 0 ));

  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  int opt_debugMode = 0;
  OFBool opt_verbose = OFFalse;
  OFBool opt_iDataset = OFFalse;
  E_TransferSyntax opt_ixfer = EXS_Unknown;
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;

  // JPEG options
  E_TransferSyntax opt_oxfer = EXS_JPEGProcess14SV1TransferSyntax;
  OFCmdUnsignedInt opt_selection_value = 6;
  OFCmdUnsignedInt opt_point_transform = 0;
  OFCmdUnsignedInt opt_quality = 90;
  OFBool           opt_huffmanOptimize = OFTrue;
  OFCmdUnsignedInt opt_smoothing = 0;
  int              opt_compressedBits = 0; // 0=auto, 8/12/16=force
  E_CompressionColorSpaceConversion opt_compCSconversion = ECC_lossyYCbCr;
  E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
  E_SubSampling    opt_sampleFactors = ESS_444;
  OFBool           opt_useYBR422 = OFFalse;
  OFCmdUnsignedInt opt_fragmentSize = 0; // 0=unlimited
  OFBool           opt_createOffsetTable = OFTrue;
  int              opt_windowType = 0;  /* default: no windowing; 1=Wi, 2=Wl, 3=Wm, 4=Wh, 5=Ww, 6=Wn, 7=Wr */
  OFCmdUnsignedInt opt_windowParameter = 0;
  OFCmdFloat       opt_windowCenter=0.0, opt_windowWidth=0.0;
  E_UIDCreation    opt_uidcreation = EUC_default;
  OFBool           opt_secondarycapture = OFFalse;
  OFCmdUnsignedInt opt_roiLeft = 0, opt_roiTop = 0, opt_roiWidth = 0, opt_roiHeight = 0;
  OFBool           opt_usePixelValues = OFTrue;
  OFBool           opt_useModalityRescale = OFFalse;


  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Encode DICOM file to JPEG transfer syntax", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--version",                                "print version information and exit", OFTrue /* exclusive */);
   cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");

  cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",              "+f",        "read file format or data set (default)");
      cmd.addOption("--read-dataset",           "-f",        "read data set without file meta information");
    cmd.addSubGroup("input transfer syntax (only with --read-dataset):", LONGCOL, SHORTCOL);
     cmd.addOption("--read-xfer-auto",          "-t=",       "use TS recognition (default)");
     cmd.addOption("--read-xfer-little",        "-te",       "read with explicit VR little endian TS");
     cmd.addOption("--read-xfer-big",           "-tb",       "read with explicit VR big endian TS");
     cmd.addOption("--read-xfer-implicit",      "-ti",       "read with implicit VR little endian TS");

  cmd.addGroup("JPEG encoding options:");
    cmd.addSubGroup("JPEG process options:");
     cmd.addOption("--encode-lossless-sv1",     "+e1",       "encode lossless sv1 (default)");
     cmd.addOption("--encode-lossless",         "+el",       "encode lossless");
     cmd.addOption("--encode-baseline",         "+eb",       "encode baseline");
     cmd.addOption("--encode-extended",         "+ee",       "encode extended sequential");
     cmd.addOption("--encode-spectral",         "+es",       "encode spectral selection");
     cmd.addOption("--encode-progressive",      "+ep",       "encode progressive");

    cmd.addSubGroup("lossless JPEG representation options:");
     cmd.addOption("--selection-value",         "+sv",    1, "[sv]: integer (1..7, default: 6)",
                                                             "use selection value sv\nonly with --encode-lossless");
     cmd.addOption("--point-transform",         "+pt",    1, "[pt]: integer (0..15, default: 0)",
                                                             "use point transform pt");

    cmd.addSubGroup("lossy JPEG representation options:");
     cmd.addOption("--quality",                 "+q",     1, "[q]: integer (0..100, default: 90)",
                                                             "use quality factor q");

    cmd.addSubGroup("other JPEG options:");
     cmd.addOption("--huffman-optimize",        "+ho",       "optimize huffman tables (default)");
     cmd.addOption("--huffman-standard",        "-ho",       "use standard huffman tables if 8 bits/sample");
     cmd.addOption("--smooth",                  "+sm",     1, "[s]: integer (0..100, default: 0)",
                                                             "use smoothing factor s");

    cmd.addSubGroup("compressed bits per sample options:");
     cmd.addOption("--bits-auto",               "+ba",       "choose bits/sample automatically (default)");
     cmd.addOption("--bits-force-8",            "+be",       "force 8 bits/sample");
     cmd.addOption("--bits-force-12",           "+bt",       "force 12 bits/sample (not with baseline)");
     cmd.addOption("--bits-force-16",           "+bs",       "force 16 bits/sample (lossless only)");

    cmd.addSubGroup("compression color space conversion options:");
      cmd.addOption("--color-ybr",              "+cy",       "use YCbCr for color images if lossy (default)");
      cmd.addOption("--color-rgb",              "+cr",       "use RGB for color images if lossy");
      cmd.addOption("--monochrome",             "+cm",       "convert color images to monochrome");

    cmd.addSubGroup("decompression color space conversion (if input is compressed):");
      cmd.addOption("--conv-photometric",       "+cp",       "convert if YCbCr photom. interpr. (default)");
      cmd.addOption("--conv-lossy",             "+cl",       "convert YCbCr to RGB if lossy JPEG");
      cmd.addOption("--conv-always",            "+ca",       "always convert YCbCr to RGB");
      cmd.addOption("--conv-never",             "+cn",       "never convert color space");

    cmd.addSubGroup("standard YCbCr component subsampling options:");
      cmd.addOption("--sample-444",             "+s4",       "4:4:4 sampling with YBR_FULL (default)");
      cmd.addOption("--sample-422",             "+s2",       "4:2:2 subsampling with YBR_FULL_422");

    cmd.addSubGroup("non-standard YCbCr component subsampling options:");
      cmd.addOption("--nonstd-422-full",        "+n2",       "4:2:2 subsampling with YBR_FULL");
      cmd.addOption("--nonstd-411-full",        "+n1",       "4:1:1 subsampling with YBR_FULL");
      cmd.addOption("--nonstd-411",             "+np",       "4:1:1 subsampling with YBR_FULL_422");

  cmd.addGroup("encapsulated pixel data encoding options:");
    cmd.addSubGroup("pixel data fragmentation options:");
     cmd.addOption("--fragment-per-frame",      "+ff",       "encode each frame as one fragment (default)");
     cmd.addOption("--fragment-size",           "+fs",    1, "[s]ize: integer",
                                                             "limit fragment size to s kbytes");
    cmd.addSubGroup("basic offset table encoding options:");
     cmd.addOption("--offset-table-create",     "+ot",       "create offset table (default)");
     cmd.addOption("--offset-table-empty",      "-ot",       "leave offset table empty");

    cmd.addSubGroup("VOI windowing options for monochrome images:");
     cmd.addOption("--no-windowing",       "-W",      "no VOI windowing (default)");
     cmd.addOption("--use-window",         "+Wi",  1, "[n]umber : integer",
                                                      "use the n-th VOI window from image file");
     cmd.addOption("--use-voi-lut",        "+Wl",  1, "[n]umber : integer",
                                                      "use the n-th VOI look up table from image file");
     cmd.addOption("--min-max-window",     "+Wm",     "compute VOI window using min-max algorithm");
     cmd.addOption("--min-max-window-n",   "+Wn",     "compute VOI window using min-max algorithm,\nignoring extreme values");
     cmd.addOption("--roi-min-max-window", "+Wr",  4, "[l]eft [t]op [w]idth [h]eight : integer",
                                                      "compute ROI window using min-max algorithm,\nregion of interest is specified by l,t,w,h");
     cmd.addOption("--histogram-window",   "+Wh",  1, "[n]umber: integer",
                                                      "compute VOI window using Histogram algorithm,\nignoring n percent");
     cmd.addOption("--set-window",         "+Ww",  2, "[c]enter [w]idth : float",
                                                      "compute VOI window using center c and width w");

    cmd.addSubGroup("pixel scaling for monochrome images (--no-windowing):");
     cmd.addOption("--scaling-pixel",      "+sp",     "scale using min/max pixel value (default)");
     cmd.addOption("--scaling-range",      "+sr",     "scale using min/max range");

    cmd.addSubGroup("rescale slope/intercept encoding for monochrome (--no-windowing):");
     cmd.addOption("--rescale-identity",   "+ri",     "encode identity modality rescale (default)\nNever used for CT images");
     cmd.addOption("--rescale-map",        "+rm",     "use modality rescale to scale pixel range\nNever used for XA/RF/XA Biplane images");

    cmd.addSubGroup("SOP Class UID options:");
     cmd.addOption("--class-default",      "+cd",     "keep SOP Class UID (default)");
     cmd.addOption("--class-sc",           "+cs",     "convert to Secondary Capture Image\n(implies --uid-always)");

    cmd.addSubGroup("SOP Instance UID options:");
     cmd.addOption("--uid-default",        "+ud",     "assign new UID if lossy compression (default)");
     cmd.addOption("--uid-always",         "+ua",     "always assign new UID");
     cmd.addOption("--uid-never",          "+un",     "never assign new UID");

  cmd.addGroup("output options:");
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
    cmd.addSubGroup("data set trailing padding:");
      cmd.addOption("--padding-retain",         "-p=",       "do not change padding (default)");
      cmd.addOption("--padding-off",            "-p",        "no padding");
      cmd.addOption("--padding-create",         "+p",    2,  "[f]ile-pad [i]tem-pad: integer",
                                                             "align file on multiple of f bytes\nand items on multiple of i bytes");

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
              CERR << endl << "External libraries used:" << endl;
#ifdef WITH_ZLIB
              CERR << "- ZLIB, Version " << zlibVersion() << endl;
#endif
              CERR << "- " << DiJPEGPlugin::getLibraryVersionString() << endl;
              return 0;
          }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      /* options */

      if (cmd.findOption("--verbose")) opt_verbose = OFTrue;
      if (cmd.findOption("--debug")) opt_debugMode = 5;

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

      // JPEG options

      cmd.beginOptionBlock();
      if (cmd.findOption("--encode-lossless-sv1")) opt_oxfer = EXS_JPEGProcess14SV1TransferSyntax;
      if (cmd.findOption("--encode-lossless")) opt_oxfer = EXS_JPEGProcess14TransferSyntax;
      if (cmd.findOption("--encode-baseline")) opt_oxfer = EXS_JPEGProcess1TransferSyntax;
      if (cmd.findOption("--encode-extended")) opt_oxfer = EXS_JPEGProcess2_4TransferSyntax;
      if (cmd.findOption("--encode-spectral")) opt_oxfer = EXS_JPEGProcess6_8TransferSyntax;
      if (cmd.findOption("--encode-progressive")) opt_oxfer = EXS_JPEGProcess10_12TransferSyntax;
      cmd.endOptionBlock();

      if (cmd.findOption("--selection-value"))
      {
          app.checkValue(cmd.getValueAndCheckMinMax(opt_selection_value, (OFCmdUnsignedInt)1, (OFCmdUnsignedInt)7));
          if (opt_oxfer != EXS_JPEGProcess14TransferSyntax)
            app.printError("--selection-value only allowed with --encode-lossless");
      }

      if (cmd.findOption("--point-transform"))
      {
          app.checkValue(cmd.getValueAndCheckMinMax(opt_point_transform, (OFCmdUnsignedInt)0, (OFCmdUnsignedInt)15));
          if ((opt_oxfer != EXS_JPEGProcess14SV1TransferSyntax) && (opt_oxfer != EXS_JPEGProcess14TransferSyntax))
            app.printError("--point-transform only allowed with lossless JPEG");
      }

      if (cmd.findOption("--quality"))
      {
          app.checkValue(cmd.getValueAndCheckMinMax(opt_quality, (OFCmdUnsignedInt)0, (OFCmdUnsignedInt)100));
          if ((opt_oxfer == EXS_JPEGProcess14SV1TransferSyntax) || (opt_oxfer == EXS_JPEGProcess14TransferSyntax))
            app.printError("--quality only allowed with lossy JPEG");
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--huffman-optimize")) opt_huffmanOptimize = OFTrue;
      if (cmd.findOption("--huffman-standard")) opt_huffmanOptimize = OFFalse;
      cmd.endOptionBlock();

      if (cmd.findOption("--smooth"))
      {
          app.checkValue(cmd.getValueAndCheckMinMax(opt_smoothing, (OFCmdUnsignedInt)0, (OFCmdUnsignedInt)100));
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--bits-auto"))     opt_compressedBits = 0;
      if (cmd.findOption("--bits-force-8"))  opt_compressedBits = 8;
      if (cmd.findOption("--bits-force-12"))
      {
        opt_compressedBits = 12;
        if (opt_oxfer == EXS_JPEGProcess1TransferSyntax)
        {
          app.printError("--bits-force-12 not allowed with --encode-baseline");
        }
      }
      if (cmd.findOption("--bits-force-16"))
      {
        opt_compressedBits = 16;
        switch (opt_oxfer)
        {
          case EXS_JPEGProcess1TransferSyntax:
            app.printError("--bits-force-16 not allowed with --encode-baseline");
            break;
          case EXS_JPEGProcess2_4TransferSyntax:
            app.printError("--bits-force-16 not allowed with --encode-extended");
            break;
          case EXS_JPEGProcess6_8TransferSyntax:
            app.printError("--bits-force-16 not allowed with --encode-spectral");
            break;
          case EXS_JPEGProcess10_12TransferSyntax:
            app.printError("--bits-force-16 not allowed with --encode-progressive");
            break;
          default:
            /* do nothing */
            break;
        }
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--color-ybr"))  opt_compCSconversion = ECC_lossyYCbCr;
      if (cmd.findOption("--color-rgb"))  opt_compCSconversion = ECC_lossyRGB;
      if (cmd.findOption("--monochrome")) opt_compCSconversion = ECC_monochrome;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--conv-photometric"))  opt_decompCSconversion = EDC_photometricInterpretation;
      if (cmd.findOption("--conv-lossy"))        opt_decompCSconversion = EDC_lossyOnly;
      if (cmd.findOption("--conv-always"))       opt_decompCSconversion = EDC_always;
      if (cmd.findOption("--conv-never"))        opt_decompCSconversion = EDC_never;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--sample-444"))
      {
        opt_sampleFactors = ESS_444;
        opt_useYBR422 = OFFalse;
      }
      if (cmd.findOption("--sample-422"))
      {
        opt_sampleFactors = ESS_422;
        opt_useYBR422 = OFTrue;
      }
      if (cmd.findOption("--nonstd-422-full"))
      {
        opt_sampleFactors = ESS_422;
        opt_useYBR422 = OFFalse;
      }
      if (cmd.findOption("--nonstd-411-full"))
      {
        opt_sampleFactors = ESS_411;
        opt_useYBR422 = OFFalse;
      }
      if (cmd.findOption("--nonstd-411"))
      {
        opt_sampleFactors = ESS_411;
        opt_useYBR422 = OFTrue;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--fragment-per-frame")) opt_fragmentSize = 0;
      if (cmd.findOption("--fragment-size"))
      {
        app.checkValue(cmd.getValueAndCheckMin(opt_fragmentSize, (OFCmdUnsignedInt)1));
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--offset-table-create")) opt_createOffsetTable = OFTrue;
      if (cmd.findOption("--offset-table-empty")) opt_createOffsetTable = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--no-windowing")) opt_windowType = 0;
      if (cmd.findOption("--use-window"))
      {
          opt_windowType = 1;
          app.checkValue(cmd.getValueAndCheckMin(opt_windowParameter, 1));
      }
      if (cmd.findOption("--use-voi-lut"))
      {
          opt_windowType = 2;
          app.checkValue(cmd.getValueAndCheckMin(opt_windowParameter, 1));
      }
      if (cmd.findOption("--min-max-window")) opt_windowType = 3;
      if (cmd.findOption("--min-max-window-n")) opt_windowType = 6;
      if (cmd.findOption("--roi-min-max-window"))
      {
          opt_windowType = 7;
          app.checkValue(cmd.getValue(opt_roiLeft));
          app.checkValue(cmd.getValue(opt_roiTop));
          app.checkValue(cmd.getValueAndCheckMin(opt_roiWidth, 1));
          app.checkValue(cmd.getValueAndCheckMin(opt_roiHeight, 1));
      }
      if (cmd.findOption("--histogram-window"))
      {
          opt_windowType = 4;
          app.checkValue(cmd.getValueAndCheckMinMax(opt_windowParameter, 0, 100));
      }
      if (cmd.findOption("--set-window"))
      {
          opt_windowType = 5;
          app.checkValue(cmd.getValue(opt_windowCenter));
          app.checkValue(cmd.getValueAndCheckMin(opt_windowWidth, 1.0));
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--scaling-pixel")) opt_usePixelValues = OFTrue;
      if (cmd.findOption("--scaling-range")) opt_usePixelValues = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--rescale-identity")) opt_useModalityRescale = OFFalse;
      if (cmd.findOption("--rescale-map")) opt_useModalityRescale = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--class-default")) opt_secondarycapture = OFFalse;
      if (cmd.findOption("--class-sc")) opt_secondarycapture = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-default")) opt_uidcreation = EUC_default;
      if (cmd.findOption("--uid-always")) opt_uidcreation = EUC_always;
      if (cmd.findOption("--uid-never")) opt_uidcreation = EUC_never;
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
      if (cmd.findOption("--padding-retain")) opt_opadenc = EPD_noChange;
      if (cmd.findOption("--padding-off")) opt_opadenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
          app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
          app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
          opt_opadenc = EPD_withPadding;
      }
      cmd.endOptionBlock();

    }

    SetDebugLevel((opt_debugMode));

    // register global decompression codecs
    DJDecoderRegistration::registerCodecs(
      opt_decompCSconversion,
      opt_uidcreation,
      EPC_default,
      opt_verbose);

    // register global compression codecs
    DJEncoderRegistration::registerCodecs(
      opt_compCSconversion,
      opt_uidcreation,
      opt_verbose,
      opt_huffmanOptimize,
      (int) opt_smoothing,
      opt_compressedBits,
      (Uint32) opt_fragmentSize,
      opt_createOffsetTable,
      opt_sampleFactors,
      opt_useYBR422,
      opt_secondarycapture,
      opt_windowType,
      opt_windowParameter,
      opt_windowCenter,
      opt_windowWidth,
      opt_roiLeft,
      opt_roiTop,
      opt_roiWidth,
      opt_roiHeight,
      opt_usePixelValues,
      opt_useModalityRescale);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        CERR << "invalid filename: <empty string>" << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "reading input file " << opt_ifname << endl;

    DcmFileFormat fileformat;
    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer,
        EGL_noChange, DCM_MaxReadLength, opt_iDataset);
    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": reading file: " <<  opt_ifname << endl;
        return 1;
    }
    DcmDataset *dataset = fileformat.getDataset();

    DcmXfer original_xfer(dataset->getOriginalXfer());
    if (original_xfer.isEncapsulated())
    {
      if (opt_verbose)
        COUT << "DICOM file is already compressed, convert to uncompressed xfer syntax first\n";
      if (EC_Normal != dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL))
      {
        CERR << "No conversion from compressed original to uncompressed xfer syntax possible!\n";
        return 1;
      }
    }

    if (opt_verbose)
        COUT << "Convert DICOM file to compressed transfer syntax\n";

    DcmXfer opt_oxferSyn(opt_oxfer);

    // create representation parameters for lossy and lossless
    DJ_RPLossless rp_lossless((int)opt_selection_value, (int)opt_point_transform);
    DJ_RPLossy rp_lossy((int)opt_quality);

    const DcmRepresentationParameter *rp = &rp_lossy;
    if ((opt_oxfer == EXS_JPEGProcess14SV1TransferSyntax)||
        (opt_oxfer == EXS_JPEGProcess14TransferSyntax))
        rp = &rp_lossless;

    dataset->chooseRepresentation(opt_oxfer, rp);
    if (dataset->canWriteXfer(opt_oxfer))
    {
        if (opt_verbose)
            COUT << "Output transfer syntax " << opt_oxferSyn.getXferName()
                 << " can be written\n";
    } else {
        CERR << "No conversion to transfer syntax " << opt_oxferSyn.getXferName()
             << " possible!\n";
        return 1;
    }

    // force meta-header to refresh SOP Class/Instance UIDs.
    DcmItem *metaInfo = fileformat.getMetaInfo();
    if (metaInfo)
    {
      delete metaInfo->remove(DCM_MediaStorageSOPClassUID);
      delete metaInfo->remove(DCM_MediaStorageSOPInstanceUID);
    }

    if (opt_verbose)
        COUT << "creating output file " << opt_ofname << endl;

    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc,
              opt_opadenc, (Uint32) opt_filepad, (Uint32) opt_itempad);

    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": writing file: " <<  opt_ofname << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "conversion successful\n";

    // deregister global codecs
    DJDecoderRegistration::cleanup();
    DJEncoderRegistration::cleanup();

    return 0;
}

/*
 * CVS/RCS Log:
 * $Log: dcmcjpeg.cc,v $
 * Revision 1.7  2002-11-26 08:44:41  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.6  2002/09/23 18:14:06  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.5  2002/08/20 12:20:58  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.4  2002/07/10 12:26:01  meichel
 * Fixed memory leak in command line applications
 *
 * Revision 1.3  2001/12/20 10:41:45  meichel
 * Fixed warnings reported by Sun CC 2.0.1
 *
 * Revision 1.2  2001/11/19 15:13:22  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:56:09  meichel
 * Initial release of module dcmjpeg
 *
 *
 */

