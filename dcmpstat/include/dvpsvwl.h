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
 *    classes: DVPSVOIWindow_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:28:59 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSVWL_H__
#define __DVPSVWL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"

class DVPSVOIWindow;


/** the list of VOI Windows contained in an image attached to a presentation state.
 *  This class manages the data structures comprising the VOI Windows
 *  of one image attached to a presentation state.
 */

class DVPSVOIWindow_PList: private OFList<DVPSVOIWindow *>
{
public:
  /// default constructor
  DVPSVOIWindow_PList();
  
  /// copy constructor
  DVPSVOIWindow_PList(const DVPSVOIWindow_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSVOIWindow_PList object containing
   *  a deep copy of this object.
   */
  DVPSVOIWindow_PList *clone() { return new DVPSVOIWindow_PList(*this); }

  /// destructor
  virtual ~DVPSVOIWindow_PList();

  /** reads a list of VOI Windows from a DICOM dataset.
   *  The DICOM elements of the VOI Window are copied
   *  from the dataset to this object.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the VOI Windows are to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
  
  /** get number of VOI Windows in this list.
   *  @return the number of VOI Windows.
   */
  size_t size() const { return OFList<DVPSVOIWindow *>::size(); }  

  /** returns a pointer to the VOI Window with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < size()
   *  @return pointer to VOI Window or NULL
   */
  DVPSVOIWindow *getVOIWindow(size_t idx);
  
};

#endif

/*
 *  $Log: dvpsvwl.h,v $
 *  Revision 1.2  2000-03-08 16:28:59  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1998/12/22 17:57:10  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */

