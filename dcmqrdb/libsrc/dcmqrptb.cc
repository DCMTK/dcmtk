/*
 *
 *  Copyright (C) 1993-2005, OFFIS
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: classes DcmQueryRetrieveProcessSlot, DcmQueryRetrieveProcessTable
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-10-25 08:56:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmqrdb/libsrc/dcmqrptb.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmqrptb.h"

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
      delete *table_.erase(first);
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


void DcmQueryRetrieveProcessTable::cleanChildren(OFBool verbose)
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
            ofConsole.lockCerr() << "error: wait for child process failed: " << strerror(errno) << endl;
            ofConsole.unlockCerr();
          }
      } 
      else if (child > 0)
      {
          if (verbose)
          {
            time_t t = time(NULL);
            ofConsole.lockCerr() << "Cleaned up after child (" << child << ") " << ctime(&t) << endl;
            ofConsole.unlockCerr();
          }

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
 * Revision 1.2  2005-10-25 08:56:18  meichel
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
