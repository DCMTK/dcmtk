/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrlerp.h"


DcmRLERepresentationParameter::DcmRLERepresentationParameter()
: DcmRepresentationParameter()
{
}

DcmRLERepresentationParameter::DcmRLERepresentationParameter(const DcmRLERepresentationParameter& arg)
: DcmRepresentationParameter(arg)
{
}

DcmRLERepresentationParameter::~DcmRLERepresentationParameter()
{
}  

DcmRepresentationParameter *DcmRLERepresentationParameter::clone() const
{
  return new DcmRLERepresentationParameter(*this);
}

const char *DcmRLERepresentationParameter::className() const
{
  return "DcmRLERepresentationParameter";
}

OFBool DcmRLERepresentationParameter::operator==(const DcmRepresentationParameter &arg) const
{
  const char *argname = arg.className();
  if (argname)
  {
    OFString argstring(argname);
    if (argstring == className()) return OFTrue;
  }
  return OFFalse;
}
