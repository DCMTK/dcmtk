/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
 *  Purpose: codec parameter class for JPEG-LS codecs
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djcparam.h"
#include "dcmtk/ofstd/ofstd.h"

DJLSCodecParameter::DJLSCodecParameter(
     OFBool jpls_optionsEnabled,
     Uint16 jpls_t1, // these are the defaults for 8bpp in lossless mode
     Uint16 jpls_t2,
     Uint16 jpls_t3,
     Uint16 jpls_reset,
     Uint16 jpls_limit,
     OFBool preferCookedEncoding,
     Uint32 fragmentSize,
     OFBool createOffsetTable,
     JLS_UIDCreation uidCreation,
     OFBool convertToSC,
     JLS_PlanarConfiguration planarConfiguration,
     OFBool ignoreOffsetTble,
     interleaveMode jplsInterleaveMode)
: DcmCodecParameter()
, jpls_optionsEnabled_(jpls_optionsEnabled)
, jpls_t1_(jpls_t1)
, jpls_t2_(jpls_t2)
, jpls_t3_(jpls_t3)
, jpls_reset_(jpls_reset)
, jpls_limit_(jpls_limit)
, fragmentSize_(fragmentSize)
, createOffsetTable_(createOffsetTable)
, preferCookedEncoding_(preferCookedEncoding)
, uidCreation_(uidCreation)
, convertToSC_(convertToSC)
, jplsInterleaveMode_(jplsInterleaveMode)
, planarConfiguration_(planarConfiguration)
, ignoreOffsetTable_(ignoreOffsetTble)
{
}


DJLSCodecParameter::DJLSCodecParameter(
    JLS_UIDCreation uidCreation,
    JLS_PlanarConfiguration planarConfiguration,
    OFBool ignoreOffsetTble)
: DcmCodecParameter()
, jpls_optionsEnabled_(OFFalse)
, jpls_t1_(3)
, jpls_t2_(7)
, jpls_t3_(21)
, jpls_reset_(64)
, jpls_limit_(0)
, fragmentSize_(0)
, createOffsetTable_(OFTrue)
, preferCookedEncoding_(OFTrue)
, uidCreation_(uidCreation)
, convertToSC_(OFFalse)
, jplsInterleaveMode_(interleaveDefault)
, planarConfiguration_(planarConfiguration)
, ignoreOffsetTable_(ignoreOffsetTble)
{
}

DJLSCodecParameter::DJLSCodecParameter(const DJLSCodecParameter& arg)
: DcmCodecParameter(arg)

, jpls_optionsEnabled_(arg.jpls_optionsEnabled_)
, jpls_t1_(arg.jpls_t1_)
, jpls_t2_(arg.jpls_t2_)
, jpls_t3_(arg.jpls_t3_)
, jpls_reset_(arg.jpls_reset_)
, jpls_limit_(arg.jpls_limit_)
, fragmentSize_(arg.fragmentSize_)
, createOffsetTable_(arg.createOffsetTable_)
, preferCookedEncoding_(arg.preferCookedEncoding_)
, uidCreation_(arg.uidCreation_)
, convertToSC_(arg.convertToSC_)
, jplsInterleaveMode_(arg.jplsInterleaveMode_)
, planarConfiguration_(arg.planarConfiguration_)
, ignoreOffsetTable_(arg.ignoreOffsetTable_)
{
}

DJLSCodecParameter::~DJLSCodecParameter()
{
}

DcmCodecParameter *DJLSCodecParameter::clone() const
{
  return new DJLSCodecParameter(*this);
}

const char *DJLSCodecParameter::className() const
{
  return "DJLSCodecParameter";
}
