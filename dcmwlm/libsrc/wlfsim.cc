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
*  Last Update:      $Author: joergr $
*  Update Date:      $Date: 2002-12-09 13:42:22 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/wlfsim.cc,v $
*  CVS/RCS Revision: $Revision: 1.5 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

// ----------------------------------------------------------------------------

#include "osconfig.h"

#define INCLUDE_CLIMITS
#include "ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>  // for struct DIR, opendir()
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>    // for struct DIR, opendir()
#endif
END_EXTERN_C

#include "diutil.h"
#include "ofconsol.h"
#include "ofstd.h"
#include "ofoset.h"
#include "ofdate.h"
#include "oftime.h"
#include "oftypes.h"
#include "dicom.h"
#include "dcdatset.h"
#include "dcitem.h"
#include "dcvrda.h"
#include "dcvrtm.h"
#include "wltypdef.h"
#include "dctk.h"
#include <stdio.h>
#include <stdlib.h>

#include "wlfsim.h"

// ----------------------------------------------------------------------------

WlmFileSystemInteractionManager::WlmFileSystemInteractionManager()
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : verboseMode( OFFalse ), debugMode( OFFalse ), logStream( NULL ), dfPath( NULL ), calledApplicationEntityTitle( NULL ),
    matchingRecords( NULL ), numOfMatchingRecords( 0 )
{
}

// ----------------------------------------------------------------------------

WlmFileSystemInteractionManager::~WlmFileSystemInteractionManager()
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  if( dfPath != NULL ) delete dfPath;
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::SetLogStream( OFConsole *value )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Set value in member variable.
// Parameters   : value - [in] The value to set.
// Return Value : none.
{
  logStream = value;
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::SetVerbose( OFBool value )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Set value in member variable.
// Parameters   : value - [in] The value to set.
// Return Value : none.
{
  verboseMode = value;
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::SetDebug( OFBool value )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Set value in member variable.
// Parameters   : value - [in] The value to set.
// Return Value : none.
{
  debugMode = value;
}

// ----------------------------------------------------------------------------

OFCondition WlmFileSystemInteractionManager::ConnectToFileSystem( char *dfPathv )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Connects to the worklist file system database.
// Parameters   : dfPathv - [in] Path to worklist file system database.
// Return Value : Indicates if the connection could be established or not.
{
  // check parameter
  if( dfPathv == NULL )
  {
    DumpMessage("Invalid parameters, cannot connect to worklist file system database...");
    return( WLM_EC_CannotConnectToDataSource );
  }

  // copy value
  dfPath = new char[ strlen( dfPathv ) + 1 ];
  strcpy( dfPath, dfPathv );

  // check if the specified path is existent and accessible for reading
  if( !OFStandard::dirExists( OFString( dfPath ) ) || !OFStandard::isReadable( OFString( dfPath ) ) )
    return( WLM_EC_CannotConnectToDataSource );
  else
    return( EC_Normal );
}

// ----------------------------------------------------------------------------

OFCondition WlmFileSystemInteractionManager::DisconnectFromFileSystem()
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Disconnects from the worklist file system database.
// Parameters   : none.
// Return Value : Indicates if the connection was disconnected successfully.
{
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::DumpMessage( const char *message )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
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

OFBool WlmFileSystemInteractionManager::IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitlev )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Checks if the given called application entity title is supported. If this is the case,
//                OFTrue will be returned, else OFFalse.
// Parameters   : calledApplicationEntityTitlev - [in] The application entity title which shall be checked
//                                                for support. Valid pointer expected.
// Return Value : OFTrue  - The called application entity title is supported.
//                OFFalse - The called application entity title is not supported or it is not given.
{
  // copy value
  calledApplicationEntityTitle = new char[ strlen( calledApplicationEntityTitlev ) + 1 ];
  strcpy( calledApplicationEntityTitle, calledApplicationEntityTitlev );

  // Determine complete path to the files that make up the data source.
  OFString fullPath( dfPath );
  if( fullPath.length() > 0 && fullPath[fullPath.length()-1] != PATH_SEPARATOR )
    fullPath += PATH_SEPARATOR;
  fullPath += calledApplicationEntityTitle;

  // Check if the path is existent.
  if( !OFStandard::dirExists( OFString( fullPath ) ) )
    return( OFFalse );
  else
    return( OFTrue );
}

// ----------------------------------------------------------------------------

unsigned long WlmFileSystemInteractionManager::DetermineMatchingRecords( DcmDataset *searchMask )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function determines the records from the worklist files which match
//                the given search mask and returns the number of matching records. Also,
//                this function will store the matching records in memory in the array
//                member variable matchingRecords.
// Parameters   : searchMask - [in] The search mask.
// Return Value : Number of matching records.
{
  OFOrderedSet<OFString> worklistFiles;
  char msg[500];

  // initialize member variables
  matchingRecords = NULL;
  numOfMatchingRecords = 0;

  // determine all worklist files
  DetermineWorklistFiles( worklistFiles );

  // go through all worklist files
  for( unsigned long i=0 ; i<worklistFiles.NumberOfElements() ; i++ )
  {
    // read information from worklist file
    DcmFileFormat fileform;
    if (fileform.loadFile(worklistFiles[i].c_str()).bad())
    {
      if( verboseMode )
      {
        sprintf( msg, "  - Could not read worklist file %s properly. File will be ignored.", worklistFiles[i].c_str() );
        DumpMessage( msg );
      }
    }
    else
    {
      // determine the data set which is contained in the worklist file
      DcmDataset *dataset = fileform.getDataset();
      if( dataset == NULL )
      {
        if( verboseMode )
        {
          sprintf( msg, "  - Worklist file %s is empty. File will be ignored.", worklistFiles[i].c_str() );
          DumpMessage( msg );
        }
      }
      else
      {
        // check if the current dataset matches the matching key attribute values
        if( !DatasetMatchesSearchMask( dataset, searchMask ) )
        {
          if( verboseMode )
          {
            sprintf( msg, "Information from worklist file %s does not match query.", worklistFiles[i].c_str() );
            DumpMessage( msg );
          }
        }
        else
        {
          if( verboseMode )
          {
            sprintf( msg, "Information from worklist file %s matches query.", worklistFiles[i].c_str() );
            DumpMessage( msg );
          }

          // since the dataset matches the matching key attribute values
          // we need to insert it into the matchingRecords array
          if( numOfMatchingRecords == 0 )
          {
            matchingRecords = new DcmDataset*[1];
            matchingRecords[0] = new DcmDataset( *dataset );
          }
          else
          {
            DcmDataset **tmp = new DcmDataset*[numOfMatchingRecords + 1];
            for( unsigned long j=0 ; j<numOfMatchingRecords ; j++ )
              tmp[j] = matchingRecords[j];
            tmp[numOfMatchingRecords] = new DcmDataset( *dataset );
            delete matchingRecords;
            matchingRecords = tmp;
          }

          numOfMatchingRecords++;
        }
      }
    }
  }

  // return result
  return( numOfMatchingRecords );
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::GetAttributeValueForMatchingRecord( DcmTagKey tag, unsigned long idx, char *&value )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function determines an attribute value of a matching record
//                and returns this value in a newly created string to the caller.
// Parameters   : tag   - [in] Attribute tag. Specifies which attribute's value shall be returned.
//                idx   - [in] Identifies the record from which the attribute value shall be retrieved.
//                value - [out] Pointer to a newly created string that contains the requested value.
//                        If value was not found an emtpy string will be returned.
// Return Value : none.
{
  // determine value of attribute tag in dataset matchingRecords[idx];
  // (note that all values of supported return key attributes are strings
  // except for attribute DCM_PregnancyStatus; this attribute is actually
  // an int which has to be converted to a string before it can be returned)
  // (### also note that for attributes DCM_ReferencedSOPClassUID and DCM_Refe-
  // rencedSOPInstanceUID always the same values will be returned; see comment
  // in wldsfs.cxx)
  if( tag == DCM_PregnancyStatus )
  {
    Uint16 v;
    OFCondition cond = matchingRecords[idx]->findAndGetUint16( tag, v, 0, OFTrue );

    // if value was found
    if( cond.good() )
    {
      // copy value
      value = new char[20];
      sprintf( value, "%u", v );
    }
    else
    {
      // assign empty string to val
      value = new char[ 2 ];
      strcpy( value, "4" );
    }
  }
  else
  {
    const char *val = NULL;
    matchingRecords[idx]->findAndGetString( tag, val, OFTrue );

    // if value was found
    if( val != NULL )
    {
      // copy value
      value = new char[ strlen( val ) + 1 ];
      strcpy( value, val );
    }
    else
    {
      // assign empty string to val
      value = new char[ 1 ];
      strcpy( value, "" );
    }
  }
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::ClearMatchingRecords()
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function frees the memory which was occupied by matchingRecords.
//                It shall be called when the matching records are no longer needed.
// Parameters   : none.
// Return Value : none.
{
  for( unsigned int i=0 ; i<numOfMatchingRecords ; i++ )
    delete matchingRecords[i];
  delete matchingRecords;
  matchingRecords = NULL;
  numOfMatchingRecords = 0;
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::DetermineWorklistFiles( OFOrderedSet<OFString> &worklistFiles )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function determines all worklist files in the directory specified by
//                dfPath and calledApplicationEntityTitle, and returns the complete path and
//                filename information in an array of strings.
// Parameters   : worklistFiles - [out] Set of strings, each specifying path and filename
//                                      to one worklist file.
// Return Value : none.
{
  unsigned int i;

  // initialize out parameters
  worklistFiles.Clear();

  // determine complete path to data source files
  // (dfPath + PATH_SEPARATOR + calledApplicationEntityTitle)
  OFString path( dfPath );
  if( path.length() > 0 && path[path.length()-1] != PATH_SEPARATOR )
    path += PATH_SEPARATOR;
  path += calledApplicationEntityTitle;

  // determine worklist files in this folder
#ifdef HAVE__FINDFIRST
  OFString searchname = path + PATH_SEPARATOR + '*';
  struct _finddata_t fileData;
  int ret = 0;

  // start a loop; in each iteration another directory entry is determined
  long hFile = _findfirst( (char*)searchname.c_str(), &fileData );
  while( hFile != -1L && ret == 0 )
  {
    // if the current entry refers to a worklist file, do something
    if( strcmp( fileData.name, "." )  !=0 && strcmp( fileData.name, ".." ) !=0 && IsWorklistFile( fileData.name ) )
    {
      // Create a string that contains path and filename of the current worklist file.
      OFString subname( path );
      subname += PATH_SEPARATOR;
      subname += fileData.name;

      // Add string to the set of strings
      worklistFiles.Insert( subname );
    }
    ret = _findnext( hFile, &fileData );
  }
  _findclose( hFile );
#else
  struct dirent *dp = NULL;

  // open directory
  DIR *dirp = opendir( path.c_str() );
  if( dirp != NULL )
  {
    // start a loop; in each iteration another directory entry is determined.
#if defined(_REENTRANT) && !defined(_WIN32) && !defined(__CYGWIN__)
    unsigned char entryBuffer[sizeof(struct dirent) + _POSIX_PATH_MAX + 1];
#ifdef HAVE_OLD_READDIR_R
    for( dp = readdir_r( dirp, (struct dirent *)entryBuffer ) ; dp != NULL ; dp = readdir_r( dirp, (struct dirent *)entryBuffer ) )
#else
    for( int readResult = readdir_r( dirp, (struct dirent *)entryBuffer, &dp ) ; readResult == 0 && dp ; readResult = readdir_r( dirp, (struct dirent *)entryBuffer, &dp ) )
#endif
#else
    for( dp = readdir( dirp ) ; dp != NULL ; dp = readdir( dirp ) )
#endif
    {
      // if the current entry refers to a worklist file
      if( IsWorklistFile( dp->d_name ) )
      {
        // create a string that contains path and filename of the current worklist file.
        OFString subname( path );
        subname += PATH_SEPARATOR;
        subname += dp->d_name;

        // add string to the set of strings
        worklistFiles.Insert( subname );
      }
    }

    // close directory
    closedir( dirp );
  }
#endif

  // in case we are running in verbose mode, dump all worklist file information
  if( verboseMode )
  {
    DumpMessage("=============================");
    DumpMessage("Worklist Database Files:");
    if( worklistFiles.NumberOfElements() == 0 )
      DumpMessage("<no files found>");
    for( i=0 ; i<worklistFiles.NumberOfElements() ; i++ )
      DumpMessage( worklistFiles[i].c_str() );
    DumpMessage("=============================");
  }
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::IsWorklistFile( const char *fname )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the given filename refers to a worklist file,
//                i.e. has an extension of ".wl".
// Parameters   : fname - [in] The name of the file.
// Return Value : OFTrue  - The given filename refers to a worklist file.
//                OFFalse - The given filename does not refer to a worklist file.
{
  // If no filename is specified, return OFFalse
  if( fname == NULL )
    return( OFFalse );

  // look for an '.' in the filename
  char *s = (char *)strrchr( fname, '.' );

  // if there was no '.' return OFFalse
  if( s == NULL )
    return( OFFalse );

  // if the extension is ".wl" return OFTrue
  if( strcmp( s, ".wl" ) == 0 )
    return( OFTrue );

  // return OFFalse in all other cases
  return( OFFalse );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::DatasetMatchesSearchMask( DcmDataset *dataset, DcmDataset *searchMask )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue, if the matching key attribute values in the
//                dataset match the matching key attribute values in the search mask.
// Parameters   : dataset    - [in] The dataset which shall be checked.
//                searchMask - [in] The search mask.
// Return Value : OFTrue  - The dataset matches the search mask in the matching key attribute values.
//                OFFalse - The dataset does not match the search mask in the matching key attribute values.
{
  OFBool dateTimeMatchHasBeenPerformed = OFFalse;

  // initialize result variable
  OFBool matchFound = OFTrue;

  // determine matching key attribute values in the dataset
  const char **mkaValuesDataset = NULL;
  DetermineMatchingKeyAttributeValues( dataset, mkaValuesDataset );

  // determine matching key attribute values in the search mask
  const char **mkaValuesSearchMask = NULL;
  DetermineMatchingKeyAttributeValues( searchMask, mkaValuesSearchMask );

  // go through the arrays of matching key attribute values
  for( unsigned long i=0 ; i<NUMBER_OF_SUPPORTED_MATCHING_KEY_ATTRIBUTES && matchFound ; i++ )
  {
    // check if the current matching key attribute actually had a value in the search mask
    if( mkaValuesSearchMask[i] != NULL )
    {
      // if it did, check if the values from search mask and dataset match
      switch( i )
      {
        case 0:
          // matching key attribute is DCM_ScheduledStationAETitle (AE, 1-n)
          matchFound = ScheduledStationAETitlesMatch( mkaValuesDataset[0], mkaValuesSearchMask[0] );
          break;

        case 1:
        case 2:
          // matching key attributes are DCM_ScheduledProcedureStepStartDate (DA, 1)
          // and DCM_ScheduledProcedureStepStartTime (TM, 1)
          // only do something if a date time match has not yet been performed
          if( !dateTimeMatchHasBeenPerformed )
          {
            matchFound = ScheduledProcedureStepStartDateTimesMatch( mkaValuesDataset[1], mkaValuesDataset[2], mkaValuesSearchMask[1], mkaValuesSearchMask[2] );
            dateTimeMatchHasBeenPerformed = OFTrue;
          }
          break;

        case 3:
          // matching key attribute is DCM_Modality (CS, 1)
          matchFound = ModalitiesMatch( mkaValuesDataset[3], mkaValuesSearchMask[3] );
          break;

        case 4:
          // matching key attribute is DCM_ScheduledPerformingPhysiciansName (PN, 1)
          matchFound = ScheduledPerformingPhysiciansNamesMatch( mkaValuesDataset[4], mkaValuesSearchMask[4] );
          break;

        case 5:
          // matching key attribute is DCM_PatientsName (PN, 1)
          matchFound = PatientsNamesMatch( mkaValuesDataset[5], mkaValuesSearchMask[5] );
          break;

        case 6:
          // matching key attribute is DCM_PatientID (LO, 1)
          matchFound = PatientIdsMatch( mkaValuesDataset[6], mkaValuesSearchMask[6] );
          break;

        default:
          break;
      }
    }
  }

  // return result
  return( matchFound );
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::DetermineMatchingKeyAttributeValues( DcmDataset *dataset, const char **&matchingKeyAttrValues )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function determines the values of the matching key attributes in the given dataset.
// Parameters   : dataset               - [in] Dataset from which the values shall be extracted.
//                matchingKeyAttrValues - [out] Contains in the end the values of the matching key
//                                        attributes in the search mask. Is an array of pointers.
// Return Value : none.
{
  // Initialize array of strings because all currently
  // supported matching key attributes are strings
  matchingKeyAttrValues = new const char*[ NUMBER_OF_SUPPORTED_MATCHING_KEY_ATTRIBUTES ];

  // find matching key attributes in the dataset and remember their values.
  for( unsigned long i=0 ; i<NUMBER_OF_SUPPORTED_MATCHING_KEY_ATTRIBUTES ; i++ )
  {
    // initialize array field
    matchingKeyAttrValues[i] = NULL;

    // determine which matching key attribute we want to find
    DcmTagKey tag;
    switch( i )
    {
      case 0 : tag = DCM_ScheduledStationAETitle           ; break;
      case 1 : tag = DCM_ScheduledProcedureStepStartDate   ; break;
      case 2 : tag = DCM_ScheduledProcedureStepStartTime   ; break;
      case 3 : tag = DCM_Modality                          ; break;
      case 4 : tag = DCM_ScheduledPerformingPhysiciansName ; break;
      case 5 : tag = DCM_PatientsName                      ; break;
      case 6 : tag = DCM_PatientID                         ; break;
      default:                                               break;
    }

    // try to find matching key attribute in the dataset
    dataset->findAndGetString( tag, matchingKeyAttrValues[i], OFTrue );
  }
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::ScheduledStationAETitlesMatch( const char *datasetValue, const char *searchMaskValue )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values in
//                attribute scheduled station AE title match; otherwise OFFalse will be returned
// Parameters   : datasetValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // in case the dataset value equals NULL, there is no match; in case the dataset's value is given,
  // case sensitive single value matching shall be used for attribute scheduled station AE title
  if( datasetValue == NULL )
    return( OFFalse );
  else
    return( CaseSensitiveSingleValueMatch( datasetValue, searchMaskValue ) );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::ScheduledProcedureStepStartDateTimesMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values in
//                attributes scheduled procedure step start date and scheduled procedure step
//                start time match; otherwise OFFalse will be returned
// Parameters   : datasetDateValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                datasetTimeValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskDateValue - [in] Value for the corresponding attribute in the search mask; might be NULL.
//                searchMaskTimeValue - [in] Value for the corresponding attribute in the search mask; might be NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  OFBool isMatch = OFFalse;

  // four things are of interest before matching
  OFBool dateGiven = ( searchMaskDateValue != NULL ) ? OFTrue : OFFalse;
  OFBool timeGiven = ( searchMaskTimeValue != NULL ) ? OFTrue : OFFalse;
  OFBool dateIsDateRange = ( dateGiven && strchr( searchMaskDateValue, '-' ) != NULL ) ? OFTrue : OFFalse;
  OFBool timeIsTimeRange = ( timeGiven && strchr( searchMaskTimeValue, '-' ) != NULL ) ? OFTrue : OFFalse;

  // depending on these four things perform different kinds of matching
  if( dateIsDateRange && timeIsTimeRange )
  {
    // perform range matching taking into account date and time
    isMatch = DateTimeRangeMatch( datasetDateValue, datasetTimeValue, searchMaskDateValue, searchMaskTimeValue );
  }
  else if( dateIsDateRange && !timeIsTimeRange )
  {
    // perform range matching taking into account date only
    isMatch = DateRangeMatch( datasetDateValue, searchMaskDateValue );
  }
  else if( dateGiven && !dateIsDateRange && timeIsTimeRange )
  {
    // perform a date single value match and a time range match
    isMatch = DateSingleValueMatch( datasetDateValue, searchMaskDateValue ) && TimeRangeMatch( datasetTimeValue, searchMaskTimeValue );
  }
  else if( !dateGiven && timeIsTimeRange )
  {
    // perform range matching taking into account time only
    isMatch = TimeRangeMatch( datasetTimeValue, searchMaskTimeValue );
  }
  else if( dateGiven && !dateIsDateRange && timeGiven && !timeIsTimeRange )
  {
    // perform single value matching taking into account date and time
    isMatch = DateTimeSingleValueMatch( datasetDateValue, datasetTimeValue, searchMaskDateValue, searchMaskTimeValue );
  }
  else if( dateGiven && !dateIsDateRange && !timeGiven )
  {
    // perform single value matching taking into account date only
    isMatch = DateSingleValueMatch( datasetDateValue, searchMaskDateValue );
  }
  else if( !dateGiven && timeGiven && !timeIsTimeRange )
  {
    // perform single value matching taking into account time only
    isMatch = TimeSingleValueMatch( datasetTimeValue, searchMaskTimeValue );
  }
  else
  {
    // return OFTrue
    isMatch = OFTrue;
  }

  // return result
  return( isMatch );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::ModalitiesMatch( const char *datasetValue, const char *searchMaskValue )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values in
//                attribute modality match; otherwise OFFalse will be returned
// Parameters   : datasetValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // in case the dataset value equals NULL, there is no match; in case the dataset's value
  // is given, case sensitive single value matching shall be used for attribute modality
  if( datasetValue == NULL )
    return( OFFalse );
  else
    return( CaseSensitiveSingleValueMatch( datasetValue, searchMaskValue ) );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::ScheduledPerformingPhysiciansNamesMatch( const char *datasetValue, const char *searchMaskValue )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values in
//                attribute scheduled performing physician's names match; otherwise OFFalse will be returned
// Parameters   : datasetValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // copy search mask value
  char *sv = new char[ strlen( searchMaskValue ) + 1 ];
  strcpy( sv, searchMaskValue );

  // strip trailing spaces
  sv = DU_stripTrailingSpaces( sv );

  // in case the search mask's value equals "*", there is a match, regardless of the
  // dataset's value; in case the search mask's value does not equal "*", and the
  // dataset's value equals NULL, there is no match; in all other cases we have to
  // perform (case sensitive) single value matching and possibly wildcard matching;
  // note that we specify that the performed single value matching is case-sensitive
  // this decision has to be mentioned in the conformance statement of this application
  if( strlen( sv ) == 1 && sv[0] == '*' )
    return( OFTrue );
  else if( datasetValue == NULL )
    return( OFFalse );
  else
  {
    if( CaseSensitiveSingleValueMatch( datasetValue, searchMaskValue ) )
      return( OFTrue );

    return( WildcardMatch( datasetValue, searchMaskValue ) );
  }
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::PatientsNamesMatch( const char *datasetValue, const char *searchMaskValue )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values in
//                attribute patient's names match; otherwise OFFalse will be returned
// Parameters   : datasetValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // copy search mask value
  char *sv = new char[ strlen( searchMaskValue ) + 1 ];
  strcpy( sv, searchMaskValue );

  // strip trailing spaces
  sv = DU_stripTrailingSpaces( sv );

  // in case the search mask's value equals "*", there is a match, regardless of the
  // dataset's value; in case the search mask's value does not equal "*", and the
  // dataset's value equals NULL, there is no match; in all other cases we have to
  // perform (case sensitive) single value matching and possibly wildcard matching;
  // note that we specify that the performed single value matching is case-sensitive
  // this decision has to be mentioned in the conformance statement of this application
  if( strlen( sv ) == 1 && sv[0] == '*' )
    return( OFTrue );
  else if( datasetValue == NULL )
    return( OFFalse );
  else
  {
    if( CaseSensitiveSingleValueMatch( datasetValue, searchMaskValue ) )
      return( OFTrue );

    return( WildcardMatch( datasetValue, searchMaskValue ) );
  }
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::PatientIdsMatch( const char *datasetValue, const char *searchMaskValue )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values in
//                attribute patient id match; otherwise OFFalse will be returned
// Parameters   : datasetValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // in case the dataset value equals NULL, there is no match; in case the dataset's value
  // is given, case sensitive single value matching shall be used for attribute patient id
  if( datasetValue == NULL )
    return( OFFalse );
  else
    return( CaseSensitiveSingleValueMatch( datasetValue, searchMaskValue ) );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::DateTimeRangeMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function performs a date time range match and returns OFTrue if the dataset's
//                and the search mask's values in the corresponding attributes match; otherwise OFFalse
//                will be returned
// Parameters   : datasetDateValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                datasetTimeValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskDateValue - [in] Value for the corresponding attribute in the search mask; never NULL.
//                searchMaskTimeValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  char *sdvv1 = NULL, *sdvv2 = NULL, *stvv1 = NULL, *stvv2 = NULL;
  OFDate datasetDateVal, searchMaskDateVal1, searchMaskDateVal2;
  OFTime datasetTimeVal, searchMaskTimeVal1, searchMaskTimeVal2;
  OFCondition cond;

  // in case parameters equal NULL it is considered to be no match
  if( datasetDateValue == NULL || datasetTimeValue == NULL )
    return( OFFalse );

  // initialize result
  OFBool isMatch = OFFalse;

  // copy values
  char *ddv = new char[ strlen( datasetDateValue ) + 1 ];     strcpy( ddv, datasetDateValue );
  char *dtv = new char[ strlen( datasetTimeValue ) + 1 ];     strcpy( dtv, datasetTimeValue );
  char *sdv = new char[ strlen( searchMaskDateValue ) + 1 ];  strcpy( sdv, searchMaskDateValue );
  char *stv = new char[ strlen( searchMaskTimeValue ) + 1 ];  strcpy( stv, searchMaskTimeValue );

  // strip trailing spaces
  ddv = DU_stripTrailingSpaces( ddv );
  dtv = DU_stripTrailingSpaces( dtv );
  sdv = DU_stripTrailingSpaces( sdv );
  stv = DU_stripTrailingSpaces( stv );

  // get actual date/time boundary values
  ExtractValuesFromRange( sdv, sdvv1, sdvv2 );
  ExtractValuesFromRange( stv, stvv1, stvv2 );

  // generate OFDate and OFTime objects from strings
  cond = DcmDate::getOFDateFromString( OFString( ddv ), datasetDateVal );
  if( cond.good() )
  {
    cond = DcmTime::getOFTimeFromString( OFString( dtv ), datasetTimeVal );
    if( cond.good() )
    {
      cond = DcmDate::getOFDateFromString( ( sdvv1 != NULL ) ? OFString( sdvv1 ) : OFString("19000101"), searchMaskDateVal1 );
      if( cond.good() )
      {
        cond = DcmDate::getOFDateFromString( ( sdvv2 != NULL ) ? OFString( sdvv2 ) : OFString("39991231"), searchMaskDateVal2 );
        if( cond.good() )
        {
          cond = DcmTime::getOFTimeFromString( ( stvv1 != NULL ) ? OFString( stvv1 ) : OFString("000000"), searchMaskTimeVal1 );
          if( cond.good() )
          {
            cond = DcmTime::getOFTimeFromString( ( stvv2 != NULL ) ? OFString( stvv2 ) : OFString("235959"), searchMaskTimeVal2 );
            if( cond.good() )
            {
              // now that we have the date and time objects we can actually
              // compare the date and time values and determine if it's a match

              // check if the lower value in the search mask is earlier
              // than or equal to the lower value in the dataset
              if( searchMaskDateVal1 < datasetDateVal ||
                  ( searchMaskDateVal1 == datasetDateVal && searchMaskTimeVal1 <= datasetTimeVal ) )
              {
                // now check if the upper value in the search mask is later
                // than or equal to the upper value in the dataset
                if( searchMaskDateVal2 > datasetDateVal ||
                    ( searchMaskDateVal2 == datasetDateVal && searchMaskTimeVal2 >= datasetTimeVal ) )
                {
                  // if all these conditions are met, then this is considered to be a match
                  isMatch = OFTrue;
                }
              }
            }
          }
        }
      }
    }
  }

  // free memory
  delete ddv;
  delete dtv;
  delete sdv;
  delete stv;
  if( sdvv1 != NULL ) delete sdvv1;
  if( sdvv2 != NULL ) delete sdvv2;
  if( stvv1 != NULL ) delete stvv1;
  if( stvv2 != NULL ) delete stvv2;

  // return result
  return( isMatch );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::DateRangeMatch( const char *datasetDateValue, const char *searchMaskDateValue )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function performs a date range match and returns OFTrue if the dataset's and
//                the search mask's values in the corresponding attributes match; otherwise OFFalse
//                will be returned
// Parameters   : datasetDateValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskDateValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  char *sdvv1 = NULL, *sdvv2 = NULL;
  OFDate datasetDateVal, searchMaskDateVal1, searchMaskDateVal2;
  OFCondition cond;

  // in case parameter equals NULL it is considered to be no match
  if( datasetDateValue == NULL )
    return( OFFalse );

  // initialize result
  OFBool isMatch = OFFalse;

  // copy values
  char *ddv = new char[ strlen( datasetDateValue ) + 1 ];     strcpy( ddv, datasetDateValue );
  char *sdv = new char[ strlen( searchMaskDateValue ) + 1 ];  strcpy( sdv, searchMaskDateValue );

  // strip trailing spaces
  ddv = DU_stripTrailingSpaces( ddv );
  sdv = DU_stripTrailingSpaces( sdv );

  // get actual date boundary values
  ExtractValuesFromRange( sdv, sdvv1, sdvv2 );

  // generate OFDate objects from strings
  cond = DcmDate::getOFDateFromString( OFString( ddv ), datasetDateVal );
  if( cond.good() )
  {
    cond = DcmDate::getOFDateFromString( ( sdvv1 != NULL ) ? OFString( sdvv1 ) : OFString("19000101"), searchMaskDateVal1 );
    if( cond.good() )
    {
      cond = DcmDate::getOFDateFromString( ( sdvv2 != NULL ) ? OFString( sdvv2 ) : OFString("39991231"), searchMaskDateVal2 );
      if( cond.good() )
      {
        // now that we have the date objects we can actually
        // compare the date values and determine if it's a match

        // check if the lower value in the search mask is earlier
        // than or equal to the lower value in the dataset
        if( searchMaskDateVal1 <= datasetDateVal )
        {
          // now check if the upper value in the search mask is later
          // than or equal to the upper value in the dataset
          if( searchMaskDateVal2 >= datasetDateVal )
          {
            // if these conditions are met, then this is considered to be a match
            isMatch = OFTrue;
          }
        }
      }
    }
  }

  // free memory
  delete ddv;
  delete sdv;
  if( sdvv1 != NULL ) delete sdvv1;
  if( sdvv2 != NULL ) delete sdvv2;

  // return result
  return( isMatch );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::TimeRangeMatch( const char *datasetTimeValue, const char *searchMaskTimeValue )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function performs a time range match and returns OFTrue if the dataset's and
//                the search mask's values in the corresponding attributes match; otherwise OFFalse
//                will be returned
// Parameters   : datasetTimeValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskTimeValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  char *stvv1 = NULL, *stvv2 = NULL;
  OFTime datasetTimeVal, searchMaskTimeVal1, searchMaskTimeVal2;
  OFCondition cond;

  // in case parameter equals NULL it is considered to be no match
  if( datasetTimeValue == NULL )
    return( OFFalse );

  // initialize result
  OFBool isMatch = OFFalse;

  // copy values
  char *dtv = new char[ strlen( datasetTimeValue ) + 1 ];     strcpy( dtv, datasetTimeValue );
  char *stv = new char[ strlen( searchMaskTimeValue ) + 1 ];  strcpy( stv, searchMaskTimeValue );

  // strip trailing spaces
  dtv = DU_stripTrailingSpaces( dtv );
  stv = DU_stripTrailingSpaces( stv );

  // get actual time boundary values
  ExtractValuesFromRange( stv, stvv1, stvv2 );

  // generate OFTime objects from strings
  cond = DcmTime::getOFTimeFromString( OFString( dtv ), datasetTimeVal );
  if( cond.good() )
  {
    cond = DcmTime::getOFTimeFromString( ( stvv1 != NULL ) ? OFString( stvv1 ) : OFString("000000"), searchMaskTimeVal1 );
    if( cond.good() )
    {
      cond = DcmTime::getOFTimeFromString( ( stvv2 != NULL ) ? OFString( stvv2 ) : OFString("235959"), searchMaskTimeVal2 );
      if( cond.good() )
      {
        // now that we have the time objects we can actually
        // compare the time values and determine if it's a match

        // check if the lower value in the search mask is earlier
        // than or equal to the lower value in the dataset
        if( searchMaskTimeVal1 <= datasetTimeVal )
        {
          // now check if the upper value in the search mask is later
          // than or equal to the upper value in the dataset
          if( searchMaskTimeVal2 >= datasetTimeVal )
          {
            // if these conditions are met, then this is considered to be a match
            isMatch = OFTrue;
          }
        }
      }
    }
  }

  // free memory
  delete dtv;
  delete stv;
  if( stvv1 != NULL ) delete stvv1;
  if( stvv2 != NULL ) delete stvv2;

  // return result
  return( isMatch );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::DateTimeSingleValueMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function performs a date time single value match and returns OFTrue if the dataset's
//                and the search mask's values in the corresponding attributes match; otherwise OFFalse
//                will be returned
// Parameters   : datasetDateValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                datasetTimeValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskDateValue - [in] Value for the corresponding attribute in the search mask; never NULL.
//                searchMaskTimeValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  OFDate datasetDateVal, searchMaskDateVal;
  OFTime datasetTimeVal, searchMaskTimeVal;
  OFCondition cond;

  // in case parameters equal NULL it is considered to be no match
  if( datasetDateValue == NULL || datasetTimeValue == NULL )
    return( OFFalse );

  // initialize result
  OFBool isMatch = OFFalse;

  // copy values
  char *ddv = new char[ strlen( datasetDateValue ) + 1 ];     strcpy( ddv, datasetDateValue );
  char *dtv = new char[ strlen( datasetTimeValue ) + 1 ];     strcpy( dtv, datasetTimeValue );
  char *sdv = new char[ strlen( searchMaskDateValue ) + 1 ];  strcpy( sdv, searchMaskDateValue );
  char *stv = new char[ strlen( searchMaskTimeValue ) + 1 ];  strcpy( stv, searchMaskTimeValue );

  // strip trailing spaces
  ddv = DU_stripTrailingSpaces( ddv );
  dtv = DU_stripTrailingSpaces( dtv );
  sdv = DU_stripTrailingSpaces( sdv );
  stv = DU_stripTrailingSpaces( stv );

  // generate OFDate and OFTime objects from strings
  cond = DcmDate::getOFDateFromString( OFString( ddv ), datasetDateVal );
  if( cond.good() )
  {
    cond = DcmTime::getOFTimeFromString( OFString( dtv ), datasetTimeVal );
    if( cond.good() )
    {
      cond = DcmDate::getOFDateFromString( OFString( sdv ), searchMaskDateVal );
      if( cond.good() )
      {
        cond = DcmTime::getOFTimeFromString( OFString( stv ), searchMaskTimeVal );
        if( cond.good() )
        {
          // now that we have the date and time objects we can actually
          // compare the date and time values and determine if it's a match

          // check if the date value in the search mask equals the date value in the dataset
          if( searchMaskDateVal == datasetDateVal )
          {
            // now check if the time value in the search mask equals the time value in the dataset
            if( searchMaskTimeVal == datasetTimeVal )
            {
              // if these conditions are met, then this is considered to be a match
              isMatch = OFTrue;
            }
          }
        }
      }
    }
  }

  // free memory
  delete ddv;
  delete dtv;
  delete sdv;
  delete stv;

  // return result
  return( isMatch );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::DateSingleValueMatch( const char *datasetDateValue, const char *searchMaskDateValue )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function performs a date single value match and returns OFTrue if the dataset's
//                and the search mask's values in the corresponding attributes match; otherwise OFFalse
//                will be returned
// Parameters   : datasetDateValue    - [in] Value for the corresponding attribute in the dataset; might be NULL.
//                searchMaskDateValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  OFDate datasetDateVal, searchMaskDateVal;
  OFCondition cond;

  // in case parameter equals NULL it is considered to be no match
  if( datasetDateValue == NULL )
    return( OFFalse );

  // initialize result
  OFBool isMatch = OFFalse;

  // copy values
  char *ddv = new char[ strlen( datasetDateValue ) + 1 ];     strcpy( ddv, datasetDateValue );
  char *sdv = new char[ strlen( searchMaskDateValue ) + 1 ];  strcpy( sdv, searchMaskDateValue );

  // strip trailing spaces
  ddv = DU_stripTrailingSpaces( ddv );
  sdv = DU_stripTrailingSpaces( sdv );

  // generate OFDate objects from strings
  cond = DcmDate::getOFDateFromString( OFString( ddv ), datasetDateVal );
  if( cond.good() )
  {
    cond = DcmDate::getOFDateFromString( OFString( sdv ), searchMaskDateVal );
    if( cond.good() )
    {
      // now that we have the date objects we can actually
      // compare the date values and determine if it's a match

      // check if the date value in the search mask equals the date value in the dataset
      if( searchMaskDateVal == datasetDateVal )
      {
        // if this condition is met, then this is considered to be a match
        isMatch = OFTrue;
      }
    }
  }

  // free memory
  delete ddv;
  delete sdv;

  // return result
  return( isMatch );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::TimeSingleValueMatch( const char *datasetTimeValue, const char *searchMaskTimeValue )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function performs a time single value match and returns OFTrue if the dataset's
//                and the search mask's values in the corresponding attributes match; otherwise OFFalse
//                will be returned
// Parameters   : datasetTimeValue    - [in] Value for the corresponding attribute in the dataset; never NULL.
//                searchMaskTimeValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  OFTime datasetTimeVal, searchMaskTimeVal;
  OFCondition cond;

  // in case parameter equals NULL it is considered to be no match
  if( datasetTimeValue == NULL )
    return( OFFalse );

  // initialize result
  OFBool isMatch = OFFalse;

  // copy values
  char *dtv = new char[ strlen( datasetTimeValue ) + 1 ];     strcpy( dtv, datasetTimeValue );
  char *stv = new char[ strlen( searchMaskTimeValue ) + 1 ];  strcpy( stv, searchMaskTimeValue );

  // strip trailing spaces
  dtv = DU_stripTrailingSpaces( dtv );
  stv = DU_stripTrailingSpaces( stv );

  // generate OFTime objects from strings
  cond = DcmTime::getOFTimeFromString( OFString( dtv ), datasetTimeVal );
  if( cond.good() )
  {
    cond = DcmTime::getOFTimeFromString( OFString( stv ), searchMaskTimeVal );
    if( cond.good() )
    {
      // now that we have the time objects we can actually
      // compare the time values and determine if it's a match

      // check if the time value in the search mask equals the time value in the dataset
      if( searchMaskTimeVal == datasetTimeVal )
      {
        // if this condition is met, then this is considered to be a match
        isMatch = OFTrue;
      }
    }
  }

  // free memory
  delete dtv;
  delete stv;

  // return result
  return( isMatch );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::CaseSensitiveSingleValueMatch( const char *datasetValue, const char *searchMaskValue )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values
//                match while performing a case sensitive single value match; otherwise OFFalse
//                will be returned
// Parameters   : datasetValue    - [in] Value for the corresponding attribute in the dataset; never NULL.
//                searchMaskValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // initialize result variable
  OFBool matchFound = OFTrue;

  // copy values
  char *dv = new char[ strlen( datasetValue ) + 1 ];
  strcpy( dv, datasetValue );
  char *sv = new char[ strlen( searchMaskValue ) + 1 ];
  strcpy( sv, searchMaskValue );

  // strip trailing spaces
  dv = DU_stripTrailingSpaces( dv );
  sv = DU_stripTrailingSpaces( sv );

  // perform match
  if( strcmp( dv, sv ) != 0 )
    matchFound = OFFalse;

  // free memory
  delete dv;
  delete sv;

  // return result
  return( matchFound );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::WildcardMatch( const char *datasetValue, const char *searchMaskValue )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if the dataset's and the search mask's values
//                match while performing a wildcard match; otherwise OFFalse will be returned.
// Parameters   : datasetValue    - [in] Value for the corresponding attribute in the dataset; never NULL.
//                searchMaskValue - [in] Value for the corresponding attribute in the search mask; never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // initialize return value
  OFBool ok = OFFalse;

  // copy values
  char *dv = new char[ strlen( datasetValue ) + 1 ];
  strcpy( dv, datasetValue );
  char *sv = new char[ strlen( searchMaskValue ) + 1 ];
  strcpy( sv, searchMaskValue );

  // remember the pointers for delete
  char *dvanchor = dv;
  char *svanchor = sv;

  // strip trailing spaces
  dv = DU_stripTrailingSpaces( dv );
  sv = DU_stripTrailingSpaces( sv );

  // go through both strings character by character as long as
  // a) we do not see an EOS in sv AND
  // b) we do not see a '*' in sv AND
  // c) the current character in dv is equal to the
  //    current character in the sv, or sv contains
  //    a '?' and sv does not yet show an EOS.
  while( *sv != '\0' &&
         *sv != '*'  &&
         ( *sv == *dv || ( *sv == '?' && *dv != '\0' ) ) )
  {
    sv++;
    dv++;
  }

  // if the current pattern character equals the star symbol (wild card symbol) we need
  // to call another function. If this is not the case, ok will be set to OFTrue or
  // OFFalse, depending on if we got to the end of both strings in the above loop.
  if( *sv == '*' )
    ok = MatchStarSymbol( dv, sv+1 );
  else
  {
    if( *sv == '\0' && *dv == '\0' )
      ok = OFTrue;
    else
      ok = OFFalse;
  }

  // free memory
  delete dvanchor;
  delete svanchor;

  // return result
  return( ok );
}

// ----------------------------------------------------------------------------

OFBool WlmFileSystemInteractionManager::MatchStarSymbol( const char *dv, const char *sv )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function is called, if the search pattern contains a star symbol. It determines
//                if dv (the dataset's value) still matches sv (the search mask's value). This function
//                takes the star symbol in sv into account. (Note that the pattern value might contain
//                more wild card symbols.) The function will return OFTrue if there is a match; if there
//                is not a match it will return OFFalse.
// Parameters   : dv - [in] Dataset's value; never NULL.
//                sv - [in] Search mask's value (may contain wild card symbols); never NULL.
// Return Value : OFTrue if the values match, OFFalse otherwise.
{
  // initialize result value
  OFBool ok = OFFalse;

  // move pointer one char to the right as long as it points to a star symbol
  while( *sv == '*' )
    sv++;

  // if we got to the end of sv, return OFTrue
  if( *sv == '\0' )
    return OFTrue;

  // if there is something else at the end of sv,
  // we need to go ahead and compare the rest of the two strings

  // as long as ok equals OFFalse and we did not get to the end of the string
  while( !ok && *dv != '\0' )
  {
    // if sv reveals a '?' or if both pointers refer to the same
    // character, we need to call WildcardMatch again, to determine a result
    if( *sv == '?' || *dv == *sv )
      ok = WildcardMatch( dv+1, sv+1 );

    // if ok still equals OFFalse, set pointer one character to the right
    if( !ok )
      dv++;
  }

  // return result
  return( ok );
}

// ----------------------------------------------------------------------------

void WlmFileSystemInteractionManager::ExtractValuesFromRange( const char *range, char *&lower, char *&upper )
// Date         : July 15, 2002
// Author       : Thomas Wilkens
// Task         : This function extracts the actual lower and upper date or time values from a given
//                date or time range.
// Parameters   : range - [in] Date or time range from which lower and upper values shall be extracted.
//                lower - [out] Newly created string specifying the lower value from the date/time range;
//                        NULL if value is not specified in range.
//                upper - [out] Newly created string specifying the upper value from the date/time range;
//                        NULL if value is not specified in range.
// Return Value : none.
{
  // get lower value
  char *tmp = strchr( range, '-' );
  int res = tmp - range;
  if( res == 0 )
    lower = NULL;
  else
  {
    lower = new char[ res + 1 ];
    strncpy( lower, range, res );
    lower[res] = '\0';
  }

  // get upper value
  int len = strlen( range );
  if( res == len - 1 )
    upper = NULL;
  else
  {
    upper = new char[ len - 1 - res + 1 ];
    strncpy( upper, tmp + 1, len - 1 - res );
    upper[len-1-res] = '\0';
  }
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wlfsim.cc,v $
** Revision 1.5  2002-12-09 13:42:22  joergr
** Renamed parameter to avoid name clash with global function index().
**
** Revision 1.4  2002/11/28 11:04:47  meichel
** Adapted module dcmwlm to use of new header file ofstdinc.h
**
** Revision 1.3  2002/08/22 09:14:04  wilkens
** Adapted code to new loadFile and saveFile methods, thus removing direct
** use of the DICOM stream classes.
**
** Revision 1.2  2002/08/12 10:56:17  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.1  2002/08/05 09:10:12  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.1  2002/07/17 13:10:20  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
**
**
*/
