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
 *  Update Date:      $Date: 2003-12-10 16:19:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/mdfconen.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
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
     *  @param appl_name Name of calling application, that instantiates
     *                   this class
     */
    MdfConsoleEngine(int argc, char *argv[],
                     const char* appl_name);

    /** Destructor
     */
    ~MdfConsoleEngine();

    /** This function looks at commandline options and decides what to do.
     *  It evaluates option-values from commandline and prepares them for
     *  starting the corresponding private functions.
     *  @return Returns 0 if successful, another value if errors occurreds
     */
    int startProvidingService();

protected:
    ///helper class for console-applications
    OFConsoleApplication *app;
    ///helper class for command-line-parsing
    OFCommandLine *cmd;
    ///ds_man holds DatasetManager, that is used for modify operations
    MdfDatasetManager *ds_man;
    ///verbose mode
    OFBool verbose_option;
    ///debug mode
    OFBool debug_option;
    ///ignore errors option
    OFBool ignore_errors_option;
    ///struct reflecting a modify operation (called Job in this context)
    struct Job {
            OFString option;
            OFString path;
            OFString value;
    };
    ///list of jobs to be executed
    OFList<Job> *jobs;
    ///list of files to be modified
    OFList<const char*> *files;

    /** This function splits a modify-option (inclusive value) as
     *  found on commandline into to parts (path and value)
     *  e.g. "(0010,0010)=value" into path "(0010,0010)" and "value"
     *  @param string to be splitted
     *  @param path returns part containing the path
     *  @param value returns part containing the value(if theres one)
     */
    void splitPathAndValue(const OFString &whole,
                                 OFString &path,
                                 OFString &value);

    /** Executes given modify-job
     *  @param job job to be executed
     *  @return returns 0 if no error occured, else the number of errors
     */
    int executeJob(const Job &job);

    /** Parses commandline options into corresponding file- and job-lists and
     *  enables debug/verbose mode. The joblist is built in order of modify
     *  options on commandline
     */
    void parseCommandLine();

    /** Backup and load file into internal MdfDatasetManager
     *  @param filename name of file to load
     *  @return OFCondition, whether loading/backuping was successful including
     *          error description
     */
    OFCondition loadFile(const char* filename);

    /** Backup given file from file to file.bak
     *  @param file_name filename of file, that should be backuped
     *  @return OFCondition, whether backup was successful or not
     */
    OFCondition backupFile(const char *file_name);

    /** Restore given file from file.bak to original (without .bak)
     *  @param restore "filename".bak to original without .bak
     *  @return OFCondition, whether restoring was successful
     */
    OFCondition restoreFile(const char *filename);

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
** Revision 1.7  2003-12-10 16:19:20  onken
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
** Revision 1.5  2003/10/13 14:51:49  onken
** improved backup-strategy
**
** Revision 1.4  2003/10/01 14:04:03  onken
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
