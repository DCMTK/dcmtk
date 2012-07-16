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
 *  Purpose: singleton class that registers RLE encoder.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrleerg.h"

#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmdata/dcrlecce.h" /* for class DcmRLECodecEncoder */
#include "dcmtk/dcmdata/dcrlecp.h"  /* for class DcmRLECodecParameter */

// initialization of static members
OFBool DcmRLEEncoderRegistration::registered                  = OFFalse;
DcmRLECodecParameter *DcmRLEEncoderRegistration::cp           = NULL;
DcmRLECodecEncoder *DcmRLEEncoderRegistration::codec          = NULL;

void DcmRLEEncoderRegistration::registerCodecs(
    OFBool pCreateSOPInstanceUID,
    Uint32 pFragmentSize,
    OFBool pCreateOffsetTable,
    OFBool pConvertToSC)
{
  if (! registered)
  {
    cp = new DcmRLECodecParameter(
      pCreateSOPInstanceUID,
      pFragmentSize,
      pCreateOffsetTable,
      pConvertToSC);

    if (cp)
    {
      codec = new DcmRLECodecEncoder();
      if (codec) DcmCodecList::registerCodec(codec, NULL, cp);
      registered = OFTrue;
    }
  }
}

void DcmRLEEncoderRegistration::cleanup()
{
  if (registered)
  {
    DcmCodecList::deregisterCodec(codec);
    delete codec;
    delete cp;
    registered = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    codec = NULL;
    cp     = NULL;
#endif

  }
}
