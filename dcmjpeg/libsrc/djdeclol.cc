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
 *  Purpose: Codec class for decoding JPEG Lossless (8/12/16-bit)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:58:26 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djdeclol.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djdeclol.h"
#include "djcparam.h"
#include "djrplol.h"
#include "djdijg8.h"
#include "djdijg12.h"
#include "djdijg16.h"

DJDecoderLossless::DJDecoderLossless()
: DJCodecDecoder()
{
}


DJDecoderLossless::~DJDecoderLossless()
{
}


E_TransferSyntax DJDecoderLossless::supportedTransferSyntax() const
{
  return EXS_JPEGProcess14TransferSyntax;
}


DJDecoder *DJDecoderLossless::createDecoderInstance(
    const DcmRepresentationParameter * /* toRepParam */,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample,
    OFBool isYBR) const
{
  if (bitsPerSample > 12) return new DJDecompressIJG16Bit(*cp, isYBR);
  else if (bitsPerSample > 8) return new DJDecompressIJG12Bit(*cp, isYBR);
  else return new DJDecompressIJG8Bit(*cp, isYBR);
}


/*
 * CVS/RCS Log
 * $Log: djdeclol.cc,v $
 * Revision 1.1  2001-11-13 15:58:26  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
