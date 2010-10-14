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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRCodingSchemeIdentificationList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:32 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DSRCodingSchemeIdentificationList
  : public DSRTypes
{

  public:

    /** Internal structure defining the list items
     */
    struct ItemStruct
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
            ResponsibleOrganization()
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
            ResponsibleOrganization.clear();
        }

        /// Coding Scheme Designator  (VR=SH, VM=1, Type=1)
        const OFString CodingSchemeDesignator;
        /// Coding Scheme Registry    (VR=LO, VM=1, Type=1C)
        OFString CodingSchemeRegistry;
        /// Coding Scheme UID         (VR=UI, VM=1, Type=1C)
        OFString CodingSchemeUID;
        /// Coding Scheme External ID (VR=ST, VM=1, Type=2C)
        OFString CodingSchemeExternalID;
        /// Coding Scheme Name        (VR=ST, VM=1, Type=3)
        OFString CodingSchemeName;
        /// Coding Scheme Version     (VR=SH, VM=1, Type=3)
        OFString CodingSchemeVersion;
        /// Responsible Organization  (VR=ST, VM=1, Type=3)
        OFString ResponsibleOrganization;
    };

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
     ** @param  dataset    DICOM dataset from which the data should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset);

    /** write list of items to the coding scheme identification sequence.
     *  Does nothing if list is empty.
     ** @param  dataset    DICOM dataset to which the data should be written
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

    /** add private OFFIS DCMTK coding scheme entry to the list.
     *  Please note that any information previously stored under the defined coding scheme
     *  designator "99_OFFIS_DCMTK" is replaced!
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addPrivateDcmtkCodingScheme();

    /** add the specified coding scheme to the list.
     *  Internally, the item is inserted into the list of coding scheme designators if
     *  not already contained in the list.  In any case the specified item is selected as
     *  the current one.  See definition of 'ItemStruct' above for VR, VM and Type.
     ** @param  codingSchemeDesignator  coding scheme designator of the item to be added.
     *                                  Designators beginning with "99" and the designator
     *                                  "L" are defined to be private or local coding schemes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &codingSchemeDesignator);

    /** remove the current item from the list.
     *  After sucessful removal the cursor is set to the next valid position.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem();

    /** remove the specified item from the list.
     *  After sucessful removal the cursor is set to the next valid position.
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
     ** @return reference to the resulting string (might be empty)
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

    /** get the responsible organization of the currently selected item
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getResponsibleOrganization(OFString &stringValue) const;

    /** set the coding scheme registry of the currently selected entry.
     *  See definition of 'ItemStruct' above for VR, VM and Type.
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeRegistry(const OFString &value);

    /** set the coding scheme UID of the currently selected entry.
     *  See definition of 'ItemStruct' above for VR, VM and Type.
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeUID(const OFString &value);

    /** set the coding scheme external ID of the currently selected entry.
     *  See definition of 'ItemStruct' above for VR, VM and Type.
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeExternalID(const OFString &value);

    /** set the coding scheme name of the currently selected entry.
     *  See definition of 'ItemStruct' above for VR, VM and Type.
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeName(const OFString &value);

    /** set the coding scheme version of the currently selected entry.
     *  See definition of 'ItemStruct' above for VR, VM and Type.
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodingSchemeVersion(const OFString &value);

    /** set the responsible organization of the currently selected entry.
     *  See definition of 'ItemStruct' above for VR, VM and Type.
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setResponsibleOrganization(const OFString &value);


  protected:

    /** add the specified coding scheme to the list (if not existent)
     ** @param  codingSchemeDesignator  coding scheme designator of the item to be added
     *  @param  item       reference to item pointer (result variable)
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

    // copy constructor - not implemented!
    DSRCodingSchemeIdentificationList(const DSRCodingSchemeIdentificationList &);
    // assignment operator - not implemented!
    DSRCodingSchemeIdentificationList &operator=(const DSRCodingSchemeIdentificationList &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcsidl.h,v $
 *  Revision 1.12  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2010-02-22 11:39:55  uli
 *  Remove some unneeded includes.
 *
 *  Revision 1.10  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.9  2007-11-15 16:33:19  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.8  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.7  2006/05/11 09:18:21  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.6  2005/12/08 16:04:57  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2004/11/22 16:39:09  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.4  2004/04/07 12:02:53  joergr
 *  Added missing member variables to constructor's member initialization list
 *  to avoid warnings reported by gcc.
 *
 *  Revision 1.3  2004/01/16 09:57:33  joergr
 *  Added clear() method to struct ItemStruct.
 *
 *  Revision 1.2  2003/12/16 16:18:35  joergr
 *  Added note on coding scheme identfication and private coding schemes.
 *
 *  Revision 1.1  2003/09/10 13:16:13  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdenticationSequence as
 *  required by CP 324.
 *
 *
 */
