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
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: abstract base class for decompression classes
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2005-11-30 14:12:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/include/Attic/djdecabs.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJDECABS_H
#define DJDECABS_H

#include "osconfig.h"
#include "dctypes.h"
#include "djutils.h" /* for enums */
#include "diutils.h" /* for EP_Interpretation */

/** abstract base class for decompression classes.
 *  Implementations of this class must support suspended decompression
 *  in which compressed data for one frame is fed block by block
 *  into the decompression routine, see description below.
 */
class DJDecoder
{
public:

  /// default constructor
  DJDecoder()
  {
  }

  /// destructor
  virtual ~DJDecoder()
  {
  }

  /** initializes internal object structures.
   *  Must be called before a new frame is decompressed.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition init() = 0;

  /** suspended decompression routine. Decompresses a JPEG frame
   *  until finished or out of data. Can be called with new data
   *  until a frame is complete.
   *  @param compressedFrameBuffer pointer to compressed input data, must not be NULL
   *  @param compressedFrameBufferSize size of buffer, in bytes
   *  @param uncompressedFrameBuffer pointer to uncompressed output data, must not be NULL.
   *     This buffer must not change between multiple decode() calls for a single frame.
   *  @param uncompressedFrameBufferSize size of buffer, in words???
   *     Buffer must be large enough to contain a complete frame.
   *  @param isSigned OFTrue, if uncompressed pixel data is signed, OFFalse otherwise
   *  @return EC_Normal if successful, EC_Suspend if more data is needed, an error code otherwise.
   */
  virtual OFCondition decode(
    Uint8 *compressedFrameBuffer,
    Uint32 compressedFrameBufferSize,
    Uint8 *uncompressedFrameBuffer,
    Uint32 uncompressedFrameBufferSize,
    OFBool isSigned) = 0;

  /** returns the number of bytes per sample that will be written when decoding.
   */
  virtual Uint16 bytesPerSample() const = 0;

  /** after successful compression,
   *  returns the color model of the decompressed image
   */
  virtual EP_Interpretation getDecompressedColorModel() const = 0;

};

#endif

/*
 * CVS/RCS Log
 * $Log: djdecabs.h,v $
 * Revision 1.2  2005-11-30 14:12:27  onken
 * Added decode() parameter denoting whether input is signed or unsigned
 *
 * Revision 1.1  2001/11/13 15:56:18  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
