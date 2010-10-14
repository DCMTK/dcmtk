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
 *    classes: DVPSOverlayCurveActivationLayer
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:31 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
  READ_FROM_DATASET(DcmCodeString, activationLayer)

  if (activationLayer.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a curve or overlay activation layer with VM > 1");
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


/*
 *  $Log: dvpsal.cc,v $
 *  Revision 1.12  2010-10-14 13:14:31  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.10  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.9  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.8  2005/12/08 15:46:18  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.7  2001/09/26 15:36:23  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:50:27  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/06/02 16:00:57  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.4  2000/05/31 13:02:35  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.3  2000/03/08 16:29:01  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  2000/03/03 14:13:57  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.1  1998/11/27 14:50:38  meichel
 *  Initial Release.
 *
 *
 */

