/*
 *
 *  Copyright (C) 2003-2016, OFFIS e.V.
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
 *    classes: DSRCodingSchemeIdentificationList
 *
 */


#ifndef DSRCSIDL_H
#define DSRCSIDL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"

#include "dcmtk/ofstd/oflist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class that maps Coding Scheme Designators to an external coding
 *  system registration, or to a private or local coding scheme.
 *  Implementation of Coding Scheme Identification Sequence.
 */
class DCMTK_DCMSR_EXPORT DSRCodingSchemeIdentificationList
  : public DSRTypes
{

  public:

    /** constructor (default)
     */
    DSRCodingSchemeIdentificationList();

    /** destructor
     */
    ~DSRCodingSchemeIdentificationList();

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

    /** read list of items from the coding scheme identification sequence
     ** @param  dataset  DICOM dataset from which the data should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const size_t flags);

    /** write list of items to the coding scheme identification sequence.
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

    /** set specific character set, which is used for checking the affected element values.
     *  Please note that this method does not return an error if the given 'value' is not
     *  defined by the DICOM standard or not supported by this implementation.
     ** @param  value  value to be set (single or multiple values) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1-n) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSpecificCharacterSet(const OFString &value,
                                        const OFBool check = OFTrue);

    /** add private OFFIS DCMTK coding scheme entry to the list.
     *  Please note that any information previously stored under the defined coding scheme
     *  designator "99_OFFIS_DCMTK" is replaced!
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addPrivateDcmtkCodingScheme();

    /** add the specified coding scheme to the list.
     *  Internally, the item is inserted into the list of coding scheme designators if
     *  not already contained in the list.  In any case, the specified item is selected as
     *  the current one.  Additional information can be set using the below setXXX() methods.
     ** @param  codingSchemeDesignator  coding scheme designator of the item to be added.
     *                                  Designators beginning with "99" and the designator
     *                                  "L" are defined to be private or local coding schemes.
     *  @param  check                   check 'codingSchemeDesignator' for conformance with
     *                                  VR (SH) and VM (1) if enabled.  An empty value is
     *                                  never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &codingSchemeDesignator,
                        const OFBool check = OFTrue);

    /** add the specified coding scheme (with additional information) to the list.
     *  Internally, the item is inserted into the list of coding scheme designators if not
     *  already contained in the list, and the additional information is set.  In any case,
     *  the specified item is selected as the current one.  Further information can be set
     *  using the below setXXX() methods.
     *  Please note that any information previously stored under the given coding scheme
     *  designator is replaced!
     ** @param  codingSchemeDesignator   coding scheme designator of the item to be added.
     *                                   (VR=SH, mandatory).  Designators beginning with "99"
     *                                   and the designator "L" are defined to be private or
     *                                   local coding schemes.
     *  @param  codingSchemeUID          coding scheme UID of the item to be added.  (VR=UI,
     *                                   conditional)
     *  @param  codingSchemeName         coding scheme name of the item to be added.  (VR=ST,
     *                                   optional)
     *  @param  responsibleOrganization  coding scheme responsible organization of the item to
     *                                   be added.  (VR=ST, optional)
     *  @param  check                    check given values for conformance with associated VR
     *                                   and VM if enabled.  An empty 'codingSchemeDesignator'
     *                                   is never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &codingSchemeDesignator,
                        const OFString &codingSchemeUID,
                        const OFString &codingSchemeName,
                        const OFString &responsibleOrganization = "",
                        const OFBool check = OFTrue);

    /** remove the current item from the list.
     *  After successful removal the cursor is set to the next valid position.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem();

    /** remove the specified item from the list.
     *  After successful removal the cursor is set to the next valid position.
     ** @param  codingSchemeDesignator  coding scheme designator of the item to be removed
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const OFString &codingSchemeDesignator);

    /** select the specified item as the current one
     ** @param  codingSchemeDesignator  coding scheme designator of the item to be selected
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const OFString &codingSchemeDesignator);

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

    /** get the coding scheme designator of the currently selected item.
     *  Designators beginning with "99" and the designator "L" are defined to be private
     *  or local coding schemes.
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty if no item is selected)
     */
    const OFString &getCodingSchemeDesignator(OFString &stringValue) const;

    /** get the coding scheme registry of the currently selected item
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getCodingSchemeRegistry(OFString &stringValue) const;

    /** get the coding scheme UID of the currently selected item
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getCodingSchemeUID(OFString &stringValue) const;

    /** get the coding scheme external ID of the currently selected item
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getCodingSchemeExternalID(OFString &stringValue) const;

    /** get the coding scheme name of the currently selected item
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getCodingSchemeName(OFString &stringValue) const;

    /** get the coding scheme version of the currently selected item
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getCodingSchemeVersion(OFString &stringValue) const;

    /** get the coding scheme responsible organization of the currently selected item
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getCodingSchemeResponsibleOrganization(OFString &stringValue) const;

    /** set the coding scheme registry of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeRegistry(const OFString &value,
                                        const OFBool check = OFTrue);

    /** set the coding scheme UID of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeUID(const OFString &value,
                                   const OFBool check = OFTrue);

    /** set the coding scheme external ID of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeExternalID(const OFString &value,
                                          const OFBool check = OFTrue);

    /** set the coding scheme name of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeName(const OFString &value,
                                    const OFBool check = OFTrue);

    /** set the coding scheme version of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeVersion(const OFString &value,
                                       const OFBool check = OFTrue);

    /** set the coding scheme responsible organization of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeResponsibleOrganization(const OFString &value,
                                                       const OFBool check = OFTrue);


  protected:

    /** Internal structure defining the list items
     */
    struct DCMTK_DCMSR_EXPORT ItemStruct
    {
        /** constructor
         ** @param  codingSchemeDesignator  Coding Scheme Designator
         */
        ItemStruct(const OFString &codingSchemeDesignator)
          : CodingSchemeDesignator(codingSchemeDesignator),
            CodingSchemeRegistry(),
            CodingSchemeUID(),
            CodingSchemeExternalID(),
            CodingSchemeName(),
            CodingSchemeVersion(),
            CodingSchemeResponsibleOrganization()
        {}

        /** clear additional information
         */
        void clear()
        {
            CodingSchemeRegistry.clear();
            CodingSchemeUID.clear();
            CodingSchemeExternalID.clear();
            CodingSchemeName.clear();
            CodingSchemeVersion.clear();
            CodingSchemeResponsibleOrganization.clear();
        }

        /// Coding Scheme Designator (VR=SH, type 1)
        const OFString CodingSchemeDesignator;
        /// Coding Scheme Registry (VR=LO, type 1C)
        OFString CodingSchemeRegistry;
        /// Coding Scheme UID (VR=UI, type 1C)
        OFString CodingSchemeUID;
        /// Coding Scheme External ID (VR=ST, type 2C)
        OFString CodingSchemeExternalID;
        /// Coding Scheme Name (VR=ST, type 3)
        OFString CodingSchemeName;
        /// Coding Scheme Version (VR=SH, type 3)
        OFString CodingSchemeVersion;
        /// Coding Scheme Responsible Organization (VR=ST, type 3)
        OFString CodingSchemeResponsibleOrganization;
    };

    /** add the specified coding scheme to the list (if not existent)
     ** @param  codingSchemeDesignator  coding scheme designator of the item to be added
     *  @param  item                    reference to item pointer (result variable)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &codingSchemeDesignator,
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
    /// specific character set used for checking purposes
    OFString SpecificCharacterSet;

    // copy constructor - not implemented!
    DSRCodingSchemeIdentificationList(const DSRCodingSchemeIdentificationList &);
    // assignment operator - not implemented!
    DSRCodingSchemeIdentificationList &operator=(const DSRCodingSchemeIdentificationList &);
};


#endif
