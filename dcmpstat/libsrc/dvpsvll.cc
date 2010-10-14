/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSVOILUT_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsvll.h"
#include "dcmtk/dcmpstat/dvpsvl.h"      /* for DVPSVOILUT */


DVPSVOILUT_PList::DVPSVOILUT_PList()
: list_()
{
}

DVPSVOILUT_PList::DVPSVOILUT_PList(const DVPSVOILUT_PList &arg)
: list_()
{
  OFListConstIterator(DVPSVOILUT *) first = arg.list_.begin();
  OFListConstIterator(DVPSVOILUT *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSVOILUT_PList::~DVPSVOILUT_PList()
{
  clear();
}

void DVPSVOILUT_PList::clear()
{
  OFListIterator(DVPSVOILUT *) first = list_.begin();
  OFListIterator(DVPSVOILUT *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSVOILUT_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSVOILUT *newObject = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newObject = new DVPSVOILUT();
        if (newObject && ditem)
        {
          if (EC_Normal == newObject->read(*ditem)) list_.push_back(newObject); else delete(newObject);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}


DVPSVOILUT *DVPSVOILUT_PList::getVOILUT(size_t idx)
{
  OFListIterator(DVPSVOILUT *) first = list_.begin();
  OFListIterator(DVPSVOILUT *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

/*
 *  $Log: dvpsvll.cc,v $
 *  Revision 1.12  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.10  2005-12-08 15:46:56  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.8  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.7  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.6  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.5  2001/09/26 15:36:36  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:41  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/06/02 16:01:10  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/03/08 16:29:13  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1998/12/22 17:57:21  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */
