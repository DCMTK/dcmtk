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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Codec class for encoding JPEG Spectral Selection (lossy, 8/12-bit)
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djencsps.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djrploss.h"
#include "dcmtk/dcmjpeg/djeijg8.h"
#include "dcmtk/dcmjpeg/djeijg12.h"


DJEncoderSpectralSelection::DJEncoderSpectralSelection()
: DJCodecEncoder()
{
}


DJEncoderSpectralSelection::~DJEncoderSpectralSelection()
{
}


E_TransferSyntax DJEncoderSpectralSelection::supportedTransferSyntax() const
{
  return EXS_JPEGProcess6_8;
}


OFBool DJEncoderSpectralSelection::isLosslessProcess() const
{
  return OFFalse;
}


void DJEncoderSpectralSelection::createDerivationDescription(
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter * /* cp */ ,
  Uint8 bitsPerSample,
  double ratio,
  OFString& derivationDescription) const
{
  DJ_RPLossy defaultRP;
  const DJ_RPLossy *rp = toRepParam ? OFreinterpret_cast(const DJ_RPLossy*, toRepParam) : &defaultRP ;
  char buf[64];
 
  derivationDescription =  "Lossy compression with JPEG spectral selection ";
  if (bitsPerSample > 8) derivationDescription += "12 bit"; else derivationDescription += "8 bit";
  derivationDescription += ", IJG quality factor ";
  sprintf(buf, "%u", rp->getQuality());
  derivationDescription += buf;
  derivationDescription += ", compression ratio ";
  appendCompressionRatio(derivationDescription, ratio);
}


DJEncoder *DJEncoderSpectralSelection::createEncoderInstance(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample) const
{
  DJ_RPLossy defaultRP;
  const DJ_RPLossy *rp = toRepParam ? OFreinterpret_cast(const DJ_RPLossy*, toRepParam) : &defaultRP ;
  DJEncoder *result = NULL;

  if (bitsPerSample > 8)
    result = new DJCompressIJG12Bit(*cp, EJM_spectralSelection, OFstatic_cast(Uint8, rp->getQuality()));
    else result = new DJCompressIJG8Bit(*cp, EJM_spectralSelection, OFstatic_cast(Uint8, rp->getQuality()));
  return result;
}
