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
 *    classes: DVPSCurve_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSCUL_H
#define DVPSCUL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"

class DVPSCurve;

/** the list of curves contained in image which is attached to a presentation state.
 *  This class manages the data structures comprising the list of curves
 *  (all instances of the Curve Module repeating elements)
 *  contained in an image object.
 */

class DVPSCurve_PList
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

/*
 *  $Log: dvpscul.h,v $
 *  Revision 1.11  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.10  2010-10-07 14:31:35  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.9  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.8  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.7  2005-12-08 16:03:38  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.6  2003/06/04 10:18:06  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.5  2001/09/26 15:36:09  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:14  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/06/02 16:00:44  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/03/08 16:28:50  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1998/12/22 17:57:05  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */
