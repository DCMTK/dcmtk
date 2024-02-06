/*
 *
 *  Copyright (C) 2007-2023, OFFIS e.V.
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
 *  Purpose: Extract PDF file from DICOM encapsulated PDF storage object
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* for O_RDONLY */
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* required for sys/stat.h */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat, fstat */
#endif
END_EXTERN_C

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcm2pdf"

static OFLogger dcm2pdfLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
    OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define FILENAME_PLACEHOLDER "#f"

static OFString replaceChars(const OFString &srcstr, const OFString &pattern, const OFString &substitute)
/*
 * This function replaces all occurrences of pattern in srcstr with substitute and returns
 * the result as a new OFString variable. Note that srcstr itself will not be changed.
 *
 * Parameters:
 *   srcstr     - [in] The source string.
 *   pattern    - [in] The pattern string which shall be substituted.
 *   substitute - [in] The substitute for pattern in srcstr.
 */
{
    OFString result = srcstr;
    size_t pos = 0;

    while (pos != OFString_npos)
    {
        pos = result.find(pattern, pos);

        if (pos != OFString_npos)
        {
            result.replace(pos, pattern.size(), substitute);
            pos += substitute.size();
        }
    }

    return result;
}


#define SHORTCOL 3
#define LONGCOL 20

int main(int argc, char *argv[])
{
    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
    const char    *opt_execString = NULL;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Extract PDF file from DICOM encapsulated PDF", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",  "DICOM input filename (\"-\" for stdin)");
    cmd.addParam("pdffile-out", "PDF output filename");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
        cmd.addOption("--help",                 "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
        cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
        OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
        cmd.addSubGroup("input file format:");
            cmd.addOption("--read-file",          "+f",     "read file format or data set (default)");
            cmd.addOption("--read-file-only",     "+fo",    "read file format only");
            cmd.addOption("--read-dataset",       "-f",     "read data set without file meta information");
        cmd.addSubGroup("input transfer syntax:");
            cmd.addOption("--read-xfer-auto",     "-t=",    "use TS recognition (default)");
            cmd.addOption("--read-xfer-detect",   "-td",    "ignore TS specified in the file meta header");
            cmd.addOption("--read-xfer-little",   "-te",    "read with explicit VR little endian TS");
            cmd.addOption("--read-xfer-big",      "-tb",    "read with explicit VR big endian TS");
            cmd.addOption("--read-xfer-implicit", "-ti",    "read with implicit VR little endian TS");
        cmd.addSubGroup("parsing of odd-length attributes:");
            cmd.addOption("--accept-odd-length",  "+ao",    "accept odd length attributes (default)");
            cmd.addOption("--assume-even-length", "+ae",    "assume real length is one byte larger");
        cmd.addSubGroup("handling of undefined length UN elements:");
            cmd.addOption("--enable-cp246",       "+ui",    "read undefined len UN as implicit VR (default)");
            cmd.addOption("--disable-cp246",      "-ui",    "read undefined len UN as explicit VR");
        cmd.addSubGroup("handling of defined length UN elements:");
            cmd.addOption("--retain-un",          "-uc",    "retain elements as UN (default)");
            cmd.addOption("--convert-un",         "+uc",    "convert to real VR if known");
        cmd.addSubGroup("automatic data correction:");
            cmd.addOption("--enable-correction",  "+dc",    "enable automatic data correction (default)");
            cmd.addOption("--disable-correction", "-dc",    "disable automatic data correction");
#ifdef WITH_ZLIB
        cmd.addSubGroup("bitstream format of deflated input:");
            cmd.addOption("--bitstream-deflated", "+bd",    "expect deflated bitstream (default)");
            cmd.addOption("--bitstream-zlib",     "+bz",    "expect deflated zlib bitstream");
#endif

    cmd.addGroup("processing options:");
    cmd.addSubGroup("execution options:");
        cmd.addOption("--exec",                "-x",  1, "[c]ommand: string",
                                                         "execute command c after PDF extraction");
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
                return EXITCODE_NO_ERROR;
            }
        }

        /* command line parameters and options */
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

        cmd.beginOptionBlock();
        if (cmd.findOption("--accept-odd-length"))
        {
            dcmAcceptOddAttributeLength.set(OFTrue);
        }
        if (cmd.findOption("--assume-even-length"))
        {
            dcmAcceptOddAttributeLength.set(OFFalse);
        }
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--enable-cp246"))
        {
            dcmEnableCP246Support.set(OFTrue);
        }
        if (cmd.findOption("--disable-cp246"))
        {
            dcmEnableCP246Support.set(OFFalse);
        }
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--retain-un"))
        {
            dcmEnableUnknownVRConversion.set(OFFalse);
        }
        if (cmd.findOption("--convert-un"))
        {
            dcmEnableUnknownVRConversion.set(OFTrue);
        }
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--enable-correction"))
        {
            dcmEnableAutomaticInputDataCorrection.set(OFTrue);
        }
        if (cmd.findOption("--disable-correction"))
        {
            dcmEnableAutomaticInputDataCorrection.set(OFFalse);
        }
        cmd.endOptionBlock();

#ifdef WITH_ZLIB
        cmd.beginOptionBlock();
        if (cmd.findOption("--bitstream-deflated"))
        {
            dcmZlibExpectRFC1950Encoding.set(OFFalse);
        }
        if (cmd.findOption("--bitstream-zlib"))
        {
            dcmZlibExpectRFC1950Encoding.set(OFTrue);
        }
        cmd.endOptionBlock();
#endif

        if (cmd.findOption("--exec"))
            app.checkValue(cmd.getValue(opt_execString));
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcm2pdfLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcm2pdfLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dcm2pdfLogger, "invalid filename: <empty string>");
        return EXITCODE_NO_INPUT_FILES;
    }

    DcmFileFormat fileformat;
    DcmDataset * dataset = fileformat.getDataset();

    OFLOG_INFO(dcm2pdfLogger, "open input file " << opt_ifname);

    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

    if (error.bad())
    {
        OFLOG_FATAL(dcm2pdfLogger, error.text() << ": reading file: " << opt_ifname);
        return EXITCODE_CANNOT_READ_INPUT_FILE;
    }

    OFString sopClass;
    error = dataset->findAndGetOFString(DCM_SOPClassUID, sopClass);
    if (error.bad() || sopClass != UID_EncapsulatedPDFStorage)
    {
        OFLOG_FATAL(dcm2pdfLogger, "not an Encapsulated PDF object: " << opt_ifname);
        return EXITCODE_INVALID_INPUT_FILE;
    }

    DcmElement *delem = NULL;
    error = dataset->findAndGetElement(DCM_EncapsulatedDocument, delem);
    if (error.bad() || delem == NULL)
    {
        OFLOG_FATAL(dcm2pdfLogger, "Encapsulated Document missing.");
        return EXITCODE_INVALID_INPUT_FILE;
    }

    Uint32 len = delem->getLength();
    Uint8 *pdfDocument = NULL;
    error = delem->getUint8Array(pdfDocument);
    if (error.bad() || pdfDocument == NULL || len == 0)
    {
        OFLOG_FATAL(dcm2pdfLogger, "Encapsulated Document empty or wrong VR.");
        return EXITCODE_INVALID_INPUT_FILE;
    }

    /* strip pad byte at end of file, if there is one. The PDF format expects
     * files to end with %%EOF followed by CR/LF (although in some cases the
     * CR/LF may be missing or you might only find CR or LF).
     * If the last character of the file is not a CR or LF, and not the
     * letter 'F', we assume it is either trailing garbage or a pad byte, and remove it.
     */
    if (pdfDocument[len-1] != 10 && pdfDocument[len-1] != 13 && pdfDocument[len-1] != 'F')
    {
        --len;
    }

    OFLOG_INFO(dcm2pdfLogger, "writing PDF file to " << opt_ofname);
    FILE *pdffile = fopen(opt_ofname, "wb");
    if (pdffile == NULL)
    {
        OFLOG_FATAL(dcm2pdfLogger, "unable to create file " << opt_ofname);
        return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
    }

    if (len != fwrite(pdfDocument, 1, len, pdffile))
    {
        OFLOG_FATAL(dcm2pdfLogger, "write error in file " << opt_ofname);
        fclose(pdffile);
        return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
    }

    fclose(pdffile);

    OFLOG_INFO(dcm2pdfLogger, "conversion successful");

    if (opt_execString)
    {
        OFString cmdStr = opt_execString;
        cmdStr = replaceChars(cmdStr, OFString(FILENAME_PLACEHOLDER), opt_ofname);

        // Execute command and return result
        return system(cmdStr.c_str());
    }

    return EXITCODE_NO_ERROR;
}
