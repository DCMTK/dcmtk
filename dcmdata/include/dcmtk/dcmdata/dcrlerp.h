/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: representation parameter for RLE
 *
 */

#ifndef DCRLERP_H
#define DCRLERP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcpixel.h" /* for class DcmRepresentationParameter */

/** representation parameter for RLE.
 *  This is actually a dummy class since there are no compression
 *  options in the RLE algorithm.
 */
class DCMTK_DCMDATA_EXPORT DcmRLERepresentationParameter: public DcmRepresentationParameter
{
public:

  /// constructor
  DcmRLERepresentationParameter();

  /// copy constructor
  DcmRLERepresentationParameter(const DcmRLERepresentationParameter& arg);

  /// destructor
  virtual ~DcmRLERepresentationParameter();
  
  /** this methods creates a copy of type DcmRepresentationParameter *
   *  it must be overweritten in every subclass.
   *  @return copy of this object
   */
  virtual DcmRepresentationParameter *clone() const;

  /** returns the class name as string.
   *  can be used in operator== as poor man's RTTI replacement.
   */
  virtual const char *className() const;

  /** compares an object to another DcmRepresentationParameter.
   *  Implementation must make sure that classes are comparable.
   *  @param arg representation parameter to compare with
   *  @return true if equal, false otherwise.
   */
  virtual OFBool operator==(const DcmRepresentationParameter &arg) const;

};

#endif
