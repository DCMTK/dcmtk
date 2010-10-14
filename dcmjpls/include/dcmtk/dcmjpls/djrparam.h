/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:19 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMJPLS_DJRPARAM_H
#define DCMJPLS_DJRPARAM_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcpixel.h" /* for class DcmRepresentationParameter */

/** representation parameter for JPEG-LS
 */
class DJLSRepresentationParameter: public DcmRepresentationParameter
{
public:

  /** constructor
   *  @param nearlosslessDeviation used as parameter NEAR in JPEG-LS nearlossless-encoding process
   *  @param losslessProcess true if lossless process is requested
   */
  DJLSRepresentationParameter(
    Uint16 nearlosslessDeviation,
    OFBool losslessProcess);

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

/*
 * CVS/RCS Log:
 * $Log: djrparam.h,v $
 * Revision 1.4  2010-10-14 13:17:19  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2010-03-01 10:35:28  uli
 * Renamed include guards to avoid name clash with e.g. dcmjpeg.
 *
 * Revision 1.2  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.1  2009-07-29 14:46:46  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.1  2007-06-15 14:35:45  meichel
 * Renamed CMake project and include directory from dcmjpgls to dcmjpls
 *
 * Revision 1.2  2007/06/13 16:41:07  meichel
 * Code clean-up and removal of dead code
 *
 *
 */
