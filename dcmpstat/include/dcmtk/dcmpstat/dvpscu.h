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
 *    classes: DVPSCurve
 *
 */

#ifndef DVPSCU_H
#define DVPSCU_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcerror.h"

class DcmItem;

/** the representation of one Curve in a DICOM image.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSCurve
{
public:
  /// default constructor
  DVPSCurve();
  
  /// copy constructor
  DVPSCurve(const DVPSCurve& copy);

  /** clone method.
   *  @return a pointer to a new DVPSCurve object containing
   *  a copy of this object.
   */
  DVPSCurve *clone() { return new DVPSCurve(*this); }

  /// destructor
  virtual ~DVPSCurve();

  /** reads a curve from a DICOM dataset.
   *  The DICOM elements of the Graphic Object item are copied
   *  from the dataset to this object.
   *  The completeness of the curve (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the data is to be read
   *  @param group lower byte of the curve repeating group to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset, Uint8 group);

  /** get group number of curve repeating group managed by this object.
   *  @return the lower byte of the curve group
   */
  Uint8 getCurveGroup() { return curveGroup; }

  /** gets the number of points in the curve.
   *  @return number of points
   */
  size_t getNumberOfPoints() { return numberOfPoints; }

  /** gets the type of data in the curve (ROI or POLY).
   *  @return type of data in curve
   */
  DVPSCurveType getTypeOfData() { return typeOfData; }

  /** gets one point from the curve data.
   *  @param idx index of the curve point, must be < getNumberOfPoints();
   *  @param x upon success the x value of the point is returned in this parameter
   *  @param y upon success the y value of the point is returned in this parameter   
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getPoint(size_t idx, double& x, double& y);

  /** gets the curve description string if present.
   *  If the description string is absent, this method returns NULL or an empty string.
   *  @return curve description
   */
  const char *getCurveDescription() { return curveDescription.c_str(); }

  /** gets the curve label string if present.
   *  If the label string is absent, this method returns NULL or an empty string.
   *  @return curve label
   */
  const char *getCurveLabel() { return curveLabel.c_str(); }

  /** gets the curve axis units string for the X dimension if present.
   *  If the string is absent, this method returns NULL or an empty string.
   *  @return curve description
   */
  const char *getCurveAxisUnitsX() { return axisUnitsX.c_str(); }

  /** gets the curve axis units string for the Y dimension if present.
   *  If the string is absent, this method returns NULL or an empty string.
   *  @return curve description
   */
  const char *getCurveAxisUnitsY() { return axisUnitsY.c_str(); }

private:
  /// private undefined assignment operator
  DVPSCurve& operator=(const DVPSCurve&);
  
  /// lower byte of the curve repeating group managed by this object
  Uint8 curveGroup;
  /// number of points in curve data
  size_t numberOfPoints;
  /// type of curve data
  DVPSCurveType typeOfData;
  /// curve data, converted to double
  double *curveData;
  /// curve data description if present
  OFString curveDescription;
  /// curve label if present
  OFString curveLabel;
  /// axis units X if present
  OFString axisUnitsX;
  /// axis units Y if present
  OFString axisUnitsY;

};

#endif
