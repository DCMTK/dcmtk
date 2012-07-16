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
 *  Purpose: singleton class that registers RLE decoder.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrledrg.h"

#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmdata/dcrleccd.h" /* for class DcmRLECodecDecoder */
#include "dcmtk/dcmdata/dcrlecp.h"  /* for class DcmRLECodecParameter */

// initialization of static members
OFBool DcmRLEDecoderRegistration::registered                  = OFFalse;
DcmRLECodecParameter *DcmRLEDecoderRegistration::cp           = NULL;
DcmRLECodecDecoder *DcmRLEDecoderRegistration::codec          = NULL;

void DcmRLEDecoderRegistration::registerCodecs(
    OFBool pCreateSOPInstanceUID,
    OFBool pReverseDecompressionByteOrder)
{
  if (! registered)
  {
    cp = new DcmRLECodecParameter(
      pCreateSOPInstanceUID,
      0, OFTrue, OFFalse,
      pReverseDecompressionByteOrder);
      
    if (cp)
    {
      codec = new DcmRLECodecDecoder();
      if (codec) DcmCodecList::registerCodec(codec, NULL, cp);
      registered = OFTrue;
    }
  }
}

void DcmRLEDecoderRegistration::cleanup()
{
  if (registered)
  {
    DcmCodecList::deregisterCodec(codec);
    delete codec;
    delete cp;
    registered = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    codec  = NULL;
    cp     = NULL;
#endif
  }
}
