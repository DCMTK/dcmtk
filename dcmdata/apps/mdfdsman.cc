/*
 *
 *  Copyright (C) 2003-2004, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2004-01-16 10:53:53 $
 *  CVS/RCS Revision: $Revision: 1.9 $
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
#include "dctk.h"

#define INCLUDE_CSTDIO
#include "ofstdinc.h"


MdfDatasetManager::MdfDatasetManager(const OFBool &debug)
:act_file(""),dfile(NULL), dset(NULL), debug_option(OFFalse)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Constructor, initializes member-variables
// Parameters   : debug - [in] enables/disables debug-messages (off per default)
// Return Value : none
{
    debug_option=debug;
}


OFCondition MdfDatasetManager::loadFile(const char *file_name)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : loads a file into Datasetmanager
// Parameters   : file_name - [in] file to be loaded
// Return Value : An OFCondition, wheter loading was succesfull or not
{
    OFCondition cond;
    //delete old dfile and free memory and reset act_file
    delete dfile;
    act_file="";
    dfile = new DcmFileFormat();
    //load File into Attribute dfile
    if (debug_option)
        debugMsg("Loading into Datasetmanager: ", file_name);
    cond=dfile->loadFile(file_name);
    //if there are errors:
    if (cond.bad())
    {
        if (debug_option)
            debugMsg("Failed loading file: ", file_name);
        dset=NULL;
    }
    //file susccessfully loaded into dfile:
    else
    {
        //get Dataset from file
        if (debug_option)
            debugMsg("Getting Dataset from loaded file ", file_name);
        dset=dfile->getDataset();
        /*load also pixel-data into memory:
         *Without this command pixel-data wouldn't be included into the file,
         *that's saved after modifying, because original filename was renamed
         *meanwhile
         */
        dset->loadAllDataIntoMemory();
        //save filename to member variable
        act_file=file_name;
    }
    return cond;
}

static DcmTagKey getTagKeyFromDictionary(OFString tag)
// Date         : December, 4th, 2003
// Author       : Michael Onken
// Task         : lookup tagstring in dictionary and return corresponding key
// Parameters   : tag - [in] tagstring for lookup
// Return Value : Returns key from successful lookup, else (0xffff,0xffff)
{
    DcmTagKey key(0xffff,0xffff);
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(tag.c_str());
    //successfull lookup in dictionary -> translate to tag and return
    if (dicent)
    {
        key = dicent->getKey();
     }
     dcmDataDict.unlock();
     return key;
}


static int readNextToken(const char *c, int& pos, DcmTagKey& key, Uint32& idx)
// Date         : November, 26th, 2003
// Author       : Marco Eichelberg
// Task         : scans a token from the given string and returns it. Ignores
//                leading whitespace.
// Parameters   : c - [in] string to parse
//                pos - [in/out] position within string, modified after
//                      successful scan
//                key - [out] tag key returned in this parameter if return
//                      value is "tag key".
//                idx - [out] index returned in this parameter if return
//                      value is "index".
// Return Value : -1 for "EOF", 0 for "parse error", 1 for "tag key",
//                2 for "index", 3 for "period"
{
  OFString aString;
  int lpos = pos;
  int spos = 0;
  if (c[lpos]=='\0') return -1;// EOF
  if (c[lpos]=='.')
  {
    ++pos;
    return 3; // period
  }
  //look for item-index between []
  if (c[lpos]=='[')
  {
    spos = ++lpos;
    while ((c[lpos] >= '0')&&(c[lpos] <= '9')) ++lpos;
    if (c[lpos] != ']') return 0; // parse error
    unsigned long newindex = 0;
    if (1 != sscanf(c+spos,"%lu", &newindex)) return 0; // parse error
    idx = OFstatic_cast(Uint32, newindex);
    pos = ++lpos;
    return 2; // index
  }
  //look for tag between ()
  if (c[lpos]=='(')
  {
    spos = ++lpos;
    while ((c[lpos] != ')')&&(c[lpos] != '\0')) ++lpos;
    if (c[lpos] != ')') return 0; // parse error
    unsigned int group=0;
    unsigned int elem=0;
    if (2 != sscanf(c+spos,"%x,%x", &group, &elem)) return 0; // parse error
    key = DcmTagKey(group,elem);
    pos = ++lpos;
    return 1; // tag key
  }
  //so far no tag and no item-index found. So check if its a dictionary-name
  spos = lpos;
  while ( ((c[lpos] >= 'a')&&(c[lpos] <= 'z')) ||
          ((c[lpos] >= 'A')&&(c[lpos] <= 'Z')) ||
          ((c[lpos] >= '0')&&(c[lpos] <= '9'))) ++lpos;
  aString.append(c + spos, (lpos-spos));
  key=getTagKeyFromDictionary(aString);
  //if key was found in dictionary, return 1 for tag key
  if ( (key.getGroup()!=0xffff) && (key.getElement()!=0xffff) )
  {
    pos=lpos;
    return 1; //tag key
  }
  //if no return-command was processed this far, the token could not be parsed
  return 0; // parse error
}


static DcmItem* getItemFromPath(DcmItem &Dataset,
                                const char *location,
                                OFString &message)
// Date         : November, 26th, 2003
// Author       : Marco Eichelberg/Michael Onken
// Task         : locates a specific item within the given Dataset
// Parameters   : Dataset - [in] Dataset to be searched
//                location - [in] location string. Format is
//                           "sequence[item]{.sequence[item]}*" Where sequence
//                           can be (gggg,eeee) or a dictionary name and items
//                           within sequences are counted from zero.
//                message - [out] error message if item couldn't be found
// Return Value : pointer to the item searched if found, NULL otherwise
{
  DcmTagKey key;
  Uint32 idx;
  int pos = 0;
  int token = 0;
  int expected = 1; //first expected is a tagkey
  OFBool finished = OFFalse;
  DcmItem *result = &Dataset;
  DcmSequenceOfItems *sq = NULL;
  DcmStack stack;
  message.clear();
  do
  {
    token = readNextToken(location, pos, key, idx);
    if ((token != expected)&&(token != -1))
    {
      message=message + "parse error in path '" + location + "'.";
      return NULL;
    }
    if (token == -1)
    {
      if (!finished)
      {
        message=message+"error: path '" + location + "' incomplete.";
        return NULL;
      }
      return result;
    }
    if (token == 1)
    {
      // we have read a tag key
      stack.clear();
      if (EC_Normal != result->search(key, stack, ESM_fromHere, OFFalse))
      {
        message=message + "error: attribute not found in Dataset (path is '"
            + location + "')";
        return NULL;
      }
      if (stack.top()->ident() == EVR_SQ)
      {
        sq = OFstatic_cast(DcmSequenceOfItems *, stack.top());
      } else {
        message=message + "error: attribute is not a sequence (path is '"
            + location + "')";
        return NULL;
      }
      expected = 2;
      finished = OFFalse;
    }
    else if (token == 2)
    {
      // we have read an index
      if (sq == NULL)
      {
        message=message + "error: sequence not found in path '" + location +"'";
        return NULL;
      }
      if (idx >= sq->card())
      {
        message=message + "error: cannot allocate item in sequence (path is '"
            + location + "')";
        return NULL;
      }
      result = sq->getItem(idx);
      if (result == NULL)
      {
        message=message + "error: item not found in path '" + location + "'";
        return NULL;
      }
      expected = 3;
      finished = OFTrue;
    }
    else if (token == 3)
    {
      // we have read a period
      expected = 1;
      finished = OFFalse;
    }
  } while (token > 0);
  return NULL;
}


static OFCondition splitTagPath(OFString &tag_path, DcmTagKey &key)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : splits tag path into two parts: the path itself and the
//                key of the target-tag. Path like (0008,1111)[0].(0010,0010) is
//                seperated into path (0008,1111)[0] and key (0010,0010)
// Parameters   : tag_path - [in/out] path
//                key - [out] parsed key of target tag
// Return Value : returns EC_normal if everything is ok, else a parse error
{
    OFString target_tag;
    int group,elem;
    int lpos,rpos;
    rpos=tag_path.size()-1;
    lpos=rpos;
    if (tag_path[rpos]==')')
    {
        //get opening '(' of target tag; if its not found -> return error
        while ( (tag_path[lpos]!='(') && (lpos>0) ) --lpos;
        if (tag_path[lpos]!='(')
            return makeOFCondition(0,0,OF_error,"Invalid tag path!");
        //now lpos and rpos "point" to braces of target-tag
        //copy target tag from tag path
        target_tag=tag_path.substr(lpos,rpos-lpos+1);
        //delete target-tag from path (inclusive trailing '.')
        tag_path.erase(lpos,tag_path.length()-lpos);
        //if theres a tag path left, remove the '.', too
        if ( (tag_path.length()>0) && (tag_path[tag_path.length()-1]=='.'))
            tag_path.erase(tag_path.length()-1,1);
        //parse target_tag into DcmTagKey
        if (2 != sscanf(target_tag.c_str(),"(%x,%x)", &group, &elem))
            return makeOFCondition(0,0,OF_error,"Invalid target tag!");
        key = DcmTagKey(group,elem);
    }
    else
    //otherwise we could have a dictionary-name
    {
        while ( (lpos>0) && (((tag_path[lpos] >= 'a')&&(tag_path[lpos] <= 'z')) ||
                ((tag_path[lpos] >= 'A')&&(tag_path[lpos] <= 'Z')) ||
                ((tag_path[lpos] >= '0')&&(tag_path[lpos] <= '9')))
              )  lpos--;
        target_tag=tag_path.substr(lpos,rpos-lpos+1);
        if (target_tag[0]=='.') target_tag.erase(0,1);
        tag_path.erase(lpos,tag_path.length()-lpos);
        key=getTagKeyFromDictionary(target_tag);
        if ( (key.getGroup()==0xffff) && (key.getElement()==0xffff) )
        {
            OFString message=target_tag;
            message.append(" not found in Dictionary!");
            return makeOFCondition(0,0,OF_error,message.c_str());
        }
    }
    return EC_Normal;
}


OFCondition MdfDatasetManager::modifyOrInsertTag(OFString tag_path,
                                                 const OFString &value,
                                                 const OFBool &only_modify)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : Modifies/Inserts a tag with a specific value
// Parameters   : tag_path - [in] holds complete path to tag
//                value - [in] value for tag
//                only_modify - [in] if true, only existing tags are processed.
//                              If false, a not existing tag is inserted.
// Return Value : returns EC_normal if everything is ok, else an error
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");

    OFCondition result;
    DcmElement *elem;
    DcmTagKey key;
    DcmItem *item = dset;
    //seperate tag path and target tag
    result=splitTagPath(tag_path, key);
    //if target-tag could not be parsed, return error
    if (!result.good())
        return result;
    //if an item-tag was specified, then tag_path still contains a path
    if (tag_path.length()!=0)
    {
        OFString error;
        item=getItemFromPath(*dset, tag_path.c_str(), error);
        //specified item not found -> return error
        if (item==NULL) return makeOFCondition(0,0,OF_error,
                               error.c_str());
    }
    if ((result=item->findAndGetElement(key, elem, OFFalse)).good())
    {
        return startModify(elem, value);
    }
    else if (!only_modify)
    {
        return startInsert(item,key,value);
    }
    return result;
}


OFCondition MdfDatasetManager::modifyAllTags(OFString tag_path,
                                             const OFString &value,
                                             int &count)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : Modifies all matching tags in Dataset to a new value
// Parameters   : tag_path - [in] denotes, which tag to modify
//                value - [in] denotes new value of tag
//                count - [out] returns number of modified tags
// Return Value : returns EC_normal if everything is ok, else an error
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");

    DcmTagKey key;
    OFCondition result;
    result=splitTagPath(tag_path,key);
    if ( (key.getGroup()==0xffff) && (key.getElement()==0xffff) )
        return result;

    //this stack will hold result of element-search
    DcmStack *result_stack=new DcmStack();
    DcmObject *elem;
    //get references to all matching tags in Dataset and store them in stack
    if (debug_option)
        debugMsg("Looking for occurences of: ", key.toString());
    result=dset->findAndGetElements(key,result_stack);
    //as long there are matching elements left on the stack
    if (debug_option)
    {
        ofConsole.lockCerr() << "Found " << result_stack->card()
                             << " occurences" << endl;
        ofConsole.unlockCerr();
    }
    while(result_stack->card()>0 && result.good())
    {
        //get the top-one
        elem=result_stack->pop();
        //if user gives e.g. a sequence-delimiter,don't try to change it!
        if (elem->isLeaf())
        {
            //and put new value to element
            if (debug_option)
                debugMsg("Accessing existing tag for modifying");
            result=startModify(OFstatic_cast(DcmElement*,elem),value);
            if (result.good()) count++;
        }
        //if user gave "unchangeable" tag:
        else result=makeOFCondition(0,0,OF_error,"Unable to modify tag!");
    }
    delete result_stack;
    return result;
}


OFCondition MdfDatasetManager::deleteTag(OFString tag_path,
                                         const OFBool &all_tags)
// Date         : May, 28th, 2003
// Author       : Michael Onken
// Task         : deletes tag in Dataset
// Parameters   : tag_path - [in] holds complete path to tag
//                allTags - [in] If true, tag is deleted at all levels of
//                          Dataset, else only 1. level is accessed
// Return Value : An OFCondition, whether deleting was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");

    //split tag-path into item-path and target-tag
    DcmTagKey key;
    OFCondition result;
    result=splitTagPath(tag_path,key);
    if (result.bad())
        return result;  //error parsing tag path

    //if tag path still contains characters, user wants to modify item-tag
    if (tag_path.length()>0)
    {
        OFString error;
        DcmItem *item=NULL;
        item=getItemFromPath(*dset,tag_path.c_str(),error);
        if (item!=NULL)
            result=item->findAndDeleteElement(key, all_tags, all_tags);
        else
            return makeOFCondition(0,0,OF_error,error.c_str());
    }
    //other user specified single tag without path
    else
        result=dset->findAndDeleteElement(key, all_tags, all_tags);

    return result;
}


OFCondition MdfDatasetManager::saveFile(const char *file)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Saves current Dataset back to a file
// Parameters   : file - [in] filename to save to
// Return Value : returns EC_normal if everything is ok, else an error
{
    //if no file loaded: return an error
    if (dfile==NULL)
        return makeOFCondition(0,0,OF_error,"No file loaded yet!");
    OFCondition result;
    //save file
    if (debug_option)
        debugMsg("Saving actual Dataset to file: ", file);
    if (dfile!=NULL)
        result=dfile->saveFile(file);
    return result;
}


OFCondition MdfDatasetManager::saveFile()
// Date         : December, 17th, 2003
// Author       : Michael Onken
// Task         : Saves current Dataset back to a file using original filename
// Return Value : returns EC_normal if everything is ok, else an error
{
    return saveFile(act_file.c_str());
}


OFCondition MdfDatasetManager::startInsert(DcmItem *item,
                                           DcmTagKey &search_key,
                                           const OFString &value)
// Date         : August, 22th, 2003
// Author       : Michael Onken
// Task         : inserts tag into item with a specific value
// Parameters   : item - [in/out] item, where tag is inserted
//                search_key - [in] specifies tag to be inserted
//                value - [in] value that should be inserted in item
// Return Value : returns an error-code as OFCondition, if an error occurs
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


OFCondition MdfDatasetManager::startModify(DcmElement *elem,
                                           const OFString &value)
// Date         : August, 22th, 2003
// Author       : Michael Onken
// Task         : modifies element a specific value
// Parameters   : elem - [in/out] pointer to element, that should be changed
//                value - [in] the value, the element should be changed to
// Return Value : OFCondition, which returns an error-code if an error occurs
{
    OFCondition result;
    //start put-function
    result = elem->putString(value.c_str());
    return result;
}


DcmDataset* MdfDatasetManager::getDataset()
// Date         : October, 1st, 2003
// Author       : Michael Onken
// Task         : Returns the Dataset, that this MdfDatasetManager handles.
//                You should use the returned Dataset readonly to avoid
//                sideeffects with other class-methods, that modify
//                this Dataset.
// Return Value : returns the Dataset, this MdfDatasetManager manages and NULL,
//                if no Dataset is loaded
{
    return dset;
}


// Date         : December, 17st, 2003
// Author       : Michael Onken
// Task         : Returns filename of the file, that's loaded actually.
// Return Value : returns filename and "" if no file is loaded.
OFString MdfDatasetManager::getFilename()
{
    return act_file;
}


void MdfDatasetManager::debugMsg(const OFString &s1,
                                 const OFString &s2,
                                 const OFString &s3)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : prints error message to console using global locking mechanism.
//                The function handles three strings, that are directly printed
//                after another. The whole message is then terminated by \n
// Parameters   : s1 - [in] first message string
//                s2 - [in] second message string (default = "")
//                s3 - [in] third message string (default = "")
// Return Value : none
{
        ofConsole.lockCerr() << s1 << s2 << s3 << endl;
        ofConsole.unlockCerr();
}


MdfDatasetManager::~MdfDatasetManager()
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Destructor, cleans up members
// Parameters   : None
// Return Value : none
{
    if (debug_option)
        debugMsg("Deleting member-variables from memory");
    //cleanup
    delete dfile;
}


/*
** CVS/RCS Log:
** $Log: mdfdsman.cc,v $
** Revision 1.9  2004-01-16 10:53:53  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
**
** Revision 1.8  2003/12/17 17:07:22  onken
** MdfDatasetManager now remembers loaded filename. Additional save function
** added.
**
** Revision 1.7  2003/12/10 16:19:20  onken
** Changed API of MdfDatasetManager, so that its transparent for user, whether
** he wants to modify itemtags or tags at 1. level.
**
** Complete rewrite of MdfConsoleEngine. It doesn't support a batchfile any more,
** but now a user can give different modify-options at the same time on
** commandline. Other purifications and simplifications were made.
**
** Revision 1.6  2003/11/11 10:55:51  onken
** - debug-mechanism doesn't use debug(..) any more
** - comments purified
** - headers adjustet to debug-modifications
**
** Revision 1.5  2003/10/13 14:46:50  onken
** startModify(...) simplified (uses only putString to put element-values),
** this also allows now inserting and modifying of elements with VRM>1.
** Method getDataset() added.
**
** Revision 1.4  2003/10/01 14:03:27  onken
** Bug fixed, that excluded pixel-data when saving a file loaded into a
** MdfDatasetManager
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
