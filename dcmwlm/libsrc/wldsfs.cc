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
 *  Purpose: Class for connecting to a file-based data source.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-08-05 09:10:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/wldsfs.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"  // specific configuration for operating system
BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>     // for O_RDWR
#endif
END_EXTERN_C
#include "dicom.h"
#include "wltypdef.h"
#include "oftypes.h"
#include "dcdatset.h"
#include "dcsequen.h"
#include "dcvrat.h"
#include "dcvrlo.h"
#include "dcdeftag.h"
#include "dcvrcs.h"
#include "wlds.h"
#include "wlfsim.h"
#include "ofstd.h"

#include "wldsfs.h"

// ----------------------------------------------------------------------------

WlmDataSourceFileSystem::WlmDataSourceFileSystem()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : WlmDataSource(), fileSystemInteractionManager( NULL ), matchingDatasets( NULL ),
    numOfMatchingDatasets( 0 ), dfPath( NULL ), handleToReadLockFile( 0 )
{
  fileSystemInteractionManager = new WlmFileSystemInteractionManager();
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

  //free memory
  delete fileSystemInteractionManager;
  if( dfPath != NULL ) delete dfPath;
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceFileSystem::ConnectToDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Connects to the data source.
// Parameters   : none.
// Return Value : Indicates if the connection was established succesfully.
{
  // set variables in fileSystemInteractionManager object
  fileSystemInteractionManager->SetLogStream( logStream );
  fileSystemInteractionManager->SetVerbose( verbose );

  // connect to file system
  OFCondition cond = fileSystemInteractionManager->ConnectToFileSystem( dfPath );

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceFileSystem::DisconnectFromDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Disconnects from the data source.
// Parameters   : none.
// Return Value : Indicates if the disconnection was completed succesfully.
{
  // disconnect from file system
  OFCondition cond = fileSystemInteractionManager->DisconnectFromFileSystem();

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::SetDfPath( const char *value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( dfPath != NULL ) delete dfPath;
    dfPath = new char[ strlen(value) + 1 ];
    strcpy( dfPath, value );
  }
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
  if( calledApplicationEntityTitle == NULL )
    return( OFFalse );
  else
    return( fileSystemInteractionManager->IsCalledApplicationEntityTitleSupported( calledApplicationEntityTitle ) );
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSourceFileSystem::StartFindRequest( DcmDataset &findRequestIdentifiers )
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

  // Set a read lock on the worklist files which shall be read from.
  SetReadlock();

  // dump some information if required
  if( verbose )
    DumpMessage( "Determining matching records from worklist files." );

  // Determine records from worklist files which match the search mask
  unsigned long numOfMatchingRecords = fileSystemInteractionManager->DetermineMatchingRecords( identifiers );

  // dump some information if required
  if( verbose )
  {
    sprintf( msg, "Matching results: %lu matching records found in worklist files.", numOfMatchingRecords );
    DumpMessage( msg );
  }

  // determine a correct return value. In case no matching records
  // were found, WLM_SUCCESS shall be returned. This is our assumption.
  WlmDataSourceStatusType status = WLM_SUCCESS;

  // Check if matching records were found in the database.
  // If that is the case, do the following:
  if( numOfMatchingRecords != 0 )
  {
    // create a container array that captures all result DcmDatasets
    numOfMatchingDatasets = numOfMatchingRecords;
    matchingDatasets = new DcmDataset*[ numOfMatchingDatasets ];

    // for each matching record do the following
    for( i=0 ; i<numOfMatchingRecords ; i++ )
    {
      // this variable is needed later, it must be initialized with NULL
      DcmElement *specificCharacterSetElement = NULL;

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
        element = matchingDatasets[i]->getElement(j);

        // Depending on if the current element is a sequence or not, process this element.
        if( element->ident() != EVR_SQ )
          HandleNonSequenceElementInResultDataset( element, i );
        else
          HandleSequenceElementInResultDataset( element, i );

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

    // Determine a corresponding return value: If matching records were found, WLM_PENDING or
    // WLM_PENDING_WARNING shall be returned, depending on if an unsupported optional key was
    // found in the search mask or not.
    if( foundUnsupportedOptionalKey )
      status = WLM_PENDING_WARNING;
    else
      status = WLM_PENDING;
  }

  // forget the matching records in the fileSystemInteractionManager (free memory)
  fileSystemInteractionManager->ClearMatchingRecords();

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
//                there is one more resulting dataset to return. In such a case, rstatus will be set
//                to WLM_PENDING or WLM_PENDING_WARNING, depending on if an unsupported key attribute
//                was encountered in the search mask or not. If there are no more datasets that match
//                the search mask, this function will return an empty dataset and WLM_SUCCESS in rstatus.
// Parameters   : rStatus - [out] A value of type WlmDataSourceStatusType that can be used to
//                          decide if there are still elements that have to be returned.
// Return Value : The next dataset that matches the given search mask, or an empty dataset if
//                there are no more matching datasets in the worklist database files.
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

void WlmDataSourceFileSystem::HandleNonSequenceElementInResultDataset( DcmElement *element, unsigned long index )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain non-sequence element whithin
//                the structure of a certain result dataset. This function assumes that all
//                elements in the result dataset are supported. In detail, a value for the
//                current element with regard to the currently processed matching record will
//                be requested from the fileSystemInteractionManager, and this value will be
//                set in the element.
// Parameters   : element - [in] Pointer to the currently processed element.
//                index   - [in] Index of the matching record (identifies this record).
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
    fileSystemInteractionManager->GetAttributeValueForMatchingRecord( tag, index, value );

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
      DumpMessage( "WlmDataSourceFileSystem::HandleNonSequenceElementInResultDataset: Could not set value in result element.\n" );

    // free memory
    delete value;
  }
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::HandleSequenceElementInResultDataset( DcmElement *element, unsigned long index )
// Date         : July 11, 2002
// Author       : Thomas Wilkens
// Task         : This function takes care of handling a certain sequence element whithin the
//                structure of a certain result dataset. This function assumes that all elements
//                in the result dataset are supported. It also assumes that there are no sequence
//                elements with length == 0 in the result dataset, and that each sequence element
//                contains one single non-empty item. In case there is more than one item in a
//                sequence element, the sequence element in the result data set will completely
//                be left unchanged. In detail, a value for the current element with regard to
//                the currently processed matching record will be requested from the fileSystem-
//                InteractionManager and this value will be set in the element.
// Parameters   : element - [in] Pointer to the currently processed element.
//                index   - [in] Index of the matching record (identifies this record).
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
        HandleNonSequenceElementInResultDataset( elementInItem, index );
      else
        HandleSequenceElementInResultDataset( elementInItem, index );
    }
  }
}

// ----------------------------------------------------------------------------

int WlmDataSourceFileSystem::SetReadlock()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function sets a read lock on the LOCKFILE in the directory
//                that is specified through dfPath and calledApplicationEntityTitle.
// Parameters   : none.
// Return Value : 0 - The read lock has been set successfully.
//                1 - The read lock has not been set successfully.
{
  char msg[2500];
#ifndef _WIN32
  struct flock lockdata;
#endif
  int result;

  // if no path or no calledApplicationEntityTitle is specified, return
  if( dfPath == NULL || calledApplicationEntityTitle == NULL )
  {
    DumpMessage("WlmDataSourceFileSystem::SetReadlock : Path to data source files not specified.");
    return 1;
  }

  // if a read lock has already been set, return
  if( readLockSetOnDataSource )
  {
    DumpMessage("WlmDataSourceFileSystem::SetReadlock : Nested read locks not allowed!");
    return 1;
  }

  // assign path to a local variable
  OFString lockname = dfPath;

  // if the given path does not show a PATH_SEPERATOR at the end, append one
  if( lockname.length() > 0 && lockname[lockname.length()-1] != PATH_SEPARATOR )
    lockname += PATH_SEPARATOR;

  // append calledApplicationEntityTitle, another PATH_SEPERATOR,
  // and LOCKFILENAME to the given path (and seperator)
  lockname += calledApplicationEntityTitle;
  lockname += PATH_SEPARATOR;
  lockname += LOCKFILENAME;

  // open corresponding file
  handleToReadLockFile = open( lockname.c_str(), O_RDWR );
  if( handleToReadLockFile == -1 )
  {
    handleToReadLockFile = 0;
    sprintf( msg, "WlmDataSourceFileSystem::SetReadlock : Cannot open file %s (return code: %s).", lockname.c_str(), strerror(errno) );
    DumpMessage( msg );
    return 1;
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
    sprintf( msg, "WlmDataSourceFileSystem::SetReadlock : Cannot set read lock on file %s.", lockname.c_str() );
    DumpMessage( msg );
    dcmtk_plockerr("return code");
    close( handleToReadLockFile );
    handleToReadLockFile = 0;
    return 1;
  }

  // update member variable to indicate that a read lock has been set successfully
  readLockSetOnDataSource = OFTrue;

  // return success
  return 0;
}

// ----------------------------------------------------------------------------

int WlmDataSourceFileSystem::ReleaseReadlock()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function releases a read lock on the LOCKFILE in the given directory.
// Parameters   : none.
// Return Value : 0 - The read lock has been released successfully.
//                1 - The read lock has not been released successfully.
{
#ifndef _WIN32
  struct flock lockdata;
#endif
  int result;

  // if no read lock is set, return
  if( !readLockSetOnDataSource )
  {
    DumpMessage("WlmDataSourceFileSystem::ReleaseReadlock : No readlock to release.");
    return 1;
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
    DumpMessage("WlmDataSourceFileSystem::ReleaseReadlock : Cannot release read lock");
    dcmtk_plockerr("return code");
    return 1;
  }

  // close read lock file
  close( handleToReadLockFile );
  handleToReadLockFile = 0;

  // update member variable to indicate that no read lock is set
  readLockSetOnDataSource = OFFalse;

  // return success
  return 0;
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wldsfs.cc,v $
** Revision 1.8  2002-08-05 09:10:11  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.7  2002/07/17 13:10:16  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.6  2002/06/10 11:24:53  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.5  2002/06/05 10:29:27  wilkens
** Changed call to readdir() so that readdir_r() is called instead.
**
** Revision 1.4  2002/05/08 13:20:38  wilkens
** Added new command line option -nse to wlmscpki and wlmscpdb.
**
** Revision 1.2  2002/04/18 14:19:52  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.4  2002/01/08 19:14:53  joergr
** Minor adaptations to keep the gcc compiler on Linux and Solaris happy.
** Currently only the "file version" of the worklist SCP is supported on
** Unix systems.
**
** Revision 1.3  2002/01/08 17:46:04  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 16:59:06  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:32:47  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
