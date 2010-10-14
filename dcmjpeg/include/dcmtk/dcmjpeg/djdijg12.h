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
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: decompression routines of the IJG JPEG library configured for 12 bits/sample.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:17 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJDIJG12_H
#define DJDIJG12_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdecabs.h" /* for class DJDecoder */

extern "C"
{
  struct jpeg_decompress_struct;
}

class DJCodecParameter;

/** this class encapsulates the decompression routines of the
 *  IJG JPEG library configured for 12 bits/sample.
 */
class DJDecompressIJG12Bit : public DJDecoder
{
public:

  /** constructor
   *  @param cp codec parameters
   *  @param isYBR flag indicating if DICOM photometric interpretation is YCbCr
   */
  DJDecompressIJG12Bit(const DJCodecParameter& cp, OFBool isYBR);

  /// destructor
  virtual ~DJDecompressIJG12Bit();

  /** initializes internal object structures.
   *  Must be called before a new frame is decompressed.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition init();

  /** suspended decompression routine. Decompresses a JPEG frame
   *  until finished or out of data. Can be called with new data
   *  until a frame is complete.
   *  @param compressedFrameBuffer pointer to compressed input data, must not be NULL
   *  @param compressedFrameBufferSize size of buffer, in bytes
   *  @param uncompressedFrameBuffer pointer to uncompressed output data, must not be NULL.
   *     This buffer must not change between multiple decode() calls for a single frame.
   *  @param uncompressedFrameBufferSize size of buffer, in bytes (!)
   *     Buffer must be large enough to contain a complete frame.
   *  @param isSigned OFTrue, if uncompressed pixel data is signed, OFFalse otherwise
   *  @return EC_Normal if successful, EC_Suspend if more data is needed, an error code otherwise.
   */
  virtual OFCondition decode(
    Uint8 *compressedFrameBuffer,
    Uint32 compressedFrameBufferSize,
    Uint8 *uncompressedFrameBuffer,
    Uint32 uncompressedFrameBufferSize,
    OFBool isSigned);

  /** returns the number of bytes per sample that will be written when decoding.
   */
  virtual Uint16 bytesPerSample() const
  {
    return sizeof(Uint16);
  }

  /** after successful compression,
   *  returns the color model of the decompressed image
   */
  virtual EP_Interpretation getDecompressedColorModel() const
  {
    return decompressedColorModel;
  }

  /** callback function used to report warning messages and the like.
   *  Should not be called by user code directly.
   *  @param msg_level -1 for warnings, 0 and above for trace messages
   */
  virtual void emitMessage(int msg_level) const;

private:

  /// private undefined copy constructor
  DJDecompressIJG12Bit(const DJDecompressIJG12Bit&);

  /// private undefined copy assignment operator
  DJDecompressIJG12Bit& operator=(const DJDecompressIJG12Bit&);

  /// cleans up cinfo structure, called from destructor and error handlers
  void cleanup();

  /// codec parameters
  const DJCodecParameter *cparam;

  /// decompression structure
  jpeg_decompress_struct *cinfo;

  /// position of last suspend
  int suspension;

  /// temporary storage for row buffer during suspension
  void *jsampBuffer;

  /// Flag indicating if DICOM photometric interpretation is YCbCr
  OFBool dicomPhotometricInterpretationIsYCbCr;

  /// color model after decompression
  EP_Interpretation decompressedColorModel;

};

#endif

/*
 * CVS/RCS Log
 * $Log: djdijg12.h,v $
 * Revision 1.6  2010-10-14 13:17:17  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2009-11-18 16:17:53  uli
 * Use more than just the INFO log level.
 *
 * Revision 1.4  2005-12-08 16:59:22  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2005/11/30 14:08:57  onken
 * Added check to decline automatic IJG color space conversion of signed pixel
 * data, because IJG lib only handles unsigned input for conversions.
 *
 * Revision 1.2  2001/11/19 15:13:27  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:56:22  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
