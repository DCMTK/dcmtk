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
 *    classes: DVPSVOIWindow_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmpstat/dvpsvwl.h"
#include "dcmtk/dcmpstat/dvpsvw.h"      /* for DVPSVOIWindow */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */


DVPSVOIWindow_PList::DVPSVOIWindow_PList()
: list_()
{
}

DVPSVOIWindow_PList::DVPSVOIWindow_PList(const DVPSVOIWindow_PList &arg)
: list_()
{
  OFListConstIterator(DVPSVOIWindow *) first = arg.list_.begin();
  OFListConstIterator(DVPSVOIWindow *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSVOIWindow_PList::~DVPSVOIWindow_PList()
{
  clear();
}

void DVPSVOIWindow_PList::clear()
{
  OFListIterator(DVPSVOIWindow *) first = list_.begin();
  OFListIterator(DVPSVOIWindow *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSVOIWindow_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSVOIWindow *newObject = NULL;

  DcmDecimalString windowCenter(DCM_WindowCenter);
  DcmDecimalString windowWidth(DCM_WindowWidth);
  DcmLongString windowCenterWidthExplanation(DCM_WindowCenterWidthExplanation);

  READ_FROM_DATASET(DcmDecimalString, windowCenter)
  READ_FROM_DATASET(DcmDecimalString, windowWidth)
  READ_FROM_DATASET(DcmLongString, windowCenterWidthExplanation)

  DcmLongString *expl=NULL;
  if (windowCenterWidthExplanation.getVM() > 0) expl = &windowCenterWidthExplanation;

  size_t vm = (size_t)windowCenter.getVM();
  for (size_t i=0; i<vm; i++)
  {
      newObject = new DVPSVOIWindow();
      if (newObject)
      {
        if (EC_Normal == newObject->read(i, windowCenter, windowWidth, expl)) list_.push_back(newObject);
        else delete(newObject);
      } else result = EC_MemoryExhausted;
  }
  return result;
}


DVPSVOIWindow *DVPSVOIWindow_PList::getVOIWindow(size_t idx)
{
  OFListIterator(DVPSVOIWindow *) first = list_.begin();
  OFListIterator(DVPSVOIWindow *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

/*
 *  $Log: dvpsvwl.cc,v $
 *  Revision 1.15  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.13  2009-09-30 10:42:39  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.12  2005-12-08 15:46:58  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.11  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.10  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.9  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.8  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.7  2001/09/26 15:36:36  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:50:42  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/06/02 16:01:11  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.4  2000/05/31 13:02:42  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.3  2000/03/08 16:29:14  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  1999/04/30 16:40:50  meichel
 *  Minor code purifications to keep Sun CC 2.0.1 quiet
 *
 *  Revision 1.1  1998/12/22 17:57:22  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */
