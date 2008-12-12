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
 *  Purpose: Class declarations for accessing DICOM dataset structures (items,
 *           sequences and leaf elements via string-based path access.
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2008-12-12 11:44:40 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCPATH_H
#define DCPATH_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"


/** Class representing a node in DcmPath. A node contains just
  * a pointer to a DcmObject (e. g. a sequence or an item). Additionally
  * an item number is stored which also makes sense in case that the
  * object pointed to is an item. The item number is necessary because
  * when having only a pointer to a DcmItem there is no way to find out
  * at which position the item actually has in the surrounding sequence.
  */
class DcmPathNode
{

public:

  /** Constructor. Creates empty path node.
   */
  DcmPathNode() : m_obj(NULL), m_itemNo(0) {}

  /** Constructor. Creates search node from object pointer and item number.
   *  @param obj [in] The object the search node points to
   *  @param itemNo [in] The item number that should be set. Only relevant
   *                if obj parameter contains an item
   */
  DcmPathNode(DcmObject* obj, Uint32 itemNo)    : m_obj(obj), m_itemNo(itemNo) {}

  /// Pointer to object this search node points to
  DcmObject* m_obj;

  /// The item number of the item in m_obj; only useful if m_obj is an item
  Uint32 m_itemNo;

private:

  /** Private undefined copy constructor
   */
  DcmPathNode(const DcmPathNode& rhs);

  /** Private undefined assignment operator
   */
  DcmPathNode& operator=(const DcmPathNode& arg);

};


/** Class representing a path of DICOM objects. A path is a "way" through
 *  a DICOM dataset, ie. it starts e. g. with an item, follwed by a sequence
 *  originally contained in this item. The sequence then could be followed by
 *  another item which is part of the sequence and so on. Thus the path
 *  should be wellformed, but if it is created manually (e. g. using append())
 *  there is no corresponding checking.
 */

class DcmPath
{

public:

  /** Constructor, creates an empty search result
   */
  DcmPath();

  /** Constructor, creates a search result from a list of search nodes.
   *  @param currentPath [in] The list of search nodes representing a
   *                     "path" through a DICOM dataset
   */
  DcmPath(const OFList<DcmPathNode*>& currentPath);

  /** Appends a search node at the end of the search result path. It is not
   *  checked whether the resulting path is still valid.
   *  @param node [in] The node to append.
   *
   */
  void append(DcmPathNode* node);

  /** Returns iterator pointing to first path component.
   *  @return Iterator to first path node.
   */
  OFListIterator(DcmPathNode*) begin();

  /** Returns last path component.
   *  @return The last path component, NULL if none exists.
   */
  DcmPathNode* back();

  /** Returns iterator pointing to last path component.
   *  @return Iterator to last path node.
   */
  OFListIterator(DcmPathNode*) end();

  /** Returns number of path components.
   *  @return The number of path components
   */
  Uint32 size() const;

  /** Returns whether path is empty, ie does not contain any path nodes
   *  @return OFTrue, if path is empty, OFFalse otherwise
   */
  OFBool empty() const;

  /** Returns a string representation of the path,
   *  e. g. "SourceImageSequence[0].ReferencedSOPInstanceUID"
   *  @return String representing path
   */
  OFString toString() const;

  /** Desctructor, cleans up memory of path nodes. Does not delete
   *  the DcmObjects the nodes point to!
   */
  ~DcmPath();

private:

  /// Internal list representing the nodes in the paths.
  OFList<DcmPathNode*> m_result;

  /** Private undefined copy constructor
   */
  DcmPath(const DcmPath& rhs);

  /** Private undefined assignment operator
   */
  DcmPath& operator=(const DcmPath& arg);
};


class DcmPathProcessor
{

public:

  /** Constructor, creates an empty search object.
   */
  DcmPathProcessor();

  /** Function that allows for finding and/or inserting a hierarchy of items
   *  and attributes as defined by a path string; also returns a list of
   *  pointers for each sucessfully found or inserted paths. Every list
   *  contains pointers pointing to all the objects along the path
   *  starting from the object given as parameter. The pointer to the
   *  starting object will not be part of the result.
   *
   *  In principle, the path string must have the following format (in
   *  arbitrary depth). Note that for searching a sequence, the example
   *  below would start with [itemNo] instead:
   *  SEQUENCE[ITEMNO].SEQUENCE[ITEMNO].ATTRIBUTE
   *  . ITEMNO must be a positive integer starting with 0.
   *  SEQUENCE and ATTRIBUTE must be a tag, written e. g.
   *  "(0010,0010)" or as a dictionary name, e. g. "PatientsName". If the
   *  path cannot be fully created (see option createIfNecessary), any
   *  possibly object changes are reverted. So a path is either fully created
   *  or no path component is created at all. The result can be obtained
   *  by calling the getResults() function.
   *
   *  Example: The path
   *  "ContentSequence[4].(0040,a043)[0].CodeValue" selects the Content
   *  Sequence in "this" item, therein the 5th item, therein the "Concept
   *  Name Code Sequence" denoted by (0040,a043), therein the first item
   *  and finally therein the tag "Code Value".
   *  The resulting object list should (if success is returned) contain
   *  1 result, consisting of a list with 5 pointers to 5 objects in the order
   *  in their logical order as they occur in the path string
   *  (in total 2 sequences, 2 items, and one leaf attribute).
   *  @param obj [in] The object to search (or create) a path in
   *  @param path [in] The path starting with an attribute (either a
   *              sequence or a a leaf attribute) as a dicitionary name or
   *              tag. The parsed attribute is removed from the path string.
   *  @param createIfNecessary [in] If set, all missing objects found
   *                           in the path string are created. If not set,
   *                           only existing paths can be accessed and
   *                           no new attribute or item is created.
   *  @return EC_Normal if successful, error code otherwise.
   */
  OFCondition findOrCreatePath(DcmObject* obj,
                               const OFString& path,
                               OFBool createIfNecessary = OFFalse);

  /** Returns the results from the search / creation call.
   *  @param searchResults [out] The resulting paths that were created/searched
   *                       Note that the memory of the search results
   *                       is freed automatically by the destructor and must
   *                       not be freed by the caller.
   *  @return Number of results returned
   */
  Uint32 getResults(OFList<DcmPath*>& searchResults);

  /** Deconstructor, cleans up memory that was allocated for any
   *  search results.
   */
  ~DcmPathProcessor();

protected:

  /** Function that allows for finding and/or inserting a hierarchy of items
   *  and attributes as defined by a path string; also returns a list of
   *  pointers for each sucessfully found or inserted paths. Every list
   *  contains pointers pointing to all the objects along the path
   *  starting from the object given as parameter. The pointer to the
   *  starting object will not be part of the result.
   *
   *  In principle, the path string must have the following format (in
   *  arbitrary depth).
   *  SEQUENCE[ITEMNO].SEQUENCE[ITEMNO].ATTRIBUTE
   *  . ITEMNO must be a positive integer starting with 0.
   *  SEQUENCE and ATTRIBUTE must be a tag, written e. g.
   *  "(0010,0010)" or as a dictionary name, e. g. "PatientsName". If the
   *  path cannot be fully created (see option createIfNecessary), any
   *  possibly object changes are reverted. So a path is either fully created
   *  or no path component is created at all. The result can be obtained
   *  by calling the getResults() function.
   *
   *  Example: The path
   *  "ContentSequence[4].(0040,a043)[0].CodeValue" selects the Content
   *  Sequence in the given item, therein the 5th item, therein the "Concept
   *  Name Code Sequence" denoted by (0040,a043), therein the first item
   *  and finally therein the tag "Code Value".
   *  The resulting object list should (if success is returned) contain
   *  1 result, consisting of a list with 5 pointers to 5 objects in the order
   *  in their logical order as they occur in the path string
   *  (in total 2 sequences, 2 items, and one leaf attribute).
   *  @param item [in] The object to search (or create) a path in
   *  @param path [in] The path starting with an attribute (either a
   *              sequence or a a leaf attribute) as a dicitionary name or
   *              tag. The parsed attribute is removed from the path string.
   *  @param createIfNecessary [in] If set, all missing objects found
   *                           in the path string are created. If not set,
   *                           only existing paths can be accessed and
   *                           no new attribute or item is created.
   *  @return EC_Normal if successful, error code otherwise.
   */
  OFCondition findOrCreateItemPath(DcmItem* item,
                                   OFString& path);

  /** Function that allows for finding and/or inserting a hierarchy of items
   *  and attributes as defined by a path string; also returns a list of
   *  pointers for each sucessfully found or inserted paths. Every list
   *  contains pointers pointing to all the objects along the path
   *  starting from the object given as parameter. The pointer to the
   *  starting object will not be part of the result.
   *
   *  In principle, the path string must have the following format (in
   *  arbitrary depth).
   *  [ITEMNO].SEQUENCE[ITEMNO].ATTRIBUTE
   *  . ITEMNO must be a positive integer starting with 0.
   *  SEQUENCE and ATTRIBUTE must be a tag, written e. g.
   *  "(0010,0010)" or as a dictionary name, e. g. "PatientsName". If the
   *  path cannot be fully created (see option createIfNecessary), any
   *  possibly object changes are reverted. So a path is either fully created
   *  or no path component is created at all. The result can be obtained
   *  by calling the getResults() function.
   *
   *  Example: The path
   *  "[4].(0040,a043)[0].CodeValue" selects the 5th item in the given
   *  sequence, therein the "Concept Name Code Sequence" denoted by
   *  (0040,a043), therein the first item and finally therein the
   *  tag "Code Value".
   *  The resulting object list should (if success is returned) contain
   *  1 result, consisting of a list with 4 pointers to 4 objects in the order
   *  in their logical order as they occur in the path string
   *  (in total 1 sequence, 2 items, and one leaf element).
   *  @param item [in] The object to search (or create) a path in
   *  @param path [in] The path starting with an attribute (either a
   *              sequence or a a leaf attribute) as a dicitionary name or
   *              tag. The parsed attribute is removed from the path string.
   *  @param createIfNecessary [in] If set, all missing objects found
   *                           in the path string are created. If not set,
   *                           only existing paths can be accessed and
   *                           no new attribute or item is created.
   *  @return EC_Normal if successful, error code otherwise.
   */
  OFCondition findOrCreateSequencePath(DcmSequenceOfItems* seq,
                                       OFString& path);

  /** helper function for findOrCreatePath(). parses an item number from
   *  the beginning of the path string. The item number must be positive,
   *  starting with 0.
   *  The path must start like "[itemnumber]...".
   *  @param path - [in/out] The path starting with the item number
   *                in square brackets, e. g. "[3]". The parsed item number
   *                and a potentially following "." are removed from the path
   *  @param itemNo - [out] The parsed item number. If a wildcard was parsed,
   *                  this output parameter is not set at all.
   *  @param wasWildcard - [out] Is set to OFTrue, if wildcard was parsed
   *                       (instead of concrete item number).
   *  @return EC_Normal, if concrete item number or wildcard was parsed
   */
  static OFCondition parseItemNoFromPath(OFString& path,                   // inout
                                         Uint32& itemNo,                   // out
                                         OFBool& wasWildcard);             // out

  /** Function that parses a tag from the beginning of a path string.
   *  The tag has to be either in numeric format, e. g. "(0010,0010)" or
   *  a dictionary name, e. g. "PatientsName". If successful, the
   *  parsed tag is removed from the path string.
   *  @param path - [in/out] The path string, starting with the attribute
   *                to parse
   *  @param tag - [out] The tag parsed
   *  @return EC_Normal if successful, error code otherwise
   */
  static OFCondition parseTagFromPath(OFString& path,         // inout
                                       DcmTag& tag);           // out

  /** Cleans up memory that was allocated for any search results.
   *  Called when a new search is started or during object destruction.
   */
  void clear();

private:

  /// Internal list that is during search for keeping track of current path
  OFList<DcmPathNode*> m_currentPath;

  /// Internal list that represents the search results found
  OFList<DcmPath*> m_results;

  /// Denotes whether missing items/sequences/attributes should be
  /// automatically inserted when calling search routine
  OFBool m_createIfNecessary;

  /** Private undefined copy constructor
   */
  DcmPathProcessor(const DcmPathProcessor& rhs);

  /** Private undefined assignment operator
   */
  DcmPathProcessor& operator=(const DcmPathProcessor& arg);

};


#endif // DCPATH_H

/*
** CVS/RCS Log:
** $Log: dcpath.h,v $
** Revision 1.1  2008-12-12 11:44:40  onken
** Moved path access functions to separate classes
**
*/
