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
 *  Purpose: class DcmQueryRetrieveDatabaseHandle
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:41 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMQRDBA_H
#define DCMQRDBA_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofcond.h"

class DcmDataset;
class DcmQueryRetrieveDatabaseStatus;

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

/** abstract base class for database handles. A database handle maintains
 *  a connection to a database and encapsulates database support for
 *  store, find and move/get operations.
 */
class DcmQueryRetrieveDatabaseHandle
{
public:

  /// default constructor
  DcmQueryRetrieveDatabaseHandle() { }

  /** Destructor. Destroys handle, cancels any ongoing
   *  request if necessary, deletes temporary files used for C-STORE and
   *  sub-operations of C-MOVE.
   */
   virtual ~DcmQueryRetrieveDatabaseHandle();

  /** create a filename under which a DICOM object that is currently
   *  being received through a C-STORE operation can be stored.
   *  @param SOPClassUID SOP class UID of DICOM instance
   *  @param SOPInstanceUID SOP instance UID of DICOM instance
   *  @param newImageFileName file name is returned in this parameter.
   *    Memory must be provided by the caller and should be at least MAXPATHLEN+1
   *    characters. The file name generated should be an absolute file name.
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  virtual OFCondition makeNewStoreFileName(
      const char *SOPClassUID,
      const char *SOPInstanceUID,
      char *newImageFileName) = 0;

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
  virtual OFCondition storeRequest(
      const char *SOPClassUID,
      const char *SOPInstanceUID,
      const char *imageFileName,
      DcmQueryRetrieveDatabaseStatus  *status,
      OFBool     isNew = OFTrue ) = 0;

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
  virtual OFCondition startFindRequest(
      const char *SOPClassUID,
      DcmDataset *findRequestIdentifiers,
      DcmQueryRetrieveDatabaseStatus *status) = 0;

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
  virtual OFCondition nextFindResponse(
      DcmDataset **findResponseIdentifiers,
      DcmQueryRetrieveDatabaseStatus *status) = 0;

  /** cancel the ongoing FIND request, stop and reset every running operation
   *  associated with this request, delete existing temporary files.
   *  @param status pointer to DB status object in which a DIMSE status code
   *    suitable for use with the C-FIND-RSP message is set.
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  virtual OFCondition cancelFindRequest(DcmQueryRetrieveDatabaseStatus *status) = 0;

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
  virtual OFCondition startMoveRequest(
      const char *SOPClassUID,
      DcmDataset *moveRequestIdentifiers,
      DcmQueryRetrieveDatabaseStatus *status) = 0;

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
  virtual OFCondition nextMoveResponse(
      char *SOPClassUID,
      char *SOPInstanceUID,
      char *imageFileName,
      unsigned short *numberOfRemainingSubOperations,
      DcmQueryRetrieveDatabaseStatus *status) = 0;

  /** cancel the ongoing MOVE request, stop and reset every running operation
   *  associated with this request, delete existing temporary files.
   *  @param status pointer to DB status object in which a DIMSE status code
   *    suitable for use with the C-MOVE-RSP message is set.
   *  @return EC_Normal upon normal completion, or some other OFCondition code upon failure.
   */
  virtual OFCondition cancelMoveRequest(DcmQueryRetrieveDatabaseStatus *status) = 0;

  /** Prune invalid records from the database.
   *  Records referring to non-existant image files are invalid.
   */
  virtual OFCondition pruneInvalidRecords() = 0;

  /** Configure the DB module to perform (or not perform) checking
   *  of FIND and MOVE request identifiers. Default is no checking.
   *  @param checkFind checking for C-FIND parameters
   *  @param checkMove checking for C-MOVE parameters
   */
  virtual void setIdentifierChecking(OFBool checkFind, OFBool checkMove) = 0;

};


/** abstract factory class. Instances of this class are able to create database
 *  handles for a given called application entity title.
 */
class DcmQueryRetrieveDatabaseHandleFactory
{
public:
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
    OFCondition& result) const = 0;

  /** Virtual Destructor
   */
  virtual ~DcmQueryRetrieveDatabaseHandleFactory() {}

};

#endif

/*
 * CVS Log
 * $Log: dcmqrdba.h,v $
 * Revision 1.7  2010-10-14 13:16:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.5  2009-08-21 09:50:07  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.4  2005/12/16 09:12:33  onken
 * - Added virtual (dummy) destructor to avoid compiler warnings
 *
 * Revision 1.3  2005/12/08 16:04:21  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2005/04/22 15:36:34  meichel
 * Passing calling aetitle to DcmQueryRetrieveDatabaseHandleFactory::createDBHandle
 *   to allow configuration retrieval based on calling aetitle.
 *
 * Revision 1.1  2005/03/30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
