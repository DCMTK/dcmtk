/*
 *
 *  Copyright (c) 2010-2022, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Test read and write methods of DICOM RT classes
 *
 *  Generated manually
 *  File created on 2010-05-10
 *  Last modified on 2023-01-27 by Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofconapp.h"

#include "dcmtk/dcmrt/drtdose.h"
#include "dcmtk/dcmrt/drtimage.h"
#include "dcmtk/dcmrt/drtplan.h"
#include "dcmtk/dcmrt/drtstrct.h"
#include "dcmtk/dcmrt/drttreat.h"
#include "dcmtk/dcmrt/drtionpl.h"
#include "dcmtk/dcmrt/drtiontr.h"


#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "drtdump"

static OFLogger drtdumpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


#define DUMP_PATIENT_DATA(rtObject)                                         \
    if (rtObject.getPatientName(tmpString).good())                          \
    {                                                                       \
        OFString outString;                                                 \
        out << "Patient     : " << tmpString;                               \
        if (rtObject.getPatientSex(tmpString).good())                       \
            appendToString(outString, tmpString);                           \
        if (rtObject.getPatientBirthDate(tmpString).good())                 \
            appendToString(outString, tmpString);                           \
        if (rtObject.getPatientID(tmpString).good())                        \
            appendToString(outString, tmpString, "#" /*prefix*/);           \
        if (!outString.empty())                                             \
            out << " (" << outString << ")";                                \
        out << OFendl;                                                      \
    }

#define DUMP_PHYSICIAN_DATA(rtObject)                                       \
    if (rtObject.getReferringPhysicianName(tmpString).good())               \
    {                                                                       \
        out << "Physician   : " << tmpString << OFendl;                     \
    }

#define DUMP_OPERATOR_DATA(rtObject)                                        \
    if (rtObject.getOperatorsName(tmpString).good())                        \
    {                                                                       \
        out << "Operator    : " << tmpString << OFendl;                     \
    }

#define DUMP_STUDY_DATA(rtObject)                                           \
    if (rtObject.getStudyDescription(tmpString).good())                     \
    {                                                                       \
        OFString outString = tmpString;                                     \
        if (rtObject.getStudyID(tmpString).good())                          \
            appendToString(outString, tmpString, "#" /*prefix*/);           \
        if (rtObject.getStudyDate(tmpString).good())                        \
            appendToString(outString, tmpString);                           \
        if (rtObject.getStudyTime(tmpString).good())                        \
            appendToString(outString, tmpString);                           \
        out << "Study       : " << outString << OFendl;                     \
    }

#define DUMP_SERIES_DATA(rtObject)                                          \
    if (rtObject.getSeriesDescription(tmpString).good())                    \
    {                                                                       \
        OFString outString = tmpString;                                     \
        if (rtObject.getSeriesNumber(tmpString).good())                     \
            appendToString(outString, tmpString, "#" /*prefix*/);           \
        out << "Series      : " << outString << OFendl;                     \
    }

#define DUMP_INSTITUTION_DATA(rtObject)                                     \
    if (rtObject.getInstitutionName(tmpString).good() ||                    \
        rtObject.getInstitutionalDepartmentName(dummyString).good())        \
    {                                                                       \
        OFString outString = tmpString;                                     \
        if (rtObject.getInstitutionalDepartmentName(tmpString).good())      \
            appendToString(outString, tmpString);                           \
        out << "Institution : " << outString << OFendl;                     \
    }

#define DUMP_DEVICE_DATA(rtObject)                                          \
    if (rtObject.getManufacturer(tmpString).good())                         \
    {                                                                       \
        OFString outString = tmpString;                                     \
        if (rtObject.getManufacturerModelName(tmpString).good())            \
            appendToString(outString, tmpString);                           \
        if (rtObject.getStationName(tmpString).good())                      \
            appendToString(outString, tmpString);                           \
        out << "Device      : " << outString << OFendl;                     \
    }


// ********************************************


static void appendToString(OFString &outString,
                           const OFString &appendString,
                           const OFString &prefix = "",
                           const OFString &suffix = "",
                           const OFString &delimiter = ", ")
{
    if (!appendString.empty())
    {
        if (!outString.empty())
            outString += delimiter;
        outString += prefix;
        outString += appendString;
        outString += suffix;
    }
}

static OFCondition dumpRTDose(STD_NAMESPACE ostream &out,
                              DcmDataset &dataset)
{
    DRTDoseIOD rtObject;
    OFCondition result = rtObject.read(dataset);
    if (result.good())
    {
        OFString tmpString, dummyString;
        out << "RT Dose object" << OFendl << OFendl;
        /* output general information */
        DUMP_PATIENT_DATA(rtObject)
        DUMP_PHYSICIAN_DATA(rtObject)
        DUMP_STUDY_DATA(rtObject)
        DUMP_SERIES_DATA(rtObject)
        DUMP_INSTITUTION_DATA(rtObject)
        DUMP_DEVICE_DATA(rtObject)
        /* output further IOD specific data */
        // -- TODO ---
        out << OFendl;
    }
    return result;
}


static OFCondition dumpRTImage(STD_NAMESPACE ostream &out,
                               DcmDataset &dataset)
{
    DRTImageIOD rtObject;
    OFCondition result = rtObject.read(dataset);
    if (result.good())
    {
        OFString tmpString, dummyString;
        out << "RT Image object" << OFendl << OFendl;
        /* output general information */
        DUMP_PATIENT_DATA(rtObject)
        DUMP_PHYSICIAN_DATA(rtObject)
        DUMP_OPERATOR_DATA(rtObject)
        DUMP_STUDY_DATA(rtObject)
        DUMP_SERIES_DATA(rtObject)
        DUMP_INSTITUTION_DATA(rtObject)
        DUMP_DEVICE_DATA(rtObject)
        out << OFendl;
        /* output further IOD specific data */
        if (rtObject.getRTImageLabel(tmpString).good())
        {
            OFString outString = tmpString;
            if (rtObject.getRTImageName(tmpString).good())
                appendToString(outString, tmpString);
            if (rtObject.getRTImageDescription(tmpString).good())
                appendToString(outString, tmpString);
            out << "RT Image Label/Name   : " << outString << OFendl;
        }
        if (rtObject.getImageType(tmpString, -1 /*all*/).good())
        {
            OFString outString = tmpString;
            if (rtObject.getConversionType(tmpString).good())
                appendToString(outString, tmpString);
            if (rtObject.getRTImagePlane(tmpString).good())
                appendToString(outString, tmpString);
            out << "Image/Conversion Type : " << outString << OFendl;
        }
        if (rtObject.getRadiationMachineName(tmpString).good())
        {
            OFString outString = tmpString;
            if (rtObject.getRadiationMachineSAD(tmpString).good())
                appendToString(outString, tmpString, "" /*prefix*/, " mm" /*suffix*/);
            out << "Radiation Machine     : " << outString << OFendl;
        }
        Uint16 rows = 0;
        Uint16 columns = 0;
        if (rtObject.getRows(rows).good() &&
            rtObject.getColumns(columns).good())
        {
            out << "Image Resolution      : " << columns << " x " << rows << OFendl;
        }
        Uint16 bitsAllocated = 0;
        Uint16 bitsStored = 0;
        Uint16 highBit = 0;
        if (rtObject.getBitsAllocated(bitsAllocated).good() &&
            rtObject.getBitsStored(bitsStored).good() &&
            rtObject.getHighBit(highBit).good())
        {
            out << "Image Encoding        : " << bitsAllocated << " / " << bitsStored << " / " << highBit << OFendl;
        }
        out << OFendl;
    }
    return result;
}


static OFCondition dumpRTPlan(STD_NAMESPACE ostream &out,
                              DcmDataset &dataset)
{
    DRTPlanIOD rtObject;
    OFCondition result = rtObject.read(dataset);
    if (result.good())
    {
        OFString tmpString, dummyString;
        out << "RT Plan object" << OFendl << OFendl;
        /* output general information */
        DUMP_PATIENT_DATA(rtObject)
        DUMP_PHYSICIAN_DATA(rtObject)
        DUMP_OPERATOR_DATA(rtObject)
        DUMP_STUDY_DATA(rtObject)
        DUMP_SERIES_DATA(rtObject)
        DUMP_INSTITUTION_DATA(rtObject)
        DUMP_DEVICE_DATA(rtObject)
        /* output further IOD specific data */
        // -- TODO ---
        out << OFendl;
    }
    return result;
}


static OFCondition dumpRTStructureSet(STD_NAMESPACE ostream &out,
                                      DcmDataset &dataset)
{
    DRTStructureSetIOD rtObject;
    OFCondition result = rtObject.read(dataset);
    if (result.good())
    {
        OFString tmpString, dummyString;
        out << "RT Structure Set object" << OFendl << OFendl;
        /* output general information */
        DUMP_PATIENT_DATA(rtObject)
        DUMP_PHYSICIAN_DATA(rtObject)
        DUMP_STUDY_DATA(rtObject)
        DUMP_SERIES_DATA(rtObject)
        DUMP_INSTITUTION_DATA(rtObject)
        DUMP_DEVICE_DATA(rtObject)
        /* output further IOD specific data */
        // -- TODO ---
        out << OFendl;
    }
    return result;
}


static OFCondition dumpRTTreatmentSummaryRecord(STD_NAMESPACE ostream &out,
                                                DcmDataset &dataset)
{
    DRTTreatmentSummaryRecordIOD rtObject;
    OFCondition result = rtObject.read(dataset);
    if (result.good())
    {
        OFString tmpString, dummyString;
        out << "RT Treatment Summary Record object" << OFendl << OFendl;
        /* output general information */
        DUMP_PATIENT_DATA(rtObject)
        DUMP_PHYSICIAN_DATA(rtObject)
        DUMP_STUDY_DATA(rtObject)
        DUMP_SERIES_DATA(rtObject)
        DUMP_INSTITUTION_DATA(rtObject)
        DUMP_DEVICE_DATA(rtObject)
        /* output further IOD specific data */
        // -- TODO ---
        out << OFendl;
    }
    return result;
}


static OFCondition dumpRTIonPlan(STD_NAMESPACE ostream &out,
                                 DcmDataset &dataset)
{
    DRTIonPlanIOD rtObject;
    OFCondition result = rtObject.read(dataset);
    if (result.good())
    {
        OFString tmpString, dummyString;
        out << "RT Ion Plan object" << OFendl << OFendl;
        /* output general information */
        DUMP_PATIENT_DATA(rtObject)
        DUMP_PHYSICIAN_DATA(rtObject)
        DUMP_STUDY_DATA(rtObject)
        DUMP_SERIES_DATA(rtObject)
        DUMP_INSTITUTION_DATA(rtObject)
        DUMP_DEVICE_DATA(rtObject)
        /* output further IOD specific data */
        // -- TODO ---
        out << OFendl;
    }
    return result;
}


static OFCondition dumpRTIonBeamsTreatmentRecord(STD_NAMESPACE ostream &out,
                                                 DcmDataset &dataset)
{
    DRTIonBeamsTreatmentRecordIOD rtObject;
    OFCondition result = rtObject.read(dataset);
    if (result.good())
    {
        OFString tmpString, dummyString;
        out << "RT Ion Beams Treatment Record object" << OFendl << OFendl;
        /* output general information */
        DUMP_PATIENT_DATA(rtObject)
        DUMP_PHYSICIAN_DATA(rtObject)
        DUMP_STUDY_DATA(rtObject)
        DUMP_SERIES_DATA(rtObject)
        DUMP_INSTITUTION_DATA(rtObject)
        DUMP_DEVICE_DATA(rtObject)
        /* output further IOD specific data */
        // -- TODO ---
        out << OFendl;
    }
    return result;
}


static OFCondition dumpFile(STD_NAMESPACE ostream &out,
                            const char *ifname,
                            const E_FileReadMode readMode,
                            const E_TransferSyntax xfer)
{
    OFCondition result = EC_IllegalParameter;
    /* check for valid input filename */
    if ((ifname != NULL) && (strlen(ifname) > 0))
    {
        /* load DICOM file or dataset */
        DcmFileFormat fileformat;
        if (readMode == ERM_dataset)
            result = fileformat.getDataset()->loadFile(ifname, xfer);
        else
            result = fileformat.loadFile(ifname, xfer);
        if (result.good())
        {
            DcmDataset *dataset = fileformat.getDataset();
            /* check SOP Class UID for one of the supported RT objects */
            OFString sopClass;
            if (dataset->findAndGetOFString(DCM_SOPClassUID, sopClass).good() && !sopClass.empty())
            {
                if (sopClass == UID_RTDoseStorage)
                    result = dumpRTDose(out, *dataset);
                else if (sopClass == UID_RTImageStorage)
                    result = dumpRTImage(out, *dataset);
                else if (sopClass == UID_RTPlanStorage)
                    result = dumpRTPlan(out, *dataset);
                else if (sopClass == UID_RTStructureSetStorage)
                    result = dumpRTStructureSet(out, *dataset);
                else if (sopClass == UID_RTTreatmentSummaryRecordStorage)
                    result = dumpRTTreatmentSummaryRecord(out, *dataset);
                else if (sopClass == UID_RTIonPlanStorage)
                    result = dumpRTIonPlan(out, *dataset);
                else if (sopClass == UID_RTIonBeamsTreatmentRecordStorage)
                    result = dumpRTIonBeamsTreatmentRecord(out, *dataset);
                else
                    OFLOG_ERROR(drtdumpLogger, "unsupported SOPClassUID (" << sopClass << ") in file: " << ifname);
            } else {
                OFLOG_ERROR(drtdumpLogger, "SOPClassUID (0008,0016) missing or empty in file: " << ifname);
            }
        } else {
            OFLOG_FATAL(drtdumpLogger, OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                << ") reading file: " << ifname);
        }
    } else {
        OFLOG_FATAL(drtdumpLogger, OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>");
    }
    return result;
}


#define SHORTCOL 3
#define LONGCOL 20


int main(int argc, char *argv[])
{
    OFBool opt_printFilename = OFFalse;
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Dump DICOM RT file and data set", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("drtfile-in", "DICOM RT input filename to be dumped\n(\"-\" for stdin)", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                 "-h",  "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                     "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",          "+f",  "read file format or data set (default)");
        cmd.addOption("--read-file-only",     "+fo", "read file format only");
        cmd.addOption("--read-dataset",       "-f",  "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax:");
        cmd.addOption("--read-xfer-auto",     "-t=", "use TS recognition (default)");
        cmd.addOption("--read-xfer-detect",   "-td", "ignore TS specified in the file meta header");
        cmd.addOption("--read-xfer-little",   "-te", "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",      "-tb", "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit", "-ti", "read with implicit VR little endian TS");

    cmd.addGroup("output options:");
      cmd.addSubGroup("printing:");
        cmd.addOption("--print-filename",     "+Pf", "print header with filename for each input file");

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
                return 0;
            }
        }

        /* options */
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

        if (cmd.findOption("--print-filename"))
            opt_printFilename = OFTrue;
    }

    /* print resource identifier */
    OFLOG_DEBUG(drtdumpLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(drtdumpLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    int errorCount = 0;
    const int count = cmd.getParamCount();
    const char *current = NULL;
    for (int i = 1; i <= count; i++)
    {
        cmd.getParam(i, current);
        if (opt_printFilename)
        {
            if (i > 1)
                COUT << OFString(79, '-') << OFendl;
            COUT << OFFIS_CONSOLE_APPLICATION << " (" << i << "/" << count << "): " << current << OFendl << OFendl;
        }
        if (dumpFile(COUT, current, opt_readMode, opt_ixfer).bad())
            errorCount++;
    }

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif
    return errorCount;
}
