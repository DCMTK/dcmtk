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
*  Purpose: Class for managing database interaction.
*
*  Last Update:      $Author: joergr $
*  Update Date:      $Date: 2002-01-08 17:40:06 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wldbim.h,v $
*  CVS/RCS Revision: $Revision: 1.3 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

class DcmTagKey;

class WlmDatabaseInteractionManager
// Date   : December 10, 2001
// Author : Thomas Wilkens, Marcel Claus
// Task   : This class encapsulates data structures and operations for managing
//          data base interaction in the framework of the DICOM basic worklist
//          management service.
{
  protected:
    WlmObjectStatusType objectStatus;
    OFBool verboseMode;
    OFBool useDBConnect;
    OFConsole *logStream;
    const int opt_serialNumber;
    char uidPrefix[65];

    void DumpMessage( const char *message );

  public:
    // Constructor/Destructor
    WlmDatabaseInteractionManager( OFConsole *logStreamv, const OFBool verbosev, char *dbDsnv, char *dbUserNamev, char *dbUserPasswordv, const int serialNumberv);

    ~WlmDatabaseInteractionManager();

    OFBool IsObjectStatusOk();
      // Task         : Returns OFTrue if the object status is okay.
      // Parameters   : none.
      // Return Value : OFTrue - The object status is ok.
      //                OFFalse - The object status is not ok.

    OFBool IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitle );
      // Task         : Checks if the given called application entity title is supported. If this is the case,
      //                OFTrue will be returned, else OFFalse.
      // Parameters   : calledApplicationEntityTitle - [in] The application entity title which shall be checked
      //                for support. Valid pointer expected.
      // Return Value : OFTrue  - The called application entity title is supported.
      //                OFFalse - The called application entity title is not supported or it is not given.

    void GetMatchingRecordIDs( char **matchingKeyAttrValues, unsigned long numOfMatchingKeyAttrValues, long *&matchingRecordIDs, unsigned long &numOfMatchingRecordIDs );
      // Task         : This function determines the ids of the database records that match the values which
      //                are passed in the matchingKeyAttrValues array. In the current implementation, this
      //                array shouls always contain 7 array fields that refer to the following attributes:
      //                 - matchingKeyAttrValues[0] refers to attribute ScheduledStationAETitle.
      //                 - matchingKeyAttrValues[1] refers to attribute ScheduledProcedureStepStartDate.
      //                 - matchingKeyAttrValues[2] refers to attribute ScheduledProcedureStepStartTime.
      //                 - matchingKeyAttrValues[3] refers to attribute Modality.
      //                 - matchingKeyAttrValues[4] refers to attribute ScheduledPerformingPhysiciansName.
      //                 - matchingKeyAttrValues[5] refers to attribute PatientsName.
      //                 - matchingKeyAttrValues[6] refers to attribute PatientID.
      //                The result value of this function is an array of ids that can be used to identify the
      //                database records which match the given attribute values.
      // Parameters   : matchingKeyAttrValues      - [in] Contains the matching key attribute values.
      //                numOfMatchingKeyAttrValues - [in] Number of matching key attribute values.
      //                matchingRecordIDs          - [out] Newly created array of database record ids.
      //                numOfmatchingRecordIDs     - [out] Number of array fields in the above array matchingRecordIDs.
      // Return Value : none.

    void GetAttributeValueForMatchingRecord( DcmTagKey tag, long matchingRecordID, char *&value );
      // Task         : This function determines an attribute value of a certain (matching) record in the database
      //                and returns this value in a newly created string to the caller.
      // Parameters   : tag      - [in] Attribute tag. Specifies which attribute's value shall be returned.
      //                recordID - [in] Identifies the record from which the attribute value shall be retrieved.
      //                value    - [out] Pointer to a newly created string that contains the requested value.
      // Return Value : none.
};


/*
** CVS Log
** $Log: wldbim.h,v $
** Revision 1.3  2002-01-08 17:40:06  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/
