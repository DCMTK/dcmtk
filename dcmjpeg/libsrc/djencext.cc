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
 *  Purpose: Codec class for encoding JPEG Extended Sequential (lossy, 8/12-bit)
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djencext.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djrploss.h"
#include "dcmtk/dcmjpeg/djeijg8.h"
#include "dcmtk/dcmjpeg/djeijg12.h"


DJEncoderExtended::DJEncoderExtended()
: DJCodecEncoder()
{
}


DJEncoderExtended::~DJEncoderExtended()
{
}


E_TransferSyntax DJEncoderExtended::supportedTransferSyntax() const
{
  return EXS_JPEGProcess2_4;
}


OFBool DJEncoderExtended::isLosslessProcess() const
{
  return OFFalse;
}

void DJEncoderExtended::createDerivationDescription(
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter * /* cp */ ,
  Uint8 bitsPerSample,
  double ratio,
  OFString& derivationDescription) const
{
  DJ_RPLossy defaultRP;
  const DJ_RPLossy *rp = toRepParam ? OFreinterpret_cast(const DJ_RPLossy*, toRepParam) : &defaultRP ;
  char buf[64];
 
  derivationDescription =  "Lossy compression with JPEG extended sequential ";
  if (bitsPerSample > 8) derivationDescription += "12 bit"; else derivationDescription += "8 bit";
  derivationDescription += ", IJG quality factor ";
  sprintf(buf, "%u", rp->getQuality());
  derivationDescription += buf;
  derivationDescription += ", compression ratio ";
  appendCompressionRatio(derivationDescription, ratio);
}


DJEncoder *DJEncoderExtended::createEncoderInstance(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample) const
{
  DJ_RPLossy defaultRP;
  const DJ_RPLossy *rp = toRepParam ? OFreinterpret_cast(const DJ_RPLossy*, toRepParam) : &defaultRP ;
  DJEncoder *result = NULL;
  if (bitsPerSample > 8) 
    result = new DJCompressIJG12Bit(*cp, EJM_sequential, OFstatic_cast(Uint8, rp->getQuality()));
  else
    result = new DJCompressIJG8Bit(*cp, EJM_sequential, OFstatic_cast(Uint8, rp->getQuality()));
  return result;
}
