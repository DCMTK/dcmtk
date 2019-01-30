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
 *  Purpose: (Partially) abstract class for connecting to an arbitrary data source.
 *
 */

// ----------------------------------------------------------------------------

#include "dcmtk/config/osconfig.h"   // specific configuration for operating system

#include "dcmtk/dcmnet/dicom.h"      // for DIC_NODENAME etc. used in "wltypdef.h"
#include "dcmtk/dcmwlm/wltypdef.h"   // for type definitions
#include "dcmtk/ofstd/oftypes.h"     // for OFBool
#include "dcmtk/dcmdata/dcdatset.h"  // for DcmDataset
#include "dcmtk/dcmdata/dcmatch.h"   // for DcmAttributeMatching
#include "dcmtk/dcmdata/dcvrat.h"    // for DcmAttributTag
#include "dcmtk/dcmdata/dcvrlo.h"    // for DcmLongString
#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcdict.h"    // for global variable dcmDataDict
#include "dcmtk/dcmdata/dcdeftag.h"  // for DCM_OffendingElement, ...
#include "dcmtk/dcmdata/dcsequen.h"  // for DcmSequenceOfItems
#include "dcmtk/dcmdata/dcdicent.h"  // needed by MSVC5 with STL
#include "dcmtk/dcmwlm/wlds.h"

OFLogger DCM_dcmwlmLogger = OFLog::getLogger("dcmtk.dcmwlm");

// ----------------------------------------------------------------------------

WlmDataSource::WlmDataSource()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : failOnInvalidQuery( OFTrue ), calledApplicationEntityTitle(""),
    identifiers( NULL ), errorElements( NULL ), offendingElements( NULL ), errorComment( NULL ),
    foundUnsupportedOptionalKey( OFFalse ), readLockSetOnDataSource( OFFalse ),
    noSequenceExpansion( OFFalse ), returnedCharacterSet( RETURN_NO_CHARACTER_SET ), matchingDatasets(),
    specificCharacterSet( "" ), superiorSequenceArray( NULL ),
    numOfSuperiorSequences( 0 )
{
  // Make sure data dictionary is loaded.
  if( !dcmDataDict.isDictionaryLoaded() )
  {
    DCMWLM_WARN("No data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  // Initialize member variables.
  identifiers = new DcmDataset();
  offendingElements = new DcmAttributeTag( DCM_OffendingElement);
  errorElements = new DcmAttributeTag( DCM_OffendingElement);
  errorComment = new DcmLongString( DCM_ErrorComment);
}

// ----------------------------------------------------------------------------

WlmDataSource::~WlmDataSource()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // free memory
  ClearDataset(identifiers);
  delete identifiers;
  delete offendingElements;
  delete errorElements;
  delete errorComment;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetCalledApplicationEntityTitle( const OFString& value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Sets the member variable that specifies called application entity title.
// Parameters   : value - Value for calledApplicationEntityTitle.
// Return Value : none.
{
  calledApplicationEntityTitle = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetFailOnInvalidQuery( OFBool value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable that determines if a call should fail on an invalid query.
// Parameters   : value - Value for failOnInvalidQuery.
// Return Value : none.
{
  failOnInvalidQuery = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetNoSequenceExpansion( OFBool value )
// Date         : May 8, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  noSequenceExpansion = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetReturnedCharacterSet( WlmReturnedCharacterSetType value )
// Date         : March 21, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  returnedCharacterSet = value;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::CheckSearchMask( DcmDataset *searchMask )
// Date         : March 18, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if the search mask has a correct format. It returns OFTrue if this
//                is the case, OFFalse if this is not the case.
// Parameters   : searchMask - [in] Contains the search mask.
// Return Value : OFTrue  - The search mask has a correct format.
//                OFFalse - The search mask does not have a correct format.
{
  // Initialize counter that counts invalid elements in the search mask. This
  // variable will later be used to determine the return value for this function.
  int invalidMatchingKeyAttributeCount = 0;

  // remember the number of data elements in the search mask
  unsigned long numOfElementsInSearchMask = searchMask->card();

  // remember potentially specified specific character set
  searchMask->findAndGetOFString( DCM_SpecificCharacterSet, specificCharacterSet );

  // dump some information if required
  DCMWLM_INFO("Checking the search mask");

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

void WlmDataSource::CheckNonSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : March 8, 2002
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
      DCMWLM_WARN("Matching key attribute (" << tag.getTagName() << ") with invalid value encountered in the search mask");
    }
  }
  // if current element is not a supported matching key attribute, determine
  // if the current element is a supported return key attribute.
  else if( IsSupportedReturnKeyAttribute( element, supSequenceElement ) )
  {
    // we need to check if the current element (a supported return key attribute) does not contain a value.
    // According to the DICOM standard part 4, section K.2.2.1.2. a return key attribute which is NOT a
    // a matching key attribute must not contain a value. If one such attribute does contain a value,
    // i.e. if the current element's length does not equal 0, we want to dump a warning message.
    if( element->getLength() != 0 )
    {
      DCMWLM_INFO("  - Non-empty return key attribute (" << tag.getTagName() << ") encountered in the search mask." << OFendl
          << "    The specified value will be overridden.");
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

    // handle special case of "Specific Character Set"
    if( tag == DCM_SpecificCharacterSet )
    {
      DCMWLM_WARN("Attribute " << tag.getTagName() << " found in the search mask, value is neither checked nor used for matching");
    }
    else
    {
      // dump a warning
      DCMWLM_INFO("  - Unsupported (non-sequence) attribute (" << tag.getTagName() << ") encountered in the search mask." << OFendl
          << "    This attribute will not be existent in any result dataset.");

      // remember this attribute's tag in the list of error elements
      foundUnsupportedOptionalKey = OFTrue;
      PutErrorElements( tag );
    }
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::CheckSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : March 8, 2002
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
  // Be aware of the following remark: the DICOM standard specifies in part 4, section C.2.2.2.6
  // that if a search mask contains a sequence attribute which contains a single empty item, all
  // attributes from that particular sequence are in fact queried and shall be returned by the SCP.
  // This implementation accounts for this specification by expanding the search mask correspondingly.

  DcmElement *elem = NULL;

  // determine current element's tag
  DcmTag tag( element->getTag() );

  // remember that the current element is a sequence of items
  DcmSequenceOfItems *sequenceElement = OFstatic_cast(DcmSequenceOfItems *, element);

  // determine if the current sequence element is a supported matching or return key attribute
  if( IsSupportedMatchingKeyAttribute( element, supSequenceElement ) || IsSupportedReturnKeyAttribute( element, supSequenceElement ) )
  {
    // if the sequence attribute is supported, check if the length of the sequence equals 0, or if the sequence
    // contains exactly one empty item
    if( element->getLength() == 0 || ( sequenceElement->card() == 1 && sequenceElement->getItem(0)->card() == 0 ) )
    {
      // an empty sequence is not allowed in a C-FIND request
      if (element->getLength() == 0)
      {
        DCMWLM_WARN("Empty sequence (" << tag.getTagName() << ") encountered within the query, "
            << "treating as if an empty item within the sequence has been sent");
      }
      // if this is the case, we need to check the value of a variable
      // which pertains to a certain command line option
      if( noSequenceExpansion == OFFalse )
      {
        // if the user did not explicitly disable the expansion of empty sequences in C-FIND request
        // messages go ahead and expand this sequence according to the remark above
        ExpandEmptySequenceInSearchMask( element );
      }
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
        DCMWLM_ERROR("More than one item in sequence (" << tag.getTagName() << ") within the query encountered, "
            << "discarding all items except for the first one");
        invalidMatchingKeyAttributeCount++;

        // also, we want to delete all items except the first one
        unsigned long numOfItems = sequenceElement->card();
        for( unsigned long i=1 ; i<numOfItems ; i++ )
        {
          delete sequenceElement->remove( i );
        }
      }

      // (now, even though we might have encountered an error, we go on scrutinizing the search mask)

      // get the first (and only) item in the sequence of items
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
    DCMWLM_INFO("  - Unsupported (sequence) attribute (" << tag.getTagName() << ") encountered in the search mask." << OFendl
        << "    This attribute will not be existent in any result dataset.");

    // remember this attribute's tag in the list of error elements
    foundUnsupportedOptionalKey = OFTrue;
    PutErrorElements( tag );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::ExpandEmptySequenceInSearchMask( DcmElement *&element )
// Date         : March 8, 2002
// Author       : Thomas Wilkens
// Task         : According to the DICOM standard (part 4, section C.2.2.2.6), if a search mask
//                contains a sequence attribute which contains no item or a single empty item, all
//                attributes from that particular sequence are in fact queried and shall be returned
//                by the SCP. This implementation accounts for this specification by inserting a
//                corresponding single item with all required attributes into such empty sequences.
//                This function performs the insertion of the required item and attributes.
// Parameters   : element - [inout] Pointer to the currently processed element.
// Return Value : none.
{
  DcmItem *item = NULL;
  DcmElement *newElement = NULL;

  // remember that the current element is a sequence of items
  DcmSequenceOfItems *sequenceElement = OFstatic_cast(DcmSequenceOfItems *, element);

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

    // depending on what kind of supported sequence attribute
    // was passed, we have to insert different attributes
    DcmTagKey key = element->getTag().getXTag();
    if( key == DCM_ScheduledProcedureStepSequence )
    {
      newElement = new DcmApplicationEntity( DcmTag( DCM_ScheduledStationAETitle ) );      if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmDate( DcmTag( DCM_ScheduledProcedureStepStartDate ) );           if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmTime( DcmTag( DCM_ScheduledProcedureStepStartTime ) );           if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmCodeString( DcmTag( DCM_Modality ) );                            if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmPersonName( DcmTag( DCM_ScheduledPerformingPhysicianName ) );    if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_ScheduledProcedureStepDescription ) );   if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledStationName ) );               if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledProcedureStepLocation ) );     if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_PreMedication ) );                       if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledProcedureStepID ) );           if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_RequestedContrastAgent ) );              if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_CommentsOnTheScheduledProcedureStep ) ); if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmCodeString( DcmTag( DCM_ScheduledProcedureStepStatus ) );        if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmDate( DcmTag( DCM_ScheduledProcedureStepEndDate ) );             if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmTime( DcmTag( DCM_ScheduledProcedureStepEndTime ) );             if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmSequenceOfItems( DcmTag( DCM_ScheduledProtocolCodeSequence ) );
      if( item->insert( newElement ) != EC_Normal )
        delete newElement;
      else
      {
        DcmItem *item2 = new DcmItem();
        if( OFstatic_cast(DcmSequenceOfItems*, newElement)->insert( item2 ) != EC_Normal )
        {
          delete item2;
          item2 = NULL;
        }
        else
        {
          DcmElement *newElement2 = NULL;
          newElement2 = new DcmShortString( DcmTag( DCM_CodeValue ) );               if( item2->insert( newElement2 ) != EC_Normal ) delete newElement2;
          newElement2 = new DcmShortString( DcmTag( DCM_CodingSchemeVersion ) );     if( item2->insert( newElement2 ) != EC_Normal ) delete newElement2;
          newElement2 = new DcmShortString( DcmTag( DCM_CodingSchemeDesignator ) );  if( item2->insert( newElement2 ) != EC_Normal ) delete newElement2;
          newElement2 = new DcmLongString( DcmTag( DCM_CodeMeaning ) );              if( item2->insert( newElement2 ) != EC_Normal ) delete newElement2;
        }
      }
    }
    else if( key == DCM_ScheduledProtocolCodeSequence || key == DCM_RequestedProcedureCodeSequence )
    {
      newElement = new DcmShortString( DcmTag( DCM_CodeValue ) );               if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_CodingSchemeVersion ) );     if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_CodingSchemeDesignator ) );  if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_CodeMeaning ) );              if( item->insert( newElement ) != EC_Normal ) delete newElement;
    }
    else if( key == DCM_ReferencedStudySequence || key == DCM_ReferencedPatientSequence )
    {
      newElement = new DcmUniqueIdentifier( DcmTag( DCM_ReferencedSOPClassUID ) );       if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmUniqueIdentifier( DcmTag( DCM_ReferencedSOPInstanceUID ) );    if( item->insert( newElement ) != EC_Normal ) delete newElement;
    }
    else
    {
      // this code should never be executed; if it is, there is a logical error
      // in the source code and we want to dump a warning message
      DCMWLM_ERROR("WlmDataSource::ExpandEmptySequenceInSearchMask: Unsupported sequence attribute encountered");
    }
  }
  else
  {
    // this code should never be executed. if it is, we want to dump a warning message
    DCMWLM_ERROR("WlmDataSource::ExpandEmptySequenceInSearchMask: Unable to find item in sequence");
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::ClearDataset( DcmDataset *idents )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function removes all elements from the given DcmDataset object.
// Parameters   : idents - [in] pointer to object which shall be cleared.
// Return Value : none.
{
  // If the given pointer is valid and refers to a non-empty structure
  if( idents && (idents->card()>0) )
  {
    // clear structure
    idents->clear();
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::PutOffendingElements( const DcmTagKey &tag )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function inserts the tag of an offending element into the
//                corresponding member variable, unless this tag is already con-
//                tained in this variable.
// Parameters   : tag - [in] The tag that shall be inserted.
// Return Value : none.
{
  DcmTagKey errortag;

  // determine how many offending elements there have been so far
  unsigned long d = offendingElements->getVM();

  // if this is the first one, insert it at position 0
  if( d==0 )
  {
    offendingElements->putTagVal(tag, 0);
  }
  // if this is not the first one, insert it at the end
  // but only if it is not yet contained.
  else
  {
    OFBool tagFound = OFFalse;
    for( unsigned long j=0 ; j<d && !tagFound ; j++ )
    {
      offendingElements->getTagVal( errortag, j );
      if( errortag == tag )
        tagFound = OFTrue;
    }

    if( !tagFound )
      offendingElements->putTagVal( tag, d );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::PutErrorElements( const DcmTagKey &tag )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function inserts the tag of an error element into the
//                corresponding member variable, without checking if it is already
//                contained in this variable.
// Parameters   : tag - [in] The tag that shall be inserted.
// Return Value : none.
{
  // insert tag
  errorElements->putTagVal( tag, errorElements->getVM() );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::CheckMatchingKey( const DcmElement *elem )
// Date         : March 18, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the passed matching key's value only uses characters
//                which are part of its data type's character repertoire. Note that at the moment
//                this application supports the following matching key attributes:
//                   DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
//                    > DCM_ScheduledStationAETitle                        (0040,0001)  AE  R  1
//                    > DCM_ScheduledProcedureStepStartDate                (0040,0002)  DA  R  1
//                    > DCM_ScheduledProcedureStepStartTime                (0040,0003)  TM  R  1
//                    > DCM_Modality                                       (0008,0060)  CS  R  1
//                    > DCM_ScheduledPerformingPhysicianName               (0040,0006)  PN  R  2
//                   DCM_PatientName                                       (0010,0010)  PN  R  1
//                   DCM_ResponsiblePerson                                 (0010,2297)  PN  O  3
//                   DCM_ResponsiblePersonRole                             (0010,2298)  CS  O  3
//                   DCM_PatientID                                         (0010,0020)  LO  R  1
//                   DCM_AccessionNumber                                   (0008,0050)  SH  O  2
//                   DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
//                   DCM_ReferringPhysicianName                            (0008,0090)  PN  O  2
//                   DCM_PatientSex                                        (0010,0040)  CS  O  2
//                   DCM_RequestingPhysician                               (0032,1032)  PN  O  2
//                   DCM_AdmissionID                                       (0038,0010)  LO  O  2
//                   DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
//                   DCM_PatientBirthDate                                  (0010,0030)  DA  O  2
//                   DCM_IssuerOfPatientID                                 (0010,0021)  LO  O  3
//                   DCM_StudyDate                                         (0008,0020)  DA  O  3
//                   DCM_StudyTime                                         (0008,0030)  TM  O  3
//                As a result, the following data types have to be supported in this function:
//                AE, DA, TM, CS, PN, LO and SH. For the correct specification of these datatypes
//                2003 DICOM standard, part 5, section 6.2, table 6.2-1.
// Parameters   : elem - [in] Element which shall be checked.
// Return Value : OFTrue  - The given element's value only uses characters which are part of
//                          the element's data type's character repertoire.
//                OFFalse - The given element's value does not only use characters which are part of
//                          the element's data type's character repertoire.
{
  OFBool ok = OFTrue;
  OFString val;

  switch( elem->ident() )
  {
    case EVR_DA:
    case EVR_DT:
    case EVR_TM:
    {
      const char* data;
      size_t size;

      {
        char* c;
        Uint32 s;
        if( OFconst_cast( DcmElement*, elem )->getString( c, s ).bad() )
            break;
        data = c;
        size = s;
      }

      OFStandard::trimString( data, size );
      if( !size )
        break;

      switch( elem->ident() )
      {
      case EVR_DA:
        ok = DcmAttributeMatching::isDateQuery( data, size );
        break;
      case EVR_DT:
        ok = DcmAttributeMatching::isDateTimeQuery( data, size );
        break;
      case EVR_TM:
        ok = DcmAttributeMatching::isTimeQuery( data, size );
        break;
      default:
        ok = false;
        break;
      }

      if( !ok )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        OFString message( "Invalid value for an attribute with VR=" );
        message += DcmVR( elem->ident() ).getVRName();
        errorComment->putOFStringArray( message );
      }
    }
      break;

    case EVR_CS:
      // get string value
      ok = GetStringValue( elem, val );
      // check if value contains only valid characters
      if( ok && !ContainsOnlyValidCharacters( val.c_str(), "*?ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype CS");
        ok = OFFalse;
      }
      else
        ok = OFTrue;
      break;

    case EVR_AE:
      // get string value
      ok = GetStringValue( elem, val );
      // check if value contains only valid characters
      if( ok && !ContainsOnlyValidCharacters( val.c_str(), " !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype AE");
        ok = OFFalse;
      }
      else
        ok = OFTrue;
      break;

    case EVR_PN:
      // get string value
      ok = GetStringValue( elem, val );
      // check if value contains only valid characters (no ESC since only allowed for ISO 2022 encoding)
      if( ok && !ContainsOnlyValidCharacters( val.c_str(), "*? !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}" ) && specificCharacterSet == "" )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype PN");
        ok = OFFalse;
      }
      else
        ok = OFTrue;
      break;

    case EVR_LO:
      // get string value
      ok = GetStringValue( elem, val );
      // check if value contains only valid characters (no ESC since only allowed for ISO 2022 encoding)
      if( ok && !ContainsOnlyValidCharacters( val.c_str(), "*? !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}" ) && specificCharacterSet == "" )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype LO");
        ok = OFFalse;
      }
      else
        ok = OFTrue;
      break;

    case EVR_SH:
      // get string value
      ok = GetStringValue( elem, val );
      // check if value contains only valid characters (no ESC since only allowed for ISO 2022 encoding)
      if( ok && !ContainsOnlyValidCharacters( val.c_str(), "*? !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}" ) && specificCharacterSet == "" )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype SH");
        ok = OFFalse;
      }
      else
        ok = OFTrue;
      break;

    default:
      break;
  }

  return( ok );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::ContainsOnlyValidCharacters( const char *s, const char *charset )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function returns OFTrue if all the characters of s can be found
//                in the string charset.
// Parameters   : s       - [in] String which shall be checked.
//                charset - [in] Possible character set for s. (valid pointer expected.)
// Return Value : This function returns OFTrue if all the characters of s can be found
//                in the string charset. If s equals NULL, OFTrue will be returned.
{
  OFBool result = OFTrue;

  // check parameter
  if( s == NULL )
  {
    return OFTrue;
  }

  // return OFTrue if all the characters of s can be found in the string charset.
  size_t s_len = strlen( s );
  size_t charset_len = strlen( charset );
  for( size_t i=0 ; i<s_len && result ; i++ )
  {
    OFBool isSetMember = OFFalse;
    for( size_t j=0 ; !isSetMember && j<charset_len ; j++ )
    {
      if( s[i] == charset[j] )
        isSetMember = OFTrue;
    }

    if( !isSetMember )
      result = OFFalse;
  }

  return( result );
}

// ----------------------------------------------------------------------------

OFString WlmDataSource::DeleteLeadingAndTrailingBlanks( const OFString& value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function makes a copy of value without leading and trailing blanks.
// Parameters   : value - [in] The source string.
// Return Value : A copy of the given string without leading and trailing blanks.
{
  OFString returnValue = value;
  size_t pos = 0;

  // delete leading blanks
  while ( !returnValue.empty() && (returnValue[pos] == ' ') )
    pos++; // count blanks
  if (pos > 0)
    returnValue.erase(0, pos);

  // delete trailing blanks, start from end of string
  pos = returnValue.length() - 1;
  while ( !returnValue.empty() && (returnValue[pos] == ' ') )
    pos--;
  if (pos < returnValue.length() -1)
    returnValue.erase(pos);

  return returnValue;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::GetStringValue( const DcmElement *elem,
                                      OFString& resultVal )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function returns the value of the given DICOM string element (attribute)
//                in the parameter resultVal and returns OFTrue if successful.
//                If the element does not refer to a string attribute or contains no value,
//                OFFalse is returned.
// Parameters   : elem - [in] The DICOM element.
//                resultVal - [out] The resulting string value
// Return Value : OFTrue if string value could be accessed, OFFalse else
{
  DcmElement *elemNonConst = OFconst_cast(DcmElement*, elem);
  OFCondition result = elemNonConst->getOFStringArray( resultVal );
  if( result.bad() || resultVal.empty() )
    return OFFalse;
  return OFTrue;
}

// ----------------------------------------------------------------------------

DcmAttributeTag *WlmDataSource::GetOffendingElements()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function returns offending elements from member variable offendingElements
//                if there are any.
// Parameters   : none.
// Return Value : Pointer to offending elements or NULL if there are none.
{
  if( offendingElements->getLength() == 0 )
    return NULL;
  else
    return offendingElements;
}

// ----------------------------------------------------------------------------

DcmLongString *WlmDataSource::GetErrorComments()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function returns error comments from member variable errorComment
//                if there are any.
// Parameters   : none.
// Return Value : Pointer to error comments or NULL if there are none.
{
  if( errorComment->getLength() == 0 )
    return NULL;
  else
    return errorComment;
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSource::CancelFindRequest()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function handles a C-CANCEL Request during the processing of a C-FIND Request.
//                In detail, in case there are still matching datasets captured in member variable
//                matchingDatasets, memory for these datasets (and the array itself) is freed and
//                all pointers are set to NULL.
// Parameters   : none.
// Return Value : WLM_CANCEL.
{
  // remove all remaining datasets from result list
  while (!matchingDatasets.empty())
  {
    DcmDataset *dset = matchingDatasets.front();
    delete dset; dset = NULL;
    matchingDatasets.pop_front();
  }

  // return WLM_CANCEL
  return WLM_CANCEL;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsSupportedMatchingKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : December 12, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if the given element refers to an attribute which is a supported
//                matching key attribute. If this is the case OFTrue is returned, else OFFalse.
//                Currently, the following attributes are supported as matching keys:
//                   DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
//                    > DCM_ScheduledStationAETitle                        (0040,0001)  AE  R  1
//                    > DCM_ScheduledProcedureStepStartDate                (0040,0002)  DA  R  1
//                    > DCM_ScheduledProcedureStepStartTime                (0040,0003)  TM  R  1
//                    > DCM_Modality                                       (0008,0060)  CS  R  1
//                    > DCM_ScheduledPerformingPhysicianName               (0040,0006)  PN  R  2
//                   DCM_PatientName                                       (0010,0010)  PN  R  1
//                   DCM_ResponsiblePerson                                 (0010,2297)  PN  O  3
//                   DCM_ResponsiblePersonRole                             (0010,2298)  CS  O  3
//                   DCM_PatientID                                         (0010,0020)  LO  R  1
//                   DCM_AccessionNumber                                   (0008,0050)  SH  O  2
//                   DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
//                   DCM_ReferringPhysicianName                            (0008,0090)  PN  O  2
//                   DCM_PatientSex                                        (0010,0040)  CS  O  2
//                   DCM_RequestingPhysician                               (0032,1032)  PN  O  2
//                   DCM_AdmissionID                                       (0038,0010)  LO  O  2
//                   DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
//                   DCM_PatientBirthDate                                  (0010,0030)  DA  O  2
//                   DCM_IssuerOfPatientID                                 (0010,0021)  LO  O  3
//                   DCM_StudyDate                                         (0008,0020)  DA  O  3
//                   DCM_StudyTime                                         (0008,0030)  TM  O  3
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
          elementKey == DCM_ScheduledPerformingPhysicianName ) )
      isSupportedMatchingKeyAttribute = OFTrue;
  }
  else
  {
    if( elementKey == DCM_ScheduledProcedureStepSequence ||
        elementKey == DCM_PatientName                    ||
        elementKey == DCM_ResponsiblePerson              ||
        elementKey == DCM_ResponsiblePersonRole          ||
        elementKey == DCM_PatientID                      ||
        elementKey == DCM_IssuerOfPatientID              ||
        elementKey == DCM_StudyDate                      ||
        elementKey == DCM_StudyTime                      ||
        elementKey == DCM_AccessionNumber                ||
        elementKey == DCM_RequestedProcedureID           ||
        elementKey == DCM_ReferringPhysicianName         ||
        elementKey == DCM_PatientSex                     ||
        elementKey == DCM_RequestingPhysician            ||
        elementKey == DCM_AdmissionID                    ||
        elementKey == DCM_RequestedProcedurePriority     ||
        elementKey == DCM_PatientBirthDate )
      isSupportedMatchingKeyAttribute = OFTrue;
  }

  // return result
  return( isSupportedMatchingKeyAttribute );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsSupportedReturnKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement )
// Date         : December 12, 2001
// Author       : Thomas Wilkens
// Task         : This function checks if the given element refers to an attribute which is a supported
//                return key attribute. If this is the case OFTrue is returned, else OFFalse.
//                Currently, the following attributes are supported as return keys:
//                   DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
//                    > DCM_ScheduledStationAETitle                        (0040,0001)  AE  R  1
//                    > DCM_ScheduledProcedureStepStartDate                (0040,0002)  DA  R  1
//                    > DCM_ScheduledProcedureStepStartTime                (0040,0003)  TM  R  1
//                    > DCM_Modality                                       (0008,0060)  CS  R  1
//                    > DCM_ScheduledPerformingPhysicianName               (0040,0006)  PN  R  2
//                    > DCM_ScheduledProcedureStepDescription              (0040,0007)  LO  O  1
//                    > DCM_ScheduledStationName                           (0040,0010)  SH  O  2
//                    > DCM_ScheduledProcedureStepLocation                 (0040,0011)  SH  O  2
//                    > DCM_PreMedication                                  (0040,0012)  LO  O  2
//                    > DCM_ScheduledProcedureStepID                       (0040,0009)  SH  O  1
//                    > DCM_RequestedContrastAgent                         (0032,1070)  LO  O  2
//                    > DCM_CommentsOnTheScheduledProcedureStep            (0040,0400)  LT  O  3  (from the Scheduled Procedure Step Module)
//                    > DCM_ScheduledProcedureStepStatus                   (0040,0020)  CS  O  3
//                    > DCM_ScheduledProcedureStepEndDate                  (0040,0004)  DA  O  3  (from the Scheduled Procedure Step Module)
//                    > DCM_ScheduledProcedureStepEndTime                  (0040,0005)  TM  O  3  (from the Scheduled Procedure Step Module)
//                    > DCM_ScheduledProtocolCodeSequence                  (0040,0008)  SQ  O  1C
//                    > > DCM_CodeValue                                    (0008,0100)  SH  O  1C
//                    > > DCM_CodingSchemeVersion                          (0008,0103)  SH  O  3
//                    > > DCM_CodingSchemeDesignator                       (0080,0102)  SH  O  1C
//                    > > DCM_CodeMeaning                                  (0080,0104)  LO  O  3
//                   DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
//                   DCM_RequestedProcedureDescription                     (0032,1060)  LO  O  1
//                   DCM_StudyInstanceUID                                  (0020,000d)  UI  O  1
//                   DCM_StudyDate                                         (0008,0020)  DA  O  3
//                   DCM_StudyTime                                         (0008,0030)  TM  O  3
//                   DCM_ReferencedStudySequence                           (0008,1110)  SQ  O  2
//                    > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  1
//                    > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  1
//                   DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
//                   DCM_PatientTransportArrangements                      (0040,1004)  LO  O  2
//                   DCM_AccessionNumber                                   (0008,0050)  SH  O  2
//                   DCM_RequestingPhysician                               (0032,1032)  PN  O  2
//                   DCM_ReferringPhysicianName                            (0008,0090)  PN  O  2
//                   DCM_AdmissionID                                       (0038,0010)  LO  O  2
//                   DCM_CurrentPatientLocation                            (0038,0300)  LO  O  2
//                   DCM_ReferencedPatientSequence                         (0008,1120)  SQ  O  2
//                    > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  2
//                    > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  2
//                   DCM_PatientName                                       (0010,0010)  PN  R  1
//                   DCM_ResponsiblePerson                                 (0010,2297)  PN  O  3
//                   DCM_ResponsiblePersonRole                             (0010,2298)  CS  O  3
//                   DCM_PatientID                                         (0010,0020)  LO  R  1
//                   DCM_IssuerOfPatientID                                 (0010,0021)  LO  O  3  (from the Patient Identification Module)
//                   DCM_PatientBirthDate                                  (0010,0030)  DA  O  2
//                   DCM_PatientSex                                        (0010,0040)  CS  O  2
//                   DCM_PatientWeight                                     (0010,1030)  DS  O  2
//                   DCM_ConfidentialityConstraintOnPatientDataDescription (0040,3001)  LO  O  2
//                   DCM_PatientState                                      (0038,0500)  LO  O  2
//                   DCM_PregnancyStatus                                   (0010,21c0)  US  O  2
//                   DCM_MedicalAlerts                                     (0010,2000)  LO  O  2
//                   DCM_ContrastAllergies                                 (0010,2110)  LO  O  2
//                   DCM_SpecialNeeds                                      (0038,0050)  LO  O  2
//                   DCM_NamesOfIntendedRecipientsOfResults                (0040,1010)  PN  O  3  (from the Requested Procedure Module)
//                   DCM_InstitutionName                                   (0008,0080)  LO  O  3  (from the Visit Identification Module)
//                   DCM_AdmittingDiagnosesDescription                     (0008,1080)  LO  O  3  (from the Visit Admission Module)
//                   DCM_RETIRED_OtherPatientIDs                           (0010,1000)  LO  O  3  (from the Patient Identification Module)
//                   DCM_PatientSize                                       (0010,1020)  DS  O  3  (from the Patient Demographic Module)
//                   DCM_EthnicGroup                                       (0010,2160)  SH  O  3  (from the Patient Demographic Module)
//                   DCM_PatientComments                                   (0010,4000)  LT  O  3  (from the Patient Demographic Module)
//                   DCM_AdditionalPatientHistory                          (0010,21b0)  LT  O  3  (from the Patient Medical Module)
//                   DCM_LastMenstrualDate                                 (0010,21d0)  DA  O  3  (from the Patient Medical Module)
//                   DCM_InstitutionAddress                                (0008,0081)  ST  O  3  (from the Visit Identification Module)
//                   DCM_OtherPatientNames                                 (0010,1001)  PN  O  3  (from the Patient Identification Module)
//                   DCM_PatientAddress                                    (0010,1040)  LO  O  3  (from the Patient Demographic Module)
//                   DCM_MilitaryRank                                      (0010,1080)  LO  O  3  (from the Patient Demographic Module)
//                   DCM_SmokingStatus                                     (0010,21a0)  CS  O  3  (from the Patient Medical Module)
//                   DCM_RequestingService                                 (0032,1033)  LO  O  3  (from the Imaging Service Request Module)
//                   DCM_RETIRED_IssuerOfAdmissionID                       (0038,0011)  LO  O  3  (from the Visit Identification Module)
//                   DCM_ReasonForTheRequestedProcedure                    (0040,1002)  LO  O  3  (from the Requested Procedure Module)
//                   DCM_RequestedProcedureLocation                        (0040,1005)  LO  O  3  (from the Requested Procedure Module)
//                   DCM_ConfidentialityCode                               (0040,1008)  LO  O  3  (from the Requested Procedure Module)
//                   DCM_ReportingPriority                                 (0040,1009)  SH  O  3  (from the Requested Procedure Module)
//                   DCM_RequestedProcedureComments                        (0040,1400)  LT  O  3  (from the Requested Procedure Module)
//                   DCM_RETIRED_ReasonForTheImagingServiceRequest         (0040,2001)  LO  O  3  (from the Imaging Service Request Module)
//                   DCM_IssueDateOfImagingServiceRequest                  (0040,2004)  DA  O  3  (from the Imaging Service Request Module)
//                   DCM_IssueTimeOfImagingServiceRequest                  (0040,2005)  TM  O  3  (from the Imaging Service Request Module)
//                   DCM_OrderEnteredBy                                    (0040,2008)  PN  O  3  (from the Imaging Service Request Module)
//                   DCM_OrderEnterersLocation                             (0040,2009)  SH  O  3  (from the Imaging Service Request Module)
//                   DCM_OrderCallbackPhoneNumber                          (0040,2010)  SH  O  3  (from the Imaging Service Request Module)
//                   DCM_PlacerOrderNumberImagingServiceRequest            (0040,2016)  LO  O  3  (from the Imaging Service Request Module)
//                   DCM_FillerOrderNumberImagingServiceRequest            (0040,2017)  LO  O  3  (from the Imaging Service Request Module)
//                   DCM_ImagingServiceRequestComments                     (0040,2400)  LT  O  3  (from the Imaging Service Request Module)
//                   DCM_RequestedProcedureCodeSequence                    (0032,1064)  SQ  O  3  (from the Requested Procedure Module)
//                    > DCM_CodeValue                                      (0008,0100)  SH  O  1C
//                    > DCM_CodingSchemeVersion                            (0008,0103)  SH  O  3
//                    > DCM_CodingSchemeDesignator                         (0080,0102)  SH  O  1C
//                    > DCM_CodeMeaning                                    (0080,0104)  LO  O  3
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
          ( elementKey == DCM_ScheduledStationAETitle                    ||
            elementKey == DCM_ScheduledProcedureStepStartDate            ||
            elementKey == DCM_ScheduledProcedureStepStartTime            ||
            elementKey == DCM_Modality                                   ||
            elementKey == DCM_ScheduledPerformingPhysicianName           ||
            elementKey == DCM_ScheduledProcedureStepDescription          ||
            elementKey == DCM_ScheduledStationName                       ||
            elementKey == DCM_ScheduledProcedureStepLocation             ||
            elementKey == DCM_PreMedication                              ||
            elementKey == DCM_ScheduledProcedureStepID                   ||
            elementKey == DCM_RequestedContrastAgent                     ||
            elementKey == DCM_CommentsOnTheScheduledProcedureStep        ||
            elementKey == DCM_ScheduledProcedureStepStatus               ||
            elementKey == DCM_ScheduledProcedureStepEndDate              ||
            elementKey == DCM_ScheduledProcedureStepEndTime              ||
            elementKey == DCM_ScheduledProtocolCodeSequence ) )          ||
        ( supSequenceElementKey == DCM_ReferencedStudySequence        &&
          ( elementKey == DCM_ReferencedSOPClassUID                      ||
            elementKey == DCM_ReferencedSOPInstanceUID ) )               ||
        ( supSequenceElementKey == DCM_ReferencedPatientSequence      &&
          ( elementKey == DCM_ReferencedSOPClassUID                      ||
            elementKey == DCM_ReferencedSOPInstanceUID ) )               ||
        ( supSequenceElementKey == DCM_ScheduledProtocolCodeSequence  &&
          ( elementKey == DCM_CodeValue                                  ||
            elementKey == DCM_CodingSchemeVersion                        ||
            elementKey == DCM_CodingSchemeDesignator                     ||
            elementKey == DCM_CodeMeaning ) )                            ||
        ( supSequenceElementKey == DCM_RequestedProcedureCodeSequence &&
          ( elementKey == DCM_CodeValue                                  ||
            elementKey == DCM_CodingSchemeVersion                        ||
            elementKey == DCM_CodingSchemeDesignator                     ||
            elementKey == DCM_CodeMeaning ) ) )
      isSupportedReturnKeyAttribute = OFTrue;
  }
  else
  {
    if( elementKey == DCM_ScheduledProcedureStepSequence                    ||
        elementKey == DCM_RequestedProcedureID                              ||
        elementKey == DCM_RequestedProcedureDescription                     ||
        elementKey == DCM_StudyInstanceUID                                  ||
        elementKey == DCM_StudyDate                                         ||
        elementKey == DCM_StudyTime                                         ||
        elementKey == DCM_ReferencedStudySequence                           ||
        elementKey == DCM_RequestedProcedurePriority                        ||
        elementKey == DCM_PatientTransportArrangements                      ||
        elementKey == DCM_AccessionNumber                                   ||
        elementKey == DCM_RequestingPhysician                               ||
        elementKey == DCM_ReferringPhysicianName                            ||
        elementKey == DCM_AdmissionID                                       ||
        elementKey == DCM_CurrentPatientLocation                            ||
        elementKey == DCM_ReferencedPatientSequence                         ||
        elementKey == DCM_PatientName                                       ||
        elementKey == DCM_ResponsiblePerson                                 ||
        elementKey == DCM_ResponsiblePersonRole                             ||
        elementKey == DCM_PatientID                                         ||
        elementKey == DCM_IssuerOfPatientID                                 ||
        elementKey == DCM_PatientBirthDate                                  ||
        elementKey == DCM_PatientSex                                        ||
        elementKey == DCM_PatientWeight                                     ||
        elementKey == DCM_ConfidentialityConstraintOnPatientDataDescription ||
        elementKey == DCM_PatientState                                      ||
        elementKey == DCM_PregnancyStatus                                   ||
        elementKey == DCM_MedicalAlerts                                     ||
        elementKey == DCM_Allergies                                         ||
        elementKey == DCM_SpecialNeeds                                      ||
        elementKey == DCM_NamesOfIntendedRecipientsOfResults                ||
        elementKey == DCM_InstitutionName                                   ||
        elementKey == DCM_AdmittingDiagnosesDescription                     ||
        elementKey == DCM_RETIRED_OtherPatientIDs                           ||
        elementKey == DCM_PatientSize                                       ||
        elementKey == DCM_EthnicGroup                                       ||
        elementKey == DCM_PatientComments                                   ||
        elementKey == DCM_AdditionalPatientHistory                          ||
        elementKey == DCM_LastMenstrualDate                                 ||
        elementKey == DCM_InstitutionAddress                                ||
        elementKey == DCM_OtherPatientNames                                 ||
        elementKey == DCM_PatientAddress                                    ||
        elementKey == DCM_MilitaryRank                                      ||
        elementKey == DCM_SmokingStatus                                     ||
        elementKey == DCM_RequestingService                                 ||
        elementKey == DCM_RETIRED_IssuerOfAdmissionID                       ||
        elementKey == DCM_ReasonForTheRequestedProcedure                    ||
        elementKey == DCM_RequestedProcedureLocation                        ||
        elementKey == DCM_ConfidentialityCode                               ||
        elementKey == DCM_ReportingPriority                                 ||
        elementKey == DCM_RequestedProcedureComments                        ||
        elementKey == DCM_RETIRED_ReasonForTheImagingServiceRequest         ||
        elementKey == DCM_IssueDateOfImagingServiceRequest                  ||
        elementKey == DCM_IssueTimeOfImagingServiceRequest                  ||
        elementKey == DCM_OrderEnteredBy                                    ||
        elementKey == DCM_OrderEntererLocation                              ||
        elementKey == DCM_OrderCallbackPhoneNumber                          ||
        elementKey == DCM_PlacerOrderNumberImagingServiceRequest            ||
        elementKey == DCM_FillerOrderNumberImagingServiceRequest            ||
        elementKey == DCM_ImagingServiceRequestComments                     ||
        elementKey == DCM_RequestedProcedureCodeSequence )
      isSupportedReturnKeyAttribute = OFTrue;
  }

  // return result
  return( isSupportedReturnKeyAttribute );
}
