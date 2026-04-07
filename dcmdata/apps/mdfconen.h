/*
 *
 *  Copyright (C) 2003-2026, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first

#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/ofstd/ofcond.h"

// forward declarations
class MdfDatasetManager;
class OFConsoleApplication;
class OFCommandLine;

/** class reflecting a modify operation (called Job in this context)
 */
class MdfJob
{

public:
    OFString option;
    OFString path;
    OFString value;

    MdfJob();

    ~MdfJob();

    MdfJob(const MdfJob& other);

    /** Comparison operator between Jobs
     * @param j: the other job to compare to
     * @return true if j is equal to this, false otherwise
     */
    OFBool operator==(const MdfJob& j) const;

    /** Assignment operator
     * @param j assign j to this
     * @return reference to this
     */
    MdfJob& operator=(const MdfJob& j);
};

/** class reflecting a condition check
 */
class MdfCondition
{

public:
    /// The option of the comparison, right now always "iff" for condition checks
    OFString option;
    /// The path for the condition check
    OFString path;
    /// Operator for the condition check.
    /// Possible checking operations are:
    /// - Existence check (coded as "E")
    /// - Non-Existence check (coded as "!")
    /// - Equality check (coded as "=")
    /// - Non-Equality check (coded as "!=")
    /// - Lesser and Lesser-equal check (coded as "<" and "<=")
    /// - Greater than and Greater-equal check (coded as ">" and ">=")
    OFString operation;
    /// Value to compare to, not existing for (non-)existence checks.
    OFString value;

    /** Constructor, initializes all members with empty strings
     */
    MdfCondition();

    /** Copy Constructor
     * @param other initialise this condition with values from the other condition
     */
    MdfCondition(const MdfCondition& other);

    /** Comparison operator between Conditions
     * @param c: the other condition to compare to
     * @return true if c is equal to this, false otherwise
     */
    OFBool operator==(const MdfCondition& c) const;

    /** Assignment operator
     * @param c assign c to this condition
     * @return reference to this condition
     */
    MdfCondition& operator=(const MdfCondition& c);

    /** Returns whether the condition is a (Non-)Existence check
     * @param group indicates whether we want to test for "E" only (false) or both, "E" or "!" (true)
     * @return true if condition represents the requested check
     */
    OFBool isExistenceCheck(OFBool group = OFTrue) const;

    /** Returns whether the condition is a (Non-)Equality check
     * @param group indicates whether we want to test for "=" only (false) or both, "=" and "!=" (true)
     * @return true if condition represents the requested check
     */
    OFBool isEqualityCheck(OFBool group = OFTrue) const;

    /** Returns whether the condition is a "Lesser than"-based check
     * @param equal indicates whether to test for lesser or lesser equal
     * @return true if condition represents the requested check
     */
    OFBool isLesserCheck(OFBool equal = OFFalse) const;
    /** Returns whether the condition is a "Greater than"-based check
     * @param equal indicates whether to test for greater or greater equal
     * @return true if condition represents the requested check
     */
    OFBool isGreaterCheck(OFBool equal = OFFalse) const;

    /** Returns a string representation of the condition
     *  @return string representation of the condition
     */
    OFString toString() const;
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
    MdfConsoleEngine(int argc, char* argv[], const char* appl_name);

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

    /** Parses commandline options into corresponding file- job, and condition lists
     *  and enables debug/verbose mode. The list of jobs and conditions is built in
     *  order as they appear on the command line.
     */
    void parseCommandLine();

    /** Checks whether given job expects more values on commandline, e. g. a
     *  a tag path or any kind of value.
     *  @param job [in] The job option to check (e. g. "i", "m", "ma"...)
     *  @return OFTrue, if a value is expected.
     */
    static OFBool jobOptionExpectsParameters(const OFString& job);

    /** This function splits a modify option (inclusive value) as
     *  found on commandline into to parts (path and value)
     *  e.g. "(0010,0010)=value" into path "(0010,0010)" and "value"
     *  @param whole string to be split
     *  @param path returns part containing the path
     *  @param value returns part containing the value (if there is one)
     */
    static void splitPathAndValue(const OFString& whole, OFString& path, OFString& value);

    /** This function splits a condition option as found on the commandline into parts
     *  (path, operation and value),
     *   e.g. splits "(0010,0010)=value" into path "(0010,0010)", "=" and "value".
     *  Possible operations are:
     *  -iff path=value  (equality operation, coded as '=')
     *  -iff path!=value (inequality operation, coded as '!=')
     *  -iff path<value  (lesser than operation, coded as '<')
     *  -iff path<=value (lesser or equal operation, coded as '<=')
     *  -iff path>value  (greater than operation, coded as '>')
     *  -iff path>=value (greater or equal operation, coded as '>=')
     *  -iff path        (exists operation, coded as 'E')
     *  -iff !path       (does not exist operation, coded as '!')
     *  @param whole string to be split
     *  @param path returns part containing the path
     *  @param operation returns part containing the operation
     *  @param value returns part containing the value (if there is one)
     */
    static void splitConditionPathAndValue(const OFString& whole, OFString& path, OFString& operation, OFString& value);

    /** Executes given modify job
     *  @param job job to be executed
     *  @param filename name of the file to be processed (optional)
     *  @return returns 0 if no error occurred, else the number of errors
     */
    int executeJob(const MdfJob& job, const char* filename = NULL);

    /** Checks given condition holds true for the current file
     *  @param condition condition to be checked
     *  @return EC_Normal, if the condition holds true, error otherwise
     */
    OFCondition checkCondition(const MdfCondition& condition);

    /** Backup and load file into internal MdfDatasetManager
     *  @param filename name of file to load
     *  @return OFCondition, whether loading/backing up was successful including
     *          error description
     */
    OFCondition loadFile(const char* filename);

    /** Backup given file from file to file.bak
     *  @param file_name filename of file, that should be backuped
     *  @return OFCondition, whether backup was successful or not
     */
    OFCondition backupFile(const char* file_name);

    /** Restore given file from file.bak to original (without .bak)
     *  @param filename restore "filename".bak to original without .bak
     *  @return OFCondition, whether restoring was successful
     */
    OFCondition restoreFile(const char* filename);

private:
    /// helper class for console applications
    OFConsoleApplication* app;

    /// helper class for commandline parsing
    OFCommandLine* cmd;

    /// dataset manager that is used for modify operations
    MdfDatasetManager* ds_man;

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

    /// list of conditions to be evaluated
    OFList<MdfCondition> conditions;

    /// list of jobs to be executed
    OFList<MdfJob>* jobs;

    /// list of files to be modified
    OFList<OFString>* files;

    /** private undefined assignment operator
     *  @param rhs right-hand side of assignment
     *  @return reference to this
     */
    MdfConsoleEngine& operator=(const MdfConsoleEngine&);

    /** private undefined copy constructor
     */
    MdfConsoleEngine(const MdfConsoleEngine&);
};

#endif // MDFCONEN_H
