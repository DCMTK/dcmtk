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
 *    classes: DVPSVOIWindow
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-06-02 16:00:55 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSVW_H__
#define __DVPSVW_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dcerror.h"     /* for E_Condition */

class DcmDecimalString;
class DcmLongString;
class OFConsole;

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
  E_Condition read(size_t idx, DcmDecimalString &wcenter, DcmDecimalString& wwidth, DcmLongString *expl=NULL);

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

  /** sets a new log stream
   *  @param stream new log stream, NULL for default logstream
   *  @param verbMode verbose mode flag
   *  @param dbgMode debug mode flag
   */
  void setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode);
  
private:
  /// private undefined assignment operator
  DVPSVOIWindow& operator=(const DVPSVOIWindow&);
  // window center
  double windowCenter;
  // window width
  double windowWidth;
  // optional window explanation
  OFString windowCenterWidthExplanation;

  /** output stream for error messages, never NULL
   */
  OFConsole *logstream;

  /** flag indicating whether we're operating in verbose mode
   */
  OFBool verboseMode;
   
  /** flag indicating whether we're operating in debug mode
   */
  OFBool debugMode;

};

#endif

/*
 *  $Log: dvpsvw.h,v $
 *  Revision 1.3  2000-06-02 16:00:55  meichel
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

