/*
 *
 *  Copyright (C) 1997-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: representation parameter for lossy JPEG
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:58:34 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djrploss.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djrploss.h"


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
      const DJ_RPLossy& argll = (const DJ_RPLossy &)arg;
      if (quality == argll.quality) return OFTrue;
    }   
  }
  return OFFalse;
}


/*
 * CVS/RCS Log
 * $Log: djrploss.cc,v $
 * Revision 1.1  2001-11-13 15:58:34  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
