/*
 *
 *  Copyright (C) 2003-2005, OFFIS
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
 *  Purpose: Class for modifying DICOM files
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 15:40:52 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first
#include "mdfdsman.h"
#include "dcmtk/dcmdata/dcvrsl.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcuid.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


MdfDatasetManager::MdfDatasetManager(const OFBool debug)
:act_file(""),dfile(NULL), dset(NULL)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Constructor, initializes member variables
// Parameters   : debug - [in] enables/disables debug messages (off per default)
// Return Value : none
{
    debug_option=debug;
}


OFCondition MdfDatasetManager::loadFile(const char *file_name,
                                        const E_FileReadMode readMode,
                                        const E_TransferSyntax xfer)
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : loads a file into dataset manager
// Parameters   : file_name - [in] file to be loaded
//                only_dataset - [in] read file without metaheader, if true.
//                               default=false
//                xfer         - [in] try to read with this transfer syntax
//                               default=autodetect
//
// Return Value : An OFCondition, wheter loading was succesfull or not
{
    OFCondition cond;
    //delete old dfile and free memory and reset act_file
    delete dfile;
    act_file="";
    dfile = new DcmFileFormat();
    dset = dfile->getDataset();

    //load file into attribute dfile
    debugMsg(debug_option,"Loading into dataset manager: ", file_name,"");
    cond=dfile->loadFile(file_name, xfer, EGL_noChange, DCM_MaxReadLength, readMode);
    //if there are errors:
    if (cond.bad())
    {
        debugMsg(debug_option,"Failed loading file: ", file_name,"");
        dset=NULL;
    }
    //file susccessfully loaded into dfile:
    else
    {
        //get dataset from file
        debugMsg(debug_option,"Getting dataset from loaded file ", file_name,"");
        dset=dfile->getDataset();
        /*load also pixeldata into memory:
         *Without this command pixeldata wouldn't be included into the file,
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
// Task         : lookup tag string in dictionary and return corresponding key
// Parameters   : tag - [in] tag string for lookup
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
// Author       : Eichelberg / Onken
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
  //look for item index between []
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
  //so far no tag and no item index found. So check if its a dictionary name
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
  //if no return command was processed this far, the token could not be parsed
  return 0; // parse error
}


static DcmItem* getItemFromPath(DcmItem &dataset,
                                const char *location,
                                OFString &message)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : locates a specific item within the given dataset
// Parameters   : dataset - [in] dataset to be searched
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
  DcmItem *result = &dataset;
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
        message=message + "error: attribute not found in dataset (path is '"
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
//                key of the target tag. Path like (0008,1111)[0].(0010,0010) is
//                seperated into path (0008,1111)[0] and key (0010,0010)
// Parameters   : tag_path - [in/out] path
//                key - [out] parsed key of target tag
// Return Value : returns EC_normal if everything is ok, else a parse error
{
    OFString target_tag;
    unsigned int group,elem;
    int lpos,rpos;
    rpos=tag_path.size()-1;
    lpos=rpos;
    if (tag_path[rpos]==')')
    {
        //get opening '(' of target tag; if its not found -> return error
        while ( (tag_path[lpos]!='(') && (lpos>0) ) --lpos;
        if (tag_path[lpos]!='(')
            return makeOFCondition(OFM_dcmdata,22,OF_error,"Invalid tag path!");
        //now lpos and rpos "point" to braces of target tag
        //copy target tag from tag path
        target_tag=tag_path.substr(lpos,rpos-lpos+1);
        //delete target tag from path (inclusive trailing '.')
        tag_path.erase(lpos,tag_path.length()-lpos);
        //if theres a tag path left, remove the '.', too
        if ( (tag_path.length()>0) && (tag_path[tag_path.length()-1]=='.'))
            tag_path.erase(tag_path.length()-1,1);
        //parse target_tag into DcmTagKey
        if (2 != sscanf(target_tag.c_str(),"(%x,%x)", &group, &elem))
            return makeOFCondition(OFM_dcmdata,22,OF_error,
            					   "Invalid target tag!");
        key = DcmTagKey(group,elem);
    }
    else
    //otherwise we could have a dictionary name
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
            return makeOFCondition(OFM_dcmdata,22,OF_error,message.c_str());
        }
    }
    return EC_Normal;
}


OFCondition MdfDatasetManager::modifyOrInsertTag(OFString tag_path,
                                                 const OFString &value,
                                                 const OFBool &only_modify,
                                                 const OFBool update_metaheader)
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
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    OFCondition result;
    DcmElement *elem;
    DcmTagKey key;
    DcmItem *item = dset;
    //seperate tag path and target tag
    result=splitTagPath(tag_path, key);
    //if target tag could not be parsed, return error
    if (!result.good())
        return result;
    //check whether tag has a valid group number, that dcmodify can work with
    if ( (result=hasValidGroupNumber(key)).bad() )
        return result;
    //delete metaheader tags, that are related to this key (if its an UID)
    if (update_metaheader)
        deleteRelatedMetaheaderTag(key);
    //if an item tag was specified, then tag_path still contains a path
    if (tag_path.length()!=0)
    {
        OFString error;
        item=getItemFromPath(*dset, tag_path.c_str(), error);
        //specified item not found -> return error
        if (item==NULL) return makeOFCondition(OFM_dcmdata,22,OF_error,
			                                   error.c_str());
    }

    //if tag is already present, start modify operation
    if ((result=item->findAndGetElement(key, elem, OFFalse)).good())
    {
        return startModify(elem, value);
    }
    //if tag is NOT present, tag is only inserted if user choosed "insert"
    else if (!only_modify)
    {
        return startInsert(item,key,value);
    }
    return result;
}


OFCondition MdfDatasetManager::modifyAllTags(OFString tag_path,
                                             const OFString &value,
                                             const OFBool update_metaheader,
                                             int &count)
// Date         : November, 26th, 2003
// Author       : Michael Onken
// Task         : Modifies all matching tags in dataset to a new value
// Parameters   : tag_path - [in] denotes, which tag to modify
//                value - [in] denotes new value of tag
//                update_metaheader - [in] if true, metaheader uids are updated,
//                                         if related dataset uids are changed
//                count - [out] returns number of modified tags
// Return Value : returns EC_normal if everything is ok, else an error
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");
    DcmTagKey key;
    OFCondition result;
    //split tag_path into the path itself and the target tag
    result=splitTagPath(tag_path,key);
    if ( result.bad() )
        return result;
    //check whether tag with this group can be modified
    result=hasValidGroupNumber(key);
    if ( result.bad() )
        return result;
    //modify metaheader if necessary
    if (update_metaheader)
        deleteRelatedMetaheaderTag(key);
    //this stack will hold result of element search
    DcmStack result_stack;
    DcmObject *elem;
    //get references to all matching tags in dataset and store them in stack
    debugMsg(debug_option,"Looking for occurences of: ", key.toString(),"");
    result=dset->findAndGetElements(key,result_stack);
    //if there are elements found, modify metaheader if necessary
    if (debug_option)
    {
        ofConsole.lockCerr() << "Found " << result_stack.card()
                             << " occurences" << endl;
        ofConsole.unlockCerr();
    }
    //as long there are matching elements left on the stack
    while(result_stack.card()>0 && result.good())
    {
        //get the top element
        elem=result_stack.pop();
        //if user gives e.g. a sequence delimiter,don't try to change it!
        if (elem->isLeaf())
        {
            //and put new value to element
            debugMsg(debug_option, "Accessing existing tag for modify operation","","");
            result=startModify(OFstatic_cast(DcmElement*,elem),value);
            if (result.good()) count++;
        }
        //if user gave "unchangeable" tag:
        else result=makeOFCondition(OFM_dcmdata,22,OF_error,"Unable to modify tag!");
    }
    return result;
}


OFCondition MdfDatasetManager::deleteTag(OFString tag_path,
                                         const OFBool &all_tags)
// Date         : May, 28th, 2003
// Author       : Michael Onken
// Task         : deletes tag in dataset
// Parameters   : tag_path - [in] holds complete path to tag
//                allTags - [in] If true, tag is deleted at all levels of
//                          dataset, else only 1. level is accessed
// Return Value : An OFCondition, whether deleting was succesfull or not
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    //split tag path into item path and target tag
    DcmTagKey key;
    OFCondition result;
    result=splitTagPath(tag_path,key);
    if (result.bad())
        return result;  //error parsing tag path
    //if group is 0,2:abort;deletion of tags with 1,3,5,7 should be allowed
    if ( (key.getGroup()==0) || (key.getGroup()==2) )
        return makeOFCondition(OFM_dcmdata,22,OF_error,
        "Tags with groups 0000 and 0002 can not be deleted directly!");

    //if tag path still contains characters, user wants to modify item tag
    if (tag_path.length()>0)
    {
        OFString error;
        DcmItem *item=NULL;
        item=getItemFromPath(*dset,tag_path.c_str(),error);
        if (item!=NULL)
            result=item->findAndDeleteElement(key, all_tags, all_tags);
        else
            return makeOFCondition(OFM_dcmdata,22,OF_error,error.c_str());
    }
    //other user specified single tag without path
    else
        result=dset->findAndDeleteElement(key, all_tags, all_tags);

    return result;
}


OFCondition MdfDatasetManager::generateNewStudyUID()
// Date         : October, 22th, 2004
// Author       : Michael Onken
// Task         : Generates new 'Study Instance UID' and inserts it into the
//                dataset. 'Series Instance UID' and 'SOP Instance UID' are
//                not affected.
// Parameters   : none
// Return Value : none
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    OFCondition result;
    char uid[100];
    dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
    DcmTagKey key(DCM_StudyInstanceUID);
    //deleteTag(OFstatic_cast(DcmItem*,dset),key);
    result=startInsert( OFstatic_cast(DcmItem*,dset), key , uid);

    return result;
}


OFCondition MdfDatasetManager::generateNewSeriesUID()
// Date         : October, 22th, 2004
// Author       : Michael Onken
// Task         : Generates new 'Series Instance UID' and inserts it into the
//                dataset. 'SOP Instance UID' is not affected.
// Parameters   : none
// Return Value : none

{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    OFCondition result;
    //crate new series uid
    char uid[100];
    dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
    DcmTagKey key(DCM_SeriesInstanceUID);
    //deleteTag(OFstatic_cast(DcmItem*,dset),key);
    result=startInsert( OFstatic_cast(DcmItem*,dset), key , uid);

    return result;
}


OFCondition MdfDatasetManager::generateNewInstanceUID()
// Date         : October, 22th, 2004
// Author       : Michael Onken
// Task         : Generates new 'SOP Instance UID' and inserts it into the
//                dataset. The related metaheader tag ('Media Storage SOP
//                Instance UID') is deleted from metaheader, so that it gets
//                created correctly, if the file  is saved to disk.
// Parameters   : none
// Return Value : none
{
    //if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    OFCondition result;
    char uid[100];
    // force meta-header to refresh SOP Class/Instance UIDs.
    DcmItem *meta_info = dfile->getMetaInfo();
    if (meta_info)
    {
        delete meta_info->remove(DCM_MediaStorageSOPInstanceUID);
        dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
        DcmTagKey key(DCM_SOPInstanceUID);
        result=startInsert( OFstatic_cast(DcmItem*,dset), key , uid);
    }

    return result;
}

OFCondition MdfDatasetManager::saveFile(const char *file_name,
                                        E_TransferSyntax opt_xfer,
                                        E_EncodingType opt_enctype,
                                        E_GrpLenEncoding opt_glenc,
                                        E_PaddingEncoding opt_padenc,
                                        OFCmdUnsignedInt opt_filepad,
                                        OFCmdUnsignedInt opt_itempad,
                                        OFBool opt_dataset)
// Date         : Nov, 5th, 2004
// Author       : Michael Onken
// Task         : Saves current dataset to a file. Caution: After saving
//                MdfDatasetManager keeps working on old filename.
// Parameters : file_name -   [in] filename to save to
//              opt_xfer -    [in] transfer syntax to save to
//                                 (EXS_Unknown: keep old)
//              opt_enctype - [in] write with explicit or implicit
//                                 length encoding
//              opt_glenc -   [in] option to set group lenghth calculation mode
//              opt_padenc -  [in] sets padding option
//              opt_filepad - [in] pad file to a multiple of this options value
//              opt_itempad - [in] pad item to a multiple of this options value
//              opt_dataset - [in] if true:ony write only dataset, else write
//                                 fileformat
// Return Value : returns EC_normal if everything is ok, else an error

{
    //if no file loaded: return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    OFCondition result;
    /* check whether transfer syntax is possible */
    if ( (opt_xfer==EXS_Unknown) || (dfile->canWriteXfer(opt_xfer)) )
    {
        /* check whether pixel data is compressed */
        if (opt_dataset && DcmXfer(opt_xfer).isEncapsulated())
        {
            debugMsg(OFTrue, "Warning: encapsulated pixel data require file format, ignoring --write-dataset", "","");
            opt_dataset = OFFalse;
        }
        /* write DICOM file */
        result = dfile->saveFile(file_name, opt_xfer, opt_enctype, opt_glenc,
                                 opt_padenc,
                                 OFstatic_cast(Uint32, opt_filepad),
                                 OFstatic_cast(Uint32, opt_itempad),
                                 opt_dataset);

    } else {
        debugMsg(debug_option, "Error: no conversion to transfer syntax ",
                 DcmXfer(opt_xfer).getXferName(), "possible! ");
        result = EC_CannotChangeRepresentation;
    }
    //save file
    debugMsg(debug_option,"Saving current dataset to file: ", file_name,"");
    return result;
}


OFCondition MdfDatasetManager::saveFile()
// Date         : December, 17th, 2003
// Author       : Michael Onken
// Task         : Saves current dataset back to a file using original filename
//                and original parameters like transfer syntax, padding etc.
// Return Value : returns EC_normal if everything is ok, else an error
{
    //Save file without changing any parameters
    return saveFile(act_file.c_str(), EXS_Unknown, EET_UndefinedLength,
                    EGL_recalcGL, EPD_noChange, 0, 0, OFFalse);
}


OFCondition MdfDatasetManager::startInsert(DcmItem *item,
                                           DcmTagKey &search_key,
                                           const OFString &value)
// Date         : August, 22th, 2003
// Author       : Michael Onken
// Task         : inserts tag into item with a specific value, overwrites
//                existing tag
// Parameters   : item - [in/out] item, where tag is inserted
//                search_key - [in] specifies tag to be inserted
//                value - [in] value that should be inserted in item
// Return Value : returns an error code as OFCondition, if an error occurs
{
    OFString error_msg, warn_msg, priv_creator;
    OFCondition result;
    //create tag with correct group and element number
    DcmTag *tag=new DcmTag(search_key);
    //make a modifiable copy of the insertion value
    OFString insert_value=value;
    //official tags and private reservation tags are found in dictionary
    if (!isTagInDictionary(search_key))
    {
        //if tag is not in dictionary, check whether its a private reserved tag
        if (isPrivateTag(search_key))
        {
            debugMsg(debug_option, "This is a private tag (non-reservation)","","");
            //check if tag has a reservation in its corresponding item
            if (hasPrivateReservationContext(search_key, item, priv_creator))
            {
                debugMsg(debug_option,"Private tag has related reservation","","");
                //set private creator in tag and try to lookup in dictionary
                tag->setPrivateCreator(priv_creator.c_str());
                tag->lookupVRinDictionary();
                //if tag with _this_ private creator is not found in dictionary
                if (tag->error().bad())
                {
                    warn_msg+="Warning: Private tag with related private creator not found in dictionary!\n";
                    warn_msg+="Warning: Setting tag to VR=UN with empty value";
                    debugMsg(OFTrue, warn_msg,"","");
                    insert_value.clear();
                }

            }
            //found a private tag but not a corresponding reservation in item
            else
            {
                warn_msg+="Warning: Private tag has no reservation context in its item!\n";
                warn_msg+="Warning: Setting tag to VR=UN with empty value";
                debugMsg(OFTrue, warn_msg,"","");
                insert_value.clear();
            }

        }
        //standard tag thats not in dictionary, set VR to UN and value to ""
        else
        {
            warn_msg+="Warning: Tag not in dictionary! Setting to VR=UN with empty value";
            debugMsg(OFTrue,warn_msg,"","");
            insert_value.clear();
        }
    }
    else
    {
        if (isPrivateReservationTag(search_key))
        {
            debugMsg(debug_option,"This is a private reservation tag","","");
        }
        else
        {
            debugMsg(debug_option,"Standard tag was found in dictionary","","");
        }
        tag->lookupVRinDictionary();
    }

    //if there was an uncorrectable error, return with error
    if (error_msg!="")
        return makeOFCondition(OFM_dcmdata,22,OF_error,error_msg.c_str());
    //holds element to insert in item
    DcmElement *elem = NULL;
    result = newDicomElement(elem, *tag);
    //if dicom element could be created, insert in to item and modify to value
    if (result.good())
    {
        result = item->insert(elem,OFTrue);
        if (result.good())
            result = startModify(elem,insert_value);
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
// Return Value : OFCondition, which returns an error code if an error occurs
{
    OFCondition result;
    //start putString function
    result = elem->putString(value.c_str());
    return result;
}


void MdfDatasetManager::deleteRelatedMetaheaderTag(const DcmTagKey &key)
// Date         : October, 22th, 2004
// Author       : Michael Onken
// Task         : If key is the tag for SOPInstanceUID or SOPClassUID, then
//                this function removes the related MediaStorage UIDs from the
//                metaheader. The metaheader is then updated automagically
//                when the file is saved back to disk
// Parameters   : key - [in] tag to examine
// Return Value : none
{
    DcmItem *meta_info=dfile->getMetaInfo();
    if (meta_info)
    {
        if (key==DCM_SOPInstanceUID)
            delete meta_info->remove(DCM_MediaStorageSOPInstanceUID);
        else if (key==DCM_SOPClassUID)
            delete meta_info->remove(DCM_MediaStorageSOPClassUID);
    }
}

DcmDataset* MdfDatasetManager::getDataset()
// Date         : October, 1st, 2003
// Author       : Michael Onken
// Task         : Returns the dataset, that this MdfDatasetManager handles.
//                You should use the returned dataset readonly to avoid
//                sideeffects with other class methods, that modify
//                this dataset.
// Return Value : returns the dataset, this MdfDatasetManager manages and NULL,
//                if no dataset is loaded
{
    return dset;
}

DcmFileFormat* MdfDatasetManager::getFileFormat()
// Date         : October, 1st, 2003
// Author       : Michael Onken
// Task         : Returns the dataset, that this MdfDatasetManager handles.
//                You should use the returned dataset readonly to avoid
//                sideeffects with other class methods, that modify
//                this dataset.
// Return Value : returns the dataset, this MdfDatasetManager manages and NULL,
//                if no dataset is loaded
{
    return dfile;
}

OFString MdfDatasetManager::getFilename() const
// Date         : December, 17st, 2003
// Author       : Michael Onken
// Task         : Returns filename of the file, that's loaded currently.
// Return Value : returns filename and "" if no file is loaded.
{
    return act_file;
}


OFBool MdfDatasetManager::isTagInDictionary(const DcmTagKey &search_key)
//Date          : October, 15th, 2004
//Author        : Michael Onken
//Task          : Returns true, if given tag key can be found in dictionary
//Parameters    : search_key - [in] tag to lookup
//Return Value  : OFTrue if tag could be found, else OFFalse
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(search_key,NULL);
    //successfull lookup in dictionary -> translate to tag and return
    dcmDataDict.unlock();
    if (dicent)
        return OFTrue;
    else return OFFalse;
}


OFCondition MdfDatasetManager::hasValidGroupNumber(const DcmTagKey &key)
//Date          : October, 22th, 2004
//Author        : Michael Onken
//Task          : Checks whether group number is 0,1,2,3,5,7 or FFFF. Then
//                an error is returned, because these groups are illegal
//                or shouldn't be modified
//Parameters    : key - [in] tag, whose group should be examined
//Return Value  : OFCondition with OF_ok if group is ok, else OF_error and
//                an error message is returned
{
    OFString msg;
    unsigned int group=key.getGroup();
    //if group is 0 or 2 return error, these tags shouldn't be modified directly
    if ( (group==0) || (group==2) )
    {
        return makeOFCondition(OFM_dcmdata,22,OF_error,
        "Dcmodify won't insert/modify tags with group 0000 or 0002!");
    }
    //if group is 1,3,5,7 return error. 1,3,5,7,FF are illegal
    if (isPrivateTag(key) && ( (key.getGroup()<=7) || key.getGroup()==0xffff ))
    {
        return makeOFCondition(OFM_dcmdata,22,OF_error,
        "Groups 0001,0003,0005,0007,FFFF are illegal!");
    }
    return makeOFCondition(OFM_dcmdata,22,OF_ok,"");
}


OFBool MdfDatasetManager::isPrivateTag(const DcmTagKey &tag_key)
//Date          : October, 15th, 2004
//Author        : Michael Onken
//Task          : Returns true, if given tag is a private tag (odd group number)
//Parameters    : tag_key - [in] tag key, that should be tested
//Return Value  : OFTrue if tag is private, else OFFalse
{
    if ( (tag_key.getGroup() & 1)==0 )
        return OFFalse;
    return OFTrue;
}


OFBool MdfDatasetManager::isPrivateReservationTag(const DcmTagKey &tag_key)
//Date          : October, 15th, 2004
//Author        : Michael Onken
//Task          : Returns true, if given tag is a private reservation tag.
//                Thats is the case, if tag has the form gggg,00ee (10<ee<FF)
//Parameters    : tag_key - [in] tag key, that should be tested
//Return Value  : OFTrue if tag is a private reservation tag, else OFFalse
{
    int elem=tag_key.getElement();
    if ( (elem >=16) && (elem <=255) )
        return OFTrue;
    else
        return OFFalse;
}


OFBool MdfDatasetManager::hasPrivateReservationContext(const DcmTagKey &tag_key,
                                                       DcmItem *item,
                                                       OFString &priv_creator)
//Date          : October, 15th, 2004
//Author        : Michael Onken
//Task          : Returns true, if given tag is reserved in given item.
//                The private creator string is returned, too.
//Parameters    : tag_key - [in] tag key, that should be checked
//                item    - [in] this item is searched for the reservation
//                priv_creator - [out] private creator string of reservation,
//                               if found
//Return Value  : OFTrue if tag has a matching reservation, else OFFalse
{
    DcmElement *reservation_elem;
    DcmTagKey reservator(calcPrivateReservationTag(tag_key));
    OFCondition cond =
        item->findAndGetElement(reservator, reservation_elem, OFFalse);
    //return if there is no reservation for this private tag
    if (cond.bad())
        return false;

    //we have a reservation for this private tag, get private creator name
    reservation_elem->getOFString(priv_creator,0);
    return true;
}


DcmTagKey MdfDatasetManager::calcPrivateReservationTag(const DcmTagKey &tag_key)
//Date          : October, 15th, 2004
//Author        : Michael Onken
//Task          : Calculates from given private tag the reservation tag, that
//                would make a reservation for this private tag
//Parameters    : tag_key - [in] tag key, whose reservation should be calculated
//Return Value  : The tag key, that would reserve given private tag
{
    int elem=tag_key.getElement();
    Uint16 reserved_number=elem;
    //isolates the xx from given tag: gggg,yzxx
    reserved_number >>= 8;
    //group stays the same, element was calculated above
    DcmTagKey reservation_tag(tag_key.getGroup(),reserved_number);
    return reservation_tag;

}

void MdfDatasetManager::debugMsg(const OFBool &condition,
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


MdfDatasetManager::~MdfDatasetManager()
// Date         : May, 13th, 2003
// Author       : Michael Onken
// Task         : Destructor, cleans up members
// Parameters   : None
// Return Value : none
{
    //cleanup
    delete dfile;
}


/*
** CVS/RCS Log:
** $Log: mdfdsman.cc,v $
** Revision 1.18  2005-12-08 15:40:52  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.17  2005/12/02 09:20:08  joergr
** Added new file read mode that makes it possible to distinguish between DICOM
** files, datasets and other non-DICOM files.  For this reason, the last
** parameter of method loadFile() changed from OFBool to E_FileReadMode.
**
** Revision 1.16  2005/11/14 15:00:14  joergr
** Made method getFilename() const.
**
** Revision 1.15  2004/11/05 17:17:24  onken
** Added input and output options for dcmodify. minor code enhancements.
**
** Revision 1.14  2004/10/22 16:53:26  onken
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
** Revision 1.13  2004/05/14 12:08:36  onken
** Additional documentation added.
**
** Revision 1.12  2004/04/19 14:45:07  onken
** Restructured code to avoid default parameter values for "complex types" like
** OFString. Required for Sun CC 2.0.1.
**
** Revision 1.11  2004/04/07 12:25:53  joergr
** Changed type of integer variables to unsigned to avoid compiler warnings
** reported by gcc.
**
** Revision 1.10  2004/02/04 16:03:38  joergr
** Removed pointer declaration from parameter "resultStack" in method
** findAndGetElements().
**
** Revision 1.9  2004/01/16 10:53:53  joergr
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
