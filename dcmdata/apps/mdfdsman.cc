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
 *  Purpose: Class for modifying DICOM files
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 10:42:43 $
 *  CVS/RCS Revision: $Revision: 1.29 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first

#include "mdfdsman.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dcistrmf.h"  /* for class DcmInputFileStream */

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


static OFLogger mdfdsmanLogger = OFLog::getLogger("dcmtk.dcmdata.mdfdsman");

MdfDatasetManager::MdfDatasetManager()
: current_file(""),
  dfile(NULL),
  dset(NULL),
  ignore_un_modifies(OFFalse)
{
}


OFCondition MdfDatasetManager::loadFile(const char *file_name,
                                        const E_FileReadMode readMode,
                                        const E_TransferSyntax xfer)
{
    OFCondition cond;
    // delete old dfile and free memory and reset current_file
    delete dfile;
    current_file = "";
    dfile = new DcmFileFormat();
    dset = dfile->getDataset();

    // load file into dfile
    OFLOG_INFO(mdfdsmanLogger, "Loading file into dataset manager: " << file_name);
    cond = dfile->loadFile(file_name, xfer, EGL_noChange, DCM_MaxReadLength, readMode);
    // if there are errors:
    if (cond.bad())
    {
        dset = NULL;
    }
    // file successfully loaded into dfile
    else
    {
        // get dataset from file
        OFLOG_INFO(mdfdsmanLogger, "Getting dataset from loaded file: " << file_name);
        dset=dfile->getDataset();
        /* load also pixeldata into memory:
         * Without this command pixeldata wouldn't be included into the file,
         * that's saved after modifying, because original filename was renamed
         * meanwhile
         */
        dset->loadAllDataIntoMemory();
        // save filename to member variable
        current_file = file_name;
    }
    return cond;
}


static DcmTagKey getTagKeyFromDictionary(OFString tag)
{
    DcmTagKey key(0xffff,0xffff);
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(tag.c_str());
    // successfull lookup in dictionary -> translate to tag and return
    if (dicent)
    {
        key = dicent->getKey();
    }
    dcmDataDict.unlock();
    return key;
}


static int readNextToken(const char *c, int& pos, DcmTagKey& key, Uint32& idx)
{
  OFString aString;
  int lpos = pos;
  int spos = 0;
  if (c[lpos]=='\0') return -1; // EOF
  if (c[lpos]=='.')
  {
    ++pos;
    return 3; // period
  }
  // look for item index between []
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
  // look for tag between ()
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
  // so far no tag and no item index found. So check if it's a dictionary name
  spos = lpos;
  while ( ((c[lpos] >= 'a')&&(c[lpos] <= 'z')) ||
          ((c[lpos] >= 'A')&&(c[lpos] <= 'Z')) ||
          ((c[lpos] >= '0')&&(c[lpos] <= '9'))) ++lpos;
  aString.append(c + spos, (lpos-spos));
  key=getTagKeyFromDictionary(aString);
  // if key was found in dictionary, return 1 for tag key
  if ( (key.getGroup()!=0xffff) && (key.getElement()!=0xffff) )
  {
    pos=lpos;
    return 1; // tag key
  }
  // if no return command was processed this far, the token could not be parsed
  return 0; // parse error
}


static DcmItem* getItemFromPath(DcmItem &dataset,
                                const char *location,
                                OFString &message)
{
  DcmTagKey key;
  Uint32 idx = 0;
  int pos = 0;
  int token = 0;
  int expected = 1; // first expected is a tagkey
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
      message=message + "parse error in path '" + location + "'";
      return NULL;
    }
    if (token == -1)
    {
      if (!finished)
      {
        message=message + "Error: path '" + location + "' incomplete";
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
        message=message + "Error: attribute not found in dataset (path is '"
            + location + "')";
        return NULL;
      }
      if (stack.top()->ident() == EVR_SQ)
      {
        sq = OFstatic_cast(DcmSequenceOfItems *, stack.top());
      } else {
        message=message + "Error: attribute is not a sequence (path is '" + location + "')";
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
        message=message + "Error: sequence not found in path '" + location +"'";
        return NULL;
      }
      if (idx >= sq->card())
      {
        message=message + "Error: cannot allocate item in sequence (path is '" + location + "')";
        return NULL;
      }
      result = sq->getItem(idx);
      if (result == NULL)
      {
        message=message + "Error: item not found in path '" + location + "'";
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


static OFCondition splitTagPath(OFString &tag_path,
                                DcmTagKey &key)
{
    OFString target_tag;
    unsigned int group,elem;
    int lpos,rpos;
    rpos=tag_path.size()-1;
    lpos=rpos;
    if (tag_path[rpos]==')')
    {
        // get opening '(' of target tag; if its not found -> return error
        while ( (tag_path[lpos]!='(') && (lpos>0) ) --lpos;
        if (tag_path[lpos]!='(')
            return makeOFCondition(OFM_dcmdata,22,OF_error,"Invalid tag path!");
        // now lpos and rpos "point" to braces of target tag
        // copy target tag from tag path
        target_tag=tag_path.substr(lpos,rpos-lpos+1);
        // delete target tag from path (inclusive trailing '.')
        tag_path.erase(lpos,tag_path.length()-lpos);
        // if there's a tag path left, remove the '.', too
        if ( (tag_path.length()>0) && (tag_path[tag_path.length()-1]=='.'))
            tag_path.erase(tag_path.length()-1,1);
        // parse target_tag into DcmTagKey
        if (2 != sscanf(target_tag.c_str(),"(%x,%x)", &group, &elem))
            return makeOFCondition(OFM_dcmdata,22,OF_error,"Invalid target tag!");
        key = DcmTagKey(group,elem);
    }
    else
    // otherwise we could have a dictionary name
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
            message.append(" not found in dictionary!");
            return makeOFCondition(OFM_dcmdata,22,OF_error,message.c_str());
        }
    }
    return EC_Normal;
}


OFCondition MdfDatasetManager::modifyOrInsertPath(OFString tag_path,
                                                  const OFString &value,
                                                  const OFBool &only_modify,
                                                  const OFBool update_metaheader,
                                                  const OFBool ignore_missing_tags,
                                                  const OFBool no_reservation_checks)
{
  // if no file loaded: return an error
  if (dfile == NULL)
      return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

  // find or create specified path
  DcmPathProcessor proc;
  proc.checkPrivateReservations(!no_reservation_checks);
  OFCondition result = proc.findOrCreatePath(dset, tag_path, !only_modify /*create if desired*/);
  // if desired, handle tag not found as being not an error
  if ( (result == EC_TagNotFound) && only_modify && ignore_missing_tags )
      return EC_Normal;
  if (result.bad()) return result;
  OFList<DcmPath*> resultPaths;
  Uint32 numResultPaths = proc.getResults(resultPaths);
  if (numResultPaths == 0) return EC_IllegalCall;

  // general validity checking; must only be done for one result
  OFListIterator(DcmPath*) resultPath = resultPaths.begin();
  // verify that groups 0 (invalid) and 2 (meta header) were not used
  if ( (*resultPath)->containsGroup(0) || (*resultPath)->containsGroup(2) )
      return makeOFCondition(OFM_dcmdata,22,OF_error, "Cannot insert/modify tags with group 0000 or 0002!");
  // also - according to the standard -  groups 1,3,5,7,FF are illegal
  if ( (*resultPath)->containsGroup(1) || (*resultPath)->containsGroup(3) ||
       (*resultPath)->containsGroup(5) || (*resultPath)->containsGroup(7) ||
       (*resultPath)->containsGroup(0xffff) )
      return makeOFCondition(OFM_dcmdata, 22, OF_error, "Groups 0001,0003,0005,0007,FFFF are illegal!");

  DcmPathNode *lastElement = (*resultPath)->back();
  if (lastElement == NULL) return EC_IllegalCall;
  DcmObject *obj = lastElement->m_obj;
  if (obj == NULL) return EC_IllegalCall;
  // if object at the end is not a leaf, the insertion is completed (or must fail)
  if (!obj->isLeaf())
  {
      // if user specified a value to be inserted into non-leaf element, return error
      if (value.length() != 0)
          return makeOFCondition(OFM_dcmdata,22,OF_error,"Cannot put value into non-leaf elements!");
      // non-leaf elements (items/sequences) cannot just be modified
      if (only_modify)
         return makeOFCondition(OFM_dcmdata,22,OF_error,"Cannot modify non-leaf elements!");
      // we have inserted an item/sequence at the end -> job completed
      return EC_Normal;
  }

  // start modifying element value as desired
  resultPath = resultPaths.begin();
  while (resultPath != resultPaths.end())
  {
      lastElement = (*resultPath)->back();
      if (lastElement == NULL) return EC_IllegalCall;
      // if tag is already present, start modify operation
      DcmElement *elem = OFstatic_cast(DcmElement*, lastElement->m_obj);
      if (elem == NULL) return EC_IllegalCall;
      result = startModify(elem, value);
      if (result.bad()) return result;
      if (update_metaheader)
          deleteRelatedMetaheaderTag(elem->getTag());
      resultPath++;
  }
  return EC_Normal;
}


OFCondition MdfDatasetManager::modifyOrInsertFromFile(OFString tag_path,
                                                      const OFString &filename,
                                                      const OFBool &only_modify,
                                                      const OFBool update_metaheader,
                                                      const OFBool ignore_missing_tags,
                                                      const OFBool no_reservation_checks)
{
  // if no file loaded: return an error
  if (dfile == NULL)
      return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

  // first, perform some basic checks on the specified file(name)
  if (filename.empty())
      return makeOFCondition(OFM_dcmdata, 22, OF_error, "No filename specified to read value from!");
  if (!OFStandard::fileExists(filename))
      return makeOFCondition(OFM_dcmdata, 22, OF_error, "File to read value from does not exist!");
  if (!OFStandard::isReadable(filename))
      return makeOFCondition(OFM_dcmdata, 22, OF_error, "File to read value from is not readable!");

  // find or create specified path
  DcmPathProcessor proc;
  proc.checkPrivateReservations(!no_reservation_checks);
  OFCondition result = proc.findOrCreatePath(dset, tag_path, !only_modify /*create if desired*/);
  // if desired, handle tag not found as being not an error
  if ((result == EC_TagNotFound) && only_modify && ignore_missing_tags)
      return EC_Normal;
  if (result.bad()) return result;
  OFList<DcmPath *> resultPaths;
  Uint32 numResultPaths = proc.getResults(resultPaths);
  if (numResultPaths == 0) return EC_IllegalCall;

  // general validity checking; must only be done for one result
  OFListIterator(DcmPath*) resultPath = resultPaths.begin();
  // verify that groups 0 (invalid) and 2 (meta header) were not used
  if ((*resultPath)->containsGroup(0) || (*resultPath)->containsGroup(2))
      return makeOFCondition(OFM_dcmdata,22,OF_error, "Cannot insert/modify tags with group 0000 or 0002!");
  // also - according to the standard -  groups 1,3,5,7,FF are illegal
  if ((*resultPath)->containsGroup(1) || (*resultPath)->containsGroup(3) ||
      (*resultPath)->containsGroup(5) || (*resultPath)->containsGroup(7) ||
      (*resultPath)->containsGroup(0xffff))
      return makeOFCondition(OFM_dcmdata, 22, OF_error, "Groups 0001,0003,0005,0007,FFFF are illegal!");

  DcmPathNode *lastElement = (*resultPath)->back();
  if (lastElement == NULL) return EC_IllegalCall;
  DcmObject *obj = lastElement->m_obj;
  if (obj == NULL) return EC_IllegalCall;
  // if object at the end is not a leaf, the insertion/modification fails
  if (!obj->isLeaf())
      return makeOFCondition(OFM_dcmdata, 22, OF_error, "Cannot put value into non-leaf elements!");

  // start modifying element value as desired
  resultPath = resultPaths.begin();
  while (resultPath != resultPaths.end())
  {
      lastElement = (*resultPath)->back();
      if (lastElement == NULL) return EC_IllegalCall;
      // if tag is already present, start modify operation
      DcmElement *elem = OFstatic_cast(DcmElement*, lastElement->m_obj);
      if (elem == NULL) return EC_IllegalCall;
      // check whether VR is "unknown"
      DcmEVR vr = elem->getTag().getEVR();
      if (ignore_un_modifies && ((vr == EVR_UN) || (vr == EVR_UNKNOWN) || (vr == EVR_UNKNOWN2B)))
      {
          OFLOG_WARN(mdfdsmanLogger, "will not write value to attribute having VR=UN: " << elem->getTag());
          return EC_Normal;
      }
      // create stream object for binary file
      DcmInputFileStream fileStream(filename.c_str());
      result = fileStream.status();
      if (result.good())
      {
          const size_t fileLen = OFStandard::getFileSize(filename);
          if (fileLen & 1)
              return makeOFCondition(OFM_dcmdata, 22, OF_error, "Cannot insert/modify value with odd length from file!");
          // read element value from binary file (requires even length)
          result = elem->createValueFromTempFile(fileStream.newFactory(), fileLen, EBO_LittleEndian);
      }
      if (result.bad()) return result;
      if (update_metaheader)
          deleteRelatedMetaheaderTag(elem->getTag());
      resultPath++;
  }
  return EC_Normal;
}


OFCondition MdfDatasetManager::modifyAllTags(OFString tag_path,
                                             const OFString &value,
                                             const OFBool update_metaheader,
                                             int &count,
                                             const OFBool ignore_missing_tags)
{
    // if no file loaded: return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");
    DcmTagKey key;
    OFCondition result;

    // split tag_path into the path itself and the target tag
    result=splitTagPath(tag_path, key);
    if ( result.bad() )
        return result;

    // check whether tag with this group can be modified
    if (!key.hasValidGroup())
        return makeOFCondition(OFM_dcmdata,22,OF_error,"Invalid group number!");

    // modify metaheader if necessary; not necessary if not a main level tag
    if ( update_metaheader && tag_path.empty() )
        deleteRelatedMetaheaderTag(key);
    // this stack will hold result of element search
    DcmStack result_stack;
    DcmObject *elem;
    // get references to all matching tags in dataset and store them in stack
    OFLOG_DEBUG(mdfdsmanLogger, "looking for occurences of: " << key.toString());
    result=dset->findAndGetElements(key, result_stack);
    // if there are elements found, modify metaheader if necessary
    OFLOG_DEBUG(mdfdsmanLogger, "found " << result_stack.card() << " occurences");
    // as long there are matching elements left on the stack
    while( result_stack.card() > 0 && result.good() )
    {
        // get the top element
        elem=result_stack.pop();
        // if user gives e.g. a sequence delimiter,don't try to change it!
        if (elem->isLeaf())
        {
            // and put new value to element
            OFLOG_DEBUG(mdfdsmanLogger, "accessing existing tag for modify operation");
            result=startModify(OFstatic_cast(DcmElement*,elem),value);
            if (result.good()) count++;
        }
        // if user gave "unchangeable" tag:
        else result = makeOFCondition(OFM_dcmdata,22,OF_error,"Unable to modify tag!");
    }
    // if desired, handle "tag not found" as being OK
    if (ignore_missing_tags && (result == EC_TagNotFound))
        return EC_Normal;
    return result;
}


OFCondition MdfDatasetManager::deleteTag(OFString tag_path,
                                         const OFBool all_tags,
                                         const OFBool ignore_missing_tags)
{

  // if no file loaded: return an error
  if (dfile == NULL)
      return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

  OFCondition result;
  if (all_tags)
  {
      // split tag path into item path and target tag
      DcmTagKey key;
      result = splitTagPath(tag_path, key);
      if (result.bad())
          return result;  // error parsing tag path

      // if group is 0 or 2: abort; deletion of tags with 1,3,5,7 should be allowed
      if ( (key.getGroup() == 0) || (key.getGroup() == 2) )
          return makeOFCondition(OFM_dcmdata,22,OF_error, "Cannot delete tags with group 0000 or 0002!");

      // if tag path still contains characters, user wants to modify item tag
      if (tag_path.length() > 0)
      {
          OFString error;
          DcmItem *item=NULL;
          item = getItemFromPath(*dset, tag_path.c_str(), error);
          if (item != NULL)
              result = item->findAndDeleteElement(key, all_tags, all_tags);
          else
              return makeOFCondition(OFM_dcmdata,22,OF_error,error.c_str());
      }
      // other user specified single tag without path
      else
      {
          result = dset->findAndDeleteElement(key, all_tags, all_tags);
      }
      if (ignore_missing_tags && (result == EC_TagNotFound))
          return EC_Normal;
      return result;
  }

  // do not delete all tags -> use path functions
  DcmPathProcessor pathProc;

  Uint32 numDeletions = 0;
  result = pathProc.findOrDeletePath(dset, tag_path, numDeletions);
  if (ignore_missing_tags && (result == EC_TagNotFound))
      return EC_Normal;
  return result;
}


OFCondition MdfDatasetManager::deletePrivateData()
{
  // if no file loaded : return an error
  if (dfile == NULL)
      return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

  DcmStack stack;
  DcmObject *dobj = NULL;
  DcmTagKey tag;
  OFCondition status = dset->nextObject(stack, OFTrue);
  while (status.good())
  {
      dobj = stack.top();
      tag = dobj->getTag();
      if (tag.getGroup() & 1) // private tag ?
      {
          stack.pop();
          delete OFstatic_cast(DcmItem *, (stack.top()))->remove(dobj);
      }
      status = dset->nextObject(stack, OFTrue);
  }
  return EC_Normal;
}


OFCondition MdfDatasetManager::generateAndInsertUID(const DcmTagKey& uidKey)
{
    // if no file loaded : return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    OFCondition result;
    char uid[100];
    if (uidKey == DCM_StudyInstanceUID)
        dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
    else if (uidKey == DCM_SeriesInstanceUID)
        dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
    else if (uidKey == DCM_SOPInstanceUID)
    {
        dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
        // force meta-header to refresh SOP Class/Instance UIDs.
        DcmItem *meta_info = dfile->getMetaInfo();
        if (meta_info)
        {
            delete meta_info->remove(DCM_MediaStorageSOPInstanceUID);
        }
    }
    else return EC_IllegalCall;
    result = dset->putAndInsertString(uidKey, uid);
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
{
    // if no file loaded: return an error
    if (dfile==NULL)
        return makeOFCondition(OFM_dcmdata,22,OF_error,"No file loaded yet!");

    OFCondition result;
    /* check whether transfer syntax is possible */
    if ( (opt_xfer==EXS_Unknown) || (dfile->canWriteXfer(opt_xfer)) )
    {
        /* check whether pixel data is compressed */
        if (opt_dataset && DcmXfer(opt_xfer).isEncapsulated())
        {
            OFLOG_WARN(mdfdsmanLogger, "encapsulated pixel data require file format, ignoring --write-dataset");
            opt_dataset = OFFalse;
        }
        /* write DICOM file */
        result = dfile->saveFile(file_name, opt_xfer, opt_enctype, opt_glenc,
                                 opt_padenc,
                                 OFstatic_cast(Uint32, opt_filepad),
                                 OFstatic_cast(Uint32, opt_itempad),
                                 (opt_dataset) ? EWM_dataset : EWM_fileformat);

    } else {
        OFLOG_DEBUG(mdfdsmanLogger, "no conversion to transfer syntax " << DcmXfer(opt_xfer).getXferName() << " possible!");
        result = EC_CannotChangeRepresentation;
    }
    // save file
    OFLOG_INFO(mdfdsmanLogger, "Saving current dataset to file: " << file_name);
    return result;
}


OFCondition MdfDatasetManager::saveFile()
{
    // save file without changing any parameters
    return saveFile(current_file.c_str(), EXS_Unknown, EET_UndefinedLength,
                    EGL_recalcGL, EPD_noChange, 0, 0, OFFalse);
}


OFCondition MdfDatasetManager::startModify(DcmElement *elem,
                                           const OFString &value)
{
    OFCondition result;
    DcmEVR vr = elem->getTag().getEVR();
    if ( ignore_un_modifies && ((vr == EVR_UN) || (vr == EVR_UNKNOWN) || (vr == EVR_UNKNOWN2B)))
    {
      OFLOG_WARN(mdfdsmanLogger, "will not write value to attribute having VR=UN: " << elem->getTag().toString());
      return EC_Normal;
    }
    // start putString function being defined on all VRs
    result = elem->putString(value.c_str());
    return result;
}


void MdfDatasetManager::deleteRelatedMetaheaderTag(const DcmTagKey &key)
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
{
    return dset;
}


DcmFileFormat* MdfDatasetManager::getFileFormat()
{
    return dfile;
}


OFString MdfDatasetManager::getFilename() const
{
    return current_file;
}


OFBool MdfDatasetManager::isTagInDictionary(const DcmTagKey &search_key)
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(search_key,NULL);
    // successfull lookup in dictionary -> translate to tag and return
    dcmDataDict.unlock();
    if (dicent)
        return OFTrue;
    else return OFFalse;
}


void MdfDatasetManager::setModifyUNValues(OFBool modifyUNValues)
{
  ignore_un_modifies = !modifyUNValues;
}


MdfDatasetManager::~MdfDatasetManager()
{
    // cleanup
    delete dfile;
}


/*
** CVS/RCS Log:
** $Log: mdfdsman.cc,v $
** Revision 1.29  2010-11-01 10:42:43  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.28  2010-10-14 13:13:30  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.27  2010-05-20 15:53:58  joergr
** Added support for reading the value of insert/modify statements from a file.
** Slightly modified log messages and log levels in order to be more consistent.
** Removed some unnecessary include directives.
**
** Revision 1.26  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.25  2009-08-21 09:25:13  joergr
** Added parameter 'writeMode' to save/write methods which allows for specifying
** whether to write a dataset or fileformat as well as whether to update the
** file meta information or to create a new file meta information header.
**
** Revision 1.24  2009-07-08 16:10:36  onken
** Adapted dcmodify to changes in DcmPath API.
**
** Revision 1.23  2009-01-16 10:03:13  onken
** Code simplification for VR checking.
**
** Revision 1.22  2009-01-15 16:11:55  onken
** Reworked dcmodify to work with the new DcmPath classes for supporting
** wildcard paths and automatic insertion of missing attributes and items.
** Added options for private tag handling and modification of UN values and
** for ignoring errors resulting from missing tags during modify and erase
** operations. Further cleanups.
**
** Revision 1.21  2006-08-15 15:50:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.20  2005/12/16 09:08:33  onken
** - Added variable initialization to avoid compiler warning
**
** Revision 1.19  2005/12/15 10:23:41  joergr
** Replaced "true" by "OFTrue" and "false" by "OFFalse".
**
** Revision 1.18  2005/12/08 15:40:52  meichel
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
