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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-07-09 12:13:13 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfconen.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "mdfconen.h"
#include "oftypes.h"
#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"       /* for dcmtk version name */
#include "oflist.h"
#include "ofstring.h"

#define SHORTCOL 3
#define LONGCOL 17
 
#ifdef WITH_ZLIB
BEGIN_EXTERN_C
#include <zlib.h>
END_EXTERN_C
#endif

// Date         : May 13th, 2003
// Author       : Michael Onken
// Task         : Constructor.
// Parameters   : argc [in] number of command line arguments
//                argv [in] array of command line arguments
//                application_name [in] name of calling application
// Return Value : none
MdfConsoleEngine::MdfConsoleEngine(int argc, char *argv[], const char *application_name)
  : app(NULL), cmd(NULL), app_name(NULL), modify_tag_option(OFFalse),
    modify_item_tag_option(OFFalse), modify_all_tags_option(OFFalse),
    insert_item_tag_option(OFFalse), batch_mode_option(OFFalse), 
    ask_option(OFFalse), insert_tag_option(OFFalse),erase_tag_option(OFFalse), 
    erase_item_tag_option(OFFalse), erase_all_tags_option(OFFalse), 
    verbose_option(OFFalse), option_value(NULL)
{   
    //print application-header
    char rcsid[200];
    sprintf( rcsid, "$dcmtk: %s v%s %s $", application_name, 
             OFFIS_DCMTK_VERSION, OFFIS_DCMTK_RELEASEDATE );    
    
    //number of "real" options like -m, -i exclusive -v and -d
    int option_count=0;
    
    //save application_name for further using
    app_name=new char[strlen(application_name)+1];
    strcpy(app_name,application_name);
    
    //current_file will iterate through the files
    const char* current_file=NULL;

    //the next lines describe command line arguments/options
    app=new OFConsoleApplication(application_name, "Modify DICOM-Files", rcsid);
    cmd=new OFCommandLine();

    cmd->setParamColumn(LONGCOL + SHORTCOL + 2);
    cmd->addParam("dcmfile-in", "DICOM input filenames to be modified", 
                  OFCmdParam::PM_MultiOptional);

    cmd->setOptionColumns(LONGCOL, SHORTCOL);
    cmd->addGroup("general options:");
        cmd->addOption("--help",                 "-h",        "print this help text and exit");
        cmd->addOption("--version",                           "print version information and exit", OFTrue /* exclusive */);
        cmd->addOption("--debug",                "-d",        "debug mode, print debug information");
        cmd->addOption("--verbose",              "-v",        "verbose mode, print verbose output\n");
    
    cmd->addGroup("insert-mode options:");
        cmd->addOption("--insert-tag",       "-i",1, "\"xxxx,xxxx=value\"", "insert given tag,\nat 1.Level denoted by gggg,eeee=value");
        cmd->addOption("--insert-item-tag", "-ii",1, "\"xxxx,xxxx[n].yyyy,yyyy=value\"", "insert tag in sequences\ndenoted by gggg,eeee[n]=value \nn specifies item-nr(starts with 0)\ncan repeated for deeper-level-access\n");

    cmd->addGroup("modify-mode options:");
        cmd->addOption("--modify-tag",       "-m",1, "\"xxxx,xxxx=value\"", "modify given tag,\nat 1.Level denoted by gggg,eeee=value");
        cmd->addOption("--modify-item-tag", "-mi",1, "\"xxxx,xxxx[n].yyyy,yyyy=value\"", "modify tag in sequences\ndenoted by gggg,eeee[n]=value \nn specifies item-nr (starts with 0)\ncan repeated for deeper-level-access");
        cmd->addOption("--modify-all-tags", "-ma",1, "\"xxxx,xxxx=value\"", "modify ALL exisiting tags matching gggg,eeee in file\n");

    cmd->addGroup("erase-mode options:");        
        cmd->addOption("--erase-tag",       "-e",1, "\"xxxx,xxxx\"", "erase given tag,\nat 1.Level denoted by gggg,eeee");  
        cmd->addOption("--erase-item-tag",  "-ei",1, "\"xxxx,xxxx[n].yyyy,yyyy=value\"", "erases tag in sequences\ndenoted by gggg,eeee[n]=value \nn specifies item-nr (starts with 0)\ncan repeated for deeper-level-access");     
        cmd->addOption("--erase-all-tags",  "-ea",1, "\"xxxx,xxxx\"", "erase ALL exisiting tags matching gggg,eeee in file\n ");    

    cmd->addGroup("batch-mode options:");
        cmd->addOption("--batch-mode", "-b",1, "batch-file", "processes batch-file, that contains commandos for \nmodifying/inserting tags\nOne line consists of: filename commando tag value ",OFTrue);
  
    //evaluate command-line
    prepareCmdLineArgs(argc, argv, application_name);
    if (app->parseCommandLine(*cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      //check whether there is given something at commandline
      if (cmd->getArgCount() == 0)
      {
        app->printUsage();
      }

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

      // options
      if( cmd->findOption("--debug") )
      {
        SetDebugLevel(3);
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
    
    // set option-flags as found on command-line:
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
        cmd->getValue(option_value);   //stores complete option-value in tag!
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
    //if more than one "real" option is given, print error and exit
    if (option_count>1)
    {
        app->printHeader(OFTrue /*print host identifier*/);
        CERR << "Please use just one tag-action at a time. If you want to do "
             << "more  than one tag-action, use -b (batchmode) instead!"<<endl;
        exit(1);
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


// Date         : May 21, 2003
// Author       : Thomas Wilkens
// Task         : This function reads the next line (until '\n' or '\r') from the open
//                filestream fp and returns this line (excluding '\n' and '\r') in str.
// Parameters   : fp - [in] Filepointer (must have been previously opened for reading with fopen())
//                str - [out] Line which was read, excluding '\n' and '\r'.
// Return Value : In case a string could be read and is returned, this function will always return
//                the number of characters in that string. In case no string could be read (i.e. if
//                an error occurred or EOF was encountered), -1 is returned.
int MdfConsoleEngine::readLine( FILE *fp, OFString &str )
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
      // in case there is still enough room for the current character in tmp,
      // copy it into the array, set EOS, and increase the counter variable
      tmp[ characters_copied ] = c;
      tmp[ characters_copied + 1 ] = '\0';
      characters_copied++;
    }

    // get next character
    c = fgetc( fp );
  }

  // when we get to here, we encountered EOF, \r or \n; in case we encountered \r
  // we need to read another character (which should be \n) to set the filepointer
  // to a new line within the file. (The files we are reading should be Windows Files,
  // in case they are UNIX files, we dont encounter \r, so nothing will happen here.)
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

// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : does real job of modifying/inserting an item-tag
// Parameters   : mod_key: [in] Tag to be modified
//                tag_value: [in] new tag-value for modify-option
// Return Value : An int, whether modify/insert was successfull(=0) or not(!=0)
int MdfConsoleEngine::startTagAction(DcmTagKey mod_key, char* tag_value)
{
    int error_count=0;
    OFString backup;
    OFCondition result;
    int result2=0;
    //file_it will iterate through the files saved in a List
    OFListIterator(const char*) file_it;
    //begin with the first...suprise;)
    file_it=files->begin();
    
    for (unsigned int i=0; i<files->size(); i++)
    {           
        ds_man=new MdfDataSetManager("Console Engine");
        if (verbose_option) COUT << "Processing File " << *file_it << endl;
        //load -> backup -> modify -> save:

        backup=*file_it;
        backup+=".bak";
        result=ds_man->loadFile(*file_it);
        //backup original file!
        result2=rename(*file_it,backup.c_str());
        if (result2==0 && verbose_option) COUT << *file_it << " was backuped to "
                             << backup.c_str() << endl;
        else if (verbose_option && result2>0)
            CERR << "Not able to backup " << *file_it << endl;          
        //if file could be loaded
        if (result.good())
        {    
            //different calls for modify/insert/erase/modify-all
            if (modify_tag_option)
            {    
                debug(3,("--modify-tag found: Starting action\n"));
                result=ds_man->modifyTag(mod_key,tag_value);
            }    
            if (insert_tag_option)
            {   
                debug(3,("--insert-tag found: Starting action\n"));
                if (verbose_option) 
                    printf("trying to insert %x,%x with value %s\n",
                        mod_key.getGroup(),mod_key.getElement(),tag_value);
                result=ds_man->modifyOrInsertTag(mod_key,tag_value);
            }   
            if (erase_tag_option || erase_all_tags_option)
            {
                debug(3,("--erase-(all)-tag(s) found: Starting action\n"));
                if (verbose_option) 
                    printf("trying to delete %x,%x\n",
                        mod_key.getGroup(),mod_key.getElement());
                result=ds_man->deleteTag(mod_key,erase_all_tags_option);
            }   
            if (modify_all_tags_option)
            {
                debug(3,("--modify-all-tags found: Starting action\n"));
                if (verbose_option) 
                    printf("trying to modify all tags %x,%x to %s\n",
                        mod_key.getGroup(),mod_key.getElement(), tag_value);
                result=ds_man->modifyAllTags(mod_key,tag_value,result2);
                if (verbose_option)
                    printf("Modified %i tags overall\n", result2);
            }   
            if (result.bad())   
            {   
                //If an error occurs, throw out error message
                CERR << "Error modifying Tag in file " 
                     << *file_it << ": " << result.text() << endl;
                error_count++;
                //and rename file back!
                result2=rename(backup.c_str(),*file_it);
                if (result2!=0 && verbose_option) CERR << "Failed to rename " 
                                     << backup.c_str() << " back to original"
                                     << endl;
                else debug(3,("%s renamed to original filename",backup.c_str()));

            }
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
        else
        {   
            CERR << "Error loading file " << *file_it 
                 << " : "<< result.text() << endl;
                 error_count++;
        }        
        delete ds_man;
        //and next file
        file_it++;
        COUT << "-----------------------------------------------------" << endl;
    }   
    return error_count;
}   
    

// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : does real job of inserting/modifying item-tags
// Parameters   : tag_path [in] path to item-tag
//                tag_value [in] (new) tag-value for item-tag
// Return Value : An Integer whether modifying was successfull(0) or not (!=0)
int MdfConsoleEngine::startItemTagAction(char *tag_path, char *tag_value)
{
    int error_count=0;
    OFCondition result;
    int result2;
    OFString backup;
            
    //file_it will iterate through the files saved in a List
    OFListIterator(const char*) file_it;
    //begin with the first...suprise;)
    file_it=files->begin();
    

    for (unsigned int i=0; i<files->size(); i++)
    {           
        ds_man=new MdfDataSetManager("Console Engine");
        //load -> backup -> modify -> save:
        backup=*file_it;
        backup+=".bak";
        result=ds_man->loadFile(*file_it);
        //backup original file!
        result2=rename(*file_it,backup.c_str());
        if (result2==0 && verbose_option) COUT << *file_it <<" was backuped to "
                             << backup.c_str() << endl;
        else if (verbose_option)
            CERR << "Not able to backup " << *file_it << endl;          
        if (result.good())
        {
            if (modify_item_tag_option || insert_item_tag_option)
                result=ds_man->modifyOrInsertItemTag(tag_path,tag_value,
                                                     OFTrue);
            else //erase-item-tag is chosen
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
                CERR << "Error modifying tag in file: " << *file_it 
                     << ": " << result.text() << endl;
                //and rename file back!
                result2=rename(backup.c_str(),*file_it);
                if (result2!=0 && verbose_option) CERR << "Failed to rename " 
                                     << backup.c_str() << " back to original"
                                     << endl;
                else if (verbose_option) 
                    COUT << backup.c_str() << " renamed to original filename"
                         << endl;                     
                error_count++;
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
        



// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Function that starts the right modify/insert functions
// Parameters   : none
// Return Value : An Integer whether service could be provided succ. or not
int MdfConsoleEngine::startProvidingService()
{   
    //return-value of this function
    int error_count=0;
    //group and elem numbers for searchkey
    unsigned int group = 0xffff;
    unsigned int elem = 0xffff;
    //key to be changed
    DcmTagKey search_key(group,elem);
    OFCondition result;
    OFString backup;
    int result2;
    
    //just for better readability, this is the first output-line on console
    COUT << endl;

    //tag_value should hold the new value for the tag, tag_path the path
    //they can not be longer than the complete option+1
    if (option_value==NULL) option_value="";
    char* tag_value = new char[strlen(option_value)+1];
    //if simple tag-options are given (no sequences/items)
    if (modify_tag_option==OFTrue || insert_tag_option==OFTrue 
    || modify_all_tags_option==OFTrue || erase_tag_option 
    || erase_all_tags_option)
    {
        if (files->empty()==OFTrue)
        {   CERR << "No dicom files given!" << endl;
            return 1;
        }   
        //tag_value should hold the new value for the tag
        //it could not be longer than the complete option+1
        char *tag_value = new char[strlen(option_value)+1];
        //parse option_value to get group,elem and new value
        result2=sscanf(option_value, "%x,%x", &group, &elem); 
        //if the optionvalue could be parsed and the value is at least 10 chars
        if (result2==2)
            //generate key from parsed information
            search_key.set(group, elem);
            
        //else we have an error
        else
        {
            CERR << "Can not parse tag!" << endl;
            delete tag_value;
            return 1;
        }   

        //now get the new tag-value, if erase is set, this is not a good idea...
        if (!erase_tag_option && !erase_all_tags_option) 
        {
            char *temp=strpbrk(option_value,"="); 
            if (temp==NULL)
            {   
                CERR << "Wrong option-format! " << endl;
                return 1;
            }   
            //Get rid of "=" 
            temp++; 
            //And copy the rest in temp (the value we need!) to member-variable 
            strcpy(tag_value,temp);
        }
        error_count=startTagAction(search_key,tag_value);
        delete tag_value;
        return error_count;
    }   
    //if an item-tag options is given
     if (modify_item_tag_option==OFTrue || insert_item_tag_option==OFTrue
        || erase_item_tag_option==OFTrue)
    {   
        if (files->empty()==OFTrue)
        {   CERR << "No dicom files given!" << endl;
            return 1;
        }   
        char *tag_path =new char[strlen(option_value)+1];
        char *temp=strpbrk(option_value,"=");
        //temp is NULL if erase-item-tag-option is given, (no = can be found)
        if (temp!=NULL)
        {
            //Everything before "=" is the tag_path
            strncpy(tag_path,option_value,strlen(option_value)-strlen(temp));
            //Get rid of "=" in temp and we've got the tag_value
            temp++; 
            //And copy the rest from temp to member-variable tag_value...ready
            strcpy(tag_value,temp);
        }
        else tag_path=(char*)option_value;    
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
    
    //if batch_mode_option is set
    if (batch_mode_option==OFTrue)
    {   
        //rest2parse will walk through a single line
        //char* rest2parse;
        //a filename should not be longer than 1026 (?!)
        debug(3,("Entered batchmode\n"));
        FILE *batch_file;
        //stores actual line-nr
        int line_count=0;
        files->clear();
        batch_file=fopen(option_value, "r");
        char* line=new char[1025]; 
        //iterate from line to line
        OFString tag_value;
        OFString rest2parse;
        OFString tag_path;
        size_t pos=0;
        OFString test;
        ifstream file(option_value);
        debug(3,("Starting to read lines from batchfile:%s",option_value));
        while( readLine( batch_file, rest2parse ) != -1 )
        {   
            debug(3,("Whole read line is:%s",rest2parse.c_str()));
            line_count++;
            if (rest2parse.length()!=0 && rest2parse.at(0)!='#' && 
                rest2parse.at(0)!=' ')
            {
                //---------------------read filename----------------------------
                pos=rest2parse.find(" ",0);
                OFString file_name=rest2parse.substr(0,pos);
                debug(3,("Found filename:%s",file_name.c_str()));
                files->push_back(file_name.c_str());
                rest2parse.erase(0,pos+1);
                pos=0;
                //---------------------read option-------------------------------
                if (rest2parse.substr(0,2)=="i ") 
                {
                    debug(3,("Found insert-tag option"));
                    insert_tag_option=OFTrue;
                    rest2parse.erase(0,2);
                }   
                if (rest2parse.substr(0,3)=="ii ") 
                {
                    debug(3,("Found insert-item-tag option"));
                    insert_item_tag_option=OFTrue;
                    rest2parse.erase(0,3);
                }   
                if (rest2parse.substr(0,2)=="m ") 
                {
                    debug(3,("Found modify-tag option"));
                    modify_tag_option=OFTrue;
                    rest2parse.erase(0,2);
                }                   
                if (rest2parse.substr(0,3)=="mi ") 
                {
                    debug(3,("Found modify-item-tag option"));
                    modify_item_tag_option=OFTrue;
                    rest2parse.erase(0,3);
                }                   
                if (rest2parse.substr(0,3)=="ma ") 
                {
                    debug(3,("Found modify-all-tags option"));
                    modify_all_tags_option=OFTrue;
                    rest2parse.erase(0,3);
                }   
                if (rest2parse.substr(0,2)=="e ") 
                {
                    debug(3,("Found erase-tag option"));
                    erase_tag_option=OFTrue;
                    rest2parse.erase(0,2);
                }                   
                if (rest2parse.substr(0,3)=="ei ") 
                {
                    debug(3,("Found erase-item-tag option"));
                    erase_item_tag_option=OFTrue;
                    rest2parse.erase(0,3);
                }   
    
                if (rest2parse.substr(0,3)=="ea ") 
                {
                    debug(3,("Found erase-all-tags-option"));
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
                debug(3,("tag-path is:%s",tag_path.c_str()));
                debug(3,("tag-value is:%s",tag_value.c_str()));
                //------------------delegate tag-actions-------------------------
                if (modify_tag_option || insert_tag_option 
                   || erase_tag_option || erase_all_tags_option
                   || modify_all_tags_option)
                {
                    //parse search_key from tag_path
                    int group=0xffff;
                    int elem=0xffff;
                    sscanf(tag_path.c_str(),"%x,%x", &group,&elem);
                    search_key.setGroup(group);
                    search_key.setElement(elem);
                    debug(3,("searchkey:%x,%x",search_key.getGroup(),search_key.getElement()));
                    char* tag=(char*)tag_value.c_str();
                    debug(3,("Starting tag-action"));
                    error_count+=startTagAction(search_key,tag);
                    
                }
                if (modify_item_tag_option || insert_item_tag_option
                   || erase_item_tag_option)
                {
                    debug(3,("Starting item-tag-action"));
                    error_count+=startItemTagAction((char*)tag_path.c_str(),
                                                    (char*)tag_value.c_str());
                }                                      
            } else if (verbose_option)
            {
                printf("Found comment or empty line, jump to next one\n");
                printf("-----------------------------------------------------\n");
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
        //delete tag_value;
        delete line;
    }   
    return error_count; 
}


// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Destructor, delete dynamically allocated objects
// Parameters   : none
// Return Value : none
MdfConsoleEngine::~MdfConsoleEngine()
{
    debug(3,("Deleting member-variables from memory"));
    if (app_name!=NULL) delete app_name;
    if (app!=NULL) delete app;
    if (cmd!=NULL) delete cmd;
    if (files!=NULL) delete files;
}

/*
** CVS/RCS Log:
** $Log: mdfconen.cc,v $
** Revision 1.3  2003-07-09 12:13:13  meichel
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
