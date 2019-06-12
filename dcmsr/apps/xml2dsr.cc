/*
 *
 *  Copyright (C) 2003-2019, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    convert the contents of an XML document to a DICOM structured reporting file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoc.h"       /* for main interface class DSRDocument */

#include "dcmtk/dcmdata/dctk.h"       /* for typical set of "dcmdata" headers */
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "xml2dsr"
#define OFFIS_CONSOLE_DESCRIPTION "Convert XML document to DICOM SR file"

static OFLogger xml2dsrLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


#ifdef WITH_LIBXML

#ifdef __ibmxl__
// IBM xlC defines __GNUC__ but does not support the GNUC extension
// __attribute__ ((format (printf, 2, 3))).
// This avoids a compiler warning in <libxml/parser.h>.
#define LIBXML_ATTR_FORMAT(fmt,args)
#endif

#include <libxml/parser.h>


#define SHORTCOL 3
#define LONGCOL 21


int main(int argc, char *argv[])
{
    OFBool opt_generateUIDs = OFFalse;
    OFBool opt_overwriteUIDs = OFFalse;
    size_t opt_readFlags = 0;
    E_TransferSyntax opt_xfer = EXS_LittleEndianExplicit;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_recalcGL;
    E_PaddingEncoding opt_padenc = EPD_withoutPadding;
    E_FileWriteMode opt_writeMode = EWM_fileformat;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;

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
      OFLog::addOptions(cmd);

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
    if (app.parseCommandLine(cmd, argc, argv))
    {
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
#if defined(LIBXML_ICONV_ENABLED) && defined(LIBXML_ZLIB_ENABLED)
                COUT << "  with built-in LIBICONV and ZLIB support" << OFendl;
#elif defined(LIBXML_ICONV_ENABLED)
                COUT << "  with built-in LIBICONV support" << OFendl;
#elif defined(LIBXML_ZLIB_ENABLED)
                COUT << "  with built-in ZLIB support" << OFendl;
#endif
                return 0;
            }
        }

        /* general options */
        OFLog::configureFromCommandLine(cmd, app);

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
        {
            opt_generateUIDs = OFTrue;
            opt_readFlags |= DSRTypes::XF_acceptEmptyStudySeriesInstanceUID;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--dont-overwrite-uids"))
        {
            app.checkDependence("--dont-overwrite-uids", "--generate-new-uids", opt_generateUIDs);
            opt_overwriteUIDs = OFFalse;
        }
        if (cmd.findOption("--overwrite-uids"))
        {
            app.checkDependence("--overwrite-uids", "--generate-new-uids", opt_generateUIDs);
            opt_overwriteUIDs = OFTrue;
        }
        cmd.endOptionBlock();

        /* output options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--write-file"))
            opt_writeMode = EWM_fileformat;
        if (cmd.findOption("--write-dataset"))
            opt_writeMode = EWM_dataset;
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
            dcmEnableGenerationOfNewVRs();
        if (cmd.findOption("--disable-new-vr"))
            dcmDisableGenerationOfNewVRs();
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
            app.checkConflict("--padding-retain", "--write-dataset", opt_writeMode == EWM_dataset);
            opt_padenc = EPD_noChange;
        }
        if (cmd.findOption("--padding-off"))
            opt_padenc = EPD_withoutPadding;
        if (cmd.findOption("--padding-create"))
        {
            app.checkConflict("--padding-create", "--write-dataset", opt_writeMode == EWM_dataset);
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

    /* print resource identifier */
    OFLOG_DEBUG(xml2dsrLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(xml2dsrLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
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
        OFLOG_FATAL(xml2dsrLogger, OFFIS_CONSOLE_APPLICATION << ": invalid input filename: <empty string>");
        result = EC_IllegalParameter;
    }
    if ((opt_ofname == NULL) || (strlen(opt_ofname) == 0))
    {
        OFLOG_FATAL(xml2dsrLogger, OFFIS_CONSOLE_APPLICATION << ": invalid output filename: <empty string>");
        result = EC_IllegalParameter;
    }

    if (result.good())
    {
        /* create new SR document */
        DSRDocument *dsrdoc = new DSRDocument();
        if (dsrdoc != NULL)
        {
            DcmFileFormat fileformat;
#ifdef LIBXML_SCHEMAS_ENABLED
            if (opt_readFlags & DSRTypes::XF_validateSchema)
                OFLOG_INFO(xml2dsrLogger, "reading and validating XML input file: " << opt_ifname);
            else
#endif
                OFLOG_INFO(xml2dsrLogger, "reading XML input file: " << opt_ifname);
            /* read XML file and feed data into DICOM fileformat */
            result = dsrdoc->readXML(opt_ifname, opt_readFlags);
            if (result.good())
            {
                DcmDataset *dataset = fileformat.getDataset();
                OFLOG_INFO(xml2dsrLogger, "writing DICOM SR output file: " << opt_ofname);
                /* write SR document to dataset */
                result = dsrdoc->write(*dataset);
                /* generate new UIDs (if required) */
                if (opt_generateUIDs)
                {
                    char uid[100];
                    if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_StudyInstanceUID))
                    {
                        OFLOG_INFO(xml2dsrLogger, "generating new Study Instance UID");
                        dataset->putAndInsertString(DCM_StudyInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
                    }
                    if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SeriesInstanceUID))
                    {
                        OFLOG_INFO(xml2dsrLogger, "generating new Series Instance UID");
                        dataset->putAndInsertString(DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
                    }
                    if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SOPInstanceUID))
                    {
                        OFLOG_INFO(xml2dsrLogger, "generating new SOP Instance UID");
                        dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
                    }
                }
                /* write DICOM file */
                if (result.good())
                {
                    result = fileformat.saveFile(opt_ofname, opt_xfer, opt_enctype, opt_glenc, opt_padenc,
                        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);
                }
                if (result.bad())
                    OFLOG_FATAL(xml2dsrLogger, result.text() << ": writing file: "  << opt_ofname);
            } else
                OFLOG_FATAL(xml2dsrLogger, result.text() << ": reading file: "  << opt_ifname);
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
