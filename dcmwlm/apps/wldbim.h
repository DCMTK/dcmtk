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
*  Author:  Thomas Wilkens, Marcel Claus
*
*  Purpose: Class for managing database interaction.
*
*  Last Update:      $Author: wilkens $
*  Update Date:      $Date: 2002-04-18 14:19:50 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldbim.h,v $
*  CVS/RCS Revision: $Revision: 1.2 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

#ifndef WlmDatabaseInteractionManager_h
#define WlmDatabaseInteractionManager_h

class DcmTagKey;
class OFConsole;
class OFCondition;

  /** Datatype for a list of supported tags and select statements which are used
   *  to select values for the corresponding tags from the database.
   */
typedef struct myDcmTag
{
  char tag[90];
  char selectStmt[1000];
  struct myDcmTag *next;
} myDcmTag;

  /** This class encapsulates data structures and operations for managing
   *  data base interaction in the framework of the DICOM basic worklist
   *  management service.
   */
class WlmDatabaseInteractionManager
{
  protected:
    OFBool verboseMode;
    OFConsole *logStream;
    char uidPrefix[65];
    myDcmTag *myDcmTagAnker;
    otl_connect db;
    char paStmt[1500];
    char wlmCountStmt[1500];
    char wlmIDStmt[1500];
    char wlmIDInsertStmt[1500];
    char wlmIDUpdateStmt[1500];
    char connectStr[200];

    void DumpMessage( const char *message );
    void ExtractRange( const char *theSource, const char *noValueMin, const char *noValueMax, const char *rangeValueMin, const char *rangeValueMax, char *targetMin, char *targetMax );
    void DoReplace( char *theStr, const char *oldStr, const char *newStr );
    void ReadDcmTags( char *iniFile );
    void ReadOneStatement( const char *readLine, char *theStmt );
    void ReadSearchStmt( char *iniFile );
    int LogonToDatabase( short do_wait, long wait_time );
    void ReplaceWildcards( char *target, const char *source );
    void GetDbValue( char *target, long numPA, const char *tagName );
    void CheckForExistingStudyInstanceUid( char *dcmStudyInstanceUID, long recordID );
    void StoreStudyInstanceUid( const char *theStudyInstanceUID, long recordID );
    OFBool IsFileExistent( char *fileName );

  public:
      /** default constructor.
       */
    WlmDatabaseInteractionManager();

      /** destructor
       */
    ~WlmDatabaseInteractionManager();

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetLogStream( OFConsole *value );

      /**  Set value in member variable.
       *  @param value The value to set.
       */
    void SetVerbose( OFBool value );

      /** Connects to the database.
       *  @param dbDsn               Data Source Name.
       *  @param dbUserName          Database user name
       *  @param dbUserPassword      Database user password
       *  @param serialNumber        Serial number
       *  @param cfgFileMatchRecords Name of configuration file for determination of matching records.
       *  @param cfgFileSelectValues Name of configuration file for selection of attribute values.
       *  @param databaseType        Type of database.
       *  @return Indicates if the connection could be established or not.
       */
    OFCondition ConnectToDatabase( char *dbDsn, char *dbUserName, char *dbUserPassword, int serialNumber, char *cfgFileMatchRecords, char *cfgFileSelectValues, WlmDatabaseType databaseType );

      /** Disconnects from the database.
       *  @return Indicates if the connection was disconnected successfully.
       */
    OFCondition DisconnectFromDatabase();

      /** Checks if the given called application entity title is supported. If this is the case,
       *  OFTrue will be returned, else OFFalse.
       *  @param calledApplicationEntityTitle The application entity title which shall be checked
       *                                      for support. Valid pointer expected.
       *  @return OFTrue, if the called application entity title is supported,
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    OFBool IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitle );

      /** This function determines the ids of the database records that match the values which
       *  are passed in the matchingKeyAttrValues array. In the current implementation, this
       *  array shouls always contain 7 array fields that refer to the following attributes:
       *   - matchingKeyAttrValues[0] refers to attribute ScheduledStationAETitle.
       *   - matchingKeyAttrValues[1] refers to attribute ScheduledProcedureStepStartDate.
       *   - matchingKeyAttrValues[2] refers to attribute ScheduledProcedureStepStartTime.
       *   - matchingKeyAttrValues[3] refers to attribute Modality.
       *   - matchingKeyAttrValues[4] refers to attribute ScheduledPerformingPhysiciansName.
       *   - matchingKeyAttrValues[5] refers to attribute PatientsName.
       *   - matchingKeyAttrValues[6] refers to attribute PatientID.
       *  The result value of this function is an array of ids that can be used to identify the
       *  database records which match the given attribute values.
       *  @param matchingKeyAttrValues      Contains the matching key attribute values.
       *  @param numOfMatchingKeyAttrValues Number of matching key attribute values.
       *  @param matchingRecordIDs          Newly created array of database record ids.
       *  @param numOfMatchingRecordIDs     Number of array fields in the above array matchingRecordIDs.
       */
    void GetMatchingRecordIDs( const char **matchingKeyAttrValues, unsigned long numOfMatchingKeyAttrValues, long *&matchingRecordIDs, unsigned long &numOfMatchingRecordIDs );

      /** This function determines an attribute value of a certain (matching) record in the database
       *  and returns this value in a newly created string to the caller.
       *  @param tag      Attribute tag; specifies which attribute's value shall be returned.
       *  @param recordID Identifies the record from which the attribute value shall be retrieved.
       *  @param value    Pointer to a newly created string that contains the requested value.
       */
    void GetAttributeValueForMatchingRecord( DcmTagKey tag, long matchingRecordID, char *&value );
};

#endif

/*
** CVS Log
** $Log: wldbim.h,v $
** Revision 1.2  2002-04-18 14:19:50  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.4  2002/01/08 17:45:33  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.3  2002/01/08 17:40:06  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/
