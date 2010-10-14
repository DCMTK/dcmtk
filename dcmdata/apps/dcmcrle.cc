/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Compress DICOM file with RLE Transfer Syntax
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.22 $
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

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmtk version name */
#include "dcmtk/dcmdata/dcrleerg.h"  /* for DcmRLEEncoderRegistration */

#ifdef WITH_ZLIB
#include <zlib.h>      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmcrle"

static OFLogger dcmcrleLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

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

  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  E_FileReadMode opt_readMode = ERM_autoDetect;
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

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Encode DICOM file to RLE transfer syntax", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                  "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
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

  cmd.addGroup("encapsulated pixel data encoding options:");
    cmd.addSubGroup("pixel data fragmentation:");
      cmd.addOption("--fragment-per-frame",  "+ff",    "encode each frame as one fragment (default)");
      cmd.addOption("--fragment-size",       "+fs", 1, "[s]ize: integer",
                                                       "limit fragment size to s kbytes (non-standard)");
    cmd.addSubGroup("basic offset table encoding:");
      cmd.addOption("--offset-table-create", "+ot",    "create offset table (default)");
      cmd.addOption("--offset-table-empty",  "-ot",    "leave offset table empty");

    cmd.addSubGroup("SOP Class UID:");
      cmd.addOption("--class-default",       "+cd",    "keep SOP Class UID (default)");
      cmd.addOption("--class-sc",            "+cs",    "convert to Secondary Capture Image\n(implies --uid-always)");

    cmd.addSubGroup("SOP Instance UID:");
      cmd.addOption("--uid-never",           "+un",    "never assign new UID (default)");
      cmd.addOption("--uid-always",          "+ua",    "always assign new UID");

  cmd.addGroup("output options:");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",       "+u",     "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",      "-u",     "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc", "+g=",    "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create", "+g",     "always write with group length elements");
      cmd.addOption("--group-length-remove", "-g",     "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",     "+e",     "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",    "-e",     "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding:");
      cmd.addOption("--padding-retain",      "-p=",    "do not change padding (default)");
      cmd.addOption("--padding-off",         "-p",     "no padding");
      cmd.addOption("--padding-create",      "+p",  2, "[f]ile-pad [i]tem-pad: integer",
                                                       "align file on multiple of f bytes\nand items on multiple of i bytes");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
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

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

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

      // RLE options

      cmd.beginOptionBlock();
      if (cmd.findOption("--fragment-per-frame")) opt_fragmentSize = 0;
      if (cmd.findOption("--fragment-size"))
      {
        app.checkValue(cmd.getValueAndCheckMin(opt_fragmentSize, OFstatic_cast(OFCmdUnsignedInt, 1)));
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

    /* print resource identifier */
    OFLOG_DEBUG(dcmcrleLogger, rcsid << OFendl);

    // register RLE compression codec
    DcmRLEEncoderRegistration::registerCodecs(opt_uidcreation,
      opt_fragmentSize, opt_createOffsetTable, opt_secondarycapture);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmcrleLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcmcrleLogger, "invalid filename: <empty string>");
        return 1;
    }

    DcmFileFormat fileformat;
    DcmDataset * dataset = fileformat.getDataset();

    OFLOG_INFO(dcmcrleLogger, "open input file " << opt_ifname);

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcmcrleLogger, error.text() << ": reading file: " << opt_ifname);
        return 1;
    }

    DcmXfer original_xfer(dataset->getOriginalXfer());
    if (original_xfer.isEncapsulated())
    {
      OFLOG_INFO(dcmcrleLogger, "DICOM file is already compressed, converting to uncompressed transfer syntax first");
      if (EC_Normal != dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL))
      {
        OFLOG_FATAL(dcmcrleLogger, "No conversion from compressed original to uncompressed transfer syntax possible!");
        return 1;
      }
    }
    
    OFString sopClass;
    if (fileformat.getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass).good())
    {
        /* check for DICOMDIR files */
        if (sopClass == UID_MediaStorageDirectoryStorage)
        {
            OFLOG_FATAL(dcmcrleLogger, "DICOMDIR files (Media Storage Directory Storage SOP Class) cannot be compressed!");
            return 1;
        }
    }

    OFLOG_INFO(dcmcrleLogger, "Convert DICOM file to compressed transfer syntax");

    DcmXfer opt_oxferSyn(opt_oxfer);

    dataset->chooseRepresentation(opt_oxfer, NULL);
    if (dataset->canWriteXfer(opt_oxfer))
    {
        OFLOG_INFO(dcmcrleLogger, "Output transfer syntax " << opt_oxferSyn.getXferName() << " can be written");
    } else {
        OFLOG_FATAL(dcmcrleLogger, "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
        return 1;
    }

    OFLOG_INFO(dcmcrleLogger, "create output file " << opt_ofname);

    fileformat.loadAllDataIntoMemory();
    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), EWM_updateMeta);

    if (error.bad())
    {
        OFLOG_FATAL(dcmcrleLogger, error.text() << ": writing file: " << opt_ofname);
        return 1;
    }

    OFLOG_INFO(dcmcrleLogger, "conversion successful");

    // deregister RLE codec
    DcmRLEEncoderRegistration::cleanup();

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmcrle.cc,v $
 * Revision 1.22  2010-10-14 13:13:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.21  2009-11-13 13:20:23  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.20  2009-11-04 09:58:06  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.19  2009-08-21 09:24:07  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 * Added check making sure that a DICOMDIR file is never compressed.
 * Use helper function checkConflict() where appropriate.
 * Made error messages more consistent with other compression tools.
 *
 * Revision 1.18  2009-08-05 10:52:49  joergr
 * Fixed various issues with syntax usage (e.g. layout and formatting).
 *
 * Revision 1.17  2009-04-21 14:02:49  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.16  2009-03-19 12:06:42  joergr
 * Replaced '\n' by OFendl where appropriate.
 *
 * Revision 1.15  2008-09-25 14:38:48  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.14  2008-09-25 11:19:48  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.13  2006/08/15 15:50:56  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.12  2006/07/27 13:52:42  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.11  2005/12/08 15:40:44  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2005/12/02 09:01:30  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 * Added new command line option that checks whether a given file starts with a
 * valid DICOM meta header.
 *
 * Revision 1.9  2005/11/07 17:10:19  meichel
 * All tools that both read and write a DICOM file now call loadAllDataIntoMemory()
 *   to make sure they do not destroy a file when output = input.
 *
 * Revision 1.8  2004/02/25 13:34:41  meichel
 * Marked option --fragment-size as non-standard since it violates a rule defined
 *   in DICOM Part 5 A.4.2: "Each frame shall be encoded in one and only one fragment".
 *
 * Revision 1.7  2004/01/16 10:53:53  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.6  2002/11/27 12:07:16  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.5  2002/11/26 08:42:59  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.4  2002/09/23 17:52:02  joergr
 * Prepared code for future support of 'config.guess' host identifiers.
 *
 * Revision 1.3  2002/09/23 13:50:40  joergr
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
