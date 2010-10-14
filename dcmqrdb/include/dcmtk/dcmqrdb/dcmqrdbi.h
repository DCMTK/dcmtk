/*
 *
 *  Copyright (C) 1993-2010, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Andrew Hewett, Marco Eichelberg
 *
 *  Purpose: class DcmQueryRetrieveIndexDatabaseHandle
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:41 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMQRDBI_H
#define DCMQRDBI_H

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrdba.h"    /* for class DcmQueryRetrieveDatabaseHandle */

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/ofstd/offname.h"

struct StudyDescRecord;
struct DB_Private_Handle;
struct DB_SmallDcmElmt;
struct IdxRecord;
struct DB_ElementList;
class DcmQueryRetrieveConfig;

#define DBINDEXFILE "index.dat"

#ifndef _WIN32
/* we lock image files on all platforms except Win32 where it does not work
 * due to the different semantics of LockFile/LockFileEx compared to flock.
 */
#define LOCK_IMAGE_FILES
#endif

/** enumeration describing the levels of the DICOM Q/R information model
 */
enum DB_LEVEL
{
  /// DICOM Q/R patient level
  PATIENT_LEVEL,
  /// DICOM Q/R study level
  STUDY_LEVEL,
  /// DICOM Q/R series level
  SERIE_LEVEL,
  /// DICOM Q/R instance level
  IMAGE_LEVEL
};

/** This enum describes the status of one entry in the database hierarchy. An 
 *  entry can describe a study, a series or an instance. A study or series is 
 *  new exactly if all subobjects (series and instances) are new. A study or 
 *  series contains new subobjecs as long as any subobject (series or instance) 
 *  has the status objectIsNew. Instances can never have the status 
 *  DVIF_objectContainsNewSubobjects. 
 */
enum DVIFhierarchyStatus
{
  /// object (study, series or instance) in the database is not new
  DVIF_objectIsNotNew,
  /// object (study, series or instance) in the database is new
  DVIF_objectIsNew,
  /// object (study or series) in the database is not new but contains new subobjects
  DVIF_objectContainsNewSubobjects
};

/// upper limit for the number of studies per storage area
#define DB_UpperMaxStudies              500

/// upper limit for the number bytes per study
#define DB_UpperMaxBytesPerStudy        0x40000000L


/** This class maintains database handles based on the classical "index.dat" file.
 *  A database handle maintains a connection to a database and encapsulates database support for
 *  store, find and move/get operations.
 */
class DcmQueryRetrieveIndexDatabaseHandle: public DcmQueryRetrieveDatabaseHandle
{
public:

  /** Constructor. Creates and initializes a index file handle for the given 
   *  database storage area (storageArea).
   *  @param storageArea name of storage area, must not be NULL
   *  @param maxStudiesPerStorageArea maximum number of studies for this storage area,
   *    needed to correctly parse the index file.
   *  @param maxBytesPerStudy maximum number of bytes per study, for quota mechanism
   *  @param result upon successful initialization of the database handle,
   *    EC_Normal is returned in this parameter, otherwise an error code is returned.
   */
  DcmQueryRetrieveIndexDatabaseHandle(
    const char *storageArea,
    long maxStudiesPerStorageArea,
    long maxBytesPerStudy,
    OFCondition& result);
  
  /** Destructor. Destroys handle, cancels any ongoing
   *  request if necessary, deletes temporary files used for C-STORE and
   *  sub-operations of C-MOVE.
   */
   ~DcmQueryRetrieveIndexDatabaseHandle();
  
  /** Configure the DB module to perform (or not perform) checking
   *  of FIND and MOVE request identifiers. Default is no checking.
   *  @param checkFind checking for C-FIND parameters
   *  @param checkMove checking for C-MOVE parameters
   */
  void setIdentifierChecking(OFBool checkFind, OFBool checkMove);
  
  /** create a filename under which a DICOM object that is currently
   *  being received through a C-STORE operation can be stored.
   *  @param SOPClassUID SOP class UID of DICOM instance
   *  @param SOPInstanceUID SOP instance UID of DICOM instance
   *  @param newImageFileName file name is returned in this parameter.
   *    Memory must be provided by the caller and should be at least MAXPATHLEN+1 
   *    characters. The file name generated should be an absolute file name.
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition makeNewStoreFileName(
      const char *SOPClassUID,
      const char *SOPInstanceUID,
      char *newImageFileName);
  
  /** register the given DICOM object, which has been received through a C-STORE 
   *  operation and stored in a file, in the database.
   *  @param SOPClassUID SOP class UID of DICOM instance
   *  @param SOPInstanceUID SOP instance UID of DICOM instance
   *  @param imageFileName file name (full path) of DICOM instance
   *  @param status pointer to DB status object in which a DIMSE status code 
        suitable for use with the C-STORE-RSP message is set.
   *  @param isNew if true, the instance is marked as "new" in the database,
   *    if such a flag is maintained in the database.   
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition storeRequest(
      const char *SOPClassUID,
      const char *SOPInstanceUID,
      const char *imageFileName,
      DcmQueryRetrieveDatabaseStatus  *status,
      OFBool     isNew = OFTrue );
  
  /** initiate FIND operation using the given SOP class UID (which identifies
   *  the query model) and DICOM dataset containing find request identifiers. 
   *  @param SOPClassUID SOP class UID of query service, identifies Q/R model
   *  @param findRequestIdentifiers dataset containing request identifiers (i.e., the query)
   *    The caller retains responsibility for destroying the 
   *    findRequestIdentifiers when no longer needed.
   *  @param status pointer to DB status object in which a DIMSE status code 
   *    suitable for use with the C-FIND-RSP message is set. Status will be
   *    PENDING if any FIND responses will be generated or SUCCESS if no FIND responses will
   *    be generated (SUCCESS indicates the completion of a operation), or
   *    another status code upon failure. 
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition startFindRequest(
      const char *SOPClassUID,
      DcmDataset *findRequestIdentifiers,
      DcmQueryRetrieveDatabaseStatus *status);     
                
  /** return the next available FIND response as a new DICOM dataset.
   *  @param findResponseIdentifiers DICOM dataset returned in this parameter.
   *    The caller is responsible for destroying the findResponseIdentifiers
   *    when no longer needed.
   *  @param status pointer to DB status object in which a DIMSE status code 
   *    suitable for use with the C-FIND-RSP message is set. Status will be
   *    PENDING if more FIND responses will be generated or SUCCESS if no more 
   *    FIND responses will be generated (SUCCESS indicates the completion of 
   *    a operation), or another status code upon failure. 
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition nextFindResponse(
      DcmDataset **findResponseIdentifiers,
      DcmQueryRetrieveDatabaseStatus *status);
   
  /** cancel the ongoing FIND request, stop and reset every running operation
   *  associated with this request, delete existing temporary files.
   *  @param status pointer to DB status object in which a DIMSE status code 
   *    suitable for use with the C-FIND-RSP message is set. 
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition cancelFindRequest(DcmQueryRetrieveDatabaseStatus *status);
          
  /** initiate MOVE operation using the given SOP class UID (which identifies
   *  the retrieve model) and DICOM dataset containing move request identifiers. 
   *  @param SOPClassUID SOP class UID of retrieve service, identifies Q/R model
   *  @param moveRequestIdentifiers dataset containing request identifiers (i.e., the query)
   *    The caller retains responsibility for destroying the 
   *    moveRequestIdentifiers when no longer needed.
   *  @param status pointer to DB status object in which a DIMSE status code 
   *    suitable for use with the C-MOVE-RSP message is set. Status will be
   *    PENDING if any MOVE responses will be generated or SUCCESS if no MOVE responses will
   *    be generated (SUCCESS indicates the completion of a operation), or
   *    another status code upon failure. 
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition startMoveRequest(
      const char *SOPClassUID,
      DcmDataset *moveRequestIdentifiers,
      DcmQueryRetrieveDatabaseStatus *status);
  
  /** Constructs the information required for the next available C-MOVE 
   *  sub-operation (the image SOP class UID, SOP Instance UID and an
   *  imageFileName containing the requested data). 
   *  @param SOPClassUID pointer to string of at least 65 characters into 
   *    which the SOP class UID for the next DICOM object to be transferred is copied.
   *  @param SOPInstanceUID pointer to string of at least 65 characters into 
   *    which the SOP instance UID for the next DICOM object to be transferred is copied.
   *  @param imageFileName pointer to string of at least MAXPATHLEN+1 characters into 
   *    which the file path for the next DICOM object to be transferred is copied.
   *  @param numberOfRemainingSubOperations On return, this parameter will contain
   *     the number of suboperations still remaining for the request
   *     (this number is needed by move responses with PENDING status).
   *  @param status pointer to DB status object in which a DIMSE status code 
   *    suitable for use with the C-MOVE-RSP message is set. Status will be
   *    PENDING if more MOVE responses will be generated or SUCCESS if no more 
   *    MOVE responses will be generated (SUCCESS indicates the completion of 
   *    a operation), or another status code upon failure. 
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */  
  OFCondition nextMoveResponse(
      char *SOPClassUID,
      char *SOPInstanceUID,
      char *imageFileName,
      unsigned short *numberOfRemainingSubOperations,
      DcmQueryRetrieveDatabaseStatus *status);
  
  /** cancel the ongoing MOVE request, stop and reset every running operation
   *  associated with this request, delete existing temporary files.
   *  @param status pointer to DB status object in which a DIMSE status code 
   *    suitable for use with the C-MOVE-RSP message is set. 
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition cancelMoveRequest(DcmQueryRetrieveDatabaseStatus *status);
  
  /** Prune invalid records from the database.
   *  Records referring to non-existant image files are invalid.
   */
  OFCondition pruneInvalidRecords();

  // methods not inherited from the base class

  /** enable/disable the DB quota system (default: enabled) which causes images
   *  to be deleted if certain boundaries (number of studies, bytes per study) are exceeded.
   */
  void enableQuotaSystem(OFBool enable);

  /** dump database index file to stdout.
   *  @param storeArea name of storage area, must not be NULL
   */
  static void printIndexFile (char *storeArea);
    
  /** deletes the given file only if the quota mechanism is enabled.
   *  The image is not de-registered from the database by this routine.
   *  @param imgFile file name (path) to the file to be deleted.
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  OFCondition deleteImageFile(char* imgFile);

  /** create lock on database
   *  @param exclusive exclusive/shared lock flag
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition DB_lock(OFBool exclusive);

  /** release lock on database
   */
  OFCondition DB_unlock();

  /** Get next Index record that is in use (i.e. references a non-empty a filename)
   *  @param idx pointer to index number, updated upon successful return
   *  @param idxRec pointer to index record structure
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition DB_IdxGetNext(int *idx, IdxRecord *idxRec);

  /** seek to beginning of image records in index file
   *  @param idx initialized to -1
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition DB_IdxInitLoop(int *idx);

  /** read index record at given index
   *  @param idx index
   *  @param idxRec pointer to index record
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition DB_IdxRead(int idx, IdxRecord *idxRec);

  /** get study descriptor record from start of index file
   *  @param pStudyDesc pointer to study record descriptor structure
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition DB_GetStudyDesc(StudyDescRecord *pStudyDesc);

  /** write study descriptor record to start of index file
   *  @param pStudyDesc pointer to study record descriptor structure
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition DB_StudyDescChange(StudyDescRecord *pStudyDesc);

  /** deactivate index record at given index by setting an empty filename
   *  @param idx index
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition DB_IdxRemove(int idx);

  /** clear the "is new" flag for the instance with the given index
   *  @param idx index
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition instanceReviewed(int idx);

  /// return name of storage area
  const char *getStorageArea() const;

  /// return path to index file
  const char *getIndexFilename() const;

      
private:

  OFCondition removeDuplicateImage(
      const char *SOPInstanceUID, const char *StudyInstanceUID,
      StudyDescRecord *pStudyDesc, const char *newImageFileName);
  int deleteOldestStudy(StudyDescRecord *pStudyDesc);
  OFCondition deleteOldestImages(StudyDescRecord *pStudyDesc, int StudyNum, char *StudyUID, long RequiredSize);
  int matchDate (DB_SmallDcmElmt *mod, DB_SmallDcmElmt *elt);
  int matchTime (DB_SmallDcmElmt *mod, DB_SmallDcmElmt *elt);
  int matchUID (DB_SmallDcmElmt *mod, DB_SmallDcmElmt *elt);
  int matchStrings (DB_SmallDcmElmt *mod, DB_SmallDcmElmt *elt);
  int matchOther (DB_SmallDcmElmt *mod, DB_SmallDcmElmt *elt);
  int dbmatch (DB_SmallDcmElmt *mod, DB_SmallDcmElmt *elt);
  void makeResponseList(DB_Private_Handle *phandle, IdxRecord *idxRec);
  int matchStudyUIDInStudyDesc (StudyDescRecord *pStudyDesc, char *StudyUID, int maxStudiesAllowed);
  OFCondition checkupinStudyDesc(StudyDescRecord *pStudyDesc, char *StudyUID, long imageSize);

  OFCondition hierarchicalCompare (
      DB_Private_Handle *phandle,
      IdxRecord         *idxRec,
      DB_LEVEL          level,
      DB_LEVEL          infLevel,
      int               *match);

  OFCondition testFindRequestList (
      DB_ElementList  *findRequestList,
      DB_LEVEL        queryLevel,
      DB_LEVEL        infLevel,
      DB_LEVEL        lowestLevel);

  OFCondition testMoveRequestList (
      DB_ElementList  *findRequestList,
      DB_LEVEL        queryLevel,
      DB_LEVEL        infLevel,
      DB_LEVEL        lowestLevel);

  /// database handle
  DB_Private_Handle *handle_;

  /// flag indicating whether or not the quota system is enabled
  OFBool quotaSystemEnabled;

  /// flag indicating whether or not the check function for FIND requests is enabled
  OFBool doCheckFindIdentifier;

  /// flag indicating whether or not the check function for MOVE requests is enabled
  OFBool doCheckMoveIdentifier;

  /// helper object for file name creation
  OFFilenameCreator fnamecreator;

};


/** Index database factory class. Instances of this class are able to create database
 *  handles for a given called application entity title.
 */
class DcmQueryRetrieveIndexDatabaseHandleFactory: public DcmQueryRetrieveDatabaseHandleFactory
{
public:

  /** constructor
   *  @param config system configuration object, must not be NULL.
   */
  DcmQueryRetrieveIndexDatabaseHandleFactory(const DcmQueryRetrieveConfig *config);

  /// destructor
  virtual ~DcmQueryRetrieveIndexDatabaseHandleFactory();

  /** this method creates a new database handle instance on the heap and returns
   *  a pointer to it, along with a result that indicates if the instance was
   *  successfully initialized, i.e. connected to the database
   *  @param callingAETitle calling aetitle
   *  @param calledAETitle called aetitle
   *  @param result result returned in this variable
   *  @return pointer to database object, must not be NULL if result is EC_Normal.
   */
  virtual DcmQueryRetrieveDatabaseHandle *createDBHandle(
    const char *callingAETitle, 
    const char *calledAETitle,
    OFCondition& result) const;

private:

  /// pointer to system configuration
  const DcmQueryRetrieveConfig *config_;
};

#endif

/*
 * CVS Log
 * $Log: dcmqrdbi.h,v $
 * Revision 1.8  2010-10-14 13:16:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.6  2009-08-21 09:50:07  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.5  2008-04-15 15:43:37  meichel
 * Fixed endless recursion bug in the index file handling code when
 *   the index file does not exist
 *
 * Revision 1.4  2005/12/08 16:04:22  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2005/04/22 15:36:34  meichel
 * Passing calling aetitle to DcmQueryRetrieveDatabaseHandleFactory::createDBHandle
 *   to allow configuration retrieval based on calling aetitle.
 *
 * Revision 1.2  2005/04/04 10:04:45  meichel
 * Added public declarations for index file functions that are
 *   used from module dcmpstat
 *
 * Revision 1.1  2005/03/30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
