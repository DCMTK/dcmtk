/*
 *
 *  Copyright (C) 1996-2019, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Class for connecting to a file-based data source.
 *
 */

// ----------------------------------------------------------------------------

#include "dcmtk/config/osconfig.h"  // specific configuration for operating system
BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>     // for O_RDWR
#endif
END_EXTERN_C
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmwlm/wltypdef.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmwlm/wlds.h"
#include "dcmtk/dcmwlm/wlfsim.h"
#include "dcmtk/ofstd/ofstd.h"

#include "dcmtk/dcmwlm/wldsfs.h"

// ----------------------------------------------------------------------------

WlmDataSourceFileSystem::WlmDataSourceFileSystem()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : fileSystemInteractionManager( ), dfPath( "" ), enableRejectionOfIncompleteWlFiles( OFTrue ),
    handleToReadLockFile( 0 )
{
}

// ----------------------------------------------------------------------------

WlmDataSourceFileSystem::~WlmDataSourceFileSystem()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // release read lock on data source if it is set
  if( readLockSetOnDataSource ) ReleaseReadlock();

}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceFileSystem::ConnectToDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Connects to the data source.
// Parameters   : none.
// Return Value : Indicates if the connection was established successfully.
{
  // set variables in fileSystemInteractionManager object
  fileSystemInteractionManager.SetEnableRejectionOfIncompleteWlFiles( enableRejectionOfIncompleteWlFiles );

  // connect to file system
  OFCondition cond = fileSystemInteractionManager.ConnectToFileSystem( dfPath );

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceFileSystem::DisconnectFromDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Disconnects from the data source.
// Parameters   : none.
// Return Value : Indicates if the disconnection was completed successfully.
{
  // disconnect from file system
  OFCondition cond = fileSystemInteractionManager.DisconnectFromFileSystem();

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::SetDfPath( const OFString& value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  dfPath = value;
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::SetEnableRejectionOfIncompleteWlFiles( OFBool value )
// Date         : May 3, 2005
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  enableRejectionOfIncompleteWlFiles = value;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::IsCalledApplicationEntityTitleSupported()
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
  if( calledApplicationEntityTitle.empty() )
    return( OFFalse );
  else
    return( fileSystemInteractionManager.IsCalledApplicationEntityTitleSupported( calledApplicationEntityTitle ) );
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::HandleExistentButEmptyDescriptionAndCodeSequenceAttributes( DcmItem *dataset, const DcmTagKey &descriptionTagKey, const DcmTagKey &codeSequenceTagKey )
// Date         : May 3, 2005
// Author       : Thomas Wilkens
// Task         : This function performs a check on two attributes in the given dataset. At two different places
//                in the definition of the DICOM worklist management service, a description attribute and a code
//                sequence attribute with a return type of 1C are mentioned, and the condition specifies that
//                either the description attribute or the code sequence attribute or both shall be supported by
//                an SCP. (I am talking about RequestedProcedureDescription vs. RequestedProcedureCodeSequence
//                and ScheduledProcedureStepDescription vs. ScheduledProtocolCodeSequence.) In both cases, this
//                implementation actually supports both, the description _and_ the code sequence attributes.
//                In cases where the description attribute is actually empty or the code sequence attribute
//                is actually empty or contains exactly one item with an empty CodeValue and an empty
//                CodingSchemeDesignator, we want to remove the empty attribute from the dataset. This is what
//                this function does. (Please note, that this function will always only delete one of the two,
//                and this function will start checking the sequence attribute.
// Parameters   : dataset            - [in] Dataset in which the consistency of the two attributes shall be checked.
//                descriptionTagKey  - [in] DcmTagKey of the description attribute which shall be checked.
//                codeSequenceTagKey - [in] DcmTagKey of the codeSequence attribute which shall be checked.
// Return Value : none.
{
  DcmElement *codeSequenceAttribute = NULL, *descriptionAttribute = NULL;
  DcmElement *elementToRemove = NULL, *codeValueAttribute = NULL, *codingSchemeDesignatorAttribute = NULL;
  OFBool codeSequenceAttributeRemoved = OFFalse;

  // only do something with the code sequence attribute if it is contained in the dataset
  if( dataset->findAndGetElement( codeSequenceTagKey, codeSequenceAttribute ).good() )
  {
    // if the code sequence attribute is empty or contains exactly one item with an empty
    // CodeValue and an empty CodingSchemeDesignator, remove the attribute from the dataset
    if( ( ((DcmSequenceOfItems*)codeSequenceAttribute)->card() == 0 ) ||
        ( ((DcmSequenceOfItems*)codeSequenceAttribute)->card() == 1 &&
          ((DcmSequenceOfItems*)codeSequenceAttribute)->getItem(0)->findAndGetElement( DCM_CodeValue, codeValueAttribute ).good() &&
          codeValueAttribute->getLength() == 0 &&
          ((DcmSequenceOfItems*)codeSequenceAttribute)->getItem(0)->findAndGetElement( DCM_CodingSchemeDesignator, codingSchemeDesignatorAttribute ).good() &&
          codingSchemeDesignatorAttribute->getLength() == 0 ) )
    {
      elementToRemove = dataset->remove( codeSequenceAttribute );
      delete elementToRemove;
      codeSequenceAttributeRemoved = OFTrue;
    }
  }

  // if the code sequence attribute has not been removed and if the description
  // attribute is empty, remove the description attribute from the dataset
  if( !codeSequenceAttributeRemoved &&
      dataset->findAndGetElement( descriptionTagKey, descriptionAttribute ).good() &&
      descriptionAttribute->getLength() == 0 )
  {
    elementToRemove = dataset->remove( descriptionAttribute );
    delete elementToRemove;
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::HandleExistentButEmptyReferencedStudyOrPatientSequenceAttributes( DcmDataset *dataset, const DcmTagKey &sequenceTagKey )
// Date         : May 3, 2005
// Author       : Thomas Wilkens
// Task         : This function performs a check on a sequence attribute in the given dataset. At two different places
//                in the definition of the DICOM worklist management service, a sequence attribute with a return type
//                of 2 is mentioned containing two 1C attributes in its item; the condition of the two 1C attributes
//                specifies that in case a sequence item is present, then these two attributes must be existent and
//                must contain a value. (I am talking about ReferencedStudySequence and ReferencedPatientSequence.)
//                In cases where the sequence attribute contains exactly one item with an empty ReferencedSOPClass
//                and an empty ReferencedSOPInstance, we want to remove the item from the sequence. This is what
//                this function does.
// Parameters   : dataset         - [in] Dataset in which the consistency of the sequence attribute shall be checked.
//                sequenceTagKey  - [in] DcmTagKey of the sequence attribute which shall be checked.
// Return Value : none.
{
  DcmElement *sequenceAttribute = NULL, *referencedSOPClassUIDAttribute = NULL, *referencedSOPInstanceUIDAttribute = NULL;

  // in case the sequence attribute contains exactly one item with an empty
  // ReferencedSOPClassUID and an empty ReferencedSOPInstanceUID, remove the item
  if( dataset->findAndGetElement( sequenceTagKey, sequenceAttribute ).good() &&
      ( (DcmSequenceOfItems*)sequenceAttribute )->card() == 1 &&
      ( (DcmSequenceOfItems*)sequenceAttribute )->getItem(0)->findAndGetElement( DCM_ReferencedSOPClassUID, referencedSOPClassUIDAttribute ).good() &&
      referencedSOPClassUIDAttribute->getLength() == 0 &&
      ( (DcmSequenceOfItems*)sequenceAttribute )->getItem(0)->findAndGetElement( DCM_ReferencedSOPInstanceUID, referencedSOPInstanceUIDAttribute, OFFalse ).good() &&
      referencedSOPInstanceUIDAttribute->getLength() == 0 )
  {
    DcmItem *item = ((DcmSequenceOfItems*)sequenceAttribute)->remove( ((DcmSequenceOfItems*)sequenceAttribute)->getItem(0) );
    delete item;
  }
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSourceFileSystem::StartFindRequest( const DcmDataset &findRequestIdentifiers )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : Based on the search mask which was passed, this function determines all the records in the
//                worklist database files which match the values of matching key attributes in the search mask.
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
  DcmElement *scheduledProcedureStepSequenceAttribute = NULL;

  // Initialize offending elements, error elements and error comment.
  delete offendingElements;
  delete errorElements;
  delete errorComment;
  offendingElements = new DcmAttributeTag( DCM_OffendingElement);
  errorElements = new DcmAttributeTag( DCM_OffendingElement);
  errorComment = new DcmLongString( DCM_ErrorComment);

  // Initialize member variable identifiers; this variable will contain the search mask.
  ClearDataset( identifiers );
  delete identifiers;
  identifiers = new DcmDataset( findRequestIdentifiers );

  // Remove group length and padding elements from the search mask.
  identifiers->computeGroupLengthAndPadding( EGL_withoutGL, EPD_withoutPadding );

  // Actually there should be no elements in array matchingDatasets. Delete them to be sure.
  // matchingDatasets will in the end contain all records (datasets) that match the search mask.
  while ( !matchingDatasets.empty() )
  {
    DcmDataset *dset = matchingDatasets.front();
    delete dset; dset = NULL;
    matchingDatasets.pop_front();
  }

  // This member variable indicates if we encountered an unsupported
  // optional key attribute in the search mask; initialize it with false.
  // It might be updated within CheckSearchMask().
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
  DCMWLM_INFO("Expanded Find SCP Request Identifiers:" << OFendl
    << DcmObject::PrintHelper(*identifiers) << OFendl
    << "=============================");

  // Set a read lock on the worklist files which shall be read from.
  if( !SetReadlock() )
    return( WLM_REFUSED_OUT_OF_RESOURCES );

  // dump some information if required
  DCMWLM_INFO("Determining matching records from worklist files");

  // Determine records from worklist files which match the search mask
  unsigned long numOfMatchingRecords = OFstatic_cast(unsigned long, fileSystemInteractionManager.DetermineMatchingRecords( identifiers ));

  // dump some information if required
  DCMWLM_INFO("Matching results: " << numOfMatchingRecords << " matching records found in worklist files");

  // determine a correct return value. In case no matching records
  // were found, WLM_SUCCESS shall be returned. This is our assumption.
  WlmDataSourceStatusType status = WLM_SUCCESS;

  // Check if matching records were found in the database.
  // If that is the case, do the following:
  if( numOfMatchingRecords != 0 )
  {
    // for each matching record do the following
    for( i=0 ; i<numOfMatchingRecords ; i++ )
    {
      // For every matching record ID, add one result dataset to result list and
      // initialize it with search mask
      DcmDataset *resultRecord = new DcmDataset(*identifiers);
      matchingDatasets.push_back(resultRecord);

      // dump some information if required
      DCMWLM_INFO("  Processing matching result no. " << i);

      // Determine the number of elements in matchingDatasets[i].
      unsigned long numOfElementsInDataset = resultRecord->card();

      // Go through all the elements in matchingDatasets[i].
      for( j=0 ; j < numOfElementsInDataset ; j++ )
      {
        // Determine the current element.
        DcmElement *element = resultRecord->getElement(j);

        // Depending on if the current element is a sequence or not, process this element.
        if( element->ident() != EVR_SQ )
          HandleNonSequenceElementInResultDataset( element, i );
        else
          HandleSequenceElementInResultDataset( element, i );
      }

      // if the ScheduledProcedureStepSequence can be found in the current dataset, handle
      // existent but empty ScheduledProcedureStepDescription and ScheduledProtocolCodeSequence
      if( resultRecord->findAndGetElement( DCM_ScheduledProcedureStepSequence, scheduledProcedureStepSequenceAttribute, OFFalse ).good() )
        HandleExistentButEmptyDescriptionAndCodeSequenceAttributes( ((DcmDataset*)((DcmSequenceOfItems*)scheduledProcedureStepSequenceAttribute)->getItem(0)), DCM_ScheduledProcedureStepDescription, DCM_ScheduledProtocolCodeSequence );

      // handle existent but empty RequestedProcedureDescription and RequestedProcedureCodeSequence
      HandleExistentButEmptyDescriptionAndCodeSequenceAttributes( resultRecord, DCM_RequestedProcedureDescription, DCM_RequestedProcedureCodeSequence );

      // handle existent but empty ReferencedStudySequence
      HandleExistentButEmptyReferencedStudyOrPatientSequenceAttributes( resultRecord, DCM_ReferencedStudySequence );

      // handle existent but empty ReferencedPatientSequence
      HandleExistentButEmptyReferencedStudyOrPatientSequenceAttributes( resultRecord, DCM_ReferencedPatientSequence );

      // after having created the entire returned data set, deal with the "Specific Character Set" attribute.
      // first option: remove character set element from result dataset
      if( returnedCharacterSet == RETURN_NO_CHARACTER_SET )
      {
        // nothing to do since it was already removed from the search mask
        // resultRecord->findAndDeleteElement( DCM_SpecificCharacterSet );
      }
      // check whether extended characters maybe used in the result dataset
      else if ( resultRecord->isAffectedBySpecificCharacterSet() )
      {
        OFCondition cond = EC_Normal;
        // second option: specify ISO 8859-1 (Latin-1) character set
        if( returnedCharacterSet == RETURN_CHARACTER_SET_ISO_IR_100 )
        {
          cond = resultRecord->putAndInsertString( DCM_SpecificCharacterSet, "ISO_IR 100" );
        }
        // third option: use character set from worklist file
        else if( returnedCharacterSet == RETURN_CHARACTER_SET_FROM_FILE )
        {
          char *value = NULL;
          fileSystemInteractionManager.GetAttributeValueForMatchingRecord( DCM_SpecificCharacterSet, NULL, 0, i, value );
          if( (value != NULL) && (strlen(value) > 0) )
          {
            cond = resultRecord->putAndInsertString( DCM_SpecificCharacterSet, value );
          }
        }
        if( cond.bad() )
          DCMWLM_WARN("Could not set value of attribute SpecificCharacterSet in result dataset");
      }
    }

    // Determine a corresponding return value: If matching records were found, WLM_PENDING or
    // WLM_PENDING_WARNING shall be returned, depending on if an unsupported optional key was
    // found in the search mask or not.
    if( foundUnsupportedOptionalKey )
      status = WLM_PENDING_WARNING;
    else
      status = WLM_PENDING;
  }

  // forget the matching records in the fileSystemInteractionManager (free memory)
  fileSystemInteractionManager.ClearMatchingRecords();

  // Now all the resulting data sets are contained in the member array matchingDatasets.
  // The variable numOfMatchingDatasets specifies the number of array fields.

  // Release the read lock which was set on the database tables.
  ReleaseReadlock();

  // return result
  return( status );
}

// ----------------------------------------------------------------------------

DcmDataset *WlmDataSourceFileSystem::NextFindResponse( WlmDataSourceStatusType &rStatus )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function will return the next dataset that matches the given search mask, if
//                there is one more resulting dataset to return. In such a case, rStatus will be set
//                to WLM_PENDING or WLM_PENDING_WARNING, depending on if an unsupported key attribute
//                was encountered in the search mask or not. If there are no more datasets that match
//                the search mask, this function will return an empty dataset and WLM_SUCCESS in rStatus.
// Parameters   : rStatus - [out] A value of type WlmDataSourceStatusType that can be used to
//                          decide if there are still elements that have to be returned.
// Return Value : The next dataset that matches the given search mask, or an empty dataset if
//                there are no more matching datasets in the worklist database files.
{
  DcmDataset *resultDataset = NULL;

  // If there are no more datasets that can be returned, do the following
  if( matchingDatasets.empty() )
  {
    // Set the return status to WLM_SUCCESS and return an empty dataset.
    rStatus = WLM_SUCCESS;
    resultDataset = NULL;
  }
  else
  {
    // We want to return the last array element and forget the pointer to this dataset here
    resultDataset = matchingDatasets.back();
    matchingDatasets.pop_back();

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

void WlmDataSourceFileSystem::HandleNonSequenceElementInResultDataset( DcmElement *element, unsigned long idx )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain non-sequence element within
//                the structure of a certain result dataset. This function assumes that all
//                elements in the result dataset are supported. In detail, a value for the
//                current element with regard to the currently processed matching record will
//                be requested from the fileSystemInteractionManager, and this value will be
//                set in the element.
// Parameters   : element - [in] Pointer to the currently processed element.
//                idx     - [in] Index of the matching record (identifies this record).
// Return Value : none.
{
  OFCondition cond;

  // determine the current elements tag.
  DcmTagKey tag( element->getTag().getXTag() );

  // check if the current element is the "Specific Character Set" (0008,0005) attribute;
  // we do not want to deal with this attribute here.
  if( tag != DCM_SpecificCharacterSet )
  {
    // in case the current element is not the "Specific Character Set" (0008,0005) attribute,
    // get a value for the current element from database; note that all values for return key
    // attributes are returned as strings by GetAttributeValueForMatchingRecord().
    char *value = NULL;
    fileSystemInteractionManager.GetAttributeValueForMatchingRecord( tag, superiorSequenceArray, numOfSuperiorSequences, idx, value );

    // put value in element
    // (note that there is currently one attribute (DCM_PregnancyStatus) for which the value must not
    // be set as a string but as an unsigned integer, because this attribute is of type US. Hence, in
    // case we are dealing with the attribute DCM_PregnancyStatus, we have to convert the returned
    // value into an unsigned integer and set it correspondingly in the element variable)
    if( tag == DCM_PregnancyStatus )
    {
      Uint16 uintValue = atoi( value );
      cond = element->putUint16( uintValue );
    }
    else
      cond = element->putString( value );
    if( cond.bad() )
      DCMWLM_WARN("WlmDataSourceFileSystem::HandleNonSequenceElementInResultDataset: Could not set value in result element");

    // free memory
    delete[] value;
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::HandleSequenceElementInResultDataset( DcmElement *element, unsigned long idx )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain sequence element within the structure
//                of a certain result dataset. On the basis of the matching record from the data source,
//                this function will add items and values for all elements in these items to the current
//                sequence element in the result dataset. This function assumes that all elements in the
//                result dataset are supported. In case the current sequence element contains no items or
//                more than one item, this element will be left unchanged.
// Parameters   : element - [in] Pointer to the currently processed element.
//                idx     - [in] Index of the matching record (identifies this record).
// Return Value : none.
{
  unsigned long i, k, numOfItemsInResultSequence;
  WlmSuperiorSequenceInfoType *tmp;

  // consider this element as a sequence of items.
  DcmSequenceOfItems *sequenceOfItemsElement = (DcmSequenceOfItems*)element;

  // according to the DICOM standard, part 4, section C.2.2.2.6, a sequence in the search
  // mask (and we made a copy of the search mask that we update here, so that it represents
  // a result value) must have exactly one item which in turn can be empty
  if( sequenceOfItemsElement->card() != 1 )
  {
    // if the sequence's cardinality does not equal 1, we want to dump a warning and do nothing here
    DCMWLM_WARN( "    - Sequence with not exactly one item encountered in the search mask" << OFendl
      << "      The corresponding sequence of the currently processed result data set will show the exact same structure as in the given search mask" );
  }
  else
  {
    // if the sequence's cardinality does equal 1, we want to process this sequence and
    // add all information from the matching record in the data source to this sequence

    // determine the current sequence elements tag.
    DcmTagKey sequenceTag( sequenceOfItemsElement->getTag().getXTag() );

    // determine how many items this sequence has in the matching record in the data source
    numOfItemsInResultSequence = fileSystemInteractionManager.GetNumberOfSequenceItemsForMatchingRecord( sequenceTag, superiorSequenceArray, numOfSuperiorSequences, idx );

    // remember all relevant information about this and all
    // superior sequence elements in superiorSequenceArray
    tmp = new WlmSuperiorSequenceInfoType[ numOfSuperiorSequences + 1 ];
    for( i=0 ; i<numOfSuperiorSequences ; i++ )
    {
      tmp[i].sequenceTag = superiorSequenceArray[i].sequenceTag;
      tmp[i].numOfItems  = superiorSequenceArray[i].numOfItems;
      tmp[i].currentItem = superiorSequenceArray[i].currentItem;
    }
    tmp[numOfSuperiorSequences].sequenceTag = sequenceTag;
    tmp[numOfSuperiorSequences].numOfItems = numOfItemsInResultSequence;
    tmp[numOfSuperiorSequences].currentItem = 0;

    if( superiorSequenceArray != NULL )
      delete[] superiorSequenceArray;

    superiorSequenceArray = tmp;

    numOfSuperiorSequences++;

    // in case this sequence has more than one item in the database, copy the first item
    // an appropriate number of times and insert all items into the result dataset
    DcmItem *firstItem = sequenceOfItemsElement->getItem(0);
    for( i=1 ; i<numOfItemsInResultSequence ; i++ )
    {
      DcmItem *newItem = new DcmItem( *firstItem );
      sequenceOfItemsElement->append( newItem );
    }

    // go through all items of the result dataset
    for( i=0 ; i<numOfItemsInResultSequence ; i++ )
    {
      // determine current item
      DcmItem *itemInSequence = sequenceOfItemsElement->getItem(i);

      // get its cardinality.
      unsigned long numOfElementsInItem = itemInSequence->card();

      // update current item indicator in superiorSequenceArray
      superiorSequenceArray[ numOfSuperiorSequences - 1 ].currentItem = i;

      // go through all elements in this item
      for( k=0 ; k<numOfElementsInItem ; k++ )
      {
        // get the current element.
        DcmElement *elementInItem = itemInSequence->getElement(k);

        // depending on if the current element is a sequence or not, process this element
        if( elementInItem->ident() != EVR_SQ )
          HandleNonSequenceElementInResultDataset( elementInItem, idx );
        else
          HandleSequenceElementInResultDataset( elementInItem, idx );
      }
    }

    // delete information about current sequence from superiorSequenceArray
    if( numOfSuperiorSequences == 1 )
    {
      delete[] superiorSequenceArray;
      superiorSequenceArray = NULL;
      numOfSuperiorSequences = 0;
    }
    else
    {
      tmp = new WlmSuperiorSequenceInfoType[ numOfSuperiorSequences - 1 ];
      for( i=0 ; i<numOfSuperiorSequences - 1; i++ )
      {
        tmp[i].sequenceTag = superiorSequenceArray[i].sequenceTag;
        tmp[i].numOfItems  = superiorSequenceArray[i].numOfItems;
        tmp[i].currentItem = superiorSequenceArray[i].currentItem;
      }

      delete[] superiorSequenceArray;
      superiorSequenceArray = tmp;

      numOfSuperiorSequences--;
    }
  }
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::SetReadlock()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function sets a read lock on the LOCKFILE in the directory
//                that is specified through dfPath and calledApplicationEntityTitle.
// Parameters   : none.
// Return Value : OFTrue - The read lock has been set successfully.
//                OFFalse - The read lock has not been set successfully.
{
#ifndef _WIN32
  struct flock lockdata;
#endif
  int result;

  // if no path or no calledApplicationEntityTitle is specified, return
  if( dfPath.empty() || calledApplicationEntityTitle.empty() )
  {
    DCMWLM_ERROR("WlmDataSourceFileSystem::SetReadlock: Path to data source files not specified");
    return OFFalse;
  }

  // if a read lock has already been set, return
  if( readLockSetOnDataSource )
  {
    DCMWLM_ERROR("WlmDataSourceFileSystem::SetReadlock: Nested read locks not allowed!");
    return OFFalse;
  }

  // assign path to a local variable
  OFString lockname = dfPath;

  // if the given path does not show a PATH_SEPERATOR at the end, append one
  if( !lockname.empty() && lockname[lockname.length()-1] != PATH_SEPARATOR )
    lockname += PATH_SEPARATOR;

  // append calledApplicationEntityTitle, another PATH_SEPERATOR,
  // and LOCKFILENAME to the given path (and separator)
  lockname += calledApplicationEntityTitle;
  lockname += PATH_SEPARATOR;
  lockname += LOCKFILENAME;

  // open corresponding file
  handleToReadLockFile = open( lockname.c_str(), O_RDWR );
  if( handleToReadLockFile == -1 )
  {
    handleToReadLockFile = 0;
    DCMWLM_ERROR("WlmDataSourceFileSystem::SetReadlock: Cannot open file " << lockname
      << " (return code: " << OFStandard::getLastSystemErrorCode().message() << ")");
    return OFFalse;
  }

  // now set a read lock on the corresponding file

#ifdef _WIN32
  // windows does not have fcntl locking, we need to use our own function
  result = dcmtk_flock( handleToReadLockFile, LOCK_SH );
#else
  lockdata.l_type = F_RDLCK;
  lockdata.l_whence=0;
  lockdata.l_start=0;
  lockdata.l_len=0;
#if SIZEOF_VOID_P == SIZEOF_INT
  // some systems, e.g. NeXTStep, need the third argument for fcntl calls to be
  // casted to int. Other systems, e.g. OSF1-Alpha, won't accept this because int
  // and struct flock * have different sizes. The workaround used here is to use a
  // typecast to int if sizeof(void *) == sizeof(int) and leave it away otherwise.
  result = fcntl( handleToReadLockFile, F_SETLKW, (int)(&lockdata) );
#else
  result = fcntl( handleToReadLockFile, F_SETLKW, &lockdata );
#endif
#endif
  if( result == -1 )
  {
    DCMWLM_ERROR("WlmDataSourceFileSystem::SetReadlock: Cannot set read lock on file " << lockname);
    dcmtk_plockerr("return code");
    close( handleToReadLockFile );
    handleToReadLockFile = 0;
    return OFFalse;
  }

  // update member variable to indicate that a read lock has been set successfully
  readLockSetOnDataSource = OFTrue;

  // return success
  return OFTrue;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::ReleaseReadlock()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function releases a read lock on the LOCKFILE in the given directory.
// Parameters   : none.
// Return Value : OFTrue - The read lock has been released successfully.
//                OFFalse - The read lock has not been released successfully.
{
#ifndef _WIN32
  struct flock lockdata;
#endif
  int result;

  // if no read lock is set, return
  if( !readLockSetOnDataSource )
  {
    DCMWLM_WARN("WlmDataSourceFileSystem::ReleaseReadlock: No readlock to release");
    return OFFalse;
  }

  // now release read lock on the corresponding file

#ifdef _WIN32
  // windows does not have fcntl locking
  result = dcmtk_flock( handleToReadLockFile, LOCK_UN );
#else
  lockdata.l_type = F_UNLCK;
  lockdata.l_whence=0;
  lockdata.l_start=0;
  lockdata.l_len=0;
#if SIZEOF_VOID_P == SIZEOF_INT
  result = fcntl( handleToReadLockFile, F_SETLKW, (int)(&lockdata) );
#else
  result = fcntl( handleToReadLockFile, F_SETLKW, &lockdata );
#endif
#endif
  if( result == -1 )
  {
    DCMWLM_WARN("WlmDataSourceFileSystem::ReleaseReadlock: Cannot release read lock");
    dcmtk_plockerr("return code");
    return OFFalse;
  }

  // close read lock file
  close( handleToReadLockFile );
  handleToReadLockFile = 0;

  // update member variable to indicate that no read lock is set
  readLockSetOnDataSource = OFFalse;

  // return success
  return OFTrue;
}
