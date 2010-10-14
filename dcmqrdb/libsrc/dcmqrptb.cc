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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: classes DcmQueryRetrieveProcessSlot, DcmQueryRetrieveProcessTable
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:36 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrptb.h"
#include "dcmtk/dcmqrdb/dcmqropt.h"

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

  /** check if is instance describes a client with write access
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

    ASC_getPresentationAddresses(assoc->params, peerName, NULL);
    ASC_getAPTitles(assoc->params, callingAETitle, calledAETitle, NULL);

    for (int i=0; i<numberOfAllDcmStorageSOPClassUIDs; i++)
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
#if defined(__NeXT__)
    union wait status; /* some systems need a union wait as argument to wait3 */
#else
    int status;
#endif
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
          time_t t = time(NULL);
          DCMQRDB_INFO("Cleaned up after child (" << child << ") " << ctime(&t));

          /* Remove Entry from Process Table */
          removeProcessFromTable(child);
      }
    }
#else
    /* cannot wait for child processes */
#endif
}


/*
 * CVS Log
 * $Log: dcmqrptb.cc,v $
 * Revision 1.9  2010-10-14 13:14:36  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2010-06-03 10:34:57  joergr
 * Replaced calls to strerror() by new helper function OFStandard::strerror()
 * which results in using the thread safe version of strerror() if available.
 *
 * Revision 1.7  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.6  2009-08-21 09:54:11  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.5  2006/08/15 16:09:34  meichel
 * Updated the code in module dcmqrdb to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.4  2005/12/08 15:47:12  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2005/11/29 10:54:52  meichel
 * Added minimal support for compressed transfer syntaxes to dcmqrscp.
 *   No on-the-fly decompression is performed, but compressed images can
 *   be stored and retrieved.
 *
 * Revision 1.2  2005/10/25 08:56:18  meichel
 * Updated list of UIDs and added support for new transfer syntaxes
 *   and storage SOP classes.
 *
 * Revision 1.1  2005/03/30 13:34:53  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
