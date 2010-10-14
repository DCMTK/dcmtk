/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.38 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


OFBool MdfJob::operator==(const MdfJob &j) const
{
    return (option == j.option) && (path == j.path) && (value == j.value);
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
    jobs(NULL), files(NULL)
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
        cmd->addOption("--ignore-errors",           "-ie",     "continue with file, if modify error occurs");
        cmd->addOption("--no-backup",               "-nb",     "don't backup files (DANGEROUS)");
    cmd->addGroup("input options:");
        cmd->addSubGroup("input file format:");
            cmd->addOption("--read-file",           "+f",      "read file format or data set (default)");
            cmd->addOption("--read-file-only",      "+fo",     "read file format only");
            cmd->addOption("--read-dataset",        "-f",      "read data set without file meta information");
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
        cmd->addSubGroup("other processing options:");
            cmd->addOption("--ignore-missing-tags", "-imt",    "treat 'tag not found' as success\nwhen modifying or erasing in datasets");
            cmd->addOption("--ignore-un-values",    "-iun",    "do not try writing any values\nto elements having VR of UN");
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
    if (app->parseCommandLine(*cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
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
    if (cmd->findOption("--ignore-errors"))
        ignore_errors_option = OFTrue;
    if (cmd->findOption("--no-meta-uid"))
        update_metaheader_uids_option = OFFalse;
    if (cmd->findOption("--no-backup"))
        no_backup_option = OFTrue;

    // input options
    cmd->beginOptionBlock();
    if (cmd->findOption("--read-file"))
        read_mode_option = ERM_autoDetect;
    if (cmd->findOption("--read-file-only"))
        read_mode_option = ERM_fileOnly;
    if (cmd->findOption("--read-dataset"))
        read_mode_option = ERM_dataset;
    cmd->endOptionBlock();

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
    {
        dcmAcceptOddAttributeLength.set(OFTrue);
    }
    if (cmd->findOption("--assume-even-length"))
    {
        dcmAcceptOddAttributeLength.set(OFFalse);
    }
    cmd->endOptionBlock();

    cmd->beginOptionBlock();
    if (cmd->findOption("--enable-correction"))
    {
        dcmEnableAutomaticInputDataCorrection.set(OFTrue);
    }
    if (cmd->findOption("--disable-correction"))
    {
        dcmEnableAutomaticInputDataCorrection.set(OFFalse);
    }
    cmd->endOptionBlock();

#ifdef WITH_ZLIB
    cmd->beginOptionBlock();
    if (cmd->findOption("--bitstream-deflated"))
    {
        dcmZlibExpectRFC1950Encoding.set(OFFalse);
    }
    if (cmd->findOption("--bitstream-zlib"))
    {
        dcmZlibExpectRFC1950Encoding.set(OFTrue);
    }
    cmd->endOptionBlock();
#endif

    // output options
    cmd->beginOptionBlock();
    if (cmd->findOption("--write-file"))
        output_dataset_option = OFFalse;
    if (cmd->findOption("--write-dataset"))
        output_dataset_option = OFTrue;
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
    }
    if (cmd->findOption("--disable-new-vr"))
    {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
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
    if (cmd->findOption("--ignore-missing-tags"))
    {
        ignore_missing_tags_option = OFTrue;
    }
    if (cmd->findOption("--no-reserv-check"))
    {
        no_reservation_checks = OFTrue;
    }
    if (cmd->findOption("--ignore-un-values"))
    {
        ignore_un_modifies = OFTrue;
    }
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
                result = ds_man->saveFile(filename, output_xfer_option,
                                          enctype_option, glenc_option,
                                          padenc_option, filepad_option,
                                          itempad_option, output_dataset_option);
                if (result.bad())
                {
                    OFLOG_ERROR(dcmodifyLogger, "couldn't save file: " << result.text());
                    errors++;
                    if (!no_backup_option)
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
            else if (!no_backup_option)
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
    result = ds_man->loadFile(filename, read_mode_option, input_xfer_option);
    if (result.good() && !no_backup_option)
        result = backupFile(filename);
    return result;
}


OFCondition MdfConsoleEngine::backupFile(const char *file_name)
{
    OFCondition backup_result;
    int result;
    OFString backup = file_name;
    backup += ".bak";
    // delete backup file, if it already exists
    if (OFStandard::fileExists(backup.c_str()))
    {
        int del_result = remove(backup.c_str());
        if (del_result != 0)
        {
            OFLOG_ERROR(dcmodifyLogger, "couldn't delete previous backup file, unable to backup!");
            return EC_IllegalCall;
        }
    }
    // if backup file could be removed, backup original file
    result = rename(file_name, backup.c_str());
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
    // successfully restored, throw out message
    else
        OFLOG_INFO(dcmodifyLogger, "Renamed backup file to original");
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


/*
** CVS/RCS Log:
** $Log: mdfconen.cc,v $
** Revision 1.38  2010-10-14 13:13:30  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.37  2010-05-28 13:19:19  joergr
** Changed logger name from "dcmtk.dcmdata.mdfconen" to "dcmtk.apps.dcmodify".
**
** Revision 1.36  2010-05-21 08:53:34  joergr
** Fixed wrong use of if statement which prevented option --insert from working.
**
** Revision 1.35  2010-05-20 15:53:58  joergr
** Added support for reading the value of insert/modify statements from a file.
** Slightly modified log messages and log levels in order to be more consistent.
** Removed some unnecessary include directives.
**
** Revision 1.34  2010-02-05 09:56:58  joergr
** Fixed issue with double locking of ofConsole.
** Fixed inconsistent source code formatting.
**
** Revision 1.33  2009-11-26 13:10:56  onken
** Added better error message to dcmodify in case write permissions for creating
** the backup file are missing.
**
** Revision 1.32  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.31  2009-09-04 13:53:09  meichel
** Minor const iterator related changes needed to compile with VC6 with HAVE_STL
**
** Revision 1.30  2009-08-21 09:25:13  joergr
** Added parameter 'writeMode' to save/write methods which allows for specifying
** whether to write a dataset or fileformat as well as whether to update the
** file meta information or to create a new file meta information header.
**
** Revision 1.29  2009-06-04 10:21:00  joergr
** Added new flag that can be used to avoid wrong warning messages (in debug
** mode) that an option has possibly never been checked.
**
** Revision 1.28  2009-04-24 12:20:42  joergr
** Fixed minor inconsistencies regarding layout/formatting in syntax usage.
**
** Revision 1.27  2009-04-21 14:02:49  joergr
** Fixed minor inconsistencies in manpage / syntax usage.
**
** Revision 1.26  2009-01-16 10:03:22  onken
** Fixed dcmodify help output
**
** Revision 1.25  2009-01-15 16:11:55  onken
** Reworked dcmodify to work with the new DcmPath classes for supporting
** wildcard paths and automatic insertion of missing attributes and items.
** Added options for private tag handling and modification of UN values and
** for ignoring errors resulting from missing tags during modify and erase
** operations. Further cleanups.
**
** Revision 1.24  2008-09-25 11:19:48  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.23  2008-03-26 17:01:40  joergr
** Fixed various layout and formatting issues.
**
** Revision 1.22  2007/11/23 15:42:53  meichel
** Removed unwanted output on console when debug flag is not set
**
** Revision 1.21  2006/12/06 09:31:49  onken
** Added "--no-backup" option to prevent dcmodify from creating backup files
**
** Revision 1.20  2006/08/15 15:50:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.19  2006/07/27 13:37:47  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
** Option "--help" is no longer an exclusive option by default.
** Made command line parameter "dcmfile-in" mandatory.
** Print help text if no command line argument is specified. This is the default
** behaviour of most DCMTK tools.
**
** Revision 1.18  2006/02/09 15:41:41  joergr
** Fixed typo in CVS log.
**
** Revision 1.17  2006/02/09 15:20:28  joergr
** Replaced OFIterator<> by OFListIterator() in order to compile if HAVE_STL
** is defined.
**
** Revision 1.16  2005/12/16 13:04:01  meichel
** Changed type to size_t to make code safe on 64bit platforms
**
** Revision 1.15  2005/12/08 15:40:51  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.14  2005/12/02 09:18:15  joergr
** Added new command line option that ignores the transfer syntax specified in
** the meta header and tries to detect the transfer syntax automatically from
** the dataset.
** Added new command line option that checks whether a given file starts with a
** valid DICOM meta header.
**
** Revision 1.13  2005/03/09 17:58:00  joergr
** Replaced "," between two delete statements by ";" since this confuses some
** compilers.
**
** Revision 1.12  2004/11/05 17:17:23  onken
** Added input and output options for dcmodify. minor code enhancements.
**
** Revision 1.11  2004/10/22 16:53:26  onken
** - fixed ignore-errors-option
** - major enhancements for supporting private tags
** - removed '0 Errors' output
** - modifications to groups 0000,0001,0002,0003,0005 and 0007 are blocked,
**   removing tags with group 0001,0003,0005 and 0007 is still possible
** - UID options:
**   - generate new study, series and instance UIDs
**   - When changing UIDs in dataset, related metaheader tags are updated
**     automatically
** - minor code improvements
**
** Revision 1.10  2004/04/19 14:45:07  onken
** Restructured code to avoid default parameter values for "complex types" like
** OFString. Required for Sun CC 2.0.1.
**
** Revision 1.9  2003/12/10 16:19:20  onken
** Changed API of MdfDatasetManager, so that its transparent for user, whether
** he wants to modify itemtags or tags at 1. level.
**
** Complete rewrite of MdfConsoleEngine. It doesn't support a batchfile any more,
** but now a user can give different modify-options at the same time on
** commandline. Other purifications and simplifications were made.
**
** Revision 1.8  2003/11/13 10:34:22  joergr
** Made help text consistent with revised man page.
**
** Revision 1.7  2003/11/11 10:55:51  onken
** - debug-mechanism doesn't use debug(..) any more
** - comments purified
** - headers adjustet to debug-modifications
**
** Revision 1.6  2003/10/13 14:51:49  onken
** improved backup-strategy
**
** Revision 1.5  2003/10/13 13:28:28  meichel
** Minor code purifications, needed for Borland C++
**
** Revision 1.4  2003/09/19 12:43:54  onken
** major bug fixes, corrections for "dcmtk-coding-style", better error-handling
**
** Revision 1.3  2003/07/09 12:13:13  meichel
** Included dcmodify in MSVC build system, updated headers
**
** Revision 1.2  2003/07/03 15:39:35  meichel
** Fixed problems with iterators, included zlib.h if needed
**
** Revision 1.1  2003/06/26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
