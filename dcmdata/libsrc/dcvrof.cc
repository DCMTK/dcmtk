/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Implementation of class DcmOtherFloat
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrof.h"
#include "dcmtk/dcmdata/dcvrfl.h"


// ********************************


DcmOtherFloat::DcmOtherFloat(const DcmTag &tag,
                             const Uint32 len)
  : DcmFloatingPointSingle(tag, len)
{
}


DcmOtherFloat::DcmOtherFloat(const DcmOtherFloat &old)
  : DcmFloatingPointSingle(old)
{
}


DcmOtherFloat::~DcmOtherFloat()
{
}


DcmOtherFloat &DcmOtherFloat::operator=(const DcmOtherFloat &obj)
{
    DcmFloatingPointSingle::operator=(obj);
    return *this;
}


OFCondition DcmOtherFloat::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmOtherFloat &, rhs);
  }
  return EC_Normal;
}

// ********************************


DcmEVR DcmOtherFloat::ident() const
{
    return EVR_OF;
}


OFCondition DcmOtherFloat::checkValue(const OFString & /*vm*/,
                                      const OFBool /*oldFormat*/)
{
    /* currently no checks are performed */
    return EC_Normal;
}


unsigned long DcmOtherFloat::getVM()
{
    /* value multiplicity for OF is defined as 1 */
    return 1;
}


/*
 * CVS/RCS Log:
 * $Log: dcvrof.cc,v $
 * Revision 1.7  2010-10-20 16:44:17  joergr
 * Use type cast macros (e.g. OFstatic_cast) where appropriate.
 *
 * Revision 1.6  2010-10-14 13:14:10  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2010-04-23 14:30:34  joergr
 * Added new method to all VR classes which checks whether the stored value
 * conforms to the VR definition and to the specified VM.
 *
 * Revision 1.4  2009-11-04 09:58:11  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2008-07-17 10:31:32  onken
 * Implemented copyFrom() method for complete DcmObject class hierarchy, which
 * permits setting an instance's value from an existing object. Implemented
 * assignment operator where necessary.
 *
 * Revision 1.2  2005-12-08 15:41:58  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/12/06 12:07:02  joergr
 * Added support for new value representation Other Float String (OF).
 *
 *
 */
