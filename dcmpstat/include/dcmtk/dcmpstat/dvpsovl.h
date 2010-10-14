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
 *    classes: DVPSOverlay_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSOVL_H
#define DVPSOVL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"

class DVPSOverlay;

/** the list of overlays contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the list of overlays
 *  (all instances of the Overlay Plane Module repeating elements)
 *  contained in a Presentation State object.
 */

class DVPSOverlay_PList
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
  OFCondition read(DcmItem &dset);

  /** writes the overlays managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the overlays are written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

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

  /** gets the number of overlays in managed by this object.
   *  @return number of overlays in this list.
   */
  size_t size() const { return list_.size(); }

  /** gets the overlay object with the given index.
   *  @param idx index of the overlay, must be < size().
   *  @return pointer to overlay object or NULL.
   */
  DVPSOverlay *getOverlay(size_t idx);

  /** removes the overlay object with the given index.
   *  @param idx index of the overlay, must be < size().
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition removeOverlay(size_t idx);

  /** changes the repeating group used for an overlay.
   *  @param idx index of the overlay, must be < size().
   *  @param newGroup new repeating group number 0x6000-0x601F (even)
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition changeOverlayGroup(size_t idx, Uint16 newGroup);

  /** adds a new overlay bitmap.
   *  The overlay is read from a DICOM dataset which must contain the
   *  attributes required for a graphic or ROI overlay, see class DVPSOverlay.
   *  The dataset can be an image or standalone overlay IOD.
   *  The overlay data is copied into the presentation state, i.e. the DICOM dataset
   *  can be deleted after execution of this method.
   *  @param overlayIOD the DICOM dataset from which the overlay is to be read
   *  @groupInItem the repeating group 0x6000..0x61F (even) of the overlay to be read
   *  @param newGroup repeating group number 0x6000-0x601F (even) to be used for
   *    the overlay in the presentation state.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition addOverlay(DcmItem& overlayIOD, Uint16 groupInItem, Uint16 newGroup);

  /** get overlay by group
   *  @param group overlay repeating group to be checked
   *  @return a pointer to the matching DVPSOverlay object if found,
   *    NULL otherwise.
   */
  DVPSOverlay *getOverlayGroup(Uint16 group);

private:

  /// private undefined assignment operator
  DVPSOverlay_PList& operator=(const DVPSOverlay_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSOverlay *> list_;

};

#endif

/*
 *  $Log: dvpsovl.h,v $
 *  Revision 1.14  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.13  2010-10-07 14:31:36  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.12  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.11  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.10  2005-12-08 16:03:54  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2003/06/04 10:18:06  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.8  2001/09/26 15:36:13  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:50:19  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/06/02 16:00:49  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.5  2000/03/08 16:28:54  meichel
 *  Updated copyright header.
 *
 *  Revision 1.4  2000/03/06 18:23:15  joergr
 *  Added const type specifier to derived method (reported by Sun CC 4.2).
 *
 *  Revision 1.3  1998/12/22 17:57:06  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:31  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:30  meichel
 *  Initial Release.
 *
 *
 */
