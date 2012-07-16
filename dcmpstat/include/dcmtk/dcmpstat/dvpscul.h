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
 *    classes: DVPSCurve_PList
 *
 */

#ifndef DVPSCUL_H
#define DVPSCUL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dpdefine.h"

class DVPSCurve;

/** the list of curves contained in image which is attached to a presentation state.
 *  This class manages the data structures comprising the list of curves
 *  (all instances of the Curve Module repeating elements)
 *  contained in an image object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSCurve_PList
{
public:
  /// default constructor
  DVPSCurve_PList();
  
  /// copy constructor
  DVPSCurve_PList(const DVPSCurve_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSCurve_PList object containing
   *  a deep copy of this object.
   */
  DVPSCurve_PList *clone() { return new DVPSCurve_PList(*this); }

  /// destructor
  virtual ~DVPSCurve_PList();

  /** reads all curve groups which can be displayed in a presentation state
   *  from a DICOM dataset.
   *  This method checks which curves are contained in the DICOM dataset.
   *  All curves complying with the presentation state requirements
   *  (i.e. 2-dimensional POLY or ROI curves without curve descriptor) are copied
   *  into the "list of curves" structure managed by this object.
   *  The completeness of the curves (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the curves are read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
  
  /** check presence of curve group
   *  @param group curve repeating group to be checked
   *  @return OFTrue if the specified curve group is present in the
   *     list of curves managed by this object.
   */
  OFBool haveCurveGroup(Uint16 group);

  /** get curve by group
   *  @param group curve repeating group to be checked
   *  @return a pointer to the matching DVPSCurve object if found,
   *    NULL otherwise.
   */
  DVPSCurve *getCurveGroup(Uint16 group);

  /** get curve by index
   *  @param idx index, must be < size()
   *  @return a pointer to the matching DVPSCurve object if it exists,
   *    NULL otherwise.
   */
  DVPSCurve *getCurve(size_t idx);

  /** get number of curves in this list.
   *  @return the number of curves.
   */
  size_t size() const { return list_.size(); }  

private:

  /// private undefined assignment operator
  DVPSCurve_PList& operator=(const DVPSCurve_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSCurve *> list_;

};

#endif
