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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Compress DICOM file with RLE Transfer Syntax
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-23 13:50:40 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmcrle.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
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
#include "dcrleerg.h"  /* for DcmRLEEncoderRegistration */

#ifdef WITH_ZLIB
#include "zlib.h"      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmcrle"

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

  // RLE options
  E_TransferSyntax opt_oxfer = EXS_RLELossless;
  OFCmdUnsignedInt opt_fragmentSize = 0; // 0=unlimited
  OFBool           opt_createOffsetTable = OFTrue;
  OFBool           opt_uidcreation = OFFalse;
  OFBool           opt_secondarycapture = OFFalse;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Encode DICOM file to RLE transfer syntax", rcsid);
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

  cmd.addGroup("encapsulated pixel data encoding options:");
    cmd.addSubGroup("pixel data fragmentation options:");
     cmd.addOption("--fragment-per-frame",      "+ff",       "encode each frame as one fragment (default)");
     cmd.addOption("--fragment-size",           "+fs",    1, "[s]ize: integer",
                                                             "limit fragment size to s kbytes");
    cmd.addSubGroup("basic offset table encoding options:");
     cmd.addOption("--offset-table-create",     "+ot",       "create offset table (default)");
     cmd.addOption("--offset-table-empty",      "-ot",       "leave offset table empty");

    cmd.addSubGroup("SOP Class UID options:");
     cmd.addOption("--class-default",      "+cd",     "keep SOP Class UID (default)");
     cmd.addOption("--class-sc",           "+cs",     "convert to Secondary Capture Image\n(implies --uid-always)");

    cmd.addSubGroup("SOP Instance UID options:");
     cmd.addOption("--uid-never",          "+un",     "never assign new UID (default)");
     cmd.addOption("--uid-always",         "+ua",     "always assign new UID");

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
            app.printHeader();          // uses ofConsole.lockCerr()
            CERR << endl << "External libraries used:" << endl;
#ifdef WITH_ZLIB
            CERR << "- ZLIB, Version " << zlibVersion() << endl;
#endif
            return 0;
         }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

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

      // RLE options

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
      if (cmd.findOption("--class-default")) opt_secondarycapture = OFFalse;
      if (cmd.findOption("--class-sc")) opt_secondarycapture = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-always")) opt_uidcreation = OFTrue;
      if (cmd.findOption("--uid-never")) opt_uidcreation = OFFalse;
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

    // register RLE compression codec
    DcmRLEEncoderRegistration::registerCodecs(opt_uidcreation, opt_verbose,
      opt_fragmentSize, opt_createOffsetTable, opt_secondarycapture);

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

    DcmFileFormat fileformat;
    DcmDataset * dataset = fileformat.getDataset();

    if (opt_verbose)
        COUT << "open input file " << opt_ifname << endl;

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_iDataset);

    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": reading file: " <<  opt_ifname << endl;
        return 1;
    }

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

    dataset->chooseRepresentation(opt_oxfer, NULL);
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
        COUT << "create output file " << opt_ofname << endl;

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

    // deregister RLE codec
    DcmRLEEncoderRegistration::cleanup();

    return 0;
}

/*
 * CVS/RCS Log:
 * $Log: dcmcrle.cc,v $
 * Revision 1.3  2002-09-23 13:50:40  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used.
 *
 * Revision 1.2  2002/08/21 10:14:14  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.1  2002/06/06 14:52:31  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
