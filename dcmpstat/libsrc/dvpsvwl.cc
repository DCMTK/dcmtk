/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSVOIWindow_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-22 17:57:22 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsvwl.h"
#include "dvpsvw.h"      /* for DVPSVOIWindow */

/* --------------- a few macros avoiding copy/paste --------------- */

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}


DVPSVOIWindow_PList::DVPSVOIWindow_PList()
: OFList<DVPSVOIWindow *>()
{
}

DVPSVOIWindow_PList::DVPSVOIWindow_PList(const DVPSVOIWindow_PList &arg)
: OFList<DVPSVOIWindow *>()
{
  OFListIterator(DVPSVOIWindow *) first = arg.begin();
  OFListIterator(DVPSVOIWindow *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSVOIWindow_PList::~DVPSVOIWindow_PList()
{
  clear();
}

void DVPSVOIWindow_PList::clear()
{
  OFListIterator(DVPSVOIWindow *) first = begin();
  OFListIterator(DVPSVOIWindow *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSVOIWindow_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
  
  size_t vm = windowCenter.getVM();
  for (size_t i=0; i<vm; i++)
  {
      newObject = new DVPSVOIWindow();
      if (newObject)
      {
        if (EC_Normal == newObject->read(i, windowCenter, windowWidth, expl)) push_back(newObject); 
        else delete(newObject);
      } else result = EC_MemoryExhausted;
  }
  return result;
}


DVPSVOIWindow *DVPSVOIWindow_PList::getVOIWindow(size_t idx)
{
  OFListIterator(DVPSVOIWindow *) first = begin();
  OFListIterator(DVPSVOIWindow *) last = end();
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
 *  Revision 1.1  1998-12-22 17:57:22  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */

