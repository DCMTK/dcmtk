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
 *  Purpose: Class for modifying DICOM files from comandline
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:52 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef MDFCONEN_H
#define MDFCONEN_H

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcxfer.h"


// forward declarations
class MdfDatasetManager;
class OFConsoleApplication;
class OFCommandLine;


/** class reflecting a modify operation (called Job in this context)
 */
class MdfJob {

public :

    OFString option;
    OFString path;
    OFString value;

    /** Comparison operator between Jobs
     */
    OFBool operator==(const MdfJob &j) const;

private :

    /** private undefined copy constructor
     */
    MdfJob &operator=(const MdfJob &j);
};


/** This class encapsulates data structures and operations for modifying
 *  Dicom files from the commandline
 */
class MdfConsoleEngine
{
public:

    /** Constructor
     *  @param argc Number of commandline arguments
     *  @param argv Array holding the commandline arguments
     *  @param appl_name Name of calling application, that instantiates
     *                   this class
     */
    MdfConsoleEngine(int argc,
                     char *argv[],
                     const char *appl_name);

    /** Destructor
     */
    ~MdfConsoleEngine();

    /** This function looks at commandline options and decides what to do.
     *  It evaluates option values from commandline and prepares them for
     *  starting the corresponding private functions.
     *  @return Returns 0 if successful, another value if errors occurreds
     */
    int startProvidingService();

protected:

    /** Checks for non-job commandline options like --debug etc. and
     *  sets corresponding internal flags
     */
    void parseNonJobOptions();

    /** Parses commandline options into corresponding file- and job lists and
     *  enables debug/verbose mode. The joblist is built in order of modify
     *  options on commandline
     */
    void parseCommandLine();

    /** Checks whether given job expects more values on commandline, e. g. a
     *  a tag path or any kind of value.
     *  @param job [in] The job option to check (e. g. "i", "m", "ma"...)
     *  @return OFTrue, if a value is expected.
     */
    static OFBool jobOptionExpectsParameters(const OFString &job);

    /** This function splits a modify option (inclusive value) as
     *  found on commandline into to parts (path and value)
     *  e.g. "(0010,0010)=value" into path "(0010,0010)" and "value"
     *  @param whole string to be splitted
     *  @param path returns part containing the path
     *  @param value returns part containing the value(if theres one)
     */
    static void splitPathAndValue(const OFString &whole,
                                  OFString &path,
                                  OFString &value);

    /** Executes given modify job
     *  @param job job to be executed
     *  @param filename name of the file to be processed (optional)
     *  @return returns 0 if no error occured, else the number of errors
     */
    int executeJob(const MdfJob &job,
                   const char *filename = NULL);

    /** Backup and load file into internal MdfDatasetManager
     *  @param filename name of file to load
     *  @return OFCondition, whether loading/backuping was successful including
     *          error description
     */
    OFCondition loadFile(const char *filename);

    /** Backup given file from file to file.bak
     *  @param file_name filename of file, that should be backuped
     *  @return OFCondition, whether backup was successful or not
     */
    OFCondition backupFile(const char *file_name);

    /** Restore given file from file.bak to original (without .bak)
     *  @param filename restore "filename".bak to original without .bak
     *  @return OFCondition, whether restoring was successful
     */
    OFCondition restoreFile(const char *filename);

private:

    /// helper class for console applications
    OFConsoleApplication *app;

    /// helper class for commandline parsing
    OFCommandLine *cmd;

    /// dataset manager that is used for modify operations
    MdfDatasetManager *ds_man;

    /// ignore errors option
    OFBool ignore_errors_option;

    /// if false, metaheader UIDs are not updated when related dataset UIDs change
    OFBool update_metaheader_uids_option;

    /// if true, no backup is made before modifying a file
    OFBool no_backup_option;

    /// read file with or without metaheader
    E_FileReadMode read_mode_option;

    /// denotes the expected transfersyntax
    E_TransferSyntax input_xfer_option;

    /// decides whether to with/without metaheader
    OFBool output_dataset_option;

    /// denotes the transfer syntax that should be written
    E_TransferSyntax output_xfer_option;

    /// option for group length recalcing
    E_GrpLenEncoding glenc_option;

    /// write explicit or implicit length encoding
    E_EncodingType enctype_option;

    /// padding output
    E_PaddingEncoding padenc_option;

    /// internal padding variables
    OFCmdUnsignedInt filepad_option;
    OFCmdUnsignedInt itempad_option;

    /// if true, 'tag not found' errors are treated as being successful for
    /// modify and erase operations
    OFBool ignore_missing_tags_option;

    /// If true, it is not checked whether there is a corresponding private
    /// reservation during insertion of private tags
    OFBool no_reservation_checks;

    /// If enabled, any value modifications of UN leaf elements are not
    /// executed
    OFBool ignore_un_modifies;

    /// list of jobs to be executed
    OFList<MdfJob> *jobs;

    /// list of files to be modified
    OFList<OFString> *files;

    /** private undefined assignment operator
     */
    MdfConsoleEngine &operator=(const MdfConsoleEngine &);

    /** private undefined copy constructor
     */
    MdfConsoleEngine(const MdfConsoleEngine &);
};

#endif // MDFCONEN_H


/*
** CVS/RCS Log:
** $Log: mdfconen.h,v $
** Revision 1.19  2010-10-14 13:17:52  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.18  2010-05-20 15:45:21  joergr
** Removed some unnecessary include directives.
**
** Revision 1.17  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.16  2009-01-15 16:11:55  onken
** Reworked dcmodify to work with the new DcmPath classes for supporting
** wildcard paths and automatic insertion of missing attributes and items.
** Added options for private tag handling and modification of UN values and
** for ignoring errors resulting from missing tags during modify and erase
** operations. Further cleanups.
**
** Revision 1.15  2008-06-23 13:39:16  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.14  2006/12/06 09:31:49  onken
** Added "--no-backup" option to prevent dcmodify from creating backup files
**
** Revision 1.13  2006/11/23 15:32:58  onken
** Made member variables private (before: protected)
**
** Revision 1.12  2005/12/08 15:46:50  meichel
** Updated Makefiles to correctly install header files
**
** Revision 1.11  2005/12/02 09:19:26  joergr
** Added new command line option that checks whether a given file starts with a
** valid DICOM meta header.
**
** Revision 1.10  2004/11/05 17:17:24  onken
** Added input and output options for dcmodify. minor code enhancements.
**
** Revision 1.9  2004/10/22 16:53:26  onken
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
** Revision 1.8  2004/04/19 14:45:07  onken
** Restructured code to avoid default parameter values for "complex types" like
** OFString. Required for Sun CC 2.0.1.
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
