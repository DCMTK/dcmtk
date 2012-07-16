/*
 *
 *  Copyright (C) 1993-2011, OFFIS e.V.
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
 *  Purpose: class DcmQueryRetrieveProcessTable
 *
 */

#ifndef DCMQRPTB_H
#define DCMQRPTB_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmqrdb/qrdefine.h"

#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
END_EXTERN_C


class DcmQueryRetrieveProcessSlot;


/** this class maintains a table of client processes. For each client process,
 *  certain key parameters such as the peer hostname, AE titles, read/write access
 *  are stored along with the process ID.
 */
class DCMTK_DCMQRDB_EXPORT DcmQueryRetrieveProcessTable
{
public:
  /// default constructor
  DcmQueryRetrieveProcessTable() : table_() { }

  /// destructor
  virtual ~DcmQueryRetrieveProcessTable();

  /** adds a new child process to the process table.
   *  @param pid process ID of the child process
   *  @param assoc peer hostname and AEtitles are read from this object
   */
  void addProcessToTable(int pid, T_ASC_Association * assoc);

  /** returns the number of child processes in the table
   *  @return number of child processes
   */
  size_t countChildProcesses() const
  {
    return table_.size();
  }

  /** check if child processes have terminated and, if yes, remove
   *  them from the process table.  This method should be called
   *  regularly.
   */
  void cleanChildren();

  /** check if we have a child process that has write access to the
   *  given aetitle. Used to enforce an ad-hoc rule that allows only
   *  one parallel association to negotiate storage presentation contexts
   *  for each storage area (aetitle)
   */
  OFBool haveProcessWithWriteAccess(const char *calledAETitle) const;

private:

  /** remove the process with the given process ID from the table
   *  @param pid process ID
   */
  void removeProcessFromTable(int pid);

  /// the list of process entries maintained by this object.
  OFList<DcmQueryRetrieveProcessSlot *> table_;
};


#endif
