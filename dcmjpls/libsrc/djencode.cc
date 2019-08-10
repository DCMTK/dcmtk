/*
 *
 *  Copyright (C) 1997-2019, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm, Uli Schlachter
 *
 *  Purpose: singleton class that registers encoders for all supported JPEG processes.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djencode.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmjpls/djcparam.h"
#include "dcmtk/dcmjpls/djcodece.h"

// initialization of static members
OFBool DJLSEncoderRegistration::registered_                             = OFFalse;
DJLSCodecParameter *DJLSEncoderRegistration::cp_                        = NULL;
DJLSLosslessEncoder *DJLSEncoderRegistration::losslessencoder_          = NULL;
DJLSNearLosslessEncoder *DJLSEncoderRegistration::nearlosslessencoder_  = NULL;


void DJLSEncoderRegistration::registerCodecs(
    Uint16 jpls_t1,
    Uint16 jpls_t2,
    Uint16 jpls_t3,
    Uint16 jpls_reset,
    OFBool preferCookedEncoding,
    Uint32 fragmentSize,
    OFBool createOffsetTable,
    JLS_UIDCreation uidCreation,
    OFBool convertToSC,
    DJLSCodecParameter::interleaveMode jplsInterleaveMode,
    OFBool useFFbitstreamPadding)
{
  if (! registered_)
  {
    cp_ = new DJLSCodecParameter(preferCookedEncoding, jpls_t1, jpls_t2, jpls_t3,
      jpls_reset, fragmentSize, createOffsetTable, uidCreation,
      convertToSC, EJLSPC_restore, OFFalse, jplsInterleaveMode, useFFbitstreamPadding);

    if (cp_)
    {
      losslessencoder_ = new DJLSLosslessEncoder();
      if (losslessencoder_) DcmCodecList::registerCodec(losslessencoder_, NULL, cp_);
      nearlosslessencoder_ = new DJLSNearLosslessEncoder();
      if (nearlosslessencoder_) DcmCodecList::registerCodec(nearlosslessencoder_, NULL, cp_);
      registered_ = OFTrue;
    }
  }
}

void DJLSEncoderRegistration::cleanup()
{
  if (registered_)
  {
    DcmCodecList::deregisterCodec(losslessencoder_);
    DcmCodecList::deregisterCodec(nearlosslessencoder_);
    delete losslessencoder_;
    delete nearlosslessencoder_;
    delete cp_;
    registered_ = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    losslessencoder_ = NULL;
    nearlosslessencoder_ = NULL;
    cp_     = NULL;
#endif
  }
}

OFString DJLSEncoderRegistration::getLibraryVersionString()
{
    return DCMJPLS_CHARLS_VERSION_STRING;
}
