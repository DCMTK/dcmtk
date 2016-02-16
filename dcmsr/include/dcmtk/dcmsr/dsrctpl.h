/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRTemplateCommon
 *
 */


#ifndef DSRCTPL_H
#define DSRCTPL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrdncsr.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRDocumentSubTree;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing common information on an SR template.
 *  This class is used as a base class only.
 */
class DCMTK_DCMSR_EXPORT DSRTemplateCommon
{

  public:

    /** clear non-static member variables
     */
    virtual void clear();

    /** check whether template identification is set
     ** @return OFTrue if set, OFFalse otherwise
     */
    virtual OFBool hasTemplateIdentification() const;

    /** check whether template identification is valid
     ** @param  check  check internally stored values for conformance with VR and VM
     *                 if enabled
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isTemplateIdentificationValid(const OFBool check = OFTrue) const;

    /** check whether template is extensible
     ** @return OFTrue if extensible, OFFalse otherwise
     */
    virtual OFBool isExtensible() const
    {
        return ExtensibleMode;
    }

    /** compare template identification with given values
     ** @param  templateIdentifier  template identifier to compare with
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional).
     *                              Not used for comparison if the value is empty.
     ** @result OFTrue if template identification is identical, OFFalse otherwise
     */
    virtual OFBool compareTemplateIdentication(const OFString &templateIdentifier,
                                               const OFString &mappingResource,
                                               const OFString &mappingResourceUID = "") const;

    /** get template identifier
     ** @return identifier of the template
     */
    inline const OFString &getTemplateIdentifier() const
    {
        return TemplateIdentifier;
    }

    /** get mapping resource
     ** @return mapping resource that defines the template
     */
    inline const OFString &getMappingResource() const
    {
        return MappingResource;
    }

    /** get optional mapping resource UID
     ** @return unique identifier of the mapping resource (might be empty)
     */
    inline const OFString &getMappingResourceUID() const
    {
        return MappingResourceUID;
    }

    /** change mode specifying whether the template is extensible or non-extensible
     ** @param  mode  set template type to extensible if OFTrue (default)
     */
    virtual void setExtensible(const OFBool mode = OFTrue)
    {
        ExtensibleMode = mode;
    }


  protected:

    /** constructor
     ** @param  templateIdentifier  identifier of the template
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional)
     */
    DSRTemplateCommon(const OFString &templateIdentifier,
                      const OFString &mappingResource,
                      const OFString &mappingResourceUID = "");

    /** destructor
     */
    virtual ~DSRTemplateCommon();

    /** clear all entries in the list of node IDs, i.e.\ set them to their initial value,
     *  which is 0
     */
    void clearEntriesInNodeList();

    /** reserve a certain number of entries in the list of node IDs.  Using this method
     *  can help to avoid unwanted memory allocations and copying of list entries.
     ** @param  count       number of entries to be reserved (for later use)
     *  @param  initialize  initialize all entries with a value of 0 if OFTrue, just
     *                      reserve but do not create them otherwise (default)
     */
    void reserveEntriesInNodeList(const size_t count,
                                  const OFBool initialize = OFFalse);

    /** store given entry at a certain position in the list of node IDs
     ** @param  pos     index of the list entry to use for storage (starting from 0)
     *  @param  nodeID  ID of the node that should be stored at the given position
     */
    void storeEntryInNodeList(const size_t pos,
                              const size_t nodeID);

    /** get a particular entry from the list of node IDs
     ** @param  pos  index of the list entry to retrieve (starting from 0)
     ** @return node ID that is stored at the given position, 0 if not found or set
     */
    size_t getEntryFromNodeList(const size_t pos) const;

    /** set given cursor to a specific node
     ** @param  cursor  reference to document tree node cursor that is used to start
     *                  searching the node from and that is set afterwards
     *  @param  pos     index of the list entry that stores the ID of the node
     ** @return ID of the new current node within the tree if successful, 0 otherwise
     */
    size_t gotoEntryFromNodeList(DSRDocumentTreeNodeCursor &cursor,
                                 const size_t pos) const;

    /** set internal cursor of a given document tree to a specific node
     ** @param  tree  pointer to document tree where nodes and cursor are stored
     *  @param  pos   index of the list entry that stores the ID of the node
     ** @return ID of the new current node within the tree if successful, 0 otherwise
     */
    size_t gotoEntryFromNodeList(DSRDocumentSubTree *tree,
                                 const size_t pos);

    /** set internal cursor of a given document tree to a particular node.
     *  The node is determined from the list of node IDs, starting from the entry
     *  specified by the parameter 'lastPos'.  The reverse search stops if either an
     *  entry with a non-zero value (valid node ID) is found or the first list entry
     *  is reached.  This approach in particular supports handling of template tables
     *  where the order of content items is significant.
     ** @param  tree      pointer to document tree where nodes and cursor are stored
     *  @param  lastPos   index of the last node list entry to start searching from
     *  @param  firstPos  optional index of the node list entry to stop searching
     ** @return ID of the new current node within the tree if successful, 0 otherwise
     */
    size_t gotoLastEntryFromNodeList(DSRDocumentSubTree *tree,
                                     const size_t lastPos,
                                     const size_t firstPos = 0);


  private:

    /// template identifier (VR=CS, mandatory)
    const OFString TemplateIdentifier;
    /// mapping resource (VR=CS, mandatory)
    const OFString MappingResource;
    /// mapping resource UID (VR=UI, optional)
    const OFString MappingResourceUID;
    /// mode indicating whether template is extensible (default: false)
    OFBool ExtensibleMode;

    /// list of node IDs used to remember certain positions in the template
    OFVector<size_t> NodeList;

 // --- declaration of default constructor and assignment operator
 // --- (the assignment operator is defined implicitly)

    DSRTemplateCommon();
    DSRTemplateCommon &operator=(const DSRTemplateCommon &);
};


#endif
