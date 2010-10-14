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
 *  Purpose: Decompress RLE-compressed DICOM file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.23 $
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
#include "dcmtk/dcmdata/dcrledrg.h"  /* for DcmRLEDecoderRegistration */

#ifdef WITH_ZLIB
#include <zlib.h>      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmdrle"

static OFLogger dcmdrleLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

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

  E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;
  E_FileReadMode opt_readMode = ERM_autoDetect;
  E_FileWriteMode opt_writeMode = EWM_fileformat;
  E_TransferSyntax opt_ixfer = EXS_Unknown;

  // RLE parameters
  OFBool opt_uidcreation = OFFalse;
  OFBool opt_reversebyteorder = OFFalse;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Decode RLE-compressed DICOM file", rcsid);
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

  cmd.addGroup("processing options:");
    cmd.addSubGroup("SOP Instance UID:");
      cmd.addOption("--uid-default",         "+ud",    "keep same SOP Instance UID (default)");
      cmd.addOption("--uid-always",          "+ua",    "always assign new UID");
    cmd.addSubGroup("RLE byte segment order:");
      cmd.addOption("--byte-order-default",  "+bd",    "most significant byte first (default)");
      cmd.addOption("--byte-order-reverse",  "+br",    "least significant byte first");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",     "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian (default)");
      cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
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
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",      "-p=",    "do not change padding\n(default if not --write-dataset)");
      cmd.addOption("--padding-off",         "-p",     "no padding (implicit if --write-dataset)");
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
      if (cmd.findOption("--uid-default")) opt_uidcreation = OFFalse;
      if (cmd.findOption("--uid-always")) opt_uidcreation = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--byte-order-default")) opt_reversebyteorder = OFFalse;
      if (cmd.findOption("--byte-order-reverse")) opt_reversebyteorder = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file"))
      {
        opt_readMode = ERM_autoDetect;
        opt_ixfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-file-only"))
      {
        opt_readMode = ERM_fileOnly;
        opt_ixfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-dataset"))
      {
        opt_readMode = ERM_dataset;
        opt_ixfer = EXS_RLELossless;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_writeMode = EWM_fileformat;
      if (cmd.findOption("--write-dataset")) opt_writeMode = EWM_dataset;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
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
        app.checkConflict("--padding-retain", "--write-dataset", opt_writeMode == EWM_dataset);
        opt_opadenc = EPD_noChange;
      }
      if (cmd.findOption("--padding-off")) opt_opadenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
        app.checkConflict("--padding-create", "--write-dataset", opt_writeMode == EWM_dataset);
        app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
        app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
        opt_opadenc = EPD_withPadding;
      }
      cmd.endOptionBlock();
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmdrleLogger, rcsid << OFendl);

    // register global decompression codecs
    DcmRLEDecoderRegistration::registerCodecs(opt_uidcreation, opt_reversebyteorder);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmdrleLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcmdrleLogger, "invalid filename: <empty string>");
        return 1;
    }

    DcmFileFormat fileformat;
    DcmDataset * dataset = fileformat.getDataset();

    OFLOG_INFO(dcmdrleLogger, "open input file " << opt_ifname);

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcmdrleLogger, error.text() << ": reading file: " <<  opt_ifname);
        return 1;
    }

    OFLOG_INFO(dcmdrleLogger, "decompressing file");

    DcmXfer opt_oxferSyn(opt_oxfer);
    DcmXfer original_xfer(dataset->getOriginalXfer());

    error = dataset->chooseRepresentation(opt_oxfer, NULL);
    if (error.bad())
    {
        OFLOG_FATAL(dcmdrleLogger, error.text() << ": decompressing file: " <<  opt_ifname);
        if (error == EC_CannotChangeRepresentation)
            OFLOG_FATAL(dcmdrleLogger, "input transfer syntax " << original_xfer.getXferName() << " not supported");
        return 1;
    }

    if (! dataset->canWriteXfer(opt_oxfer))
    {
        OFLOG_FATAL(dcmdrleLogger, "no conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible");
        return 1;
    }

    OFLOG_INFO(dcmdrleLogger, "create output file " << opt_ofname);

    // update file meta information with new SOP Instance UID
    if (opt_uidcreation && (opt_writeMode == EWM_fileformat))
        opt_writeMode = EWM_updateMeta;

    fileformat.loadAllDataIntoMemory();
    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcmdrleLogger, error.text() << ": writing file: " <<  opt_ofname);
        return 1;
    }

    OFLOG_INFO(dcmdrleLogger, "conversion successful");

    // deregister RLE codec
    DcmRLEDecoderRegistration::cleanup();

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmdrle.cc,v $
 * Revision 1.23  2010-10-14 13:13:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.22  2009-11-13 13:20:23  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.21  2009-11-04 09:58:06  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.20  2009-08-21 09:23:37  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 * Use helper function checkConflict() where appropriate.
 *
 * Revision 1.19  2009-08-05 10:52:49  joergr
 * Fixed various issues with syntax usage (e.g. layout and formatting).
 *
 * Revision 1.18  2009-04-21 14:02:49  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.17  2009-03-19 12:07:49  joergr
 * Added more explicit message in case input transfer syntax is not supported.
 *
 * Revision 1.16  2008-09-25 14:38:48  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.15  2008-09-25 11:19:48  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.14  2006/08/15 15:50:56  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.13  2006/07/27 13:52:42  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.12  2005/12/08 15:40:45  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.11  2005/12/02 09:03:52  joergr
 * Added new command line option that ignores the transfer syntax specified in
 * the meta header and tries to detect the transfer syntax automatically from
 * the dataset.
 *
 * Revision 1.10  2005/11/07 17:10:19  meichel
 * All tools that both read and write a DICOM file now call loadAllDataIntoMemory()
 *   to make sure they do not destroy a file when output = input.
 *
 * Revision 1.9  2005/07/26 17:08:28  meichel
 * Added option to RLE decoder that allows to correctly decode images with
 *   incorrect byte order of byte segments (LSB instead of MSB).
 *
 * Revision 1.8  2005/05/26 14:52:22  meichel
 * Added option --read-dataset to dcmdrle that allows to decompress RLE
 *   compressed DICOM objects that have been stored as dataset without meta-header.
 *   Such a thing should not exist since the transfer syntax cannot be reliably
 *   determined without meta-header, but unfortunately it does.
 *
 * Revision 1.7  2004/01/16 10:53:53  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.6  2002/11/27 12:07:16  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.5  2002/11/26 08:43:00  meichel
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
 * Revision 1.1  2002/06/06 14:52:32  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
