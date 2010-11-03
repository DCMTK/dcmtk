/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Purpose: Codec class for encoding JPEG Lossless (8/12/16-bit)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-03 11:22:38 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djenclol.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djrplol.h"
#include "dcmtk/dcmjpeg/djeijg8.h"
#include "dcmtk/dcmjpeg/djeijg12.h"
#include "dcmtk/dcmjpeg/djeijg16.h"


DJEncoderLossless::DJEncoderLossless()
: DJCodecEncoder()
{
}


DJEncoderLossless::~DJEncoderLossless()
{
}


E_TransferSyntax DJEncoderLossless::supportedTransferSyntax() const
{
  return EXS_JPEGProcess14TransferSyntax;
}


OFBool DJEncoderLossless::isLosslessProcess() const
{
  return OFTrue;
}


void DJEncoderLossless::createDerivationDescription(
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter *cp,
  Uint8 /* bitsPerSample */ ,
  double ratio,
  OFString& derivationDescription) const
{
  DJ_RPLossless defaultRP;
  const DJ_RPLossless *rp = toRepParam ? (const DJ_RPLossless *)toRepParam : &defaultRP ;
  char buf[64];

  if (cp->getTrueLosslessMode())
    derivationDescription = "Lossless JPEG compression, selection value ";
  else
    derivationDescription = "Pseudo-Lossless JPEG compression, selection value ";
  sprintf(buf, "%u", rp->getPrediction());
  derivationDescription += buf;
  derivationDescription += ", point transform ";
  sprintf(buf, "%u", rp->getPointTransformation());
  derivationDescription += buf;
  derivationDescription += ", compression ratio ";
  appendCompressionRatio(derivationDescription, ratio);
}


DJEncoder *DJEncoderLossless::createEncoderInstance(
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter *cp,
  Uint8 bitsPerSample) const
{
  DJ_RPLossless defaultRP;
  const DJ_RPLossless *rp = toRepParam ? (const DJ_RPLossless *)toRepParam : &defaultRP ;
  DJEncoder *result = NULL;
  if (bitsPerSample > 12)
    result = new DJCompressIJG16Bit(*cp, EJM_lossless, rp->getPrediction(), rp->getPointTransformation());
  else if (bitsPerSample > 8)
    result = new DJCompressIJG12Bit(*cp, EJM_lossless, rp->getPrediction(), rp->getPointTransformation());
  else
    result = new DJCompressIJG8Bit(*cp, EJM_lossless, rp->getPrediction(), rp->getPointTransformation());
  return result;
}


/*
 * CVS/RCS Log
 * $Log: djenclol.cc,v $
 * Revision 1.4  2010-11-03 11:22:38  joergr
 * Since the pseudo-lossless encoder is not guaranteed to result in lossless
 * compression, the modifications to the DICOM header are treated in the same
 * way as for lossy compression (e.g Lossy Compression Flag is set to "01").
 *
 * Revision 1.3  2010-10-14 13:14:22  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.2  2005-12-08 15:43:44  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2001/11/13 15:58:32  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
