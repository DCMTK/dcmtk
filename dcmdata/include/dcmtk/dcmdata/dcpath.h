/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V.
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
 *  Purpose: Class declarations for accessing DICOM dataset structures (items,
 *           sequences and leaf elements via string-based path access.
 *
 */

#ifndef DCPATH_H
#define DCPATH_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcdatset.h"


/** Class representing a node in DcmPath. A node contains just
  * a pointer to a DcmObject (e.g. a sequence or an item). Additionally
  * an item number is stored which also makes sense in case that the
  * object pointed to is an item. The item number is necessary because
  * when having only a pointer to a DcmItem there is no way to find out
  * at which position the item actually has in the surrounding sequence.
  */
class DCMTK_DCMDATA_EXPORT DcmPathNode
{

public:

  /** Constructor. Creates empty path node.
   */
  DcmPathNode() : m_obj(NULL), m_itemNo(0) {}

  /** Constructor. Creates search node from object pointer and item number.
   *  @param obj [in] The object the search node points to. The memory of the
   *             given DICOM object is not handled by the node itself but
   *             must be handled (i.e. freed) from outside.
   *  @param itemNo [in] The item number that should be set. Only relevant
   *                if obj parameter contains an item
   */
  DcmPathNode(DcmObject* obj, Uint32 itemNo)    : m_obj(obj), m_itemNo(itemNo) {}

  /** Destructor. Nothing to do, the DICOM object is not freed automatically!
   */
  ~DcmPathNode() { }

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
 *  a DICOM dataset, i.e. it starts e.g. with an item, followed by a sequence
 *  originally contained in this item. The sequence then could be followed by
 *  another item which is part of the sequence and so on. Thus the path
 *  should be well-formed, but if it is created manually (e.g. using append())
 *  there is no corresponding checking.
 */
class DCMTK_DCMDATA_EXPORT DcmPath
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

  /** Removes last path node from path. Also frees memory of path node
   *  but does _not_ free memory of the underlying DICOM item or element.
   *  @return none
   */
  void deleteBackNode();

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
   *  e.g.\ "SourceImageSequence[0].ReferencedSOPInstanceUID"
   *  @return String representing path
   */
  OFString toString() const;

  /** Returns whether the path contains tags of a given group.
   *  Might be useful for looking after (unwanted) meta header tags etc.
   *  @param groupNo [in] The group number to look for
   *  @return OFTrue if group number is found in path, OFFalse otherwise
   */
  OFBool containsGroup(const Uint16 groupNo) const;

  /** Returns a string representation of each path node separately.
   *  Tags are represented as numbers surrounded by braces "(gggg,eeee)",
   *  not dictionary names. Items are represented by a number or wildcard
   *  in square brackets, eg. "[12]" or "[*]".
   *  @param path The path to parse into different nodes
   *  @param result [out] List containing the resulting strings
   *  @return none
   */
  static OFCondition separatePathNodes(const OFString& path, OFList<OFString>& result);

  /** Helper function for findOrCreatePath(). Parses an item number from
   *  the beginning of the path string. The item number must be positive,
   *  starting with 0.
   *  The path must start like "[itemnumber]...".
   *  @param path [in/out] The path starting with the item number in square
   *              brackets, e.g.\ "[3]". The parsed item number and a
   *              potentially following "." are removed from the path
   *  @param itemNo [out] The parsed item number. If a wildcard was parsed,
   *                this output parameter is not set at all.
   *  @param wasWildcard [out] Is set to OFTrue, if wildcard was parsed
   *                     (instead of concrete item number).
   *  @return EC_Normal, if concrete item number or wildcard was parsed
   */
  static OFCondition parseItemNoFromPath(OFString& path,                   // inout
                                         Uint32& itemNo,                   // out
                                         OFBool& wasWildcard);             // out

  /** Function that parses a tag from the beginning of a path string.
   *  The tag has to be either in numeric format, e.g. "(0010,0010)" or
   *  a dictionary name, e.g. "PatientName". If successful, the
   *  parsed tag is removed from the path string.
   *  @param path [in/out] The path string, starting with the attribute
   *              to parse
   *  @param tag [out] The tag parsed
   *  @return EC_Normal if successful, error code otherwise
   */
  static OFCondition parseTagFromPath(OFString& path,         // inout
                                      DcmTag& tag);           // out

  /** Desctructor, cleans up memory of path nodes. Does not delete
   *  the DcmObjects the nodes point to (this is also not done
   *  by the desctructor of the path nodes, so the caller is responsible
   *  for freeing any related DICOM objects.
   */
  ~DcmPath();

private:

  /// Internal list representing the nodes in the path.
  OFList<DcmPathNode*> m_path;

  /** Private undefined copy constructor
   */
  DcmPath(const DcmPath& rhs);

  /** Private undefined assignment operator
   */
  DcmPath& operator=(const DcmPath& arg);
};


class DCMTK_DCMDATA_EXPORT DcmPathProcessor
{

public:

  /** Constructor, creates an empty search object.
   */
  DcmPathProcessor();

  /** Sets whether searching/creating paths will support wildcard for
   *  items. If set to false, any path operation with item wildcard characters
   *  will return an error.
   *  @param supported [in] If true, wildcard are enabled (default)
   *                   If false, item wildcard support is disabled.
   */
  void setItemWildcardSupport(const OFBool supported);


  /** Enables (class default: enabled) or disables checking of private
   *  reservations when inserting private tags. If enabled and a private
   *  tag is created that has no private reservation, an error is returned.
   *  If disabled, it is possible to insert private tags that do not have
   *  a reservation in the corresponding item/dataset.
   *  @param doChecking [in] OFTrue enables reservation checking,
   *                    OFFalse disables it.
   */
  void checkPrivateReservations(const OFBool doChecking);

  /** Checks in item, whether a private reservation for a given
   *  tag key exists.
   *  @param item   [in] The item to search in
   *  @param tagKey [in/out] The tag to be checked.
   *  @param privateCreator [in] The private creator to check for (if known,
   *                        can be left empty)
   *  @return Return EC_Normal if reservation checking was successful.
   *          Otherwise an error code is returned.
   */
  static OFCondition checkPrivateTagReservation(DcmItem* item,
                                                const DcmTagKey& tagKey,
                                                const OFString& privateCreator = "");

  /** Checks in item, whether a private reservation for a given
   *  tag key. If so, a dictionary lookup is performed and the VR and private
   *  creator of the tag is updated correspondingly.
   *  @param item [in] The item to search in
   *  @param tag [in/out] The tag to be checked. Will be updated with VR and
   *                      private creator.
   *  @param privateCreator [in] The private creator to check for (if known,
   *                        can be left empty)
   *  @return Return EC_Normal if reservation checking and updating the
   *          tag was successful. Otherwise an error code is returned.
   */
  static OFCondition checkPrivateTagReservation(DcmItem* item,
                                                DcmTag& tag,
                                                const OFString& privateCreator = "");

  /** Function that allows for finding and/or inserting a hierarchy of items
   *  and attributes as defined by a path string; also returns a list of
   *  pointers for each successfully found or inserted paths. Every list
   *  contains pointers pointing to all the objects along the path
   *  starting from the object given as parameter. The pointer to the
   *  starting object will not be part of the result.
   *
   *  In principle, the path string must have the following format (in
   *  arbitrary depth). Note that for searching a sequence, the example
   *  below would start with [ITEMNO] instead:
   *  SEQUENCE[ITEMNO].SEQUENCE[ITEMNO].ATTRIBUTE
   *  . ITEMNO must be a positive integer starting with 0.
   *  SEQUENCE and ATTRIBUTE must be a tag, written e.g.
   *  "(0010,0010)" or as a dictionary name, e.g. "PatientName". If the
   *  path cannot be fully created (see option createIfNecessary), any
   *  possibly object changes are reverted. So a path is either fully created
   *  or no path component is created at all. The result can be obtained
   *  by calling the getResults() function.
   *
   *  Example: The path
   *  "ContentSequence[4].(0040,a043)[0].CodeValue" selects the Content
   *  Sequence in the given object, therein the 5th item, therein the "Concept
   *  Name Code Sequence" denoted by (0040,a043), therein the first item
   *  and finally therein the tag "Code Value".
   *  The resulting object list should (if success is returned) contain
   *  1 result, consisting of a list with 5 pointers to 5 objects in the order
   *  in their logical order as they occur in the path string
   *  (in total 2 sequences, 2 items, and one leaf attribute).
   *  @param obj [in] The object to search (or create) a path in
   *  @param path [in] The path either starting with an attribute (either a
   *              sequence or a a leaf attribute as a dicitionary name or
   *              tag) or starting with an item
   *  @param createIfNecessary [in] If set, all missing objects found
   *                           in the path string are created. If not set,
   *                           only existing paths can be accessed and
   *                           no new attribute or item is created.
   *  @return EC_Normal if successful, error code otherwise.
   */
  OFCondition findOrCreatePath(DcmObject* obj,
                               const OFString& path,
                               OFBool createIfNecessary = OFFalse);

  /** Function that allows for deleting elements and items from
   *  a DICOM object tree.
   *  In principle, the path string must have the following format (in
   *  arbitrary depth). Note that for searching in a sequence, the example
   *  below would start with [ITEMNO] instead:
   *  SEQUENCE[ITEMNO].SEQUENCE[ITEMNO].ATTRIBUTE
   *  . ITEMNO must be a positive integer starting with 0.
   *  SEQUENCE and ATTRIBUTE must be a tag, written e.g.
   *  "(0010,0010)" or as a dictionary name, e.g. "PatientName".
   *
   *  Example: The path
   *  "ContentSequence[4].(0040,a043)[0].CodeValue" selects the Content
   *  Sequence in the given object, therein the 5th item, therein the "Concept
   *  Name Code Sequence" denoted by (0040,a043), therein the first item
   *  and finally therein the tag "Code Value". Only "Code Value" will be
   *  deleted by the function.
   *  @param obj [in] The object to delete attribute or item from
   *  @param path [in] The path either starting with an attribute (either a
   *              sequence or a a leaf attribute as a dicitionary name or
   *              tag) or starting with an item
   *  @param numDeleted [out] Number of deleted attributes/items
   *  @return EC_Normal if successful, error code otherwise. If the desired
   *  attribute/item was not found, EC_TagNotFound is returned.
   */
  OFCondition findOrDeletePath(DcmObject* obj,
                               const OFString& path,
                               Uint32& numDeleted);

  /** Returns the results from the search / creation call.
   *  @param searchResults [out] The resulting paths that were created/searched
   *                       Note that the memory of the search results is freed
   *                       automatically by the destructor and must not be freed
   *                       by the caller.
   *  @return Number of results returned
   */
  Uint32 getResults(OFList<DcmPath*>& searchResults);


  /** Helper function that applies a specified "override key" in path syntax
   *  to the given dataset. The name "override" indicates that these keys have
   *  higher precedence than identical keys in a request dataset that might
   *  possibly read from a DICOM query file.
   *  @param dataset [in/out] the dataset (e.g.\ query keys) the override key is
   *                 applied to. Must be non-NULL.
   *  @param overrideKey [in] the override key in path syntax (see class DcmPath).
   *                     Also the path can end with a value assignment, e.g.
   *                     "PatientName=Doe^John". An empty (or missing value) will
   *                     not be ignored but will be written as empty to the
   *                     attribute (if not a sequence or item).
   *  @return EC_Normal if adding was successful, error code otherwise
   */
  OFCondition applyPathWithValue(DcmDataset* dataset,
                                 const OFString& overrideKey);

  /** Deconstructor, cleans up memory that was allocated for any
   *  search results.
   */
  ~DcmPathProcessor();

protected:

  /** Function that allows for finding and/or inserting a hierarchy of items
   *  and attributes as defined by a path string; also returns a list of
   *  pointers for each successfully found or inserted paths. Every list
   *  contains pointers pointing to all the objects along the path
   *  starting from the object given as parameter. The pointer to the
   *  starting object will not be part of the result.
   *
   *  In principle, the path string must have the following format (in
   *  arbitrary depth).
   *  SEQUENCE[ITEMNO].SEQUENCE[ITEMNO].ATTRIBUTE
   *  . ITEMNO must be a positive integer starting with 0.
   *  SEQUENCE and ATTRIBUTE must be a tag, written e.g.
   *  "(0010,0010)" or as a dictionary name, e.g. "PatientName". If the
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
   *  @return EC_Normal if successful, error code otherwise.
   */
  OFCondition findOrCreateItemPath(DcmItem* item,
                                   OFString& path);

  /** Function that allows for finding and/or inserting a hierarchy of items
   *  and attributes as defined by a path string; also returns a list of
   *  pointers for each successfully found or inserted paths. Every list
   *  contains pointers pointing to all the objects along the path
   *  starting from the object given as parameter. The pointer to the
   *  starting object will not be part of the result.
   *
   *  In principle, the path string must have the following format (in
   *  arbitrary depth).
   *  [ITEMNO].SEQUENCE[ITEMNO].ATTRIBUTE
   *  . ITEMNO must be a positive integer starting with 0.
   *  SEQUENCE and ATTRIBUTE must be a tag, written e.g.
   *  "(0010,0010)" or as a dictionary name, e.g. "PatientName". If the
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
   *  @param seq  [in] The object to search (or create) a path in
   *  @param path [in] The path starting with an item. The parsed item number
   *  (e.g. "[0]") is removed from the path string.
   *  @return EC_Normal if successful, error code otherwise.
   */
  OFCondition findOrCreateSequencePath(DcmSequenceOfItems* seq,
                                       OFString& path);

  /** Helper function that looks at the last node in a given path and deletes
   *  the corresponding DICOM object. Does not delete the path node itself:
   *  That is done by the calling function, findOrCreateItemPath().
   *  @param objSearchedIn [in/out] The object the given path starts in.
   *  @param path [in/out] The complete path to the DICOM object to delete
   *  @param toDelete [in/out] The path node to delete. This node must be
   *                  identical to the last node in the path parameter. Also
   *                  the node must represent a DICOM sequence or leaf element,
   *                  not an item. However, because it is isolated already by
   *                  the calling function, it is provided here for convenience.
   */
  static OFCondition deleteLastElemFromPath(DcmObject* objSearchedIn,
                                            DcmPath* path,
                                            DcmPathNode* toDelete);

  /** Helper function that looks at the last node in a given path and deletes
   *  the corresponding DICOM object. Does not delete the path node itself:
   *  That is done by the calling function, findOrCreateItemPath().
   *  @param objSearchedIn [in/out] The object the given path starts in.
   *  @param path [in/out] The complete path to the DICOM object to delete
   *  @param toDelete [in/out] The path node to delete. This node must be
   *                  identical to the last node in the path parameter. Also
   *                  the node must represent a DICOM item, not a sequence
   *                  However, because it is isolated already by the calling
   *                  function, it is provided here for convenience.
   */
  static OFCondition deleteLastItemFromPath(DcmObject* objSearchedIn,
                                            DcmPath* path,
                                            DcmPathNode* toDelete);

  /** Returns the private reservation tag key for a given private tag
   *  @param privateKey [in] The private key to calculate reservation tag for
   *  @return The reservation key. If given key is not private or an error,
   *          return DCM_UndefinedTagKey. If the given key is a reservation
   *          itself, it is directly returned.
   */
  static DcmTagKey calcPrivateReservationTag(const DcmTagKey& privateKey);

  /** Cleans up memory that was allocated for any search results.
   *  Called when a new search is started or during object destruction.
   *  The DICOM data all freed paths and path nodes point to, is not
   *  touched, i.e. all memory to the DICOM objects pointed to must be
   *  freed from outside. Processing options like checking for private
   *  reservations and so on are not reset to default values but
   *  keep valid.
   */
  void clear();

private:

  /// Internal list that is during search for keeping track of current path
  OFList<DcmPathNode*> m_currentPath;

  /// Internal list that represents the search results found
  OFList<DcmPath*> m_results;

  /// Denotes whether missing items/sequences/attributes should be
  /// automatically inserted when using findAndCreate routines
  OFBool m_createIfNecessary;

  /// If enabled (default), any insertions of private tags will fail, if no
  /// corresponding reservation exists in the underlying item
  OFBool m_checkPrivateReservations;

  /// Denotes, whether a path is accepted that contains wildcards.
  /// If false, then any search containing wildcards will return an error.
  OFBool m_itemWildcardsEnabled;

  /** Private undefined copy constructor
   */
  DcmPathProcessor(const DcmPathProcessor& rhs);

  /** Private undefined assignment operator
   */
  DcmPathProcessor& operator=(const DcmPathProcessor& arg);
};


#endif // DCPATH_H
