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
 *  Purpose: Codec class for encoding JPEG Extended Sequential (lossy, 8/12-bit)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:58:31 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djencext.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djencext.h"
#include "djcparam.h"
#include "djrploss.h"
#include "djeijg8.h"
#include "djeijg12.h"


DJEncoderExtended::DJEncoderExtended()
: DJCodecEncoder()
{
}


DJEncoderExtended::~DJEncoderExtended()
{
}


E_TransferSyntax DJEncoderExtended::supportedTransferSyntax() const
{
  return EXS_JPEGProcess2_4TransferSyntax;
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
  const DJ_RPLossy *rp = toRepParam ? (const DJ_RPLossy *)toRepParam : &defaultRP ;
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
  const DJ_RPLossy *rp = toRepParam ? (const DJ_RPLossy *)toRepParam : &defaultRP ;
  DJEncoder *result = NULL;
  if (bitsPerSample > 8) 
    result = new DJCompressIJG12Bit(*cp, EJM_sequential, rp->getQuality());
    else result = new DJCompressIJG8Bit(*cp, EJM_sequential, rp->getQuality());
  return result;
}


/*
 * CVS/RCS Log
 * $Log: djencext.cc,v $
 * Revision 1.1  2001-11-13 15:58:31  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
