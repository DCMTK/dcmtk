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
 *    classes: DVPSSoftcopyVOI_PList
 *
 */

#ifndef DVPSSVL_H
#define DVPSSVL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmdata/dcitem.h"

class DVPSSoftcopyVOI;
class DVPSReferencedSeries_PList;

/** the list of softcopy VOI LUT items contained in a presentation state (internal use only).
 *  This class manages the data structures comprising one complete Softcopy VOI LUT sequence
 *  contained in a presentation state object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSSoftcopyVOI_PList
{
public:
  /// default constructor
  DVPSSoftcopyVOI_PList();

  /// copy constructor
  DVPSSoftcopyVOI_PList(const DVPSSoftcopyVOI_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSSoftcopyVOI_PList object containing
   *  a deep copy of this object.
   */
  DVPSSoftcopyVOI_PList *clone() { return new DVPSSoftcopyVOI_PList(*this); }

  /// destructor
  virtual ~DVPSSoftcopyVOI_PList();

  /** reads a list of softcopy VOI LUTs (SoftcopyVOILUTSequence) from a DICOM dataset.
   *  The DICOM elements of the softcopy VOI LUT item are copied from the dataset to this object.
   *  The completeness of all items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the sequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the list of softcopy VOI LUTs managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the SoftcopyVOILUTSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** gets the number of softcopy VOI LUTs in this list.
   *  @return the number of softcopy VOI LUTs.
   */
  size_t size() const { return list_.size(); }

  /** creates a default softcopy VOI LUT sequence for a presentation state from a DICOM image.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset containing the image IOD
   *  @param allReferences list of series/instance references registered for the
   *    presentation state.
   *  @param sopclassUID SOP class UID of the current image
   *  @param instanceUID SOP instance UID of the current image
   *  @param voiActivation flag defining how VOI LUTs or VOI window width/center should
   *    be handled. Default: Use VOI and prefer VOI LUT from VOI window.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition createFromImage(
    DcmItem &dset,
    DVPSReferencedSeries_PList& allReferences,
    const char *sopclassUID,
    const char *instanceUID,
    DVPSVOIActivation voiActivation);

  /** checks if a softcopy VOI LUT item exists for the given image and frame.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @return pointer to the softcopy VOI LUT item if it exists, NULL otherwise.
   */
  DVPSSoftcopyVOI *findSoftcopyVOI(const char *instanceUID, unsigned long frame);

  /** finds or creates a softcopy VOI LUT SQ item
   *  with an applicability controlled by the applicability, instanceUID and frame
   *  parameters. The softcopy VOI LUT sequence is rearranged such that
   *  all other referenced images/frames keep their old settings.
   *  @param allReferences list of series/instance references registered for the
   *    presentation state.
   *  @param sopclassUID SOP class UID of the current image
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @param numberOfFrames number of frames of the current image
   *  @param applicability applicability of the new softcopy VOI LUT
   *  @return pointer to a softcopy VOI LUT object from the list
   *    that matches the applicability parameters. NULL is returned if
   *    out of memory.
   */
  DVPSSoftcopyVOI *createSoftcopyVOI(
    DVPSReferencedSeries_PList& allReferences,
    const char *sopclassUID,
    const char *instanceUID,
    unsigned long frame,
    unsigned long numberOfFrames,
    DVPSObjectApplicability applicability);

  /** removes the softcopy VOI for a set of references
   *  controlled by the applicability, instanceUID and frame
   *  parameters. The softcopy VOI LUT sequence is rearranged such that
   *  all other referenced images/frames keep their old settings.
   *  @param allReferences list of series/instance references registered for the
   *    presentation state.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @param numberOfFrames number of frames of the current image
   *  @param applicability applicability of the VOI LUT removal
   */
  void removeSoftcopyVOI(
    DVPSReferencedSeries_PList& allReferences,
    const char *instanceUID,
    unsigned long frame,
    unsigned long numberOfFrames,
    DVPSObjectApplicability applicability);

private:

  /// private undefined assignment operator
  DVPSSoftcopyVOI_PList& operator=(const DVPSSoftcopyVOI_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSSoftcopyVOI *> list_;

};


#endif
