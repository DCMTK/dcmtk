/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *    classes: DVPSOverlay_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:30 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSOVL_H__
#define __DVPSOVL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"

class DVPSOverlay;

/** the list of overlays contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the list of overlays
 *  (all instances of the Overlay Plane Module repeating elements)
 *  contained in a Presentation State object.
 */

class DVPSOverlay_PList: private OFList<DVPSOverlay *>
{
public:
  /// default constructor
  DVPSOverlay_PList();
  
  /// copy constructor
  DVPSOverlay_PList(const DVPSOverlay_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSOverlay_PList object containing
   *  a deep copy of this object.
   */
  DVPSOverlay_PList *clone() { return new DVPSOverlay_PList(*this); }

  /// destructor
  virtual ~DVPSOverlay_PList();

  /** reads all overlay groups from a DICOM dataset.
   *  This method checks which overlays are contained in the DICOM dataset.
   *  All overlays that contain the OverlayData element are copied
   *  into the "list of overlays" structure managed by this object.
   *  The completeness of the overlays (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the overlays are read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the overlays managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the overlays are written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
  
  /** check presence of overlay group
   *  @param group overlay repeating group to be checked
   *  @return OFTrue if the specified overlay group is present in the
   *     list of overlays managed by this object.
   */
  OFBool haveOverlayGroup(Uint16 group);
  
};

#endif

/*
 *  $Log: dvpsovl.h,v $
 *  Revision 1.1  1998-11-27 14:50:30  meichel
 *  Initial Release.
 *
 *
 */

