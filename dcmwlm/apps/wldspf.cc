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
 *  Update Date:      $Date: 2002-04-18 10:30:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldspf.cc,v $
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
#include "dcvrae.h"
#include "dcvrda.h"
#include "dcvrcs.h"
#include "dcvrpn.h"
#include "dcvrtm.h"
#include "dcvrsh.h"
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

OFBool WlmDataSourcePkiFile::CheckSearchMask( DcmDataset *searchMask )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if the search mask has a correct format. It returns OFTrue if this
//                is the case, OFFalse if this is not the case. (Note that this function corresponds to
//                WlmDataSourceFiles::CheckIdentifiers and it should be the same as WlmDataSourceDatabase::CheckSearchMask.)
// Parameters   : searchMask - [in] Contains the search mask.
// Return Value : OFTrue  - The search mask has a correct format.
//                OFFalse - The search mask does not have a correct format.
{
  // Initialize counter that counts invalid elements in the search mask. This
  // variable will later be used to determine the return value for this function.
  int invalidMatchingKeyAttributeCount = 0;

  // remember the number of data elements in the search mask
  unsigned long numOfElementsInSearchMask = searchMask->card();

  // dump some information if required
  if( verbose )
    DumpMessage( "Checking the search mask." );

  // this member variable indicates if we encountered an unsupported
  // optional key attribute in the search mask; initialize it with false.
  foundUnsupportedOptionalKey = OFFalse;

  // start a loop; go through all data elements in the search mask, for each element, do some checking
  unsigned long i = 0;
  while( i < numOfElementsInSearchMask )
  {
    // determine current element
    DcmElement *element = searchMask->getElement(i);

    // Depending on if the current element is a sequence or not, process this element.
    if( element->ident() != EVR_SQ )
      CheckNonSequenceElementInSearchMask( searchMask, invalidMatchingKeyAttributeCount, element );
    else
      CheckSequenceElementInSearchMask( searchMask, invalidMatchingKeyAttributeCount, element );

    // Depending on if the number of elements in the search mask has changed, the current element was
    // deleted in the above called function or not. In case the current element was deleted, we do not
    // need to increase counter i, we only need to update numOfElementsInSearchMask. If the current
    // element was not deleted, we need to increase i but don't have to update numOfElementsInSearchMask.
    unsigned long currentNumOfElementsInSearchMask = searchMask->card();
    if( currentNumOfElementsInSearchMask != numOfElementsInSearchMask )
      numOfElementsInSearchMask = currentNumOfElementsInSearchMask;
    else
      i++;
  }

  // if there was more than 1 error, override the error comment
  if( invalidMatchingKeyAttributeCount > 1 )
    errorComment->putString("Syntax error in 1 or more matching keys");

  // return OFTrue or OFFalse depending on the number of invalid matching attributes
  return( invalidMatchingKeyAttributeCount == 0 );
}

// ----------------------------------------------------------------------------

void WlmDataSourcePkiFile::CheckNonSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if a non-sequence element in the search mask has a correct format.
//                Note that if the current element is an unsupported element, the entire element will be re-
//                moved from the search mask, since unsupported elements shall not be returned to the caller.
// Parameters   : searchMask                       - [in] Pointer to the search mask.
//                invalidMatchingKeyAttributeCount - [inout] Counter that counts invalid elements in the search mask.
//                element                          - [in] Pointer to the currently processed element.
//                supSequenceElement               - [in] Pointer to the superordinate sequence element of which
//                                                   the currently processed element is an attribute.
// Return Value : none.
{
  char msg[200];
  DcmElement *elem = NULL;

  // determine current element's tag
  DcmTag tag( element->getTag() );

  // determine if the current element is a supported matching key attribute
  if( IsSupportedMatchingKeyAttribute( element, supSequenceElement ) )
  {
    // if the current element is a supported matching key attribute, check if the current element meets
    // certain conditions (at the moment we only check if the element's data type and value go together)
    if( !CheckMatchingKey( element ) )
    {
      // if there is a problem with the current element, increase the corresponding counter and dump an error message.
      invalidMatchingKeyAttributeCount++;
      sprintf( msg, "WlmDataSourcePkiFile::CheckNonSequenceElementInSearchMask : Error: Matching key attribute (%s) with invalid value encountered in the search mask.", tag.getTagName() );
      DumpMessage( msg );
    }
  }
  // if current element is not a supported matching key attribute, determine
  // if the current element is a supported return key attribute.
  else if( IsSupportedReturnKeyAttribute( element, supSequenceElement ) )
  {
    // if the current element is a supported return key attribute everything is okay as long as
    // this attribute does not contain a value. According to the 2001 DICOM standard part 4,
    // section K.2.2.1.2. a return key attribute which is NOT a matching key attribute must not
    // contain a value. If one such attribute does contain a value, i.e. if the current element's
    // length does not equal 0, we want to dump a warning message (only in verbose mode).
    if( element->getLength() != 0 )
    {
      if( verbose )
        DumpMessage( "  - Non-empty return key attribute encountered in the search mask.\n    The specified value will be overridden." );
    }
  }
  // if current element is neither a supported matching key attribute nor a supported return key
  // attribute we've found an unsupported optional attribute; we want to delete this element from
  // the search mask, dump a warning and remember this attribute's tag in the list of error elements.
  else
  {
    // delete element from the search mask
    // in case there is a superordinate sequence element in whose first item this element occurs,
    // we need to delete this element from this particular item in the superordinate sequence element
    if( supSequenceElement != NULL )
    {
      elem = supSequenceElement->getItem(0)->remove( element );
      delete elem;
    }
    // in case there is NO superordinate sequence element in whose first item this element occurs,
    // we need to delete this element from the search mask itself.
    else
    {
      elem = searchMask->remove( element );
      delete elem;
    }

    // dump a warning
    if( verbose )
      DumpMessage( "  - Unsupported (non-sequence) attribute encountered in the search mask. \n    This attribute will not be existent in any result dataset." );

    // remember this attribute's tag in the list of error elements
    foundUnsupportedOptionalKey = OFTrue;
    PutErrorElements( tag );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourcePkiFile::CheckSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if a sequence element in the search mask has a correct format.
//                Note that if the current element is an unsupported element, the entire element will be re-
//                moved from the search mask, since unsupported elements shall not be returned to the caller.
//                Moreover, in case the sequence element in the search mask is supported but empty, this
//                function will expand the sequence element by inserting all required attributes into that sequence.
// Parameters   : searchMask                       - [in] Pointer to the search mask.
//                invalidMatchingKeyAttributeCount - [inout] Counter that counts invalid elements in the search mask.
//                element                          - [in] Pointer to the currently processed element.
//                supSequenceElement               - [in] Pointer to the superordinate sequence element of which
//                                                   the currently processed element is an attribute.
// Return Value : none.
{
  // Be aware of the following remark: the 2001 DICOM standard specifies in part 4, section C.2.2.2.6
  // that if a search mask contains a sequence attribute which contains no item or a single empty item,
  // all attributes from that particular sequence are in fact queried and shall be returned by the SCP.
  // This implementation accounts for this specification by expanding the search mask correspondingly.

  char msg[200];
  DcmElement *elem = NULL;

  // determine current element's tag
  DcmTag tag( element->getTag() );

  // remember that the current element is a sequence of items
  DcmSequenceOfItems *sequenceElement = (DcmSequenceOfItems*)element;

  // determine if the current sequence element is a supported matching or return key attribute
  if( IsSupportedMatchingKeyAttribute( element, supSequenceElement ) || IsSupportedReturnKeyAttribute( element, supSequenceElement ) )
  {
    // if the sequence attribute is supported, check if the length of the sequence equals 0, or if the sequence
    // contains exactly one empty item
    if( element->getLength() == 0 || ( sequenceElement->card() == 1 && sequenceElement->getItem(0)->card() == 0 ) )
    {
      // if this is the case, we want to expand this sequence according to the remark above
      ExpandEmptySequenceInSearchMask( element );
    }
    else
    {
      // if this is not the case we want to check the cardinality of the sequence; note that there should
      // always be exactly one item in a sequence within the search mask.
      if( sequenceElement->card() != 1 )
      {
        // if there is not exactly one item in the sequence of items, we want to remember this
        // attribute in the list of offending elements, we want to set the error comment correspondingly,
        // we want to dump an error message and we want to increase the corresponding counter.
        PutOffendingElements(tag);
        errorComment->putString("More than 1 item in sequence.");
        sprintf( msg, "WlmDataSourcePkiFile::CheckSearchMask : Error: More than one item in sequence %s (in the search mask) encountered.", tag.getTagName() );
        DumpMessage( msg );
        invalidMatchingKeyAttributeCount++;
      }

      // (now, even though we might have encountered an error, we go on scrutinizing the search mask)

      // get the first item in the sequence of items
      DcmItem *item = sequenceElement->getItem(0);

      // determine the cardinality of this item
      unsigned long numOfElementsInItem = item->card();
      unsigned long k = 0;

      // go through all elements of this item
      while( k < numOfElementsInItem )
      {
        // determine the current element
        DcmElement *elementseq = item->getElement(k);

        // Depending on if the current element is a sequence or not, process this element.
        if( elementseq->ident() != EVR_SQ )
          CheckNonSequenceElementInSearchMask( searchMask, invalidMatchingKeyAttributeCount, elementseq, sequenceElement );
        else
          CheckSequenceElementInSearchMask( searchMask, invalidMatchingKeyAttributeCount, elementseq, sequenceElement );

        // Depending on if the number of elements in the item has changed, the current element was
        // deleted in the above called function or not. In case the current element was deleted, we do not
        // need to increase counter k, we only need to update numOfElementsInItem. If the current
        // element was not deleted, we need to increase k but don't have to update numOfElementsInItem.
        unsigned long currentNumOfElementsInItem = item->card();
        if( currentNumOfElementsInItem != numOfElementsInItem )
          numOfElementsInItem = currentNumOfElementsInItem;
        else
          k++;
      }
    }
  }
  // if current element is neither a supported matching key attribute sequence nor a supported return key
  // attribute sequence we've found an unsupported sequence attribute; we want to delete this element from
  // the search mask, dump a warning message (only in verbose mode) remember this attribute in the list of
  // error elements.
  else
  {
    // delete element from the search mask
    // in case there is a superordinate sequence element in whose first item this element occurs,
    // we need to delete this element from this particular item in the superordinate sequence element
    if( supSequenceElement != NULL )
    {
      elem = supSequenceElement->getItem(0)->remove( element );
      delete elem;
    }
    // in case there is NO superordinate sequence element in whose first item this element occurs,
    // we need to delete this element from the search mask itself.
    else
    {
      elem = searchMask->remove( element );
      delete elem;
    }

    // dump a warning
    if( verbose )
      DumpMessage( "  - Unsupported (sequence) attribute encountered in the search mask. \n    This attribute will not be existent in any result dataset." );

    // remember this attribute's tag in the list of error elements
    foundUnsupportedOptionalKey = OFTrue;
    PutErrorElements( tag );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourcePkiFile::ExpandEmptySequenceInSearchMask( DcmElement *&element )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : According to the 2001 DICOM standard (part 4, section C.2.2.2.6), if a search mask
//                contains a sequence attribute which contains no item or a single empty item, all
//                attributes from that particular sequence are in fact queried and shall be returned
//                by the SCP. This implementation accounts for this specification by inserting a
//                corresponding single item with all required attributes into such emtpy sequences.
//                This function performs the insertion of the required item and attributes.
// Parameters   : element - [inout] Pointer to the currently processed element.
// Return Value : none.
{
  DcmItem *item = NULL;
  DcmElement *newElement = NULL;

  // remember that the current element is a sequence of items
  DcmSequenceOfItems *sequenceElement = (DcmSequenceOfItems*)element;

  // determine if the length of the sequence equals 0
  if( element->getLength() == 0 )
  {
    // if the length of the sequence attribute equals 0, we have to insert a single item
    item = new DcmItem();
    if( sequenceElement->insert( item ) != EC_Normal )
    {
      delete item;
      item = NULL;
    }
  }
  else
  {
    // if the length of the sequence attribute does not equal 0, we have to determine the
    // one and only item in the sequence
    item = sequenceElement->getItem(0);
  }

  // check if item does not equal NULL, only if this is the case, we can insert attributes
  // into that item. In general, this should always be the case.
  if( item != NULL )
  {
    // at this point, the current element contains exactly one item, which is empty, i.e. does not
    // contain any attributes. Now we have to insert the required attributes into this item.

    // depending on what kind of supported sequence attribute was passed, we have to
    // insert different attributes. The DICOM standard requires the support of three
    // different sequence attributes: the Scheduled Procedure Step Sequence, the
    // Referenced Study Sequence, and the Referenced Patient Sequence
    DcmTagKey key = element->getTag().getXTag();
    if( key == DCM_ScheduledProcedureStepSequence )
    {
      // if we are currently dealing with a scheduled procedure step sequence attribute,
      // we need to insert 11 different attributes (see 2001 DICOM standard, Table K.6-1.)
      newElement = new DcmApplicationEntity( DcmTag( DCM_ScheduledStationAETitle ) );    if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmDate( DcmTag( DCM_ScheduledProcedureStepStartDate ) );         if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmTime( DcmTag( DCM_ScheduledProcedureStepStartTime ) );         if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmCodeString( DcmTag( DCM_Modality ) );                          if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmPersonName( DcmTag( DCM_ScheduledPerformingPhysiciansName ) ); if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_ScheduledProcedureStepDescription ) ); if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledStationName ) );             if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledProcedureStepLocation ) );   if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_PreMedication ) );                     if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledProcedureStepID ) );         if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_RequestedContrastAgent ) );            if( item->insert( newElement ) != EC_Normal ) delete newElement;
    }
    else if( key == DCM_ReferencedStudySequence )
    {
      // if we are currently dealing with a referenced study sequence attribute,
      // we need to insert 2 different attributes (see 2001 DICOM standard, Table K.6-1.)
      newElement = new DcmUniqueIdentifier( DcmTag( DCM_ReferencedSOPClassUID ) );       if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmUniqueIdentifier( DcmTag( DCM_ReferencedSOPInstanceUID ) );    if( item->insert( newElement ) != EC_Normal ) delete newElement;
    }
    else if( key == DCM_ReferencedPatientSequence )
    {
      // if we are currently dealing with a referenced patient sequence attribute,
      // we need to insert 2 different attributes (see 2001 DICOM standard, Table K.6-1.)
      newElement = new DcmUniqueIdentifier( DcmTag( DCM_ReferencedSOPClassUID ) );       if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmUniqueIdentifier( DcmTag( DCM_ReferencedSOPInstanceUID ) );    if( item->insert( newElement ) != EC_Normal ) delete newElement;
    }
    else
    {
      // this code should never be executed. if it is, there is a logical error
      // in the source code and we want to dump a warning message
      DumpMessage( "WlmDataSourcePkiFile::ExpandEmptySequenceInSearchMask : Error: Unsupported sequence attribute encountered." );
    }
  }
  else
  {
    // this code should never be executed. if it is, we want to dump a warning message
    DumpMessage( "WlmDataSourcePkiFile::ExpandEmptySequenceInSearchMask : Error: Unable to find item in sequence." );
  }
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

OFBool WlmDataSourcePkiFile::IsSupportedMatchingKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if the given element refers to an attribute which is a supported
//                matching key attribute. If this is the case OFTrue is returned, else OFFalse.
//                Currently, the following attributes are supported matching key attributes:
//                 - DCM_ScheduledProcedureStepSequence         (0040,0100)  SQ  R  1
//                    > DCM_ScheduledStationAETitle             (0040,0001)  AE  R  1
//                    > DCM_ScheduledProcedureStepStartDate     (0040,0002)  DA  R  1
//                    > DCM_ScheduledProcedureStepStartTime     (0040,0003)  TM  R  1
//                    > DCM_Modality                            (0008,0060)  CS  R  1
//                    > DCM_ScheduledPerformingPhysiciansName   (0040,0006)  PN  R  2
//                 - DCM_PatientsName                           (0010,0010)  PN  R  1
//                 - DCM_PatientID                              (0010,0020)  LO  R  1
//                The list of these attributes completely covers the required matching key attributes
//                which are specified in the 2001 DICOM standard, part 4, Table K.6-1, i.e. all matching
//                key attributes which (according to the 2001 DICOM standard) have to be supported by an
//                SCP are supported by this SCP.
// Parameters   : element            - [in] Pointer to the element which shall be checked.
//                supSequenceElement - [in] Pointer to the superordinate sequence element of which
//                                     the currently processed element is an attribute, or NULL if
//                                     the currently processed element does not belong to any sequence.
// Return Value : OFTrue  - The given tag is a supported matching key attribute.
//                OFFalse - The given tag is not a supported matching key attribute.
{
  DcmTagKey elementKey, supSequenceElementKey;

  // determine the current element's tag
  elementKey = element->getTag().getXTag();

  // determine the sequence element's tag, if there is one
  if( supSequenceElement != NULL )
    supSequenceElementKey = supSequenceElement->getTag().getXTag();

  // initialize result variable
  OFBool isSupportedMatchingKeyAttribute = OFFalse;

  // perform check, depending on if a superordinate sequence element is given or not
  if( supSequenceElement != NULL )
  {
    if( supSequenceElementKey == DCM_ScheduledProcedureStepSequence &&
        ( elementKey == DCM_ScheduledStationAETitle           ||
          elementKey == DCM_ScheduledProcedureStepStartDate   ||
          elementKey == DCM_ScheduledProcedureStepStartTime   ||
          elementKey == DCM_Modality                          ||
          elementKey == DCM_ScheduledPerformingPhysiciansName ) )
      isSupportedMatchingKeyAttribute = OFTrue;
  }
  else
  {
    if( elementKey == DCM_ScheduledProcedureStepSequence ||
        elementKey == DCM_PatientsName                   ||
        elementKey == DCM_PatientID )
      isSupportedMatchingKeyAttribute = OFTrue;
  }

  // return result
  return( isSupportedMatchingKeyAttribute );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourcePkiFile::IsSupportedReturnKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if the given element refers to an attribute which is a supported
//                return key attribute. If this is the case OFTrue is returned, else OFFalse.
//                Currently, the following required return key attributes are supported:
//                 - DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
//                    > DCM_ScheduledStationAETitle                        (0040,0001)  AE  R  1
//                    > DCM_ScheduledProcedureStepStartDate                (0040,0002)  DA  R  1
//                    > DCM_ScheduledProcedureStepStartTime                (0040,0003)  TM  R  1
//                    > DCM_Modality                                       (0008,0060)  CS  R  1
//                    > DCM_ScheduledPerformingPhysiciansName              (0040,0006)  PN  R  2
//                    > DCM_ScheduledProcedureStepDescription              (0040,0007)  LO  O  1
//                    > DCM_ScheduledStationName                           (0040,0010)  SH  O  2
//                    > DCM_ScheduledProcedureStepLocation                 (0040,0011)  SH  O  2
//                    > DCM_PreMedication                                  (0040,0012)  LO  O  2
//                    > DCM_ScheduledProcedureStepID                       (0040,0009)  SH  O  1
//                    > DCM_RequestedContrastAgent                         (0032,1070)  LO  O  2
//                 - DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
//                 - DCM_RequestedProcedureDescription                     (0032,1060)  LO  O  1
//                 - DCM_StudyInstanceUID                                  (0020,000d)  UI  O  1
//                 - DCM_ReferencedStudySequence                           (0008,1110)  SQ  O  2
//                    > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  1  Note that the standard specifies this attribute as 1. unfortunately, this implementation only supports this attribute as 2. Also note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                    > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  1  Note that the standard specifies this attribute as 1. unfortunately, this implementation only supports this attribute as 2. Also note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                 - DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
//                 - DCM_PatientTransportArrangements                      (0040,1004)  LO  O  2
//                 - DCM_AccessionNumber                                   (0008,0050)  SH  O  2
//                 - DCM_RequestingPhysician                               (0032,1032)  PN  O  2
//                 - DCM_ReferringPhysiciansName                           (0008,0090)  PN  O  2
//                 - DCM_AdmissionID                                       (0038,0010)  LO  O  2
//                 - DCM_CurrentPatientLocation                            (0038,0300)  LO  O  2
//                 - DCM_ReferencedPatientSequence                         (0008,1120)  SQ  O  2
//                    > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  2  Note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                    > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  2  Note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                 - DCM_PatientsName                                      (0010,0010)  PN  R  1
//                 - DCM_PatientID                                         (0010,0020)  LO  R  1
//                 - DCM_PatientsBirthDate                                 (0010,0030)  DA  O  2
//                 - DCM_PatientsSex                                       (0010,0040)  CS  O  2
//                 - DCM_PatientsWeight                                    (0010,1030)  DS  O  2
//                 - DCM_ConfidentialityConstraintOnPatientDataDescription (0040,3001)  LO  O  2
//                 - DCM_PatientState                                      (0038,0500)  LO  O  2
//                 - DCM_PregnancyStatus                                   (0010,21c0)  US  O  2
//                 - DCM_MedicalAlerts                                     (0010,2000)  LO  O  2
//                 - DCM_ContrastAllergies                                 (0010,2110)  LO  O  2
//                 - DCM_SpecialNeeds                                      (0038,0050)  LO  O  2
//                The list of these attributes completely covers the required return key attributes which are
//                specified in the 2001 DICOM standard, part 4, Table K.6-1, i.e. all return key attributes
//                which (according to the 2001 DICOM standard) have to be supported by an SCP are supported by
//                this SCP. Additionally, the following optional return key attributes are supported by this SCP:
//                 - DCM_NamesOfIntendedRecipientsOfResults (from the Requested Procedure Module)    (0040,1010)  PN  O  3
//                 - DCM_InstitutionName                    (from the Visit Identification Module)   (0008,0080)  LO  O  3
//                 - DCM_AdmittingDiagnosesDescription      (from the Visit Admission Module)        (0008,1080)  LO  O  3
//                 - DCM_OtherPatientIDs                    (from the Patient Identification Module) (0010,1000)  LO  O  3
//                 - DCM_PatientsSize                       (from the Patient Demographic Module)    (0010,1020)  DS  O  3
//                 - DCM_EthnicGroup                        (from the Patient Demographic Module)    (0010,2160)  SH  O  3
//                 - DCM_PatientComments                    (from the Patient Demographic Module)    (0010,4000)  LT  O  3
//                 - DCM_AdditionalPatientHistory           (from the Patient Medical Module)        (0010,21b0)  LT  O  3
//                 - DCM_LastMenstrualDate                  (from the Patient Medical Module)        (0010,21d0)  DA  O  3
// Parameters   : element            - [in] Pointer to the element which shall be checked.
//                supSequenceElement - [in] Pointer to the superordinate sequence element of which
//                                     the currently processed element is an attribute, or NULL if
//                                     the currently processed element does not belong to any sequence.
// Return Value : OFTrue  - The given tag is a supported return key attribute.
//                OFFalse - The given tag is not a supported return key attribute.
{
  DcmTagKey elementKey, supSequenceElementKey;

  // determine the current element's tag
  elementKey = element->getTag().getXTag();

  // determine the sequence element's tag, if there is one
  if( supSequenceElement != NULL )
    supSequenceElementKey = supSequenceElement->getTag().getXTag();

  // initialize result variable
  OFBool isSupportedReturnKeyAttribute = OFFalse;

  // perform check, depending on if a superordinate sequence element is given or not
  if( supSequenceElement != NULL )
  {
    if( ( supSequenceElementKey == DCM_ScheduledProcedureStepSequence &&
          ( elementKey == DCM_ScheduledStationAETitle           ||
            elementKey == DCM_ScheduledProcedureStepStartDate   ||
            elementKey == DCM_ScheduledProcedureStepStartTime   ||
            elementKey == DCM_Modality                          ||
            elementKey == DCM_ScheduledPerformingPhysiciansName ||
            elementKey == DCM_ScheduledProcedureStepDescription ||
            elementKey == DCM_ScheduledStationName              ||
            elementKey == DCM_ScheduledProcedureStepLocation    ||
            elementKey == DCM_PreMedication                     ||
            elementKey == DCM_ScheduledProcedureStepID          ||
            elementKey == DCM_RequestedContrastAgent ) )                  ||
        ( supSequenceElementKey == DCM_ReferencedStudySequence        &&
          ( elementKey == DCM_ReferencedSOPClassUID             ||
            elementKey == DCM_ReferencedSOPInstanceUID ) )                ||
        ( supSequenceElementKey == DCM_ReferencedPatientSequence      &&
          ( elementKey == DCM_ReferencedSOPClassUID             ||
            elementKey == DCM_ReferencedSOPInstanceUID ) ) )
      isSupportedReturnKeyAttribute = OFTrue;
  }
  else
  {
    if( elementKey == DCM_ScheduledProcedureStepSequence                    ||
        elementKey == DCM_RequestedProcedureID                              ||
        elementKey == DCM_RequestedProcedureDescription                     ||
        elementKey == DCM_StudyInstanceUID                                  ||
        elementKey == DCM_ReferencedStudySequence                           ||
        elementKey == DCM_RequestedProcedurePriority                        ||
        elementKey == DCM_PatientTransportArrangements                      ||
        elementKey == DCM_AccessionNumber                                   ||
        elementKey == DCM_RequestingPhysician                               ||
        elementKey == DCM_ReferringPhysiciansName                           ||
        elementKey == DCM_AdmissionID                                       ||
        elementKey == DCM_CurrentPatientLocation                            ||
        elementKey == DCM_ReferencedPatientSequence                         ||
        elementKey == DCM_PatientsName                                      ||
        elementKey == DCM_PatientID                                         ||
        elementKey == DCM_PatientsBirthDate                                 ||
        elementKey == DCM_PatientsSex                                       ||
        elementKey == DCM_PatientsWeight                                    ||
        elementKey == DCM_ConfidentialityConstraintOnPatientDataDescription ||
        elementKey == DCM_PatientState                                      ||
        elementKey == DCM_PregnancyStatus                                   ||
        elementKey == DCM_MedicalAlerts                                     ||
        elementKey == DCM_ContrastAllergies                                 ||
        elementKey == DCM_SpecialNeeds                                      ||
        elementKey == DCM_NamesOfIntendedRecipientsOfResults                ||        //optional attributes start here
        elementKey == DCM_InstitutionName                                   ||
        elementKey == DCM_AdmittingDiagnosesDescription                     ||
        elementKey == DCM_OtherPatientIDs                                   ||
        elementKey == DCM_PatientsSize                                      ||
        elementKey == DCM_EthnicGroup                                       ||
        elementKey == DCM_PatientComments                                   ||
        elementKey == DCM_AdditionalPatientHistory                          ||
        elementKey == DCM_LastMenstrualDate )
      isSupportedReturnKeyAttribute = OFTrue;
  }

  // return result
  return( isSupportedReturnKeyAttribute );
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wldspf.cc,v $
** Revision 1.1  2002-04-18 10:30:09  wilkens
** Performed split between db-variant and pki-variant.
**
**
**
*/
