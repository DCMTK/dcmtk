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
 *  Update Date:      $Date: 2001-11-13 15:56:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/include/Attic/djdeclol.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJDECLOL_H
#define DJDECLOL_H

#include "osconfig.h"
#include "djcodecd.h" /* for class DJCodecDecoder */


/** Decoder class for JPEG Lossless (8/12/16-bit)
 */
class DJDecoderLossless : public DJCodecDecoder
{
public: 

  /// default constructor
  DJDecoderLossless();

  /// destructor
  virtual ~DJDecoderLossless();

  /** returns the transfer syntax that this particular codec
   *  is able to encode and decode.
   *  @return supported transfer syntax
   */
  virtual E_TransferSyntax supportedTransferSyntax() const;

private:

  /** creates an instance of the compression library to be used for decoding.
   *  @param toRepParam representation parameter passed to decode()
   *  @param cp codec parameter passed to decode()
   *  @param bitsPerSample bits per sample for the image data
   *  @param isYBR flag indicating whether DICOM photometric interpretation is YCbCr
   *  @return pointer to newly allocated decoder object
   */
  virtual DJDecoder *createDecoderInstance(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample,
    OFBool isYBR) const;
    
};

#endif

/*
 * CVS/RCS Log
 * $Log: djdeclol.h,v $
 * Revision 1.1  2001-11-13 15:56:19  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
