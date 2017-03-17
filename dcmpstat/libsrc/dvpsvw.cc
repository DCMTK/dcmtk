/*
 *
 *  Copyright (C) 1998-2017, OFFIS e.V.
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
  OFCondition result = wcenter.getFloat64(wc, OFstatic_cast(Uint32, idx));
  if (EC_Normal==result) result= wwidth.getFloat64(ww, OFstatic_cast(Uint32, idx));
  if (EC_Normal==result)
  {
    windowCenter = wc;
    windowWidth = ww;
    if (expl && (expl->getVM() > idx)) expl->getOFString(windowCenterWidthExplanation, OFstatic_cast(Uint32, idx));
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
  if (windowCenterWidthExplanation.empty()) return NULL;
  return windowCenterWidthExplanation.c_str();
}
