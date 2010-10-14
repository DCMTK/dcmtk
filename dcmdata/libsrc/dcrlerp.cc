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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*
 * CVS/RCS Log
 * $Log: dcrlerp.cc,v $
 * Revision 1.4  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.2  2005-12-08 15:41:35  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/06/06 14:52:43  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
