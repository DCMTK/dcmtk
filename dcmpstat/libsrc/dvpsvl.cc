/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSVOILUT
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-22 17:57:20 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsvl.h"

/* --------------- a few macros avoiding copy/paste --------------- */

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}


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

E_Condition DVPSVOILUT::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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

void DVPSVOILUT::assign(DcmUnsignedShort& descriptor, DcmUnsignedShort& data, DcmLongString& explanation)
{
  descriptor = voiLUTDescriptor;
  data = voiLUTData;
  explanation = voiLUTExplanation;
  return;
}

/*
 *  $Log: dvpsvl.cc,v $
 *  Revision 1.1  1998-12-22 17:57:20  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */

