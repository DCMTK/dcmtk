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
 *  Purpose: codec parameter class for dcmjpeg codecs
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-09 13:51:26 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/include/Attic/djcparam.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJCPARAM_H
#define DJCPARAM_H

#include "osconfig.h"
#include "dccodec.h" /* for DcmCodecParameter */
#include "djutils.h" /* for enums */

/** codec parameter for IJG codecs
 */
class DJCodecParameter: public DcmCodecParameter
{
public:

  /** constructor.
   *  @param pCompressionCSConversion color conversion mode for compression
   *  @param pDecompressionCSConversion color conversion mode for decompression
   *  @param pCreateSOPInstanceUID mode for SOP Instance UID creation
   *  @param pPlanarConfiguration flag describing how planar configuration of
   *    decompressed color images should be handled
   *  @param pVerbose verbose mode flag
   *  @param pOptimizeHuffman perform huffman table optimization for 8 bits/pixel compression?
   *  @param pSmoothingFactor smoothing factor for image compression, 0..100
   *  @param pForcedBitDepth forced bit depth for image compression, 0 (auto) or 8/12/16
   *  @param pFragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @param pCreateOffsetTable create offset table during image compression?
   *  @param pSampleFactors subsampling mode for color image compression
   *  @param pWriteYBR422 flag indicating whether a compressed YBR color stream should
   *    be marked as YBR_FULL or YBR_FULL_422 on DICOM level
   *  @param pConvertToSC flag indicating whether image should be converted to
   *    Secondary Capture upon compression
   *  @param pWindowType mode for VOI transformation of monochrome images
   *  @param pWindowParameter parameter for VOI transform of monochrome images, used in modes 1, 2, 4, 6
   *  @param pVoiCenter VOI window center for mode 5
   *  @param pVoiWidth  VOI window width for mode 5
   *  @param pRoiLeft   Region of Interest left corner for for VOI transform of monochrome images, mode 7
   *  @param pRoiTop    Region of Interest upper corner for for VOI transform of monochrome images, mode 7
   *  @param pRoiWidth  Region of Interest width for for VOI transform of monochrome images, mode 7
   *  @param pRoiHeight Region of Interest height for for VOI transform of monochrome images, mode 7
   *  @param pUsePixelValues Check smallest and largest pixel value and optimize compression, mode 0 only
   *  @param pUseModalityRescale Create Rescale Slope/Intercept to scale back
   *     to original pixel range, mode 0 only
   */
  DJCodecParameter(
    E_CompressionColorSpaceConversion pCompressionCSConversion,
    E_DecompressionColorSpaceConversion pDecompressionCSConversion,
    E_UIDCreation pCreateSOPInstanceUID,
    E_PlanarConfiguration pPlanarConfiguration,
    OFBool pVerbose = OFFalse,
    OFBool pOptimizeHuffman = OFFalse,
    int pSmoothingFactor = 0,
    int pForcedBitDepth = 0,
    Uint32 pFragmentSize = 0,
    OFBool pCreateOffsetTable = OFTrue,
    E_SubSampling pSampleFactors = ESS_444,
    OFBool pWriteYBR422 = OFFalse,
    OFBool pConvertToSC = OFFalse,
    unsigned long pWindowType = 0,
    unsigned long pWindowParameter = 0,
    double pVoiCenter = 0.0,
    double pVoiWidth = 0.0,
    unsigned long pRoiLeft = 0,
    unsigned long pRoiTop = 0,
    unsigned long pRoiWidth = 0,
    unsigned long pRoiHeight = 0,
    OFBool pUsePixelValues = OFTrue,
    OFBool pUseModalityRescale = OFFalse);

  /// copy constructor
  DJCodecParameter(const DJCodecParameter& arg);

  /// destructor
  virtual ~DJCodecParameter();

  /** this methods creates a copy of type DcmCodecParameter *
   *  it must be overweritten in every subclass.
   *  @return copy of this object
   */
  virtual DcmCodecParameter *clone() const;

  /** returns the class name as string.
   *  can be used as poor man's RTTI replacement.
   */
  virtual const char *className() const;

  /** returns huffman table optimization flag for 8 bits/pixel compression
   *  @return huffman table optimization flag for 8 bits/pixel compression
   */
  OFBool getOptimizeHuffmanCoding() const
  {
    return optimizeHuffman;
  }

  /** returns smoothing factor for image compression, 0..100
   *  @return smoothing factor for image compression, 0..100
   */
  int getSmoothingFactor() const
  {
    return smoothingFactor;
  }

  /** returns forced bit depth for image compression, 0 (auto) or 8/12/16
   *  @return forced bit depth for image compression
   */
  int getForcedBitDepth() const
  {
    return forcedBitDepth;
  }

  /** returns maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @returnmaximum fragment size for compression
   */
  Uint32 getFragmentSize() const
  {
    return fragmentSize;
  }

  /** returns offset table creation flag
   *  @return offset table creation flag
   */
  OFBool getCreateOffsetTable() const
  {
    return createOffsetTable;
  }

  /** returns subsampling mode for color image compression
   *  @return subsampling mode for color image compression
   */
  E_SubSampling getSampleFactors() const
  {
    return sampleFactors;
  }

  /** returns flag indicating whether a compressed YBR color stream should be marked
   *  as YBR_FULL or YBR_FULL_422 on DICOM level
   *  @return YBR422 flag
   */
  OFBool getWriteYBR422() const
  {
    return writeYBR422;
  }

  /** returns secondary capture conversion flag
   *  @return secondary capture conversion flag
   */
  OFBool getConvertToSC() const
  {
    return convertToSC;
  }

  /** returns mode for SOP Instance UID creation
   *  @return mode for SOP Instance UID creation
   */
  E_UIDCreation getUIDCreation() const
  {
    return uidCreation;
  }

  /** returns mode for VOI transformation of monochrome images.
   *  @return mode for VOI transformation of monochrome images
   */
  unsigned long getWindowType() const
  {
    return windowType;
  }

  /** returns parameter for VOI transform of monochrome images, used in VOI modes 1, 2, 4, 6
   *  @return parameter for VOI transform of monochrome images, used in VOI modes 1, 2, 4, 6
   */
  unsigned long getWindowParameter() const
  {
    return windowParameter;
  }

  /** returns VOI window center and width for VOI mode 5
   *  @param center VOI center returned in this parameter
   *  @param width VOI width returned in this parameter
   */
  void getVOIWindow(double& center, double& width) const
  {
    center = voiCenter;
    width = voiWidth;
  }

  /** returns ROI coordinates for VOI mode 7
   *  @param left_pos ROI left edge returned in this parameter
   *  @param top_pos ROI top edge returned in this parameter
   *  @param width ROI width returned in this parameter
   *  @param height ROI height returned in this parameter
   */
  void getROI(
    unsigned long& left_pos,
    unsigned long& top_pos,
    unsigned long& width,
    unsigned long& height) const
  {
    left_pos = roiLeft;
    top_pos = roiTop;
    width = roiWidth;
    height = roiHeight;
  }

  /** returns planar configuration flag
   *  @return planar configuration flag
   */
  E_PlanarConfiguration getPlanarConfiguration() const
  {
    return planarConfiguration;
  }

  /** returns color conversion mode for compression
   *  @return color conversion mode for compression
   */
  E_CompressionColorSpaceConversion getCompressionColorSpaceConversion() const
  {
    return compressionCSConversion;
  }

  /** returns color conversion mode for compression
   *  @return color conversion mode for compression
   */
  E_DecompressionColorSpaceConversion getDecompressionColorSpaceConversion() const
  {
    return decompressionCSConversion;
  }

  /** returns flag indicating if compression to be optimized for real pixel values
   *  @return flag indicating if compression to be optimized for real pixel values
   */
  OFBool getUsePixelValues() const
  {
    return usePixelValues;
  }

  /** returns flag indicating if compression should compute Rescale Slope/Intercept
   *  @return flag indicating if compression should compute Rescale Slope/Intercept
   */
  OFBool getUseModalityRescale() const
  {
    return useModalityRescale;
  }

  /** returns verbose mode flag
   *  @return verbose mode flag
   */
  OFBool isVerbose() const
  {
    return verboseMode;
  }

private:

  /// private undefined copy assignment operator
  DJCodecParameter& operator=(const DJCodecParameter&);

  /// color conversion mode for compression
  E_CompressionColorSpaceConversion compressionCSConversion;

  /// color conversion mode for decompression
  E_DecompressionColorSpaceConversion decompressionCSConversion;

  /// flag describing how planar configuration of decompressed color images should be handled
  E_PlanarConfiguration planarConfiguration;

  /// perform huffman table optimization for 8 bits/pixel compression?
  OFBool optimizeHuffman;

  /// smoothing factor for image compression, 0..100
  int smoothingFactor;

  /// forced bit depth for image compression, 0 (auto) or 8/12/16
  int forcedBitDepth;

  /// maximum fragment size (in kbytes) for compression, 0 for unlimited.
  Uint32 fragmentSize;

  /// create offset table during image compression
  OFBool createOffsetTable;

  /// subsampling mode for color image compression
  E_SubSampling sampleFactors;

  /** flag indicating whether a compressed YBR color stream should be marked
   *  as YBR_FULL or YBR_FULL_422 on DICOM level
   */
  OFBool writeYBR422;

  /// flag indicating whether image should be converted to Secondary Capture upon compression
  OFBool convertToSC;

  /// mode for SOP Instance UID creation
  E_UIDCreation uidCreation;

  /// mode for VOI transformation of monochrome images
  unsigned long windowType;

  /// parameter for VOI transform of monochrome images, used in modes 1, 2, 4, 6
  unsigned long windowParameter;

  /// VOI window center for mode 5
  double voiCenter;

  /// VOI window width for mode 5
  double voiWidth;

  /// Region of Interest left corner for for VOI transform of monochrome images, mode 7
  unsigned long roiLeft;

  /// Region of Interest upper corner for for VOI transform of monochrome images, mode 7
  unsigned long roiTop;

  /// Region of Interest width for for VOI transform of monochrome images, mode 7
  unsigned long roiWidth;

  /// Region of Interest height for for VOI transform of monochrome images, mode 7
  unsigned long roiHeight;

  /// Check smallest and largest pixel value and optimize compression, mode 0 only
  OFBool usePixelValues;

  /// Create Rescale Slope/Intercept to scale back to original pixel range, mode 0 only
  OFBool useModalityRescale;

  /// verbose mode flag. If true, warning messages are printed to console
  OFBool verboseMode;
};


#endif

/*
 * CVS/RCS Log
 * $Log: djcparam.h,v $
 * Revision 1.3  2002-12-09 13:51:26  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.2  2001/11/19 15:13:26  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:56:17  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
