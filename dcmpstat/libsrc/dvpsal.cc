/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *    classes: DVPSOverlayCurveActivationLayer
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-06-02 16:00:57 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsal.h"
#include "dvpsdef.h"     /* for constants and macros */


/* --------------- class DVPSOverlayCurveActivationLayer --------------- */

DVPSOverlayCurveActivationLayer::DVPSOverlayCurveActivationLayer()
: repeatingGroup(0)
, activationLayer(DCM_OverlayActivationLayer) // default is Overlay not Curve
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSOverlayCurveActivationLayer::DVPSOverlayCurveActivationLayer(const DVPSOverlayCurveActivationLayer& copy)
: repeatingGroup(copy.repeatingGroup)
, activationLayer(copy.activationLayer)
, logstream(copy.logstream)
, verboseMode(copy.verboseMode)
, debugMode(copy.debugMode)
{
}

DVPSOverlayCurveActivationLayer::~DVPSOverlayCurveActivationLayer()
{
}

E_Condition DVPSOverlayCurveActivationLayer::read(DcmItem &dset, Uint16 ovGroup)
{
  E_Condition result = EC_Normal;
  DcmStack stack;

  repeatingGroup = ovGroup;
  activationLayer.setGTag(repeatingGroup);
  READ_FROM_DATASET(DcmCodeString, activationLayer)

  if (activationLayer.getVM() > 1)
  {
    result=EC_IllegalCall;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: presentation state contains a curve or overlay activation layer with VM > 1" << endl;
      logstream->unlockCerr();
    }
  }

  return result;
}

E_Condition DVPSOverlayCurveActivationLayer::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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

void DVPSOverlayCurveActivationLayer::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
}


/*
 *  $Log: dvpsal.cc,v $
 *  Revision 1.5  2000-06-02 16:00:57  meichel
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

