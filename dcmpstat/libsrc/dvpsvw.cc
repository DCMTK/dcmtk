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
 *    classes: DVPSVOIWindow
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsvw.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

/* --------------- class DVPSVOIWindow --------------- */

DVPSVOIWindow::DVPSVOIWindow()
: windowCenter(0.0)
, windowWidth(0.0)
, windowCenterWidthExplanation()
{
}

DVPSVOIWindow::DVPSVOIWindow(const DVPSVOIWindow& copy)
: windowCenter(copy.windowCenter)
, windowWidth(copy.windowWidth)
, windowCenterWidthExplanation(copy.windowCenterWidthExplanation)
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
      char descr[32];
      windowCenterWidthExplanation = "window center=";
      OFStandard::ftoa(descr, sizeof(descr), wc, OFStandard::ftoa_uppercase);
      windowCenterWidthExplanation += descr;
      windowCenterWidthExplanation += " width=";
      OFStandard::ftoa(descr, sizeof(descr), ww, OFStandard::ftoa_uppercase);
      windowCenterWidthExplanation += descr;
    }
  }
  return result;
}

const char *DVPSVOIWindow::getExplanation()
{
  if (windowCenterWidthExplanation.length() == 0) return NULL;
  return windowCenterWidthExplanation.c_str();
}

/*
 *  $Log: dvpsvw.cc,v $
 *  Revision 1.11  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.10  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.9  2005-12-08 15:46:57  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2002/12/04 10:41:39  meichel
 *  Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *    double to string conversions that are supposed to be locale independent
 *
 *  Revision 1.7  2002/11/27 15:48:17  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.6  2002/08/20 12:41:01  meichel
 *  Added explicit includes for header files included implicitly
 *    via dcstream before.
 *
 *  Revision 1.5  2001/09/26 15:36:36  meichel
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

