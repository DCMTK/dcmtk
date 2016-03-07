/*
 *
 *  Copyright (C) 2002-2016, OFFIS e.V.
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
 *    classes: DSRSOPInstanceReferenceList
 *             - InstanceStruct, SeriesStruct, StudyStruct
 *
 */


#ifndef DSRSOPRF_H
#define DSRSOPRF_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcodvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for SOP instance reference lists
 */
class DCMTK_DCMSR_EXPORT DSRSOPInstanceReferenceList
  : public DSRTypes
{

  public:

    /** constructor
     ** @param  sequence  DICOM tag specifying the attribute (sequence) of the reference list
     */
    DSRSOPInstanceReferenceList(const DcmTagKey &sequence);

    /** destructor
     */
    ~DSRSOPInstanceReferenceList();

    /** clear list of references
     */
    void clear();

    /** check whether list of references is empty
     ** @return OFTrue if list is empty, OFFalse otherwise
     */
    OFBool empty() const;

    /** get number of instance stored in the list of references
     ** @return number of instances
     */
    size_t getNumberOfInstances() const;

    /** read list of referenced SOP instances.
     *  The hierarchical structure is automatically reorganized in a way that each study,
     *  each series (within a study) and each instance (within a series) only exist once,
     *  i.e. the structure might look different when written back to a dataset.  However,
     *  the content is identical and this way of storing information saves storage space.
     ** @param  dataset  DICOM dataset from which the data should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const size_t flags);

    /** write list of referenced SOP instances.
     *  Does nothing if list is empty.
     ** @param  dataset  DICOM dataset to which the data should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset) const;

    /** read list of references from XML document.
     *  The hierarchical structure is automatically reorganized in a way that each study,
     *  each series (within a study) and each instance (within a series) only exist once,
     *  i.e. the structure might look different when written back to a dataset.  However,
     *  the content is identical and this way of storing information saves storage space.
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const DSRXMLDocument &doc,
                        DSRXMLCursor cursor,
                        const size_t flags);

    /** write current list of references in XML format
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

    /** add the specified item to the list of references.
     *  Before adding the item, the given UID values are usually checked.  If one of the
     *  values is invalid, nothing is done but an error is returned.  If successful, the
     *  item is inserted into the hierarchical structure of studies, series and instances,
     *  if not already contained in the list. In any case, the specified item is selected
     *  as the current one.
     ** @param  studyUID     study instance UID of the entry to be added
     *  @param  seriesUID    series instance UID of the entry to be added
     *  @param  sopClassUID  SOP class UID of the entry to be added
     *  @param  instanceUID  SOP instance UID of the entry to be added
     *  @param  check        if enabled, check values for validity before adding them.
     *                       See checkSOPInstance() method for details.  Empty values are
     *                       never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &studyUID,
                        const OFString &seriesUID,
                        const OFString &sopClassUID,
                        const OFString &instanceUID,
                        const OFBool check = OFTrue);

    /** add item from specified DICOM dataset to the list of references.
     *  Internally an item representing the given dataset is inserted into the hierarchical
     *  structure of studies, series and instances, if not already contained in the list.
     *  In any case, the specified item is selected as the current one.
     ** @param  dataset  reference to DICOM dataset from which the relevant UIDs are retrieved
     *  @param  check    if enabled, check values for validity before adding them.  See
     *                   checkSOPInstance() method for details.  An empty value is never
     *                   accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(DcmItem &dataset,
                        const OFBool check = OFTrue);

    /** remove the current item from the list of referenced SOP instances.
     *  After successful removal the cursor is set to the next valid position.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem();

    /** remove the specified item from the list of references.
     *  After successful removal the cursor is set to the next valid position.
     ** @param  sopClassUID  SOP class UID of the item to be removed
     *  @param  instanceUID  SOP instance UID of the item to be removed
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const OFString &sopClassUID,
                           const OFString &instanceUID);

    /** remove the specified item from the list of references.
     *  After successful removal the cursor is set to the next valid position.
     ** @param  studyUID     study instance UID of the item to be removed
     *  @param  seriesUID    series instance UID of the item to be removed
     *  @param  instanceUID  SOP instance UID of the item to be removed
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const OFString &studyUID,
                           const OFString &seriesUID,
                           const OFString &instanceUID);

    /** select the specified item as the current one
     ** @param  sopClassUID  SOP class UID of the item to be selected
     *  @param  instanceUID  SOP instance UID of the item to be selected
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const OFString &sopClassUID,
                         const OFString &instanceUID);

    /** select the specified item as the current one
     ** @param  studyUID     study instance UID of the item to be selected
     *  @param  seriesUID    series instance UID of the item to be selected
     *  @param  instanceUID  SOP instance UID of the item to be selected
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const OFString &studyUID,
                         const OFString &seriesUID,
                         const OFString &instanceUID);

    /** select the first item in the list.
     *  That means the first instance in the first series of the first study
     *  is selected (if available).
     ** @return status, EC_Normal if successful, an error code otherwise.
     *    (e.g. if the list is empty)
     */
    OFCondition gotoFirstItem();

    /** select the next item in the list.
     *  That means the next instance in the current series, or the first instance
     *  in the next series, or the first instance in the first series of the next
     *  study is selected (if available).  The combination of this function and
     *  gotoFirstItem() allows to iterate over all referenced SOP instances.
     ** @return status, EC_Normal if successful, an error code otherwise.
     *    (e.g. if the end of the list has been reached)
     */
    OFCondition gotoNextItem();

    /** get the study instance UID of the currently selected entry
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getStudyInstanceUID(OFString &stringValue) const;

    /** get the series instance UID of the currently selected entry
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSeriesInstanceUID(OFString &stringValue) const;

    /** get the SOP instance UID of the currently selected entry
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPInstanceUID(OFString &stringValue) const;

    /** get the SOP class UID of the currently selected entry
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPClassUID(OFString &stringValue) const;

    /** get name associated with the SOP class UID of the currently selected entry
     ** @param  stringValue  reference to string variable in which the result is stored
     *  @param  defaultName  string value that is returned if the SOP class UID is unknown
     ** @return reference to the resulting string (might be empty even if 'defaultName' is
     *          non-empty, e.g. in case no entry is currently selected)
     */
    const OFString &getSOPClassName(OFString &stringValue,
                                    const OFString &defaultName = "unknown SOP Class UID") const;

    /** get the retrieve application entity title of the currently selected entry (optional).
     *  The resulting string may contain multiple values separated by a backslash ("\").
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getRetrieveAETitle(OFString &stringValue) const;

    /** get the retrieve location UID of the currently selected entry (optional)
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getRetrieveLocationUID(OFString &stringValue) const;

    /** get the storage media file set ID of the currently selected entry (optional)
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getStorageMediaFileSetID(OFString &stringValue) const;

    /** get the storage media file set UID of the currently selected entry (optional)
     ** @param  stringValue  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getStorageMediaFileSetUID(OFString &stringValue) const;

    /** get purpose of reference code of the currently selected entry (optional)
     ** @param  codeValue  variable where a copy of the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getPurposeOfReference(DSRCodedEntryValue &codeValue) const;

    /** set the retrieve application entity title of the currently selected entry
     ** @param  value  string value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (AE) and VM (1-n) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRetrieveAETitle(const OFString &value,
                                   const OFBool check = OFTrue);

    /** set the retrieve location UID of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRetrieveLocationUID(const OFString &value,
                                       const OFBool check = OFTrue);

    /** set the storage media file set ID of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStorageMediaFileSetID(const OFString &value,
                                         const OFBool check = OFTrue);

    /** set the storage media file set UID of the currently selected entry
     ** @param  value  string value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStorageMediaFileSetUID(const OFString &value,
                                          const OFBool check = OFTrue);

    /** set purpose of reference code of the currently selected entry.
     *  Before setting the value, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.
     ** @param  codeValue  value to be set (optional).  Use an empty code to remove the
     *                     current value.
     *  @param  check      if enabled, check value for validity before setting it.  See
     *                     checkPurposeOfReference() method for details.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPurposeOfReference(const DSRCodedEntryValue &codeValue,
                                      const OFBool check = OFTrue);


  protected:

    /** Internal structure defining the instance list items
     */
    struct DCMTK_DCMSR_EXPORT InstanceStruct
    {
        /** constructor
         ** @param  sopClassUID  SOP class UID
         ** @param  instanceUID  SOP instance UID
         */
        InstanceStruct(const OFString &sopClassUID,
                       const OFString &instanceUID);

        /** clear additional information
         */
        void clear();

        /// SOP Class UID (VR=UI, type 1)
        const OFString SOPClassUID;
        /// SOP Instance UID (VR=UI, type 1)
        const OFString InstanceUID;
        /// Purpose of Reference Code Sequence (VR=SQ, type 3).
        /// NB: According to the DICOM standard, "One or more Items are permitted in this
        ///     sequence."  However, the current implementation only supports a single
        ///     item.  Also see documentation of DSRCodedEntryValue::readSequence().
        DSRCodedEntryValue PurposeOfReference;
    };

    /** Internal structure defining the series list items
     */
    struct DCMTK_DCMSR_EXPORT SeriesStruct
    {
        /** constructor
         ** @param  seriesUID  series instance UID
         */
        SeriesStruct(const OFString &seriesUID);

        /** destructor
         */
        ~SeriesStruct();

        /** get number of instance stored in the list of instances
         ** @return number of instances
         */
        size_t getNumberOfInstances() const;

        /** read instance level attributes from dataset
         ** @param  dataset  DICOM dataset from which the list should be read
         *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition read(DcmItem &dataset,
                         const size_t flags);

        /** write series and instance level attributes to dataset
         ** @param  dataset  DICOM dataset to which the list should be written
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition write(DcmItem &dataset) const;

        /** read series and instance level attributes from XML document
         ** @param  doc     document containing the XML file content
         *  @param  cursor  cursor pointing to the starting node
         *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition readXML(const DSRXMLDocument &doc,
                            DSRXMLCursor cursor,
                            const size_t flags);

        /** write series and instance level attributes in XML format
         ** @param  stream  output stream to which the XML document is written
         *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition writeXML(STD_NAMESPACE ostream &stream,
                             const size_t flags) const;

        /** set cursor to the specified instance (if existent)
         ** @param  instanceUID  SOP instance UID of the entry to be searched for
         ** @return pointer to the instance structure if successful, NULL otherwise
         */
        InstanceStruct *gotoInstance(const OFString &instanceUID);

        /** select the first item in the list.
         *  That means the first instance in the current series.
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition gotoFirstItem();

        /** select the next item in the list.
         *  That means the next instance in the current series (if available).
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition gotoNextItem();

        /** add new entry to the list of instances (if not already existent).
         *  Finally, the specified item is selected as the current one.
         ** @param  sopClassUID  SOP class UID of the entry to be added
         *  @param  instanceUID  SOP instance UID of the entry to be added
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition addItem(const OFString &sopClassUID,
                            const OFString &instanceUID);

        /** remove the current item from the list of instances.
         *  After successful removal the cursor is set to the next valid position.
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition removeItem();

        /// Series Instance UID (VR=UI, VM=1, type 1)
        const OFString SeriesUID;
        /// Retrieve Application Entity Title (VR=AE, VM=1-n, type 3)
        OFString RetrieveAETitle;
        /// Retrieve Location UID (VR=UI, VM=1, type 3)
        OFString RetrieveLocationUID;
        /// Storage Media File Set ID (VR=SH, VM=1, type 3)
        OFString StorageMediaFileSetID;
        /// Storage Media File Set UID (VR=UI, VM=1, type 3)
        OFString StorageMediaFileSetUID;

        /// list of referenced instances
        OFList<InstanceStruct *> InstanceList;
        /// currently selected instance (cursor)
        OFListIterator(InstanceStruct *) Iterator;
    };

    /** To make VC6 happy.
     */
    friend struct SeriesStruct;

    /** Internal structure defining the study list items
     */
    struct DCMTK_DCMSR_EXPORT StudyStruct
    {
        /** constructor
         ** @param  studyUID  study instance UID
         */
        StudyStruct(const OFString &studyUID);

        /** destructor
         */
        ~StudyStruct();

        /** get number of instance stored in the list of series
         ** @return number of instances
         */
        size_t getNumberOfInstances() const;

        /** read series and instance level from dataset
         ** @param  dataset  DICOM dataset from which the list should be read
         *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition read(DcmItem &dataset,
                         const size_t flags);

        /** write study, series and instance level attributes to dataset
         ** @param  dataset  DICOM dataset to which the list should be written
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition write(DcmItem &dataset) const;

        /** read study, series and instance level attributes from XML document
         ** @param  doc     document containing the XML file content
         *  @param  cursor  cursor pointing to the starting node
         *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition readXML(const DSRXMLDocument &doc,
                            DSRXMLCursor cursor,
                            const size_t flags);

        /** write study, series and instance level attributes in XML format
         ** @param  stream  output stream to which the XML document is written
         *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition writeXML(STD_NAMESPACE ostream &stream,
                             const size_t flags) const;

        /** set cursor to the specified series entry (if existent)
         ** @param  seriesUID  series instance UID of the entry to be searched for
         ** @return pointer to the series structure if successful, NULL otherwise
         */
        SeriesStruct *gotoSeries(const OFString &seriesUID);

        /** set cursor to the specified instance entry (if existent)
         ** @param  instanceUID  SOP instance UID of the entry to be searched for
         ** @return pointer to the instance structure if successful, NULL otherwise
         */
        InstanceStruct *gotoInstance(const OFString &instanceUID);

        /** select the first item in the list.
         *  That means the first instance in the first series of the current study.
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition gotoFirstItem();

        /** select the next item in the list.
         *  That means the next instance in the current series, or the first instance
         *  in the next series (if available).
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition gotoNextItem();

        /** add new entry to the list of series and instances (if not already existent).
         *  Finally, the specified items are selected as the current one.
         ** @param  seriesUID    series instance UID of the entry to be added
         *  @param  sopClassUID  SOP class UID of the entry to be added
         *  @param  instanceUID  SOP instance UID of the entry to be added
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition addItem(const OFString &seriesUID,
                            const OFString &sopClassUID,
                            const OFString &instanceUID);

        /** remove the current item from the list of series and instances.
         *  After successful removal the cursors are set to the next valid position.
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition removeItem();

        /** remove empty/incomplete items from the list.
         *  (e.g. series with no instances)
         *  Please note that this function modifies the value of 'Iterator'.
         */
        void removeIncompleteItems();

        /// Study Instance UID (VR=UI, type 1)
        const OFString StudyUID;

        /// list of referenced series
        OFList<SeriesStruct *> SeriesList;
        /// currently selected series (cursor)
        OFListIterator(SeriesStruct *) Iterator;
    };

    /** To make VC6 happy.
     */
    friend struct StudyStruct;

    /** set cursor to the specified study entry (if existent)
     ** @param  studyUID  study instance UID of the entry to be searched for
     ** @return pointer to the study structure if successful, NULL otherwise
     */
    StudyStruct *gotoStudy(const OFString &studyUID);

    /** get pointer to currently selected study structure (if any)
     ** @return pointer to the study structure if successful, NULL otherwise
     */
    StudyStruct *getCurrentStudy() const;

    /** get pointer to currently selected series structure (if any)
     ** @return pointer to the series structure if successful, NULL otherwise
     */
    SeriesStruct *getCurrentSeries() const;

    /** get pointer to currently selected instance structure (if any)
     ** @return pointer to the instance structure if successful, NULL otherwise
     */
    InstanceStruct *getCurrentInstance() const;

    /** remove empty/incomplete items from the list.
     *  (e.g. series with no instances or studies with no series)
     *  Please note that this function modifies the value of 'Iterator'.
     */
    void removeIncompleteItems();

    /** check the four specified UID values for validity.
     *  Currently, the only checks performed are that the strings are non-empty and that
     *  the given values conform to the corresponding VR (UI) and VM (1).
     ** @param  studyUID     study instance UID to be checked
     *  @param  seriesUID    series instance UID to be checked
     *  @param  sopClassUID  SOP class UID to be checked
     *  @param  instanceUID  SOP instance UID to be checked
     ** @return status, EC_Normal if all values are valid, an error code otherwise
     */
    OFCondition checkSOPInstance(const OFString &studyUID,
                                 const OFString &seriesUID,
                                 const OFString &sopClassUID,
                                 const OFString &instanceUID) const;

    /** check the specified purpose of reference code for validity.
     *  Internally, the method DSRCodedEntryValue::checkCurrentValue()
     *  is used for this purpose (if the code is not empty).
     ** @param  purposeOfReference  purpose of reference code to be checked
     ** @return status, EC_Normal if code is valid, an error code otherwise
     */
    OFCondition checkPurposeOfReference(const DSRCodedEntryValue &purposeOfReference) const;


  private:

    /// DICOM tag specifying the attribute (sequence) of the reference list
    const DcmTagKey SequenceTag;

    /// list of studies
    OFList<StudyStruct *> StudyList;
    /// internal cursor to current (selected) list item
    OFListIterator(StudyStruct *) Iterator;
    /// specific character set used for checking purposes
    OFString SpecificCharacterSet;

    // default constructor - not implemented!
    DSRSOPInstanceReferenceList();
    // copy constructor - not implemented!
    DSRSOPInstanceReferenceList(const DSRSOPInstanceReferenceList &);
    // assignment operator - not implemented!
    DSRSOPInstanceReferenceList &operator=(const DSRSOPInstanceReferenceList &);
};


#endif
