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
 *  Purpose: representation parameter for JPEG-LS
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:24 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djrparam.h"
#include "dcmtk/ofstd/ofstd.h"

DJLSRepresentationParameter::DJLSRepresentationParameter(
    Uint16 nearlosslessDeviation,
    OFBool losslessProcess)
: DcmRepresentationParameter()
, nearlosslessDeviation_(nearlosslessDeviation)
, losslessProcess_(losslessProcess)
{
}

DJLSRepresentationParameter::DJLSRepresentationParameter(const DJLSRepresentationParameter& arg)
: DcmRepresentationParameter(arg)
, nearlosslessDeviation_(arg.nearlosslessDeviation_)
, losslessProcess_(arg.losslessProcess_)
{
}

DJLSRepresentationParameter::~DJLSRepresentationParameter()
{
}  

DcmRepresentationParameter *DJLSRepresentationParameter::clone() const
{
  return new DJLSRepresentationParameter(*this);
}

const char *DJLSRepresentationParameter::className() const
{
  return "DJLSRepresentationParameter";
}

OFBool DJLSRepresentationParameter::operator==(const DcmRepresentationParameter &arg) const
{
  const char *argname = arg.className();
  if (argname)
  {
    OFString argstring(argname);
    if (argstring == className())
    {
      const DJLSRepresentationParameter& argll = OFreinterpret_cast(const DJLSRepresentationParameter &, arg);
      if (losslessProcess_ && argll.losslessProcess_) return OFTrue;
      else if (losslessProcess_ != argll.losslessProcess_) return OFFalse;
	  else if (nearlosslessDeviation_ != argll.nearlosslessDeviation_) return OFFalse;
      return OFTrue;
    }	
  }
  return OFFalse;
}

/*
 * CVS/RCS Log:
 * $Log: djrparam.cc,v $
 * Revision 1.3  2010-10-14 13:14:24  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.2  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.1  2009-07-29 14:46:48  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.3  2007-06-15 14:35:45  meichel
 * Renamed CMake project and include directory from dcmjpgls to dcmjpls
 *
 * Revision 1.2  2007/06/13 16:41:07  meichel
 * Code clean-up and removal of dead code
 *
 *
 */
