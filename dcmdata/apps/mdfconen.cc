/*
 *
 *  Copyright (C) 2003-2013, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: Class for modifying DICOM files from comandline
 *
 */

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first

#include "mdfconen.h"
#include "mdfdsman.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */

#define SHORTCOL 4
#define LONGCOL 21

#ifdef WITH_ZLIB
BEGIN_EXTERN_C
#include <zlib.h>
END_EXTERN_C
#endif

static OFLogger dcmodifyLogger = OFLog::getLogger("dcmtk.apps.dcmodify");

MdfJob::MdfJob(const MdfJob& other)
: option(other.option), path(other.path), value(other.value)
{
}

OFBool MdfJob::operator==(const MdfJob &j) const
{
    return (option == j.option) && (path == j.path) && (value == j.value);
}

MdfJob &MdfJob::operator=(const MdfJob &j)
{
    option = j.option;
    path = j.path;
    value = j.value;
    return *this;
}


MdfConsoleEngine::MdfConsoleEngine(int argc, char *argv[],
                                   const char *application_name)
  : app(NULL), cmd(NULL), ds_man(NULL), ignore_errors_option(OFFalse),
    update_metaheader_uids_option(OFTrue), no_backup_option(OFFalse),
    read_mode_option(ERM_autoDetect), input_xfer_option(EXS_Unknown),
    output_dataset_option(OFFalse), output_xfer_option(EXS_Unknown),
    glenc_option(EGL_recalcGL), enctype_option(EET_ExplicitLength),
    padenc_option(EPD_withoutPadding), filepad_option(0),
    itempad_option(0), ignore_missing_tags_option(OFFalse),
    no_reservation_checks(OFFalse), ignore_un_modifies(OFFalse),
    create_if_necessary(OFFalse), was_created(OFFalse), jobs(NULL), files(NULL)
{
    char rcsid[200];
    // print application header
    sprintf(rcsid, "$dcmtk: %s v%s %s $", application_name, OFFIS_DCMTK_VERSION, OFFIS_DCMTK_RELEASEDATE);

    // the next lines describe commandline arguments/options
    app = new OFConsoleApplication(application_name, "Modify DICOM files", rcsid);
    cmd = new OFCommandLine();

    cmd->setOptionColumns(LONGCOL, SHORTCOL);
    cmd->setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd->addParam("dcmfile-in", "DICOM input filename to be modified", OFCmdParam::PM_MultiMandatory);

    // add options to commandline application
    cmd->addGroup("general options:", LONGCOL, SHORTCOL + 2);
        cmd->addOption("--help",                    "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
        cmd->addOption("--version",                            "print version information and exit", OFCommandLine::AF_Exclusive);
        OFLog::addOptions(*cmd);
    cmd->addGroup("input options:");
        cmd->addSubGroup("input file format:");
            cmd->addOption("--read-file",           "+f",      "read file format or data set (default)");
            cmd->addOption("--read-file-only",      "+fo",     "read file format only");
            cmd->addOption("--read-dataset",        "-f",      "read data set without file meta information");
            cmd->addOption("--create-file",         "+fc",     "create file format if file does not exist");
        cmd->addSubGroup("input transfer syntax:");
            cmd->addOption("--read-xfer-auto",      "-t=",     "use TS recognition (default)");
            cmd->addOption("--read-xfer-detect",    "-td",     "ignore TS specified in the file meta header");
            cmd->addOption("--read-xfer-little",    "-te",     "read with explicit VR little endian TS");
            cmd->addOption("--read-xfer-big",       "-tb",     "read with explicit VR big endian TS");
            cmd->addOption("--read-xfer-implicit",  "-ti",     "read with implicit VR little endian TS");
        cmd->addSubGroup("parsing of odd-length attributes:");
            cmd->addOption("--accept-odd-length",   "+ao",     "accept odd length attributes (default)");
            cmd->addOption("--assume-even-length",  "+ae",     "assume real length is one byte larger");
        cmd->addSubGroup("automatic data correction:");
            cmd->addOption("--enable-correction",   "+dc",     "enable automatic data correction (default)");
            cmd->addOption("--disable-correction",  "-dc",     "disable automatic data correction");
#ifdef WITH_ZLIB
        cmd->addSubGroup("bitstream format of deflated input:");
            cmd->addOption("--bitstream-deflated",  "+bd",     "expect deflated bitstream (default)");
            cmd->addOption("--bitstream-zlib",      "+bz",     "expect deflated zlib bitstream");
#endif

    cmd->addGroup("processing options:");
        cmd->addSubGroup("backup input files:");
            cmd->addOption("--backup",                         "backup files before modifying (default)");
            cmd->addOption("--no-backup",           "-nb",     "don't backup files (DANGEROUS)");
        cmd->addSubGroup("insert mode:");
            cmd->addOption("--insert",              "-i",   1, "\"[t]ag-path=[v]alue\"",
                                                               "insert (or overwrite) path at position t\nwith value v", OFCommandLine::AF_NoWarning);
            cmd->addOption("--insert-from-file",    "-if",  1, "\"[t]ag-path=[f]ilename\"",
                                                               "insert (or overwrite) path at position t\nwith value from file f", OFCommandLine::AF_NoWarning);
            cmd->addOption("--no-reserv-check",     "-nrc",    "do not check private reservations\nwhen inserting private tags");
        cmd->addSubGroup("modify mode:");
            cmd->addOption("--modify",              "-m",   1, "\"[t]ag-path=[v]alue\"",
                                                               "modify tag at position t to value v", OFCommandLine::AF_NoWarning);
            cmd->addOption("--modify-from-file",    "-mf",  1, "\"[t]ag-path=[f]ilename\"",
                                                               "modify tag at position t to value from file f", OFCommandLine::AF_NoWarning);
            cmd->addOption("--modify-all",          "-ma",  1, "\"[t]ag=[v]alue\"",
                                                               "modify ALL matching tags t in file to value v", OFCommandLine::AF_NoWarning);
        cmd->addSubGroup("erase mode:");
            cmd->addOption("--erase",               "-e",   1, "\"[t]ag-path\"",
                                                               "erase tag/item at position t", OFCommandLine::AF_NoWarning);
            cmd->addOption("--erase-all",           "-ea",  1, "\"[t]ag\"",
                                                               "erase ALL matching tags t in file", OFCommandLine::AF_NoWarning);
            cmd->addOption("--erase-private",       "-ep",     "erase ALL private data from file", OFCommandLine::AF_NoWarning);
        cmd->addSubGroup("unique identifier:");
            cmd->addOption("--gen-stud-uid",        "-gst",    "generate new Study Instance UID", OFCommandLine::AF_NoWarning);
            cmd->addOption("--gen-ser-uid",         "-gse",    "generate new Series Instance UID", OFCommandLine::AF_NoWarning);
            cmd->addOption("--gen-inst-uid",        "-gin",    "generate new SOP Instance UID", OFCommandLine::AF_NoWarning);
            cmd->addOption("--no-meta-uid",         "-nmu",    "do not update metaheader UIDs if related\nUIDs in the dataset are modified");
        cmd->addSubGroup("error handling:");
            cmd->addOption("--ignore-errors",       "-ie",     "continue with file, if modify error occurs");
            cmd->addOption("--ignore-missing-tags", "-imt",    "treat 'tag not found' as success\nwhen modifying or erasing in datasets");
            cmd->addOption("--ignore-un-values",    "-iun",    "do not try writing any values to elements\nhaving a VR of UN");
    cmd->addGroup("output options:");
        cmd->addSubGroup("output file format:");
            cmd->addOption("--write-file",          "+F",      "write file format (default)");
            cmd->addOption("--write-dataset",       "-F",      "write data set without file meta information");
        cmd->addSubGroup("output transfer syntax:");
            cmd->addOption("--write-xfer-same",     "+t=",     "write with same TS as input (default)");
            cmd->addOption("--write-xfer-little",   "+te",     "write with explicit VR little endian TS");
            cmd->addOption("--write-xfer-big",      "+tb",     "write with explicit VR big endian TS");
            cmd->addOption("--write-xfer-implicit", "+ti",     "write with implicit VR little endian TS");
        cmd->addSubGroup("post-1993 value representations:");
            cmd->addOption("--enable-new-vr",       "+u",      "enable support for new VRs (UN/UT) (default)");
            cmd->addOption("--disable-new-vr",      "-u",      "disable support for new VRs, convert to OB");
        cmd->addSubGroup("group length encoding:");
            cmd->addOption("--group-length-recalc", "+g=",     "recalculate group lengths if present (default)");
            cmd->addOption("--group-length-create", "+g",      "always write with group length elements");
            cmd->addOption("--group-length-remove", "-g",      "always write without group length elements");
        cmd->addSubGroup("length encoding in sequences and items:");
            cmd->addOption("--length-explicit",     "+le",     "write with explicit lengths (default)");
            cmd->addOption("--length-undefined",    "-le",     "write with undefined lengths");
        cmd->addSubGroup("data set trailing padding (not with --write-dataset):");
            cmd->addOption("--padding-retain",      "-p=",     "do not change padding\n(default if not --write-dataset)");
            cmd->addOption("--padding-off",         "-p",      "no padding (implicit if --write-dataset)");
            cmd->addOption("--padding-create",      "+p",   2, "[f]ile-pad [i]tem-pad: integer",
                                                               "align file on multiple of f bytes\nand items on multiple of i bytes");

    // evaluate commandline
    prepareCmdLineArgs(argc, argv, application_name);
    if (app->parseCommandLine(*cmd, argc, argv))
    {
        /* print help text and exit */
        if (cmd->getArgCount() == 0)
            app->printUsage();

        /* check exclusive options first */
        if (cmd->hasExclusiveOption())
        {
            if (cmd->findOption("--version"))
            {
                app->printHeader(OFTrue /*print host identifier*/);
                ofConsole.lockCout() << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
                ofConsole.getCout() << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
                ofConsole.getCout() << " none" << OFendl;
#endif
                ofConsole.unlockCout();
                delete app;
                delete cmd;
                exit(0);
            }
        }

        // iterate the files (parameters) and save them in list
        files = new OFList<OFString>;
        OFString current_file;
        for (int i = 1; i <= cmd->getParamCount(); i++)
        {
            cmd->getParam(i,current_file);
            files->push_back(current_file);
        }
        // if no files are given: return with error message
        if (files->empty())
        {
            OFLOG_ERROR(dcmodifyLogger, "no dicom files given!");
            delete app;
            delete cmd;
            exit(1);
        }

        // make sure data dictionary is loaded
        if (!dcmDataDict.isDictionaryLoaded())
            OFLOG_WARN(dcmodifyLogger, "no data dictionary loaded, "
                << "check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmodifyLogger, rcsid << OFendl);
}


void MdfConsoleEngine::parseNonJobOptions()
{
    // catch "general" options
    OFLog::configureFromCommandLine(*cmd, *app);

    // input options
    cmd->beginOptionBlock();
    if (cmd->findOption("--read-file"))
        read_mode_option = ERM_autoDetect;
    if (cmd->findOption("--read-file-only"))
        read_mode_option = ERM_fileOnly;
    if (cmd->findOption("--read-dataset"))
        read_mode_option = ERM_dataset;
    cmd->endOptionBlock();

    if (cmd->findOption("--create-file"))
        create_if_necessary = OFTrue;

    cmd->beginOptionBlock();
    if (cmd->findOption("--read-xfer-auto"))
        input_xfer_option = EXS_Unknown;
    if (cmd->findOption("--read-xfer-detect"))
        dcmAutoDetectDatasetXfer.set(OFTrue);
    if (cmd->findOption("--read-xfer-little"))
    {
        app->checkDependence("--read-xfer-little", "--read-dataset", read_mode_option == ERM_dataset);
        input_xfer_option = EXS_LittleEndianExplicit;
    }
    if (cmd->findOption("--read-xfer-big"))
    {
        app->checkDependence("--read-xfer-big", "--read-dataset", read_mode_option == ERM_dataset);
        input_xfer_option = EXS_BigEndianExplicit;
    }
    if (cmd->findOption("--read-xfer-implicit"))
    {
        app->checkDependence("--read-xfer-implicit", "--read-dataset", read_mode_option == ERM_dataset);
        input_xfer_option = EXS_LittleEndianImplicit;
    }
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--accept-odd-length"))
        dcmAcceptOddAttributeLength.set(OFTrue);
    if (cmd->findOption("--assume-even-length"))
        dcmAcceptOddAttributeLength.set(OFFalse);
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--enable-correction"))
        dcmEnableAutomaticInputDataCorrection.set(OFTrue);
    if (cmd->findOption("--disable-correction"))
        dcmEnableAutomaticInputDataCorrection.set(OFFalse);
    cmd->endOptionBlock();

#ifdef WITH_ZLIB
    cmd->beginOptionBlock();
    if (cmd->findOption("--bitstream-deflated"))
        dcmZlibExpectRFC1950Encoding.set(OFFalse);
    if (cmd->findOption("--bitstream-zlib"))
        dcmZlibExpectRFC1950Encoding.set(OFTrue);
    cmd->endOptionBlock();
#endif

    // processing options
    cmd->beginOptionBlock();
    if (cmd->findOption("--backup"))
        no_backup_option = OFFalse;
    if (cmd->findOption("--no-backup"))
        no_backup_option = OFTrue;
    cmd->endOptionBlock();

    if (cmd->findOption("--no-reserv-check"))
        no_reservation_checks = OFTrue;

    if (cmd->findOption("--no-meta-uid"))
        update_metaheader_uids_option = OFFalse;

    if (cmd->findOption("--ignore-errors"))
        ignore_errors_option = OFTrue;
    if (cmd->findOption("--ignore-missing-tags"))
        ignore_missing_tags_option = OFTrue;
    if (cmd->findOption("--ignore-un-values"))
        ignore_un_modifies = OFTrue;

    // output options
    cmd->beginOptionBlock();
    if (cmd->findOption("--write-file"))
        output_dataset_option = OFFalse;
    if (cmd->findOption("--write-dataset"))
    {
        output_dataset_option = OFTrue;
        app->checkConflict("--write-dataset", "--create-file", create_if_necessary);
    }
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--write-xfer-same"))
        output_xfer_option = EXS_Unknown;
    if (cmd->findOption("--write-xfer-little"))
        output_xfer_option = EXS_LittleEndianExplicit;
    if (cmd->findOption("--write-xfer-big"))
        output_xfer_option = EXS_BigEndianExplicit;
    if (cmd->findOption("--write-xfer-implicit"))
        output_xfer_option = EXS_LittleEndianImplicit;
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--enable-new-vr"))
    {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
        dcmEnableOtherFloatStringVRGeneration.set(OFTrue);
        dcmEnableOtherDoubleStringVRGeneration.set(OFTrue);
    }
    if (cmd->findOption("--disable-new-vr"))
    {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
        dcmEnableOtherFloatStringVRGeneration.set(OFFalse);
        dcmEnableOtherDoubleStringVRGeneration.set(OFFalse);
    }
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--group-length-recalc"))
        glenc_option = EGL_recalcGL;
    if (cmd->findOption("--group-length-create"))
        glenc_option = EGL_withGL;
    if (cmd->findOption("--group-length-remove"))
        glenc_option = EGL_withoutGL;
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--length-explicit"))
        enctype_option = EET_ExplicitLength;
    if (cmd->findOption("--length-undefined"))
        enctype_option = EET_UndefinedLength;
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--padding-retain"))
    {
        app->checkConflict("--padding-retain", "--write-dataset", output_dataset_option);
        padenc_option = EPD_noChange;
    }
    if (cmd->findOption("--padding-off"))
        padenc_option = EPD_withoutPadding;
    if (cmd->findOption("--padding-create"))
    {
        app->checkConflict("--padding-create", "--write-dataset", output_dataset_option);
        app->checkValue(cmd->getValueAndCheckMin(filepad_option, 0));
        app->checkValue(cmd->getValueAndCheckMin(itempad_option, 0));
        padenc_option = EPD_withPadding;
    }
    cmd->endOptionBlock();
}


void MdfConsoleEngine::parseCommandLine()
{
    jobs = new OFList<MdfJob>;
    OFString option_string;
    // check all options, that don't belong to a specific job
    parseNonJobOptions();

    cmd->gotoFirstOption();
    // iterate over commandline arguments from first to last
    do {
        if (cmd->getCurrentOption(option_string))
        {
            MdfJob aJob;
            OFString option_value, tag_path, tag_value;
            if (option_string == "--insert")
                aJob.option = "i";
            else if (option_string == "--insert-from-file")
                aJob.option = "if";
            else if (option_string == "--modify")
                aJob.option = "m";
            else if (option_string == "--modify-from-file")
                aJob.option = "mf";
            else if (option_string == "--modify-all")
                aJob.option = "ma";
            else if (option_string == "--erase")
                aJob.option = "e";
            else if (option_string == "--erase-all")
                aJob.option = "ea";
            else if (option_string == "--erase-private")
                aJob.option = "ep";
            else if (option_string == "--gen-stud-uid")
                aJob.option = "gst";
            else if (option_string == "--gen-ser-uid")
                aJob.option = "gse";
            else if (option_string == "--gen-inst-uid")
                aJob.option = "gin";
            // else this is a non job option, e.g. -v, -d, -f, ...
            else
                continue;
            // get any parameters if job expects some
            if (jobOptionExpectsParameters(aJob.option))
            {
                cmd->getValue(option_value);
                splitPathAndValue(option_value, tag_path, tag_value);
                aJob.path = tag_path;
                aJob.value = tag_value;
            }
            // finally, and schedule job
            jobs->push_back(aJob);
        }
    } while (cmd->gotoNextOption());
}


OFBool MdfConsoleEngine::jobOptionExpectsParameters(const OFString &job)
{
    return (job != "ep") && (job != "gst") && (job != "gse") && (job != "gin");
}


void MdfConsoleEngine::splitPathAndValue(const OFString &whole,
                                         OFString &path,
                                         OFString &value)
{
    size_t pos = whole.find("=");
    if (pos != OFString_npos)
    {
        path = whole.substr(0, pos);
        value = whole.substr(pos + 1, value.length() - 1);
    }
    else path = whole;
}


int MdfConsoleEngine::executeJob(const MdfJob &job,
                                 const char *filename)
{
    OFCondition result;
    int count = 0;
    int error_count = 0;
    OFLOG_INFO(dcmodifyLogger, "Executing (option|path|value): "
        << job.option << "|" << job.path << "|" << job.value);
    // start modify operation based on job option
    if (job.option=="i")
        result = ds_man->modifyOrInsertPath(job.path, job.value, OFFalse, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    else if (job.option == "if")
        result = ds_man->modifyOrInsertFromFile(job.path, job.value /*filename*/, OFFalse, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    else if (job.option == "m")
        result = ds_man->modifyOrInsertPath(job.path, job.value, OFTrue, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    else if (job.option == "mf")
        result = ds_man->modifyOrInsertFromFile(job.path, job.value /*filename*/, OFTrue, update_metaheader_uids_option, ignore_missing_tags_option, no_reservation_checks);
    else if (job.option == "ma")
        result = ds_man->modifyAllTags(job.path, job.value, update_metaheader_uids_option, count);
    else if (job.option == "e")
        result = ds_man->deleteTag(job.path, OFFalse, ignore_missing_tags_option);
    else if (job.option == "ea")
        result = ds_man->deleteTag(job.path, OFTrue, ignore_missing_tags_option);
    else if (job.option == "ep")
        result = ds_man->deletePrivateData();
    else if (job.option == "gst")
        result = ds_man->generateAndInsertUID(DCM_StudyInstanceUID);
    else if (job.option == "gse")
        result = ds_man->generateAndInsertUID(DCM_SeriesInstanceUID);
    else if (job.option == "gin")
        result = ds_man->generateAndInsertUID(DCM_SOPInstanceUID);
    // no valid job option found:
    else
    {
        error_count++;
        OFLOG_ERROR(dcmodifyLogger, "no valid option: " << job.option);
    }
    // if modify operation failed
    if (result.bad() && error_count == 0)
    {
        if (filename != NULL)
            OFLOG_ERROR(dcmodifyLogger, "modifying tag in file " << OFString(filename) << ": " << result.text());
        else
            OFLOG_ERROR(dcmodifyLogger, "modifying tag: " << result.text());
        error_count++;
    }
    return error_count;
}


int MdfConsoleEngine::startProvidingService()
{
    OFCondition result;
    const char *filename;
    // return value of this function
    int errors = 0;
    // parse command line into file and job list
    parseCommandLine();
    // iterators for job and file loops
    OFListIterator(MdfJob) job_it;
    OFListIterator(MdfJob) job_last = jobs->end();;
    OFListIterator(OFString) file_it = files->begin();
    OFListIterator(OFString) file_last = files->end();;
    // outer loop: iterate over all files
    while (file_it != file_last)
    {
        filename = (*file_it).c_str();
        result = loadFile(filename);

        // if file could be loaded:
        if (result.good())
        {
            // for each file, set job iterator back to first entry
            job_it = jobs->begin();
            // inner loop: iterate over jobs, execute all jobs for current file
            while (job_it != job_last)
            {
                errors += executeJob(*job_it, filename);
                job_it++;
            }
            // if there were no errors or user wants to override them, save:
            if (errors == 0 || ignore_errors_option)
            {
                if (was_created && (output_xfer_option == EXS_Unknown))
                {
                  output_xfer_option = EXS_LittleEndianExplicit;
                }
                result = ds_man->saveFile(filename, output_xfer_option,
                                          enctype_option, glenc_option,
                                          padenc_option, filepad_option,
                                          itempad_option, output_dataset_option);
                if (result.bad())
                {
                    OFLOG_ERROR(dcmodifyLogger, "couldn't save file: " << result.text());
                    errors++;
                    if (!no_backup_option && !was_created)
                    {
                        result = restoreFile(filename);
                        if (result.bad())
                        {
                            OFLOG_ERROR(dcmodifyLogger, "couldn't restore file: " << result.text());
                            errors++;
                        }
                    }
                }
            }
            // errors occured and user doesn't want to ignore them:
            else if (!no_backup_option && !was_created)
            {
                result = restoreFile(filename);
                if (result.bad())
                {
                    OFLOG_ERROR(dcmodifyLogger, "couldn't restore file!");
                    errors++;
                }
            }
        }
        // if loading fails:
        else
        {
            errors++;
            OFLOG_ERROR(dcmodifyLogger, "unable to load file " << filename <<": " << result.text());
        }
        file_it++;
        // output separator line if required
        if ((file_it != file_last) || (errors > 0))
          OFLOG_INFO(dcmodifyLogger, "------------------------------------");
    }
    return errors;
}


OFCondition MdfConsoleEngine::loadFile(const char *filename)
{
    OFCondition result;
    // free memory
    delete ds_man;
    ds_man = new MdfDatasetManager();
    ds_man->setModifyUNValues(!ignore_un_modifies);
    OFLOG_INFO(dcmodifyLogger, "Processing file: " << filename);
    // load file into dataset manager
    was_created = !OFStandard::fileExists(filename);
    result = ds_man->loadFile(filename, read_mode_option, input_xfer_option, create_if_necessary);
    if (result.good() && !no_backup_option && !was_created)
        result = backupFile(filename);
    return result;
}


OFCondition MdfConsoleEngine::backupFile(const char *filename)
{
    int result;
    OFString backup = filename;
    backup += ".bak";
    OFLOG_INFO(dcmodifyLogger, "Creating backup of input file: " << backup);
    // delete backup file, if it already exists
    if (OFStandard::fileExists(backup.c_str()))
    {
        result = remove(backup.c_str());
        if (result != 0)
        {
            OFLOG_ERROR(dcmodifyLogger, "couldn't delete previous backup file, unable to backup!");
            return EC_IllegalCall;
        }
    }
    // if backup file could be removed, backup original file
    result = rename(filename, backup.c_str());
    // set return value
    if (result != 0)
    {
        OFLOG_ERROR(dcmodifyLogger, "unable to backup, no write permission?");
        return EC_IllegalCall;
    }

    return EC_Normal;
}


OFCondition MdfConsoleEngine::restoreFile(const char *filename)
{
    int result;
    OFString backup = filename;
    backup += ".bak";
    OFLOG_INFO(dcmodifyLogger, "Restoring original file from backup");
    // delete the (original) file that dcmodify couldn't modify
    if (OFStandard::fileExists(filename))
    {
        result = remove(filename);
        if (result != 0)
        {
            OFLOG_ERROR(dcmodifyLogger, "unable to delete original file for restoring backup!");
            return EC_IllegalCall;
        }
    }
    // and rename backup file back to original filename
    result = rename(backup.c_str(), filename);
    // error renaming backup file
    if (result != 0)
    {
        OFLOG_ERROR(dcmodifyLogger, "unable to rename backup file to original filename!");
        return EC_IllegalCall;
    }
    // you only get to this point, if restoring was completely successful
    return EC_Normal;
}


MdfConsoleEngine::~MdfConsoleEngine()
{
    delete app;
    delete cmd;
    delete files;
    delete jobs;
    delete ds_man;
}
