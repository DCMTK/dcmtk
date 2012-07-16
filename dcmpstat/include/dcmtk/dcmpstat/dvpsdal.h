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
 *    classes: DVPSDisplayedArea_PList
 *
 */

#ifndef DVPSDAL_H
#define DVPSDAL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmdata/dcitem.h"

class DVPSDisplayedArea;
class DVPSReferencedSeries_PList;

/** the list of displayed area selections contained in a presentation state (internal use only).
 *  This class manages the data structures comprising one complete displayed area selection sequence
 *  contained in a presentation state object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSDisplayedArea_PList
{
public:
  /// default constructor
  DVPSDisplayedArea_PList();
  
  /// copy constructor
  DVPSDisplayedArea_PList(const DVPSDisplayedArea_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSDisplayedArea_PList object containing
   *  a deep copy of this object.
   */
  DVPSDisplayedArea_PList *clone() { return new DVPSDisplayedArea_PList(*this); }

  /// destructor
  virtual ~DVPSDisplayedArea_PList();

  /** reads a list of displayed area selections (DisplayedAreaSelectionSequence) from a DICOM dataset.
   *  The DICOM elements of the displayed area selection item are copied from the dataset to this object.
   *  The completeness of all items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the sequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);
  
  /** writes the list of displayed area selections managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the DisplayedAreaSelectionSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
      
  /** gets the number of displayed area selections in this list.
   *  @return the number of displayed area selections.
   */
  size_t size() const { return list_.size(); }

  /** checks if an displayed area selection exists for the given image and frame.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @return pointer to the displayed area if it exists, NULL otherwise.
   */
  DVPSDisplayedArea *findDisplayedArea(const char *instanceUID, unsigned long frame);

  /** finds or creates a displayed area selection SQ item
   *  with an applicability controlled by the applicability, instanceUID and frame
   *  parameters. The displayed area selection sequence is rearranged such that
   *  all other referenced images/frames keep their old displayed area settings.
   *  @param allReferences list of series/instance references registered for the
   *    presentation state.
   *  @param sopclassUID SOP class UID of the current image
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @param numberOfFrames number of frames of the current image
   *  @param applicability applicability of the new displayed area selection
   *  @return pointer to a displayed area selection object from the list
   *    that matches the applicability parameters. NULL is returned if
   *    out of memory.
   */
  DVPSDisplayedArea *createDisplayedArea(
    DVPSReferencedSeries_PList& allReferences,
    const char *sopclassUID, 
    const char *instanceUID, 
    unsigned long frame, 
    unsigned long numberOfFrames, 
    DVPSObjectApplicability applicability);

  /** adjusts all displayed area coordinates for the rotation and flipping
   *  status of the image.
   *  @param rotationFrom previous rotation
   *  @param isFlippedFrom previous flip status
   *  @param rotationTo new rotation
   *  @param isFlippedTo new flip status
   */
  void rotateAndFlip(
    DVPSRotationType rotationFrom, 
    OFBool isFlippedFrom,
    DVPSRotationType rotationTo, 
    OFBool isFlippedTo);

private:

  /** private undefined assignment operator
   */
  DVPSDisplayedArea_PList& operator=(const DVPSDisplayedArea_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSDisplayedArea *> list_;

};


#endif
