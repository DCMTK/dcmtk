/*
 *
 *  Copyright (C) 1997-2014, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djrploss.h"


DJ_RPLossy::DJ_RPLossy(int aQuality)
: DcmRepresentationParameter()
, quality(aQuality)
{
}

DJ_RPLossy::DJ_RPLossy(const DJ_RPLossy& arg)
: DcmRepresentationParameter(arg)
, quality(arg.quality)
{
}

DJ_RPLossy::~DJ_RPLossy()
{
}  

DcmRepresentationParameter *DJ_RPLossy::clone() const
{
  return new DJ_RPLossy(*this);
}

const char *DJ_RPLossy::className() const
{
  return "DJ_RPLossy";
}

OFBool DJ_RPLossy::operator==(const DcmRepresentationParameter &arg) const
{
  const char *argname = arg.className();
  if (argname)
  {
    OFString argstring(argname);
    if (argstring == className())
    {
      const DJ_RPLossy& argll = OFstatic_cast(const DJ_RPLossy&, arg);
      if (quality == argll.quality) return OFTrue;
    }
  }
  return OFFalse;
}
