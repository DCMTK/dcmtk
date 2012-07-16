/*
 *
 *  Copyright (C) 2001-2011, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: Codec class for decoding JPEG Baseline (lossy, 8-bit)
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdecbas.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djrploss.h"

#include "dcmtk/dcmjpeg/djdijg8.h"


DJDecoderBaseline::DJDecoderBaseline()
: DJCodecDecoder()
{
}


DJDecoderBaseline::~DJDecoderBaseline()
{
}


E_TransferSyntax DJDecoderBaseline::supportedTransferSyntax() const
{
  return EXS_JPEGProcess1;
}


DJDecoder *DJDecoderBaseline::createDecoderInstance(
    const DcmRepresentationParameter * /* toRepParam */,
    const DJCodecParameter *cp,
    Uint8 /* bitsPerSample */,
    OFBool isYBR) const
{
  return new DJDecompressIJG8Bit(*cp, isYBR);
}
