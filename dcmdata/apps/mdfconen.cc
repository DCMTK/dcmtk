/*
 *
 *  Copyright (C) 2003, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for modifying DICOM files from comandline
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2004-10-22 16:53:26 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfconen.cc,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"   // make sure OS specific configuration is included first
#include "mdfconen.h"
#include "oftypes.h"
#include "dctk.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"       /* for dcmtk version name */
#include "oflist.h"
#include "ofstring.h"
#include "ofstd.h"

#define SHORTCOL 3
#define LONGCOL 17

#ifdef WITH_ZLIB
BEGIN_EXTERN_C
#include <zlib.h>
END_EXTERN_C
#endif


OFBool MdfJob::operator==(const MdfJob& j) const
{
// Date         : April 18th, 2004
// Author       : Michael Onken
// Task         : Comparison operator for MdfJob
// Parameters   : j - [in] Job to compare with
// Return Value : OFTrue, if jobs are equal, OFFalse else
    if ( (option==j.option) && (path==j.path) && (value==j.value) )
        return OFTrue;
    else
        return OFFalse;
}


MdfConsoleEngine::MdfConsoleEngine(int argc, char *argv[],
                                   const char *application_name)
  : app(NULL), cmd(NULL), ds_man(NULL), verbose_option(OFFalse),
    debug_option(OFFalse), ignore_errors_option(OFFalse),
    update_metaheader_uids_option(OFTrue), jobs(NULL), files(NULL)
// Date         : May 13th, 2003
// Author       : Michael Onken
// Task         : Constructor.
// Parameters   : argc - [in] number of commandline arguments
//                argv - [in] array of commandline arguments
//                application_name - [in] name of calling application
// Return Value : none
{
    //print application header
    char rcsid[200];
    sprintf( rcsid, "$dcmtk: %s v%s %s $", application_name,
             OFFIS_DCMTK_VERSION, OFFIS_DCMTK_RELEASEDATE );

    //the next lines describe commandline arguments/options
    app=new OFConsoleApplication(application_name, "Modify DICOM files", rcsid);
    cmd=new OFCommandLine();

    cmd->setParamColumn(LONGCOL + SHORTCOL + 2);
    cmd->addParam("dcmfile-in", "DICOM input filename to be modified",
                  OFCmdParam::PM_MultiOptional);

    cmd->setOptionColumns(LONGCOL, SHORTCOL);
    //add options to commandline application
    cmd->addGroup("general options:");
        cmd->addOption("--help", "-h", "print this help text and exit");
        cmd->addOption("--version", "print version information and exit", OFTrue /* exclusive */);
        cmd->addOption("--debug", "-d", "debug mode, print debug information");
        cmd->addOption("--verbose", "-v", "verbose mode, print verbose output");
        cmd->addOption("--ignore-errors", "-ie", "continue with file, if modify error occurs\n");

    cmd->addGroup("insert-mode options:");
        cmd->addOption("--insert-tag", "-i",1, "\"tag-path=value\"", "insert (or overwrite) tag");

    cmd->addGroup("modify-mode options:");
        cmd->addOption("--modify-tag", "-m",1, "\"tag-path=value\"", "modify tag");
        cmd->addOption("--modify-all-tags", "-ma",1, "\"tag=value\"", "modify ALL matching tags in file");

    cmd->addGroup("erase-mode options:");
        cmd->addOption("--erase-tag", "-e",1, "\"tag-path\"", "erase tag");
        cmd->addOption("--erase-all-tags", "-ea",1, "\"tag\"", "erase ALL matching tags in file");

    cmd->addGroup("uid options:");
        cmd->addOption("--generate-study-uid", "-gst", "generate new Study Instance UID");
        cmd->addOption("--generate-series-uid", "-gse", "generate new Series Instance UID");
        cmd->addOption("--generate-instance-uid", "-gin", "generate new SOP Instance UID");
        cmd->addOption("--dont-update-metaheader-uids", "-dum", "don't update metaheader UIDs");

    //evaluate commandline
    prepareCmdLineArgs(argc, argv, application_name);
    if (app->parseCommandLine(*cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
        //if no argument or --help is given, print usage
        if ( (cmd->getArgCount() == 0) || (cmd->findOption("--help")) )
        {
            app->printUsage();
            delete app, delete cmd;
            exit(0);
        }

        //if no Parameter is given, check for --version and print info
        if (cmd->findOption("--version"))
        {
            app->printHeader(OFTrue /*print host identifier*/);
            debugMsg(OFTrue,"External libraries used: ","","");
#ifdef WITH_ZLIB
            debugMsg(OFTrue,"ZLIB, Version ", zlibVersion(),"");
#else
            debugMsg(OFTrue," none","","");
            delete app, delete cmd;
            exit(0);
#endif
        }

        //iterate the files (parameters) and save them in list
        files=new OFList<OFString>;
        OFString current_file;
        for (int i=1; i<=cmd->getParamCount(); i++)
        {
            cmd->getParam(i,current_file);
            files->push_back(current_file);
        }
        //if no files are given: return with error message
        if (files->empty()==OFTrue)
        {
            debugMsg(OFTrue,"No dicom files given!","","");
            delete app, delete cmd;
            exit(1);
        }

        // make sure data dictionary is loaded
        if (!dcmDataDict.isDictionaryLoaded())
            debugMsg(OFTrue,"Warning: no data dictionary loaded, ",
                "check environment variable: ", DCM_DICT_ENVIRONMENT_VARIABLE);
    }
}


void MdfConsoleEngine::splitPathAndValue(const OFString &whole,
                                         OFString &path,
                                         OFString &value)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : This function splits a modify option (inclusive value) as
//                found on commandline into two parts (path and value)
//                e.g. "(0010,0010)=value" into path "(0010,0010)" and "value"
//                if given string is "", path and value will be empty, too
// Parameters   : whole - [in] string to be splitted
//                path - [out] returns part containing the path
//                value - [out] returns part containing the value(if theres one)
// Return Value : none
{
    unsigned int pos=whole.find("=");
    if (pos!=OFString_npos)
    {
        path=whole.substr(0,pos);
        value=whole.substr(pos+1,value.length()-1);
    }
    else path=whole;
}


int MdfConsoleEngine::executeJob(const MdfJob &job)
// Date         : December, 4th, 2003
// Author       : Michael Onken
// Task         : executes given modify job
// Parameters   : job - [in] job to be executed
// Return Value : Returns 0 if no error occured, else the number of errors
{
    OFCondition result;
    int count=0; int error_count=0;
    if (verbose_option)
    {
         debugMsg(verbose_option, OFString("Executing (option|path|value): ")
            + job.option + "|" + job.path + "|" + job.value,"","");
    }
    //start modify operation based on job option
    if (job.option=="i")
        result=ds_man->modifyOrInsertTag(job.path, job.value, OFFalse, update_metaheader_uids_option);
    else if (job.option=="m")
        result=ds_man->modifyOrInsertTag(job.path, job.value, OFTrue, update_metaheader_uids_option);
    else if (job.option=="ma")
        result=ds_man->modifyAllTags(job.path, job.value, update_metaheader_uids_option, count);
    else if (job.option=="e")
        result=ds_man->deleteTag(job.path, OFFalse);
    else if (job.option=="ea")
        result=ds_man->deleteTag(job.path, OFTrue);
    else if (job.option=="gst")
        result=ds_man->generateNewStudyUID();
    else if (job.option=="gse")
        result=ds_man->generateNewSeriesUID();
    else if (job.option=="gin")
        result=ds_man->generateNewInstanceUID();

    //no valid job option found:
    else
    {
        error_count++;
        debugMsg(verbose_option, "error, no valid option: ", job.option, "");
    }
    //if modify operation failed
    if (result.bad() && error_count==0)
    {
        debugMsg(verbose_option,"Error modifying tag: ", result.text(),"");
        error_count++;
    }
    return error_count;
}


void MdfConsoleEngine::parseCommandLine()
// Date         : December, 4th, 2003
// Author       : Michael Onken
// Task         : parses commandline options into corresponding file- and
//                job lists and enables debug/verbose mode. The job list is
//                built in order of modify options on commandline
// Parameters   : none
// Return Value : none
{
    jobs=new OFList<MdfJob>;
    OFString option_string;
    cmd->gotoFirstOption();
    //iterate over commandline arguments from first to last
    do {
        if (cmd->getCurrentOption(option_string))
        {
            MdfJob aJob;
            OFString option_value, tag_path, tag_value;
            cmd->getValue(option_value);
            splitPathAndValue(option_value, tag_path, tag_value);
            //catch --verbose, --debug and --ignore-errors options
            if (option_string=="--verbose")
                verbose_option=OFTrue;
            else if (option_string=="--debug")
                debug_option=OFTrue;
            else if (option_string=="--ignore-errors")
                ignore_errors_option=OFTrue;
            else if (option_string=="--dont-update-metaheader-uids")
                update_metaheader_uids_option=OFFalse;
            //catch modify options
            else
            {
                if (option_string=="--insert-tag")
                    aJob.option="i";
                if (option_string=="--modify-tag")
                    aJob.option="m";
                if (option_string=="--erase-tag")
                    aJob.option="e";
                if (option_string=="--modify-all-tags")
                    aJob.option="ma";
                if (option_string=="--erase-all-tags")
                    aJob.option="ea";
                if (option_string=="--generate-study-uid")
                    aJob.option="gst";
                if (option_string=="--generate-series-uid")
                    aJob.option="gse";
                if (option_string=="--generate-instance-uid")
                    aJob.option="gin";
                //save to joblist
                aJob.path=tag_path;
                aJob.value=tag_value;
                jobs->push_back(aJob);
            }
        }
    } while (cmd->gotoNextOption());
    //if debug was set, verbose should also be enabled
    verbose_option=verbose_option || debug_option;
}


int MdfConsoleEngine::startProvidingService()
// Date         : October, 8th, 2003
// Task         : this function starts commandline parsing and then executes
//                found modify options
// Parameters   : none
// Return Value : Returns 0 if successful, another value if errors occurred
{
    OFCondition result;
    //return value of this function
    int errors=0;
    //just for better readability on console
    debugMsg(OFTrue,"\n","","");
    //parse command line into file and job list
    parseCommandLine();
    //iterators for job and file loops
    OFIterator<MdfJob> job_it;
    OFIterator<OFString> file_it;
    file_it=files->begin();
    //outer loop: iterate over all files
    for (unsigned int i=0; i<files->size(); i++)
    {
        result=loadFile((*file_it).c_str() );
        //if file could be loaded:
        if (result.good())
        {
            //for each file, set job iterator back to first entry
            job_it=jobs->begin();
            //inner loop: iterave over jobs, execute all jobs for current file
            for (unsigned int j=0; j<jobs->size(); j++)
            {
                errors+=executeJob(*job_it);
                job_it++;
            }
            //if there were no errors or user wants to override them, save:
            if (errors==0 || ignore_errors_option)
            {
                result=ds_man->saveFile( (*file_it).c_str());
                if (result.bad())
                {
                    debugMsg(OFTrue, "error: couldn't save file: ",
                        result.text(),"");
                    errors++;
                    result=restoreFile((*file_it).c_str());
                    if (result.bad())
                    {
                        debugMsg(OFTrue,
                            "error: couldnt restore file!","","");
                        errors++;
                    }
                }
            }
            //errors occured and user doesn't want to ignore them:
            else
            {
                result=restoreFile((*file_it).c_str());
                if (result.bad())
                {
                    debugMsg(OFTrue, "error: couldnt restore file!",
                    "","");
                    errors++;
                }
            }
        }
        //if loading fails:
        else
        {
            errors++;
            debugMsg(OFTrue,"error: unable to load file ",*file_it,
            "\n");
        }
        file_it++;
        debugMsg(verbose_option, "------------------------------------","","");
    }
    return errors;
}


OFCondition MdfConsoleEngine::loadFile(const char *filename)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : backup and loads file into internal MdfDatasetManager
// Parameters   : filename - [in] filename name of file to load
// Return Value : OFCondition, whether loading/backuping was successful
//                including error description
{
    OFCondition result;
    //free memory
    delete ds_man;
    ds_man = new MdfDatasetManager(debug_option);
    debugMsg(verbose_option,"Processing file: ", filename, "");
    //load file into dataset manager
    result=ds_man->loadFile(filename);
    if (result.good())
        result=backupFile(filename);
    return result;
}

OFCondition MdfConsoleEngine::backupFile(const char *file_name)
// Date         : October, 8th, 2003
// Author       : Michael Onken
// Task         : backup given file to file.bak
// Parameters   : file_name - [in] filename of file, that should be backuped
// Return Value : An OFCondition, whether backup was successful
{
    OFCondition backup_result;
    int result;
    OFString backup = file_name;
    backup+=".bak";
    //delete backup file, if it already exists:
    if ( OFStandard::fileExists(backup.c_str()) )
    {
        int del_result = remove(backup.c_str());
        if (del_result!=0)
        {
            backup_result=makeOFCondition(0,0,OF_error,
            "Couldn't delete previous backup file, unable to backup!\n");
        }
    }
    //if backup file could be removed:
    if (backup_result.good())
    {
        //backup original file!
        result=rename(file_name,backup.c_str());
        //set return value
        if (result!=0)
            backup_result=makeOFCondition(0,0,OF_error,"Unable to backup!\n");
    }
    return backup_result;
}


OFCondition MdfConsoleEngine::restoreFile(const char *filename)
// Date         : October, 8th, 2003
// Author       : Michael Onken
// Task         : restore "filename".bak to original without .bak
// Parameters   : filename - [in] filename of file, that should be restored
// Return Value : An OFCondition, whether restoring was successful
{
    int result;
    OFString backup = filename;
    backup+=".bak";
    //delete the (original) file, that dcmodify couldn't modify
    if ( OFStandard::fileExists(filename) )
    {
        result=remove(filename);
        if (result!=0)
        {
            debugMsg(OFTrue,
                "Unable to delete original filename for restoring backup!",
                "","");
            return EC_IllegalCall;
        }
    }
    //and rename backup file back to original filename:
    result=rename(backup.c_str(),filename);
    //error renaming backup file
    if (result!=0)
    {
        debugMsg(OFTrue,"Error renaming backup file to original","","");
        return EC_IllegalCall;
    }
    //successfully restored, throw out message:
    else
        debugMsg(verbose_option, "Renamed backup file to original", "", "");
    //you only get to this point, if restoring was completely successful
    return EC_Normal;
}


void MdfConsoleEngine::debugMsg(const OFBool &condition,
                                const OFString &s1,
                                const OFString &s2,
                                const OFString &s3)
// Date         : October, 15th, 2004
// Author       : Michael Onken
// Task         : The function handles three strings, that are directly printed
//                after another. The whole message is then terminated by \n
// Parameters   : condition - [in] message is printed, if condition is true
//                s1 - [in] first message string
//                s2 - [in] second message string
//                s3 - [in] third message string
// Return Value : none
{
    if (condition)
    {
        ofConsole.lockCerr() << s1 << s2 << s3 << endl;
        ofConsole.unlockCerr();
    }
}


MdfConsoleEngine::~MdfConsoleEngine()
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Destructor, delete dynamically allocated objects
// Parameters   : none
// Return Value : none
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
** Revision 1.11  2004-10-22 16:53:26  onken
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
