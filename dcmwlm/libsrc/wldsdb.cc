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
 *  Purpose: Class for connecting to a database-based data source.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 16:32:47 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/Attic/wldsdb.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"
#include "dicom.h"
#include "wltypdef.h"
#include "oftypes.h"
#include "dcdatset.h"
#include "dcdeftag.h"
#include "dcsequen.h"
#include "dcvrat.h"
#include "dcvrlo.h"
#include "wldbim.h"
#include "wlds.h"
#include "ofconsol.h"

#include "wldsdb.h"

// ----------------------------------------------------------------------------

WlmDataSourceDatabase::WlmDataSourceDatabase( OFConsole *logStreamv, char *dbDsnv, char *dbUserNamev, char *dbUserPasswordv, char *dbSchemav )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : logStreamv      - [in] Stream that can be used to dump information.
//                dbDsnv          - [in] The data source name of the database that shall be used.
//                dbUserNamev     - [in] The database user name that shall be used for querying information.
//                dbUserPasswordv - [in] The password that belongs to the database user name.
//                dbSchemav       - [in] The data schema that holds the tables which shall be queried.
// Return Value : none.
  : WlmDataSource( logStreamv ), dbDsn( NULL ), dbUserName( NULL ), dbUserPassword( NULL ), dbSchema( NULL ),
    databaseInteractionManager( NULL ), matchingDatasets( NULL ), numOfMatchingDatasets( 0 )
{
  // Check parameters
  if( dbDsnv == NULL || dbUserNamev == NULL || dbUserPasswordv == NULL || dbSchemav == NULL )
  {
    // Indicate that the initialization could not be performed properly
    objectStatus = WLM_STATUS_INIT_FAILED;
  }
  else
  {
    // Copy parameters into member variables
    dbDsn = new char[ strlen(dbDsnv) + 1 ];
    strcpy( dbDsn, dbDsnv );
    dbUserName = new char[ strlen(dbUserNamev) + 1 ];
    strcpy( dbUserName, dbUserNamev );
    dbUserPassword = new char[ strlen(dbUserPasswordv) + 1 ];
    strcpy( dbUserPassword, dbUserPasswordv );
    dbSchema = new char[ strlen(dbSchemav) + 1 ];
    strcpy( dbSchema, dbSchemav );

    // create an WlmDatabaseInteractionManager object
    databaseInteractionManager = new WlmDatabaseInteractionManager( logStream, dbDsn, dbUserName, dbUserPassword, dbSchema );
    if( !databaseInteractionManager->IsObjectStatusOk() )
      objectStatus = WLM_STATUS_INIT_FAILED;
  }
}

// ----------------------------------------------------------------------------

WlmDataSourceDatabase::~WlmDataSourceDatabase()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // free memory
  if( dbDsn != NULL ) delete dbDsn;
  if( dbUserName != NULL ) delete dbUserName;
  if( dbUserPassword != NULL ) delete dbUserPassword;
  if( dbSchema != NULL ) delete dbSchema;
  if( databaseInteractionManager != NULL ) delete databaseInteractionManager;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceDatabase::IsDataSourceAvailable()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Checks if the database-based data source is available.
// Parameters   : none.
// Return Value : OFTrue  - The database-based data source is available.
//                OFFalse - The database-based data source is not available.
{
  if( databaseInteractionManager != NULL && databaseInteractionManager->IsObjectStatusOk() )
    return( OFTrue );
  else
    return( OFFalse );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceDatabase::IsCalledApplicationEntityTitleSupported()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Checks if the called application entity title is supported. This function expects
//                that the called application entity title was made available for this instance through
//                WlmDataSource::SetCalledApplicationEntityTitle(). If this is not the case, OFFalse
//                will be returned.
// Parameters   : none.
// Return Value : OFTrue  - The called application entity title is supported.
//                OFFalse - The called application entity title is not supported or it is not given.
{
  // Check if calledApplicationEntityTitle does not have a valid value
  if( calledApplicationEntityTitle == NULL )
    return( OFFalse );
  else
    return( databaseInteractionManager->IsCalledApplicationEntityTitleSupported( calledApplicationEntityTitle ) );
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSourceDatabase::StartFindRequest( DcmDataset &findRequestIdentifiers )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function
// Parameters   : findRequestIdentifiers - [in] Contains the search mask.
// Return Value : A WlmDataSourceStatusType value denoting the following:
//                WLM_SUCCESS         - No matching records found.
//                WLM_PENDING         - Matching records found, all return keys supported by this
//                                      application.
//                WLM_PENDING_WARNING - Matching records found, not all return keys supported by this
//                                      application.
//                WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS - Error in the search mask encountered.
{
  unsigned long i, j;
  char msg[200];

  // Initialize offending elements, error elements and error comment.
  delete offendingElements;
  delete errorElements;
  delete errorComment;
  offendingElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorComment = new DcmLongString( DCM_ErrorComment, 0 );

  // Initialize member variable identifiers; this variable will contain the search mask.
  ClearDataset( identifiers );
  delete identifiers;
  identifiers = new DcmDataset( findRequestIdentifiers );

  // Remove group length and padding elements from the search mask.
  identifiers->computeGroupLengthAndPadding( EGL_withoutGL, EPD_withoutPadding );

  // Actually there should be no elements in array matchingDatasets. But just to make sure,
  // remove all elements in the array and the array itself, if the array is not NULL; note
  // that this variable will in the end contain all records (datasets) that match the search mask.
  if( matchingDatasets != NULL )
  {
    for( i=0 ; i<numOfMatchingDatasets ; i++ )
      delete matchingDatasets[i];
    delete matchingDatasets;
    matchingDatasets = NULL;
    numOfMatchingDatasets = 0;
  }

  // This member variable indicates if we encountered an unsupported
  // optional key attribute in the search mask; initialize it with false.
  // It might be updated whithin CheckIdentifiers().
  foundUnsupportedOptionalKey = OFFalse;

  // Scrutinize the search mask.
  if( !CheckIdentifiers( identifiers ) )
  {
    // In case we encountered an error in the search
    // mask, we may have to return to the caller
    if( failOnInvalidQuery )
      return( WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS );
  }

  // Set a read lock on the database tables which shall be read from.
  int error = SetReadlock();

  // Determine the values of the matching key attributes in the search mask. 
  char **matchingKeyAttrValues = NULL;
  unsigned long numOfMatchingKeyAttrValues = 0;
  DetermineMatchingKeyAttributeValues( matchingKeyAttrValues, numOfMatchingKeyAttrValues );

  if( verbose )
    DumpMessage( "Determining matching database records." );

  // Determine the ids of the database records that match the search mask
  long *matchingRecordIDs = NULL;
  unsigned long numOfmatchingRecordIDs = 0;
  databaseInteractionManager->GetMatchingRecordIDs( matchingKeyAttrValues, numOfMatchingKeyAttrValues, matchingRecordIDs, numOfmatchingRecordIDs );

  // free memory
  delete matchingKeyAttrValues;

  // dump some information if required
  if( verbose )
  {
    sprintf( msg, "Matching results: %d matching records found in database.", numOfmatchingRecordIDs );
    DumpMessage( msg );
  }

  // determine a correct return value. In case no matching records
  // were found, WLM_SUCCESS shall be returned. This is our assumption.
  WlmDataSourceStatusType status = WLM_SUCCESS;

  // Check if matching records were found in the database.
  // If that is the case, do the following:
  if( numOfmatchingRecordIDs != 0 )
  {
    // create a container array that captures all result DcmDataset variables
    numOfMatchingDatasets = numOfmatchingRecordIDs;
    matchingDatasets = new DcmDataset*[ numOfMatchingDatasets ];

    // for each matching record do the following
    for( i=0 ; i<numOfmatchingRecordIDs ; i++ )
    {
      // dump some information if required
      if( verbose )
      {
        sprintf( msg, "  Processing matching result no. %d.", i );
        DumpMessage( msg );
      }

      // copy the search mask into matchingDatasets[i]
      matchingDatasets[i] = new DcmDataset( *identifiers );

      // Determine the number of elements in matchingDatasets[i].
      unsigned long numOfElementsInDataset = matchingDatasets[i]->card();
      j = 0;

      // Go through all the elements in matchingDatasets[i].
      while( j < numOfElementsInDataset )
      {
        // Determine the current element.
        DcmElement *element = matchingDatasets[i]->getElement(j);

        // Depending on if the current element is a sequence or not, process this element.
        if( element->ident() != EVR_SQ )
          HandleNonSequenceElementInResultDataset( matchingDatasets[i], element, matchingRecordIDs[i] );
        else
          HandleSequenceElementInResultDataset( matchingDatasets[i], element, matchingRecordIDs[i] );

        // Depending on if the number of elements in the dataset has changed, the current element was
        // deleted in the above called function. In case the current element was deleted, we do not
        // need to increase counter j, we only need to update numOfElementsInDataset. If the current
        // element was not deleted, we need to increase j but don't have to update numOfElementsInDataset.
        unsigned long currentNumOfElementsInDataset = matchingDatasets[i]->card();
        if( currentNumOfElementsInDataset != numOfElementsInDataset )
          numOfElementsInDataset = currentNumOfElementsInDataset;
        else
          j++;
      }
    }

    // free memory
    delete matchingRecordIDs;

    // Determine a corresponding return value: If matching records were found, WLM_PENDING or
    // WLM_PENDING_WARNING shall be returned, depending on if an unsupported optional key was
    // found in the search mask or not.
    if( foundUnsupportedOptionalKey )
      status = WLM_PENDING_WARNING;
    else
      status = WLM_PENDING;
  }

  // Now all the resulting data sets are contained in the member array matchingDatasets.
  // The variable numOfMatchingDatasets specifies the number of array fields.

  return( status );
}

// ----------------------------------------------------------------------------

DcmDataset *WlmDataSourceDatabase::NextFindResponse( WlmDataSourceStatusType &rStatus )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function will return the next dataset that matches the given search mask, if
//                there is one more resulting dataset to return. In such a case, rstatus will be set
//                to WLM_PENDING or WLM_PENDING_WARNING, depending on if an unsupported key attribute
//                was encountered in the search mask or not. If there are no more datasets that match
//                the search mask, this function will return an empty dataset and WLM_SUCCESS in rstatus.
// Parameters   : rStatus - [out] A value of type WlmDataSourceStatusType that can be used to
//                          decide if there are still elements that have to be returned.
// Return Value : The next dataset that matches the given search mask, or an empty dataset if
//                there are no more matching datasets in the database.
{
  DcmDataset *resultDataset = NULL;

  // If there are no more datasets that can be returned, do the following
  if( numOfMatchingDatasets == 0 )
  {
    // Set the return status to WLM_SUCCESS and return an empty dataset.
    rStatus = WLM_SUCCESS;
    resultDataset = new DcmDataset();
  }
  else
  {
    // We want to return the last array element.
    resultDataset = matchingDatasets[ numOfMatchingDatasets - 1 ];

    // Forget the pointer to this dataset here.
    matchingDatasets[ numOfMatchingDatasets - 1 ] = NULL;
    numOfMatchingDatasets--;

    // If there are no more elements to return, delete the array itself.
    if( numOfMatchingDatasets == 0 )
    {
      delete matchingDatasets;
      matchingDatasets = NULL;
    }

    // Determine a return status.
    if( foundUnsupportedOptionalKey )
      rStatus = WLM_PENDING_WARNING;
    else
      rStatus = WLM_PENDING;
  }

  // return resulting dataset
  return( resultDataset );
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::HandleNonSequenceElementInResultDataset( DcmItem *matchingDataset, DcmElement *&element, long matchingRecordID )
// Date         : December 20, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain non-sequence element whithin
//                the structure of a certain result dataset. In detail, it will be checked if
//                the current element is a supported return key attribute; if this is the case
//                a value for this particular element with regard to the currently processed
//                matching record will be requested from the database, and this value will be
//                set in the element; if this is not the case, the entire element will be re-
//                moved from the dataset, since unsupported elements shall not be returned to
//                the caller.
// Parameters   : matchingDataset  - [inout] Pointer to the currently processed result dataset.
//                element          - [inout] Pointer to the currently processed element.
//                matchingRecordID - [in] Identifies the matching record in the database.
// Return Value : none.
{
  // Determine the current elements tag.
  DcmTagKey tag( element->getTag() );

  // if the current element is a supported return key attribute
  if( IsSupportedReturnKeyAttribute( tag, 0 ) )
  {
    // get value from database; note that all currently supported
    // return key attributes can be returned as strings. If the
    // set of supported return key attribut changes and a non-string
    // attribute is added, the following code has to change.
    char *value = NULL;
    databaseInteractionManager->GetAttributeValueForMatchingRecord( tag, matchingRecordID, value );

    // put value in element
    OFCondition cond = element->putString( value );
    if( cond.bad() )
      DumpMessage( "WlmDataSourceDatabase::HandleNonSequenceElementInResultDataset: Could not set value in result element.\n" );

    // free memory
    delete value;
  }
  else
  {
    // delete element from result dataset
    DcmElement *elem = matchingDataset->remove( element );
    delete elem;

    // dump a warning
    DumpMessage( "    - Unsupported optional (non-sequence) return key attribute found in the search mask.\n      This attribute will not be existent in the result dataset." );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::HandleSequenceElementInResultDataset( DcmItem *matchingDataset, DcmElement *&element, long matchingRecordID )
// Date         : December 20, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain sequence element whithin the
//                structure of a certain result dataset. In detail, it will be checked if
//                the current element is a supported return key attribute; if this is the case
//                a value for this particular element with regard to the currently processed
//                matching record will be requested from the database, and this value will be
//                set in the element; if this is not the case, the entire element will be re-
//                moved from the dataset, since unsupported elements shall not be returned to
//                the caller.
// Parameters   : matchingDataset  - [inout] Pointer to the currently processed result dataset.
//                element          - [inout] Pointer to the currently processed element.
//                matchingRecordID - [in] Identifies the matching record in the database.
// Return Value : none.
{
  // Determine the current elements length and tag.
  Uint32 length = element->getLength();
  DcmTagKey tag( element->getTag() );

  // if the current element is a supported return key attribute
  if( IsSupportedReturnKeyAttribute( tag, 0 ) )
  {
    // and if its length does not equal 0
    if( length != 0 )
    {
      // Consider this element as a sequence of items.
      DcmSequenceOfItems *sequenceOfItemsElement = (DcmSequenceOfItems*)element;

      // Get the sequence of items element's cardinality 
      unsigned long numOfItemsInSequence = sequenceOfItemsElement->card();

      // according to the 2001 DICOM standard, part 4, section C.2.2.2.6, a sequence in the search mask
      // (and we made a copy of the search mask that we update here, so that it represents a result value)
      // must have exactly one item which in turn can be empty. Hence, if numOfItemsInSequence does not
      // equal 1 here, we want to dump a warning and do nothing here.
      if( numOfItemsInSequence != 1 )
      {
        DumpMessage( "    - Non-empty sequence with not exactly one item encountered in the search mask.\n      The corresponding sequence of the currently processed result data set will show the exact same structure as in the given search mask." );
      }
      else
      {
        // ### wilkens: Im Moment sehen wir nicht vor, daß aus einem Sequenzitem in der Suchmaske im
        // ### resultierenden Dataset mehrere gemacht werden müssen, weil die Datenbank solche 1-m
        // ### Beziehungen in den momentan unterstützten Return Key Attributen nicht vorsieht. Dies
        // ### kann sich später ändern, dann muß an dieser Stelle bestimmt werden, wieviel Items die
        // ### resultierende Sequenz haben soll und es müssen entsprechend viele Items in die Sequenz
        // ### eingefügt und in ihren Attributen mit Werten gefüllt werden.

        // Determine the sequence of items element's one and only item.
        DcmItem *itemInSequence = sequenceOfItemsElement->getItem(0);
        
        // Get its cardinality.
        unsigned long numOfElementsInItem = itemInSequence->card();
        unsigned long k = 0;

        // Start a loop and go through all elements of this item.
        while( k < numOfElementsInItem )
        {
          // Get the current element.
          DcmElement *elementInItem = itemInSequence->getElement(k);

          // Depending on if the current element is a sequence or not, process this element.
          // (Note that through the recursive call to HandleSequenceElementInResultDataset()
          // sequences of arbitrary depth are supported.)
          if( elementInItem->ident() != EVR_SQ )
            HandleNonSequenceElementInResultDataset( itemInSequence, elementInItem, matchingRecordID );
          else
            HandleSequenceElementInResultDataset( itemInSequence, elementInItem, matchingRecordID );

          // Depending on if the number of elements in the item has changed, the current element was
          // deleted in the above called function. In case the current element was deleted, we do not
          // need to increase counter k, we only need to update numOfElementsInItem. If the current
          // element was not deleted, we need to increase k but don't have to update numOfElementsInItem.
          unsigned long currentNumOfElementsInItem = itemInSequence->card();
          if( currentNumOfElementsInItem != numOfElementsInItem )
            numOfElementsInItem = currentNumOfElementsInItem;
          else
            k++;
        }
      }
    }
    // according to the 2001 DICOM standard, part 4, section C.2.2.2.6, the length of a sequence in the
    // search mask (and we made a copy of the search mask that we update here, so that it represents a
    // result value) must never be 0, since every sequence is supposed to have exactly one item which
    // in turn can be empty. Hence, if we encounter an empty sequence, we want to dump a warning.
    else
    {
      DumpMessage( "    - Empty sequence in the search mask encountered.\n      The corresponding sequence will be left empty in the currently processed result data set." );
    }
  }
  else
  {
    // delete sequence element from result dataset
    DcmElement *elem = matchingDataset->remove( element );
    delete elem;

    // dump a warning
    DumpMessage( "    - Unsupported optional (sequence) return key attribute found in the search mask.\n      This attribute will not be existent in the currently processed result dataset." );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::DetermineMatchingKeyAttributeValues( char **&matchingKeyAttrValues, unsigned long &numOfMatchingKeyAttrValues )
// Date         : December 19, 2001
// Author       : Thomas Wilkens
// Task         : This function determines the values of the matching key attributes in the search mask.
// Parameters   : matchingKeyAttrValues      - [out] Contains in the end the values of the matching key
//                                                   attributes in the search mask. Is an array of pointers.
//                numOfMatchingKeyAttrValues - [out] The number of array elements in the above array.
// Return Value : none.
{
  // Currently, this application supports 7 (non-sequence) matching key attributes.
  // We want to try to find these 7 attributes in the search mask and remember their
  // values. Note that currently all matching key attributes are strings.
  numOfMatchingKeyAttrValues = 7;
  matchingKeyAttrValues = new char*[ numOfMatchingKeyAttrValues ];

  // Find matching key attributes in the search mask and remember their values.
  for( unsigned long i=0 ; i<numOfMatchingKeyAttrValues ; i++ )
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

    // try to find matching key attribute in the search mask
    identifiers->findAndGetString( tag, matchingKeyAttrValues[i], OFTrue );
  }
}

// ----------------------------------------------------------------------------

int WlmDataSourceDatabase::SetReadlock()
// Date         : December 12, 2001
// Author       : Thomas Wilkens
// Task         : This function sets a read lock on certain tables of the database.
// Parameters   : none.
// Return Value : 0 - The read lock has been set successfully.
//                1 - The read lock has not been set successfully.
{
  // ### not yet implemented
  return( 0 );
}

// ----------------------------------------------------------------------------

int WlmDataSourceDatabase::ReleaseReadlock()
// Date         : December 12, 2001
// Author       : Thomas Wilkens
// Task         : This function releases a read lock that was set on certain tables of the database.
// Parameters   : none.
// Return Value : 0 - The read lock has been released successfully.
//                1 - The read lock has not been released successfully.
{
  // ### not yet implemented
  return( 0 );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceDatabase::IsSupportedMatchingKeyAttribute( const DcmTagKey &key, int /* level */ )
// Date         : 1995
// Author       : Thomas Wilkens
// Task         : This function checks if the given tag refers to an attribute which is a supported
//                matching key attribute. If this is the case OFTrue is returned, else OFFalse.
//                Currently, the following attributes are supported matching key attributes:
//                 - DCM_ScheduledProcedureStepSequence
//                 - DCM_ScheduledStationAETitle
//                 - DCM_ScheduledProcedureStepStartDate
//                 - DCM_ScheduledProcedureStepStartTime
//                 - DCM_Modality
//                 - DCM_ScheduledPerformingPhysiciansName
//                 - DCM_PatientsName
//                 - DCM_PatientID
// Parameters   : key   - [in] The tag that shall be checked.
//                level - [in] !!not used!!
// Return Value : OFTrue  - The given tag is a supported matching key attribute.
//                OFFalse - The given tag is not a supported matching key attribute.
{
  // ### wilkens: siehe 2001er Standard, Teil 4, Tabelle K.6-1 . Alles, was in der Spalte Matching Key Type
  // ### eine Ausprägung 'R' hat, muß als Matching Key Attribut von einem SCP unterstützt werden. (Das heißt
  // ### nicht, daß dieses Attribut auch immer in einer Suchmaske auftritt, sondern lediglich, daß wenn es
  // ### auftritt, daß der SCP dann etwas damit anzufangen weiß, d.h. es gegen einen Wert in der DB matchen
  // ### kann!) Für diese Funktion bedeutet das, das für die in der Tabelle mit 'R' bezeichneten Attribute
  // ### OFTrue zurückgeliefert werden muß.

  // initialize result variable
  OFBool ok = OFFalse;

  // perform check
  if( key == DCM_ScheduledProcedureStepSequence    ||
      key == DCM_ScheduledStationAETitle           ||
      key == DCM_ScheduledProcedureStepStartDate   ||
      key == DCM_ScheduledProcedureStepStartTime   ||
      key == DCM_Modality                          ||
      key == DCM_ScheduledPerformingPhysiciansName ||
      key == DCM_PatientsName                      ||
      key == DCM_PatientID )
    ok = OFTrue;

  // return result
  return ok;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceDatabase::IsSupportedReturnKeyAttribute( const DcmTagKey &key, int /* level */ )
// Date         : 1995
// Author       : Thomas Wilkens
// Task         : This function checks if the given tag refers to an attribute which is a supported
//                return key attribute. If this is the case OFTrue is returned, else OFFalse.
//                Currently, the following attributes are supported return key attributes:
//                 - DCM_ScheduledProcedureStepSequence
//                 - DCM_ScheduledStationAETitle
//                 - DCM_ScheduledProcedureStepStartDate
//                 - DCM_ScheduledProcedureStepStartTime
//                 - DCM_Modality
//                 - DCM_ScheduledPerformingPhysiciansName
//                 - DCM_PatientsName
//                 - DCM_PatientID
//                 - DCM_ScheduledProcedureStepDescription
//                 - DCM_ScheduledStationName
//                 - DCM_ScheduledProcedureStepID
//                 - DCM_RequestedProcedureID
//                 - DCM_RequestedProcedureDescription
//                 - DCM_ReferencedStudySequence
//                 - DCM_AccessionNumber
//                 - DCM_ReferringPhysiciansName
//                 - DCM_ReferencedPatientSequence
//                 - DCM_PatientsBirthDate
//                 - DCM_PatientsSex
//                 - DCM_InstitutionName
//                 - DCM_StudyInstanceUID
// Parameters   : key   - [in] The tag that shall be checked.
//                level - [in] !!not used!!
// Return Value : OFTrue  - The given tag is a supported return key attribute.
//                OFFalse - The given tag is not a supported return key attribute.
{
  // ### wilkens: siehe 2001er Standard, Teil 4, Tabelle K.6-1 . Alles, was in der Spalte Return Key Type
  // ### eine Ausprägung 1 oder 2 hat, muß als Return Key Attribut von einem SCP unterstützt werden. (Für Attribute
  // ### mit 1C und 2C müssen genau geschaut werden, ob sie unterstützt werden müssen. Siehe Spalte Remark
  // ### in der Tabelle.) Für diese Funktion bedeutet das, das für die in der Tabelle mit 1, 2 (und evtl.1C, 2C)
  // ### bezeichneten Attribute OFTrue zurückgeliefert werden muß. Außerdem sind auch alle Matching Key
  // ### Attribute Return Key Attribute, also muß obige Funktion IsSupportedMatchingKeyAttribute() aufgerufen
  // ### werden.

  // Initialize result variable.
  OFBool ok = OFFalse;

  // perform check
  if( IsSupportedMatchingKeyAttribute( key, 0 )    ||
      key == DCM_ScheduledProcedureStepDescription ||
      key == DCM_ScheduledStationName              ||
      key == DCM_ScheduledProcedureStepID          ||
      key == DCM_RequestedProcedureID              ||
      key == DCM_RequestedProcedureDescription     ||
      key == DCM_ReferencedStudySequence           ||
      key == DCM_AccessionNumber                   ||
      key == DCM_ReferringPhysiciansName           ||
      key == DCM_ReferencedPatientSequence         ||
      key == DCM_PatientsBirthDate                 ||
      key == DCM_PatientsSex                       ||
      key == DCM_InstitutionName                   ||
      key == DCM_StudyInstanceUID )
    ok = OFTrue;

  // return result
  return ok;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


/*
** CVS Log
** $Log: wldsdb.cc,v $
** Revision 1.1  2002-01-08 16:32:47  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
