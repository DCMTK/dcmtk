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
 *  Purpose: Class for connecting to a pki-file-based data source.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-07-17 13:10:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldspf.cc,v $
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
#include "wlpfim.h"
#include "wlds.h"
#include "ofconsol.h"

#include "wldspf.h"

// ----------------------------------------------------------------------------

WlmDataSourcePkiFile::WlmDataSourcePkiFile()
// Date         : March 18, 2002
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : WlmDataSource(), pkiFileInteractionManager( NULL ), matchingDatasets( NULL ), numOfMatchingDatasets( 0 ),
    pfFileName( NULL ), serialNumber( 0 )
{
  pkiFileInteractionManager = new WlmPkiFileInteractionManager();
}

// ----------------------------------------------------------------------------

WlmDataSourcePkiFile::~WlmDataSourcePkiFile()
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  //free memory
  delete pkiFileInteractionManager;
  if( pfFileName != NULL ) delete pfFileName;
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourcePkiFile::ConnectToDataSource()
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : Connects to the data source.
// Parameters   : none.
// Return Value : Indicates if the connection was established succesfully.
{
  // set variables in pkiFileInteractionManager object
  pkiFileInteractionManager->SetLogStream( logStream );
  pkiFileInteractionManager->SetVerbose( verbose );

  // connect to pki-file
  OFCondition cond = pkiFileInteractionManager->ConnectToPkiFile( pfFileName, serialNumber );

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourcePkiFile::DisconnectFromDataSource()
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : Disconnects from the data source.
// Parameters   : none.
// Return Value : Indicates if the disconnection was completed succesfully.
{
  // disconnect from pki-file
  OFCondition cond = pkiFileInteractionManager->DisconnectFromPkiFile();

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

void WlmDataSourcePkiFile::SetPfFileName( const char *value )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( pfFileName != NULL ) delete pfFileName;
    pfFileName = new char[ strlen(value) + 1 ];
    strcpy( pfFileName, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourcePkiFile::SetSerialNumber( const int value )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  serialNumber = value;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourcePkiFile::IsCalledApplicationEntityTitleSupported()
// Date         : March 18, 2001
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
    return( pkiFileInteractionManager->IsCalledApplicationEntityTitleSupported( calledApplicationEntityTitle ) );
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSourcePkiFile::StartFindRequest( DcmDataset &findRequestIdentifiers )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : Based on the search mask which was passed, this function determines all the records
//                in the pki-file which match the values of matching key attributes in the search mask.
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

  // Set a read lock on the pki-file which shall be read from.
  SetReadlock();

  // Determine the values of the matching key attributes in the search mask.
  const char **matchingKeyAttrValues = NULL;
  unsigned long numOfMatchingKeyAttrValues = 0;
  DetermineMatchingKeyAttributeValues( matchingKeyAttrValues, numOfMatchingKeyAttrValues );

  if( verbose )
    DumpMessage( "Determining matching pki-file record." );

  // Determine the id of the pki-file record that matches the search mask. The pki-file in reality contains
  // exactly one record which is always supposed to match the search mask. The following call should therefore
  // always return one if of a matching record.
  long *matchingRecordIDs = NULL;
  unsigned long numOfMatchingRecordIDs = 0;
  pkiFileInteractionManager->GetMatchingRecordIDs( matchingKeyAttrValues, numOfMatchingKeyAttrValues, matchingRecordIDs, numOfMatchingRecordIDs );

  // free memory, reset variables
  delete matchingKeyAttrValues;
  matchingKeyAttrValues = NULL;
  numOfMatchingKeyAttrValues = 0;

  // dump some information if required
  if( verbose )
  {
    sprintf( msg, "Matching results: %lu matching records found in pki-file.", numOfMatchingRecordIDs );
    DumpMessage( msg );
  }

  // determine a correct return value. In case no matching records
  // were found, WLM_SUCCESS shall be returned. This is our assumption.
  WlmDataSourceStatusType status = WLM_SUCCESS;

  // Check if matching records were found in the pki-file.
  // If that is the case, do the following:
  if( numOfMatchingRecordIDs != 0 )
  {
    // create a container array that captures all result DcmDataset variables
    numOfMatchingDatasets = numOfMatchingRecordIDs;
    matchingDatasets = new DcmDataset*[ numOfMatchingDatasets ];

    // for each matching record do the following
    for( i=0 ; i<numOfMatchingRecordIDs ; i++ )
    {
      // dump some information if required
      if( verbose )
      {
        sprintf( msg, "  Processing matching result no. %lu.", i );
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
        DcmElement *element = matchingDatasets[i]->getElement(j);

        // Depending on if the current element is a sequence or not, process this element.
        if( element->ident() != EVR_SQ )
          HandleNonSequenceElementInResultDataset( element, matchingRecordIDs[i] );
        else
          HandleSequenceElementInResultDataset( element, matchingRecordIDs[i] );
      }
    }

    // free memory, reset variables
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

  // Release the read lock which was set on the pki-file.
  ReleaseReadlock();

  // return result
  return( status );
}

// ----------------------------------------------------------------------------

DcmDataset *WlmDataSourcePkiFile::NextFindResponse( WlmDataSourceStatusType &rStatus )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function will return the next dataset that matches the given search mask, if
//                there is one more resulting dataset to return. In such a case, rstatus will be set
//                to WLM_PENDING or WLM_PENDING_WARNING, depending on if an unsupported key attribute
//                was encountered in the search mask or not. If there are no more datasets that match
//                the search mask, this function will return an empty dataset and WLM_SUCCESS in rstatus.
// Parameters   : rStatus - [out] A value of type WlmDataSourceStatusType that can be used to
//                          decide if there are still elements that have to be returned.
// Return Value : The next dataset that matches the given search mask, or an empty dataset if
//                there are no more matching datasets in the pki-file.
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

void WlmDataSourcePkiFile::HandleNonSequenceElementInResultDataset( DcmElement *element, long matchingRecordID )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain non-sequence element whithin
//                the structure of a certain result dataset. This function assumes that all
//                elements in the result dataset are supported. In detail, a value for the
//                current element with regard to the currently processed matching record will
//                be requested from the pki-file, and this value will be set in the element.
// Parameters   : element          - [in] Pointer to the currently processed element.
//                matchingRecordID - [in] Identifies the matching record in the pki-file.
// Return Value : none.
{
  OFCondition cond;

  // Determine the current elements tag.
  DcmTagKey tag( element->getTag() );

  // get value from pki-file; note that all values for return key attributes are returned as strings.
  char *value = NULL;
  pkiFileInteractionManager->GetAttributeValueForMatchingRecord( tag, matchingRecordID, value );

  // work-around for pki interface: since the pki-file does not contain values for Scheduled Station
  // AE Title or Modality, do not update the values for these attributes fields in the result dataset
  if( ( value == NULL || strlen(value) == 0 ) && ( tag == DCM_ScheduledStationAETitle || tag == DCM_Modality ) )
    ;
  else
  {
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
      DumpMessage( "WlmDataSourcePkiFile::HandleNonSequenceElementInResultDataset: Could not set value in result element.\n" );
  }

  // free memory
  delete value;
}

// ----------------------------------------------------------------------------

void WlmDataSourcePkiFile::HandleSequenceElementInResultDataset( DcmElement *element, long matchingRecordID )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain sequence element whithin the
//                structure of a certain result dataset. This function assumes that all elements
//                in the result dataset are supported. It also assumes that there are no sequence
//                elements with length == 0 in the result dataset, and that each sequence element
//                contains one single non-empty item. In case there are more than one item in a
//                sequence element, the sequence element in the result data set will completely
//                be left unchanged. In detail, a value for the current element with regard to
//                the currently processed matching record will be requested from the pki-file,
//                and this value will be set in the element.
// Parameters   : element          - [in] Pointer to the currently processed element.
//                matchingRecordID - [in] Identifies the matching record in the pki-file.
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

void WlmDataSourcePkiFile::DetermineMatchingKeyAttributeValues( const char **&matchingKeyAttrValues, unsigned long &numOfMatchingKeyAttrValues )
// Date         : March 18, 2001
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
  matchingKeyAttrValues = new const char*[ numOfMatchingKeyAttrValues ];

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

int WlmDataSourcePkiFile::SetReadlock()
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function sets a read lock on the pki-file.
// Parameters   : none.
// Return Value : 0 - The read lock has been set successfully.
//                1 - The read lock has not been set successfully.
{
  // at the moment we do not use locking at all
  return( 0 );
}

// ----------------------------------------------------------------------------

int WlmDataSourcePkiFile::ReleaseReadlock()
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function releases a read lock that was set on the pki-file.
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
** $Log: wldspf.cc,v $
** Revision 1.4  2002-07-17 13:10:18  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.3  2002/05/08 13:20:39  wilkens
** Added new command line option -nse to wlmscpki and wlmscpdb.
**
** Revision 1.2  2002/04/18 14:19:54  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
**
**
*/
