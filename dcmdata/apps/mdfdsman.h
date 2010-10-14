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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for modifying DICOM files
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:52 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef MDFDSMAN_H
#define MDFDSMAN_H

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcxfer.h"


// forward declarations
class DcmDataset;
class DcmFileFormat;
class DcmElement;


/** This class encapsulates data structures and operations for modifying
 *  Dicom files. Therefore it allows the process of load->modify->save to
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
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition loadFile(const char *file_name,
                         const E_FileReadMode readMode = ERM_autoDetect,
                         const E_TransferSyntax xfer = EXS_Unknown);

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
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyOrInsertPath(OFString tag_path,
                                   const OFString &value,
                                   const OFBool &only_modify,
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
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyOrInsertFromFile(OFString tag_path,
                                       const OFString &filename,
                                       const OFBool &only_modify,
                                       const OFBool update_metaheader = OFTrue,
                                       const OFBool ignore_missing_tags = OFFalse,
                                       const OFBool no_reservation_checks = OFFalse);

    /** Modifies all matching tags in dataset to a new value
     *  @param tag_path denotes, which tag to modify
     *  @param value denotes new value of tag
     *  @param update_metaheader if true, metaheader uids are updated,
     *         if related dataset uids are changed, (default=true)
     *  @param count returns holds the number of tags, that were affected
     *  @param ignore_missing_tags if true, tags that could not be found
     *                             while modifying (only_modify must be true)
     *                             are handled as non-errors
     *  @return returns EC_normal if everything is ok, else an error
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
     *                             while modifying (only_modify must be true)
     *                             are handled as non-errors
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition deleteTag(OFString tag_path,
                          const OFBool all_tags,
                          const OFBool ignore_missing_tags);

    /** Deletes all private data from file, ie. all tags having
     *  odd group numbers.
     *  @return EC_Normal, if deletion was successful; error code otherwise
     */
    OFCondition deletePrivateData();

    /** Inserts new Study, Series or SOP Instance UID. If SOP Instance
     *  UID is generated, the related tag in the metaheader is deleted, too
     *  so that the new UID is also applied there when saving to disk later.
     *  @param uidkey [in] The Instance UID to insert (study, series or sop
     *                instance UID key permitted).
     *  @return EC_Normal, if insertion was successful, error otherwise
     */
    OFCondition generateAndInsertUID(const DcmTagKey &uidKey);

     /** Saves current dataset back to a file. Caution: After saving
     *  MdfDatasetManager keeps working on old filename.
     *  @param file_name filename to save to
     *  @param opt_xfer transfer syntax to save to (EXS_Unknown: dont change)
     *  @param opt_enctype write with explicit or implicit length encoding
     *  @param opt_glenc option to set group lenghth calculation mode
     *  @param opt_padenc sets padding option
     *  @param opt_filepad pad file to a multiple of this options value
     *  @param opt_itempad pad item to a multiple of this options value
     *  @param opt_dataset if true:ony write only dataset, else write fileformat
     *  @return returns EC_normal if everything is ok, else an error
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
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition saveFile();

    /** Returns the dataset, that this MdfDatasetManager handles.
     *  You should use the returned object with care to avoid
     *  sideeffects with other class methods, that modify this object, too.
     *  @return returns the dataset, this MdfDatasetManager manages and NULL, if
     *          no dataset is loaded
     */
    DcmDataset* getDataset();


    /** Returns the DcmFileFormat, that this MdfDatasetManager handles.
     *  You should use the returned object with care to avoid
     *  sideeffects with other class methods, that modify this object, too.
     *  @return returns the DcmFileFormat, this MdfDatasetManager manages and
     *          NULL, if no file is loaded
     */
    DcmFileFormat* getFileFormat();

    /** Returns filename of the file, that's loaded currently.
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
     *  @param elem element, that should be changed
     *  @param value the value, the element should be changed to
     *  @return OFCondition, which returns an error code if an error occurs
     */
    OFCondition startModify(DcmElement *elem,
                            const OFString &value);

    /** If key is the tag for SOPInstanceUID or SOPClassUID, then this function
     *  removes the related MediaStorage UIDs from the metaheader. The
     *  metaheader is then updated automagically when the file is saved back to
     *  disk.
     *  @param key tag to examine
     */
    void deleteRelatedMetaheaderTag(const DcmTagKey &key);

    /** Returns true, if given tag key can be found in dictionary
     *  @param search_key tag to lookup
     *  @return OFTrue if tag could be found, else OFFalse
     */
    OFBool isTagInDictionary(const DcmTagKey &search_key);

private:

    /// name of file, that is loaded currently
    OFString current_file;

    /// will hold file to modify
    DcmFileFormat *dfile;

    /// will hold the dataset, that should be modified
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


/*
** CVS/RCS Log:
** $Log: mdfdsman.h,v $
** Revision 1.22  2010-10-14 13:17:52  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.21  2010-05-20 15:44:55  joergr
** Added support for reading the value of insert/modify statements from a file.
** Removed some unnecessary include directives.
**
** Revision 1.20  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.19  2009-01-15 16:11:55  onken
** Reworked dcmodify to work with the new DcmPath classes for supporting
** wildcard paths and automatic insertion of missing attributes and items.
** Added options for private tag handling and modification of UN values and
** for ignoring errors resulting from missing tags during modify and erase
** operations. Further cleanups.
**
** Revision 1.18  2008-07-11 08:37:45  joergr
** Fixed typo in API documentation.
**
** Revision 1.17  2006/11/23 15:32:58  onken
** Made member variables private (before: protected)
**
** Revision 1.16  2005/12/08 15:46:50  meichel
** Updated Makefiles to correctly install header files
**
** Revision 1.15  2005/12/02 09:21:47  joergr
** Added new file read mode that makes it possible to distinguish between DICOM
** files, datasets and other non-DICOM files.  For this reason, the last
** parameter of method loadFile() changed from OFBool to E_FileReadMode.
**
** Revision 1.14  2005/11/30 16:41:41  onken
** Added standard parameter values for saveFile()
**
** Revision 1.13  2005/11/14 15:00:14  joergr
** Made method getFilename() const.
**
** Revision 1.12  2004/11/05 17:17:24  onken
** Added input and output options for dcmodify. minor code enhancements.
**
** Revision 1.11  2004/10/22 16:53:26  onken
** - fixed ignore-errors-option
** - major enhancements for supporting private tags
** - removed '0 Errors' output
** - modifications to groups 0000,0001,0002,0003,0005 and 0007 are blocked,
**   removing tags with group 0001,0003,0005 and 0007 is still possible
** - UID options:
**   - generate new study, series and instance UIDs
**   - When changing UIDs in dataset, related metaheader tags are updated
**     automatically
** - minor code improvements
**
** Revision 1.10  2004/05/14 12:08:36  onken
** Additional documentation added.
**
** Revision 1.9  2004/04/19 14:45:07  onken
** Restructured code to avoid default parameter values for "complex types" like
** OFString. Required for Sun CC 2.0.1.
**
** Revision 1.8  2003/12/17 17:07:22  onken
** MdfDatasetManager now remembers loaded filename. Additional save function
** added.
**
** Revision 1.7  2003/12/10 16:19:20  onken
** Changed API of MdfDatasetManager, so that its transparent for user, whether
** he wants to modify itemtags or tags at 1. level.
**
** Complete rewrite of MdfConsoleEngine. It doesn't support a batchfile any more,
** but now a user can give different modify-options at the same time on
** commandline. Other purifications and simplifications were made.
**
** Revision 1.6  2003/11/11 10:55:51  onken
** - debug-mechanism doesn't use debug(..) any more
** - comments purified
** - headers adjustet to debug-modifications
**
** Revision 1.5  2003/10/13 14:46:50  onken
** startModify(...) simplified (uses only putString to put element-values),
** this also allows now inserting and modifying of elements with VRM>1.
** Method getDataset() added.
**
** Revision 1.4  2003/10/01 14:04:03  onken
** Corrected doxygen-information in headerfiles
**
** Revision 1.3  2003/09/19 12:41:11  onken
** major bugfixes, new code structure, better error-handling, corrections for "dcmtk-coding-style",Handling of VR's corrected
**
** Revision 1.2  2003/07/09 12:13:13  meichel
** Included dcmodify in MSVC build system, updated headers
**
** Revision 1.1  2003/06/26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
