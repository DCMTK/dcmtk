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
 *  Purpose: Class for modifying DICOM-Files from comandline
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2003-10-01 14:04:03 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfconen.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef MDFCONEN_H
#define MDFCONEN_H

#include "osconfig.h"   // make sure OS specific configuration is included first
#include "mdfdsman.h"
#include "oftypes.h"
#include "ofconapp.h"
#include "oflist.h"
#include "ofcond.h"
#include "dctagkey.h"

/** This class encapsulates data structures and operations for modifying
 *  Dicom-files from the commandline
 */
class MdfConsoleEngine
{
public:

    /** Constructor
     *  @param argc Number of commandline-arguments
     *  @param argv Array holding the Commandline-arguments
     *  @param appl_name Name of calling applic., that instantiates this class
     */
    MdfConsoleEngine(int argc, char *argv[],
                     const char* appl_name);

    /** Destructor
     */
    ~MdfConsoleEngine();

    /** This function looks at commandline options and decides what to do.
     *  It evaluates option-values from commandline and prepares them for
     *  starting the corresponding private functions that
     *  @return An Integer value denoting how many errors occured while
     *          providing the service
     */
    int startProvidingService();

protected:

    ///helper class for console-applications
    OFConsoleApplication *app;
    ///helper class for command-line-parsing
    OFCommandLine *cmd;
    ///name of calling application
    OFString *app_name;
    ///ds_man holds DatasetManager, that is used for modify operations
    MdfDataSetManager *ds_man;
    ///set, if -m option was chosen from command-line
    OFBool modify_tag_option;
    ///set, if -mi option was chosen from command-line
    OFBool modify_item_tag_option;
    ///set, if -ma option was chosen from command-line
    OFBool modify_all_tags_option;
    ///set, if -ii option was chosen from command-line
    OFBool insert_item_tag_option;
    ///set, if -b option was chosen from command-line
    OFBool batch_mode_option;
    ///set, if -a option was chosen from command-line
    OFBool ask_option;
    ///set, if -i option was chosen from command-line
    OFBool insert_tag_option;
    ///set, if -e option was chosen from command-line
    OFBool erase_tag_option;
    ///set, if -ei option was chosen from command-line
    OFBool erase_item_tag_option;
    ///set, if -ea option was chosen from command-line
    OFBool erase_all_tags_option;
    ///verbose mode
    OFBool verbose_option;

    ///most options need a value to work
    const char *option_value;
    ///list of files to be modified
    OFList<const char*> *files;
    ///number of "real" options like -m, -i exclusive -v and -d
    int option_count;

    /** This function reads the next line (until '\\n' or '\r') from the open
     *   filestream fp and returns this line (excluding '\n' and '\r') in str.
     *@param  fp Filepointer (must have been previously opened for reading with fopen())
     *@param  str Line which was read, excluding '\\n' and '\r'.
     *@return case a string could be read and is returned, this function
     *        will always return the number of characters in that string.
     *        In case no string could be read (i.e. if an error occurred or
     *        EOF was encountered), -1 is returned.
     */
    int readLine( FILE *fp, OFString &str );

    /** This function is responsible for delegating all actions on simple
     *  tags, that does not contain a "path" to be searched for.
     *  It uses the other member-variables to decide which action to perform
     *  @param search_key tag to be worked on
     *  @param tag_value new value of tag
     *  @return An Integer value denoting how many errors occured while
     *           providing the service
     */
    int startTagAction(DcmTagKey search_key, char *tag_value);

    /** This function is responsible for delegating all actions on simple
     *  tags, that does not contain a "path" to be searched for.
     *  It uses the other member-variables to decide which action to perform
     *  @param tag_path path to tag, that should be worked on
     *  @param tag_value new value of tag
     *  @return An Integer value denoting how many errors occured while
     *           providing the service
     */
    int startItemTagAction(char *tag_path, char *tag_value);

    /** This function organizes data for starting a tag-modification and then
     *  starts specific modification.
     *  @return An Integer value denoting how many errors occured while
     *  modifying
     */
    int delegateTagAction();

    /** This function organizes data for starting a item-tag-modification and
     *  then starts specific modification.
     *  @return An Integer value denoting how many errors occured while
     *  modifying
     */
    int delegateItemTagAction();

    /** This function organizes reads the lines from the batchfile and performs
     *  specific modify-operations depending on the options found there
     *  @return An Integer value denoting how many errors occured while
     *  modifying
     */
    int delegateBatchMode();

private:

    /** private undefined assignment operator
     */
    MdfConsoleEngine &operator=(const MdfConsoleEngine &);

    /** private undefined copy constructor
     */
    MdfConsoleEngine(const MdfConsoleEngine &);

};


#endif //MDFCONEN_H

/*
** CVS/RCS Log:
** $Log: mdfconen.h,v $
** Revision 1.4  2003-10-01 14:04:03  onken
** Corrected doxygen-information in headerfiles
**
** Revision 1.3  2003/09/19 12:43:54  onken
** major bug fixes, corrections for "dcmtk-coding-style", better error-handling
**
** Revision 1.2  2003/07/09 12:13:13  meichel
** Included dcmodify in MSVC build system, updated headers
**
** Revision 1.1  2003/06/26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
