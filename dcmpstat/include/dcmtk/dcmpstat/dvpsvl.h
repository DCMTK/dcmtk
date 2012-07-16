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
 *    classes: DVPSVOILUT
 *
 */

#ifndef DVPSVL_H
#define DVPSVL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"

class DVPSSoftcopyVOI;

/** the representation of one VOI LUT in a DICOM image.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSVOILUT
{
public:
  /// default constructor
  DVPSVOILUT();

  /// copy constructor
  DVPSVOILUT(const DVPSVOILUT& copy);

  /** clone method.
   *  @return a pointer to a new DVPSVOILUT object containing
   *  a copy of this object.
   */
  DVPSVOILUT *clone() { return new DVPSVOILUT(*this); }

  /// destructor
  virtual ~DVPSVOILUT();

  /** reads a VOI LUT from a DICOM dataset.
   *  The DICOM elements of the VOI LUT item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the VOI LUT Sequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** resets the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** gets the LUT explanation for this VOI LUT.
   *  If no explanation exists, NULL is returned.
   *  @return LUT explanation or NULL
   */
  const char *getExplanation();

  /** assigns the contents of this VOI LUT to the
   *  references passed as parameters.
   *  @param reference to the Softcopy VOI in which the LUT is stored.
   */
  OFCondition assign(DVPSSoftcopyVOI& voi);

private:

  /// private undefined assignment operator
  DVPSVOILUT& operator=(const DVPSVOILUT&);

  /// Module=VOI_LUT, VR=xs, VM=3, Type 1c
  DcmUnsignedShort         voiLUTDescriptor;
  /// Module=VOI_LUT, VR=LO, VM=1, Type 3
  DcmLongString            voiLUTExplanation;
  /// Module=VOI_LUT, VR=xs, VM=1-n, Type 1c
  DcmUnsignedShort         voiLUTData;
};

#endif
