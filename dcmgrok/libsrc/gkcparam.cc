/*
 *
 *  Copyright (C) 2026, Grok Image Compression Inc.
 *  All rights reserved.
 *
 *  This software and supporting documentation were developed by
 *  Grok Image Compression Inc.
 *
 *  Module:  dcmgrok
 *
 *  Author:  Aaron Boxer
 *
 *  Purpose: codec parameter class for Grok JPEG 2000 codecs
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmgrok/gkcparam.h"


DGRKCodecParameter::DGRKCodecParameter(
    GRK_UIDCreation uidCreation,
    GRK_PlanarConfiguration planarConfiguration,
    OFBool ignoreOffsetTable,
    Uint32 numThreads)
: DcmCodecParameter()
, uidCreation_(uidCreation)
, planarConfiguration_(planarConfiguration)
, ignoreOffsetTable_(ignoreOffsetTable)
, numThreads_(numThreads)
{
}


DGRKCodecParameter::DGRKCodecParameter(const DGRKCodecParameter& arg)
: DcmCodecParameter(arg)
, uidCreation_(arg.uidCreation_)
, planarConfiguration_(arg.planarConfiguration_)
, ignoreOffsetTable_(arg.ignoreOffsetTable_)
, numThreads_(arg.numThreads_)
{
}


DGRKCodecParameter::~DGRKCodecParameter()
{
}


DcmCodecParameter *DGRKCodecParameter::clone() const
{
    return new DGRKCodecParameter(*this);
}


const char *DGRKCodecParameter::className() const
{
    return "DGRKCodecParameter";
}
