/*
 *
 *  Copyright (C) 2003, OFFIS
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
 *  Purpose: Class for modifying DICOM-Files and Datasets
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2004-05-14 12:08:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfdsman.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef MDFDSMAN_H
#define MDFDSMAN_H

#include "osconfig.h"   // make sure OS specific configuration is included first
#include "dctagkey.h"
#include "dcfilefo.h"
#include "ofcond.h"
#include "oflist.h"
#include "dcvrat.h"
//#include "dcdebug.h"

/** This class encapsulates data structures and operations for modifying
 *  Dicom-files. Therefore it allows the process of load->modify->save to
 *  provide this service.
 */
class MdfDatasetManager
{
public:
    /** Constructor, initializes member-variables
     *  @param debug enables/disables debug-messages (off per default)
     */
    MdfDatasetManager(const OFBool debug);

    /** Destructor
     */
    ~MdfDatasetManager();

    /** Loads a file into Datasetmanager
     *  @param file_name file to be loaded
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition loadFile(const char *file_name);

    /** Modifies/Inserts a tag with a specific value
     *  @param tag_path holds complete path to tag
     *  @param value denotes new value of tag
     *  @param only_modify if true, only existing tags are processed. If false,
     *                     not existing tag is inserted
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyOrInsertTag(OFString tag_path,
                                  const OFString &value,
                                  const OFBool &only_modify);

    /** Modifies all matching tags in Dataset to a new value
     *  @param tag_path denotes, which tag to modify
     *  @param value denotes new value of tag
     *  @param count returns holds the number of tags, that were affected
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyAllTags(OFString tag_path,
                              const OFString &value,
                              int &count);

    /** Deletes tag in Dataset
     *  @param tag_path holds complete path to tag
     *  @param all_tags If true, tag is deleted at all levels of Dataset,
     *                  else only 1. level is accessed
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition deleteTag(OFString tag_path,
                          const OFBool &all_tags);

    /** Saves current Dataset back to a file. Caution: After saving
     *  MdfDatasetManager keeps working on old filename.
     *  @param file filename to save to
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition saveFile(const char *file);

    /** Saves current Dataset back to file using original filename
     *  @return returns EC_normal if everything is ok, else an error
     */
    OFCondition saveFile();

    /** Returns the Dataset, that this MdfDatasetManager handles.
     *  You should use the returned object with care to avoid
     *  sideeffects with other class-methods, that modify this object, too.
     *  @return returns the Dataset, this MdfDatasetManager manages and NULL, if
     *          no Dataset is loaded
     */
    DcmDataset* getDataset();


    /** Returns the DcmFileFormat, that this MdfDatasetManager handles.
     *  You should use the returned object with care to avoid
     *  sideeffects with other class-methods, that modify this object, too.
     *  @return returns the DcmFileFormat, this MdfDatasetManager manages and
     *          NULL, if no file is loaded
     */
    DcmFileFormat* getFileFormat();

    /** Returns filename of the file, that's loaded actually.
     *  @return returns filename and "" if no file is loaded.
     */
    OFString getFilename();

    /** The function handles three strings, that are directly printed
     *  after another to CERR. The whole message is then terminated by \n
     *  @param s1 first message string
     *  @param s2 second message string
     *  @param s2 third message string
     */
    static void debugMsg(const OFString &s1,
                         const OFString &s2,
                         const OFString &s3);

protected:

    /** modifies element to a specific value
     *  @param elem element, that should be changed
     *  @param value the value, the element should be changed to
     *  @return OFCondition, which returns an error-code if an error occurs
     */
    OFCondition startModify(DcmElement *elem, const OFString &value);

    /** inserts tag into item with a specific value
     *  @param item - item, where tag is inserted
     *  @param search_key specifies tag to be inserted
     *  @param value value that should be inserted in item
     *  @return returns an error-code as OFCondition, if an error occurs
     */
    OFCondition startInsert(DcmItem *item, DcmTagKey &search_key,
                            const OFString &value);

    ///name of file, that is loaded actually
    OFString act_file;
    ///will hold file to modify
    DcmFileFormat *dfile;
    ///will hold dset, we want to modify
    DcmDataset *dset;
    ///enable debug-messages
    OFBool debug_option;

private:

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
** Revision 1.10  2004-05-14 12:08:36  onken
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
