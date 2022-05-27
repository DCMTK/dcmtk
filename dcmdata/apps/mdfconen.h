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
 *  Purpose: Class for modifying DICOM files from commandline
 *
 */

#ifndef MDFCONEN_H
#define MDFCONEN_H

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/ofstd/ofcmdln.h"


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

    MdfJob() : option(), path(), value() { }

    MdfJob(const MdfJob& other);

    /** Comparison operator between Jobs
     */
    OFBool operator==(const MdfJob &j) const;

    /** Assignment operator
     */
    MdfJob &operator=(const MdfJob &j);
};


/** This class encapsulates data structures and operations for modifying
 *  DICOM files from the commandline
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
     *  @return Returns 0 if successful, another value if errors occurred
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
     *  @param whole string to be split
     *  @param path returns part containing the path
     *  @param value returns part containing the value (if there is one)
     */
    static void splitPathAndValue(const OFString &whole,
                                  OFString &path,
                                  OFString &value);

    /** Executes given modify job
     *  @param job job to be executed
     *  @param filename name of the file to be processed (optional)
     *  @return returns 0 if no error occurred, else the number of errors
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

    /// denotes the expected transfer syntax
    E_TransferSyntax input_xfer_option;

    /// decides whether to with/without metaheader
    OFBool output_dataset_option;

    /// denotes the transfer syntax that should be written
    E_TransferSyntax output_xfer_option;

    /// option for group length recalculation
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

    /// If enabled, a new dataset is created in memory if a file is not existing.
    OFBool create_if_necessary;

    /// Used to remember, whether a file was newly created.
    OFBool was_created;

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
