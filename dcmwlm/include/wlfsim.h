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
*  Module:  dcmwlm
*
*  Author:  Thomas Wilkens
*
*  Purpose: Class for managing file system interaction.
*
*  Last Update:      $Author: wilkens $
*  Update Date:      $Date: 2003-07-02 09:17:55 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wlfsim.h,v $
*  CVS/RCS Revision: $Revision: 1.5 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

#ifndef WlmFileSystemInteractionManager_h
#define WlmFileSystemInteractionManager_h

#include "osconfig.h"

template <class T> class OFOrderedSet;
class DcmTagKey;
class OFConsole;
class OFCondition;

/** This class encapsulates data structures and operations for managing
 *  data base interaction in the framework of the DICOM basic worklist
 *  management service.
 */
class WlmFileSystemInteractionManager
{
  private:
      /** Privately defined copy constructor.
       *  @param old Object which shall be copied.
       */
    WlmFileSystemInteractionManager(const WlmFileSystemInteractionManager &old);

      /** Privately defined assignment operator.
       *  @param obj Object which shall be copied.
       */
    WlmFileSystemInteractionManager &operator=(const WlmFileSystemInteractionManager &obj);

  protected:
    /// indicates if the application is run in verbose mode
    OFBool verboseMode;
    /// indicates if the application is run in debug mode
    OFBool debugMode;
    /// stream logging information will be dumped to
    OFConsole *logStream;
    /// path to database files
    char *dfPath;
    /// called AE title
    char *calledApplicationEntityTitle;
    /// array of matching records
    DcmDataset **matchingRecords;
    /// number of array fields
    unsigned long numOfMatchingRecords;

      /** This function dumps the given information on a stream.
       *  Used for dumping information in normal, debug and verbose mode.
       *  @param message The message to dump.
       */
    void DumpMessage( const char *message );

      /** This function determines all worklist files in the directory specified by
       *  dfPath and calledApplicationEntityTitle, and returns the complete path and
       *  filename information in an array of strings.
       *  @param worklistFiles Set of strings, each specifying path and filename to one worklist file.
       */
    void DetermineWorklistFiles( OFOrderedSet<OFString> &worklistFiles );

      /** This function returns OFTrue if the given filename refers to a worklist file,
       *  i.e. has an extension of ".wl".
       *  @param fname The name of the file.
       *  @return OFTrue in case the given filename refers to a worklist file, OFFalse otherwise.
       */
    OFBool IsWorklistFile( const char *fname );

      /** This function returns OFTrue, if the matching key attribute values in the
       *  dataset match the matching key attribute values in the search mask.
       *  @param dataset    The dataset which shall be checked.
       *  @param searchMask The search mask.
       *  @return OFTrue in case the dataset matches the search mask in the matching key attribute values, OFFalse otherwise.
       */
    OFBool DatasetMatchesSearchMask( DcmDataset *dataset, DcmDataset *searchMask );

      /** This function determines the values of the matching key attributes in the given dataset.
       *  @param dataset Dataset from which the values shall be extracted.
       *  @param matchingKeyAttrValues Contains in the end the values of the matching key
       *         attributes in the search mask. Is an array of pointers.
       */
    void DetermineMatchingKeyAttributeValues( DcmDataset *dataset, const char **&matchingKeyAttrValues );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute scheduled station AE title match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ScheduledStationAETitlesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attributes scheduled procedure step start date and scheduled procedure step
       *  start time match; otherwise OFFalse will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; might be NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; might be NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ScheduledProcedureStepStartDateTimesMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute modality match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ModalitiesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute scheduled performing physician's names match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ScheduledPerformingPhysiciansNamesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute patient's names match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool PatientsNamesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute patient id match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool PatientIdsMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function performs a date time range match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateTimeRangeMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );

      /** This function performs a date range match and returns OFTrue if the dataset's and
       *  the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateRangeMatch( const char *datasetDateValue, const char *searchMaskDateValue );

      /** This function performs a time range match and returns OFTrue if the dataset's and
       *  the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool TimeRangeMatch( const char *datasetTimeValue, const char *searchMaskTimeValue );

      /** This function performs a date time single value match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateTimeSingleValueMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );

      /** This function performs a date single value match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateSingleValueMatch( const char *datasetDateValue, const char *searchMaskDateValue );

      /** This function performs a time single value match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; never NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool TimeSingleValueMatch( const char *datasetTimeValue, const char *searchMaskTimeValue );

      /** This function returns OFTrue if the dataset's and the search mask's values
       *  match while performing a case sensitive single value match; otherwise OFFalse
       *  will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; never NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool CaseSensitiveSingleValueMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values
       *  match while performing a wildcard match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; never NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool WildcardMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function is called, if the search pattern contains a star symbol. It determines
       *  if dv (the dataset's value) still matches sv (the search mask's value). This function
       *  takes the star symbol in sv into account. (Note that the pattern value might contain
       *  more wild card symbols.) The function will return OFTrue if there is a match; if there
       *  is not a match it will return OFFalse.
       *  @param dv Dataset's value; never NULL.
       *  @param sv Search mask's value (may contain wild card symbols); never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool MatchStarSymbol( const char *dv, const char *sv );

      /** This function extracts the actual lower and upper date or time values from a given
       *  date or time range.
       *  @param range Date or time range from which lower and upper values shall be extracted.
       *  @param lower Newly created string specifying the lower value from the date/time range;
       *               NULL if value is not specified in range.
       *  @param upper Newly created string specifying the upper value from the date/time range;
       *               NULL if value is not specified in range.
       */
    void ExtractValuesFromRange( const char *range, char *&lower, char *&upper );

  public:
      /** default constructor.
       */
    WlmFileSystemInteractionManager();

      /** destructor
       */
    ~WlmFileSystemInteractionManager();

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetLogStream( OFConsole *value );

      /**  Set value in member variable.
       *  @param value The value to set.
       */
    void SetVerbose( OFBool value );

      /**  Set value in member variable.
       *  @param value The value to set.
       */
    void SetDebug( OFBool value );

      /** Connects to the worklist file system database.
       *  @param dfPathv Path to worklist file system database.
       *  @return Indicates if the connection could be established or not.
       */
    OFCondition ConnectToFileSystem( char *dfPathv );

      /** Disconnects from the worklist file system database..
       *  @return Indicates if the connection was disconnected successfully.
       */
    OFCondition DisconnectFromFileSystem();

      /** Checks if the given called application entity title is supported. If this is the case,
       *  OFTrue will be returned, else OFFalse.
       *  @param calledApplicationEntityTitlev The application entity title which shall be checked
       *                                       for support. Valid pointer expected.
       *  @return OFTrue, if the called application entity title is supported,
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    OFBool IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitlev );

      /** This function determines the records from the worklist files which match
       *  the given search mask and returns the number of matching records. Also,
       *  this function will store the matching records in memory in the array
       *  member variable matchingRecords.
       *  @param searchMask - [in] The search mask.
       *  @return Number of matching records.
       */
    unsigned long DetermineMatchingRecords( DcmDataset *searchMask );

      /** This function determines an attribute value of a matching record
       *  and returns this value in a newly created string to the caller.
       *  @param tag   Attribute tag; specifies which attribute's value shall be returned.
       *  @param idx   Identifies the record from which the attribute value shall be retrieved.
       *  @param value Pointer to a newly created string that contains the requested value.
       *               If value was not found an emtpy string will be returned.
       */
    void GetAttributeValueForMatchingRecord( DcmTagKey tag, unsigned long idx, char *&value );

      /** This function frees the memory which was occupied by matchingRecords.
       *  It shall be called when the matching records are no longer needed.
       */
    void ClearMatchingRecords();
};

#endif

/*
** CVS Log
** $Log: wlfsim.h,v $
** Revision 1.5  2003-07-02 09:17:55  wilkens
** Updated documentation to get rid of doxygen warnings.
**
** Revision 1.4  2002/12/16 11:08:35  wilkens
** Added missing #include "osconfig.h" to certain files.
**
** Revision 1.3  2002/12/09 13:41:43  joergr
** Renamed parameter to avoid name clash with global function index().
** Added private undefined copy constructor and/or assignment operator.
**
** Revision 1.2  2002/08/12 10:56:08  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.1  2002/08/05 09:09:58  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.1  2002/07/17 13:10:21  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
**
**
*/
