/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRSOPInstanceReferenceList
 *             - InstanceStruct, SeriesStruct, StudyStruct
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-08-30 14:16:59 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRSOPRF_H
#define DSRSOPRF_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "oflist.h"
#include "ofstring.h"
#include "ofconsol.h"
#include "ofcond.h"

#include "dcitem.h"
#include "dctagkey.h"

#include "dsrtypes.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for SOP instance reference lists
 */
class DSRSOPInstanceReferenceList
  : public DSRTypes
{

  public:

    /** Internal structure defining the instance list items
     */
    struct InstanceStruct
    {
        /** constructor
         ** @param  sopClassUID  SOP class UID
         ** @param  instanceUID  SOP instance UID
         */
        InstanceStruct(const OFString &sopClassUID,
                       const OFString &instanceUID);

        /// SOP class UID (VR=UI, VM=1)
        const OFString SOPClassUID;
        /// SOP instance UID (VR=UI, VM=1)
        const OFString InstanceUID;
    };

    /** Internal structure defining the series list items
     */
    struct SeriesStruct
    {
        /** constructor
         ** @param  seriesUID  series instance UID
         */
        SeriesStruct(const OFString &seriesUID);

        /** destructor
         */
        ~SeriesStruct();

        /** read instance level attributes from dataset
         ** @param  dataset    DICOM dataset from which the list should be read
         *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition read(DcmItem &dataset,
                         OFConsole *logStream);

        /** write series and instance level attributes to dataset
         ** @param  dataset    DICOM dataset to which the list should be written
         *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition write(DcmItem &dataset,
                          OFConsole *logStream) const;

        /** write series and instance level attributes in XML format
         ** @param  stream  output stream to which the XML document is written
         *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition writeXML(ostream &stream,
                             const size_t flags = 0) const;

        /** set cursor to the specified instance (if existent)
         ** @param  instanceUID  SOP instance UID of the entry to be searched for
         ** @return pointer to the instance structure if successful, NULL otherwise
         */
        InstanceStruct *gotoInstance(const OFString &instanceUID);

        /** add new entry to the list of instances (if not already existent).
         *  Finally, the specified item is selected as the current one.
         ** @param  sopClassUID  SOP class UID of the entry to be added
         *  @param  instanceUID  SOP instance UID of the entry to be added
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition addItem(const OFString &sopClassUID,
                            const OFString &instanceUID);

        /** remove the current item from the list of instances.
         *  After sucessful removal the cursor is set to the next valid position.
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition removeItem();

        /// series instance UID (VR=UI, VM=1)
        const OFString SeriesUID;
        /// optional: retrieve application entity title (VR=AE, VM=1-n)
        OFString RetrieveAETitle;
        /// optional: storage media file set ID (VR=SH, VM=1)
        OFString StorageMediaFileSetID;
        /// optional: storage media file set UID (VR=UI, VM=1)
        OFString StorageMediaFileSetUID;

        /// list of referenced instances
        OFList<InstanceStruct *> InstanceList;
        /// currently selected instance (cursor)
        OFListIterator(InstanceStruct *) Iterator;
    };

    /** Internal structure defining the study list items
     */
    struct StudyStruct
    {
        /** constructor
         ** @param  studyUID  study instance UID
         */
        StudyStruct(const OFString &studyUID);

        /** destructor
         */
        ~StudyStruct();

        /** read series and instance level from dataset
         ** @param  dataset    DICOM dataset from which the list should be read
         *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition read(DcmItem &dataset,
                         OFConsole *logStream);

        /** write study, series and instance level attributes to dataset
         ** @param  dataset    DICOM dataset to which the list should be written
         *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition write(DcmItem &dataset,
                          OFConsole *logStream) const;

        /** write study, series and instance level attributes in XML format
         ** @param  stream  output stream to which the XML document is written
         *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition writeXML(ostream &stream,
                             const size_t flags = 0) const;

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
         *  After sucessful removal the cursors are set to the next valid position.
         ** @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition removeItem();

        /// study instance UID (VR=UI, VM=1)
        const OFString StudyUID;

        /// list of referenced series
        OFList<SeriesStruct *> SeriesList;
        /// currently selected series (cursor)
        OFListIterator(SeriesStruct *) Iterator;
    };


    /** constructor
     ** @param  tagKey  DICOM tag specifying the attribute (sequence) of the reference list
     */
    DSRSOPInstanceReferenceList(const DcmTagKey &sequence);

    /** destructor
     */
    ~DSRSOPInstanceReferenceList();

    /** clear list of references
     */
    void clear();

    /** check whether list of references is empty
     *  @return OFTrue if list is empty, OFFalse otherwise
     */
    OFBool empty() const;

    /** read list of referenced SOP instances
     ** @param  dataset    DICOM dataset from which the data should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     OFConsole *logStream);

    /** write list of referenced SOP instances
     ** @param  dataset    DICOM dataset to which the data should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset,
                      OFConsole *logStream) const;

    /** write current list of references in XML format
     ** @param  stream  output stream to which the XML data is written
     *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(ostream &stream,
                         const size_t flags = 0) const;

    /** add the specified item to the list of references.
     *  Internally the item is inserted into the hierarchical structure of studies, series
     *  and instances, if not already contained in the list. In any case the specified item
     *  is selected as the current one.
     ** @param  studyUID     study instance UID of the entry to be added
     *  @param  seriesUID    series instance UID of the entry to be added
     *  @param  sopClassUID  SOP class UID of the entry to be added
     *  @param  instanceUID  SOP instance UID of the entry to be added
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(const OFString &studyUID,
                        const OFString &seriesUID,
                        const OFString &sopClassUID,
                        const OFString &instanceUID);

    /** remove the current item from the list of referenced SOP instances.
     *  After sucessful removal the cursor is set to the next valid position.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem();

    /** remove the specified item from the list of references.
     *  After sucessful removal the cursor is set to the next valid position.
     ** @param  sopClassUID  SOP class UID of the item to be removed
     *  @param  instanceUID  SOP instance UID of the item to be removed
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const OFString &sopClassUID,
                           const OFString &instanceUID);

    /** remove the specified item from the list of references.
     *  After sucessful removal the cursor is set to the next valid position.
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
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const OFString &sopClassUID,
                         const OFString &instanceUID);

    /** select the specified item as the current one
     ** @param  studyUID     study instance UID of the item to be selected
     *  @param  seriesUID    series instance UID of the item to be selected
     *  @param  instanceUID  SOP instance UID of the item to be selected
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const OFString &studyUID,
                         const OFString &seriesUID,
                         const OFString &instanceUID);

    /** get the study instance UID of the currently selected entry
     ** @param  string  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getStudyInstanceUID(OFString &string) const;

    /** get the series instance UID of the currently selected entry
     ** @param  string  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSeriesInstanceUID(OFString &string) const;

    /** get the SOP instance UID of the currently selected entry
     ** @param  string  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPInstanceUID(OFString &string) const;

    /** get the SOP class UID of the currently selected entry
     ** @param  string  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getSOPClassUID(OFString &string) const;

    /** get the retrieve application entity title of the currently selected entry (optional).
     *  The resulting string may contain multiple values separated by a backslash ("\").
     ** @param  string  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getRetrieveAETitle(OFString &string) const;

    /** get the storage media file set ID of the currently selected entry (optional)
     ** @param  string  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getStorageMediaFileSetID(OFString &string) const;

    /** get the storage media file set UID of the currently selected entry (optional)
     ** @param  string  reference to string variable in which the result is stored
     ** @return reference to the resulting string (might be empty)
     */
    const OFString &getStorageMediaFileSetUID(OFString &string) const;

    /** set the retrieve application entity title of the currently selected entry.
     *  Multiple values are to be separated by a backslash ("\").
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRetrieveAETitle(const OFString &value);

    /** set the storage media file set ID of the currently selected entry
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStorageMediaFileSetID(const OFString &value);

    /** set the storage media file set UID of the currently selected entry
     ** @param  value  string value to be set (use empty string to omit optional attribute)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStorageMediaFileSetUID(const OFString &value);


  protected:

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


  private:

    /// DICOM tag specifying the attribute (sequence) of the reference list
    const DcmTagKey SequenceTag;

    /// list of studies
    OFList<StudyStruct *> StudyList;
    /// internal cursor to current (selected) list item
    OFListIterator(StudyStruct *) Iterator;

    // default constructor - not implemented!
    DSRSOPInstanceReferenceList();
    // copy constructor - not implemented!
    DSRSOPInstanceReferenceList(const DSRSOPInstanceReferenceList &);
    // assignment operator - not implemented!
    DSRSOPInstanceReferenceList &operator=(const DSRSOPInstanceReferenceList &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrsoprf.h,v $
 *  Revision 1.4  2002-08-30 14:16:59  joergr
 *  Removed "friend" statements from class declaration and moved sub-classes to
 *  the "public" section (required for Sun CC 6).
 *
 *  Revision 1.3  2002/05/14 08:16:07  joergr
 *  Added removeItem() methods.
 *
 *  Revision 1.2  2002/05/07 14:04:44  joergr
 *  Added "friend" statements to class declaration (required for MSVC).
 *
 *  Revision 1.1  2002/05/07 12:49:31  joergr
 *  Added support for the Current Requested Procedure Evidence Sequence and the
 *  Pertinent Other Evidence Sequence to the dcmsr module.
 *
 *
 */
