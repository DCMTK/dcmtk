/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: representation parameter for lossy JPEG
 *
 */

#ifndef DJRPLOSS_H
#define DJRPLOSS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcpixel.h" /* for class DcmRepresentationParameter */
#include "dcmtk/dcmjpeg/djdefine.h"

/** representation parameter for lossy JPEG
 */
class DCMTK_DCMJPEG_EXPORT DJ_RPLossy: public DcmRepresentationParameter
{
public:

  /** constructor
   *  @param aQuality quality factor
   */
  DJ_RPLossy(int aQuality=90);

  /// copy constructor
  DJ_RPLossy(const DJ_RPLossy& arg);

  /// destructor
  virtual ~DJ_RPLossy();
  
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

  /** returns the compression quality factor
   *  @return compression quality factor
   */
  int getQuality() const
  {
    return quality;
  }

private:

  /// compression quality factor
  int quality;

};


#endif
