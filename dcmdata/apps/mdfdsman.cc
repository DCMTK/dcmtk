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
 *  Purpose: Class for modifying DICOM-Files and Datasets
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2003-10-01 14:03:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfdsman.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"   // make sure OS specific configuration is included first
#include "mdfdsman.h"
#include "dcvrsl.h"
#include "ofcast.h"
#include "ofstd.h"

#define INCLUDE_CSTDIO
#include "ofstdinc.h"


MdfDataSetManager::MdfDataSetManager()
:dfile(NULL), dset(NULL)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Constructor, just initializes members-variables
// Return Value : none
{
}


OFCondition MdfDataSetManager::loadFile(const char *file_name)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : loads a file into Datasetmanager (this class)
// Parameters   : file_name - [in] file to be loaded
// Return Value : An Integer, wheter loading was succesfull or not
{
    OFCondition cond;
    //to be sure, delete old dfile
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
    //file susccessfully loaded into dfile:
    else
    {
        //get dataset from file
        debug(3,("Getting Dataset from loaded file %s",file_name));
        dset=dfile->getDataset();
        /*load also pixel-data into memory:
         *Without this line pixel-data can't be included into the file
         *that is saved after modifying.
         */
        dset->loadAllDataIntoMemory();
    }
    return cond;
}


OFCondition MdfDataSetManager::modifyOrInsertTag(DcmTagKey search_key,
                                                 const char* value,
                                                 OFBool only_modify)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : modifies a given tag to a (new) value, replaces old value.
//                It DOES insert tag, if it doesnt exist already!
// Parameters   : search_key - [in] Tag to be searched
//                value - [in] value that should be placed into the Dataset
//                only_modify - [in] if false, a non exisiting tag is inserted
// Return Value : An OFCondition, whether modify/insert was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    OFCondition result;
    DcmElement *elem;

    //look for the tag to be modified/inserted
    debug(3,("Searching for tag %x,%x", search_key.getGroup(),
                                        search_key.getElement()));
    //if tag can be found, try to modify it
    if (dset->findAndGetElement(search_key, elem, OFFalse).good())
    {
        //just an ordinary modify...
        debug(3,("Tag found, trying to modify"));
        result=startModify(elem, value);
    }
    //if the tag could not be found, try to insert it:
    else
    {
        //but only insert it, if allowed per !only_modify
        if (!only_modify)
        {
            debug(3,("Tag not found, trying to insert new one"));
            result=startInsert(dset, search_key, value);
        }
        //only_modify is selected and tag is not in dataset, return errors
        else result=EC_TagNotFound;
    }
    return result;
}


OFCondition MdfDataSetManager::modifyAllTags(DcmTagKey search_key,
                                             const char* value,
                                             int &count)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : modifies ALL tags in the dataset to a new value
//                does NOT insert tag, if it doesnt exist already!
// Parameters   : search_key - [in] Tag to be searched
//                value - [in] value that should be placed into the Dataset
//                count - [out] returns number of modified tags
// Return Value : An OFCondition, whether modifying was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    OFCondition result;
    //this stack will hold result of element-search
    DcmStack *result_stack=new DcmStack();
    DcmObject *elem;
    //get references to all matching tags in dataset and store them in stack
    debug(3,("Trying to find all references to tag matching %x,%x",
             search_key.getGroup(), search_key.getElement()));
    result=dset->findAndGetElements(search_key,result_stack);
    //as long there are matching elements left on the stack
    debug(3,("Found %i occurences", result_stack->card()));
    while(result_stack->card()>0 && result.good())
    {
        //get the top-one
        elem=result_stack->pop();
        //if user gives e.g. a sequence-delimiter,don't try to change it!
        if (elem->isLeaf())
        {
            //and put new value to element
            debug(3,("Accessing existing tag for modifying"));
            result=startModify(OFstatic_cast(DcmElement*,elem),value);
            //result=startModify((DcmElement *)elem,value);
            //if an error occurs we better go home;)
            if (result.good()) count++;
        }
        //if user gave "unchangable" tag:
        else result=makeOFCondition(0,0,OF_error,"Unable to modify tag!");
    }
    delete result_stack;
    return result;
}


OFCondition MdfDataSetManager::deleteTag(DcmTagKey search_key,
                                         OFBool all_tags)
// Date         : May, 28th, 2003
// Author       : Michael Onken
// Task         : deletes tag in dataset
// Parameters   : search_key - [in] Tag to be deleted
//                allTags - [in] whether to delete only first matching
//                          or all matching tags in dataset
// Return Value : An OFCondition, whether deleting was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    OFCondition result;
    debug(3,("Trying to delete tag %x,%x",
        search_key.getGroup(), search_key.getElement()));
    result=dset->findAndDeleteElement(search_key, all_tags, all_tags);
    return result;
}


OFCondition MdfDataSetManager::deleteItemTag(char *tag_path)
// Date         : June, 18th, 2003
// Author       : Michael Onken
// Task         : deletes tag
//                The tag is denoted by the tag_path, so it can be inside a
//                sequence
// Parameters   : taq path - [in] Tag to be searched
// Return Value : An OFCondition, whether deleting was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    DcmItem *item=NULL;
    //walk through path and get item to work on
    OFCondition result = getItemFromPath(item, tag_path);
    if (result.good())
    {
        int group, elem;
        DcmTagKey search_key(0xffff,0xffff);
        //now tag_path consists of a simple tag & item points to right  itemset
        if (sscanf(tag_path, "%x,%x%*s", &group, &elem) == 2 )
        {
            //generate key from parsed information
            search_key.set(group, elem);
            //Here we reached our tag and try to modify!
            debug(3,("Trying to delete tag"));
            //(OFFalse, OFFalse) means: not allOccurences,not diveIntoSub
            result=item->findAndDeleteElement(search_key,OFFalse,OFFalse);
        }
    }
    return result;
}


OFCondition MdfDataSetManager::modifyOrInsertItemTag(char* tag_path,
                                                     const char *value,
                                                     OFBool only_modify)
// Date         : May, 19th, 2003
// Author       : Michael Onken
// Task         : modifies/inserts a given tag to a (new) value
//                The tag is denoted by the tag_path, so it can be inside a
//                sequence
// Parameters   : taq path - [in] Tag to be searched
//                value - [in] value that should be placed into the Dataset
//                only_modify - [in] flag, whether not yet existing tag should be
//                inserted
// Return Value : An OFCondition, whether modifying was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    DcmItem *one_item=NULL;
    //a Tag to be searched, incl. group and element
    DcmTagKey search_key(0xffff,0xffff);
    int group;
    int elem;
    OFCondition result;
    OFCondition found;
    //copy tag_path to new char* to hide the side-effect of getItemFromPath
    char *path=new char[strlen(tag_path)+1];
    OFStandard::strlcpy(path,tag_path,strlen(tag_path)+1);
    result = getItemFromPath(one_item, path);
    //now path consists of a simple tag & one_item points to right itemset
    if (result.good())  {
        if (sscanf(path, "%x,%x%*s", &group, &elem) == 2 )
        {
            //generate key from parsed information
            search_key.set(group, elem);
            debug(3,("walked whole tag-path, reached tag %x,%x",
                search_key.getGroup(),search_key.getElement()));
            //Here we reached our tag and try to modify!
            debug(3,("Trying to modify tag"));

            //if only_modify is selected (no overwrite!) or you get here
            //per insert-option and tag is in actual item:
            if (only_modify || one_item->tagExists(search_key, OFFalse))
            {
                DcmElement *element = NULL;
                //get element from item
                result=one_item->findAndGetElement(search_key,element,OFFalse);
                //if element was found:
                if (element != NULL)
                {
                    //modify element
                    result=startModify(element, value);
                }
            }
            //you want to insert and element isn't in item yet:
            else
            {
                result=startInsert(one_item, search_key, value);
            }
        }
        //if group and element couldnt be parsed
        else
        {
            result = makeOFCondition(0,0,OF_error,
                "Parse Error: Couldnt parse tag!");
        }

    }
    delete path;
    return result;
}


OFCondition MdfDataSetManager::getElements(DcmStack &result_stack, OFBool intoSub)
// Date         : August, 8th, 2003
// Author       : Michael Onken
// Task         : get next Object from internal dataset. if intoSub true, scan
//                complete hierarchy, false scan only elements direct in this
//                item (not deeper).
// Parameters   : result_stack - [out] the result of nextObject is stored here
//                intoSub - [in] dive into sub
// Return Value : returns EC_normal if everything is ok, else an error
{
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    OFCondition result = dset->nextObject(result_stack, intoSub);
    return result;
}


OFCondition MdfDataSetManager::saveFile(const char *file)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : saves current file loaded into the Datasetmanager to disk
// Parameters   : file - [in] filename to be saved to
// Return Value : An Integer, wheter saving was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    OFCondition result;
    //saving the file with metaheader to file (to save changes)
    debug(3,("Saving actual dataset to file %s",file));
    if (dfile!=NULL)
    {
        result=dfile->saveFile(file);
    }
    return result;
}


OFCondition MdfDataSetManager::startInsert(DcmItem *item,
                                           const DcmTagKey &search_key,
                                           const char *value)
// Date         : August, 22 th, 2003
// Author       : Michael Onken
// Task         : inserts tag into item with a specific value
// Parameters   : item - [in/out] pointer to item, where tag is inserted
//                search_key - [in] specifies tag to be inserted
//                value - [in] value that should be inserted in item
// Return Value : OFCondition, which returns an error-code if an error occurs
{
    OFCondition result;
    //holds element to insert in item
    DcmElement *elem = NULL;
    //normally the VR of the element is looked up in dictionary
    //if element's tag describes no dicom-tag or an ambigous VR,
    //it's VR is set to UN
    result = newDicomElement(elem, search_key);
    //if dicom-element could be created, insert in to item and modify to value
    if (result.good())
    {
        result = item->insert(elem);
        if (result.good())
            result = startModify(elem,value);
    }
    return result;
}


OFCondition MdfDataSetManager::startModify(DcmElement *elem,
                                           const char *value)
// Date         : August, 22th, 2003
// Author       : Michael Onken
// Task         : modifies element a specific value
// Parameters   : elem - [in/out] pointer to element, that should be changed
//                value - [in] value, the element should be changed to
// Return Value : OFCondition, which returns an error-code if an error occurs
{
    Sint32 s32;
    Sint16 s16;
    Uint32 u32;
    Uint16 u16;
    OFCondition result;
    //start right put-function based on element's VR
    switch(elem->getTag().getEVR())
    {
        case EVR_SL:
            s32 = atoi(value);
            result = elem->putSint32(s32);
            break;
        case EVR_SS:
            s16 = atoi(value);
            result = elem->putSint16(s16);
            break;
        case EVR_UL:
            u32 = atoi(value);
            result = elem->putUint32(u32);
            break;
        case EVR_US:
            u16 = atoi(value);
            result = elem->putUint16(u16);
            break;
        //putString can handle all values except SL,SS,UL,US
        default:
            result = elem->putString(value);
    }
    return result;
}


OFCondition MdfDataSetManager::getItemFromPath(DcmItem *&result_item,
                                               char *&tag_path)
// Date         : August, 22th, 2003
// Author       : Michael Onken
// Task         : walks along a tag path like gggg,eeee[0].gggg,eeee=gggg,eeee
// Parameters   : result_item - [in/out] pointer to item, thats the last in path
//                tag_path - [in] complete path to tag; after returning
//                           tag_path only consists of last tag in path
//                           (the destination tag!)
// Return Value : OFCondition containing an error-code if an error occurs
{
    //will hold itemset to be copied to one_item
    DcmItem *item_copy=NULL;
    //a Tag to be searched, incl. group and element
    DcmTagKey search_key(0xffff,0xffff);
    int item_nr;
    int group, elem, length;
    OFCondition found;
    OFCondition result;

    //As long we haven an i in path, wich denotes an item and no error occured
    while (strpbrk(tag_path,"[")!=NULL && result.good())
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
            result=makeOFCondition(0,0,OF_error,
                "Parse Error: Couldnt parse tag!");
        }
        //if tag was parsed successfully
        if (result.good())
        {
            //now parse item_nr, therefore skip the '['
            tag_path++; // go to next char, now we should get the item-number
            //now parse the item-nr itself
            debug(3,("Now scanning for item-nr in []"));
            if (sscanf(tag_path, "%i%n", &item_nr, &length) == 1)
            {
                //go ahead the characters that represented item-nr + 1 for "]."
                tag_path=tag_path+length+2;
            }
            else
            {
                result=makeOFCondition(0,0,OF_error,
                    "Parse Error: Couldnt parse item-nr!");
            }
            if (result.good())
            {
                 //look up parsed sequence
                debug(3,("Now we've got seq.+item-tag, now lookup in dataset"));
                if (item_copy==NULL)
                    found=dset->
                        findAndGetSequenceItem(search_key,item_copy,item_nr);
                else
                    found=item_copy->
                        findAndGetSequenceItem(search_key,item_copy,item_nr);
                //found.good() -> one_item:=item_copy
                if (found.good())
                {
                    result_item=item_copy;

                }
                else
                {
                    if (found == EC_IllegalParameter)
                        result=makeOFCondition(0,0,OF_error,
                            "Specified item could not be found!");
                    else
                        result=makeOFCondition(0,0,OF_error,
                            "Specified sequence could not be found!");
                }
            }
        }
    }
    //if while-loop could not be entered, result must set manually to an error
    if (result_item==NULL && result.good())
    {
        result=makeOFCondition(0,0,OF_error,
            "Parse Error: Perhaps wrong tag-path-format?");
    }
    return result;
}


MdfDataSetManager::~MdfDataSetManager()
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Destructor, cleans up members
// Return Value : none
{
    //cleanup
    debug(3,("Deleting member-variables from memory"));
    delete dfile;
}

/*
** CVS/RCS Log:
** $Log: mdfdsman.cc,v $
** Revision 1.4  2003-10-01 14:03:27  onken
** Bug fixed, that excluded pixel-data when saving a file loaded into a
** MdfDataSetManager
**
** Revision 1.3  2003/09/19 12:41:11  onken
** major bugfixes, new code structure, better error-handling, corrections for "dcmtk-coding-style",Handling of VR's corrected
**
** Revision 1.2  2003/07/09 12:13:13  meichel
** Included dcmodify in MSVC build system, updated headers
**
** Revision 1.1  2003/06/26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
