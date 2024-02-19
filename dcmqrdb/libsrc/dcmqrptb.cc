/*
 *
 *  Copyright (C) 1993-2024, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: classes DcmQueryRetrieveProcessSlot, DcmQueryRetrieveProcessTable
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrptb.h"
#include "dcmtk/dcmqrdb/dcmqropt.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include <ctime>


/** helper class that describes entries in the process slot table. Internal use only.
 */
class DcmQueryRetrieveProcessSlot
{
public:
  /** constructor.
   *  @param peerName hostname or IP address of peer system
   *  @param callingAETitle calling aetitle of peer
   *  @param calledAETitle called aetitle we're responding to
   *  @param processId process ID of child process
   *  @param startTime time the child process was started
   *  @param hasStorageAbility true if the child process is allowed to store
   */
  DcmQueryRetrieveProcessSlot(
    const char *peerName,
    const char *callingAETitle,
    const char *calledAETitle,
    int processId,
    time_t startTime,
    OFBool hasStorageAbility);

  /// destructor
  virtual ~DcmQueryRetrieveProcessSlot() {}

  /** check if this instance matches the given process ID
   */
  OFBool matchesPID(int pid) const
  {
    return pid == processId_;
  }

  /** check if this instance describes a client with write access
   */
  OFBool isProcessWithWriteAccess(
    const char *calledAETitle) const;

private:

    /// hostname or IP address of peer system
    OFString peerName_;

    /// calling aetitle of peer
    OFString callingAETitle_;

    /// called aetitle we're responding to
    OFString calledAETitle_;

    /// process ID of child process
    int      processId_;

    /// time the child process was started
    time_t   startTime_;

    /// true if the child process is allowed to store
    OFBool   hasStorageAbility_;
};


DcmQueryRetrieveProcessSlot::DcmQueryRetrieveProcessSlot(
      const char *peerName,
      const char *callingAETitle,
      const char *calledAETitle,
      int processId,
      time_t startTime,
      OFBool hasStorageAbility)
: peerName_()
, callingAETitle_()
, calledAETitle_()
, processId_(processId)
, startTime_(startTime)
, hasStorageAbility_(hasStorageAbility)
{
  if (peerName) peerName_ = peerName;
  if (callingAETitle) callingAETitle_ = callingAETitle;
  if (calledAETitle) calledAETitle_ = calledAETitle;
}


OFBool DcmQueryRetrieveProcessSlot::isProcessWithWriteAccess(const char *calledAETitle) const
{
  (void) startTime_;
  return (hasStorageAbility_ && calledAETitle && calledAETitle_ == calledAETitle);
}


/* ================================================================== */


DcmQueryRetrieveProcessTable::~DcmQueryRetrieveProcessTable()
{
  OFListIterator(DcmQueryRetrieveProcessSlot *) first = table_.begin();
  OFListIterator(DcmQueryRetrieveProcessSlot *) last = table_.end();
  while (first != last)
  {
    delete *first;
    first = table_.erase(first);
  }
}

void DcmQueryRetrieveProcessTable::addProcessToTable(int pid, T_ASC_Association * assoc)
{
    DIC_NODENAME peerName;
    DIC_AE       callingAETitle;
    DIC_AE       calledAETitle;
    OFBool hasStorageAbility = OFFalse;

    ASC_getPresentationAddresses(assoc->params, peerName, sizeof(peerName), NULL, 0);
    ASC_getAPTitles(assoc->params, callingAETitle, sizeof(callingAETitle), calledAETitle, sizeof(calledAETitle), NULL, 0);

    for (int i=0; i<numberOfDcmAllStorageSOPClassUIDs; i++)
    {
      if (ASC_findAcceptedPresentationContextID(assoc, dcmAllStorageSOPClassUIDs[i]))
      {
        hasStorageAbility = OFTrue;
        break;  /* out of for loop */
      }
    }

    DcmQueryRetrieveProcessSlot *slot = new DcmQueryRetrieveProcessSlot(
      peerName, callingAETitle, calledAETitle, pid, time(NULL), hasStorageAbility);

    /* add to start of list */
    table_.push_front(slot);
}

void DcmQueryRetrieveProcessTable::removeProcessFromTable(int pid)
{
  OFListIterator(DcmQueryRetrieveProcessSlot *) first = table_.begin();
  OFListIterator(DcmQueryRetrieveProcessSlot *) last = table_.end();
  while (first != last)
  {
    if ((*first)->matchesPID(pid))
    {
      delete (*first);
      table_.erase(first);
      return;
    }
    ++first;
  }
}

OFBool DcmQueryRetrieveProcessTable::haveProcessWithWriteAccess(const char *calledAETitle) const
{
  OFListConstIterator(DcmQueryRetrieveProcessSlot *) first = table_.begin();
  OFListConstIterator(DcmQueryRetrieveProcessSlot *) last = table_.end();
  while (first != last)
  {
    if ((*first)->isProcessWithWriteAccess(calledAETitle)) return OFTrue;
    ++first;
  }
  return OFFalse;
}


void DcmQueryRetrieveProcessTable::cleanChildren()
{
#if defined(HAVE_WAITPID) || defined(HAVE_WAIT3)

    /* declare local variables for waitpid/wait3 */
#ifdef HAVE_WAITPID
    int stat_loc;
#else
    int status;
    struct rusage rusage;
#endif

    int child = 1;
    while (child > 0)
    {
#ifdef HAVE_WAITPID
      child = OFstatic_cast(int, waitpid(-1, &stat_loc, WNOHANG));
#else
      child = wait3(&status, WNOHANG, &rusage);
#endif
      if (child < 0)
      {
        if (errno == ECHILD)
        {
          /* no children so don't complain */
        }
        else if (errno != 0)
        {
          char buf[256];
          DCMQRDB_ERROR("wait for child process failed: " << OFStandard::strerror(errno, buf, sizeof(buf)));
        }
      }
      else if (child > 0)
      {
        DCMQRDB_INFO("Cleaned up after child (" << child << ")");

        /* Remove Entry from Process Table */
        removeProcessFromTable(child);
      }
    }
#else
    /* cannot wait for child processes */
#endif
}
