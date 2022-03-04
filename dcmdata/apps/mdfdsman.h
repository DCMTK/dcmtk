/*
 *
 *  Copyright (C) 2003-2022, OFFIS e.V.
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
 *  Purpose: Class for modifying DICOM files
 *
 */

#ifndef MDFDSMAN_H
#define MDFDSMAN_H

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/ofstd/ofcmdln.h"


// forward declarations
class DcmDataset;
class DcmFileFormat;
class DcmElement;


/** This class encapsulates data structures and operations for modifying
 *  DICOM files. Therefore it allows the process of load->modify->save to
 *  provide this service.
 */
class MdfDatasetManager
{
public:

    /** Constructor, initializes member-variables
     */
    MdfDatasetManager();

    /** Destructor
     */
    ~MdfDatasetManager();

    /** Loads a file into dataset manager
     *  @param file_name file to be loaded
        @param readMode read file with or without metaheader. Default=autodetect
        @param xfer try to read with this transfer syntax. Default=autodetect
        @param createIfNecessary If true, the file is created if it does not exist
     *  @return returns EC_Normal if everything is OK, else an error
     */
    OFCondition loadFile(const char *file_name,
                         const E_FileReadMode readMode = ERM_autoDetect,
                         const E_TransferSyntax xfer = EXS_Unknown,
                         const OFBool createIfNecessary = OFFalse);

    /** Modifies/Inserts a path (with a specific value if desired).
     *  @param tag_path path to item/element
     *  @param value denotes new value of tag
     *  @param only_modify if true, only existing tags are processed. If false,
     *                     any not existing tag is inserted
     *  @param update_metaheader updates metaheader UIDs, if related UIDs in
     *                           dataset are changed (default=true)
     *  @param ignore_missing_tags if true, tags that could not be found
     *                             while modifying (only_modify must be true)
     *                             are handled as non-errors
     *  @param no_reservation_checks if true, any missing private reservation
     *                               tags are ignored when inserting private
     *                               tags. Only makes sense w/o only_modify
     *  @return returns EC_Normal if everything is OK, else an error
     */
    OFCondition modifyOrInsertPath(OFString tag_path,
                                   const OFString &value,
                                   const OFBool only_modify,
                                   const OFBool update_metaheader = OFTrue,
                                   const OFBool ignore_missing_tags = OFFalse,
                                   const OFBool no_reservation_checks = OFFalse);

    /** Modifies/Inserts a path with a specific value read from file
     *  @param tag_path path to item/element
     *  @param filename name of the file from which the value should be read
     *  @param only_modify if true, only existing tags are processed. If false,
     *                     any not existing tag is inserted
     *  @param update_metaheader updates metaheader UIDs, if related UIDs in
     *                           dataset are changed (default=true)
     *  @param ignore_missing_tags if true, tags that could not be found
     *                             while modifying (only_modify must be true)
     *                             are handled as non-errors
     *  @param no_reservation_checks if true, any missing private reservation
     *                               tags are ignored when inserting private
     *                               tags. Only makes sense w/o only_modify
     *  @return returns EC_Normal if everything is OK, else an error
     */
    OFCondition modifyOrInsertFromFile(OFString tag_path,
                                       const OFString &filename,
                                       const OFBool only_modify,
                                       const OFBool update_metaheader = OFTrue,
                                       const OFBool ignore_missing_tags = OFFalse,
                                       const OFBool no_reservation_checks = OFFalse);

    /** Modifies all matching tags in dataset to a new value
     *  @param tag_path denotes, which tag to modify
     *  @param value denotes new value of tag
     *  @param update_metaheader if true, metaheader UIDs are updated,
     *         if related dataset UIDs are changed, (default=true)
     *  @param count returns the number of tags that were affected
     *  @param ignore_missing_tags if true, tags that could not be found
     *                             while modifying are handled as non-errors
     *  @return returns EC_Normal if everything is OK, else an error
     */
    OFCondition modifyAllTags(OFString tag_path,
                              const OFString &value,
                              const OFBool update_metaheader,
                              int &count,
                              const OFBool ignore_missing_tags = OFFalse);

    /** Deletes tag in dataset
     *  @param tag_path holds complete path to tag
     *  @param all_tags if true, tag is deleted at all levels of dataset,
     *                  else only 1st level is accessed
     *  @param ignore_missing_tags if true, tags that could not be found
     *                             while modifying are handled as non-errors
     *  @return returns EC_Normal if everything is OK, else an error
     */
    OFCondition deleteTag(OFString tag_path,
                          const OFBool all_tags,
                          const OFBool ignore_missing_tags);

    /** Deletes all private data from file,\ i.e. all tags having
     *  odd group numbers.
     *  @return EC_Normal, if deletion was successful; error code otherwise
     */
    OFCondition deletePrivateData();

    /** Inserts new Study, Series or SOP Instance UID. If SOP Instance
     *  UID is generated, the related tag in the metaheader is deleted, too
     *  so that the new UID is also applied there when saving to disk later.
     *  @param uidKey [in] The Instance UID to insert (study, series or sop
     *                instance UID key permitted).
     *  @return EC_Normal, if insertion was successful, error otherwise
     */
    OFCondition generateAndInsertUID(const DcmTagKey &uidKey);

     /** Saves current dataset back to a file. Caution: After saving
     *  MdfDatasetManager keeps working on old filename.
     *  @param file_name filename to save to
     *  @param opt_xfer transfer syntax to save to (EXS_Unknown: don't change)
     *  @param opt_enctype write with explicit or implicit length encoding
     *  @param opt_glenc option to set group length calculation mode
     *  @param opt_padenc sets padding option
     *  @param opt_filepad pad file to a multiple of this options value
     *  @param opt_itempad pad item to a multiple of this options value
     *  @param opt_dataset if true, write only dataset, else write fileformat
     *  @return returns EC_Normal if everything is OK, else an error
     */
    OFCondition saveFile(const char *file_name,
                         E_TransferSyntax opt_xfer = EXS_Unknown,
                         E_EncodingType opt_enctype = EET_UndefinedLength,
                         E_GrpLenEncoding opt_glenc = EGL_recalcGL,
                         E_PaddingEncoding opt_padenc = EPD_noChange,
                         OFCmdUnsignedInt opt_filepad = 0,
                         OFCmdUnsignedInt opt_itempad = 0,
                         OFBool opt_dataset = OFFalse);

    /** Saves current dataset back to file using original filename and original
     *  parameters like transfer syntax, padding etc.
     *  @return returns EC_Normal if everything is OK, else an error
     */
    OFCondition saveFile();

    /** Returns the dataset that this MdfDatasetManager handles.
     *  You should use the returned object with care to avoid side effects with
     *  other class methods that modify this object, too.
     *  @return returns the dataset, this MdfDatasetManager manages and NULL, if
     *          no dataset is loaded
     */
    DcmDataset* getDataset();


    /** Returns the DcmFileFormat that this MdfDatasetManager handles.
     *  You should use the returned object with care to avoid side-effects with
     *  other class methods that modify this object, too.
     *  @return returns the DcmFileFormat, this MdfDatasetManager manages and
     *          NULL, if no file is loaded
     */
    DcmFileFormat* getFileFormat();

    /** Returns filename of the file that is currently loaded.
     *  @return returns filename and "" if no file is loaded.
     */
    OFString getFilename() const;

    /** Sets whether attributes with VR of UN should be modified or
     *  left alone.
     *  @param modifyUNValues [in] If set, UN values will be modified (default)
     */
    void setModifyUNValues(OFBool modifyUNValues);

protected:

    /** modifies element to a specific value
     *  @param elem element that should be changed
     *  @param value the value, the element should be changed to
     *  @return OFCondition, which returns an error code if an error occurs
     */
    OFCondition startModify(DcmElement *elem,
                            const OFString &value);

    /** If key is the tag for SOPInstanceUID or SOPClassUID, then this function
     *  removes the related MediaStorage UIDs from the metaheader. The
     *  metaheader is then updated "automagically" when the file is saved back to
     *  disk.
     *  @param key tag to examine
     */
    void deleteRelatedMetaheaderTag(const DcmTagKey &key);

    /** Returns true, if given tag key can be found in dictionary
     *  @param search_key tag to lookup
     *  @return OFTrue if tag could be found, else OFFalse
     */
    OFBool isTagInDictionary(const DcmTagKey &search_key);

    /** Make user user is not trying to replace encapsulated Pixel Data element value.
     *  This is currently not implemented since there is no direct / obvious way to insert
     *  raw data into the pixel sequence structure managed by the DcmPixelData class.
     *  @param elem The potential Pixel Data element to be checked
     *  @return EC_Normal if modification is possible, error code otherwise
     */
    OFCondition checkPixelDataInsertion(DcmElement* elem);

private:

    /// name of file that is currently loaded
    OFString current_file;

    /// will hold file to modify
    DcmFileFormat *dfile;

    /// will hold the dataset that should be modified
    DcmDataset *dset;

    /// if enabled, no value modifications on attributes having VR of UN
    /// are not executed
    OFBool ignore_un_modifies;

    /** private undefined assignment operator
     */
    MdfDatasetManager &operator=(const MdfDatasetManager &);

    /** private undefined copy constructor
     */
    MdfDatasetManager(const MdfDatasetManager &);
};

#endif // MDFDSMAN_H
