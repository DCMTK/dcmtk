/*
 *
 *  Copyright (C) 2003-2009, OFFIS
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
 *  Purpose: Convert the contents of an XML document to a DICOM structured
 *            reporting file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-05-07 08:56:51 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmdata/dcdebug.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "xml2dsr"
#define OFFIS_CONSOLE_DESCRIPTION "Convert XML document to DICOM SR file"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


#ifdef WITH_LIBXML

#include <libxml/parser.h>


#define SHORTCOL 3
#define LONGCOL 21


int main(int argc, char *argv[])
{
    int opt_debug = 0;
    OFBool opt_verbose = OFFalse;
    OFBool opt_dataset = OFFalse;
    OFBool opt_generateUIDs = OFFalse;
    OFBool opt_overwriteUIDs = OFFalse;
    size_t opt_readFlags = 0;
    E_TransferSyntax opt_xfer = EXS_LittleEndianExplicit;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_recalcGL;
    E_PaddingEncoding opt_padenc = EPD_withoutPadding;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;

    SetDebugLevel(( 0 ));

    /* set-up command line parameters and options */
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("xmlfile-in",   "XML input filename to be converted (stdin: \"-\")", OFCmdParam::PM_Mandatory);
    cmd.addParam("dsrfile-out",  "DICOM SR output filename", OFCmdParam::PM_Mandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--arguments",                       "print expanded command line arguments");
      cmd.addOption("--verbose",               "-v",     "verbose mode, print processing details");
      cmd.addOption("--debug",                 "-d",     "debug mode, print debug information");

    cmd.addGroup("input options:");
      cmd.addSubGroup("encoding:");
        cmd.addOption("--template-envelope",   "+Ee",    "template element encloses content items");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("validation:");
#ifdef LIBXML_SCHEMAS_ENABLED
        cmd.addOption("--validate-schema",     "+Vs",    "validate XML document against Schema\n(not with --template-envelope)");
#endif
        cmd.addOption("--check-namespace",     "+Vn",    "check XML namespace in document root");
      cmd.addSubGroup("unique identifiers:");
        cmd.addOption("--generate-new-uids",   "+Ug",    "generate new Study/Series/SOP Instance UID");
        cmd.addOption("--dont-overwrite-uids", "-Uo",    "do not overwrite existing UIDs (default)");
        cmd.addOption("--overwrite-uids",      "+Uo",    "overwrite existing UIDs");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output file format:");
        cmd.addOption("--write-file",          "+F",     "write file format (default)");
        cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
      cmd.addSubGroup("output transfer syntax:");
        cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian TS (def.)");
        cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
        cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
        cmd.addOption("--write-xfer-deflated", "+td",    "write with deflated expl. VR little endian TS");
#endif
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
#ifdef WITH_ZLIB
      cmd.addSubGroup("deflate compression level (only with --write-xfer-deflated):");
        cmd.addOption("--compression-level",   "+cl", 1, "[l]evel: integer (default: 6)",
                                                         "0=uncompressed, 1=fastest, 9=best compression");
#endif

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
        /* check whether to print the command line arguments */
        if (cmd.findOption("--arguments"))
            app.printArguments();

        /* check exclusive options first */
        if (cmd.hasExclusiveOption())
        {
            if (cmd.findOption("--version"))
            {
                app.printHeader(OFTrue /*print host identifier*/);
                COUT << OFendl << "External libraries used:" << OFendl;
#ifdef WITH_ZLIB
                COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
                COUT << "- LIBXML, Version " << LIBXML_DOTTED_VERSION << OFendl;
#ifndef LIBXML_SCHEMAS_ENABLED
                COUT << "  without XML Schema support" << OFendl;
#endif
                return 0;
            }
        }

        /* general options */
        if (cmd.findOption("--verbose"))
            opt_verbose = OFTrue;
        if (cmd.findOption("--debug"))
        {
            opt_debug = 5;
            opt_readFlags |= DSRTypes::XF_enableLibxmlErrorOutput;
        }

        /* input options */
        if (cmd.findOption("--template-envelope"))
            opt_readFlags |= DSRTypes::XF_templateElementEnclosesItems;

        /* processing options */
#ifdef LIBXML_SCHEMAS_ENABLED
        if (cmd.findOption("--validate-schema"))
            opt_readFlags |= DSRTypes::XF_validateSchema;
#endif
        if (cmd.findOption("--check-namespace"))
            opt_readFlags |= DSRTypes::XF_useDcmsrNamespace;

        if (cmd.findOption("--generate-new-uids"))
            opt_generateUIDs = OFTrue;

        cmd.beginOptionBlock();
        if (cmd.findOption("--dont-overwrite-uids"))
            opt_overwriteUIDs = OFFalse;
        if (cmd.findOption("--overwrite-uids"))
            opt_overwriteUIDs = OFTrue;
        cmd.endOptionBlock();

        /* output options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--write-file"))
            opt_dataset = OFFalse;
        if (cmd.findOption("--write-dataset"))
            opt_dataset = OFTrue;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--write-xfer-little"))
            opt_xfer = EXS_LittleEndianExplicit;
        if (cmd.findOption("--write-xfer-big"))
            opt_xfer = EXS_BigEndianExplicit;
        if (cmd.findOption("--write-xfer-implicit"))
            opt_xfer = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
        if (cmd.findOption("--write-xfer-deflated"))
            opt_xfer = EXS_DeflatedLittleEndianExplicit;
#endif
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
        if (cmd.findOption("--group-length-recalc"))
            opt_glenc = EGL_recalcGL;
        if (cmd.findOption("--group-length-create"))
            opt_glenc = EGL_withGL;
        if (cmd.findOption("--group-length-remove"))
            opt_glenc = EGL_withoutGL;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--length-explicit"))
            opt_enctype = EET_ExplicitLength;
        if (cmd.findOption("--length-undefined"))
            opt_enctype = EET_UndefinedLength;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--padding-retain"))
        {
            app.checkConflict("--padding-retain", "--write-dataset", opt_dataset);
            opt_padenc = EPD_noChange;
        }
        if (cmd.findOption("--padding-off"))
            opt_padenc = EPD_withoutPadding;
        if (cmd.findOption("--padding-create"))
        {
            app.checkConflict("--padding-create", "--write-dataset", opt_dataset);
            app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
            app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
            opt_padenc = EPD_withPadding;
        }
        cmd.endOptionBlock();

#ifdef WITH_ZLIB
        if (cmd.findOption("--compression-level"))
        {
            OFCmdUnsignedInt comprLevel = 0;
            app.checkDependence("--compression-level", "--write-xfer-deflated", opt_xfer == EXS_DeflatedLittleEndianExplicit);
            app.checkValue(cmd.getValueAndCheckMinMax(comprLevel, 0, 9));
            dcmZlibCompressionLevel.set(OFstatic_cast(int, comprLevel));
        }
#endif

        /* check conflicts and dependencies */
        if (opt_readFlags & DSRTypes::XF_validateSchema)
            app.checkConflict("--validate-schema", "--template-envelope", (opt_readFlags & DSRTypes::XF_templateElementEnclosesItems) > 0);
    }

    if (opt_debug)
        app.printIdentifier();
    SetDebugLevel((opt_debug));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << OFendl;
    }

    /* check for compatible libxml version */
    LIBXML_TEST_VERSION
    /* initialize the XML library (only required for MT-safety) */
    xmlInitParser();

    OFCondition result = EC_Normal;
    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
    cmd.getParam(1, opt_ifname);
    cmd.getParam(2, opt_ofname);

    /* check filenames */
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid input filename: <empty string>" << OFendl;
        result = EC_IllegalParameter;
    }
    if ((opt_ofname == NULL) || (strlen(opt_ofname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid output filename: <empty string>" << OFendl;
        result = EC_IllegalParameter;
    }

    if (result.good())
    {
        /* create new SR document */
        DSRDocument *dsrdoc = new DSRDocument();
        if (dsrdoc != NULL)
        {
            DcmFileFormat fileformat;
            if (opt_debug)
                dsrdoc->setLogStream(&ofConsole);
            if (opt_verbose)
            {
                COUT << "reading ";
#ifdef LIBXML_SCHEMAS_ENABLED
                if (opt_readFlags & DSRTypes::XF_validateSchema)
                    COUT << "and validating ";
#endif
                COUT << "XML input file: " << opt_ifname << OFendl;
            }
            /* read XML file and feed data into DICOM fileformat */
            result = dsrdoc->readXML(opt_ifname, opt_readFlags);
            if (result.good())
            {
                DcmDataset *dataset = fileformat.getDataset();
                if (opt_verbose)
                    COUT << "writing DICOM SR output file: " << opt_ofname << OFendl;
                /* write SR document to dataset */
                result = dsrdoc->write(*dataset);
                /* generate new UIDs (if required) */
                if (opt_generateUIDs)
                {
                    char uid[100];
                    if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_StudyInstanceUID))
                    {
                        if (opt_verbose)
                            COUT << "generating new Study Instance UID" << OFendl;
                        dataset->putAndInsertString(DCM_StudyInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
                    }
                    if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SeriesInstanceUID))
                    {
                        if (opt_verbose)
                            COUT << "generating new Series Instance UID" << OFendl;
                        dataset->putAndInsertString(DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
                    }
                    if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SOPInstanceUID))
                    {
                        if (opt_verbose)
                            COUT << "generating new SOP Instance UID" << OFendl;
                        dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
                    }
                }
                /* write DICOM file */
                if (result.good())
                    result = fileformat.saveFile(opt_ofname, opt_xfer, opt_enctype, opt_glenc, opt_padenc,
                                                 OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad),
                                                 opt_dataset);
                if (result.bad())
                    CERR << "Error: " << result.text() << ": writing file: "  << opt_ofname << OFendl;
            } else
                CERR << "Error: " << result.text() << ": reading file: "  << opt_ifname << OFendl;
        }
        delete dsrdoc;
    }

    /* clean up XML library before quitting */
    xmlCleanupParser();

    return result.status();
}

#else /* WITH_LIBXML */

int main(int, char *[])
{
  CERR << rcsid << OFendl << OFFIS_CONSOLE_DESCRIPTION << OFendl << OFendl
       << OFFIS_CONSOLE_APPLICATION " requires the libxml library." << OFendl
       << "This " OFFIS_CONSOLE_APPLICATION " has been configured and compiled without libxml." << OFendl
       << "Please reconfigure your system and recompile if appropriate." << OFendl;
  return 0;
}

#endif /* WITH_LIBXML */


/*
 * CVS/RCS Log:
 * $Log: xml2dsr.cc,v $
 * Revision 1.11  2009-05-07 08:56:51  joergr
 * Added new command line options that allow for generating new Study/Series/SOP
 * Instance UIDs (incl. an option for overwriting existing values).
 *
 * Revision 1.10  2008-11-03 15:46:28  joergr
 * Removed "option block" encapsulation from option --compression-level.
 *
 * Revision 1.9  2008-11-03 15:36:51  joergr
 * Added ZLIB related output options --write-xfer-deflated, --compression-level.
 *
 * Revision 1.8  2008-09-25 14:14:21  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.7  2006-08-15 16:40:02  meichel
 * Updated the code in module dcmsr to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.6  2006/07/27 14:52:00  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.5  2005/12/08 15:47:36  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2005/03/22 13:56:14  joergr
 * Added call of macro LIBXML_TEST_VERSION.
 *
 * Revision 1.3  2004/09/09 13:58:36  joergr
 * Added option to control the way the template identification is encoded for
 * the XML output ("inside" or "outside" of the content items).
 *
 * Revision 1.2  2004/08/04 12:12:37  joergr
 * Disabled support for XML Schema if not compiled into libxml2 library.
 *
 * Revision 1.1  2003/08/07 12:06:59  joergr
 * Added new command line tool xml2dsr (convert XML document to DICOM SR file).
 *
 *
 */
