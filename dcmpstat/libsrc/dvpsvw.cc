/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *    classes: DVPSVOIWindow
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 15:36:36 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsvw.h"
#include "dctk.h"

/* --------------- class DVPSVOIWindow --------------- */

DVPSVOIWindow::DVPSVOIWindow()
: windowCenter(0.0)
, windowWidth(0.0)
, windowCenterWidthExplanation()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSVOIWindow::DVPSVOIWindow(const DVPSVOIWindow& copy)
: windowCenter(copy.windowCenter)
, windowWidth(copy.windowWidth)
, windowCenterWidthExplanation(copy.windowCenterWidthExplanation)
, logstream(copy.logstream)
, verboseMode(copy.verboseMode)
, debugMode(copy.debugMode)
{
}

DVPSVOIWindow::~DVPSVOIWindow()
{
}

void DVPSVOIWindow::clear()
{
  windowCenter = 0.0;
  windowWidth = 0.0;
  windowCenterWidthExplanation.clear();
}

OFCondition DVPSVOIWindow::read(size_t idx, DcmDecimalString &wcenter, DcmDecimalString& wwidth, DcmLongString *expl)
{
  if (wcenter.getVM() <= idx) return EC_IllegalCall;
  if (wwidth.getVM() <= idx) return EC_IllegalCall;
  Float64 wc=0.0, ww=0.0;
  OFCondition result = wcenter.getFloat64(wc, idx);
  if (EC_Normal==result) result= wwidth.getFloat64(ww, idx);
  if (EC_Normal==result)
  {
    windowCenter = wc;
    windowWidth = ww;
    if (expl && (expl->getVM() > idx)) expl->getOFString(windowCenterWidthExplanation, idx);
    else
    {
      // create default description for VOI Window
      char descr[100];
      sprintf(descr, "window center=%G width=%G", wc, ww);
      windowCenterWidthExplanation = descr;
    }
  }
  return result;
}

const char *DVPSVOIWindow::getExplanation()
{
  if (windowCenterWidthExplanation.length() == 0) return NULL;
  return windowCenterWidthExplanation.c_str();
}

void DVPSVOIWindow::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
}


/*
 *  $Log: dvpsvw.cc,v $
 *  Revision 1.5  2001-09-26 15:36:36  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:42  meichel
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

