/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 *    classes: DVPSOverlayCurveActivationLayer
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmpstat/dvpsal.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */


/* --------------- class DVPSOverlayCurveActivationLayer --------------- */

DVPSOverlayCurveActivationLayer::DVPSOverlayCurveActivationLayer()
: repeatingGroup(0)
, activationLayer(DCM_OverlayActivationLayer) // default is Overlay not Curve
{
}

DVPSOverlayCurveActivationLayer::DVPSOverlayCurveActivationLayer(const DVPSOverlayCurveActivationLayer& copy)
: repeatingGroup(copy.repeatingGroup)
, activationLayer(copy.activationLayer)
{
}

DVPSOverlayCurveActivationLayer::~DVPSOverlayCurveActivationLayer()
{
}

OFCondition DVPSOverlayCurveActivationLayer::read(DcmItem &dset, Uint16 ovGroup)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  repeatingGroup = ovGroup;
  activationLayer.setGTag(repeatingGroup);
  READ_FROM_DATASET(DcmCodeString, EVR_CS, activationLayer)

  if (activationLayer.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a curve or overlay activation layer with VM > 1");
  }

  return result;
}

OFCondition DVPSOverlayCurveActivationLayer::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;

  ADD_REPEATING_ELEMENT_TO_DATASET(DcmCodeString, activationLayer, repeatingGroup)
  
  return result;
}

void DVPSOverlayCurveActivationLayer::setActivationLayer(const char *aLayer)
{
  if (aLayer) activationLayer.putString(aLayer); else activationLayer.clear();
  return;
}

void DVPSOverlayCurveActivationLayer::setRepeatingGroup(Uint16 rGroup)
{
  repeatingGroup = rGroup;
  return;
}

const char *DVPSOverlayCurveActivationLayer::getActivationLayer()
{
  char *c = NULL;
  if (EC_Normal == activationLayer.getString(c)) return c; else return NULL;
}

Uint16 DVPSOverlayCurveActivationLayer::getRepeatingGroup()
{
  return repeatingGroup;
}

OFBool DVPSOverlayCurveActivationLayer::isRepeatingGroup(Uint16 rGroup)
{
  if (rGroup==repeatingGroup) return OFTrue; else return OFFalse;
}
