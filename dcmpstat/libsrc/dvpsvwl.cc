/*
 *
 *  Copyright (C) 1998-2003, OFFIS
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
 *  Update Date:      $Date: 2003-06-04 10:18:07 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsvwl.h"
#include "dvpsvw.h"      /* for DVPSVOIWindow */
#include "dvpsdef.h"     /* for constants and macros */


DVPSVOIWindow_PList::DVPSVOIWindow_PList()
: list_()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSVOIWindow_PList::DVPSVOIWindow_PList(const DVPSVOIWindow_PList &arg)
: list_()
, logstream(arg.logstream)
, verboseMode(arg.verboseMode)
, debugMode(arg.debugMode)
{
  OFListIterator(DVPSVOIWindow *) first = arg.list_.begin();
  OFListIterator(DVPSVOIWindow *) last = arg.list_.end();
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

void DVPSVOIWindow_PList::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  OFListIterator(DVPSVOIWindow *) first = list_.begin();
  OFListIterator(DVPSVOIWindow *) last = list_.end();
  while (first != last)
  {
    (*first)->setLog(logstream, verbMode, dbgMode);
    ++first;
  }	
}

/*
 *  $Log: dvpsvwl.cc,v $
 *  Revision 1.8  2003-06-04 10:18:07  meichel
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

