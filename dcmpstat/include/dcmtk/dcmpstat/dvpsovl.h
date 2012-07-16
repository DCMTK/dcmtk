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
 *    classes: DVPSOverlay_PList
 *
 */

#ifndef DVPSOVL_H
#define DVPSOVL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dpdefine.h"

class DVPSOverlay;

/** the list of overlays contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the list of overlays
 *  (all instances of the Overlay Plane Module repeating elements)
 *  contained in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSOverlay_PList
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
