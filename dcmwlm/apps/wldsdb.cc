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
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-07-17 13:10:14 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldsdb.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
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
#include "dcvrcs.h"
#define OTL_ORA7
#include <otlv32.h>
#include "wldbim.h"
#include "wlds.h"
#include "ofconsol.h"

#include "wldsdb.h"

// ----------------------------------------------------------------------------

WlmDataSourceDatabase::WlmDataSourceDatabase()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : WlmDataSource(), databaseInteractionManager( NULL ), matchingDatasets( NULL ), numOfMatchingDatasets( 0 ),
    dbDsn( NULL ), dbUserName( NULL ), dbUserPassword( NULL ), cfgFileMatchRecords( NULL ),
    cfgFileSelectValues( NULL ), databaseType( DATABASE_TYPE_UNKNOWN ), serialNumber( 0 )
{
  databaseInteractionManager = new WlmDatabaseInteractionManager();
}

// ----------------------------------------------------------------------------

WlmDataSourceDatabase::~WlmDataSourceDatabase()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  //free memory
  delete databaseInteractionManager;
  if( dbDsn != NULL ) delete dbDsn;
  if( dbUserName != NULL ) delete dbUserName;
  if( dbUserPassword != NULL ) delete dbUserPassword;
  if( cfgFileMatchRecords != NULL ) delete cfgFileMatchRecords;
  if( cfgFileSelectValues != NULL ) delete cfgFileSelectValues;
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceDatabase::ConnectToDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Connects to the data source.
// Parameters   : none.
// Return Value : Indicates if the connection was established succesfully.
{
  // set variables in databaseInteractionManager object
  databaseInteractionManager->SetLogStream( logStream );
  databaseInteractionManager->SetVerbose( verbose );

  // connect to database
  OFCondition cond = databaseInteractionManager->ConnectToDatabase( dbDsn, dbUserName, dbUserPassword, serialNumber, cfgFileMatchRecords, cfgFileSelectValues, databaseType );

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceDatabase::DisconnectFromDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Disconnects from the data source.
// Parameters   : none.
// Return Value : Indicates if the disconnection was completed succesfully.
{
  // disconnect from database
  OFCondition cond = databaseInteractionManager->DisconnectFromDatabase();

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::SetDbDsn( const char *value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( dbDsn != NULL ) delete dbDsn;
    dbDsn = new char[ strlen(value) + 1 ];
    strcpy( dbDsn, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::SetDbUserName( const char *value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( dbUserName != NULL ) delete dbUserName;
    dbUserName = new char[ strlen(value) + 1 ];
    strcpy( dbUserName, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::SetDbUserPassword( const char *value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( dbUserPassword != NULL ) delete dbUserPassword;
    dbUserPassword = new char[ strlen(value) + 1 ];
    strcpy( dbUserPassword, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::SetSerialNumber( const int value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  serialNumber = value;
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::SetCfgFileMatchRecords( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( cfgFileMatchRecords != NULL ) delete cfgFileMatchRecords;
    cfgFileMatchRecords = new char[ strlen(value) + 1 ];
    strcpy( cfgFileMatchRecords, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::SetCfgFileSelectValues( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( cfgFileSelectValues != NULL ) delete cfgFileSelectValues;
    cfgFileSelectValues = new char[ strlen(value) + 1 ];
    strcpy( cfgFileSelectValues, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::SetDatabaseType( WlmDatabaseType value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  databaseType = value;
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
// Task         : Based on the search mask which was passed, this function determines all the records
//                in the database which match the values of matching key attributes in the search mask.
//                For each matching record, a DcmDataset structure is generated which will later be
//                returned to the SCU as a result of query. The DcmDataset structures for all matching
//                records will be stored in the protected member variable matchingDatasets.
// Parameters   : findRequestIdentifiers - [in] Contains the search mask.
// Return Value : A WlmDataSourceStatusType value denoting the following:
//                WLM_SUCCESS         - No matching records found.
//                WLM_PENDING         - Matching records found, all return keys supported by this
//                                      application.
//                WLM_PENDING_WARNING - Matching records found, not all return keys supported by this
//                                      application.
//                WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS - Error in the search mask encountered.
{
  DcmElement *element;
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
  // It might be updated whithin CheckSearchMask().
  foundUnsupportedOptionalKey = OFFalse;

  // Scrutinize the search mask.
  if( !CheckSearchMask( identifiers ) )
  {
    // In case we encountered an error in the search
    // mask, we may have to return to the caller
    if( failOnInvalidQuery )
      return( WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS );
  }

  // dump search mask (it might have been expanded)
  if( verbose && logStream != NULL )
  {
    logStream->lockCout();
    logStream->getCout() << "Expanded Find SCP Request Identifiers:" << endl;
    identifiers->print( logStream->getCout() );
    logStream->getCout() << "=============================" << endl;
    logStream->unlockCout();
  }

  // Set a read lock on the database tables which shall be read from.
  SetReadlock();

  // dump some information if required
  if( verbose )
    DumpMessage( "Determining matching database records." );

  // Determine the ids of the database records that match the search mask
  char **matchingRecordIDs = NULL;
  unsigned long numOfMatchingRecordIDs = 0;
  databaseInteractionManager->GetMatchingRecordIDs( identifiers, matchingRecordIDs, numOfMatchingRecordIDs );

  // dump some information if required
  if( verbose )
  {
    sprintf( msg, "Matching results: %d matching records found in database.", numOfMatchingRecordIDs );
    DumpMessage( msg );
  }

  // determine a correct return value. In case no matching records
  // were found, WLM_SUCCESS shall be returned. This is our assumption.
  WlmDataSourceStatusType status = WLM_SUCCESS;

  // Check if matching records were found in the database.
  // If that is the case, do the following:
  if( numOfMatchingRecordIDs != 0 )
  {
    // create a container array that captures all result DcmDataset variables
    numOfMatchingDatasets = numOfMatchingRecordIDs;
    matchingDatasets = new DcmDataset*[ numOfMatchingDatasets ];

    // for each matching record do the following
    for( i=0 ; i<numOfMatchingRecordIDs ; i++ )
    {
      // this variable is needed later, it must be initialized with NULL
      DcmElement *specificCharacterSetElement = NULL;

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

      // Go through all the elements in matchingDatasets[i].
      for( j=0 ; j < numOfElementsInDataset ; j++ )
      {
        // Determine the current element.
        element = matchingDatasets[i]->getElement(j);

        // Depending on if the current element is a sequence or not, process this element.
        if( element->ident() != EVR_SQ )
          HandleNonSequenceElementInResultDataset( element, matchingRecordIDs[i] );
        else
          HandleSequenceElementInResultDataset( element, matchingRecordIDs[i] );

        // in case the current element is the "Specific Character Set" attribute, remember this element for later
        if( element->getTag().getXTag() == DCM_SpecificCharacterSet )
          specificCharacterSetElement = element;
      }

      // after having created the entire returned data set, deal with the "Specific Character Set" attribute.
      // If it shall not be contained in the returned data set
      if( returnedCharacterSet == RETURN_NO_CHARACTER_SET )
      {
        // and it is already included, delete it
        if( specificCharacterSetElement != NULL )
        {
          DcmElement *elem = matchingDatasets[i]->remove( specificCharacterSetElement );
          delete elem;
        }
      }
      else
      {
        // if it shall be contained in the returned data set, check if it is not already included
        if( specificCharacterSetElement == NULL )
        {
          // if it is not included in the returned dataset, create a new element and insert it
          specificCharacterSetElement = new DcmCodeString( DcmTag( DCM_SpecificCharacterSet ) );
          if( matchingDatasets[i]->insert( specificCharacterSetElement ) != EC_Normal )
          {
            delete specificCharacterSetElement;
            specificCharacterSetElement = NULL;
            DumpMessage( "WlmDataSourceDatabase::StartFindRequest: Could not insert specific character set element into dataset.\n" );
          }
        }
        // and set the value of the attribute accordingly
        if( specificCharacterSetElement != NULL )
        {
          if( returnedCharacterSet == RETURN_CHARACTER_SET_ISO_IR_100 )
          {
            OFCondition cond = specificCharacterSetElement->putString( "ISO_IR 100" );
            if( cond.bad() )
              DumpMessage( "WlmDataSourceDatabase::StartFindRequest: Could not set value in result element.\n" );
          }
        }
      }
    }

    // free memory, reset variables
    for( unsigned int k=0 ; k<numOfMatchingRecordIDs ; k++ )
      delete matchingRecordIDs[k];
    delete matchingRecordIDs;
    matchingRecordIDs = NULL;
    numOfMatchingRecordIDs = 0;

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

  // Release the read lock which was set on the database tables.
  ReleaseReadlock();

  // return result
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

void WlmDataSourceDatabase::HandleNonSequenceElementInResultDataset( DcmElement *element, char *matchingRecordID )
// Date         : December 20, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain non-sequence element whithin
//                the structure of a certain result dataset. This function assumes that all
//                elements in the result dataset are supported. In detail, a value for the
//                current element with regard to the currently processed matching record will
//                be requested from the database, and this value will be set in the element.
// Parameters   : element          - [in] Pointer to the currently processed element.
//                matchingRecordID - [in] Identifies the matching record in the database.
// Return Value : none.
{
  OFCondition cond;

  // Determine the current elements tag.
  DcmTagKey tag( element->getTag() );

  // check if the current element is the "Specific Character Set" (0008,0005) attribute.
  // We do not want to deal with this attribute here, this attribute will be taken care
  // of when the entire result dataset is completed.
  if( tag != DCM_SpecificCharacterSet )
  {
    // in case the current element is not the "Specific Character Set" (0008,0005) attribute,
    // get a value for the current element from database; note that all values for return key
    // attributes are returned as strings by GetAttributeValueForMatchingRecord().
    char *value = NULL;
    databaseInteractionManager->GetAttributeValueForMatchingRecord( tag, matchingRecordID, value );

    // put value in element
    // Note that there is currently one attribute (DCM_PregnancyStatus) in which the value must not
    // be set as a string but as an unsigned integer, because this attribute is of type US. Hence,
    // in case we are dealing with the attribute DCM_PregnancyStatus, we have to convert the returned
    // value into an unsigned integer and set it correspondingly in the element variable.
    if( tag == DCM_PregnancyStatus )
    {
      Uint16 uintValue = atoi( value );
      cond = element->putUint16( uintValue );
    }
    else
      cond = element->putString( value );
    if( cond.bad() )
      DumpMessage( "WlmDataSourceDatabase::HandleNonSequenceElementInResultDataset: Could not set value in result element.\n" );

    // free memory
    delete value;
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceDatabase::HandleSequenceElementInResultDataset( DcmElement *element, char *matchingRecordID )
// Date         : December 20, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain sequence element whithin the
//                structure of a certain result dataset. This function assumes that all elements
//                in the result dataset are supported. It also assumes that there are no sequence
//                elements with length == 0 in the result dataset, and that each sequence element
//                contains one single non-empty item. In case there are more than one item in a
//                sequence element, the sequence element in the result data set will completely
//                be left unchanged. In detail, a value for the current element with regard to
//                the currently processed matching record will be requested from the database,
//                and this value will be set in the element.
// Parameters   : element          - [in] Pointer to the currently processed element.
//                matchingRecordID - [in] Identifies the matching record in the database.
// Return Value : none.
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
    DumpMessage( "    - Sequence with not exactly one item encountered in the search mask.\n      The corresponding sequence of the currently processed result data set will show the exact same structure as in the given search mask." );
  }
  else
  {
    // ### Note that at the moment we do not account for having more than one item in a sequence
    // ### in the result dataset, since we only support the sequences "Scheduled Procedure Step
    // ### Sequence" (which shall only have one item anyway), "Referenced Study Sequence"
    // ### and "Referenced Patient Sequence". According to the comment in the head of function
    // ### bla, the latter two are not fully supported in this implementation, because
    // ### they contain equally named attributes for which the same values will always be returned
    // ### by this SCP. We decided to choose this approach because the latter two sequences might
    // ### be removed from the standard in the near future anyhow. Also, if there is the need to
    // ### create additional sequence items within a sequence and set corresponding values in the
    // ### attributes of these sequences, the following code has to change.

    // Determine the sequence of items element's one and only item.
    DcmItem *itemInSequence = sequenceOfItemsElement->getItem(0);

    // Get its cardinality.
    unsigned long numOfElementsInItem = itemInSequence->card();

    // (note that it _is_ possible that a sequence item does not contain any attributes at this point:
    // if the original search mask contained a sequence with a single item which did only contain un-
    // supported attributes, all those unsupported attributes were removed from the item in CheckSearchMask(),
    // leaving the item empty. This is not significant though because the following for-loop does not affect
    // items with no attributes; the SCP will simply not return any values for such empty items.)

    // Start a loop and go through all elements of this item.
    for( unsigned long k=0 ; k<numOfElementsInItem ; k++ )
    {
      // Get the current element.
      DcmElement *elementInItem = itemInSequence->getElement(k);

      // Depending on if the current element is a sequence or not, process this element.
      // (Note that through the recursive call to HandleSequenceElementInResultDataset()
      // sequences of arbitrary depth are supported.)
      if( elementInItem->ident() != EVR_SQ )
        HandleNonSequenceElementInResultDataset( elementInItem, matchingRecordID );
      else
        HandleSequenceElementInResultDataset( elementInItem, matchingRecordID );
    }
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
  // at the moment we do not use locking at all
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
  // at the moment we do not use locking at all
  return( 0 );
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wldsdb.cc,v $
** Revision 1.4  2002-07-17 13:10:14  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.3  2002/05/08 13:20:36  wilkens
** Added new command line option -nse to wlmscpki and wlmscpdb.
**
** Revision 1.2  2002/04/18 14:19:50  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.4  2002/01/08 17:46:03  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.3  2002/01/08 17:31:23  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/
