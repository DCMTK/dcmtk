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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: singleton class that registers encoders for all supported JPEG processes.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:17 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJENCODE_H
#define DJENCODE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"  /* for OFBool */
#include "dcmtk/dcmjpeg/djutils.h"
#include "dcmtk/dcmdata/dctypes.h"  /* for Uint32 */

class DJCodecParameter;
class DJEncoderBaseline;
class DJEncoderExtended;
class DJEncoderLossless;
class DJEncoderP14SV1;
class DJEncoderProgressive;
class DJEncoderSpectralSelection;

/** singleton class that registers encoders for all supported JPEG processes.
 */
class DJEncoderRegistration
{
public:

  /** registers encoders for all supported JPEG processes.
   *  If already registered, call is ignored unless cleanup() has
   *  been performed before.
   *  @param pCompressionCSConversion color conversion mode for compression
   *  @param pCreateSOPInstanceUID mode for SOP Instance UID creation
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
   *  @param pAcceptWrongPaletteTags Accept wrong palette attribute tags (only "pseudo lossless" encoder)
   *  @param pAcrNemaCompatibility Accept old ACR-NEMA images without photometric interpretation (only "pseudo lossless" encoder)
   *  @param pRealLossless Enables true lossless compression (replaces old "pseudo" lossless encoders)
   */
  static void registerCodecs(
    E_CompressionColorSpaceConversion pCompressionCSConversion = ECC_lossyYCbCr,
    E_UIDCreation pCreateSOPInstanceUID = EUC_default,
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
    OFBool pUseModalityRescale = OFFalse,
    OFBool pAcceptWrongPaletteTags = OFFalse,
    OFBool pAcrNemaCompatibility = OFFalse,
    OFBool pRealLossless = OFTrue);

  /** deregisters encoders.
   *  Attention: Must not be called while other threads might still use
   *  the registered codecs, e.g. because they are currently encoding
   *  DICOM data sets through dcmdata.
   */
  static void cleanup();

private:

  /// flag indicating whether the encoders are already registered.
  static OFBool registered;

  /// pointer to codec parameter shared by all encoders
  static DJCodecParameter *cp;

  /// pointer to encoder for baseline JPEG
  static DJEncoderBaseline *encbas;

  /// pointer to encoder for extended JPEG
  static DJEncoderExtended *encext;

  /// pointer to encoder for spectral selection JPEG
  static DJEncoderSpectralSelection *encsps;

  /// pointer to encoder for progressive JPEG
  static DJEncoderProgressive *encpro;

  /// pointer to encoder for lossless SV1 JPEG
  static DJEncoderP14SV1 *encsv1;

  /// pointer to encoder for lossless JPEG
  static DJEncoderLossless *enclol;

};

#endif

/*
 * CVS/RCS Log
 * $Log: djencode.h,v $
 * Revision 1.9  2010-10-14 13:17:17  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2009-10-07 12:44:33  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.7  2008-04-30 12:45:52  meichel
 * DJEncoderRegistration::registerCodecs now by default enables the
 *   true lossless codec instead of pseudo-lossless.
 *
 * Revision 1.6  2005/12/08 16:59:32  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2005/11/29 15:57:05  onken
 * Added commandline options --accept-acr-nema and --accept-palettes
 * (same as in dcm2pnm) to dcmcjpeg and extended dcmjpeg to support
 * these options. Thanks to Gilles Mevel for suggestion.
 *
 * Revision 1.3  2005/11/29 08:50:34  onken
 * Added support for "true" lossless compression in dcmjpeg, that doesn't
 *   use dcmimage classes, but compresses raw pixel data (8 and 16 bit) to
 *   avoid losses in quality caused by color space conversions or modality
 *   transformations etc.
 * Corresponding commandline option in dcmcjpeg (new default)
 *
 * Revision 1.2  2001/11/19 15:13:29  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:56:27  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
