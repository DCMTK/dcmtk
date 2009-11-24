/*
 *
 *  Copyright (C) 1993-2009, OFFIS
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
 *  Purpose: class DcmQueryRetrieveProcessTable
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-24 10:10:42 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMQRPTB_H
#define DCMQRPTB_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmnet/assoc.h"

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
class DcmQueryRetrieveProcessTable
{
public:
  /// default constructor
  DcmQueryRetrieveProcessTable() { }

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

/*
 * CVS Log
 * $Log: dcmqrptb.h,v $
 * Revision 1.4  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.3  2009-08-21 09:50:07  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.2  2005/12/08 16:04:26  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2005/03/30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
