/*
 *
 *  Copyright (C) 2008, OFFIS
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
 *  Purpose: Class definitions for accessing DICOM dataset structures (items,
 *           sequences and leaf elements via string-based path access.
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2008-12-12 12:07:11 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/ofstd/ofstd.h"


/*******************************************************************/
/*              Implementation of class DcmPath                    */
/*******************************************************************/

DcmPath::DcmPath() : m_result()
{
}


DcmPath::DcmPath(const OFList<DcmPathNode*>& currentPath)
{
  OFListIterator(DcmPathNode*) it = currentPath.begin();
  OFListConstIterator(DcmPathNode*) endOfPath = currentPath.end();
  while (it != endOfPath)
  {
    m_result.push_back(new DcmPathNode( (*it)->m_obj, (*it)->m_itemNo ));
    it++;
  }
}


void DcmPath::append(DcmPathNode* node)
{
  if (node != NULL)
    m_result.push_back(node); // do any validity checking?
}


OFListIterator(DcmPathNode*) DcmPath::begin()
{
  return m_result.begin();
}


DcmPathNode* DcmPath::back()
{
  return m_result.back();
}


OFListIterator(DcmPathNode*) DcmPath::end()
{
  return m_result.end();
}


Uint32 DcmPath::size() const
{
  return m_result.size();
}


OFBool DcmPath::empty() const
{
  return (m_result.size() == 0);
}


OFString DcmPath::toString() const
{
  OFListIterator(DcmPathNode*) it = m_result.begin();
  OFListConstIterator(DcmPathNode*) endOfList = m_result.end();
  OFString pathStr; DcmEVR vr; DcmObject* obj;
  char buf[500];
  while (it != endOfList)
  {
    if ( ((*it) == NULL) || ((*it)->m_obj == NULL) )
      return "Invalid search result";
    obj = (*it)->m_obj;
    vr = obj->ident();
    if ((vr == EVR_SQ) || ( obj->isLeaf()) )
    {
      pathStr.append( OFconst_cast(DcmTag*, &obj->getTag())->getTagName() );
      it++;
    }
    else if ( (vr == EVR_item) || (vr == EVR_dataset) )
    {
      sprintf(buf, "[%u]", (*it)->m_itemNo);
      pathStr.append(buf);
      it++;
      if (it != endOfList) pathStr.append(".");
    }
    else
    {
      pathStr.append("<UNKNOWN>");
      it++;
    }
  }
  return pathStr;
}


DcmPath::~DcmPath()
{
  // free dynamically allocated memory
  while (m_result.size() != 0)
  {
    DcmPathNode* node = m_result.front();
    delete node; node = NULL;
    m_result.pop_front();
  }
}


/*******************************************************************/
/*          Implementation of class DcmPathProcessor               */
/*******************************************************************/

DcmPathProcessor::DcmPathProcessor() :
  m_currentPath(),
  m_results(),
  m_createIfNecessary(OFFalse)
{
}


OFCondition DcmPathProcessor::findOrCreatePath(DcmObject* obj,
                                        const OFString& path,
                                        const OFBool createIfNecessary)
{
  // check input parameters
  if ( (obj == NULL) || path.empty())
    return EC_IllegalParameter;

  clear();
  m_createIfNecessary = createIfNecessary;

  // do real work in private member functions
  OFString pathCopy = path;
  if ((obj->ident() == EVR_item) || (obj->ident() == EVR_dataset))
    return findOrCreateItemPath(OFstatic_cast(DcmItem*, obj), pathCopy);
  else if (obj->ident() == EVR_SQ)
    return findOrCreateSequencePath(OFstatic_cast(DcmSequenceOfItems*, obj), pathCopy);
  else
    return EC_IllegalParameter;

}


/* protected helper functions */

OFCondition DcmPathProcessor::findOrCreateItemPath(DcmItem* item,
                                            OFString& path)
{
  if (item == NULL)
    return EC_IllegalParameter;

  if (path.length() == 0)
      return EC_IllegalParameter;

  OFString restPath(path);
  OFCondition status = EC_Normal;
  DcmTag tag;
  OFBool newlyCreated = OFFalse; // denotes whether an element was created
  DcmElement *elem = NULL;
  DcmPath* currentResult = NULL;

  // parse tag
  status = parseTagFromPath(restPath, tag);
  if (status.bad())
    return status;

  // insert element or sequence
  if ( !(item->tagExists(tag)) ) // do not to overwrite existing tags
  {
    if (m_createIfNecessary)
    {
      elem = newDicomElement(tag);
      if (elem == NULL)
        return EC_IllegalCall;
      status = item->insert(elem, OFTrue);
      if (status.bad())
          return status;
      newlyCreated = OFTrue;
    }
    else
      return EC_TagNotFound;
  }

  // get element
  status = item->findAndGetElement(tag, elem);
  if (status.bad())
    return EC_CorruptedData; // should not happen

  // start recursion if element was a sequence
  if (tag.getEVR() == EVR_SQ)
  {
    DcmSequenceOfItems* seq = NULL;
    seq = OFstatic_cast(DcmSequenceOfItems*, elem);
    if (!seq)
      status = EC_IllegalCall; // should not happen
    else
    {
      // if sequence could be inserted and there is nothing more to do: add current path to results and return success
      if (restPath.length() == 0)
      {
          currentResult = new DcmPath(m_currentPath);
          currentResult->append(new DcmPathNode(elem,0));
          m_results.push_back(currentResult);
          return EC_Normal;
      }
      // start recursion if there is path left
      DcmPathNode* node = new DcmPathNode(seq, 0);
      m_currentPath.push_back(node);
      status = findOrCreateSequencePath(seq, restPath);
      m_currentPath.pop_back(); // avoid side effects
      delete node;
     }
  }
  else if (restPath.length() == 0) // we inserted a leaf element: path must be completed
  {
     // add element and add current path to overall results; then return success
    {
      currentResult = new DcmPath(m_currentPath);
      currentResult->append(new DcmPathNode(elem, 0));
      m_results.push_back(currentResult);
      return EC_Normal;
    }
  }
  else // we inserted a leaf element but there is path left -> error
    status = makeOFCondition(OFM_dcmdata, 25, OF_error, "Invalid Path: Non-sequence tag found with rest path following");

  // in case of errors: delete result path copy and delete DICOM element if it was newly created
  if ( status.bad() && (elem != NULL) )
  {
    m_results.remove(currentResult); // remove from search result
    if (currentResult)
    {
      delete currentResult;
      currentResult = NULL;
    }
    if (newlyCreated) // only delete from this dataset and memory if newly created ("undo")
    {
      if (item->findAndDeleteElement(tag).bad())
          delete elem; // delete manually if not found in dataset
    }
    elem = NULL;
  }
  return status;
}


OFCondition DcmPathProcessor::findOrCreateSequencePath(DcmSequenceOfItems* seq,
                                                OFString& path)
{
  if (seq == NULL)
    return EC_IllegalParameter;

  // prepare variables
  OFString restPath(path);
  OFCondition status = EC_Normal;
  DcmItem *resultItem = NULL;
  Uint32 itemNo = 0;
  Uint32 newlyCreated = 0;    // number of items created (appended) (only non-wildcard mode)
  Uint32 newPathsCreated = 0; // wildcard mode: number of paths found

  // parse item number
  OFBool isWildcard = OFFalse;
  status = parseItemNoFromPath(restPath, itemNo, isWildcard);
  if (status.bad())
    return status;

  // wildcard code: add result path for every matching item
  if (isWildcard)
  {
    // if there are no items -> no results are found
    Uint32 numItems = seq->card();
    if (numItems == 0)
      return EC_TagNotFound;

    // copy every item to result
    for (itemNo = 0; itemNo < numItems; itemNo++)
    {
      DcmItem* oneItem = seq->getItem(itemNo);
      /* if we found an item that matches, copy current result path, then
         add the item found and finally start recursive search for
         that item.
       */
      if (oneItem != NULL)
      {
        // if the item was the last thing to parse, add list to results and return
        if (restPath.length() == 0)
        {
          DcmPathNode* itemNode = new DcmPathNode(oneItem, itemNo);
          DcmPath* currentResult = new DcmPath(m_currentPath);
          currentResult->append(itemNode);
          m_results.push_back(currentResult);
          newPathsCreated++;
        }
        // else there is path left: continue searching in the new item
        else
        {
         DcmPathNode* itemNode = new DcmPathNode(oneItem, itemNo);
         m_currentPath.push_back(itemNode);
         status = findOrCreateItemPath(oneItem, restPath);
         m_currentPath.pop_back(); // avoid side effects
         delete itemNode; itemNode = NULL;
         if (status.bad()) // we did not find the path in that item
         {
           if (status != EC_TagNotFound)
             return status;
         }
         else
         {
           newPathsCreated++;
         }
        }
      }
      else // should be possible to get every item, however...
          return EC_IllegalCall;
    }
    // if there was at least one result, success can be returned
    if (newPathsCreated != 0)
    {
        return EC_Normal;
    }
    else
        return EC_TagNotFound;
  }


  /* no wildcard, just select single item or create it if necessary */

  // if item already exists, just grab a reference
  if (itemNo < seq->card())
      resultItem = seq->getItem(itemNo);
  // if item does not exist, create new if desired
  else if (m_createIfNecessary)
  {
      // create and insert items until desired item count is reached
      while ( (seq->card() <= itemNo) || (status.bad()) )
      {
          resultItem = new DcmItem();
          if (!resultItem) return EC_MemoryExhausted;
          status = seq->insert(resultItem);
          if (status.bad())
              delete resultItem;
          else
              newlyCreated++;
      }
  }
  // item does not exist and should not be created newly, return "path not found"
  else
      return EC_TagNotFound;

  // at this point, the item has been obtained and everyhthing is fine so far

  // finding/creating the path was successful. now check whether there is more to do
  if (restPath.length() != 0)
  {
    // push new item to result path and continue
    DcmPathNode* itemNode = new DcmPathNode(resultItem, itemNo);
    m_currentPath.push_back(itemNode);
    status = findOrCreateItemPath(resultItem, restPath);
    m_currentPath.pop_back(); // avoid side effects to input parameter
    delete itemNode; itemNode = NULL;
    // in case of no success, delete any items that were newly created and return error
    if (status.bad())
    {
      for (Uint32 i=newlyCreated; i > 0; i--)
      {
          DcmItem *todelete = seq->remove(i-1);
          if (todelete != NULL)
          {
            delete todelete;
            todelete = NULL;
          }
      }
      return status;
    }
  }
  else // finally everything was successful
  {
    DcmPathNode* itemNode = new DcmPathNode(resultItem, itemNo);
    m_currentPath.push_back(itemNode);
    m_results.push_back(new DcmPath(m_currentPath));
    m_currentPath.pop_back(); // avoid side effects
    delete itemNode; itemNode = NULL;
    status = EC_Normal;
  }
  return status;
}


OFCondition DcmPathProcessor::parseItemNoFromPath(OFString& path,        // inout
                                           Uint32& itemNo,        // out
                                           OFBool& wasWildcard)   // out
{
  wasWildcard = OFFalse;
  itemNo = 0;
  // check whether there is an item to parse
  size_t closePos = path.find_first_of(']', 0);
  if ( (closePos != OFString_npos) && (path[0] == '[') )
  {
      long int parsedNo;
      // try parsing item number; parsing for %lu would cause overflows in case of negative numbers
      int parsed = sscanf(path.c_str(), "[%ld]", &parsedNo);
      if (parsed == 1)
      {
          if (parsedNo < 0)
          {
              OFString errMsg = "Negative item number (not permitted) at beginning of path: "; errMsg += path;
              return makeOFCondition(OFM_dcmdata, 25, OF_error, errMsg.c_str());
          }
          itemNo = OFstatic_cast(Uint32, parsedNo);
          if (closePos + 1 < path.length()) // if end of path not reached, cut off "."
              closePos ++;
          path.erase(0, closePos + 1); // remove item from path
          return EC_Normal;
      }
      char aChar;
      parsed = sscanf(path.c_str(), "[%c]", &aChar);
      if ( (parsed == 1) && (aChar =='*') )
      {
        wasWildcard = OFTrue;
        if (closePos + 1 < path.length()) // if end of path not reached, cut off "."
            closePos ++;
        path.erase(0, closePos + 1); // remove item from path
        return EC_Normal;
      }
  }
  OFString errMsg = "Unable to parse item number at beginning of path: "; errMsg += path;
  return makeOFCondition(OFM_dcmdata, 25, OF_error, errMsg.c_str());
}


OFCondition DcmPathProcessor::parseTagFromPath(OFString& path,           // inout
                                        DcmTag& tag)              // out
{
  OFCondition result;
  OFString errMsg = "";
  size_t pos = OFString_npos;

  // In case we have a tag "(gggg,xxxx)"
  if ( path[0] == '(')
  {
    pos = path.find_first_of(')', 0);
    if (pos != OFString_npos)
        result = DcmTag::findTagFromName(path.substr(1, pos - 1).c_str() /* "gggg,eeee" */, tag);
    else
    {
        OFString errMsg("Unable to parse tag at beginning of path: "); errMsg += path;
        return makeOFCondition(OFM_dcmdata, 25, OF_error, errMsg.c_str());
    }
    pos++; // also cut off closing bracket
  }
  // otherwise we could have a dictionary name
  else
  {
    // maybe an item follows
    pos = path.find_first_of('[', 0);
    if (pos == OFString_npos)
        result = DcmTag::findTagFromName(path.c_str(), tag); // check full path
    else
        result = DcmTag::findTagFromName(path.substr(0, pos).c_str(), tag); // parse path up to "[" char
  }
  // construct error message if necessary and return
  if (result.bad())
  {
    OFString errMsg("Unable to parse tag/dictionary name at beginning of path: "); errMsg += path;
    return makeOFCondition(OFM_dcmdata, 25, OF_error, errMsg.c_str());
  }
  // else remove parsed tag from path and return success
  else
    path.erase(0, pos);
  return EC_Normal;
}


Uint32 DcmPathProcessor::getResults(OFList<DcmPath*>& searchResults)
{
  if (m_results.size() > 0)
  {
    // explicitely copy (shallow)
    OFListIterator(DcmPath*) it = m_results.begin();
    while (it != m_results.end())
    {
      searchResults.push_back(*it);
      it++;
    }
  }
  return m_results.size();
}


void DcmPathProcessor::clear()
{
  while (m_results.size() != 0)
  {
    DcmPath* result = m_results.front();
    if (result != NULL)
    {
      delete result;
      result = NULL;
    }
    m_results.pop_front();
  }

  while (m_currentPath.size() != 0)
  {
    DcmPathNode* node = m_currentPath.front();
    if (node != NULL)
    {
      delete node;
      node = NULL;
    }
    m_currentPath.pop_front();
  }
  
  m_createIfNecessary = OFFalse;

}


DcmPathProcessor::~DcmPathProcessor()
{
  clear();
}

/*
** CVS/RCS Log:
** $Log: dcpath.cc,v $
** Revision 1.2  2008-12-12 12:07:11  onken
** Fixed memory leak in path searching function.
**
** Revision 1.1  2008-12-12 11:44:41  onken
** Moved path access functions to separate classes
**
*/
