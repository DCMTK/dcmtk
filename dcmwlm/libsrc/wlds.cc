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
 *  Purpose: (Partially) abstract class for connecting to an arbitrary data source.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2003-12-23 13:04:39 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/wlds.cc,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"  // specific configuration for operating system

#include "dicom.h"     // for DIC_NODENAME etc. used in "wltypdef.h"
#include "wltypdef.h"  // for type definitions
#include "oftypes.h"   // for OFBool
#include "dcdatset.h"  // for DcmDataset
#include "dcvrat.h"    // for DcmAttributTag
#include "dcvrlo.h"    // for DcmLongString
#include "dcvrae.h"
#include "dcvrda.h"
#include "dcvrcs.h"
#include "dcvrpn.h"
#include "dcvrtm.h"
#include "dcvrsh.h"
#include "dcdict.h"    // for global variable dcmDataDict
#include "dcdeftag.h"  // for DCM_OffendingElement, ...
#include "dcsequen.h"  // for DcmSequenceOfItems
#include "dcdicent.h"  // needed by MSVC5 with STL
#include "wlds.h"

// ----------------------------------------------------------------------------

WlmDataSource::WlmDataSource()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : failOnInvalidQuery( OFTrue ), calledApplicationEntityTitle( NULL ), verbose( OFFalse ), debug( OFFalse ),
    identifiers( NULL ), errorElements( NULL ), offendingElements( NULL ), errorComment( NULL ),
    foundUnsupportedOptionalKey( OFFalse ), readLockSetOnDataSource( OFFalse ), logStream( NULL ),
    noSequenceExpansion( OFFalse ), returnedCharacterSet( RETURN_NO_CHARACTER_SET ), matchingDatasets( NULL ),
    numOfMatchingDatasets( 0 )
{
  char msg[200];

  // Make sure data dictionary is loaded.
  if( !dcmDataDict.isDictionaryLoaded() )
  {
    sprintf( msg, "Warning: no data dictionary loaded, check environment variable: %s\n", DCM_DICT_ENVIRONMENT_VARIABLE );
    DumpMessage( msg );
  }

  // Initialize member variables.
  identifiers = new DcmDataset();
  offendingElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorComment = new DcmLongString( DCM_ErrorComment, 0 );
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
  if( calledApplicationEntityTitle != NULL ) delete calledApplicationEntityTitle;
  ClearDataset(identifiers);
  delete identifiers;
  delete offendingElements;
  delete errorElements;
  delete errorComment;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetCalledApplicationEntityTitle( char *value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Sets the member variable that specifies called application entity title.
// Parameters   : value - Value for calledApplicationEntityTitle.
// Return Value : none.
{
  if( value != NULL )
  {
    if( calledApplicationEntityTitle != NULL ) delete calledApplicationEntityTitle;
    calledApplicationEntityTitle = new char[ strlen( value ) + 1 ];
    strcpy( calledApplicationEntityTitle, value );
  }
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

void WlmDataSource::SetLogStream( OFConsole *value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  logStream = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetVerbose( OFBool value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable that determines if information shall be dumped at processing time or not.
// Parameters   : value - Value for verbose.
// Return Value : none.
{
  verbose = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetDebug( OFBool value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable that determines if information shall be dumped at processing time or not.
// Parameters   : value - Value for verbose.
// Return Value : none.
{
  debug = value;
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
  char msg[300];
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
      sprintf( msg, "WlmDataSource::CheckNonSequenceElementInSearchMask : Error: Matching key attribute (%s) with invalid value encountered in the search mask.", tag.getTagName() );
      DumpMessage( msg );
    }
  }
  // if current element is not a supported matching key attribute, determine
  // if the current element is a supported return key attribute.
  else if( IsSupportedReturnKeyAttribute( element, supSequenceElement ) )
  {
    // check if the current supported return key attribute is not the "Specific Character Set" (0008,0005) attribute.
    // (For this specific attribute there is nothing to do here.)
    if( element->getTag().getXTag() != DCM_SpecificCharacterSet )
    {
      // in case the current element is not the "Specific Character Set" attribute, we need to check
      // if the current element (a supported return key attribute) does not contain a value. According
      // to the 2001 DICOM standard part 4, section K.2.2.1.2. a return key attribute which is NOT a
      // a matching key attribute must not contain a value. If one such attribute does contain a value,
      // i.e. if the current element's length does not equal 0, we want to dump a warning message.
      if( element->getLength() != 0 )
      {
        if( verbose )
          DumpMessage( "  - Non-empty return key attribute encountered in the search mask.\n    The specified value will be overridden." );
      }
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
      // if this is the case, we need to check the value of a variable
      // which pertains to a certain command line option
      if( noSequenceExpansion == OFFalse )
      {
        // if the user did not explicitely disable the expansion of empty sequences in C-FIND request
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
        sprintf( msg, "WlmDataSource::CheckSequenceElementInSearchMask : Error: More than one item in sequence %s (in the search mask) encountered.", tag.getTagName() );
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

void WlmDataSource::ExpandEmptySequenceInSearchMask( DcmElement *&element )
// Date         : March 8, 2002
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
      // we need to insert 11 different required attributes (see 2001 DICOM standard, Table K.6-1.)
      newElement = new DcmApplicationEntity( DcmTag( DCM_ScheduledStationAETitle ) );      if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmDate( DcmTag( DCM_ScheduledProcedureStepStartDate ) );           if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmTime( DcmTag( DCM_ScheduledProcedureStepStartTime ) );           if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmCodeString( DcmTag( DCM_Modality ) );                            if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmPersonName( DcmTag( DCM_ScheduledPerformingPhysiciansName ) );   if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_ScheduledProcedureStepDescription ) );   if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledStationName ) );               if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledProcedureStepLocation ) );     if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_PreMedication ) );                       if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmShortString( DcmTag( DCM_ScheduledProcedureStepID ) );           if( item->insert( newElement ) != EC_Normal ) delete newElement;
      newElement = new DcmLongString( DcmTag( DCM_RequestedContrastAgent ) );              if( item->insert( newElement ) != EC_Normal ) delete newElement;
      // and we also add one supported optional attributes
      newElement = new DcmLongString( DcmTag( DCM_CommentsOnTheScheduledProcedureStep ) ); if( item->insert( newElement ) != EC_Normal ) delete newElement;
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
      DumpMessage( "WlmDataSource::ExpandEmptySequenceInSearchMask : Error: Unsupported sequence attribute encountered." );
    }
  }
  else
  {
    // this code should never be executed. if it is, we want to dump a warning message
    DumpMessage( "WlmDataSource::ExpandEmptySequenceInSearchMask : Error: Unable to find item in sequence." );
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

void WlmDataSource::PutOffendingElements( DcmTagKey &tag )
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

void WlmDataSource::PutErrorElements( DcmTagKey &tag )
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

OFBool WlmDataSource::CheckMatchingKey( DcmElement *elem )
// Date         : March 18, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the passed matching key's value only uses characters
//                which are part of its data type's character repertoire. Note that at the moment
//                this application supports the following matching key attributes:
//                  - DCM_ScheduledProcedureStepSequence         (0040,0100)  SQ  R  1
//                     > DCM_ScheduledStationAETitle             (0040,0001)  AE  R  1
//                     > DCM_ScheduledProcedureStepStartDate     (0040,0002)  DA  R  1
//                     > DCM_ScheduledProcedureStepStartTime     (0040,0003)  TM  R  1
//                     > DCM_Modality                            (0008,0060)  CS  R  1
//                     > DCM_ScheduledPerformingPhysiciansName   (0040,0006)  PN  R  2
//                  - DCM_PatientsName                           (0010,0010)  PN  R  1
//                  - DCM_PatientID                              (0010,0020)  LO  R  1
//                As a result, the following data types have to be supported in this function:
//                AE, DA, TM, CS, PN and LO. For the correct specification of these datatypes
//                2001 DICOM standard, part 5, section 6.2, table 6.2-1.
// Parameters   : elem - [in] Element which shall be checked.
// Return Value : OFTrue  - The given element's value only uses characters which are part of
//                          the element's data type's character repertoire.
//                OFFalse - The given element's value does not only use characters which are part of
//                          the element's data type's character repertoire.
{
  OFBool ok = OFTrue;
  char *val = NULL;

  switch( elem->ident() )
  {
    case EVR_DA:
      // get string value
      val = GetStringValue( elem );
      // if there is a value and if the value is not a date or a date range, return invalid value
      if( val != NULL && !IsValidDateOrDateRange( val ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid value for an attribute of datatype DA");
        ok = OFFalse;
      }
      break;

    case EVR_TM:
      // get string value
      val = GetStringValue( elem );
      // if there is a value and if the value is not a time or a time range, return invalid value
      if( val != NULL && !IsValidTimeOrTimeRange( val ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid value for an attribute of datatype TM");
        ok = OFFalse;
      }
      break;

    case EVR_CS:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, "*?ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype CS");
        ok = OFFalse;
      }
      break;

    case EVR_AE:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, "*? !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype AE");
        ok = OFFalse;
      }
      break;

    case EVR_PN:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, "*? !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\033" ) )  // ESC=\033
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype PN");
        ok = OFFalse;
      }
      break;

    case EVR_LO:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, " !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\033\012\014\015" ) )  // ESC=\033, LF=\012, FF=\014, CR=\015
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype LO");
        ok = OFFalse;
      }
      break;

    default:
      break;
  }

  return( ok );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidDateOrDateRange( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given value is a valid date or date range.
// Parameters   : value - [in] The value which shall be checked.
// Return Value : OFTrue  - The given value is a valid date or date range.
//                OFFalse - The given value is not a valid date or date range.
{
  char *tmp;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *dateRange = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( dateRange ) == 0 )
  {
    delete dateRange;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( dateRange, "0123456789.-" ) )
  {
    delete dateRange;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidDateRange = OFFalse;

  // Determine if a hyphen occurs in the date range
  char *hyphen = strchr( dateRange, '-' );
  if( hyphen != NULL )
  {
    // determine if two date values are given or not
    if( dateRange[0] == '-' )
    {
      // if the hyphen occurs at the beginning, there is just one date value which has to be checked for validity
      tmp = dateRange;
      tmp++;
      isValidDateRange = IsValidDate( tmp );
    }
    else if( dateRange[ strlen( dateRange ) - 1 ] == '-' )
    {
      // if the hyphen occurs at the end, there is just one date value which has to be checked for validity
      char *newDateRange = new char[ strlen( dateRange ) - 1 + 1 ];
      strncpy( newDateRange, dateRange, strlen( dateRange ) - 1 );
      newDateRange[ strlen( dateRange ) - 1 ] = '\0';
      isValidDateRange = IsValidDate( newDateRange );
      delete newDateRange;
    }
    else
    {
      // in this case the hyphen occurs somewhere in between beginning and end; hence there are two date values
      // which have to be checked for validity. Determine where the hyphen occurs exactly
      int indexval = hyphen - dateRange;

      // determine the first date
      char *date1 = new char[ indexval + 1 ];
      strncpy( date1, dateRange, indexval );
      date1[indexval] = '\0';

      // determine the second date
      tmp = hyphen;
      tmp++;
      char *date2 = new char[ strlen( tmp ) + 1 ];
      strcpy( date2, tmp );

      // check both dates for validity
      if( IsValidDate( date1 ) && IsValidDate( date2 ) )
        isValidDateRange = OFTrue;

      // free memory
      delete date1;
      delete date2;
    }
  }
  else
  {
    // if there is no hyphen, there is just one date value which has to be checked for validity
    isValidDateRange = IsValidDate( dateRange );
  }

  // free memory
  delete dateRange;

  // return result
  return( isValidDateRange );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidDate( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given date value is valid.
//                According to the 2001 DICOM standard, part 5, Table 6.2-1, a date
//                value is either in format "yyyymmdd" or in format "yyyy.mm.dd",
//                so that e.g. "19840822" represents August 22, 1984.
// Parameters   : value - [in] The value which shall be checked.
// Return Value : OFTrue  - Date is valid.
//                OFFalse - Date is not valid.
{
  int year=0, month=0, day=0;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *date = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( date ) == 0 )
  {
    delete date;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( date, "0123456789." ) )
  {
    delete date;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidDate = OFFalse;

  // check which of the two formats applies to the given string
  if( strlen( date ) == 8 )
  {
    // scan given date string
    sscanf( date, "%4d%2d%2d", &year, &month, &day );
    if( year > 0 && month >= 1 && month <= 12 && day >= 1 && day <= 31 )
      isValidDate = OFTrue;
  }
  else if( strlen( date ) == 10 )
  {
    // scan given date string
    sscanf( date, "%4d.%2d.%2d", &year, &month, &day );
    if( year > 0 && month >= 1 && month <= 12 && day >= 1 && day <= 31 )
      isValidDate = OFTrue;
  }

  // free memory
  delete date;

  // return result
  return( isValidDate );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidTimeOrTimeRange( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given value is a valid time or time range.
// Parameters   : timeRange - [in] The value which shall be checked.
// Return Value : OFTrue  - The given value is a valid time or time range.
//                OFFalse - The given value is not a valid time or time range.
{
  char *tmp;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *timeRange = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( timeRange ) == 0 )
  {
    delete timeRange;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( timeRange, "0123456789.:-" ) )
  {
    delete timeRange;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidTimeRange = OFFalse;

  // Determine if a hyphen occurs in the time range
  char *hyphen = strchr( timeRange, '-' );
  if( hyphen != NULL )
  {
    // determine if two time values are given or not
    if( timeRange[0] == '-' )
    {
      // if the hyphen occurs at the beginning, there is just one time value which has to be checked for validity
      tmp = timeRange;
      tmp++;
      isValidTimeRange = IsValidTime( tmp );
    }
    else if( timeRange[ strlen( timeRange ) - 1 ] == '-' )
    {
      // if the hyphen occurs at the end, there is just one time value which has to be checked for validity
      char *newTimeRange = new char[ strlen( timeRange ) - 1 + 1 ];
      strncpy( newTimeRange, timeRange, strlen( timeRange ) - 1 );
      newTimeRange[ strlen( timeRange ) - 1 ] = '\0';
      isValidTimeRange = IsValidTime( newTimeRange );
      delete newTimeRange;
    }
    else
    {
      // in this case the hyphen occurs somewhere in between beginning and end; hence there are two time values
      // which have to be checked for validity. Determine where the hyphen occurs exactly
      int indexval = hyphen - timeRange;

      // determine the first time
      char *time1 = new char[ indexval + 1 ];
      strncpy( time1, timeRange, indexval );
      time1[indexval] = '\0';

      // determine the second time
      tmp = hyphen;
      tmp++;
      char *time2 = new char[ strlen( tmp ) + 1 ];
      strcpy( time2, tmp );

      // check both times for validity
      if( IsValidTime( time1 ) && IsValidTime( time2 ) )
        isValidTimeRange = OFTrue;

      // free memory
      delete time1;
      delete time2;
    }
  }
  else
  {
    // if there is no hyphen, there is just one date value which has to be checked for validity
    isValidTimeRange = IsValidTime( timeRange );
  }

  // free memory
  delete timeRange;

  // return result
  return( isValidTimeRange );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidTime( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given time value is valid.
//                According to the 2001 DICOM standard, part 5, Table 6.2-1, a time
//                value is either in format "hhmmss.fracxx" or "hh:mm:ss.fracxx" where
//                 - hh represents the hour (0-23)
//                 - mm represents the minutes (0-59)
//                 - ss represents the seconds (0-59)
//                 - fracxx represents the fraction of a second in millionths of seconds (000000-999999)
//                Note that one or more of the components mm, ss, or fracxx may be missing as
//                long as every component to the right of a missing component is also missing.
//                If fracxx is missing, the "." character in front of fracxx is also missing.
// Parameters   : value - [in] The value which shall be checked.
// Return Value : OFTrue  - Time is valid.
//                OFFalse - Time is not valid.
{
  int hour=0, min=0, sec=0, frac=0, fieldsRead=0;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *timevalue = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( timevalue ) == 0 )
  {
    delete timevalue;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( timevalue, "0123456789.:" ) )
  {
    delete timevalue;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidTime = OFFalse;

  // check which of the two formats applies to the given string
  char *colon = strchr( timevalue, ':' );
  if( colon != NULL )
  {
    // time format is "hh:mm:ss.fracxx"

    // check which components are missing
    if( strlen( timevalue ) == 5 )
    {
      // scan given time string "hh:mm"
      fieldsRead = sscanf( timevalue, "%2d:%2d", &hour, &min );
      if( fieldsRead == 2 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timevalue ) == 8 )
    {
      // scan given time string "hh:mm:ss"
      fieldsRead = sscanf( timevalue, "%2d:%2d:%2d", &hour, &min, &sec );
      if( fieldsRead == 3 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timevalue ) > 8 && strlen( timevalue ) < 16 )
    {
      // scan given time string "hh:mm:ss.fracxx"
      fieldsRead = sscanf( timevalue, "%2d:%2d:%2d.%6d", &hour, &min, &sec, &frac );
      if( fieldsRead == 4 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 && frac >= 0 && frac <= 999999 )
        isValidTime = OFTrue;
    }
  }
  else
  {
    // time format is "hhmmss.fracxx"

    // check which components are missing
    if( strlen( timevalue ) == 4 )
    {
      // scan given time string "hhmm"
      fieldsRead = sscanf( timevalue, "%2d%2d", &hour, &min );
      if( fieldsRead == 2 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timevalue ) == 6 )
    {
      // scan given time string "hhmmss"
      fieldsRead = sscanf( timevalue, "%2d%2d%2d", &hour, &min, &sec );
      if( fieldsRead == 3 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timevalue ) > 6 && strlen( timevalue ) < 14 )
    {
      // scan given time string "hhmmss.fracxx"
      fieldsRead = sscanf( timevalue, "%2d%2d%2d.%6d", &hour, &min, &sec, &frac );
      if( fieldsRead == 4 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 && frac >= 0 && frac <= 999999 )
        isValidTime = OFTrue;
    }
  }

  // free memory
  delete timevalue;

  // return result
  return( isValidTime );
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
  int s_len = strlen( s );
  int charset_len = strlen( charset );
  for( int i=0 ; i<s_len && result ; i++ )
  {
    OFBool isSetMember = OFFalse;
    for( int j=0 ; !isSetMember && j<charset_len ; j++ )
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

char *WlmDataSource::DeleteLeadingAndTrailingBlanks( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function makes a copy of value without leading and trailing blanks.
// Parameters   : value - [in] The source string.
// Return Value : A copy of the given string without leading and trailing blanks.
{
  char *returnValue;

  unsigned int i;
  OFBool stop = OFFalse;
  for( i=0 ; i<strlen(value) && !stop ; )
  {
    if( value[i] == ' ' )
      value++;
    else
      stop = OFTrue;
  }
  returnValue = new char[ strlen( value ) + 1 ];
  strcpy( returnValue, value );

  if( strlen( returnValue ) > 0 )
  {
    stop = OFFalse;
    for( i=strlen(returnValue)-1 ; i>0 && !stop ; i-- )
    {
      if( returnValue[i] == ' ' )
        returnValue[i] = '\0';
      else
        stop = OFTrue;
    }
  }

  return( returnValue );
}

// ----------------------------------------------------------------------------

char *WlmDataSource::GetStringValue( DcmElement *elem )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function returns the value of the given DICOM string element (attribute).
//                If the element does not refer to a string attribute, a NULL pointer is returned.
// Parameters   : elem - [in] The DICOM element.
// Return Value : The value of the given DICOM (string) element or NULL.
{
  char *val = NULL;

  if( elem && elem->isaString() )
    elem->getString( val );

  return val;
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
  // remove all remaining elements in the array and the array itself, if the array is not NULL
  if( matchingDatasets != NULL )
  {
    for( unsigned long i=0 ; i<numOfMatchingDatasets ; i++ )
      delete matchingDatasets[i];
    delete matchingDatasets;
    matchingDatasets = NULL;
    numOfMatchingDatasets = 0;
  }

  // return WLM_CANCEL
  return WLM_CANCEL;
}

// ----------------------------------------------------------------------------

void WlmDataSource::DumpMessage( const char *message )
// Date         : December 10, 2001
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
//                    > DCM_ScheduledPerformingPhysiciansName              (0040,0006)  PN  R  2
//                   DCM_PatientsName                                      (0010,0010)  PN  R  1
//                   DCM_PatientID                                         (0010,0020)  LO  R  1
//                   DCM_AccessionNumber                                   (0008,0050)  SH  O  2
//                   DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
//                   DCM_ReferringPhysiciansName                           (0008,0090)  PN  O  2
//                   DCM_PatientsSex                                       (0010,0040)  CS  O  2
//                   DCM_RequestingPhysician                               (0032,1032)  PN  O  2
//                   DCM_AdmissionID                                       (0038,0010)  LO  O  2
//                   DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
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
        elementKey == DCM_PatientID                      ||
        elementKey == DCM_AccessionNumber                ||
        elementKey == DCM_RequestedProcedureID           ||
        elementKey == DCM_ReferringPhysiciansName        ||
        elementKey == DCM_PatientsSex                    ||
        elementKey == DCM_RequestingPhysician            ||
        elementKey == DCM_AdmissionID                    ||
        elementKey == DCM_RequestedProcedurePriority )
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
//                   DCM_SpecificCharacterSet                              (0008,0005)  CS  O  1
//                   DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
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
//                    > DCM_CommentsOnTheScheduledProcedureStep            (0040,0400)  LT  O  3  (from the Scheduled Procedure Step Module)
//                   DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
//                   DCM_RequestedProcedureDescription                     (0032,1060)  LO  O  1
//                   DCM_StudyInstanceUID                                  (0020,000d)  UI  O  1
//                   DCM_ReferencedStudySequence                           (0008,1110)  SQ  O  2
//                    > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  1  Note that the standard specifies this attribute as 1. unfortunately, this implementation only supports this attribute as 2. Also note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                    > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  1  Note that the standard specifies this attribute as 1. unfortunately, this implementation only supports this attribute as 2. Also note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                   DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
//                   DCM_PatientTransportArrangements                      (0040,1004)  LO  O  2
//                   DCM_AccessionNumber                                   (0008,0050)  SH  O  2
//                   DCM_RequestingPhysician                               (0032,1032)  PN  O  2
//                   DCM_ReferringPhysiciansName                           (0008,0090)  PN  O  2
//                   DCM_AdmissionID                                       (0038,0010)  LO  O  2
//                   DCM_CurrentPatientLocation                            (0038,0300)  LO  O  2
//                   DCM_ReferencedPatientSequence                         (0008,1120)  SQ  O  2
//                    > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  2  Note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                    > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  2  Note that currently there are two ReferencedSOPClassUID attributes in two different sequences. For these two attributes, always the same values will be returned by this SCP.
//                   DCM_PatientsName                                      (0010,0010)  PN  R  1
//                   DCM_PatientID                                         (0010,0020)  LO  R  1
//                   DCM_PatientsBirthDate                                 (0010,0030)  DA  O  2
//                   DCM_PatientsSex                                       (0010,0040)  CS  O  2
//                   DCM_PatientsWeight                                    (0010,1030)  DS  O  2
//                   DCM_ConfidentialityConstraintOnPatientDataDescription (0040,3001)  LO  O  2
//                   DCM_PatientState                                      (0038,0500)  LO  O  2
//                   DCM_PregnancyStatus                                   (0010,21c0)  US  O  2
//                   DCM_MedicalAlerts                                     (0010,2000)  LO  O  2
//                   DCM_ContrastAllergies                                 (0010,2110)  LO  O  2
//                   DCM_SpecialNeeds                                      (0038,0050)  LO  O  2
//                   DCM_NamesOfIntendedRecipientsOfResults                (0040,1010)  PN  O  3  (from the Requested Procedure Module)
//                   DCM_InstitutionName                                   (0008,0080)  LO  O  3  (from the Visit Identification Module)
//                   DCM_AdmittingDiagnosesDescription                     (0008,1080)  LO  O  3  (from the Visit Admission Module)
//                   DCM_OtherPatientIDs                                   (0010,1000)  LO  O  3  (from the Patient Identification Module)
//                   DCM_PatientsSize                                      (0010,1020)  DS  O  3  (from the Patient Demographic Module)
//                   DCM_EthnicGroup                                       (0010,2160)  SH  O  3  (from the Patient Demographic Module)
//                   DCM_PatientComments                                   (0010,4000)  LT  O  3  (from the Patient Demographic Module)
//                   DCM_AdditionalPatientHistory                          (0010,21b0)  LT  O  3  (from the Patient Medical Module)
//                   DCM_LastMenstrualDate                                 (0010,21d0)  DA  O  3  (from the Patient Medical Module)
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
            elementKey == DCM_RequestedContrastAgent            ||
            elementKey == DCM_CommentsOnTheScheduledProcedureStep ) )     ||
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
    if( elementKey == DCM_SpecificCharacterSet                              ||
        elementKey == DCM_ScheduledProcedureStepSequence                    ||
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
        elementKey == DCM_NamesOfIntendedRecipientsOfResults                ||
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
** $Log: wlds.cc,v $
** Revision 1.13  2003-12-23 13:04:39  wilkens
** Integrated new matching key attributes into wlmscpfs.
**
** Revision 1.12  2003/08/21 13:39:39  wilkens
** Moved declaration and initialization of member variables matchingDatasets and
** numOfMatchingDatasets to base class.
** Got rid of superfluous member variable objlist and of superfluous function
** ClearObjList().
**
** Revision 1.11  2003/06/04 14:28:37  meichel
** Added various includes needed by MSVC5 with STL
**
** Revision 1.10  2003/02/17 12:02:09  wilkens
** Made some minor modifications to be able to modify a special variant of the
** worklist SCP implementation (wlmscpki).
**
** Revision 1.9  2002/12/03 12:15:56  wilkens
** Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
** so that dcmwlm can now completely replace wlistctn in the public domain part
** of dcmtk. Pertaining to this replacement requirement, another optional return
** key attribute was integrated into the wlm utilities.
**
** Revision 1.8  2002/08/12 10:56:14  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.7  2002/07/17 13:10:42  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.6  2002/06/10 11:58:26  wilkens
** Some more corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.5  2002/06/10 11:25:10  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.4  2002/04/18 14:20:23  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.3  2002/01/08 17:46:03  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 17:02:55  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:32:46  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
