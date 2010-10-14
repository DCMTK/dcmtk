/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:39 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmDataSourceFileSystem_h
#define WlmDataSourceFileSystem_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmwlm/wlds.h"
#include "dcmtk/dcmwlm/wlfsim.h"

//class WlmFileSystemInteractionManager;
class DcmItem;
class DcmDataset;
class OFCondition;
class DcmElement;
class DcmItem;

/** This class encapsulates data structures and operations for connecting to a file-based
 *  data source in the framework of the DICOM basic worklist management service.
 */
class WlmDataSourceFileSystem : public WlmDataSource
{
  protected:
    /// manager for file system interaction
    WlmFileSystemInteractionManager fileSystemInteractionManager;
    /// path to database files
    OFString dfPath;
    /// indicates if wl-files which are lacking return type 1 attributes or information in such attributes shall be rejected or not
    OFBool enableRejectionOfIncompleteWlFiles;
    /// handle to the read lock file
    int handleToReadLockFile;

      /** This function sets a read lock on the LOCKFILE in the directory
       *  that is specified through dfPath and calledApplicationEntityTitle.
       *  @return true in case the read lock has been set successfully, false otherwise.
       */
    OFBool SetReadlock();

      /** This function releases a read lock on the LOCKFILE in the given directory.
       *  @return true in case the read lock has been released successfully, false otherwise.
       */
    OFBool ReleaseReadlock();

      /** This function takes care of handling a certain non-sequence element whithin
       *  the structure of a certain result dataset. This function assumes that all
       *  elements in the result dataset are supported. In detail, a value for the
       *  current element with regard to the currently processed matching record will
       *  be requested from the fileSystemInteractionManager, and this value will be
       *  set in the element.
       *  @param element Pointer to the currently processed element.
       *  @param idx     Index of the matching record (identifies this record).
       */
    void HandleNonSequenceElementInResultDataset( DcmElement *element, unsigned long idx );

      /** This function takes care of handling a certain sequence element within the structure
       *  of a certain result dataset. On the basis of the matching record from the data source,
       *  this function will add items and values for all elements in these items to the current
       *  sequence element in the result dataset. This function assumes that all elements in the
       *  result dataset are supported. In case the current sequence element contains no items or
       *  more than one item, this element will be left unchanged.
       *  @param element Pointer to the currently processed element.
       *  @param idx     Index of the matching record (identifies this record).
       */
    void HandleSequenceElementInResultDataset( DcmElement *element, unsigned long idx );

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmDataSourceFileSystem( const WlmDataSourceFileSystem &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmDataSourceFileSystem &operator=( const WlmDataSourceFileSystem &Src );


  public:
      /** default constructor.
       */
    WlmDataSourceFileSystem();

      /** destructor
       */
    ~WlmDataSourceFileSystem();

      /** Connects to the data source.
       * @return Indicates if the connection was established succesfully.
       */
    OFCondition ConnectToDataSource();

      /** Disconnects from the data source.
       * @return Indicates if the disconnection was completed succesfully.
       */
    OFCondition DisconnectFromDataSource();

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetDfPath( const OFString& value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetEnableRejectionOfIncompleteWlFiles( OFBool value );

      /** Checks if the called application entity title is supported. This function expects
       *  that the called application entity title was made available for this instance through
       *  WlmDataSource::SetCalledApplicationEntityTitle(). If this is not the case, OFFalse
       *  will be returned.
       *  @return OFTrue, if the called application entity title is supported;
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    OFBool IsCalledApplicationEntityTitleSupported();

      /** This function performs a check on two attributes in the given dataset. At two different places
       *  in the definition of the DICOM worklist management service, a description attribute and a code
       *  sequence attribute with a return type of 1C are mentioned, and the condition specifies that
       *  either the description attribute or the code sequence attribute or both shall be supported by
       *  an SCP. (I am talking about RequestedProcedureDescription vs. RequestedProcedureCodeSequence
       *  and ScheduledProcedureStepDescription vs. ScheduledProtocolCodeSequence.) In both cases, this
       *  implementation actually supports both, the description _and_ the code sequence attributes.
       *  In cases where the description attribute is actually empty or the code sequence attribute
       *  is actually empty or contains exactly one item with an empty CodeValue and an empty
       *  CodingSchemeDesignator, we want to remove the empty attribute from the dataset. This is what
       *  this function does. (Please note, that this function will always only delete one of the two,
       *  and this function will start checking the sequence attribute.
       *  @param dataset Dataset in which the consistency of the two attributes shall be checked.
       *  @param descriptionTagKey DcmTagKey of the description attribute which shall be checked.
       *  @param codeSequenceTagKey DcmTagKey of the codeSequence attribute which shall be checked.
       */
    void HandleExistentButEmptyDescriptionAndCodeSequenceAttributes( DcmItem *dataset, const DcmTagKey &descriptionTagKey, const DcmTagKey &codeSequenceTagKey );

      /** This function performs a check on a sequence attribute in the given dataset. At two different places
       *  in the definition of the DICOM worklist management service, a sequence attribute with a return type
       *  of 2 is mentioned containing two 1C attributes in its item; the condition of the two 1C attributes
       *  specifies that in case a sequence item is present, then these two attributes must be existent and
       *  must contain a value. (I am talking about ReferencedStudySequence and ReferencedPatientSequence.)
       *  In cases where the sequence attribute contains exactly one item with an empty ReferencedSOPClass
       *  and an empty ReferencedSOPInstance, we want to remove the item from the sequence. This is what
       *  this function does.
       *  @param dataset Dataset in which the consistency of the sequence attribute shall be checked.
       *  @param sequenceTagKey DcmTagKey of the sequence attribute which shall be checked.
       */
    void HandleExistentButEmptyReferencedStudyOrPatientSequenceAttributes( DcmDataset *dataset, const DcmTagKey &sequenceTagKey );

      /** Based on the search mask which was passed, this function determines all the records in the
       *  worklist database files which match the values of matching key attributes in the search mask.
       *  For each matching record, a DcmDataset structure is generated which will later be
       *  returned to the SCU as a result of query. The DcmDataset structures for all matching
       *  records will be stored in the protected member variable matchingDatasets.
       *  @param findRequestIdentifiers Contains the search mask.
       *  @return A WlmDataSourceStatusType value denoting the following:
       *          WLM_SUCCESS: No matching records found;
       *          WLM_PENDING: Matching records found, all return keys supported by this application;
       *          WLM_PENDING_WARNING: Matching records found, not all return keys supported by this application;
       *          WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS: Error in the search mask encountered.
       */
    WlmDataSourceStatusType StartFindRequest( const DcmDataset &findRequestIdentifiers );

      /** This function will return the next dataset that matches the given search mask, if
       *  there is one more resulting dataset to return. In such a case, rstatus will be set
       *  to WLM_PENDING or WLM_PENDING_WARNING, depending on if an unsupported key attribute
       *  was encountered in the search mask or not. If there are no more datasets that match
       *  the search mask, this function will return an empty dataset and WLM_SUCCESS in rstatus.
       *  @param rStatus A value of type WlmDataSourceStatusType that can be used to
       *                 decide if there are still elements that have to be returned.
       *  @return The next dataset that matches the given search mask, or an empty dataset if
       *          there are no more matching datasets in the worklist database files.
       */
    DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus );
};

#endif

/*
** CVS Log
** $Log: wldsfs.h,v $
** Revision 1.17  2010-10-14 13:16:39  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.16  2006/12/15 14:49:21  onken
** Removed excessive use char* and C-array in favour of OFString and
** OFList. Simplified some implementation details.
**
** Revision 1.15  2005/12/08 16:05:41  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.14  2005/05/04 11:33:17  wilkens
** Modified handling of the attributes ScheduledProcedureStepDescription/
** ScheduledProtocolCodeSequence and RequestedProcedureDescription/
** RequestedProcedureCodeSequence in wlmscpfs: in case one of the two attributes
** does not contain any information in a C-Find RSP message which is about to be
** sent to an SCU, the empty attribute will be removed from the C-Find RSP message
** before the message is sent, in order not to send an invalid RSP message.
** Added two command line options --enable-file-reject (default) and
** --disable-file-reject to wlmscpfs: these options can be used to enable or
** disable a file rejection mechanism which makes sure only complete worklist files
** will be used during the matching process. A worklist file is considered to be
** complete if it contains all necessary type 1 information which the SCP might
** have to return to an SCU in a C-Find response message.
**
** Revision 1.13  2004/05/26 10:36:53  meichel
** Fixed minor bug in worklist server regarding failed read locks.
**
** Revision 1.12  2004/01/07 08:32:28  wilkens
** Added new sequence type return key attributes to wlmscpfs. Fixed bug that for
** equally named attributes in sequences always the same value will be returned.
** Added functionality that also more than one item will be returned in sequence
** type return key attributes.
**
** Revision 1.11  2003/08/21 13:38:55  wilkens
** Moved declaration and initialization of member variables matchingDatasets and
** NumOfMatchingDatasets to base class.
**
** Revision 1.10  2003/07/02 09:17:55  wilkens
** Updated documentation to get rid of doxygen warnings.
**
** Revision 1.9  2002/12/16 11:08:34  wilkens
** Added missing #include "osconfig.h" to certain files.
**
** Revision 1.8  2002/12/09 13:40:49  joergr
** Renamed parameter to avoid name clash with global function index().
**
** Revision 1.7  2002/08/12 10:56:08  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.6  2002/08/05 09:09:57  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.4  2002/07/17 13:10:17  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.3  2002/06/10 11:24:54  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.2  2002/04/18 14:19:53  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.3  2002/01/08 17:45:34  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 16:47:53  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:30:59  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
