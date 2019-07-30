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
 *  Author:  Martin Willkomm
 *
 *  Purpose: singleton class that registers decoders for all supported JPEG-LS processes.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djdecode.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmjpls/djcparam.h"
#include "dcmtk/dcmjpls/djcodecd.h"

// initialization of static members
OFBool DJLSDecoderRegistration::registered_                            = OFFalse;
DJLSCodecParameter *DJLSDecoderRegistration::cp_                       = NULL;
DJLSLosslessDecoder *DJLSDecoderRegistration::losslessdecoder_         = NULL;
DJLSNearLosslessDecoder *DJLSDecoderRegistration::nearlosslessdecoder_ = NULL;

void DJLSDecoderRegistration::registerCodecs(
    JLS_UIDCreation uidcreation,
    JLS_PlanarConfiguration planarconfig,
    OFBool ignoreOffsetTable,
    OFBool forceSingleFragmentPerFrame)
{
  if (! registered_)
  {
    cp_ = new DJLSCodecParameter(uidcreation, planarconfig, ignoreOffsetTable, forceSingleFragmentPerFrame);
    if (cp_)
    {
      losslessdecoder_ = new DJLSLosslessDecoder();
      if (losslessdecoder_) DcmCodecList::registerCodec(losslessdecoder_, NULL, cp_);

      nearlosslessdecoder_ = new DJLSNearLosslessDecoder();
      if (nearlosslessdecoder_) DcmCodecList::registerCodec(nearlosslessdecoder_, NULL, cp_);
      registered_ = OFTrue;
    }
  }
}

void DJLSDecoderRegistration::cleanup()
{
  if (registered_)
  {
    DcmCodecList::deregisterCodec(losslessdecoder_);
    DcmCodecList::deregisterCodec(nearlosslessdecoder_);
    delete losslessdecoder_;
    delete nearlosslessdecoder_;
    delete cp_;
    registered_ = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    losslessdecoder_ = NULL;
    nearlosslessdecoder_ = NULL;
    cp_      = NULL;
#endif
  }
}

OFString DJLSDecoderRegistration::getLibraryVersionString()
{
    return DCMJPLS_CHARLS_VERSION_STRING;
}
