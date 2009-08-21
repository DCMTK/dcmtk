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
 *  Purpose: class DcmQueryRetrieveDatabaseStatus
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-08-21 09:54:11 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrdbs.h"
#include "dcmtk/dcmdata/dcdatset.h"    /* for class DcmDataset */

#define INCLUDE_CSTDDEF
#include "dcmtk/ofstd/ofstdinc.h"


DcmQueryRetrieveDatabaseStatus::DcmQueryRetrieveDatabaseStatus(Uint16 status)
: status_(status)
, statusDetail_(NULL)
{
}

DcmQueryRetrieveDatabaseStatus::DcmQueryRetrieveDatabaseStatus(const DcmQueryRetrieveDatabaseStatus& org)
: status_(org.status_)
, statusDetail_(NULL)
{
  if (org.statusDetail_) statusDetail_ = OFstatic_cast(DcmDataset *, org.statusDetail_->clone());
}

DcmQueryRetrieveDatabaseStatus::~DcmQueryRetrieveDatabaseStatus()
{
  delete statusDetail_;
}

DcmQueryRetrieveDatabaseStatus& DcmQueryRetrieveDatabaseStatus::operator=(const DcmQueryRetrieveDatabaseStatus& org)
{
  if (this != &org)
  {
    status_ = org.status_;
    if (org.statusDetail_ != statusDetail_)
    {
      delete statusDetail_;
      if (org.statusDetail_) statusDetail_ = OFstatic_cast(DcmDataset *, org.statusDetail_->clone()); else statusDetail_ = NULL;
    }
  }
  return *this;
}

void DcmQueryRetrieveDatabaseStatus::deleteStatusDetail()
{
  delete statusDetail_;
  statusDetail_ = NULL;
}

DcmDataset *DcmQueryRetrieveDatabaseStatus::extractStatusDetail()
{
  DcmDataset *result = statusDetail_;
  statusDetail_ = NULL;
  return result;
}

/*
 * CVS Log
 * $Log: dcmqrdbs.cc,v $
 * Revision 1.3  2009-08-21 09:54:11  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.2  2005/12/08 15:47:10  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2005/03/30 13:34:53  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
