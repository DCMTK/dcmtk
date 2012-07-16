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
 *    classes: DVPSVOILUT_PList
 *
 */

#ifndef DVPSVLL_H
#define DVPSVLL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmdata/dcitem.h"

class DVPSVOILUT;


/** the list of VOI LUTs contained in an image attached to a presentation state.
 *  This class manages the data structures comprising the VOI LUT Sequence
 *  of one image attached to a presentation state.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSVOILUT_PList
{
public:
  /// default constructor
  DVPSVOILUT_PList();

  /// copy constructor
  DVPSVOILUT_PList(const DVPSVOILUT_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSVOILUT_PList object containing
   *  a deep copy of this object.
   */
  DVPSVOILUT_PList *clone() { return new DVPSVOILUT_PList(*this); }

  /// destructor
  virtual ~DVPSVOILUT_PList();

  /** reads a list of VOI LUTs from a DICOM dataset.
   *  The DICOM elements of the VOI LUT Sequence are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the VOI LUT Sequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** get number of VOI LUTs in this list.
   *  @return the number of VOI LUTs.
   */
  size_t size() const { return list_.size(); }

  /** returns a pointer to the VOI LUT with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < size()
   *  @return pointer to VOI LUT or NULL
   */
  DVPSVOILUT *getVOILUT(size_t idx);

private:

  /// private undefined assignment operator
  DVPSVOILUT_PList& operator=(const DVPSVOILUT_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSVOILUT *> list_;
};

#endif
