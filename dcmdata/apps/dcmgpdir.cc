/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Author:  Andrew Hewett, Joerg Riesmeier
 *
 *  Purpose:
 *  Make a DICOMDIR according to the DICOM Part 11 Media Storage Application
 *  Profiles. Supports the following profiles:
 *  - General Purpose CD-R Interchange (STD-GEN-CD)
 *  - General Purpose Interchange on DVD-RAM Media (STD-GEN-DVD-RAM)
 *  If build with 'BUILD_DCMGPDIR_AS_DCMMKDIR' it also supports:
 *  - General Purpose DVD with Compression Interchange (STD-GEN-DVD-JPEG/J2K)
 *  - General Purpose USB and Flash Memory with Compression Interchange (STD-GEN-USB/MMC/CF/SD-JPEG/J2K)
 *  - General Purpose MIME Interchange (STD-GEN-MIME)
 *  - DVD Interchange with MPEG2 MP@ML (STD-DVD-MPEG2-MPML)
 *  - Basic Cardiac X-Ray Angiographic Studies on CD-R Media (STD-XABC-CD)
 *  - 1024 X-Ray Angiographic Studies on CD-R Media (STD-XA1K-CD)
 *  - 1024 X-Ray Angiographic Studies on DVD Media (STD-XA1K-DVD)
 *  - Dental Radiograph Interchange (STD-DEN-CD)
 *  - CT/MR Studies on various Media (STD-CTMR-xxxx)
 *  - Ultrasound Single Frame for Image Display (STD-US-ID-SF-xxxx)
 *  - Ultrasound Single Frame with Spatial Calibration (STD-US-SC-SF-xxxx)
 *  - Ultrasound Single Frame with Combined Calibration (STD-US-CC-SF-xxxx)
 *  - Ultrasound Single & Multi-Frame for Image Display (STD-US-ID-MF-xxxx)
 *  - Ultrasound Single & Multi-Frame with Spatial Calibration (STD-US-SC-MF-xxxx)
 *  - Ultrasound Single & Multi-Frame with Combined Calibration (STD-US-CC-MF-xxxx)
 *  - 12-lead ECG Interchange on Diskette (STD-WVFM-ECG-FD)
 *  - Hemodynamic Waveform Interchange on Diskette (STD-WVFM-HD-FD)
 *  There should be no need to set this compiler flag manually, just compile
 *  dcmjpeg/apps/dcmmkdir.cc.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.95 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcddirif.h"    /* for class DicomDirInterface */
#include "dcmtk/ofstd/ofstd.h"         /* for class OFStandard */
#include "dcmtk/ofstd/ofconapp.h"      /* for class OFConsoleApplication */
#include "dcmtk/ofstd/ofcond.h"        /* for class OFCondition */

#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
#include "dcmtk/dcmimage/diregist.h"   /* include to support color images */
#include "dcmtk/dcmdata/dcrledrg.h"    /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpeg/djdecode.h"    /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"    /* for dcmimage JPEG plugin */
#include "dcmtk/dcmjpeg/ddpiimpl.h"    /* for class DicomDirImageImplementation */
#endif

#ifdef WITH_ZLIB
#include <zlib.h>         /* for zlibVersion() */
#endif

#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
#define OFFIS_CONSOLE_APPLICATION "dcmmkdir"
#define OFFIS_CONSOLE_DESCRIPTION "Create a DICOMDIR file"
#else
#define OFFIS_CONSOLE_APPLICATION "dcmgpdir"
#define OFFIS_CONSOLE_DESCRIPTION "Create a general purpose DICOMDIR"
#endif

#if defined (HAVE_WINDOWS_H) || defined(HAVE_FNMATCH_H)
#define PATTERN_MATCHING_AVAILABLE
#endif


static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

static OFLogger dcmgpdirLogger = OFLog::getLogger("dcmtk.dcmdata." OFFIS_CONSOLE_APPLICATION);

#define SHORTCOL 4
#define LONGCOL 23

// ********************************************

int main(int argc, char *argv[])
{
    OFBool opt_write = OFTrue;
    OFBool opt_append = OFFalse;
    OFBool opt_update = OFFalse;
    OFBool opt_recurse = OFFalse;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_withoutGL;
    const char *opt_output = DEFAULT_DICOMDIR_NAME;
    const char *opt_fileset = DEFAULT_FILESETID;
    const char *opt_descriptor = NULL;
    const char *opt_charset = DEFAULT_DESCRIPTOR_CHARSET;
    const char *opt_directory = "";
    const char *opt_pattern = "";
    DicomDirInterface::E_ApplicationProfile opt_profile = DicomDirInterface::AP_GeneralPurpose;

#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
    // register global decompression codecs (no verbose/debug mode set)
    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();
    // enable the following lines for JPEG 2000 decompression support
/*
    D2JPEG2000Library::initialize();
    D2DecoderRegistration::registerCodecs();
*/
#endif

    /* DICOMDIR interface (checks for JPEG/RLE availability) */
    DicomDirInterface ddir;

    /* set-up command line parameters and options */
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in", "referenced DICOM file", OFCmdParam::PM_MultiOptional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                     "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                            "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("DICOMDIR identifiers:");
        cmd.addOption("--output-file",           "+D",  1, "[f]ilename: string",
                                                           "generate specific DICOMDIR file\n(default: " DEFAULT_DICOMDIR_NAME " in current directory)");
        cmd.addOption("--fileset-id",            "+F",  1, "[i]d: string (default: " DEFAULT_FILESETID ")",
                                                           "use specific file set ID");
        cmd.addOption("--descriptor",            "+R",  1, "[f]ilename: string",
                                                           "add a file set descriptor file ID\n(e.g. README, default: no descriptor)");
        cmd.addOption("--char-set",              "+C",  1, "[c]harset: string",
                                                           "add a specific character set for descriptor\n(default: \"" DEFAULT_DESCRIPTOR_CHARSET "\" if descriptor present)");
      cmd.addSubGroup("reading:");
        cmd.addOption("--input-directory",       "+id", 1, "[d]irectory: string",
                                                           "read referenced DICOM files from directory d\n(default for --recurse: current directory)");
        cmd.addOption("--keep-filenames",        "-m",     "expect filenames to be in DICOM format (def.)");
        cmd.addOption("--map-filenames",         "+m",     "map to DICOM filenames (lowercase->uppercase,\nand remove trailing period)");
        cmd.addOption("--no-recurse",            "-r",     "do not recurse within directories (default)");
        cmd.addOption("--recurse",               "+r",     "recurse within filesystem directories");
#ifdef PATTERN_MATCHING_AVAILABLE
        cmd.addOption("--pattern",               "+p",  1, "[p]attern: string (only with --recurse)",
                                                           "pattern for filename matching (wildcards)");
#endif
    cmd.addGroup("processing options:");
      cmd.addSubGroup("consistency check:");
        cmd.addOption("--no-consistency-check",  "-W",     "do not check files for consistency");
        cmd.addOption("--warn-inconsist-files",  "+W",     "warn about inconsistent files (default)");
        cmd.addOption("--abort-inconsist-file",  "-a",     "abort on first inconsistent file");
      cmd.addSubGroup("type 1 attributes:");
        cmd.addOption("--strict",                "-I",     "exit with error if DICOMDIR type 1 attributes\nare missing in DICOM file (default)");
        cmd.addOption("--invent",                "+I",     "invent DICOMDIR type 1 attributes\nif missing in DICOM file");
        cmd.addOption("--invent-patient-id",     "+Ipi",   "invent new PatientID in case of inconsistent\nPatientName attributes");
      cmd.addSubGroup("other checks:");
        cmd.addOption("--allow-retired-sop",     "+Nrs",   "allow retired SOP classes defined in previous\neditions of the DICOM standard");
        cmd.addOption("--no-xfer-check",         "-Nxc",   "do not reject images with non-standard\ntransfer syntax (just warn)");
#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
        cmd.addOption("--no-encoding-check",     "-Nec",   "do not reject images with non-standard\npixel encoding (just warn)");
        cmd.addOption("--no-resolution-check",   "-Nrc",   "do not reject images with non-standard\nspatial resolution (just warn)");
      cmd.addSubGroup("icon images:");
        cmd.addOption("--add-icon-image",        "+X",     "add monochrome icon image on IMAGE level\n(default for cardiac profiles)");
        cmd.addOption("--icon-image-size",       "-Xs", 1, "[s]ize: integer (1..128)",
                                                           "width and height of the icon image (in pixel)\n(fixed: 128 for XA, 64 for CT/MR profile)");
        cmd.addOption("--icon-file-prefix",      "-Xi", 1, "[p]refix: string",
                                                           "use PGM image 'prefix'+'dcmfile-in' as icon\n(default: create icon from DICOM image)");
        cmd.addOption("--default-icon",          "-Xd", 1, "[f]ilename: string",
                                                           "use specified PGM image if icon cannot be\ncreated automatically (default: black image)");
#endif
    cmd.addGroup("output options:");
#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
      cmd.addSubGroup("profiles:");
        cmd.addOption("--general-purpose",       "-Pgp",   "General Purpose Interchange on CD-R or\nDVD-RAM Media (STD-GEN-CD/DVD-RAM, default)");
        cmd.addOption("--general-purpose-dvd",   "-Pdv",   "General Purpose DVD with Compression\nInterchange (STD-GEN-DVD-JPEG/J2K)");
        cmd.addOption("--general-purpose-mime",  "-Pmi",   "General Purpose MIME Interchange\n(STD-GEN-MIME)");
        cmd.addOption("--usb-and-flash",         "-Pfl",   "General Purpose USB/Flash Memory with Compr.\nInterchange (STD-GEN-USB/MMC/CF/SD-JPEG/J2K)");
        cmd.addOption("--mpeg2-mpml-dvd",        "-Pmp",   "DVD Interchange with MPEG2 Main Profile @\nMain Level (STD-DVD-MPEG2-MPML)");
        cmd.addOption("--basic-cardiac",         "-Pbc",   "Basic Cardiac X-Ray Angiographic Studies on\nCD-R Media (STD-XABC-CD)");
        cmd.addOption("--xray-angiographic",     "-Pxa",   "1024 X-Ray Angiographic Studies on CD-R Media\n(STD-XA1K-CD)");
        cmd.addOption("--xray-angiographic-dvd", "-Pxd",   "1024 X-Ray Angiographic Studies on DVD Media\n(STD-XA1K-DVD)");
        cmd.addOption("--dental-radiograph",     "-Pde",   "Dental Radiograph Interchange (STD-DEN-CD)");
        cmd.addOption("--ct-and-mr",             "-Pcm",   "CT/MR Studies (STD-CTMR-xxxx)");
        cmd.addOption("--ultrasound-id-sf",      "-Pus",   "Ultrasound Single Frame for Image Display\n(STD-US-ID-SF-xxxx)");
        cmd.addOption("--ultrasound-sc-sf",                "Ultrasound Single Frame with Spatial\nCalibration (STD-US-SC-SF-xxxx)");
        cmd.addOption("--ultrasound-cc-sf",                "Ultrasound Single Frame with Combined\nCalibration (STD-US-CC-SF-xxxx)");
        cmd.addOption("--ultrasound-id-mf",      "-Pum",   "Ultrasound Single & Multi-Frame for Image\nDisplay (STD-US-ID-MF-xxxx)");
        cmd.addOption("--ultrasound-sc-mf",                "Ultrasound Single & Multi-Frame with Spatial\nCalibration (STD-UD-SC-MF-xxxx)");
        cmd.addOption("--ultrasound-cc-mf",                "Ultrasound Single & Multi-Frame with Combined\nCalibration (STD-UD-CC-MF-xxxx)");
        cmd.addOption("--12-lead-ecg",           "-Pec",   "12-lead ECG Interchange on Diskette\n(STD-WVFM-ECG-FD)");
        cmd.addOption("--hemodynamic-waveform",  "-Phd",   "Hemodynamic Waveform Interchange on Diskette\n(STD-WVFM-HD-FD)");
#endif
      cmd.addSubGroup("writing:");
        cmd.addOption("--replace",               "-A",     "replace existing DICOMDIR (default)");
        cmd.addOption("--append",                "+A",     "append to existing DICOMDIR");
        cmd.addOption("--update",                "+U",     "update existing DICOMDIR");
        cmd.addOption("--discard",               "-w",     "do not write out DICOMDIR");
        cmd.addOption("--no-backup",             "-nb",    "do not create a backup of existing DICOMDIR");
      cmd.addSubGroup("post-1993 value representations:");
        cmd.addOption("--enable-new-vr",         "+u",     "enable support for new VRs (UN/UT) (default)");
        cmd.addOption("--disable-new-vr",        "-u",     "disable support for new VRs, convert to OB");
      cmd.addSubGroup("group length encoding:");
        cmd.addOption("--group-length-remove",   "-g",     "write without group length elements (default)");
        cmd.addOption("--group-length-create",   "+g",     "write with group length elements");
      cmd.addSubGroup("length encoding in sequences and items:");
        cmd.addOption("--length-explicit",       "+e",     "write with explicit lengths (default)");
        cmd.addOption("--length-undefined",      "-e",     "write with undefined lengths");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
        /* print help text and exit */
        if (cmd.getArgCount() == 0)
            app.printUsage();

        /* check exclusive options first */
        if (cmd.hasExclusiveOption())
        {
            if (cmd.findOption("--version"))
            {
                app.printHeader(OFTrue /*print host identifier*/);
                COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(BUILD_DCMGPDIR_AS_DCMMKDIR)
                COUT << " none" << OFendl;
#else
                COUT << OFendl;
#endif
#ifdef WITH_ZLIB
                COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
                COUT << "- " << DiJPEGPlugin::getLibraryVersionString() << OFendl;
#endif
                return 0;
            }
        }

        /* general options */
        OFLog::configureFromCommandLine(cmd, app);

        /* input options */
        if (cmd.findOption("--output-file"))
            app.checkValue(cmd.getValue(opt_output));
        if (cmd.findOption("--fileset-id"))
            app.checkValue(cmd.getValue(opt_fileset));
        if (cmd.findOption("--descriptor"))
            app.checkValue(cmd.getValue(opt_descriptor));
        if (cmd.findOption("--char-set"))
            app.checkValue(cmd.getValue(opt_charset));

        if (cmd.findOption("--input-directory"))
            app.checkValue(cmd.getValue(opt_directory));

        cmd.beginOptionBlock();
        if (cmd.findOption("--keep-filenames"))
            ddir.enableMapFilenamesMode(OFFalse);
        if (cmd.findOption("--map-filenames"))
            ddir.enableMapFilenamesMode(OFTrue);
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--no-recurse"))
            opt_recurse = OFFalse;
        if (cmd.findOption("--recurse"))
        {
            opt_recurse = OFTrue;
            if ((opt_directory == NULL) || (strlen(opt_directory) == 0))
                opt_directory = ".";
        }
        cmd.endOptionBlock();

#ifdef PATTERN_MATCHING_AVAILABLE
        if (cmd.findOption("--pattern"))
        {
            app.checkDependence("--pattern", "--recurse", opt_recurse);
            app.checkValue(cmd.getValue(opt_pattern));
        }
#endif

        /* processing options */
        cmd.beginOptionBlock();
        if (cmd.findOption("--no-consistency-check"))
            ddir.disableConsistencyCheck();
        if (cmd.findOption("--warn-inconsist-files"))
            ddir.enableAbortMode(OFFalse);
        if (cmd.findOption("--abort-inconsist-file"))
            ddir.enableAbortMode(OFTrue);
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--strict"))
            ddir.enableInventMode(OFFalse);
        if (cmd.findOption("--invent"))
            ddir.enableInventMode(OFTrue);
        cmd.endOptionBlock();

        if (cmd.findOption("--invent-patient-id"))
            ddir.enableInventPatientIDMode();

        if (cmd.findOption("--allow-retired-sop"))
            ddir.enableRetiredSOPClassSupport();
        if (cmd.findOption("--no-xfer-check"))
            ddir.disableTransferSyntaxCheck();

#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
        if (cmd.findOption("--no-encoding-check"))
            ddir.disableEncodingCheck();
        if (cmd.findOption("--no-resolution-check"))
            ddir.disableResolutionCheck();
        if (cmd.findOption("--add-icon-image"))
            ddir.enableIconImageMode();
        if (cmd.findOption("--icon-image-size"))
        {
            OFCmdUnsignedInt iconSize = 0;
            app.checkValue(cmd.getValueAndCheckMinMax(iconSize, 1, 128));
            ddir.setIconSize(OFstatic_cast(unsigned int, iconSize));
        }
        if (cmd.findOption("--icon-file-prefix"))
        {
            const char *iconPrefix = NULL;
            app.checkValue(cmd.getValue(iconPrefix));
            ddir.setIconPrefix(iconPrefix);
        }
        if (cmd.findOption("--default-icon"))
        {
            const char *defaultIcon = NULL;
            app.checkValue(cmd.getValue(defaultIcon));
            ddir.setDefaultIcon(defaultIcon);
        }
#endif

        /* output options */
#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
        cmd.beginOptionBlock();
        if (cmd.findOption("--general-purpose"))
            opt_profile = DicomDirInterface::AP_GeneralPurpose;
        if (cmd.findOption("--general-purpose-dvd"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeDVD;
        if (cmd.findOption("--general-purpose-mime"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeMIME;
        if (cmd.findOption("--usb-and-flash"))
            opt_profile = DicomDirInterface::AP_USBandFlash;
        if (cmd.findOption("--mpeg2-mpml-dvd"))
            opt_profile = DicomDirInterface::AP_MPEG2MPatMLDVD;
        if (cmd.findOption("--basic-cardiac"))
            opt_profile = DicomDirInterface::AP_BasicCardiac;
        if (cmd.findOption("--xray-angiographic"))
            opt_profile = DicomDirInterface::AP_XrayAngiographic;
        if (cmd.findOption("--xray-angiographic-dvd"))
            opt_profile = DicomDirInterface::AP_XrayAngiographicDVD;
        if (cmd.findOption("--dental-radiograph"))
            opt_profile = DicomDirInterface::AP_DentalRadiograph;
        if (cmd.findOption("--ct-and-mr"))
            opt_profile = DicomDirInterface::AP_CTandMR;
        if (cmd.findOption("--ultrasound-id-sf"))
            opt_profile = DicomDirInterface::AP_UltrasoundIDSF;
        if (cmd.findOption("--ultrasound-sc-sf"))
            opt_profile = DicomDirInterface::AP_UltrasoundSCSF;
        if (cmd.findOption("--ultrasound-cc-sf"))
            opt_profile = DicomDirInterface::AP_UltrasoundCCSF;
        if (cmd.findOption("--ultrasound-id-mf"))
            opt_profile = DicomDirInterface::AP_UltrasoundIDMF;
        if (cmd.findOption("--ultrasound-sc-mf"))
            opt_profile = DicomDirInterface::AP_UltrasoundSCMF;
        if (cmd.findOption("--ultrasound-cc-mf"))
            opt_profile = DicomDirInterface::AP_UltrasoundCCMF;
        if (cmd.findOption("--12-lead-ecg"))
            opt_profile = DicomDirInterface::AP_TwelveLeadECG;
        if (cmd.findOption("--hemodynamic-waveform"))
            opt_profile = DicomDirInterface::AP_HemodynamicWaveform;
        cmd.endOptionBlock();
#endif

        cmd.beginOptionBlock();
        if (cmd.findOption("--replace"))
        {
            opt_write = OFTrue;
            opt_append = OFFalse;
            opt_update = OFFalse;
        }
        if (cmd.findOption("--append"))
        {
            opt_write = OFTrue;
            opt_append = OFTrue;
            opt_update = OFFalse;
        }
        if (cmd.findOption("--update"))
        {
            opt_write = OFTrue;
            opt_append = OFFalse;
            opt_update = OFTrue;
        }
        if (cmd.findOption("--discard"))
        {
            opt_write = OFFalse;
            opt_append = OFFalse;
            opt_update = OFFalse;
        }
        cmd.endOptionBlock();
        if (cmd.findOption("--no-backup"))
            ddir.disableBackupMode();

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

        /* post check */
        if ((opt_profile == DicomDirInterface::AP_BasicCardiac) ||
            (opt_profile == DicomDirInterface::AP_XrayAngiographic) ||
            (opt_profile == DicomDirInterface::AP_CTandMR))
        {
            app.checkConflict("--icon-image-size", "--basic-cardiac, --xray-angiographic or --ct-and-mr", cmd.findOption("--icon-image-size"));
        }
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmgpdirLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_FATAL(dcmgpdirLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
        return 1;  /* DcmDicomDir class dumps core when no data dictionary present */
    }

    /* create list of input files */
    OFList<OFString> fileNames;
    OFString pathname;
    const char *param = NULL;
    const int count = cmd.getParamCount();
    if (opt_recurse)
        OFLOG_INFO(dcmgpdirLogger, "determining input files ...");
    /* no parameters? */
    if (count == 0)
    {
        if (opt_recurse)
            OFStandard::searchDirectoryRecursively("", fileNames, opt_pattern, opt_directory);
        else {
            OFLOG_FATAL(dcmgpdirLogger, "missing parameter dcmfile-in");
            return 1;
        }
    } else {
        /* iterate over all input filenames */
        for (int i = 1; i <= count; i++)
        {
            cmd.getParam(i, param);
            /* add input directory */
            OFStandard::combineDirAndFilename(pathname, opt_directory, param, OFTrue /*allowEmptyDirName*/);
            /* search directory recursively (if required) */
            if (opt_recurse && OFStandard::dirExists(pathname))
                OFStandard::searchDirectoryRecursively(param, fileNames, opt_pattern, opt_directory);
            else
                fileNames.push_back(param);
        }
    }
    /* check whether there are any input files */
    if (fileNames.empty())
    {
        OFLOG_FATAL(dcmgpdirLogger, "no input files: DICOMDIR not created");
        return 1;
    }

#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
    // add image support to DICOMDIR class
    DicomDirImageImplementation imagePlugin;
    ddir.addImageSupport(&imagePlugin);
#endif

    OFCondition result;
    /* create new general purpose DICOMDIR, append to or update existing one */
    if (opt_append)
        result = ddir.appendToDicomDir(opt_profile, opt_output);
    else if (opt_update)
        result = ddir.updateDicomDir(opt_profile, opt_output);
    else
        result = ddir.createNewDicomDir(opt_profile, opt_output, opt_fileset);
    if (result.good())
    {
        /* set fileset descriptor and character set */
        result = ddir.setFilesetDescriptor(opt_descriptor, opt_charset);
        if (result.good())
        {
            /* collect 'bad' files */
            OFList<OFString> badFiles;
            unsigned int goodFiles = 0;
            OFListIterator(OFString) iter = fileNames.begin();
            OFListIterator(OFString) last = fileNames.end();
            /* iterate over all input filenames */
            while ((iter != last) && result.good())
            {
                /* add files to the DICOMDIR */
                result = ddir.addDicomFile((*iter).c_str(), opt_directory);
                if (result.bad())
                {
                    badFiles.push_back(*iter);
                    if (!ddir.abortMode())
                    {
                        /* ignore inconsistent file, just warn (already done inside "ddir") */
                        result = EC_Normal;
                    }
                } else
                    ++goodFiles;
                ++iter;
            }
            /* evaluate result of file checking/adding procedure */
            if (goodFiles == 0)
            {
                OFLOG_ERROR(dcmgpdirLogger, "no good files: DICOMDIR not created");
                result = EC_IllegalCall;
            }
            else if (!badFiles.empty())
            {
                OFOStringStream oss;
                oss << badFiles.size() << " file(s) cannot be added to DICOMDIR: ";
                iter = badFiles.begin();
                last = badFiles.end();
                while (iter != last)
                {
                    oss << OFendl << "  " << (*iter);
                    ++iter;
                }
                oss << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(oss, tmpString)
                OFLOG_WARN(dcmgpdirLogger, tmpString);
                OFSTRINGSTREAM_FREESTR(tmpString)
            }
            /* write DICOMDIR file */
            if (result.good() && opt_write)
                result = ddir.writeDicomDir(opt_enctype, opt_glenc);
        }
    }

#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
    // deregister global decompression codecs
    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
    // enable the following lines for JPEG 2000 decompression support
/*
    D2DecoderRegistration::cleanup();
    D2JPEG2000Library::cleanup();
*/
#endif

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return result.status();
}


/*
 * CVS/RCS Log:
 * $Log: dcmgpdir.cc,v $
 * Revision 1.95  2010-10-14 13:13:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.94  2010-08-09 13:04:19  joergr
 * Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 * the official "keyword" is used for the attribute name which results in a
 * number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 * Revision 1.93  2010-07-21 14:28:19  joergr
 * Enhanced check of input directory variable (also check for empty string).
 *
 * Revision 1.92  2010-07-21 13:29:48  joergr
 * Made sure that no NULL pointer is passed to the OFString constructor. This
 * occurred when option --recurse was used without --pattern, or option
 * --no-recurse was used without --input-directory.
 *
 * Revision 1.91  2010-03-23 15:17:24  joergr
 * Use printError() method for command line parsing errors only. After the
 * resource identifier has been printed to the log stream use "oflog" instead.
 *
 * Revision 1.90  2009-11-04 09:58:06  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.89  2009-04-21 14:02:49  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.88  2009-01-15 10:15:23  joergr
 * Added support for optional JPEG 2000 decompression (currently uncommented).
 *
 * Revision 1.87  2008-09-25 14:38:48  joergr
 * Moved output of resource identifier in order to avoid printing the same
 * information twice.
 *
 * Revision 1.86  2008-09-25 11:19:48  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.85  2007/01/10 13:05:18  joergr
 * Added new option that enables support for retired SOP classes.
 * Re-ordered and re-structured command line options.
 *
 * Revision 1.84  2006/12/15 14:26:14  joergr
 * Added new option that allows to update existing entries in a DICOMDIR. This
 * also adds support for mixed media stored application profiles.
 * Fixed wrong spelling of command line option which prevented the MPEG2-DVD
 * application profile from working.
 * Changed name of enum value for the MPEG2-DVD application profile in order to
 * be more consistent with other names.
 *
 * Revision 1.83  2006/08/15 15:50:56  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.82  2006/07/27 13:52:42  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.81  2005/12/08 15:40:48  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.80  2005/11/28 15:28:54  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.79  2005/06/13 14:36:07  joergr
 * Added new options to disable check on pixel encoding and transfer syntax.
 *
 * Revision 1.78  2005/03/09 17:56:20  joergr
 * Added support for new Media Storage Application Profiles according to DICOM
 * PS 3.12-2004. Removed support for non-standard conformant "No profile".
 *
 * Revision 1.77  2004/05/06 16:37:47  joergr
 * Added typecasts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.76  2004/01/16 10:52:58  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.75  2003/08/12 15:22:05  joergr
 * Replaced call of OFCommandLine::getValueAndCheckMin() by OFCommandLine::
 * getValueAndCheckMinMax() - warning reported by MSVC 5.
 *
 * Revision 1.74  2003/08/12 14:34:00  joergr
 * Adapted implementation to use new DICOMDIR class. Added new command line
 * options (e.g. --input-directory or --pattern).
 *
 * Revision 1.73  2003/05/20 08:50:19  joergr
 * Added support for SOP Class "Chest CAD SR" (Supplement 65).
 *
 * Revision 1.72  2003/03/12 17:32:38  meichel
 * Updated DcmObject::print() flags
 *
 * Revision 1.71  2002/11/27 12:07:17  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.70  2002/11/26 14:03:02  joergr
 * Numerous code purifications, e.g. made local functions "static".
 *
 * Revision 1.69  2002/11/26 08:43:00  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.68  2002/11/04 16:39:18  joergr
 * Added new command line option preventing the creation of a backup of an
 * existing DICOMDIR.
 *
 * Revision 1.67  2002/09/23 17:52:03  joergr
 * Prepared code for future support of 'config.guess' host identifiers.
 *
 * Revision 1.66  2002/09/23 13:50:41  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used.
 *
 * Revision 1.65  2002/08/21 10:14:15  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.64  2002/08/13 09:56:44  joergr
 * Added new profile (NONE) based on STD-GEN-xxxx which allows DICOM objects
 * of any transfer syntax to be referenced from a DICOMDIR.  NB: there's no
 * equivilent application profile in the DICOM standard.
 *
 * Revision 1.63  2002/07/11 16:08:26  joergr
 * Added support for CT/MR application profile.  Added general support for
 * monochrome icon images.
 * Added new command line flags to handle inconsistent header information
 * (patient ID and name).
 *
 * Revision 1.62  2002/07/02 16:52:14  joergr
 * Minor fixes to keep MSVC6 quiet.
 *
 * Revision 1.61  2002/07/02 16:16:16  joergr
 * Added support for ultrasound and waveform media storage application profiles.
 * Added Mammography CAD SR to the list of supported SOP classes.
 *
 * Revision 1.60  2002/04/16 13:38:54  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.59  2002/04/11 12:35:54  joergr
 * Replaced direct call of system routines by new standard date and time
 * functions.
 * Use the new standard file system routines like fileExists() etc.
 *
 * Revision 1.58  2001/12/06 14:03:16  joergr
 * Minor "stylistic" changes.
 *
 * Revision 1.57  2001/11/29 16:51:45  joergr
 * Added new command line option to dcmmkdir that allows to ignore non-standard
 * conformant spatial resolutions for images (e.g. images larger than 1024*1024
 * for the cardiac profiles).
 *
 * Revision 1.56  2001/11/19 17:53:36  joergr
 * Implemented performance optimization for the generation of icon images of
 * compressed multi-frame images.
 *
 * Revision 1.55  2001/11/19 12:43:17  joergr
 * Re-added dcmgpdir tool to dcmdata module.
 *
 * Revision 1.1  2001/11/13 17:57:14  joergr
 * Replaced utility dcmgpdir with dcmmkdir which supports other Media Storage
 * Application Profiles in addition to the General Purpose one.
 *
 *
 */
