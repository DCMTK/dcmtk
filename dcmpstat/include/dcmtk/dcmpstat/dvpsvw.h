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
 *  Update Date:      $Date: 2010-10-14 13:16:37 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSVW_H
#define DVPSVW_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcerror.h"     /* for OFCondition */

class DcmDecimalString;
class DcmLongString;

/** the representation of one VOI Window in a DICOM image.
 */

class DVPSVOIWindow
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

/*
 *  $Log: dvpsvw.h,v $
 *  Revision 1.9  2010-10-14 13:16:37  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.8  2010-10-07 14:31:36  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.7  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.6  2005-12-08 16:04:13  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2001/09/26 15:36:19  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:25  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/06/02 16:00:55  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/03/08 16:28:59  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1998/12/22 17:57:10  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */
