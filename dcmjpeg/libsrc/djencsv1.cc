/*
 *
 *  Copyright (C) 1997-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: Codec class for encoding JPEG Lossless Selection Value 1 (8/12/16-bit)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:58:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djencsv1.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djencsv1.h"
#include "djcparam.h"
#include "djrplol.h"
#include "djeijg8.h"
#include "djeijg12.h"
#include "djeijg16.h"


DJEncoderP14SV1::DJEncoderP14SV1()
: DJCodecEncoder()
{
}


DJEncoderP14SV1::~DJEncoderP14SV1()
{
}


E_TransferSyntax DJEncoderP14SV1::supportedTransferSyntax() const
{
  return EXS_JPEGProcess14SV1TransferSyntax;
}


OFBool DJEncoderP14SV1::isLosslessProcess() const
{
  return OFTrue;
}


void DJEncoderP14SV1::createDerivationDescription(
  const DcmRepresentationParameter * toRepParam,
  const DJCodecParameter * /* cp */ ,
  Uint8 /* bitsPerSample */,
  double ratio,
  OFString& derivationDescription) const
{
  DJ_RPLossless defaultRP;
  const DJ_RPLossless *rp = toRepParam ? (const DJ_RPLossless *)toRepParam : &defaultRP ;
  char buf[64];
 
  derivationDescription =  "Lossless JPEG compression, selection value 1, point transform ";
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
  const DJ_RPLossless *rp = toRepParam ? (const DJ_RPLossless *)toRepParam : &defaultRP ;
  DJEncoder * result = NULL;
  // prediction/selection value is always 1 for this transfer syntax
  if (bitsPerSample > 12)
    result = new DJCompressIJG16Bit(*cp, EJM_lossless, 1, rp->getPointTransformation());
  else if (bitsPerSample > 8)
    result = new DJCompressIJG12Bit(*cp, EJM_lossless, 1, rp->getPointTransformation());
  else 
    result = new DJCompressIJG8Bit(*cp, EJM_lossless, 1, rp->getPointTransformation());  
  return result;
}


/*
 * CVS/RCS Log
 * $Log: djencsv1.cc,v $
 * Revision 1.1  2001-11-13 15:58:33  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
