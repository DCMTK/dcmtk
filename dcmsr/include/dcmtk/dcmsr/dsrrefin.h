/*
 *
 *  Copyright (C) 2011-2012, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-01-06 09:13:10 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
 *  SR document.  An implementation of the Referenced Instance Sequence.
 */
class DCMTK_DCMSR_EXPORT DSRReferencedInstanceList
  : public DSRTypes
{

  public:

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

        /// SOP Class UID (VR=UI, VM=1, Type=1)
        const OFString SOPClassUID;
        /// SOP Instance UID (VR=UI, VM=1, Type=1)
        const OFString InstanceUID;
        /// Purpose of Reference Code Sequence (VR=SQ, VM=1, Type=1)
        DSRCodedEntryValue PurposeOfReference;
    };

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
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset);

    /** write list of items to the referenced instance sequence.
     *  Does nothing if list is empty.
     ** @param  dataset  DICOM dataset to which the data should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset) const;

    /** read list of items from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const DSRXMLDocument &doc,
                        DSRXMLCursor cursor,
                        const size_t flags);

    /** write current list in XML format
     ** @param  stream  output stream to which the XML data is written
     *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(STD_NAMESPACE ostream &stream,
                         const size_t flags = 0) const;

    /** add new entry to the list of instances (if not already existent).
     *  Finally, the specified item is selected as the current one.
     ** @param  sopClassUID  SOP class UID of the entry to be added
     *  @param  instanceUID  SOP instance UID of the entry to be added
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &sopClassUID,
                        const OFString &instanceUID);

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
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPClassUID(OFString &stringValue) const;

    /** get the SOP instance UID of the currently selected entry
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPInstanceUID(OFString &stringValue) const;

    /** get purpose of reference code of the currently selected entry
     ** @param  codeValue  variable where a copy of the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getPurposeOfReference(DSRCodedEntryValue &codeValue) const;

    /** set purpose of reference code of the currently selected entry.
     *  The Context Group with CID 7006 (SR Document Purposes of Reference) contains
     *  a list of appropriate code values.  See DICOM standard for details.
     ** @param  codeValue  value to be set (not checked for validity)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPurposeOfReference(const DSRCodedEntryValue &codeValue);


  protected:

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


/*
 *  CVS/RCS Log:
 *  $Log: dsrrefin.h,v $
 *  Revision 1.2  2012-01-06 09:13:10  uli
 *  Make it possible to build dcmsr as a DLL.
 *
 *  Revision 1.1  2011-12-09 15:00:08  joergr
 *  Added support for the Referenced Instance Sequence (0008,114A) introduced
 *  with CP-670 (Reference rendering of SR), which allows for referencing an
 *  equivalent CDA document or a rendering as an Encapsulated PDF document.
 *
 *
 *
 */
