/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSVOILUT
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsvl.h"
#include "dcmtk/dcmpstat/dvpssv.h"      /* for DVPSSoftcopyVOI */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */


/* --------------- class DVPSVOILUT --------------- */

DVPSVOILUT::DVPSVOILUT()
: voiLUTDescriptor(DCM_LUTDescriptor)
, voiLUTExplanation(DCM_LUTExplanation)
, voiLUTData(DCM_LUTData)
{
}

DVPSVOILUT::DVPSVOILUT(const DVPSVOILUT& copy)
: voiLUTDescriptor(copy.voiLUTDescriptor)
, voiLUTExplanation(copy.voiLUTExplanation)
, voiLUTData(copy.voiLUTData)
{
}

DVPSVOILUT::~DVPSVOILUT()
{
}

void DVPSVOILUT::clear()
{
  voiLUTDescriptor.clear();
  voiLUTExplanation.clear();
  voiLUTData.clear();
}

OFCondition DVPSVOILUT::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmUnsignedShort, voiLUTDescriptor)
  READ_FROM_DATASET(DcmLongString, voiLUTExplanation)
  READ_FROM_DATASET(DcmUnsignedShort, voiLUTData)

  if (EC_Normal == result)
  {
    if (voiLUTData.getLength() == 0) result=EC_IllegalCall;
    if (voiLUTDescriptor.getVM() != 3) result=EC_IllegalCall;
  }
  if ((EC_Normal==result) && (voiLUTExplanation.getLength() == 0))
  {
    Uint16 numEntries=0, bits=0;
    voiLUTDescriptor.getUint16(numEntries,0);
    voiLUTDescriptor.getUint16(bits,2);
    char descr[100];
    sprintf(descr, "VOI LUT entries=%u bits=%u", (unsigned int)numEntries, (unsigned int)bits);
    voiLUTExplanation.putString(descr);
  }
  return result;
}

const char *DVPSVOILUT::getExplanation()
{
  char *value = NULL;
  if (EC_Normal != voiLUTExplanation.getString(value)) return NULL;
  return value;
}

OFCondition DVPSVOILUT::assign(DVPSSoftcopyVOI& voi)
{
  return voi.setVOILUT(voiLUTDescriptor, voiLUTData, voiLUTExplanation);
}

/*
 *  $Log: dvpsvl.cc,v $
 *  Revision 1.11  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.10  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.9  2005-12-08 15:46:55  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2003/06/04 12:30:29  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.7  2001/09/26 15:36:35  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:50:41  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/06/02 16:01:10  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.4  2000/05/31 13:02:41  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.3  2000/03/08 16:29:13  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  1999/07/22 16:40:06  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.1  1998/12/22 17:57:20  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */

