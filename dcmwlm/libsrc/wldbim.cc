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
 *  Update Date:      $Date: 2002-01-08 16:32:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/Attic/wldbim.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"
#include "ofconsol.h"
#include "dicom.h"
#include "wltypdef.h"
#include "dctagkey.h"

#include "wldbim.h"

// ----------------------------------------------------------------------------

WlmDatabaseInteractionManager::WlmDatabaseInteractionManager( OFConsole *logStreamv, char *dbDsnv, char *dbUserNamev, char *dbUserPasswordv, char *dbSchemav )
// Date         : December 18, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : logStreamv      - [in] Stream that can be used to dump a message to.
//                dbDsnv          - [in] The data source name of the database that shall be used.
//                dbUserNamev     - [in] The database user name that shall be used for querying information.
//                dbUserPasswordv - [in] The password that belongs to the database user name.
//                dbSchemav       - [in] The data schema that holds the tables which shall be queried.
// Return Value : none.
  : logStream( logStreamv ), objectStatus( WLM_STATUS_OK )
{
/*

  // in case we want to access the database to select the information
  if( strcmp( dbDsnv, "text" ) != 0 )
  {
    // connect to the specified database

    if( connectionCouldNotBeEstablished )
      objectStatus = WLM_STATUS_INIT_FAILED;
  }
  // in case we want to access this particular file to select the information
  else
  {
    // check if the file is existent.

    if( fileIsNotExistent )
      objectStatus = WLM_STATUS_INIT_FAILED;
  }

*/
}

// ----------------------------------------------------------------------------

WlmDatabaseInteractionManager::~WlmDatabaseInteractionManager()
// Date         : December 18, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
/*
  // terminate database connection if there is one
*/
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::DumpMessage( const char *message )
// Date         : 1998
// Author       : Jörg Riesmeier
// Task         : This function dumps the given information on a stream.
//                Used for dumping information in normal, debug and verbose mode.
// Parameters   : message - [in] The message to dump.
// Return Value : none.
{
  if( logStream != NULL && message != NULL )
  {
    logStream->lockCout();
    logStream->getCout() << message << endl;
    logStream->unlockCout();
  }
}

// ----------------------------------------------------------------------------

OFBool WlmDatabaseInteractionManager::IsObjectStatusOk()
// Date         : December 19, 2001
// Author       : Thomas Wilkens
// Task         : Returns OFTrue if the object status is okay.
// Parameters   : none.
// Return Value : OFTrue - The object status is ok.
//                OFFalse - The object status is not ok.
{
  if( objectStatus == WLM_STATUS_OK )
    return OFTrue;
  else
    return OFFalse;
}

// ----------------------------------------------------------------------------

OFBool WlmDatabaseInteractionManager::IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitle )
// Date         : December 19, 2001
// Author       : Thomas Wilkens
// Task         : Checks if the given called application entity title is supported. If this is the case,
//                OFTrue will be returned, else OFFalse.
// Parameters   : calledApplicationEntityTitle - [in] The application entity title which shall be checked
//                for support. Valid pointer expected.
// Return Value : OFTrue  - The called application entity title is supported.
//                OFFalse - The called application entity title is not supported or it is not given.
{
  // Check if calledApplicationEntityTitle is supported
  // ### not yet implemented
  return( OFTrue );
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::GetMatchingRecordIDs( char **matchingKeyAttrValues, unsigned long numOfMatchingKeyAttrValues, long *&matchingRecordIDs, unsigned long &numOfMatchingRecordIDs )
// Date         : December 19, 2001
// Author       : Marcel Claus
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
//                numOfMatchingRecordIDs     - [out] Number of array fields in the above array matchingRecordIDs.
// Return Value : none.
{
  // ### not yet implemented
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::GetAttributeValueForMatchingRecord( DcmTagKey tag, long recordID, char *&value )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function determines an attribute value of a certain (matching) record in the database
//                and returns this value in a newly created string to the caller.
// Parameters   : tag      - [in] Attribute tag. Specifies which attribute's value shall be returned.
//                recordID - [in] Identifies the record from which the attribute value shall be retrieved.
//                value    - [out] Pointer to a newly created string that contains the requested value.
// Return Value : none.
{
  // ### not yet implemented
/*
  um zu erfahren, um welches Attribut es sich handelt, brauchst Du hier sowas:

  switch( tag )
  {
    case DCM_ScheduledStationAETitle: 
      break;
    case DCM_ScheduledProcedureStepStartDate:
      break;
    case DCM_ScheduledProcedureStepStartTime:
      break;
    case DCM_Modality:
      break;
    case DCM_ScheduledPerformingPhysiciansName:
      break;
    case DCM_PatientsName:
      break;
    case DCM_PatientID:
      break;
    case DCM_ScheduledProcedureStepDescription:
      break;
    case DCM_ScheduledStationName:
      break;
    case DCM_ScheduledProcedureStepID:
      break;
    case DCM_RequestedProcedureID:
      break;
    case DCM_RequestedProcedureDescription:
      break;
    case DCM_AccessionNumber:
      break;
    case DCM_ReferringPhysiciansName:
      break;
    case DCM_PatientsBirthDate:
      break;
    case DCM_PatientsSex:
      break;
    case DCM_InstitutionName:
      break;
    case DCM_StudyInstanceUID:
      break;
    default:
      DumpMessage("WlmDatabaseInteractionManager::GetAttributeValueForMatchingRecord: Unknown key passed.");
      break;
  }

  Dies sind alle Attribute, die Du unterstützen mußt. Jedenfalls nach meiner Liste.
*/

}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wldbim.cc,v $
** Revision 1.1  2002-01-08 16:32:45  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
