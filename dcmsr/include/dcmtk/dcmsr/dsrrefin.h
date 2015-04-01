/*
 *
 *  Copyright (C) 2011-2015, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRReferencedInstanceList
 *
 */


#ifndef DSRREFIN_H
#define DSRREFIN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcodvl.h"

#include "dcmtk/ofstd/oflist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class that manages a list of SOP instances significantly related to the current
 *  SR document.  This is an implementation of the Referenced Instance Sequence.
 */
class DCMTK_DCMSR_EXPORT DSRReferencedInstanceList
  : public DSRTypes
{

  public:

    /** constructor (default)
     */
    DSRReferencedInstanceList();

    /** destructor
     */
    ~DSRReferencedInstanceList();

    /** clear the list
     */
    void clear();

    /** check whether list is empty
     ** @return OFTrue if list is empty, OFFalse otherwise
     */
    OFBool empty() const;

    /** get number of items stored in the list
     ** @return number of items
     */
    size_t getNumberOfItems() const;

    /** read list of items from the referenced instance sequence
     ** @param  dataset  DICOM dataset from which the data should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const size_t flags);

    /** write list of items to the referenced instance sequence.
     *  Does nothing if list is empty.
     ** @param  dataset  DICOM dataset to which the data should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset) const;

    /** read list of items from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const DSRXMLDocument &doc,
                        DSRXMLCursor cursor,
                        const size_t flags);

    /** write current list in XML format
     ** @param  stream  output stream to which the XML data is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(STD_NAMESPACE ostream &stream,
                         const size_t flags) const;

    /** add new entry to the list of instances (if not already existent).
     *  Before adding (or searching for) the entry, the given UID values are usually
     *  checked.  If one of the values is invalid, nothing is done but an error is
     *  returned.  If successful, the specified item is selected as the current one.
     ** @param  sopClassUID  SOP class UID of the entry to be added (VR=UI, mandatory)
     *  @param  instanceUID  SOP instance UID of the entry to be added (VR=UI, mandatory)
     *  @param  check        if enabled, check values for validity before adding them.
     *                       See checkSOPInstance() method for details.  An empty value
     *                       is never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &sopClassUID,
                        const OFString &instanceUID,
                        const OFBool check = OFTrue);

    /** remove the current item from the list.
     *  After successful removal the cursor is set to the next valid position.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem();

    /** remove the specified item from the list.
     *  After successful removal the cursor is set to the next valid position.
     ** @param  sopClassUID  SOP class UID of the item to be removed
     *  @param  instanceUID  SOP instance UID of the item to be removed
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const OFString &sopClassUID,
                           const OFString &instanceUID);

    /** select the specified item as the current one
     ** @param  sopClassUID  SOP class UID of the item to be selected
     *  @param  instanceUID  SOP instance UID of the item to be selected
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const OFString &sopClassUID,
                         const OFString &instanceUID);

    /** select the first item in the list
     ** @return status, EC_Normal if successful, an error code otherwise.
     *    (e.g. if the list is empty)
     */
    OFCondition gotoFirstItem();

    /** select the next item in the list
     ** @return status, EC_Normal if successful, an error code otherwise.
     *    (e.g. if the end of the list has been reached)
     */
    OFCondition gotoNextItem();

    /** get the SOP class UID of the currently selected entry
     ** @param  sopClassUID  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPClassUID(OFString &sopClassUID) const;

    /** get the SOP instance UID of the currently selected entry
     ** @param  instanceUID  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPInstanceUID(OFString &instanceUID) const;

    /** get purpose of reference code of the currently selected entry
     ** @param  purposeOfReference  variable where a copy of the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getPurposeOfReference(DSRCodedEntryValue &purposeOfReference) const;

    /** set purpose of reference code of the currently selected entry.
     *  Before setting the value, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.
     ** @param  purposeOfReference  value to be set (mandatory)
     *  @param  check               if enabled, check value for validity before setting it.
     *                              See checkPurposeOfReference() method for details.  An
     *                              empty value is never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPurposeOfReference(const DSRCodedEntryValue &purposeOfReference,
                                      const OFBool check = OFTrue);


  protected:

    /** Internal structure defining the list items
     */
    struct DCMTK_DCMSR_EXPORT ItemStruct
    {
        /** constructor
         ** @param  sopClassUID  SOP class UID
         ** @param  instanceUID  SOP instance UID
         */
        ItemStruct(const OFString &sopClassUID,
                   const OFString &instanceUID)
          : SOPClassUID(sopClassUID),
            InstanceUID(instanceUID),
            PurposeOfReference()
        {}

        /** clear additional information
         */
        void clear()
        {
            // an empty code is invalid
            PurposeOfReference.clear();
        }

        /// SOP Class UID (VR=UI, type 1)
        const OFString SOPClassUID;
        /// SOP Instance UID (VR=UI, type 1)
        const OFString InstanceUID;
        /// Purpose of Reference Code Sequence (VR=SQ, type 1)
        DSRCodedEntryValue PurposeOfReference;
    };

    /** add the specified SOP instance to the list (if not existent)
     ** @param  sopClassUID  SOP class UID of the entry to be added
     *  @param  instanceUID  SOP instance UID of the entry to be added
     *  @param  item         reference to item pointer (result variable)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &sopClassUID,
                        const OFString &instanceUID,
                        ItemStruct *&item);

    /** get pointer to currently selected item structure (if any)
     ** @return pointer to the item structure if successful, NULL otherwise
     */
    ItemStruct *getCurrentItem() const;

    /** check the specified SOP class UID and SOP instance UID for validity.
     *  Currently, the only checks performed are that the strings are non-empty and that
     *  the given values conform to the corresponding VR (UI) and VM (1).
     ** @param  sopClassUID  SOP class UID to be checked
     *  @param  instanceUID  SOP instance UID to be checked
     ** @return status, EC_Normal if all values are valid, an error code otherwise
     */
    OFCondition checkSOPInstance(const OFString &sopClassUID,
                                 const OFString &instanceUID) const;

    /** check the specified purpose of reference code for validity.
     *  Internally, the methods DSRCodedEntryValue::isEmpty() and
     *  DSRCodedEntryValue::checkCurrentValue() are used for this purpose.  Conformance
     *  with the Context Group 7006 (as defined in the DICOM standard) is not yet checked.
     ** @param  purposeOfReference  purpose of reference code to be checked
     ** @return status, EC_Normal if code is valid, an error code otherwise
     */
    OFCondition checkPurposeOfReference(const DSRCodedEntryValue &purposeOfReference) const;


  private:

    /// list of items
    OFList<ItemStruct *> ItemList;
    /// internal cursor to current (selected) list item
    OFListIterator(ItemStruct *) Iterator;

    // copy constructor - not implemented!
    DSRReferencedInstanceList(const DSRReferencedInstanceList &);
    // assignment operator - not implemented!
    DSRReferencedInstanceList &operator=(const DSRReferencedInstanceList &);
};


#endif
