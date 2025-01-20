/*
 *
 *  Copyright (C) 1994-2024, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Andrew Hewett, Joerg Riesmeier
 *
 *  Purpose:
 *  Make a DICOMDIR according to the DICOM Part 11 Media Storage Application
 *  Profiles. Supports the following profiles:
 *  - General Purpose CD-R Interchange (STD-GEN-CD)
 *  - General Purpose Interchange on DVD-RAM Media (STD-GEN-DVD-RAM)
 *  - General Purpose Interchange on BD Media (STD-GEN-BD)
 *  If build with 'BUILD_DCMGPDIR_AS_DCMMKDIR' it also supports:
 *  - General Purpose DVD Interchange with JPEG (STD-GEN-DVD-JPEG)
 *  - General Purpose DVD Interchange with JPEG 2000 (STD-GEN-DVD-J2K)
 *  - General Purpose BD Interchange with JPEG (STD-GEN-BD-JPEG)
 *  - General Purpose BD Interchange with JPEG 2000 (STD-GEN-BD-J2K)
 *  - General Purpose BD Interchange with MPEG2 MP@ML (STD-GEN-BD-MPEG2-MPML)
 *  - General Purpose BD Interchange with MPEG2 MP@HL (STD-GEN-BD-MPEG2-MPHL)
 *  - General Purpose BD Interchange with MPEG-4 AVC/H.264 HiP@Level4.1 (STD-GEN-BD-MPEG4-HPLV41)
 *  - General Purpose BD Interchange with MPEG-4 AVC/H.264 BD-Compatible HiP@Level4.1 (STD-GEN-BD-MPEG4-HPLV41BD)
 *  - General Purpose BD Interchange with MPEG-4 AVC/H.264 HiP\@Level4.2 for 2D video (STD-GEN-BD-MPEG4-HPLV42-2D)
 *  - General Purpose BD Interchange with MPEG-4 AVC/H.264 HiP\@Level4.2 for 3D video (STD-GEN-BD-MPEG4-HPLV42-3D)
 *  - General Purpose BD Interchange with MPEG-4 AVC/H.264 Stereo HiP\@Level4.2 (STD-GEN-BD-MPEG4-SHPLV42)
 *  - General Purpose USB and Flash Memory Interchange with JPEG (STD-GEN-USB/MMC/CF/SD-JPEG)
 *  - General Purpose USB and Flash Memory Interchange with JPEG 2000 (STD-GEN-USB/MMC/CF/SD-J2K)
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
 */

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */

// We now always compile the tool with JPEG support
#define BUILD_DCMGPDIR_AS_DCMMKDIR

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

// this macro either expands to main() or wmain()
DCMTK_MAIN_FUNCTION
{
    OFBool opt_write = OFTrue;
    OFBool opt_append = OFFalse;
    OFBool opt_update = OFFalse;
    OFBool opt_recurse = OFFalse;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_withoutGL;
    OFFilename opt_output(DEFAULT_DICOMDIR_NAME);
    const char *opt_fileset = DEFAULT_FILESETID;
    const char *opt_descriptor = NULL;
    const char *opt_charset = DEFAULT_DESCRIPTOR_CHARSET;
    OFFilename opt_directory;
    OFFilename opt_pattern;
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

    cmd.addParam("dcmfile-in", "ref. DICOM file (or directory to be scanned)", OFCmdParam::PM_MultiOptional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                     "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                            "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("DICOMDIR identifiers:");
        cmd.addOption("--fileset-id",            "+F",  1, "[i]d: string",
                                                           "use specific file-set ID\n(default: " DEFAULT_FILESETID ", \"\" for none)");
        cmd.addOption("--descriptor",            "+R",  1, "[f]ilename: string",
                                                           "add a file-set descriptor file ID\n(e.g. README, default: no descriptor)");
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
      cmd.addSubGroup("DICOMDIR file:");
        cmd.addOption("--output-file",           "+D",  1, "[f]ilename: string",
                                                           "generate specific DICOMDIR file\n(default: " DEFAULT_DICOMDIR_NAME " in current directory)");
#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
      cmd.addSubGroup("profiles:");
        cmd.addOption("--general-purpose",       "-Pgp",   "General Purpose Interchange on CD-R, DVD-RAM\nor BD Media (STD-GEN-CD/DVD-RAM/BD, default)");
        cmd.addOption("--general-dvd-jpeg",      "-Pdv",   "General Purpose DVD Interchange with JPEG\n(STD-GEN-DVD-JPEG)");
        cmd.addOption("--general-dvd-j2k",       "-Pd2",   "General Purpose DVD Interchange with JPEG\n2000 (STD-GEN-DVD-J2K)");
        cmd.addOption("--general-bd-jpeg",       "-Pbd",   "General Purpose BD Interchange with JPEG\n(STD-GEN-BD-JPEG)");
        cmd.addOption("--general-bd-j2k",        "-Pb2",   "General Purpose BD Interchange with JPEG\n2000 (STD-GEN-BD-J2K)");
        cmd.addOption("--general-bd-mpeg2-mpml", "-Pbm",   "General Purpose BD Interchange with MPEG2\nMP@ML (STD-GEN-BD-MPEG2-MPML)");
        cmd.addOption("--general-bd-mpeg2-mphl", "-Pbh",   "General Purpose BD Interchange with MPEG2\nMP@HL (STD-GEN-BD-MPEG2-MPHL)");
        cmd.addOption("--general-bd-mpeg4-hp",   "-Pba",   "General Purpose BD Interchange with MPEG-4\nAVC/H.264 HiP@Level4.1\n(STD-GEN-BD-MPEG4-HPLV41)");
        cmd.addOption("--general-bd-mpeg4-hpbd", "-Pbb",   "General Purpose BD Interchange with MPEG-4\nAVC/H.264 BD-Compatible HiP@Level4.1\n(STD-GEN-BD-MPEG4-HPLV41BD)");
        cmd.addOption("--general-bd-mpeg4-hp2d",           "General Purpose BD Interchange with MPEG-4\nAVC/H.264 HiP@Level4.2 for 2D video\n(STD-GEN-BD-MPEG4-HPLV42-2D)");
        cmd.addOption("--general-bd-mpeg4-hp3d",           "General Purpose BD Interchange with MPEG-4\nAVC/H.264 HiP@Level4.2 for 3D video\n(STD-GEN-BD-MPEG4-HPLV42-3D)");
        cmd.addOption("--general-bd-mpeg4-hpst",           "General Purpose BD Interchange with MPEG-4\nAVC/H.264 Stereo HiP@Level4.2\n(STD-GEN-BD-MPEG4-SHPLV42)");
        cmd.addOption("--usb-and-flash-jpeg",    "-Pfl",   "General Purpose USB/Flash Memory Interchange\nwith JPEG (STD-GEN-USB/MMC/CF/SD-JPEG)");
        cmd.addOption("--usb-and-flash-j2k",     "-Pf2",   "General Purpose USB/Flash Memory Interchange\nwith JPEG 2000 (STD-GEN-USB/MMC/CF/SD-J2K)");
        cmd.addOption("--general-mime",          "-Pmi",   "General Purpose MIME Interchange\n(STD-GEN-MIME)");
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
      cmd.addSubGroup("backup:");
        cmd.addOption("--create-backup",                   "create a backup of existing DICOMDIR (def.)");
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
    if (app.parseCommandLine(cmd, argc, argv))
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
            if (opt_directory.isEmpty())
                opt_directory.set(".", OFTrue /*convert*/);
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
            OFFilename iconPrefix;
            app.checkValue(cmd.getValue(iconPrefix));
            ddir.setIconPrefix(iconPrefix);
        }
        if (cmd.findOption("--default-icon"))
        {
            OFFilename defaultIcon;
            app.checkValue(cmd.getValue(defaultIcon));
            ddir.setDefaultIcon(defaultIcon);
        }
#endif

        /* output options */
        if (cmd.findOption("--output-file"))
            app.checkValue(cmd.getValue(opt_output));

#ifdef BUILD_DCMGPDIR_AS_DCMMKDIR
        cmd.beginOptionBlock();
        if (cmd.findOption("--general-purpose"))
            opt_profile = DicomDirInterface::AP_GeneralPurpose;
        if (cmd.findOption("--general-dvd-jpeg"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeDVDJPEG;
        if (cmd.findOption("--general-dvd-j2k"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeDVDJPEG2000;
        if (cmd.findOption("--general-bd-jpeg"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDJPEG;
        if (cmd.findOption("--general-bd-j2k"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDJPEG2000;
        if (cmd.findOption("--general-bd-mpeg2-mpml"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDMPEG2MPatML;
        if (cmd.findOption("--general-bd-mpeg2-mphl"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDMPEG2MPatHL;
        if (cmd.findOption("--general-bd-mpeg4-hp"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDMPEG4HPatLV41;
        if (cmd.findOption("--general-bd-mpeg4-hpbd"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDMPEG4HPatLV41BD;
        if (cmd.findOption("--general-bd-mpeg4-hp2d"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDMPEG4HPatLV42_2D;
        if (cmd.findOption("--general-bd-mpeg4-hp3d"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDMPEG4HPatLV42_3D;
        if (cmd.findOption("--general-bd-mpeg4-hpst"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeBDMPEG4StereoHPatLV42;
        if (cmd.findOption("--usb-and-flash-jpeg"))
            opt_profile = DicomDirInterface::AP_USBandFlashJPEG;
        if (cmd.findOption("--usb-and-flash-j2k"))
            opt_profile = DicomDirInterface::AP_USBandFlashJPEG2000;
        if (cmd.findOption("--general-mime"))
            opt_profile = DicomDirInterface::AP_GeneralPurposeMIME;
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

        cmd.beginOptionBlock();
        if (cmd.findOption("--create-backup"))
            ddir.disableBackupMode(OFTrue);
        if (cmd.findOption("--no-backup"))
            ddir.disableBackupMode(OFFalse);
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--enable-new-vr"))
            dcmEnableGenerationOfNewVRs();
        if (cmd.findOption("--disable-new-vr"))
            dcmDisableGenerationOfNewVRs();
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

    /* make sure input directory exists (if specified) */
    if (!opt_directory.isEmpty())
    {
        if (!OFStandard::dirExists(opt_directory))
        {
            OFLOG_FATAL(dcmgpdirLogger, OFFIS_CONSOLE_APPLICATION << ": specified input directory does not exist");
            return 1;
        }
    }

    /* create list of input files */
    OFFilename paramValue;
    OFFilename pathName;
    OFList<OFFilename> fileNames;
    const int count = cmd.getParamCount();
    if (opt_recurse)
        OFLOG_INFO(dcmgpdirLogger, "determining input files ...");
    /* no parameters? */
    if (count == 0)
    {
        if (opt_recurse)
            OFStandard::searchDirectoryRecursively(OFFilename(), fileNames, opt_pattern, opt_directory);
        else
            app.printError("Missing parameter dcmfile-in");
    } else {
        /* iterate over all input filenames */
        for (int i = 1; i <= count; i++)
        {
            cmd.getParam(i, paramValue);
            /* add input directory */
            OFStandard::combineDirAndFilename(pathName, opt_directory, paramValue, OFTrue /*allowEmptyDirName*/);
            /* search directory recursively (if required) */
            if (OFStandard::dirExists(pathName))
            {
                if (opt_recurse)
                    OFStandard::searchDirectoryRecursively(paramValue, fileNames, opt_pattern, opt_directory);
                else
                    OFLOG_WARN(dcmgpdirLogger, "ignoring directory because option --recurse is not set: " << paramValue);
            } else
                fileNames.push_back(paramValue);
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
    const char *action = "";
    /* create new general purpose DICOMDIR, append to or update existing one */
    if (opt_append)
    {
        action = "appending";
        result = ddir.appendToDicomDir(opt_profile, opt_output);
    }
    else if (opt_update)
    {
        action = "updating";
        result = ddir.updateDicomDir(opt_profile, opt_output);
    } else {
        action = "creating";
        result = ddir.createNewDicomDir(opt_profile, opt_output, opt_fileset);
    }
    if (result.good())
    {
        /* set fileset descriptor and character set */
        result = ddir.setFilesetDescriptor(opt_descriptor, opt_charset);
        if (result.good())
        {
            /* collect 'bad' files */
            OFList<OFFilename> badFiles;
            unsigned int goodFiles = 0;
            OFListIterator(OFFilename) iter = fileNames.begin();
            OFListIterator(OFFilename) last = fileNames.end();
            /* iterate over all input filenames */
            while ((iter != last) && result.good())
            {
                /* add files to the DICOMDIR */
                result = ddir.addDicomFile((*iter), opt_directory);
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
            {
                action = "writing";
                result = ddir.writeDicomDir(opt_enctype, opt_glenc);
            }
        }
    }

    /* some final error reporting */
    if (result.bad() && (result != EC_IllegalCall))
    {
        OFLOG_FATAL(dcmgpdirLogger, OFFIS_CONSOLE_APPLICATION << ": error ("
            << result.text() << ") " << action << " file: " << opt_output);
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
