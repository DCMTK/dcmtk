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
 *  Purpose: Class for modifying DICOM-Files from comandline
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-11-13 10:34:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfconen.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
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

MdfConsoleEngine::MdfConsoleEngine(int argc, char *argv[],
                                   const char *application_name)
  : app(NULL), cmd(NULL), app_name(NULL), modify_tag_option(OFFalse),
    modify_item_tag_option(OFFalse), modify_all_tags_option(OFFalse),
    insert_item_tag_option(OFFalse), batch_mode_option(OFFalse),
    ask_option(OFFalse), insert_tag_option(OFFalse),erase_tag_option(OFFalse),
    erase_item_tag_option(OFFalse), erase_all_tags_option(OFFalse),
    verbose_option(OFFalse), debug_option(OFFalse), option_value(NULL),
    option_count(0)
// Date         : May 13th, 2003
// Author       : Michael Onken
// Task         : Constructor.
// Parameters   : argc - [in] number of command line arguments
//                argv - [in] array of command line arguments
//                application_name - [in] name of calling application
// Return Value : none
{
    //print application-header
    char rcsid[200];
    sprintf( rcsid, "$dcmtk: %s v%s %s $", application_name,
             OFFIS_DCMTK_VERSION, OFFIS_DCMTK_RELEASEDATE );

    //save application_name for further using
    if (application_name!=NULL)
        app_name=new OFString(application_name);
    else app_name=new OFString("");

    //current_file will iterate through the files
    const char* current_file=NULL;

    //the next lines describe command line arguments/options
    app=new OFConsoleApplication(application_name, "Modify DICOM files", rcsid);
    cmd=new OFCommandLine();

    cmd->setParamColumn(LONGCOL + SHORTCOL + 2);
    cmd->addParam("dcmfile-in", "DICOM input filename to be modified",
                  OFCmdParam::PM_MultiOptional);

    cmd->setOptionColumns(LONGCOL, SHORTCOL);
    //add options to commandline-application
    cmd->addGroup("general options:");
        cmd->addOption("--help",                 "-h",        "print this help text and exit");
        cmd->addOption("--version",                           "print version information and exit", OFTrue /* exclusive */);
        cmd->addOption("--debug",                "-d",        "debug mode, print debug information");
        cmd->addOption("--verbose",              "-v",        "verbose mode, print verbose output\n");

    cmd->addGroup("insert-mode options:");
        cmd->addOption("--insert-tag",       "-i",1, "\"xxxx,xxxx=value\"", "insert (and overwrite) given tag,\nat 1.Level denoted by gggg,eeee=value");
        cmd->addOption("--insert-item-tag", "-ii",1, "\"xxxx,xxxx[n].yyyy,yyyy=value\"", "insert (and overwrite) tag in sequence\ndenoted by gggg,eeee[n]=value \nn specifies item-nr(starts with 0)\ncan repeated for deeper-level-access\n");

    cmd->addGroup("modify-mode options:");
        cmd->addOption("--modify-tag",       "-m",1, "\"xxxx,xxxx=value\"", "modify given tag,\nat 1.Level denoted by gggg,eeee=value");
        cmd->addOption("--modify-item-tag", "-mi",1, "\"xxxx,xxxx[n].yyyy,yyyy=value\"", "modify tag in sequence\ndenoted by gggg,eeee[n]=value \nn specifies item-nr (starts with 0)\ncan repeated for deeper-level-access");
        cmd->addOption("--modify-all-tags", "-ma",1, "\"xxxx,xxxx=value\"", "modify ALL exisiting tags matching gggg,eeee in file\n");

    cmd->addGroup("erase-mode options:");
        cmd->addOption("--erase-tag",       "-e",1, "\"xxxx,xxxx\"", "erase given tag,\nat 1.Level denoted by gggg,eeee");
        cmd->addOption("--erase-item-tag",  "-ei",1, "\"xxxx,xxxx[n].yyyy,yyyy=value\"", "erases tag in sequence\ndenoted by gggg,eeee[n]=value \nn specifies item-nr (starts with 0)\ncan repeated for deeper-level-access");
        cmd->addOption("--erase-all-tags",  "-ea",1, "\"xxxx,xxxx\"", "erase ALL exisiting tags matching gggg,eeee in file\n ");

    cmd->addGroup("batch-mode options:");
        cmd->addOption("--batch-mode", "-b",1, "batch-file", "processes batch-file, that contains commandos for \nmodifying/inserting tags\nOne line consists of: filename commando tag value ",OFTrue);

    //evaluate command-line
    prepareCmdLineArgs(argc, argv, application_name);
    if (app->parseCommandLine(*cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
        //if no argument is given, print usage
        if (cmd->getArgCount() == 0)
        {
            app->printUsage();
        }

        //if no Parameter is given, check for --version and print info
        if (cmd->getParamCount() == 0)
        {
            if (cmd->findOption("--version"))
            {
                //next line uses ofConsole.lockCerr()
                app->printHeader(OFTrue /*print host identifier*/);
                CERR << endl << "External libraries used:";
#ifdef WITH_ZLIB
                CERR << endl << "- ZLIB, Version " << zlibVersion() << endl;
#else
                CERR << " none" << endl;
                exit(0);
#endif
            }
        }

        //iterate the files (parameters) and save them in files-OFList
        files=new OFList<const char*>;
        for (int i=1; i<=cmd->getParamCount(); i++)
        {
            cmd->getParam(i,current_file);
            files->push_back(current_file);
        }

        // check and enable debug/verbose-options
        if( cmd->findOption("--debug") )
        {
            debug_option=OFTrue;
            verbose_option=OFTrue;
            COUT << "DEBUG-mode enabled" << endl;
        }
        if( cmd->findOption("--verbose") )
        {
            verbose_option=OFTrue;
        }

        // make sure data dictionary is loaded
        if (!dcmDataDict.isDictionaryLoaded())
        {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
        }
    }

    // set option-flags in member-variables as found on command-line:
    if (cmd->findOption("--insert-tag"))
    {
        //save this option+value in class-variables
        insert_tag_option=OFTrue;
        //stores complete option-value in tag!
        cmd->getValue(option_value);
        option_count++;
    }
    if (cmd->findOption("--modify-tag"))
    {
        //save this option+value in class-variables
        modify_tag_option=OFTrue;
        cmd->getValue(option_value);
        //stores complete option-value in tag!
        option_count++;
    }
    if (cmd->findOption("--erase-tag"))
    {
        //save this option+value in class-variables
        erase_tag_option=OFTrue;
        //stores complete option-value in tag! should be "" for -e options!
        cmd->getValue(option_value);
        option_count++;
    }
    if (cmd->findOption("--erase-item-tag"))
    {
        //save this option+value in class-variables
        erase_item_tag_option=OFTrue;
        //stores complete option-value in tag! should be "" for -e options!
        cmd->getValue(option_value);
        option_count++;
    }
    if (cmd->findOption("--erase-all-tags"))
    {
        //save this option+value in class-variables
        erase_all_tags_option=OFTrue;
        //stores complete option-value in tag! should be "" for -e options!
        cmd->getValue(option_value);
        option_count++;
    }
    if (cmd->findOption("--modify-item-tag"))
    {
        //save this option+value in class-variables
        modify_item_tag_option=OFTrue;
        //stores complete option-value in tag!
        cmd->getValue(option_value);
        option_count++;
    }
    if (cmd->findOption("--modify-all-tags"))
    {
        //save this option+value in class-variables
        modify_all_tags_option=OFTrue;
        //stores complete option-value in tag!
        cmd->getValue(option_value);
        option_count++;
    }
    if (cmd->findOption("--insert-item-tag"))
    {
        //save this option+value in class-variables
        insert_item_tag_option=OFTrue;
        //stores complete option-value in tag!
        cmd->getValue(option_value);
        option_count++;
    }

    if (cmd->findOption("--batch-mode"))
    {
        //save this option+value in class-variables
        batch_mode_option=OFTrue;
        //stores complete option-value in tag!
        cmd->getValue(option_value);
        option_count++;
    }
}


int MdfConsoleEngine::readLine( FILE *fp, OFString &str )
// Date         : May 21, 2003
// Author       : Thomas Wilkens
// Task         : This function reads the next line (until '\n' or '\r')
//                from the open filestream fp and returns this line
//                (excluding '\n' and '\r') in str.
// Parameters   : fp - [in] Filepointer (must have been previously opened for
    //                reading with fopen())
//                str - [out] Line which was read, excluding '\n' and '\r'.
// Return Value : In case a string could be read and is returned, this function
//                will always return the number of characters in that string.
//                In case no string could be read (i.e. if an error occurred or
//                EOF was encountered), -1 is returned.
{
    const int array_size = 100;
    char *tmp, c;

    // check filepointer
    if( fp == NULL )
        return( -1 );

    // initialize out parameter
    str = "";

    // init char array
    tmp = new char[ array_size ];
    strcpy( tmp, "" );

    // read character by character
    unsigned int characters_copied = 0;
    c = fgetc( fp );
    while( c != EOF && c != '\r' && c != '\n' )
    {
        if( characters_copied == array_size - 1 )
        {
            // in case we already copied 99 characters into the tmp char array,
            // we need to complete the tmp char array with an EOS, attach the
            // string to the out parameter and reset the counter variable
            str += tmp;
            tmp[0] = c;
            tmp[1] = '\0';
            characters_copied = 1;
        }
        else
        {
            // in case there is still enough room for the current character in
            // tmp, copy it into the array, set EOS, and increase the counter
            // variable
            tmp[characters_copied] = c;
            tmp[characters_copied+1] = '\0';
            characters_copied++;
        }
    // get next character
    c = fgetc( fp );
    }

    // when we get to here, we encountered EOF, \r or \n;
    // in case we encountered \r we need to read another character (which
    // should be \n) to set the filepointer to a new line within the file.
    // (The files we are reading should be Windows Files, in case they are
    // UNIX files, we dont encounter \r, so nothing will happen here.)
    if( c == '\r' )
        c = fgetc( fp );

    // now append the current contents of the char array into the out parameter
    str += tmp;

    // free memory
    delete tmp;

    // return result
    if( c == EOF && str.length() == 0 )
        return( -1 );
    else
        return( str.length() );
}


int MdfConsoleEngine::startTagAction(DcmTagKey mod_key, char* tag_value)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : does real job of modifying/inserting an item-tag
// Parameters   : mod_key - [in] Tag to be modified
//                tag_value - [in] new tag-value for modify-option
// Return Value : An int, whether modify/insert was successful(=0) or not(!=0)
{
    int error_count=0;
    OFCondition backup_result;
    OFCondition result;
    int result2=0;
    //file_it will iterate through the files saved in a List
    OFListIterator(const char*) file_it;
    //begin with the first
    file_it=files->begin();
    //iterate over all files
    for (unsigned int i=0; i<files->size(); i++)
    {
        ds_man = new MdfDataSetManager(debug_option);
        if (verbose_option) COUT << "Processing File " << *file_it << endl;
        //load file into DataSetManager
        result=ds_man->loadFile(*file_it);
        //if file could be loaded
        if (result.good())
        {
            //backup file to .bak
            backup_result=backupFile(*file_it);
            //different calls for modify/insert/erase/modify-all
            if (modify_tag_option)
            {
                if (debug_option)
                {
                    ofConsole.lockCerr()
                        << "--modify-tag found: Starting action" << endl;
                    ofConsole.unlockCerr();
                }
                result=ds_man->modifyOrInsertTag(mod_key,tag_value,OFTrue);
            }
            if (insert_tag_option)
            {
                if (debug_option)
                {
                    ofConsole.lockCerr()
                        << "--insert-tag found: Starting action" << endl;
                    ofConsole.unlockCerr();
                }
                if (verbose_option)
                    printf("trying to insert %x,%x with value %s\n",
                        mod_key.getGroup(),mod_key.getElement(),tag_value);
                //OFFalse means here: insert tag if doesn't exist yet
                result=ds_man->modifyOrInsertTag(mod_key,tag_value, OFFalse);
            }
            if (erase_tag_option || erase_all_tags_option)
            {
                if (debug_option)
                {
                    ofConsole.lockCerr()
                        << "--erase-(all)-tag(s) found: Starting action"<< endl;
                    ofConsole.unlockCerr();
                }
                if (verbose_option)
                {
                    printf("trying to delete %x,%x\n",
                        mod_key.getGroup(),mod_key.getElement());
                }
                result=ds_man->deleteTag(mod_key,erase_all_tags_option);
            }
            if (modify_all_tags_option)
            {
                if (debug_option)
                {
                    ofConsole.lockCerr()
                        << "--modify-all-tags found: Starting action"  << endl;
                    ofConsole.unlockCerr();
                }
                if (verbose_option)
                    printf("trying to modify all tags %x,%x to %s\n",
                        mod_key.getGroup(),mod_key.getElement(), tag_value);
                result=ds_man->modifyAllTags(mod_key,tag_value,result2);
                if (verbose_option)
                    printf("Modified %i tags overall\n", result2);
            }
            //if modifying wasn't successful then restore original from backup
            if (result.bad())
            {
                //If an error occurs, throw out error message
                CERR << "Error modifying Tag in file "
                     << *file_it << ": " << result.text() << endl;
                error_count++;
                //and rename file back, if it was backuped before
                if (backup_result.good())
                {
                    OFCondition restore;
                    restore=restoreFile(*file_it);
                    if (restore.bad()) error_count++;
                 }
            }
            //else if we had success modifying tag, save file
            else
            {
                COUT << "Successfully processed tag in file: "
                     << *file_it << endl;
                result=ds_man->saveFile(*file_it);
                if (result.good() && verbose_option)
                    COUT << "Saved file to " << *file_it << endl;
                else if (result.bad())
                {
                    CERR << "Error saving file to "
                    << *file_it << ": " << result.text() << endl;
                    error_count++;
                }
            }
        }
        //else if file could not be loaded
        else
        {
            CERR << "Error loading file " << *file_it
                 << " : "<< result.text() << endl;
                 error_count++;
        }
        //and next file
        file_it++;
        COUT << "-----------------------------------------------------" << endl;
        delete ds_man;
    }
    return error_count;
}


int MdfConsoleEngine::startItemTagAction(char *tag_path, char *tag_value)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : does real job of inserting/modifying item-tags
// Parameters   : tag_path - [in] path to item-tag
//                tag_value - [in] (new) tag-value for item-tag
// Return Value : An Integer whether modifying was successful(0) or not (!=0)
{
    int error_count=0;
    OFCondition result, backup_result;
    OFString backup;
    //file_it will iterate through the files saved in a list
    OFListIterator(const char*) file_it;
    //begin with the first
    file_it=files->begin();

    for (unsigned int i=0; i<files->size(); i++)
    {
        ds_man=new MdfDataSetManager(debug_option);
        //load -> backup -> modify -> save:
        backup=*file_it;
        backup+=".bak";
        result=ds_man->loadFile(*file_it);
        if (result.good())
        {
            //backup file to .bak
            backup_result=backupFile(*file_it);
            //all modify-item-tag-options excluding delete:
            if (modify_item_tag_option || insert_item_tag_option)
                result=ds_man->modifyOrInsertItemTag(tag_path,tag_value,
                                                     modify_item_tag_option);
            //else erase-item-tag was chosen
            else
                result=ds_man->deleteItemTag(tag_path);
            if (result.good())
            {
                COUT << "Successfully processed item-tag "
                     << "in file: "  << *file_it << endl;
                result=ds_man->saveFile(*file_it);
                if (result.good() && verbose_option)
                    COUT << "Saved file to: " << *file_it << endl;
                else if (result.bad())
                {
                    CERR << "Error saving file: " << result.text() << endl;
                    error_count++;
                }
            }
            else
            {
                //print error-message
                CERR << "Error modifying tag in file " << *file_it
                     << ": " << result.text() << endl;
                error_count++;
                //and rename file back, if it was backuped before
                if (backup_result.good())
                {
                    OFCondition restore;
                    restore=restoreFile(*file_it);
                    if (restore.bad()) error_count++;
                 }
            }
        }
        else
        {
            CERR << "Error loading file " << *file_it << endl;
            error_count++;
        }
        delete ds_man;
        file_it++;
        COUT << "-----------------------------------------------------" << endl;
    }
    return error_count;
}


int MdfConsoleEngine::delegateTagAction()
// Date         : August, 20th, 2003
// Author       : Michael Onken
// Task         : organizes data for startTagAction and starts it
// Return Value : An Integer whether modifying was successful(0) or not (!=0)
{
    //return-value of this function
    int error_count=0;
    int result;
    //group and elem numbers for searchkey
    unsigned int group = 0xffff;
    unsigned int elem = 0xffff;
    //key to be changed
    DcmTagKey search_key(group,elem);
    const char *temp;
    char* tag_value = new char[strlen(option_value)+1];
    //parse option_value to get group,elem and new value
    result=sscanf(option_value, "%x,%x", &group, &elem);
    //if the optionvalue could be parsed and the value is at least 10 chars
    if (result==2)
        //generate key from parsed information
        search_key.set(group, elem);
    //else we have an error
    else
    {
        CERR << "Can not parse tag!" << endl;
         error_count++;
    }
    //now get the new tag-value, if erase is set, there is none
    if (!erase_tag_option && !erase_all_tags_option)
    {
        temp=strpbrk(option_value,"=");
        //if the user didn't give tag-value, assume "" as default
        if (temp==NULL)
        {
            tag_value="";

        }
        //if tag-value is given, copy it to member-variable
        else
        {
            //Get rid of "="
            temp++;
            //And copy the rest in temp (the value we need!) to member-variable
            OFStandard::strlcpy(tag_value,temp,strlen(temp)+1);
        }
    }
    //if no general errors occured above, start the tag action
    if (error_count==0)
    {
        error_count+=startTagAction(search_key,tag_value);
    }
    return error_count;
}


int MdfConsoleEngine::delegateItemTagAction()
// Date         : August, 20th, 2003
// Author       : Michael Onken
// Task         : organizes data for startItemTagAction and starts it
// Return Value : An Integer whether modifying was successful(0) or not (!=0)
{
    int error_count = 0;
    const char *temp;
    //tag_value should hold the new value for the tag, tag_path the path
    //they can not be longer than the complete option+1
    char *tag_path = new char[strlen(option_value)+1];
    char *tag_value = new char[strlen(option_value)+1];
    temp=strpbrk(option_value,"=");
    //temp is NULL if erase-item-tag-option is given, (no "=" can be found)
    if (temp!=NULL)
    {
        //Everything before "=" is the tag_path
        OFStandard::strlcpy(tag_path,option_value,
            strlen(option_value)-strlen(temp)+1);
        //finish string with \0
        tag_path[strlen(option_value)-strlen(temp)+1] = '\0';
        //Get rid of "=" in temp and we've got the tag_value
        temp++;
        //And copy the rest from temp to member-variable tag_value...ready
        OFStandard::strlcpy(tag_value,temp,strlen(temp)+1);
    }
    else
    {
        //no "=" is given, so assume whole option-value as tag-path
        tag_path=(char*)option_value;
        //and initalize tag-value as empty
        tag_value="";
    }
    error_count+=startItemTagAction(tag_path, tag_value);
    //check for errors and print out on console
    if (modify_item_tag_option==OFTrue)
        if (error_count!=0) CERR << "There were " << error_count
                                 << " error(s) while modifying!" << endl;
    if  (insert_item_tag_option==OFTrue)
        if (error_count==0) COUT << "Successfully inserted Item-Tag: "
                                 << tag_path << " to " << tag_value << endl;
        else CERR << "There were " << error_count
                  << " error(s) while inserting!" << endl;
    if  (erase_item_tag_option==OFTrue)
        if (error_count==0) COUT << "Successfully deleted Item-Tag: "
                                 << tag_path << endl;
        else CERR << "There were " << error_count
                  << " error(s) while inserting!" << endl;
    delete tag_path;
    delete tag_value;
    return error_count;
}


int MdfConsoleEngine::delegateBatchMode()
// Date         : August, 20th, 2003
// Author       : Michael Onken
// Task         : reads operation-options from batch-file and processes them
//                as possible
// Return Value : An Integer whether modifying was successful(0) or not (!=0)
{
    int error_count=0;
    if (debug_option)
    {
        ofConsole.lockCerr() << "Entered batchmode" << endl;
        ofConsole.unlockCerr();
    }
    unsigned int group = 0xffff;
    unsigned int elem = 0xffff;
    //key to be changed
    DcmTagKey search_key(group,elem);
    FILE *batch_file;
    //stores actual line-nr
    int line_count=0;
    files->clear();
    batch_file=fopen(option_value, "r");
    //a filename should not be longer than 1025 chars
    char* line=new char[1025];
    //iterate from line to line
    OFString tag_value;
    //rest2parse will walk through a single line
    OFString rest2parse;
    OFString tag_path;
    size_t pos=0;
    OFString test;
    if (debug_option)
    {
        ofConsole.lockCerr() << "Starting to read lines from batchfile: "
                             << option_value << endl;
        ofConsole.unlockCerr();
    }
    while( readLine( batch_file, rest2parse ) != -1 )
    {
        if (debug_option)
        {
            ofConsole.lockCerr() << "Whole read line is:"
                                 << rest2parse << endl;
            ofConsole.unlockCerr();
        }
        line_count++;
        if (rest2parse.length()!=0 && rest2parse.at(0)!='#' &&
            rest2parse.at(0)!=' ')
        {
            //---------------------read filename----------------------------
            pos=rest2parse.find(" ",0);
            OFString file_name=rest2parse.substr(0,pos);
            if (debug_option)
            {
                ofConsole.lockCerr() << "Found filename: "
                                     << file_name << endl;
                ofConsole.unlockCerr();
            }
            files->push_back(file_name.c_str());
            rest2parse.erase(0,pos+1);
            pos=0;
            //---------------------read option-------------------------------
            if (rest2parse.substr(0,2)=="i ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found insert-tag option" << endl;
                    ofConsole.unlockCerr();
                }
                insert_tag_option=OFTrue;
                rest2parse.erase(0,2);
            }
            if (rest2parse.substr(0,3)=="ii ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found insert-item-tag option"
                                         << endl;
                    ofConsole.unlockCerr();
                }
                insert_item_tag_option=OFTrue;
                rest2parse.erase(0,3);
            }
            if (rest2parse.substr(0,2)=="m ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found modify-tag option" << endl;
                    ofConsole.unlockCerr();
                }
                modify_tag_option=OFTrue;
                rest2parse.erase(0,2);
            }
            if (rest2parse.substr(0,3)=="mi ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found modify-item-tag option"
                                         << endl;
                    ofConsole.unlockCerr();
                }
                modify_item_tag_option=OFTrue;
                rest2parse.erase(0,3);
            }
            if (rest2parse.substr(0,3)=="ma ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found modify-all-tags option"
                                         << endl;
                    ofConsole.unlockCerr();
                }
                modify_all_tags_option=OFTrue;
                rest2parse.erase(0,3);
            }
            if (rest2parse.substr(0,2)=="e ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found erase-tag option" << endl;
                    ofConsole.unlockCerr();
                }
                erase_tag_option=OFTrue;
                rest2parse.erase(0,2);
            }
            if (rest2parse.substr(0,3)=="ei ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found erase-item-tag option"
                                         << endl;
                    ofConsole.unlockCerr();
                }
                erase_item_tag_option=OFTrue;
                rest2parse.erase(0,3);
            }

            if (rest2parse.substr(0,3)=="ea ")
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "Found erase-all-tags option"
                                         << endl;
                    ofConsole.unlockCerr();
                }
                erase_all_tags_option=OFTrue;
                rest2parse.erase(0,3);
            }
            //------------------read tag-path and value----------------------
            //if erase is given, we wouldnt need to look for a tag_value;
            //Because the erase-implementation doesnt look whats given
            //in tag_value, we just read it here, as if it was there.
            //So for erase-options tag_value will be read here as ""
            pos=rest2parse.find('=');
            tag_path=rest2parse.substr(0,pos);
            //cut off whole tag_path including '='
            rest2parse.erase(0,pos+1);
            //rest is tag_value
            tag_value=rest2parse.substr(0,rest2parse.size());
            if (debug_option)
            {
                ofConsole.lockCerr() << "tag-path is: "
                                     << tag_path << endl;
                ofConsole.unlockCerr();
            }

            //------------------delegate tag-actions-------------------------
            if (modify_tag_option || insert_tag_option
               || erase_tag_option || erase_all_tags_option
               || modify_all_tags_option)
            {
                //parse search_key from tag_path
                sscanf(tag_path.c_str(),"%x,%x", &group,&elem);
                search_key.setGroup(group);
                search_key.setElement(elem);
                if (debug_option)
                {
                    ofConsole.lockCerr() << "search-key is: "
                                         << hex << search_key.getGroup() << ","
                                         << hex << search_key.getElement()
                                         << endl;
                    ofConsole.unlockCerr();
                }
                char* tag=(char*)tag_value.c_str();
                if (debug_option)
                {
                    ofConsole.lockCerr() << "starting tag-action: " << endl;
                    ofConsole.unlockCerr();
                }
                error_count+=startTagAction(search_key,tag);

            }
            if (modify_item_tag_option || insert_item_tag_option
               || erase_item_tag_option)
            {
                if (debug_option)
                {
                    ofConsole.lockCerr() << "starting item-tag-action: "
                                         << endl;
                    ofConsole.unlockCerr();
                }
                error_count+=startItemTagAction((char*)tag_path.c_str(),
                                                (char*)tag_value.c_str());
            }
        } else if (verbose_option)
        {
            if (verbose_option)
            {   printf("Found comment or empty line, jump to next one\n");
                printf("---------------------------------------------------\n");
            }
        }

        //reset option-flags because in -b-mode other options may follow
        modify_tag_option=OFFalse;
        insert_tag_option=OFFalse;
        modify_item_tag_option=OFFalse;
        modify_all_tags_option=OFFalse;
        insert_item_tag_option=OFFalse;
        erase_tag_option=OFFalse;
        erase_item_tag_option=OFFalse;
        erase_all_tags_option=OFFalse;
        files->clear();
    }
    fclose(batch_file);
    delete line;
    return error_count;
}


int MdfConsoleEngine::startProvidingService()
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Function that starts the right modify/insert functions
// Parameters   : none
// Return Value : An Integer whether service could be provided succ. or not
{
    //return-value of this function
    int error_count=0;
    //if more than one "real" option is given, print error and exit
    if (option_count>1)
    {
        app->printHeader(OFTrue /*print host identifier*/);
        CERR << "Please use just one tag-action at a time. If you want to do "
             << "more  than one tag-action, use -b (batchmode) instead!"<<endl;
        error_count++;
    }
    //if not batchmode is given, there must be files given on command-line
    if (files->empty()==OFTrue && batch_mode_option==OFFalse)
    {
        CERR << "No dicom files given!" << endl;
        //if no files are given return 1;
        error_count++;
    }
    //just for better readability, this is the first output-line on console
    COUT << endl;

    //if there are no general errors were detected above, modifies are started
    if (error_count == 0)
    {
        //if option_value is empty, set it to ""
        if (option_value==NULL) option_value="";
        //if simple tag-options are given (no sequences/items)
        if (modify_tag_option==OFTrue || insert_tag_option==OFTrue
        || modify_all_tags_option==OFTrue || erase_tag_option
        || erase_all_tags_option)
        {
            error_count+=delegateTagAction();
        }
        //if an item-tag options is given
         if (modify_item_tag_option==OFTrue || insert_item_tag_option==OFTrue
            || erase_item_tag_option==OFTrue)
        {
            error_count+=delegateItemTagAction();
        }

        //if batch_mode_option is set
        if (batch_mode_option==OFTrue)
        {
            error_count+=delegateBatchMode();
        }
    }
    return error_count;
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
    //delete backup-file, if it already exists
    if ( OFStandard::fileExists(backup.c_str()) )
    {
        int del_result = remove(backup.c_str());
        if (del_result!=0)
        {
            backup_result=makeOFCondition(0,0,OF_error,
            "Couldn't delete previous backup file, unable to backup!\n");
        }
    }
    //if backup-file could be removed:
    if (backup_result.good())
    {
        //backup original file!
        result=rename(file_name,backup.c_str());
        //set return-value
        if (result==0)
            backup_result=makeOFCondition(0,0,OF_ok,"Backup successful\n");
        else
            backup_result=makeOFCondition(0,0,OF_error,"Unable to backup!\n");
    }
    if (verbose_option)
    {
        COUT << "Tried to backup " << file_name
             << " to " << file_name << ".bak: "
             << backup_result.text();
    }
    return backup_result;
}

OFCondition MdfConsoleEngine::restoreFile(const char *file_name)
// Date         : October, 8th, 2003
// Author       : Michael Onken
// Task         : restore given file from file.bak to original (without .bak)
// Parameters   : file_name - [in] filename of file, that should be restored
// Return Value : An OFCondition, whether restoring was successful
{
    OFCondition restore_result;
    int result, del_result;
    OFString backup = file_name;
    backup+=".bak";
    //delete the (original) file, that dcmodify couldn't modify
    if ( OFStandard::fileExists(file_name) )
    {
        del_result=remove(file_name);
        if (del_result!=0)
            makeOFCondition(0,0,OF_error,"Unable to delete working-file!\n");
    }
    //and rename backup-file back to original file-name
    result=rename(backup.c_str(),file_name);
    if ( result!=0)
    {
        restore_result=makeOFCondition(0,0,OF_error,
            "Error renaming backup-filename to original\n");
    }
    else
    {
        restore_result=makeOFCondition(0,0,OF_ok,
            "Successfully renamed backup-file to original\n");
    }
    //throw out messages
    if (restore_result.bad())
    {
        CERR << "Couldnt restore " << backup << " to file "
             << file_name << ": " << restore_result.text() << endl;
    }
    else if (verbose_option)
        COUT << restore_result.text();
    //and return result of restore-operation
    return restore_result;
}

MdfConsoleEngine::~MdfConsoleEngine()
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Destructor, delete dynamically allocated objects
// Parameters   : none
// Return Value : none
{
    if (debug_option)
    {
        ofConsole.lockCerr() << "Deleting member-variables from memory" << endl;
        ofConsole.unlockCerr();
    }
    delete app_name;
    delete app;
    delete cmd;
    delete files;
}

/*
** CVS/RCS Log:
** $Log: mdfconen.cc,v $
** Revision 1.8  2003-11-13 10:34:22  joergr
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
