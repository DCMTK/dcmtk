/*
 *
 *  Copyright (C) 2007-2014, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class to extract pixel data and meta information from JPEG file
 *
 */

#ifndef I2DJPGS_H
#define I2DJPGS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/dcmdata/libi2d/i2dimgs.h"

/**
 * JPEG markers consist of one or more 0xFF bytes, followed by a marker code byte
 * (which is not an FF). This enum lists the second byte of all these markers.
 * @note RESn markers are not fully listed, but only the first (RES0) and the
 *       last (RESN)
 */
enum E_JPGMARKER { E_JPGMARKER_SOF0 = 0xC0, E_JPGMARKER_SOF1 = 0xC1, E_JPGMARKER_SOF2 = 0xC2,
                   E_JPGMARKER_SOF3 = 0xC3, /*C4 and CC are not SOF markers,*/ E_JPGMARKER_SOF5 = 0xC5,
                   E_JPGMARKER_SOF6 = 0xC6, E_JPGMARKER_SOF7 = 0xC7, E_JPGMARKER_JPG = 0xC8,
                   E_JPGMARKER_SOF9 = 0xC9, E_JPGMARKER_SOF10 = 0xCA, E_JPGMARKER_SOF11 = 0xCB,
                   E_JPGMARKER_SOF13 = 0xCD, E_JPGMARKER_SOF14 = 0xCE, E_JPGMARKER_SOF15 = 0xCF,
                   E_JPGMARKER_DHT = 0xC4, E_JPGMARKER_DAC = 0xCC, E_JPGMARKER_RST0 = 0xD0,
                   E_JPGMARKER_RST1 = 0xD1, E_JPGMARKER_RST2 = 0xD2, E_JPGMARKER_RST3 = 0xD3,
                   E_JPGMARKER_RST4 = 0xD4, E_JPGMARKER_RST5 = 0xD5, E_JPGMARKER_RST6 = 0xD6,
                   E_JPGMARKER_RST7 = 0xD7, E_JPGMARKER_SOI = 0xD8, E_JPGMARKER_EOI = 0xD9,
                   E_JPGMARKER_SOS = 0xDA, E_JPGMARKER_DQT = 0xDB, E_JPGMARKER_DNL = 0xDC,
                   E_JPGMARKER_DRI = 0xDD, E_JPGMARKER_DHP = 0xDE, E_JPGMARKER_EXP = 0xDF,
                   E_JPGMARKER_APP0 = 0xE0, E_JPGMARKER_APP1 = 0xE1, E_JPGMARKER_APP2 = 0xE2,
                   E_JPGMARKER_APP3 = 0xE3, E_JPGMARKER_APP4 = 0xE4, E_JPGMARKER_APP5 = 0xE5,
                   E_JPGMARKER_APP6 = 0xE6, E_JPGMARKER_APP7 = 0xE7, E_JPGMARKER_APP8 = 0xE8,
                   E_JPGMARKER_APP9 = 0xE9, E_JPGMARKER_APP10 = 0xEA, E_JPGMARKER_APP11 = 0xEB,
                   E_JPGMARKER_APP12 = 0xEC, E_JPGMARKER_APP13 = 0xED, E_JPGMARKER_APP14 = 0xEE,
                   E_JPGMARKER_APP15 = 0xEF, E_JPGMARKER_JPGN0 = 0xF0, E_JPGMARKER_JPGN1 = 0xF1,
                   E_JPGMARKER_JPGN2 = 0xF2, E_JPGMARKER_JPGN3 = 0xF3, E_JPGMARKER_JPGN4 = 0xF4,
                   E_JPGMARKER_JPGN5 = 0xF5, E_JPGMARKER_JPGN6 = 0xF6, E_JPGMARKER_JPGN7 = 0xF7,
                   E_JPGMARKER_JPGN8 = 0xF8, E_JPGMARKER_JPGN9 = 0xF9, E_JPGMARKER_JPGN10 = 0xFA,
                   E_JPGMARKER_JPGN11 = 0xFB, E_JPGMARKER_JPGN12 = 0xFC, E_JPGMARKER_JPGN13 = 0xFD,
                   E_JPGMARKER_COM = 0xFE, E_JPGMARKER_TEM = 0x01, E_JPGMARKER_RES0 = 0x02,
                   E_JPGMARKER_RESN = 0xBF };

/**
 * Struct that represents a marker in a JPEG file, i.e. it consists
 * of the byte position of the marker and the marker code itself
 */
struct DCMTK_I2D_EXPORT JPEGFileMapEntry {
  /// The byte position of the marker
  offile_off_t bytePos;
  /// The marker byte itself
  E_JPGMARKER marker;
};


/** This is a I2DImgSource implementation that can parse JPEG files and convert
 *  them into DICOM images.
 */
class DCMTK_I2D_EXPORT I2DJpegSource : public I2DImgSource
{

public:

  /** Constructor, initializes member variables
   *  @return none
   */
  I2DJpegSource();

  /** Returns format of input image. For this class "JPEG" is returned.
   *  @return Returns format of input image, i.e. "JPEG".
   */
  OFString inputFormat() const;

  /** Extracts the raw JPEG pixel data stream from a JPEG file and returns some
   *  further information about this pixel data. Raw means here that all APP
   *  markers (e.g. JFIF information) are removed from the JPEG stream.
   *  The pixel data returned is a JPEG stream in JPEG interchange format.
   *  This function allocates memory for the pixel data returned to the user.
   *  The caller of this function is responsible for deleting the memory buffer.
   *  @param rows - [out] Rows of image
   *  @param cols - [out] Columns of image
   *  @param samplesPerPixel - [out] Number of components per pixel
   *  @param photoMetrInt - [out] The DICOM color model used for the compressed data
   *  @param bitsAlloc - [out] Bits Allocated for one sample
   *  @param bitsStored - [out] Bits Stored, Number of bits actually stored within
   *                            Bits Allocated
   *  @param highBit - [out] High Bit, Highest stored in bit within Bits Allocated
   *  @param pixelRepr - [out] Pixel Representation (0=unsigned, 1=signed)
   *  @param planConf - [out] Planar Configuration
   *  @param pixAspectH - [out] Horizontal value of pixel aspect ratio
   *  @param pixAspectV - [out] Vertical value of pixel aspect ratio
   *  @param pixData - [out] Pointer to the pixel data in JPEG Interchange Format
   *                         (but without APPx markers).
   *  @param length - [out] Length of pixel data
   *  @param ts - [out] The transfer syntax imposed by the imported pixel pixel
   *                    data. This is necessary for the JPEG importer that needs
   *                    to report which TS must be used for the imported JPEG data
   *                    (ie. baseline, progressive, ...).
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readPixelData( Uint16& rows,
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
                             E_TransferSyntax& ts);

  /** After reading of pixel data, this function can be used for getting
   *  information about lossy compression parameters.
   *  @param srcEncodingLossy - [out] Denotes, whether the encoding of the pixel
   *                            data read was lossy (OFTrue) or lossless (OFFalse)
   *  @param srcLossyComprMethod - [out] Denotes the lossy compression method used
   *                               in source if there is one (srcEncodingLossy = OFTrue).
   *                               Should use defined terms of attribute Lossy
   *                               Compression Method.
   *  @return EC_Normal if information is available, error otherwise
   */
  OFCondition getLossyComprInfo(OFBool& srcEncodingLossy,
                                OFString& srcLossyComprMethod) const;

  /** Enable/Disable support for Extended Sequential JPEG Coding
   *  @param enabled - [in] OFTrue: support Extended Sequential,
   *                        OFFalse: do not support
   *  @return none
   */
  void setExtSeqSupport(const OFBool enabled);

  /** Enable/Disable support for Progressive JPEG Coding
   *  @param enabled - [in] OFTrue: support Extended Sequential,
   *                        OFFalse: do not support
   *  @return none
   */
  void setProgrSupport(const OFBool enabled);

  /** If enabled, conversion will only take place if JFIF data could be found
   *  and evaluated. Many digital cameras do not provide a JFIF header, so this
   *  is disabled by default.
   */
  void setInsistOnJFIF(const OFBool enabled);

  /**  If enabled, APPn markers (except JFIF!) are also copied to the output file.
   *   This can cause a huge speedup because instead of parsing the whole
   *   JPEG file (for finding any APPn markers) the parsing stops after finding
   *   the SOFn marker (which is relevant for extracting width/height and so on.
   *   Default: false
   *   @param enabled - [in] OFTrue: copy APPn, OFFalse: cut off APPn info
   *   @return none
   */
  void setKeepAPPn(const OFBool enabled);

  /** Returns a string representation of a JPEG marker code.
   *  @param marker - [in] The marker to be converted
   *  @return A string representation of the marker
   */
  static OFString jpegMarkerToString(const E_JPGMARKER& marker);

  /** Destructor, frees some memory.
   *  @return none
   */
  ~I2DJpegSource();

protected:

  /** Opens the JPEG file specified by the given filename.
   *  @param filename - [in] The file to be opened
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition openFile(const OFString& filename);

  /** Closes JPEG file.
   *  @return A string representation of the marker
   */
  void closeFile();

  /** Function that scans a JPEG file and creates a "file map" which
   *  includes all JPEG markers and their byte positions in the file.
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition createJPEGFileMap();

  /** Dump the internal JPEG file map to a given stream. The file map
   *  lists JPEG markers and their position in the bitstream found in the JPEG
   *  file
   *  @return none
   */
  void debugDumpJPEGFileMap() const;

  /** Get image parameters as found at given SOF marker of the JPEG image.
   *  @param entry - [in] This specifies the marker and the byte position of the
   *                      SOF marker
   *  @param imageWidth - [out] The width of the image
   *  @param imageHeight - [out] The height of the image
   *  @param samplesPerPixel - [out] Number of components per pixel
   *  @param bitsPerSample - [out] Number of bits per pixel component
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition getSOFImageParameters( const JPEGFileMapEntry& entry,
                                     Uint16& imageWidth,
                                     Uint16& imageHeight,
                                     Uint16& samplesPerPixel,
                                     Uint16& bitsPerSample);

  /** Get JPEG parameters as found at given JFIF marker of the JPEG image.
   *  @param entry - [in] This specifies the marker and the byte position of the
   *                      JFIF marker
   *  @param jfifVersion - [out] The JFIF version of the JFIF data
   *  @param pixelAspectH - [out] The horizontal pixel aspect ratio
   *  @param pixelAspectV - [out] The vertical pixel aspect ratio
   *  @param unit - [out] The contents of the pixel aspect ratio unit field
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition getJFIFImageParameters( const JPEGFileMapEntry& entry,
                                      Uint16& jfifVersion,
                                      Uint16& pixelAspectH,
                                      Uint16& pixelAspectV,
                                      Uint16& unit);

  /** Check, whether a given JPEG image marker (SOFn) is supported by
   *  this plugin
   *  @param jpegEncoding - [in] Image marker that should be tested
   *  @return EC_Normal, marker is supported, error otherwise
   */
  OFCondition isJPEGEncodingSupported(const E_JPGMARKER& jpegEncoding) const;


  /** Returns if possible the DICOM transfer syntax matching the coding of the
   *  JPEG data.
   *  @param jpegEncoding - [in] Image marker that should be tested
   *  @return EC_Normal, marker is supported, error otherwise
   */
  static E_TransferSyntax associatedTS(const E_JPGMARKER& jpegEncoding);

  /** Returns true if marker is one of the RST0 to RST7 markers
   *  @param jpegEncoding - [in] Image marker that should be tested
   *  @return OFTrue, if marker is RST0 to RST7, OFFalse otherwise
   */
  static OFBool isRSTMarker(const E_JPGMARKER& jpegEncoding);

  /** Returns true if marker is one of the SOF0 to SOF15 markers
   *  @param jpegEncoding - [in] Image marker that should be tested
   *  @return OFTrue, if marker is SOF0 to SOF15, OFFalse otherwise
   */
  static OFBool isSOFMarker(const E_JPGMARKER& jpegEncoding);


  /** Extract raw JPEG stream (i.e. without APPn markers) from JPEG file.
   *  @param pixelData - [out] The resulting JPEG stream
   *  @param pixLength - [out] The length of the resulting stream
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition extractRawJPEGStream(char*& pixelData,
                                   Uint32& pixLength);

  /** Copies JPEG stream (with APPn markers, excluding JFIF) from JPEG file.
   *  @param pixelData - [out] The resulting JPEG stream
   *  @param pixLength - [out] The length of the resulting stream
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition copyJPEGStream(char*& pixelData,
                             Uint32& pixLength);

  /** Skips one marker while scanning through the JPEG file stream.
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition skipVariable();

  /** Tries to read the SOI marker.
   *  @param result - [out] The code of the SOI marker if successful (0xD8)
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition firstMarker(E_JPGMARKER& result);

  /** Tries to find the next JPEG marker in underlying file stream.
   *  @param lastWasSOSMarker - [in] Denotes, whether the last marker read
   *         before was the SOS (start of scan) marker. This is needed to
   *         ignore non-marker 0xFF occurrences in the compressed data.
   *  @param result - [out] The result marker
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition nextMarker(const OFBool& lastWasSOSMarker,
                         E_JPGMARKER& result);

  /** Read 2 bytes from the byte stream.
   *  @param result - [out] The result
   *  @return 0, if successful, EOF if rest of stream does not contain enough bytes
   */
  inline int read2Bytes(Uint16& result);

  /** Read 1 byte from the byte stream.
   *  @param result - [out] The result
   *  @return 0, if successful, EOF if rest of stream does not contain enough bytes
   */
  inline int read1Byte(Uint8& result);

  /** Deletes internal JPEG file map and frees memory.
   *  @return none
   */
  void clearMap();

  /// JPEG file map. This map includes all JPEG markers and their byte positions
  /// in the JPEG file.
  OFList<JPEGFileMapEntry*> m_jpegFileMap;

  /// The JPEG file, if opened
  OFFile jpegFile;

  /// If true, JPEGs with progressive coding are not supported
  OFBool m_disableProgrTs;

  /// If true, JPEGs with extended sequential coding are not supported
  OFBool m_disableExtSeqTs;

  /// If true, conversion will only work if JFIF header can be found.
  /// Default: false
  OFBool m_insistOnJFIF;

  /// If true, APPn markers (except JFIF!) are also copied to the output file.
  /// This can cause a huge speedup because instead of parsing the whole
  /// JPEG file (for finding any APPn markers) the parsing stops after finding
  /// the SOFn marker (which is relevant for extracting width/height and so on.
  /// Default: false
  OFBool m_keepAPPn;

  /// After reading pixel data, this denotes whether the source
  /// data is already lossy compressed
  OFBool m_lossyCompressed;
};

#endif // I2DJPGS_H
