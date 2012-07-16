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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm
 *
 *  Purpose: representation parameters for JPEG-LS
 *
 */

#ifndef DCMJPLS_DJRPARAM_H
#define DCMJPLS_DJRPARAM_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcpixel.h" /* for class DcmRepresentationParameter */
#include "dcmtk/dcmjpls/dldefine.h"

/** representation parameter for JPEG-LS
 */
class DCMTK_DCMJPLS_EXPORT DJLSRepresentationParameter: public DcmRepresentationParameter
{
public:

  /** constructor
   *  @param nearlosslessDeviation used as parameter NEAR in JPEG-LS nearlossless-encoding process
   *  @param losslessProcess true if lossless process is requested
   */
  DJLSRepresentationParameter(
    Uint16 nearlosslessDeviation = 2,
    OFBool losslessProcess = OFTrue);

  /// copy constructor
  DJLSRepresentationParameter(const DJLSRepresentationParameter& arg);

  /// destructor
  virtual ~DJLSRepresentationParameter();

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

  /** returns the desired NEAR parameter
   *  @return return desired NEAR-Parameter
   */
  Uint16 getnearlosslessDeviation() const
  {
    return nearlosslessDeviation_;
  }

  /** returns true if lossless compression is desired
   *  @return true if lossless compression is desired
   */
  OFBool useLosslessProcess() const
  {
    return losslessProcess_;
  }

private:

  /** desired NEAR parameter
   *  Default is 0 (lossless compression).
   *  Ignored during lossless compression.
   */
  Uint16 nearlosslessDeviation_;

  /// true if lossless process should be used even in lossy transfer syntax
  OFBool losslessProcess_;

};


#endif
