/*
*
*  Copyright (C) 1996-2001, OFFIS
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
*  Update Date:      $Date: 2002-08-05 09:09:58 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wlfsim.h,v $
*  CVS/RCS Revision: $Revision: 1.1 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

#ifndef WlmFileSystemInteractionManager_h
#define WlmFileSystemInteractionManager_h

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
  protected:
    OFBool verboseMode;
    OFConsole *logStream;
    char *dfPath;
    char *calledApplicationEntityTitle;
    DcmDataset **matchingRecords;
    unsigned long numOfMatchingRecords;

    void DumpMessage( const char *message );
    void DetermineWorklistFiles( OFOrderedSet<OFString> &worklistFiles );
    OFBool IsWorklistFile( const char *fname );
    OFBool DatasetMatchesSearchMask( DcmDataset *dataset, DcmDataset *searchMask );
    void DetermineMatchingKeyAttributeValues( DcmDataset *dataset, const char **&matchingKeyAttrValues );
    OFBool ScheduledStationAETitlesMatch( const char *datasetValue, const char *searchMaskValue );
    OFBool ScheduledProcedureStepStartDateTimesMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );
    OFBool ModalitiesMatch( const char *datasetValue, const char *searchMaskValue );
    OFBool ScheduledPerformingPhysiciansNamesMatch( const char *datasetValue, const char *searchMaskValue );
    OFBool PatientsNamesMatch( const char *datasetValue, const char *searchMaskValue );
    OFBool PatientIdsMatch( const char *datasetValue, const char *searchMaskValue );
    OFBool DateTimeRangeMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );
    OFBool DateRangeMatch( const char *datasetDateValue, const char *searchMaskDateValue );
    OFBool TimeRangeMatch( const char *datasetTimeValue, const char *searchMaskTimeValue );
    OFBool DateTimeSingleValueMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );
    OFBool DateSingleValueMatch( const char *datasetDateValue, const char *searchMaskDateValue );
    OFBool TimeSingleValueMatch( const char *datasetTimeValue, const char *searchMaskTimeValue );
    OFBool CaseSensitiveSingleValueMatch( const char *datasetValue, const char *searchMaskValue );
    OFBool WildcardMatch( const char *datasetValue, const char *searchMaskValue );
    OFBool MatchStarSymbol( const char *dv, const char *sv );
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

      /** Connects to the worklist file system database.
       *  @param dfPath Path to worklist file system database.
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
       *  @param index Identifies the record from which the attribute value shall be retrieved.
       *  @param value Pointer to a newly created string that contains the requested value.
       *               If value was not found an emtpy string will be returned.
       */
    void GetAttributeValueForMatchingRecord( DcmTagKey tag, unsigned long index, char *&value );

      /** This function frees the memory which was occupied by matchingRecords.
       *  It shall be called when the matching records are no longer needed.
       */
    void ClearMatchingRecords();
};

#endif

/*
** CVS Log
** $Log: wlfsim.h,v $
** Revision 1.1  2002-08-05 09:09:58  wilkens
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
