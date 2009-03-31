/*
 *
 *  Copyright (C) 2001-2007, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Base Class for plugins extracting pixel data from standard
 *           image files
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2009-03-31 11:32:06 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef I2DIMGS_H
#define I2DIMGS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

class I2DImgSource
{

public:

  /** Constructor
   */
  I2DImgSource() : m_logStream(NULL), m_debug(OFFalse), m_imageFile() {}

  /** Returns identifier for the image input format the plugin imports.
   *  @return A short identifier (e. g. "JPEG")
   */
  virtual OFString inputFormat() const =0;

  /** Extracts the raw JPEG pixel data stream from a JPEG file and returns some
   *  image information about this pixel data.
   *  Raw means here that all APP markers (e.g. JFIF information) are removed from the JPEG stream.
   *  The pixel data returned is a JPEG stream in JPEG interchange format.
   *  This function allocates memory for the pixel data returned to the user. The caller of this
   *  function is responsible for deleting the memory buffer
   *  @param rows - [out] Rows of image
   *  @param cols - [out] Columns of image
   *  @param samplesPerPixel - [out] Number of components per pixel
   *  @param photoMetrInt - [out] The DICOM color model used for the compressed data
   *  @param bitsAlloc - [out] Bits Allocated for one sample
   *  @param bitsStored - [out] Bits Stored, number of bits stored within Bits Allocated
   *  @param highBit - [out] High Bit, hightest bit position set within Bits Allocated
   *  @param pixelRepr - [out] Pixel Representation (0=unsigned, 1=signed)
   *  @param planConf - [out] Planar Configuration
   *  @param pixAspectH - [out] Horizontal value of pixel aspect ratio
   *  @param pixAspectV - [out] Vertical value of pixel aspect ratio
   *  @param pixData - [out] Pointer to the pixel data in JPEG Interchange Format (but without APPx markers).
   *  @param length - [out] Length of pixel data
   *  @param srcEncodingLossy - [out] Denotes, whether the encoding of the pixel
   *                            data read was lossy (OFtrue) or lossless (OFFalse)
   *  @param ts - [out] The transfer syntax imposed by the imported pixel pixel data.
                        This is necessary for the JPEG importer that needs to report
                        which TS must be used for the imported JPEG data (ie. baseline, progressive, ...).
                        If pixel data is uncompressed, EXS_Unknown is returned
   *  @return EC_Normal, if successful, error otherwise
   */
  virtual OFCondition readPixelData( Uint16& rows,
                                     Uint16& cols,
                                     Uint16& samplesPerPixel,
                                     OFString& photoMetrInt,
                                     Uint16& bitsAlloc,
                                     Uint16& bitsStored,
                                     Uint16& highBit,
                                     Uint16& pixelRepr,
                                     Uint16& planConf,
                                     Uint16& pixAspectH,
                                     Uint16& pixAspectV,
                                     char*&  pixData,
                                     Uint32& length,
                                     OFBool& srcEncodingLossy,
                                     E_TransferSyntax& ts) =0;

  /** Sets the input image file to read.
   *  @param filename - [in] The filename of the image input file
   *  @return none
   */
  void setImageFile(const OFString& filename) { m_imageFile = filename; };

  /** Returns the input image file that is going to be read
   *  @return The filename of the image input file
   */
  OFString getImageFile() { return m_imageFile; };

  /** Sets the log stream
   *  The log stream is used to report any warnings and error messages.
   *  @param stream - [out] pointer to the log stream (might be NULL = no messages)
   *  @return none
   */
  void setLogStream(OFConsole *stream)
  {
    m_logStream= stream;
  }

  /** Sets the debug mode
   *  @param debugMode - [in] New status for debug mode
   *  @return none
   */
  void setDebugMode(const OFBool& debugMode) { m_debug = debugMode; };

  /** Prints a message to the given stream.
   ** @param  stream - [out] output stream to which the message is printed
   *  @param  message1 - [in] first part of message to be printed
   *  @param  message2 - [in] second part of message to be printed
   *  @return none
   */
  static void printMessage(OFConsole *stream,
                           const OFString& message1,
                           const OFString& message2 = "")
  {
    if (stream != NULL)
    {
        stream->lockCerr() << message1 << message2 << OFendl;
        stream->unlockCerr();
    }
  }

  /** Virtual Destructor
   */
  virtual ~I2DImgSource() {}

protected:

  /// stream where warning/error message are sent to.
  /// can be NULL (default, no output).
  OFConsole *m_logStream;

  /// debug mode status
  OFBool m_debug;

  /// The input file
  OFString m_imageFile;

};

#endif // #ifndef I2DIMGS_H

/*
 * CVS/RCS Log:
 * $Log: i2dimgs.h,v $
 * Revision 1.3  2009-03-31 11:32:06  onken
 * Attribute "Lossy Image Compression" is now written per default if
 * source image already had a lossy encoding. Thanks to Mathieu Malaterre
 * for the suggestion.
 *
 * Revision 1.2  2009-01-16 09:51:55  onken
 * Completed doxygen documentation for libi2d.
 *
 * Revision 1.1  2008-01-16 15:08:38  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 * Revision 1.2  2008-01-11 14:17:53  onken
 * Added various options to i2dlib. Changed logging to use a configurable
 * logstream. Added output plugin for the new Multiframe Secondary Capture SOP
 * Classes. Added mode for JPEG plugin to copy exsiting APPn markers (except
 * JFIF). Changed img2dcm default behaviour to invent type1/type2 attributes (no
 * need for templates any more). Added some bug fixes.
 *
 * Revision 1.1  2007/11/08 15:58:55  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */

