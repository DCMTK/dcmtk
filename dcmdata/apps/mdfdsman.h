/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Update Date:      $Date: 2003-06-26 09:17:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfdsman.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef MDFDSMAN_H
#define MDFDSMAN_H

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
        /** Constructor
        *@param app_name [in] Name of calling applic., that instantiates this class
        */
        MdfDataSetManager(const char *app_name);

        /** Destructor
        */
        ~MdfDataSetManager();

        /** Loads a file into Datasetmanager (this class)
        *@param file_name [in] file to be loaded
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition loadFile(const char *file_name);

        /** Modifies a tag to a specific value, works only at 1. level!
        * It does not insert non-existing tag
        *@param tag_key [in] holds tag-key to be modified
        *@param value [in] denotes new value of tag
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition modifyTag(DcmTagKey tag_key,
                              const char *value);

        /** Modifies a tag to a specific value, works only at 1. level!
        * Inserts new tag, if tag does not exist so far
        *@param tag_key [in] holds tag-key to be modified
        *@param value [in] denotes new value of tag
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition modifyOrInsertTag(DcmTagKey tag_key,
                                      const char *value);

        /** Deletes tag in dataset
        *@param tag_key [in] holds tag-key to be modified
        *@param all_tags [in] If true, tag is deletet at all levels of dataset,
        * else only 1. level is accessed
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition deleteTag(DcmTagKey tag_key,
                              OFBool all_tags);

        /** Deletes tag stored in a sequence/item
        *@param tag_path [in] path to item
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition deleteItemTag(char *tag_path);
        
        /** Modifies a tag in an item to a specific value
        *@param tag_path [in] holds path to item-tag. The path should look like this:
        * gggg,eeee[0].gggg,eeee[3].gggg,eeee
        *@param value [in] denotes new value of tag
        *@param only_modify [in] if true, only existing tags are modified
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition modifyOrInsertItemTag(char *tag_path, 
                                          const char *value, 
                                          OFBool only_modify);

        /** Modifies all matching tags in dataset to a new value
        *@param tag_key [in] denotes, which tag to modify
        *@param value [in] denotes new value of tag
        *@param count [out] holds the number of tags, that were affected from this
        * function
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition modifyAllTags(DcmTagKey tag_key,
                                  const char *value,
                                  int &count);

        /** Saves current dataset back to a file
        *@param file_name [in] file is saved to this filename
        *@return returns EC_normal if everything is ok, else an error
        */
        OFCondition saveFile(const char *file_name);
        
    protected:
        ///file to modify
        DcmFileFormat *dfile;   
        ///will hold dset, we want to modify
        DcmDataset *dset;
        ///app thats using the DataManager
        char *app_name;
};

#endif //MDFDSMAN_H

/*
** CVS/RCS Log:
** $Log: mdfdsman.h,v $
** Revision 1.1  2003-06-26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
