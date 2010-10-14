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
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: representation parameter for lossless JPEG
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:22 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djrplol.h"


DJ_RPLossless::DJ_RPLossless(int aPrediction, int aPt)
: DcmRepresentationParameter()
, prediction(aPrediction)
, pt(aPt)
{
}

DJ_RPLossless::DJ_RPLossless(const DJ_RPLossless& arg)
: DcmRepresentationParameter(arg)
, prediction(arg.prediction)
, pt(arg.pt)
{
}

DJ_RPLossless::~DJ_RPLossless()
{
}  

DcmRepresentationParameter *DJ_RPLossless::clone() const
{
  return new DJ_RPLossless(*this);
}

const char *DJ_RPLossless::className() const
{
  return "DJ_RPLossless";
}

OFBool DJ_RPLossless::operator==(const DcmRepresentationParameter &arg) const
{
  const char *argname = arg.className();
  if (argname)
  {
    OFString argstring(argname);
    if (argstring == className())
    {
      const DJ_RPLossless& argll = (const DJ_RPLossless &)arg;
      if ((prediction == argll.prediction) && (pt == argll.pt)) return OFTrue;
    }   
  }
  return OFFalse;
}


/*
 * CVS/RCS Log
 * $Log: djrplol.cc,v $
 * Revision 1.3  2010-10-14 13:14:22  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.2  2005-12-08 15:43:49  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2001/11/13 15:58:34  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
