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
*  Update Date:      $Date: 2002-04-18 10:30:11 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wlpfim.cc,v $
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
#include "dctk.h"
#include <stdio.h>

#include "wlpfim.h"

// ----------------------------------------------------------------------------

WlmPkiFileInteractionManager::WlmPkiFileInteractionManager()
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : verboseMode( OFFalse ), logStream( NULL )
{
  strcpy( uidPrefix, "" );
  strcpy( pfFileName, "" );
}

// ----------------------------------------------------------------------------

WlmPkiFileInteractionManager::~WlmPkiFileInteractionManager()
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::SetLogStream( OFConsole *value )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : Set value in member variable.
// Parameters   : value - [in] The value to set.
// Return Value : none.
{
  logStream = value;
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::SetVerbose( OFBool value )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : Set value in member variable.
// Parameters   : value - [in] The value to set.
// Return Value : none.
{
  verboseMode = value;
}

// ----------------------------------------------------------------------------

OFCondition WlmPkiFileInteractionManager::ConnectToPkiFile( char *fileName, int serialNumber )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : Connects to the pki-file.
// Parameters   : fileName     - [in] Name of the pki-file
//                serialNumber - [in] serial number
// Return Value : none.
{
  // add serial number to OFFIS GO-Kard UID root
  sprintf( uidPrefix, "1.2.276.0.7230010.8.%d", serialNumber );

  // connect to pki-file
  // file name must be given to be able to connect
  if( fileName == NULL )
  {
    // if it is not given, dump a warning
    if( logStream != NULL )
    {
      logStream->lockCerr() << "Unable to open PKI-inifile, invalid filename..." << endl;
      logStream->unlockCerr();
    }

    // return error
    return( WLM_EC_CannotConnectToDataSource );
  }

  // store file name in member variable
  strcpy( pfFileName, fileName );

  // return success
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

OFCondition WlmPkiFileInteractionManager::DisconnectFromPkiFile()
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : Disconnects from the pki-file.
// Parameters   : none.
// Return Value : none.
{
  // return success
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::DumpMessage( const char *message )
// Date         : March 18, 2002
// Author       : Marcel Claus
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

OFBool WlmPkiFileInteractionManager::IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitle )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : Checks if the given called application entity title is supported. If this is the case,
//                OFTrue will be returned, else OFFalse.
// Parameters   : calledApplicationEntityTitle - [in] The application entity title which shall be checked
//                                               for support. Valid pointer expected.
// Return Value : OFTrue  - The called application entity title is supported.
//                OFFalse - The called application entity title is not supported or it is not given.
{
  // Check if calledApplicationEntityTitle is supported
  // At the moment we support everything.
  return( OFTrue );
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::GetMatchingRecordIDs( const char **matchingKeyAttrValues, unsigned long numOfMatchingKeyAttrValues, long *&matchingRecordIDs, unsigned long &numOfMatchingRecordIDs )
// Date         : March 18, 2001
// Author       : Marcel Claus
// Task         : This function determines the ids of the pki file records that match the values which
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
//                database records which match the given attribute values. Note that the pki-file will
//                always contain only one record which shall always be returned. This circumstance will
//                be indicated by always returning one matching record with a record id of -1.
// Parameters   : matchingKeyAttrValues      - [in] Contains the matching key attribute values.
//                numOfMatchingKeyAttrValues - [in] Number of matching key attribute values.
//                matchingRecordIDs          - [out] Newly created array of database record ids.
//                numOfMatchingRecordIDs     - [out] Number of array fields in the above array matchingRecordIDs.
// Return Value : none.
{
  // in the pki-file there is always just one record. This circumstance will be indicated
  // by a matching record ID of -1 in a result array with just one array field.
  numOfMatchingRecordIDs = 1;
  matchingRecordIDs = (long *) malloc(sizeof(long));
  matchingRecordIDs[0] = -1;

  // dump some information if required
  if( verboseMode && logStream != NULL )
  {
    logStream->lockCout() << "using PKI-file, only one PA " << matchingRecordIDs[0] << endl;
    logStream->unlockCout();
  }
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::ExtractPkiValues( char *name, char *value, const char *source )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : This function extracts attribute tag and value from a line which was read from the pki-file.
// Parameters   : name   - [out] The tag of the attribute.
//                value  - [out] The value of the attribute.
//                source - [in] The line which was read from the pki-file.
// Return Value : none.
{
  int i, status, nameIndex, valueIndex;
  status = 0;
  i = 0;
  name[0] = 0;
  nameIndex = 0;
  value[0] = 0;
  valueIndex = 0;

  while( source[i] )
  {
    if( source[i] == '<' && ( status == 1 || status == 0 ) )
      status = 2;
    else
    {
      if( source[i] == ' ' && status == 0 )
        status = 1;
      else
      {
        if( source[i] == '>' && status == 2 )
        {
          status = 3;
        }
        else
        {
          switch( status )
          {
            case 0: name[nameIndex] = source[i]; nameIndex++; break;
            case 2: value[valueIndex] = source[i]; valueIndex++; break;
          }
        }
      }
    }
    i++;
  }

  value[valueIndex] = 0;
  name[nameIndex] = 0;
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::GetFileValue( char *target, const char *pkiUpperName, const char *pkiLowerName )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : This function reads a value from the pki-file for a given dcmTag returns it in target.
//                If nothing was found target[0] will be 0 (empty string).
// Parameters   : target       - [out] value which was determined.
//                pkiUpperName - [in] tag of the attribute for which a value shall be returned in uppercase letters
//                pkiLowerName - [in] tag of the attribute for which a value shall be returned in lowercase letters
// Return Value : none.
{
  FILE *theDat;
  char hStr[500], pkiTagName[100], pkiTagValue[100];
  short goon;

  // open file
  if( ( theDat = fopen( pfFileName, "r" ) ) != NULL )
  {
    goon = 1;

    // as long as we want to go on, read another line from the file
    while( fgets( hStr, 500, theDat ) && goon )
    {
      // if the line is neither empty nor a comment (which starts with a '#')
      if( hStr[0] && hStr[0] != '#' )
      {
        // extract value from file
        ExtractPkiValues( pkiTagName, pkiTagValue, hStr );

        if( (strcmp( pkiTagName, pkiUpperName ) == 0) || ( strcmp(pkiTagName, pkiLowerName) == 0 ) )
        {
          strcpy( target, pkiTagValue );
          goon = 0;
        }
      }
    }
    fclose(theDat);
  }
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::GetPkiValue( char *target, const char *dcmName )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : This function reads the corresponding value from the pki-file for dcmTag with name
//                tagName and returns it in target. If nothing was found target[0] will be 0 (empty string).
// Parameters   : target   - [out] value which was determined.
//                dcmName  - [in] tag of the attribute for which a value shall be returned
// Return Value : none.
{
  char valueStr1[200], valueStr2[200];

  // initialize return value
  target[0] = 0;

  // determine return value
  if( strcmp( dcmName, "PatientID" ) == 0 )
    GetFileValue( target, "PATIENT_ID", "patient_id" );
  else if( strcmp( dcmName, "PatientsName" ) == 0)
  {
    GetFileValue( valueStr1, "PATIENT_LAST_NAME", "patient_last_name" );
    GetFileValue( valueStr2, "PATIENT_FORE_NAME", "patient_fore_name" );
    /* sprintf(target, "%s^%s", valueStr1, valueStr2); */ // this ist standard conform and will be supported sometime by the new software from philips
    sprintf( target, "%s, %s", valueStr1, valueStr2 );
  }
  else if( strcmp( dcmName, "AccessionNumber" ) == 0 )
  {
    // if AccessionNumber is requested the value in PATIENT_UID is returned
    // this is a bug in the format of the PKI-file!
    GetFileValue( target, "PATIENT_UID", "patient_uid" );
    //GetFileValue (target, "ACCESSION_NUMBER", "accession_number" );
  }
  else if( strcmp( dcmName, "ScheduledProcedureStepID" ) == 0 )
    GetFileValue( target, "ACCESSION_NUMBER", "accession_number" );
  else if( strcmp( dcmName, "RequestedProcedureID" ) == 0 )
    GetFileValue (target, "PATIENT_UID", "patient_uid" );
  else if( strcmp(dcmName, "PatientsBirthDate" ) == 0 )
    GetFileValue( target, "PATIENT_BIRTHDAY", "patient_birthday" );
  else if( strcmp( dcmName, "ScheduledProcedureStepStartTime" ) == 0 )
    GetFileValue( target, "EXAM_TIME", "exam_time" );
  else if( strcmp(dcmName, "PatientsSex" ) == 0 )
  {
    GetFileValue( valueStr1, "PATIENT_GENDER", "patient_gender" );
    if( valueStr1[0] == 'F' || valueStr1[0] == 'f' || valueStr1[0] == 'W' || valueStr1[0] == 'w' )
      target[0] = 'F';
    else
      target[0] = 'M';
    target[1] = 0;
  }
  else if( strcmp( dcmName, "ScheduledProcedureStepStartDate" ) == 0 )
    GetFileValue( target, "EXAM_DATE", "exam_date" );
  else if( strcmp(dcmName, "ScheduledPerformingPhysiciansName" ) == 0 )
    GetFileValue( target, "PHYSICIAN_CODE", "physician_code" );
}

// ----------------------------------------------------------------------------

void WlmPkiFileInteractionManager::GetAttributeValueForMatchingRecord( DcmTagKey tag, long recordID, char *&value )
// Date         : March 18, 2002
// Author       : Marcel Claus
// Task         : This function determines an attribute value of a certain (matching) record in the pki-file
//                and returns this value in a newly created string to the caller.
// Parameters   : tag      - [in] Attribute tag. Specifies which attribute's value shall be returned.
//                recordID - [in] Identifies the record from which the attribute value shall be retrieved.
//                value    - [out] Pointer to a newly created string that contains the requested value.
// Return Value : none.
{
  char tmpStr[2000];

  // dump some information in verbose mode
  if( verboseMode && logStream != NULL )
  {
    logStream->lockCout() << "get1Value PA: " << recordID << ", TagName: " << DcmTag(tag).getTagName() << endl;
    logStream->unlockCout();
  }

  // if we are to select a value for the Study Instance UID attribute
  if( tag == DCM_StudyInstanceUID )
  {
    // generate a new Study Instance UID
    dcmGenerateUniqueIdentifier( tmpStr, uidPrefix );
  }
  // if we are dealing with some other attribute
  else
  {
    // select a value from the pki-file
    GetPkiValue( tmpStr, DcmTag(tag).getTagName() );
  }

  // copy generated/selected value into out parameter
  value = new char[ strlen(tmpStr) + 1 ];
  strcpy( value, tmpStr );
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wlpfim.cc,v $
** Revision 1.1  2002-04-18 10:30:11  wilkens
** Performed split between db-variant and pki-variant.
**
**
**
*/
