/*
 *
 *  Copyright (C) 2003-2005, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for modifying DICOM files
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2006-11-23 15:32:58 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef MDFDSMAN_H
#define MDFDSMAN_H

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmdata/dcvrat.h"

/** This class encapsulates data structures and operations for modifying
 *  Dicom files. Therefore it allows the process of load->modify->save to
 *  provide this service.
 */
class MdfDatasetManager
{
public:
    /** Constructor, initializes member-variables
     *  @param debug enables/disables debug-messages (off per default)
     */
    MdfDatasetManager(const OFBool debug=OFFalse);

    /** Destructor
     */
    ~MdfDatasetManager();

    /** Loads a file into dataset manager
     *  @param file_name file to be loaded
        @param readMode read file with or without metaheader
        @param xfer try to read with this transfer syntax. Default=autodetect
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition loadFile(const char *file_name,
                         const E_FileReadMode readMode=ERM_autoDetect,
                         const E_TransferSyntax xfer=EXS_Unknown);

    /** Modifies/Inserts a tag with a specific value
     *  @param tag_path holds complete path to tag
     *  @param value denotes new value of tag
     *  @param only_modify if true, only existing tags are processed. If false,
     *                     not existing tag is inserted
     *  @param update_metaheader updates metaheader UIDs, if related UIDs in
     *                           dataset are changed (default=true)
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyOrInsertTag(OFString tag_path,
                                  const OFString &value,
                                  const OFBool &only_modify,
                                  const OFBool update_metaheader=OFTrue);

    /** Modifies all matching tags in dataset to a new value
     *  @param tag_path denotes, which tag to modify
     *  @param value denotes new value of tag
     *  @param update_metaheader if true, metaheader uids are updated,
     *         if related dataset uids are changed, (default=true)
     *  @param count returns holds the number of tags, that were affected
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyAllTags(OFString tag_path,
                              const OFString &value,
                              const OFBool update_metaheader,
                              int &count);

    /** Deletes tag in dataset
     *  @param tag_path holds complete path to tag
     *  @param all_tags If true, tag is deleted at all levels of dataset,
     *                  else only 1. level is accessed
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition deleteTag(OFString tag_path,
                          const OFBool &all_tags);

    /** Generates new 'Study Instance UID' and inserts it into the dataset.
     *  'Series Instance UID' and 'SOP Instance UID' are not affected.
     */
    OFCondition generateNewStudyUID();

    /** Generates new 'Series Instance UID' and inserts it into the dataset.
     *  'SOP Instance UID' is not affected.
     */
    OFCondition generateNewSeriesUID();

    /** Generates new 'SOP Instance UID' and inserts it into the dataset.
     *  The related metaheader tag ('Media Storage SOP Instance UID') is
     *  deleted from metaheader, so that it gets created correctly, if the file
     *  is saved to disk.
     */
    OFCondition generateNewInstanceUID();

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


protected:

    /** modifies element to a specific value
     *  @param elem element, that should be changed
     *  @param value the value, the element should be changed to
     *  @return OFCondition, which returns an error code if an error occurs
     */
    OFCondition startModify(DcmElement *elem, const OFString &value);

    /** inserts tag into item with a specific value, overwrites existing tag
     *  @param item item, where tag is inserted
     *  @param search_key specifies tag to be inserted
     *  @param value value that should be inserted in item
     *  @return returns an error code as OFCondition, if an error occurs
     */
    OFCondition startInsert(DcmItem *item, DcmTagKey &search_key,
                            const OFString &value);

    /** If key is the tag for SOPInstanceUID or SOPClassUID, then this function
     *  removes the related MediaStorage UIDs from the metaheader. The
     *  metaheader is then updated automagically when the file is saved back to
     *  disk.
     *  @param key tag to examine
     */
    void deleteRelatedMetaheaderTag(const DcmTagKey &key);

    /** Checks whether group number is 0,1,2,3,5 or 7. Then an error is
     *  returned, because these groups are illegal or shouldn't be modified
     *  @param key tag, whose group should be examined
     *  @return OFCondition with OF_ok if group is ok, else OF_error and
     *          an error message are returned
     */
    OFCondition hasValidGroupNumber(const DcmTagKey &key);

    /** Returns true, if given tag is a private tag (odd group number)
     *  @param tag_key tag key, that should be tested
     *  @return OFTrue if tag is private, else OFFalse
     */
    OFBool isPrivateTag(const DcmTagKey &tag_key);

    /** Returns true, if given tag is a private reservation tag.
     *  Thats is the case, if tag has the form gggg,00ee (10<ee<FF)
     *  @param tag_key tag key, that should be tested
     *  @return OFTrue if tag is a private reservation tag, else OFFalse
     */
    OFBool isPrivateReservationTag(const DcmTagKey &tag_key);

    /** Returns true, if given tag is reserved in given item.
     *  The private creator string is returned, too.
     *  @param tag_key tag key, that should be checked
     *  @param item this item is searched for the reservation
     *  @param priv_creator OUT:private creator string of reservation, if found
     *  @return OFTrue if tag has a matching reservation, else OFFalse
     */
    OFBool hasPrivateReservationContext(const DcmTagKey &tag_key,
                                        DcmItem *item,
                                        OFString &priv_creator);

    /** Calculates from given private tag the reservation tag, that
     *  would make a reservation for this private tag
     *  @param tag_key tag key, whose reservation should be calculated
     *  @return the tag key, that would reserve given private tag
     */
    DcmTagKey calcPrivateReservationTag(const DcmTagKey &tag_key);

    /** Returns true, if given tag key can be found in dictionary
     *  @param search_key tag to lookup
     *  @return OFTrue if tag could be found, else OFFalse
     */
    OFBool isTagInDictionary(const DcmTagKey &search_key);

    /** The function handles three strings, that are directly printed
     *  after another. The whole message is then terminated by \n
     *  @param condition message is printed, if condition is true
     *  @param s1 first message string
     *  @param s2 second message string
     *  @param s2 third message string
     */
    void debugMsg(const OFBool &condition,
                  const OFString &s1,
                  const OFString &s2,
                  const OFString &s3);
private:

    ///name of file, that is loaded currently
    OFString act_file;

    ///will hold file to modify
    DcmFileFormat *dfile;

    ///will hold the dataset, that should be modified
    DcmDataset *dset;

    ///enable debug messages
    OFBool debug_option;

    /** private undefined assignment operator
     */
    MdfDatasetManager &operator=(const MdfDatasetManager &);

    /** private undefined copy constructor
     */
    MdfDatasetManager(const MdfDatasetManager &);

};

#endif //MDFDSMAN_H

/*
** CVS/RCS Log:
** $Log: mdfdsman.h,v $
** Revision 1.17  2006-11-23 15:32:58  onken
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
