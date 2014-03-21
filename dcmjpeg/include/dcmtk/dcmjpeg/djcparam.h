/*
 *
 *  Copyright (C) 1997-2014, OFFIS e.V.
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
 *  Purpose: codec parameter class for dcmjpeg codecs
 *
 */

#ifndef DJCPARAM_H
#define DJCPARAM_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dccodec.h" /* for DcmCodecParameter */
#include "dcmtk/dcmjpeg/djutils.h" /* for enums */

/** codec parameter for IJG codecs
 */
class DCMTK_DCMJPEG_EXPORT DJCodecParameter: public DcmCodecParameter
{
public:

  /** constructor.
   *  @param pCompressionCSConversion color conversion mode for compression
   *  @param pDecompressionCSConversion color conversion mode for decompression
   *  @param pCreateSOPInstanceUID mode for SOP Instance UID creation
   *  @param pPlanarConfiguration flag describing how planar configuration of
   *    decompressed color images should be handled
   *  @param predictor6WorkaroundEnable enable workaround for buggy lossless compressed images with
   *    overflow in predictor 6 for images with 16 bits/pixel
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
   *  @param pUsePixelValues check smallest and largest pixel value and optimize compression, mode 0 only
   *  @param pUseModalityRescale create Rescale Slope/Intercept to scale back to original pixel range, mode 0 only
   *  @param pAcceptWrongPaletteTags accept wrong palette attribute tags (only "pseudo lossless" encoder)
   *  @param pAcrNemaCompatibility accept old ACR-NEMA images without photometric interpretation
   *    (only "pseudo" lossless encoder)
   *  @param pTrueLosslessMode Enables true lossless compression (replaces old "pseudo lossless" encoder)
   */
  DJCodecParameter(
    E_CompressionColorSpaceConversion pCompressionCSConversion,
    E_DecompressionColorSpaceConversion pDecompressionCSConversion,
    E_UIDCreation pCreateSOPInstanceUID,
    E_PlanarConfiguration pPlanarConfiguration,
    OFBool predictor6WorkaroundEnable = OFFalse,
    OFBool pOptimizeHuffman = OFFalse,
    int pSmoothingFactor = 0,
    int pForcedBitDepth = 0,
    Uint32 pFragmentSize = 0,
    OFBool pCreateOffsetTable = OFTrue,
    E_SubSampling pSampleFactors = ESS_444,
    OFBool pWriteYBR422 = OFFalse,
    OFBool pConvertToSC = OFFalse,
    size_t pWindowType = 0,
    size_t pWindowParameter = 0,
    double pVoiCenter = 0.0,
    double pVoiWidth = 0.0,
    size_t pRoiLeft = 0,
    size_t pRoiTop = 0,
    size_t pRoiWidth = 0,
    size_t pRoiHeight = 0,
    OFBool pUsePixelValues = OFTrue,
    OFBool pUseModalityRescale = OFFalse,
    OFBool pAcceptWrongPaletteTags = OFFalse,
    OFBool pAcrNemaCompatibility = OFFalse,
    OFBool pTrueLosslessMode = OFTrue);

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
  size_t getWindowType() const
  {
    return windowType;
  }

  /** returns parameter for VOI transform of monochrome images, used in VOI modes 1, 2, 4, 6
   *  @return parameter for VOI transform of monochrome images, used in VOI modes 1, 2, 4, 6
   */
  size_t getWindowParameter() const
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
    size_t& left_pos,
    size_t& top_pos,
    size_t& width,
    size_t& height) const
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

  /** Returns flag indicating if incorrect palette attribute tags should be accepted
   *  @return flag indicating if incorrect palette attribute tags should be accepted
   */
  OFBool getAcceptWrongPaletteTags() const
  {
    return acceptWrongPaletteTags;
  }

  /** Returns flag indicating if compatibility to old ACR-NEMA images without
   *  photometric interpretation is enabled.
   *  @return flag indicating if ACR-NEMA compatibility is enabled
   */

  OFBool getAcrNemaCompatibility() const
  {
    return acrNemaCompatibility;
  }

  /** returns flag indicating if real lossless mode is enabled
   *  @return flag indicating if real lossless mode is enabled
   */
  OFBool getTrueLosslessMode() const
  {
    return trueLosslessMode;
  }

  /** returns flag indicating whether the workaround for buggy JPEG lossless images with incorrect predictor 6 is enabled
   *  @return flag indicating whether the workaround for buggy JPEG lossless images with incorrect predictor 6 is enabled
   */
  OFBool predictor6WorkaroundEnabled() const
  {
    return predictor6WorkaroundEnabled_;
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

  /** mode for VOI transformation of monochrome images. possible values:
      0: no VOI transformation (default). See also variables
         usePixelValues (default) or useModalityRescale denoting two
         further variables tweaking compression behaviour.
      1: use the n-th VOI window from the image file (see variable windowType)
      2: use the n-th VOI look up table from the image file (see variable windowType)
      3: compute VOI window using min-max algorithm
      4: compute VOI window using Histogram algorithm, ignoring n percent
      5: compute VOI window using center r and width s
      6: compute VOI window using min-max algorithm ignoring extremes
      7: compute region of interest VOI window
   */
  size_t windowType;

  /// parameter for VOI transform of monochrome images, used in modes 1, 2, 4, 6
  size_t windowParameter;

  /// VOI window center for mode 5
  double voiCenter;

  /// VOI window width for mode 5
  double voiWidth;

  /// Region of Interest left corner for for VOI transform of monochrome images, mode 7
  size_t roiLeft;

  /// Region of Interest upper corner for for VOI transform of monochrome images, mode 7
  size_t roiTop;

  /// Region of Interest width for for VOI transform of monochrome images, mode 7
  size_t roiWidth;

  /// Region of Interest height for for VOI transform of monochrome images, mode 7
  size_t roiHeight;

  /// Check smallest and largest pixel value and optimize compression, mode 0 only
  OFBool usePixelValues;

  /// Create Rescale Slope/Intercept to scale back to original pixel range, mode 0 only
  OFBool useModalityRescale;

  /// Accept wrong palette attribute tags
  OFBool acceptWrongPaletteTags;

  /// Compatibility with old ACR-NEMA images without photometric interpretation
  OFBool acrNemaCompatibility;

  /// True losless mode, replaces old "pseudo" lossless encoders, when true (default)
  OFBool trueLosslessMode;

  /// flag indicating that the workaround for buggy JPEG lossless images with incorrect predictor 6 is enabled
  OFBool predictor6WorkaroundEnabled_;

};


#endif
