/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Purpose: Class for modifying DICOM-Files and Datasets
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2003-06-26 09:17:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfdsman.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "mdfdsman.h"

// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Constructor.
// Parameters   : app  [in] name of calling application
// Return Value : none
MdfDataSetManager::MdfDataSetManager(const char *app)
:dfile(NULL), dset(NULL), app_name(NULL)
{
    //copy application-name to local attribute
    if (app!=NULL)
    {
        this->app_name=new char[strlen(app)+1];
        strcpy(app_name,app);
    }
}




// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : loads a file into Datasetmanager (this class)
// Parameters   : file_name [in] file to be loaded
// Return Value : An Integer, wheter loading was succesfull or not
OFCondition MdfDataSetManager::loadFile(const char *file_name)
{   
    OFCondition result;
    OFCondition cond;
    delete dfile;
    dfile = new DcmFileFormat();
    //load File into Attribute dfile
    debug(3,("Loading %s into Datasetmanager",file_name));
    cond=dfile->loadFile(file_name);
    //if there are errors:
    if (cond.bad())
    {
        debug(3,("Failed loading file %s",file_name));
        dset=NULL;
    }
    //file loaded into dfile:
    else 
    {
        //get dataset from file
        debug(3,("Getting Dataset from loaded file %s",file_name));
        dset=dfile->getDataset();
    }   
    return cond;
}




// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : modifies a given tag to a (new) value, replaces old value
//                does NOT insert tag, if it doesnt exist already!
// Parameters   : search_key [in] Tag to be searched
//                value [in] value that should be placed into the Dataset
// Return Value : An OFCondition, whether modifying was succesfull or not
OFCondition MdfDataSetManager::modifyTag(DcmTagKey search_key, const char* value)
{   
    OFCondition result;
    OFString result_string;
    
    //look for the tag to be modified
    debug(3,("Searching for tag %x,%x", search_key.getGroup(),
                                        search_key.getElement()));
    result=dset->findAndGetOFString(search_key, result_string);
    if (dset->tagExists(search_key))
    {
        debug(3,("Tag found, now trying to modify"));
        //and modify it! OFTrue means here: only replace old, not insert new one
        result=dset->putAndInsertString(search_key, value, OFTrue);
    }
        
    //here result is the result of trying to modify tag
    return result;
}



// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : modifies a given tag to a (new) value, replaces old value.
//                It DOES insert tag, if it doesnt exist already!
// Parameters   : search_key [in] Tag to be searched
//                value [in] value that should be placed into the Dataset
// Return Value : An OFCondition, whether modify/insert was succesfull or not
OFCondition MdfDataSetManager::modifyOrInsertTag(DcmTagKey search_key, 
                                                 const char* value)
{   
    OFCondition result;
    OFString result_string;

    //look for the tag to be modified/inserted
    debug(3,("Searching for tag %x,%x", search_key.getGroup(),
                                        search_key.getElement()));
    if (dset->findAndGetOFString(search_key, result_string).good()) 
    {
        //just an ordinary modify...
        debug(3,("Tag found, trying to modify"));
        result=modifyTag(search_key, value);
    }
    //if the tag could not be found, try to insert it:
    else
    {  
       debug(3,("Tag not found, trying to insert new one"));          
       result=dset->putAndInsertString(search_key, value, OFFalse);
    }
    return result;
}    



// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : modifies ALL tags in the dataset to a new value
//                does NOT insert tag, if it doesnt exist already!
// Parameters   : search_key [in] Tag to be searched
//                value [in] value that should be placed into the Dataset
//                count [out] returns number of modified tags
// Return Value : An OFCondition, whether modifying was succesfull or not
OFCondition MdfDataSetManager::modifyAllTags(DcmTagKey search_key, 
                                             const char* value,
                                             int &count)
{   
    OFCondition result;
    //this stack will hold result of element-search
    DcmStack *stack=new DcmStack();
    DcmObject *elem;
    //get references to all matching tags in dataset and store them in stack
    debug(3,("Trying to find all references to tag matching %x,%x",
             search_key.getGroup(), search_key.getElement()));
    result=dset->findAndGetElements(search_key,stack);
    //as long there are matching elements left on the stack
    debug(3,("Found %i occurences", stack->card()));
    while(stack->card()>0)
    {
        //get the top-one
        elem=stack->pop();
        //if user gives e.g. a sequence-delimiter,don't try to change it!
        if (elem->isLeaf())
        {
            //and put new value to element
            debug(3,("Accessing existing tag for modifying"));
            result=((DcmElement *)elem)->putString(value);
            //if an error occurs we better go home;)
            if (result.bad()) return result;
            else count++;
        }   
        //if user gave "unchangable" tag: 
        else return EC_InvalidTag;  
    }   
    delete stack;
    return result;
}



// Date         : May, 28th, 2003
// Author       : Michael Onken
// Task         : deletes tag in dataset
// Parameters   : search_key [in] Tag to be deleted
//                allTags [in] whether to delete only first matching 
//                          or all matching tags in dataset
//                dive_down [in] dive into sub-sequences, automatically true
//                          if allTags is set
// Return Value : An OFCondition, whether deleting was succesfull or not
OFCondition MdfDataSetManager::deleteTag
        (DcmTagKey search_key, OFBool all_tags)
{   
    OFCondition result;
    OFString result_string;
    debug(3,("Trying to delete tag %x,%x", 
             search_key.getGroup(), search_key.getElement()));
    result=dset->findAndDeleteElement(search_key, all_tags, all_tags);
    return result;
}    


// Date         : June, 18th, 2003
// Author       : Michael Onken
// Task         : deletes tag
//                The tag is denoted by the tag_path, so it can be inside a
//                sequence              
// Parameters   : taq path [in] Tag to be searched
// Return Value : An OFcondition, whether deleting was succesfull or not
OFCondition MdfDataSetManager::deleteItemTag (char *tag_path)
{
    //--------------------------
    //  should be recoded, nearly same as modifyOrInsertItemTag, ->merge!
    //--------------------------
    DcmItem *one_item=NULL;
    //will hold itemset to be copied to one_item
    DcmItem *item_copy;
    //a Tag to be searched, incl. group and element
    DcmTagKey search_key(0xffff,0xffff);
    int group;
    int elem;   
    int item_nr;    //now a tag can be inside a sequence->we need item-nr!
    int length;     //should only mean length of a character string
    OFCondition result; //the return value of this funct.
    OFCondition found;  //can searched Tag be found?    
    
    //As long we haven an i in path, wich denotes an item:
    while (strpbrk(tag_path,"[")!=NULL)
    {
        //get next path-element: Tag gggg,eeee
        if (sscanf(tag_path, "%x,%x", &group, &elem) == 2 && strlen(tag_path)>9) 
        {   
            //generate key from parsed information
            debug(3,("Next tag-key in path beeing processed is %x,%x",
                     search_key.getGroup(), search_key.getElement()));
            search_key.set(group, elem); 
            //path is like gggg,eeee[n], so jump to the "["
            tag_path+=9;
        }
        else
        {   
            CERR << "Error parsing given path element: " << tag_path << endl;
            return makeOFCondition(0,0,OF_error,"Parse Error");
        }   
        
        //now parse item_nr, therefore skip the 'i'
        tag_path++; // go to next char, now we should get the item-number
        //now parse the item-nr itself
        debug(3,("Now scanning for item-nr in []"));
        if (sscanf(tag_path, "%i%n", &item_nr, &length) == 1)
        {   
            //go ahead the characters that represented item-nr + 1 for "]"
            tag_path=tag_path+length+1;
        }   
        else CERR << "failed reading item_nr" ;
        
        //look up parsed sequence
        debug(3,("Now we've got sequence+item-tag, now try to lookup in dataset"));
        found=dset->findAndGetSequenceItem(search_key,item_copy,item_nr);
        //item_copy!=NULL -> found.good() -> one_item:=item_copy 
        if (found.good())
        {   one_item=item_copy;
        }   
        //item_copy==NULL weil Seq. nicht vorhanden -> Fehler ausgeben
        else
        {   
            tag_path++; //For error message cut off leading ","
            CERR << "Couldn't find path to sequence: " 
                 << search_key.getGroup() << ","
                 << search_key.getElement() << endl;
            return found;   
        }
    }
    //skip the '.'
    tag_path++;
    //now tag_path consists of a simple tag & one_item points to right  itemset
    debug(3,("walked whole tag-path, reached tag %x,%x",
            search_key.getGroup(),search_key.getElement()));
    if (sscanf(tag_path, "%x,%x%*s", &group, &elem) == 2 ) 
    {   
        //generate key from parsed information
        search_key.set(group, elem);
        //Here we reached our tag and try to modify!
        debug(3,("Trying to delete tag"));
        //delete tag, (OFFalse, OFFalse) means: no allOccurences,not diveIntoSub
        result=one_item->findAndDeleteElement(search_key,OFFalse,OFFalse);
    }
    else
    {   CERR << "Couldn't parse path to tag: " << tag_path << endl;
    }     

    return result;
}    

// Date         : May, 19th, 2003
// Author       : Michael Onken
// Task         : modifies/inserts a given tag to a (new) value
//                The tag is denoted by the tag_path, so it can be inside a
//                sequence              
// Parameters   : taq path [in] Tag to be searched
//                value [in] value that should be placed into the Dataset
//                only_modify [in] flag, whether not yet existing tag should be
//                inserted
// Return Value : An OFcondition, whether modifying was succesfull or not
OFCondition MdfDataSetManager::modifyOrInsertItemTag
        (char* tag_path, const char *value, OFBool only_modify)
{   
    DcmItem *one_item=NULL;
    DcmItem *item_copy; //will hold itemset to be copied to one_item
    //a Tag to be searched, incl. group and element
    DcmTagKey search_key(0xffff,0xffff);
    int group;
    int elem;   
    int item_nr;    //now a tag can be inside a sequence->we need item-nr!
    int length;     //should only mean length of a character string
    OFCondition result; //the return value of this funct.
    OFCondition found;  //can searched Tag be found?    
    
    //As long we haven an i in path, wich denotes an item:
    while (strpbrk(tag_path,"[")!=NULL)
    {
        //get next path-element: Tag gggg,eeee
        if (sscanf(tag_path, "%x,%x", &group, &elem) == 2 && strlen(tag_path)>9) 
        {   
            //generate key from parsed information
            search_key.set(group, elem); 
            debug(3,("Next tag-key in path beeing processed is %x,%x",
                     search_key.getGroup(), search_key.getElement()));
            //path is like gggg,eeee[n], so jump to the "["
            tag_path+=9;
        }
        else
        {
            CERR << "Error parsing given path element: " << tag_path << endl;
            return makeOFCondition(0,0,OF_error,"Parse Error");
        }   
        
        //now parse item_nr, therefore skip the 'i'
        tag_path++; // go to next char, now we should get the item-number
        //now parse the item-nr itself
        debug(3,("Now scanning for item-nr in []"));
        if (sscanf(tag_path, "%i%n", &item_nr, &length) == 1)
        {   
            //go ahead the characters that represented item-nr + 1 for "]"
            tag_path=tag_path+length+1;
        }   
        else CERR << "failed reading item_nr" ;
        
        //look up parsed sequence
        debug(3,("Now we've got sequence+item-tag, now try to lookup in dataset"));
        found=dset->findAndGetSequenceItem(search_key,item_copy,item_nr);
        //item_copy!=NULL -> found.good() -> one_item:=item_copy 
        if (found.good())
        {   one_item=item_copy;
        }   
        //item_copy==NULL weil Seq. nicht vorhanden -> Fehler ausgeben
        else
        {   
            tag_path++; //For error message cut off leading ","
            CERR << "Couldn't find path to sequence: " 
                 << search_key.getGroup() << ","
                 << search_key.getElement() << endl;
            return found;   
        }
    }
    //skip the '.'
    tag_path++;
    //now tag_path consists of a simple tag & one_item points to right  itemset

    if (sscanf(tag_path, "%x,%x%*s", &group, &elem) == 2 ) 
    {   
        //generate key from parsed information
        search_key.set(group, elem);
        debug(3,("walked whole tag-path, reached tag %x,%x",
            search_key.getGroup(),search_key.getElement()));
        //Here we reached our tag and try to modify!
        debug(3,("Trying to modify tag"));
        if (one_item!=NULL)
        {
            result=one_item->putAndInsertString(search_key, value, only_modify);
        }
        else return EC_InvalidTag;
    }    
    else
    {   CERR << "Couldn't parse path to tag: " << tag_path << endl;
        return EC_InvalidTag;
    }     

    return result;
}    




// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : saves current file loaded into the Datasetmanager to disk
// Parameters   : save_file_name [in] filename to be saved to
// Return Value : An Integer, wheter saving was succesfull or not
OFCondition MdfDataSetManager::saveFile(const char *save_file_name)
{   
    OFCondition result;
    //saving the file with metaheader to file (to save changes)
    debug(3,("Saving actual dataset to file %s",save_file_name));
    result=dfile->saveFile(save_file_name);
    return result;
}    


MdfDataSetManager::~MdfDataSetManager()
{   
    //cleanup
    debug(3,("Deleting member-variables from memory"));
    delete dfile;
    delete app_name;
}  

/*
** CVS/RCS Log:
** $Log: mdfdsman.cc,v $
** Revision 1.1  2003-06-26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
