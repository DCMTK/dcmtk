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
*  Author:  Marcel Claus
*
*  Purpose: Class for managing pki-file interaction.
*
*  Last Update:      $Author: wilkens $
*  Update Date:      $Date: 2002-06-10 11:24:57 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wlpfim.h,v $
*  CVS/RCS Revision: $Revision: 1.3 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

class DcmTagKey;
class OFConsole;
class OFCondition;

  /** This class encapsulates data structures and operations for managing
   *  pki-file interaction in the framework of the DICOM basic worklist
   *  management service.
   */
class WlmPkiFileInteractionManager
{
  protected:
    OFBool verboseMode;
    OFConsole *logStream;
    char uidPrefix[65];
    char pfFileName[200];

    void DumpMessage( const char *message );
    void ExtractPkiValues( char *name, char *value, const char *source );
    void GetFileValue( char *target, const char *pkiUpperName, const char *pkiLowerName );
    void GetPkiValue( char *target, const char *dcmName );

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmPkiFileInteractionManager( const WlmPkiFileInteractionManager &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmPkiFileInteractionManager &operator=( const WlmPkiFileInteractionManager &Src );


  public:
      /** default constructor.
       */
    WlmPkiFileInteractionManager();

      /** destructor
       */
    ~WlmPkiFileInteractionManager();

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetLogStream( OFConsole *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetVerbose( OFBool value );

      /** Connects to the pki-file.
       *  @param fileName     Name of the pki-file.
       *  @param serialNumber Serial number.
       */
    OFCondition ConnectToPkiFile( char *fileName, int serialNumber );

      /** Disconnects from the pki-file.
       */
    OFCondition DisconnectFromPkiFile();

      /** Checks if the given called application entity title is supported. If this is the case,
       *  OFTrue will be returned, else OFFalse.
       *  @param calledApplicationEntityTitle The application entity title which shall be checked
       *                                      for support. Valid pointer expected.
       *  @return OFTrue, if the called application entity title is supported,
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    OFBool IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitle );

      /** This function determines the ids of the pki file records that match the values which
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
       *  database records which match the given attribute values. Note that the pki-file will
       *  always contain only one record which shall always be returned. This circumstance will
       *  be indicated by always returning one matching record with a record id of -1.
       *  @param matchingKeyAttrValues      Contains the matching key attribute values.
       *  @param numOfMatchingKeyAttrValues Number of matching key attribute values.
       *  @param matchingRecordIDs          Newly created array of database record ids.
       *  @param numOfMatchingRecordIDs     Number of array fields in the above array matchingRecordIDs.
       */
    void GetMatchingRecordIDs( const char **matchingKeyAttrValues, unsigned long numOfMatchingKeyAttrValues, long *&matchingRecordIDs, unsigned long &numOfMatchingRecordIDs );

      /** This function determines an attribute value of a certain (matching) record in the pki-file
       *  and returns this value in a newly created string to the caller.
       *  @param tag      Attribute tag. Specifies which attribute's value shall be returned.
       *  @param recordID Identifies the record from which the attribute value shall be retrieved.
       *  @param value    Pointer to a newly created string that contains the requested value.
       */
    void GetAttributeValueForMatchingRecord( DcmTagKey tag, long matchingRecordID, char *&value );
};


/*
** CVS Log
** $Log: wlpfim.h,v $
** Revision 1.3  2002-06-10 11:24:57  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.2  2002/04/18 14:19:59  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
**
**
*/
