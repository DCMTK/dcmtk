/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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

  // LUTDescriptor can be US or SS
  if ((EC_Normal == dset.search((DcmTagKey &)voiLUTDescriptor.getTag(),
    stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_SS))
  {
    // We explicitly use DcmElement::operator=(), which works for US and SS
    DcmElement *vLUTDescriptor = &voiLUTDescriptor;
    vLUTDescriptor->operator=(* OFstatic_cast(DcmElement *, stack.top()));
  }

  READ_FROM_DATASET(DcmLongString, EVR_LO, voiLUTExplanation)

  stack.clear();
  if ((EC_Normal == dset.search((DcmTagKey &)voiLUTData.getTag(), stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_OW))
  {
    // we deliberately call DcmElement::operator=() here, which will work for both DcmUnsignedShort and DcmOtherByteOtherWord parameters
    DcmElement *vldata = &voiLUTData;
    vldata->operator=(*(DcmElement *)(stack.top()));
  }

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
