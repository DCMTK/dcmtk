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
 *  Update Date:      $Date: 2003-10-01 14:04:03 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfdsman.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
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
#include "dcdebug.h"

/** This class encapsulates data structures and operations for modifying
 *  Dicom-files. Therefore it allows the process of load->modify->save to
 *  provide this service.
 */
class MdfDataSetManager
{
public:
    /** Constructor, just initializes member-variables
     */
    MdfDataSetManager();

    /** Destructor
     */
    ~MdfDataSetManager();

    /** Loads a file into Datasetmanager (this class)
     *@param file_name file to be loaded
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition loadFile(const char *file_name);

    /** Modifies a tag to a specific value, works only at 1. level!
     * It does not insert non-existing tag
     *@param tag_key holds tag-key to be modified
     *@param value denotes new value of tag
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyTag(DcmTagKey tag_key,
                          const char *value);

    /** Modifies a tag to a specific value, works only at 1. level!
     * Inserts new tag, if tag does not exist so far
     *@param tag_key holds tag-key to be modified
     *@param value denotes new value of tag
     *@param only_modify if true, only existing tags are processed.
     * if false, a tag with specified value is inserted
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyOrInsertTag(DcmTagKey tag_key,
                                  const char *value,
                                  OFBool only_modify);

    /** Deletes tag in dataset
     *@param tag_key holds tag-key to be modified
     *@param all_tags If true, tag is deletet at all levels of dataset,
     * else only 1. level is accessed
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition deleteTag(DcmTagKey tag_key,
                          OFBool all_tags);

    /** Deletes tag stored in a sequence/item
     *@param tag_path path to item
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition deleteItemTag(char *tag_path);

    /** Modifies a tag in an item to a specific value
     *@param tag_path holds path to item-tag. The path should look like this:
     * gggg,eeee[0].gggg,eeee[3].gggg,eeee
     *@param value denotes new value of tag
     *@param only_modify if true, only existing tags are modified. If
     * its false, a non existing tag is inserted with specified value

     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyOrInsertItemTag(char *tag_path,
                                      const char *value,
                                      OFBool only_modify);

    /** Modifies all matching tags in dataset to a new value
     *@param tag_key denotes, which tag to modify
     *@param value denotes new value of tag
     *@param count holds the number of tags, that were affected from this
     * function
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition modifyAllTags(DcmTagKey tag_key,
                              const char *value,
                              int &count);

    /** Saves current dataset back to a file
     *@param file file is saved to this filename
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition saveFile(const char *file);

    /** get next Object from internal dataset. if intoSub true, scan
     * complete hierarchy, false scan only elements direct in this
     * item (not deeper).
     *@param result_stack the result of nextObject is stored here
     *@param intoSub dive into sub
     *@return returns EC_normal if everything is ok, else an error
     */
    OFCondition getElements(DcmStack &result_stack, OFBool intoSub=OFTrue);

protected:

    /** modifies element a specific value
     *@param elem element, that should be changed
     *@param value this is the value, the element should be changed to
     *@return returns an error-code as OFCondition, if an error occurs
     */
    OFCondition startModify(DcmElement *elem, const char *value);

    /** inserts tag into item with a specific value
     *@param item - item, where tag is inserted
     *@param search_key specifies tag to be inserted
     *@param value value that should be inserted in item
     *@return returns an error-code as OFCondition, if an error occurs
     */
    OFCondition startInsert(DcmItem *item, const DcmTagKey &search_key,
                            const char *value);

    /** walks along a tag path like gggg,eeee[0].gggg,eeee=gggg,eeee
     *@param result_item pointer to item, thats the last in path
     *@param tag_path complete path to tag, after returning,
     *                  tag_path only consists of the last tag! This is the
     *                  "destination tag"!
     *@return returns OFCondition containing an error-code if error occurs
     */
    OFCondition getItemFromPath(DcmItem *&result_item, char *&tag_path);

    ///file to modify
    DcmFileFormat *dfile;
    ///will hold dset, we want to modify
    DcmDataset *dset;

private:

    /** private undefined assignment operator
     */
    MdfDataSetManager &operator=(const MdfDataSetManager &);

    /** private undefined copy constructor
     */
    MdfDataSetManager(const MdfDataSetManager &);

};

#endif //MDFDSMAN_H

/*
** CVS/RCS Log:
** $Log: mdfdsman.h,v $
** Revision 1.4  2003-10-01 14:04:03  onken
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
