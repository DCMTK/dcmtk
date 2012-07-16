/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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

#ifndef DVPSVW_H
#define DVPSVW_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcerror.h"     /* for OFCondition */

class DcmDecimalString;
class DcmLongString;

/** the representation of one VOI Window in a DICOM image.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSVOIWindow
{
public:
  /// default constructor
  DVPSVOIWindow();

  /// copy constructor
  DVPSVOIWindow(const DVPSVOIWindow& copy);

  /** clone method.
   *  @return a pointer to a new DVPSVOIWindow object containing
   *  a copy of this object.
   */
  DVPSVOIWindow *clone() { return new DVPSVOIWindow(*this); }

  /// destructor
  virtual ~DVPSVOIWindow();

  /** reads a VOI Window from DICOM elements.
   *  The DICOM elements of the given VOI Window are copied to this object.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param idx the index of the VOI window to be read, must be < wcenter.getVM()
   *  @param wcenter the window center(s)
   *  @param wwidth the window width(s). wwidth.getVM() must be == wcenter.getVM().
   *  @param expl the window center/window width explanation. If omitted, explanation remains empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(size_t idx, DcmDecimalString &wcenter, DcmDecimalString& wwidth, DcmLongString *expl=NULL);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** gets the Window center/width explanation for this VOI Window.
   *  If no explanation exists, NULL is returned.
   *  @return Window explanation or NULL
   */
  const char *getExplanation();

  /** gets the Window Center of this VOI Window.
   *  @return window center
   */
  double getWindowCenter() { return windowCenter; }

  /** gets the Window Width of this VOI Window.
   *  @return window width
   */
  double getWindowWidth() { return windowWidth; }

private:
  /// private undefined assignment operator
  DVPSVOIWindow& operator=(const DVPSVOIWindow&);
  // window center
  double windowCenter;
  // window width
  double windowWidth;
  // optional window explanation
  OFString windowCenterWidthExplanation;

};

#endif
