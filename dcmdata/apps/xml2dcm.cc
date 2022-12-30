/*
 *
 *  Copyright (C) 2003-2022, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Convert XML document to DICOM file or data set
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"   /* for class DcmPixelItem */
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmdata/dcmxml/xml2dcm.h"

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "xml2dcm"
#define OFFIS_CONSOLE_DESCRIPTION "Convert XML document to DICOM file or data set"

// currently not used since DTD is always retrieved from XML document
//#define DOCUMENT_TYPE_DEFINITION_FILE "dcm2xml.dtd"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

#ifdef WITH_LIBXML

static OFLogger xml2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{
    OFBool opt_metaInfo = OFTrue;
    OFBool opt_namespace = OFFalse;
    OFBool opt_validate = OFFalse;
    OFBool opt_generateUIDs = OFFalse;
    OFBool opt_overwriteUIDs = OFFalse;
    OFBool opt_stopOnErrors = OFTrue;
    E_TransferSyntax opt_xfer = EXS_Unknown;
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

    cmd.addParam("xmlfile-in",  "XML input filename to be converted\n(\"-\" for stdin)", OFCmdParam::PM_Mandatory);
    cmd.addParam("dcmfile-out", "DICOM output filename\n(\"-\" for stdout)", OFCmdParam::PM_Mandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-meta-info",      "+f",     "read meta information if present (default)");
        cmd.addOption("--ignore-meta-info",    "-f",     "ignore file meta information");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("validation:");
        cmd.addOption("--validate-document",   "+Vd",    "validate XML document against DTD");
        cmd.addOption("--check-namespace",     "+Vn",    "check XML namespace in document root");
      cmd.addSubGroup("unique identifiers:");
        cmd.addOption("--generate-new-uids",   "+Ug",    "generate new Study/Series/SOP Instance UID");
        cmd.addOption("--dont-overwrite-uids", "-Uo",    "do not overwrite existing UIDs (default)");
        cmd.addOption("--overwrite-uids",      "+Uo",    "overwrite existing UIDs");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output file format:");
        cmd.addOption("--write-file",          "+F",     "write file format (default)");
        cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
        cmd.addOption("--update-meta-info",    "+Fu",    "update particular file meta information");
      cmd.addSubGroup("output transfer syntax:");
        cmd.addOption("--write-xfer-same",     "+t=",    "write with same TS as input (default)");
        cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian TS");
        cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
        cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
        cmd.addOption("--write-xfer-deflated", "+td",    "write with deflated expl. VR little endian TS");
#endif
      cmd.addSubGroup("error handling:");
        cmd.addOption("--stop-on-error",       "-E",     "do not write if document is invalid (default)");
        cmd.addOption("--ignore-errors",       "+E",     "attempt to write even if document is invalid");
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

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-meta-info"))
            opt_metaInfo = OFTrue;
        if (cmd.findOption("--ignore-meta-info"))
            opt_metaInfo = OFFalse;
        cmd.endOptionBlock();

        /* processing options */

        if (cmd.findOption("--validate-document"))
            opt_validate = OFTrue;
        if (cmd.findOption("--check-namespace"))
            opt_namespace = OFTrue;

        if (cmd.findOption("--generate-new-uids"))
            opt_generateUIDs = OFTrue;

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

        if (cmd.findOption("--update-meta-info"))
        {
            app.checkConflict("--update-meta-info", "--write-dataset", opt_writeMode == EWM_dataset);
            opt_writeMode = EWM_updateMeta;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--write-xfer-same"))
            opt_xfer = EXS_Unknown;
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
        if (cmd.findOption("--stop-on-error")) opt_stopOnErrors = OFTrue;
        if (cmd.findOption("--ignore-errors")) opt_stopOnErrors = OFFalse;
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
    }

    /* print resource identifier */
    OFLOG_DEBUG(xml2dcmLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(xml2dcmLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // initialize XML parser
    DcmXMLParseHelper::initLibrary();

    OFCondition result = EC_Normal;
    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
    cmd.getParam(1, opt_ifname);
    cmd.getParam(2, opt_ofname);

    /* check filenames */
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_ERROR(xml2dcmLogger, OFFIS_CONSOLE_APPLICATION << ": invalid input filename: <empty string>");
        result = EC_IllegalParameter;
    }
    if ((opt_ofname == NULL) || (strlen(opt_ofname) == 0))
    {
        OFLOG_ERROR(xml2dcmLogger, OFFIS_CONSOLE_APPLICATION << ": invalid output filename: <empty string>");
        result = EC_IllegalParameter;
    }

    if (result.good())
    {
        DcmFileFormat fileformat;
        E_TransferSyntax xfer;
        DcmXMLParseHelper parser;
        OFLOG_INFO(xml2dcmLogger, "reading XML input file: " << opt_ifname);
        /* read XML file and feed data into DICOM fileformat */
        result = parser.readXmlFile(opt_ifname, fileformat, xfer, opt_metaInfo, opt_namespace, opt_validate, opt_stopOnErrors);
        if (result.good())
        {
            DcmDataset *dataset = fileformat.getDataset();
            /* generate new UIDs (if required) */
            if (opt_generateUIDs)
            {
                char uid[100];
                if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_StudyInstanceUID))
                {
                    OFLOG_INFO(xml2dcmLogger, "generating new Study Instance UID");
                    dataset->putAndInsertString(DCM_StudyInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
                }
                if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SeriesInstanceUID))
                {
                    OFLOG_INFO(xml2dcmLogger, "generating new Series Instance UID");
                    dataset->putAndInsertString(DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
                }
                if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SOPInstanceUID))
                {
                    OFLOG_INFO(xml2dcmLogger, "generating new SOP Instance UID");
                    dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
                }
            }
            OFLOG_INFO(xml2dcmLogger, "writing DICOM output file: " << opt_ofname);
            /* determine transfer syntax to write the file */
            if ((opt_xfer == EXS_Unknown) && (xfer != EXS_Unknown))
                opt_xfer = xfer;
            /* check whether this is possible */
            if (fileformat.canWriteXfer(opt_xfer))
            {
                /* check whether pixel data is compressed */
                if ((opt_writeMode == EWM_dataset) && DcmXfer(xfer).isEncapsulated())
                {
                    OFLOG_WARN(xml2dcmLogger, "encapsulated pixel data require file format, ignoring --write-dataset");
                    opt_writeMode = EWM_fileformat;
                }
                /* write DICOM file */
                result = fileformat.saveFile(opt_ofname, opt_xfer, opt_enctype, opt_glenc, opt_padenc,
                    OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);
                if (result.bad())
                    OFLOG_ERROR(xml2dcmLogger, result.text() << ": writing file: "  << opt_ofname);
            } else {
                OFLOG_ERROR(xml2dcmLogger, "no conversion to transfer syntax " << DcmXfer(opt_xfer).getXferName() << " possible!");
                result = EC_CannotChangeRepresentation;
            }
        }
    }

    /* clean up XML library before quitting */
    DcmXMLParseHelper::cleanupLibrary();

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
