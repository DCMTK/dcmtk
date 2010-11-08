/*
 *
 *  Copyright (C) 2008-2010, OFFIS e.V.
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
 *  Purpose: Class definitions for accessing DICOM dataset structures (items,
 *           sequences and leaf elements via string-based path access.
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-08 09:49:03 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dcsequen.h"


/*******************************************************************/
/*              Implementation of class DcmPath                    */
/*******************************************************************/

// Constructor
DcmPath::DcmPath() : m_path()
{
}


// Construct from existing path (kind of copy constructor)
DcmPath::DcmPath(const OFList<DcmPathNode*>& currentPath)
    : m_path()
{
  OFListConstIterator(DcmPathNode*) it = currentPath.begin();
  OFListConstIterator(DcmPathNode*) endOfPath = currentPath.end();
  while (it != endOfPath)
  {
    m_path.push_back(new DcmPathNode( (*it)->m_obj, (*it)->m_itemNo ));
    it++;
  }
}


// Append a node to the path
void DcmPath::append(DcmPathNode* node)
{
  if (node != NULL)
    m_path.push_back(node); // do any validity checking?
}


// Deletes last node from the path and frees corresponding memory
void DcmPath::deleteBackNode()
{
  DcmPathNode *node = m_path.back();
  m_path.pop_back();
  if (node)
  {
    delete node; node = NULL;
  }
}


// Returns iterator to first element of the path
OFListIterator(DcmPathNode*) DcmPath::begin()
{
  return m_path.begin();
}


// Returns iterator to last element of the path
DcmPathNode* DcmPath::back()
{
  return m_path.back();
}

// Returns iterator to the end of path, ie. dummy after actual last element
OFListIterator(DcmPathNode*) DcmPath::end()
{
  return m_path.end();
}


// Returns number of path nodes in the path
Uint32 DcmPath::size() const
{
  return m_path.size();
}


// Returns true if path is empty, ie. number of path nodes is zero
OFBool DcmPath::empty() const
{
  return (m_path.size() == 0);
}


// Returns string representation of the path
OFString DcmPath::toString() const
{
  OFListConstIterator(DcmPathNode*) it = m_path.begin();
  OFListConstIterator(DcmPathNode*) endOfList = m_path.end();
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
#if SIZEOF_LONG == 8
      sprintf(buf, "[%u]", (*it)->m_itemNo);
#else
      sprintf(buf, "[%lu]", (*it)->m_itemNo);
#endif
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


// Checks whether a specific group number is used in the path's path nodes
OFBool DcmPath::containsGroup(const Uint16& groupNo) const
{
  OFListConstIterator(DcmPathNode*) it = m_path.begin();
  OFListConstIterator(DcmPathNode*) endOfList = m_path.end();
  while (it != endOfList)
  {
    DcmPathNode* node = *it;
    if ( (node == NULL) || (node->m_obj == NULL) ) return OFFalse;
    if (node->m_obj->getGTag() == groupNo) return OFTrue;
    it++;
  }
  return OFFalse;
}


// Helper function for findOrCreatePath(). Parses item no from start of string
OFCondition DcmPath::parseItemNoFromPath(OFString& path,        // inout
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


// Function that parses a tag from the beginning of a path string.
OFCondition DcmPath::parseTagFromPath(OFString& path,           // inout
                                      DcmTag& tag)              // out
{
  OFCondition result;
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


// Destructor, frees memory of path nodes (but not of underlying DICOM objects)
DcmPath::~DcmPath()
{
  // free dynamically allocated memory
  while (m_path.size() != 0)
  {
    DcmPathNode* node = m_path.front();
    delete node; node = NULL;
    m_path.pop_front();
  }
}


// Seperate a string path into the different nodes
OFCondition DcmPath::separatePathNodes(const OFString& path,
                                       OFList<OFString>& result)
{
  OFString pathStr(path);
  OFCondition status = EC_Normal;
  OFBool nextIsItem = OFTrue;
  Uint32 itemNo = 0;
  OFBool isWildcard = OFFalse;

  // initialize parsing loop
  if (!pathStr.empty())
  {
    if (pathStr[0] != '[')
      nextIsItem = OFFalse;
  }

  char buf[100];
  // parse node for node and only stop if error occurs or parsing completes
  while ( !pathStr.empty() )
  {
    if (nextIsItem)
    {
      status = parseItemNoFromPath(pathStr, itemNo, isWildcard);
      if (status.bad()) return status;
      if (isWildcard)
        result.push_back("[*]");
      else
      {
#if SIZEOF_LONG == 8
        if (sprintf(buf, "[%u]", itemNo) < 2) return EC_IllegalParameter;
#else
        if (sprintf(buf, "[%lu]", itemNo) < 2) return EC_IllegalParameter;
#endif
        result.push_back(buf);
      }
      nextIsItem = OFFalse;
    }
    else
    {
      DcmTag tag;
      status = parseTagFromPath(pathStr, tag);
      if (status.bad())
        return status;
      if (sprintf(buf, "(%04X,%04X)", tag.getGroup(), tag.getElement()) != 11)
        return EC_IllegalParameter;
      result.push_back(buf);
      nextIsItem = OFTrue;
    }
  }
  return status;
}


/*******************************************************************/
/*          Implementation of class DcmPathProcessor               */
/*******************************************************************/


// Constructor, constructs an empty path processor
DcmPathProcessor::DcmPathProcessor() :
  m_currentPath(),
  m_results(),
  m_createIfNecessary(OFFalse),
  m_checkPrivateReservations(OFTrue),
  m_itemWildcardsEnabled(OFTrue)
{
}


// enables (class default:enabled) or disables checking of private reservations
void DcmPathProcessor::checkPrivateReservations(const OFBool& doChecking)
{
  m_checkPrivateReservations = doChecking;
}


// enables (class default:enabled) or disables support for item wildcards
void DcmPathProcessor::setItemWildcardSupport(const OFBool& supported)
{
  m_itemWildcardsEnabled = supported;
}


// Permits finding and creating DICOM object hierarchies based on a path string
OFCondition DcmPathProcessor::findOrCreatePath(DcmObject* obj,
                                               const OFString& path,
                                               OFBool createIfNecessary)
{
  // check input parameters
  if ( (obj == NULL) || path.empty())
    return EC_IllegalParameter;

  if (!m_itemWildcardsEnabled)
  {
    if (path.find('*') != OFString_npos)
    {
      return makeOFCondition(OFM_dcmdata, 25, OF_error, "Item wildcard '*' found in path but wildcards disabled");
    }
  }
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


// Permits deleting DICOM object hierarchies based on a path string
OFCondition DcmPathProcessor::findOrDeletePath(DcmObject* obj,
                                               const OFString& path,
                                               Uint32& numDeleted)
{
  // check input parameters
  if ( (obj == NULL) || path.empty())
    return EC_IllegalParameter;
  numDeleted = 0;
  if (!m_itemWildcardsEnabled)
  {
    if (path.find('*') != OFString_npos)
    {
      return makeOFCondition(OFM_dcmdata, 25, OF_error, "Item wildcard '*' found in path but wildcards disabled");
    }
  }

  // search
  m_createIfNecessary = OFFalse;
  OFString pathCopy = path;
  OFCondition result;
  clear();
  if ((obj->ident() == EVR_item) || (obj->ident() == EVR_dataset))
    result = findOrCreateItemPath(OFstatic_cast(DcmItem*, obj), pathCopy);
  else if (obj->ident() == EVR_SQ)
    result = findOrCreateSequencePath(OFstatic_cast(DcmSequenceOfItems*, obj), pathCopy);
  else
    return EC_IllegalParameter;
  if (result.bad()) return result;

  // check results
  OFList<DcmPath*> resultPaths;
  Uint32 numPaths = getResults(resultPaths);
  if (numPaths == 0) return EC_IllegalCall; // should never happen at this point
  OFListIterator(DcmPath*) pathIt = resultPaths.begin();
  OFListIterator(DcmPath*) endIt = resultPaths.end();
  while (pathIt != endIt)
  {
    // get last item/element from path which should be deleted
    DcmPathNode* nodeToDelete = (*pathIt)->back();
    if ( (nodeToDelete == NULL) || (nodeToDelete->m_obj == NULL) ) return EC_IllegalCall;

    // if it's not an item, delete element from item.
    // deletes DICOM content of node but not node itself (done later)
    if (nodeToDelete->m_obj->ident() != EVR_item)
    {
      result = deleteLastElemFromPath(obj, *pathIt, nodeToDelete);
    }
    // otherwise we need to delete an item from a sequence
    else
    {
      result = deleteLastItemFromPath(obj, *pathIt, nodeToDelete);
    }
    if (result.bad()) return result;
    // if success, remove node from path and clear node memory
    (*pathIt)->deleteBackNode();
    numDeleted++;
    pathIt++;
  }
  return result;
}


// Get results of a an operation started before (e. g. findOrCreatePath())
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

// applies a string path (optionally with value) to a dataset
OFCondition DcmPathProcessor::applyPathWithValue(DcmDataset *dataset,
                                                 const OFString& overrideKey)
{
  if (dataset == NULL) return EC_IllegalCall;
  if (overrideKey.empty()) return EC_Normal;
  OFString path = overrideKey;
  OFString value;
  size_t pos = path.find('=');
  // separate tag from value if there is one
  if (pos != OFString_npos)
  {
    value = path.substr(pos+1); // value now contains value
    path.erase(pos);            // pure path without value
  }
  clear();

  // create path
  OFCondition result = findOrCreatePath(dataset, path, OFTrue /* create if necessary */);
  if (result.bad())
    return result;

  // prepare for value insertion
  OFListConstIterator(DcmPath*) it = m_results.begin();
  OFListConstIterator(DcmPath*) endList = m_results.end();
  DcmPathNode *last = (*it)->back();
  if (last == NULL) return EC_IllegalCall;
  // if value is specified, be sure path does not end with item
  if ( !last->m_obj->isLeaf() )
  {
    if (value.empty())
      return EC_Normal;
    else
      return makeOFCondition(OFM_dcmdata, 25, OF_error, "Cannot insert value into path ending with item or sequence");
  }
  // insert value into each element affected by path
  while (it != endList)
  {
    last = (*it)->back();
    if (last == NULL) return EC_IllegalCall;
    DcmElement *elem = OFstatic_cast(DcmElement*, last->m_obj);
    if (elem == NULL) return EC_IllegalCall;
    result = elem->putString(value.c_str());
    if (result.bad())
      break;
    it++;
  }
  return result;
}


// Resets status (including results) of DcmPathProcessor and frees corresponding memory
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

}


// Destructor, frees memory by calling clear()
DcmPathProcessor::~DcmPathProcessor()
{
  clear();
}


/* protected helper functions */

// Helper function that deletes last DICOM element from a path from the DICOM hierarchy
OFCondition DcmPathProcessor::deleteLastElemFromPath(DcmObject* objSearchedIn,
                                                     DcmPath *path,
                                                     DcmPathNode* toDelete)
{
  // item containing the element to delete
  DcmItem *containingItem = NULL;
  if ( path->size() == 1)
  {
    // if we have only a single elem in path, given object must be cont. item
    if ( (objSearchedIn->ident() != EVR_item) && (objSearchedIn->ident() != EVR_dataset) )
      return makeOFCondition(OFM_dcmdata, 25, OF_error, "Cannot search leaf element in object being not an item");
    containingItem = OFstatic_cast(DcmItem*, objSearchedIn);
  }
  else
  {
    // get containing item from path which is the penultimate in the path
    OFListIterator(DcmPathNode*) temp = path->end();
    temp--; temp--;
    if (*temp == NULL) return EC_IllegalCall; // never happens here...
    if ( (*temp)->m_obj == NULL ) return EC_IllegalCall;
    if ( (*temp)->m_obj->ident() != EVR_item) // (no test for dataset needed)
      return makeOFCondition(OFM_dcmdata, 25, OF_error, "Cannot search leaf element in object being not an item");
    containingItem = OFstatic_cast(DcmItem*, (*temp)->m_obj);
  }
  if (containingItem == NULL) return EC_IllegalCall;
  OFCondition result = containingItem->findAndDeleteElement(toDelete->m_obj->getTag(), OFFalse, OFFalse);
  return result;
}


// Helper function that deletes last DICOM item from a path from the DICOM hierarchy
OFCondition DcmPathProcessor::deleteLastItemFromPath(DcmObject* objSearchedIn,
                                                     DcmPath *path,
                                                     DcmPathNode* toDelete)
{
  DcmSequenceOfItems *containingSeq = NULL;
  if ( path->size() == 1)
  {
    // if we have only a single elem in path, given object must be cont. item
    if (objSearchedIn->ident() != EVR_SQ)
      return makeOFCondition(OFM_dcmdata, 25, OF_error, "Cannot search item in object being not a sequence");
    containingSeq = OFstatic_cast(DcmSequenceOfItems*, objSearchedIn);
  }
  else
  {
    // get containing item from path which is the penultimate in the path
    OFListIterator(DcmPathNode*) temp = path->end();
    temp--; temp--;
    if (*temp == NULL) return EC_IllegalCall; // never happens here...
    if ( (*temp)->m_obj == NULL ) return EC_IllegalCall;
    if ( (*temp)->m_obj->ident() != EVR_SQ)
      return makeOFCondition(OFM_dcmdata, 25, OF_error, "Cannot search item in object being not a sequence");
    containingSeq = OFstatic_cast(DcmSequenceOfItems*, (*temp)->m_obj);
  }
  if (containingSeq == NULL ) return EC_IllegalCall;
  DcmItem *item2BDeleted = containingSeq->remove(OFstatic_cast(DcmItem*, toDelete->m_obj));
  if ( item2BDeleted == NULL )
    return EC_IllegalCall; // should not happen here...
  delete item2BDeleted; item2BDeleted = NULL;
  return EC_Normal;
}


// Helper function that does work for findOrCreatePath()
OFCondition DcmPathProcessor::findOrCreateItemPath(DcmItem* item,
                                                   OFString& path)
{
  if (item == NULL)
    return EC_IllegalParameter;

  if (path.empty())
    return EC_IllegalParameter;

  OFString restPath(path);
  OFCondition status = EC_Normal;
  DcmTag tag;
  OFString privCreator;
  OFBool newlyCreated = OFFalse; // denotes whether an element was created
  DcmElement *elem = NULL;
  DcmPath* currentResult = NULL;

  // parse tag
  status = DcmPath::parseTagFromPath(restPath, tag);
  if (status.bad())
    return status;

  // insert element or sequence
  if ( !(item->tagExists(tag)) ) // do not to overwrite existing tags
  {
    if (m_createIfNecessary)
    {
      // private tags needs special handling, e. g. checking reservation
      if (tag.isPrivate() && m_checkPrivateReservations)
      {
        status = checkPrivateTagReservation(item, tag);
        if (status.bad()) return status;
      }
      elem = newDicomElement(tag);
      if (elem == NULL)
        return EC_IllegalCall;
      status = item->insert(elem, OFTrue);
      if (status.bad())
      {
        delete elem; elem = NULL;
        return status;
      }
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
      if (restPath.empty())
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
  else if (restPath.empty()) // we inserted a leaf element: path must be completed
  {
    // add element and add current path to overall results; then return success
    currentResult = new DcmPath(m_currentPath);
    currentResult->append(new DcmPathNode(elem, 0));
    m_results.push_back(currentResult);
    return EC_Normal;
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


// Helper function that does work for findOrCreatePath()
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
  status = DcmPath::parseItemNoFromPath(restPath, itemNo, isWildcard);
  if (status.bad())
    return status;

  // wildcard code: add result path for every matching item
  if (isWildcard)
  {
    // if there are no items -> no results are found
    Uint32 numItems = seq->card();
    if (numItems == 0)
    {
      if (!m_createIfNecessary)
        return EC_TagNotFound;
      else
        return makeOFCondition(OFM_dcmdata, 25, OF_error, "Cannot insert unspecified number (wildcard) of items into sequence");
    }

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
        if (restPath.empty())
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
  if (!restPath.empty())
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


DcmTagKey DcmPathProcessor::calcPrivateReservationTag(const DcmTagKey &privateKey)
{
  DcmTagKey reservationTag(0xFFFF, 0xFFFF);
  // if not a private key is given return "error"
  if (!privateKey.isPrivate())
    return reservationTag;
  // if the private key given is already a reservation key, return it
  if (privateKey.isPrivateReservation())
    return privateKey;

  // Calculate corresponding private creator element
  Uint16 elemNo = privateKey.getElement();
  // Get yz from given element number wxyz, groups stays the same
  elemNo >>= 8;
  reservationTag.setGroup(privateKey.getGroup());
  reservationTag.setElement(elemNo);
  return reservationTag;
}


OFCondition DcmPathProcessor::checkPrivateTagReservation(DcmItem *item /* in */,
                                                         DcmTag& tag /* in/out */)
{
  OFCondition result;
  // if this is already a private reservation, there is nothing to do
  if (m_checkPrivateReservations && !tag.isPrivateReservation())
  {
    DcmTagKey reservationKey = calcPrivateReservationTag(tag);
    if (reservationKey == DCM_UndefinedTagKey)
    {
      OFString msg("Path evaluation error: Unable to compute private reservation for tag: "); msg += tag.toString();
      return makeOFCondition(OFM_dcmdata, 25, OF_error, msg.c_str());
    }
    if (!item->tagExists(reservationKey))
    {
      OFString msg("Invalid path: No private reservation found for tag: "); msg += tag.toString();
      return makeOFCondition(OFM_dcmdata, 25, OF_error, msg.c_str());
    }
    else
    {
      // set private creator for new element
      OFString privCreator;
      result = item->findAndGetOFString(reservationKey, privCreator);
      if (result.bad() || (privCreator.empty()))
      {
        privCreator = "Invalid or empty private creator tag: "; privCreator += reservationKey.toString();
        return makeOFCondition(OFM_dcmdata, 25, OF_error, privCreator.c_str());
      }
      // tell tag that its private creator and VR
      tag.setPrivateCreator(privCreator.c_str());
      tag.lookupVRinDictionary(); // not done automatically when saving
    }
  }
  return EC_Normal;
}


/*
** CVS/RCS Log:
** $Log: dcpath.cc,v $
** Revision 1.15  2010-11-08 09:49:03  uli
** Fixed even more gcc warnings caused by additional compiler flags.
**
** Revision 1.14  2010-11-01 10:42:44  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.13  2010-10-14 13:14:08  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.12  2010-07-01 16:56:17  joergr
** Replaced "OFString::length() == 0" by "OFString::empty()".
** Removed superfluous curly brackets.
**
** Revision 1.11  2009-11-04 09:58:10  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.10  2009-09-29 12:55:25  uli
** Include only the needed headers in dcpath.h and dcpath.cc
**
** Revision 1.9  2009-09-04 13:53:09  meichel
** Minor const iterator related changes needed to compile with VC6 with HAVE_STL
**
** Revision 1.8  2009-08-26 07:47:34  joergr
** Added check on size of long in order to avoid warnings reported by gcc 4.3.2.
**
** Revision 1.7  2009-08-19 11:56:58  meichel
** Fixed parameter that was declared as const in the implementation and
**   as non-const in the class declaration.
**
** Revision 1.6  2009-07-10 13:12:16  onken
** Added override key functionality used by tools like findscu to the more
** central DcmPathProcessor class.
**
** Revision 1.5  2009-07-08 16:09:12  onken
** Cleaned up code for private reservation checking and added option for
** disabling item wildcards for searching/creating tag paths.
**
** Revision 1.4  2009-01-15 16:04:02  onken
** Added options for handling of private tags and fixed bug for deleting
** tags on main level.
**
** Revision 1.3  2009-01-12 12:37:41  onken
** Fixed iterators to also compile with STL classes being enabled.
**
** Revision 1.2  2008-12-12 12:07:11  onken
** Fixed memory leak in path searching function.
**
** Revision 1.1  2008-12-12 11:44:41  onken
** Moved path access functions to separate classes
**
*/
