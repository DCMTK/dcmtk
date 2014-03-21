/*
 *
 *  Copyright (C) 2001-2014, OFFIS e.V.
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
 *  Purpose: Codec class for encoding JPEG Lossless Selection Value 1 (8/12/16-bit)
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djencsv1.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djrplol.h"
#include "dcmtk/dcmjpeg/djeijg8.h"
#include "dcmtk/dcmjpeg/djeijg12.h"
#include "dcmtk/dcmjpeg/djeijg16.h"


DJEncoderP14SV1::DJEncoderP14SV1()
: DJCodecEncoder()
{
}


DJEncoderP14SV1::~DJEncoderP14SV1()
{
}


E_TransferSyntax DJEncoderP14SV1::supportedTransferSyntax() const
{
  return EXS_JPEGProcess14SV1;
}


OFBool DJEncoderP14SV1::isLosslessProcess() const
{
  return OFTrue;
}


void DJEncoderP14SV1::createDerivationDescription(
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter *cp,
  Uint8 /* bitsPerSample */,
  double ratio,
  OFString& derivationDescription) const
{
  DJ_RPLossless defaultRP;
  const DJ_RPLossless *rp = toRepParam ? OFreinterpret_cast(const DJ_RPLossless*, toRepParam) : &defaultRP ;
  char buf[64];

  if (cp->getTrueLosslessMode())
    derivationDescription = "Lossless JPEG compression, selection value 1, point transform ";
  else
    derivationDescription = "Pseudo-Lossless JPEG compression, selection value 1, point transform ";
  sprintf(buf, "%u", rp->getPointTransformation());
  derivationDescription += buf;
  derivationDescription += ", compression ratio ";
  appendCompressionRatio(derivationDescription, ratio);
}


DJEncoder *DJEncoderP14SV1::createEncoderInstance(
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter *cp,
  Uint8 bitsPerSample) const
{
  DJ_RPLossless defaultRP;
  const DJ_RPLossless *rp = toRepParam ? OFreinterpret_cast(const DJ_RPLossless*, toRepParam) : &defaultRP ;
  DJEncoder *result = NULL;
  // prediction/selection value is always 1 for this transfer syntax
  if (bitsPerSample > 12)
    result = new DJCompressIJG16Bit(*cp, EJM_lossless, 1, rp->getPointTransformation());
  else if (bitsPerSample > 8)
    result = new DJCompressIJG12Bit(*cp, EJM_lossless, 1, rp->getPointTransformation());
  else
    result = new DJCompressIJG8Bit(*cp, EJM_lossless, 1, rp->getPointTransformation());
  return result;
}
