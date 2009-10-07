/*
 *
 *  Copyright (C) 1997-2006, OFFIS
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: singleton class that registers decoders for all supported JPEG processes.
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-10-07 12:44:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/include/dcmtk/dcmjpeg/djdecode.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJDECODE_H
#define DJDECODE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"  /* for OFBool */
#include "dcmtk/dcmjpeg/djutils.h"

class DJCodecParameter;
class DJDecoderBaseline;
class DJDecoderExtended;
class DJDecoderLossless;
class DJDecoderP14SV1;
class DJDecoderProgressive;
class DJDecoderSpectralSelection;

/** singleton class that registers decoders for all supported JPEG processes.
 */
class DJDecoderRegistration 
{
public: 
  /** registers decoders for all supported JPEG processes.
   *  If already registered, call is ignored unless cleanup() has
   *  been performed before.
   *  @param pCreateSOPInstanceUID flag indicating whether or not
   *    a new SOP Instance UID should be assigned upon decompression.
   *  @param pPlanarConfiguration flag indicating how planar configuration
   *    of color images should be encoded upon decompression.
   *  @param predictor6WorkaroundEnable enable workaround for buggy lossless compressed images with
   *           overflow in predictor 6 for images with 16 bits/pixel
   */   
  static void registerCodecs(
    E_DecompressionColorSpaceConversion pDecompressionCSConversion = EDC_photometricInterpretation,
    E_UIDCreation pCreateSOPInstanceUID = EUC_default,
    E_PlanarConfiguration pPlanarConfiguration = EPC_default,
    OFBool predictor6WorkaroundEnable = OFFalse);

  /** deregisters decoders.
   *  Attention: Must not be called while other threads might still use
   *  the registered codecs, e.g. because they are currently decoding
   *  DICOM data sets through dcmdata.
   */  
  static void cleanup();

private:

  /// flag indicating whether the decoders are already registered.
  static OFBool registered;

  /// pointer to codec parameter shared by all decoders
  static DJCodecParameter *cp;
  
  /// pointer to decoder for baseline JPEG
  static DJDecoderBaseline *decbas;

  /// pointer to decoder for extended JPEG
  static DJDecoderExtended *decext;

  /// pointer to decoder for spectral selection JPEG
  static DJDecoderSpectralSelection *decsps;

  /// pointer to decoder for progressive JPEG
  static DJDecoderProgressive *decpro;

  /// pointer to decoder for lossless SV1 JPEG
  static DJDecoderP14SV1 *decsv1;

  /// pointer to decoder for lossless JPEG
  static DJDecoderLossless *declol;
  
};

#endif

/*
 * CVS/RCS Log
 * $Log: djdecode.h,v $
 * Revision 1.5  2009-10-07 12:44:33  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.4  2006-03-29 15:58:52  meichel
 * Added support for decompressing images with 16 bits/pixel compressed with
 *   a faulty lossless JPEG encoder that produces integer overflows in predictor 6.
 *
 * Revision 1.3  2005/12/08 16:59:18  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2001/11/19 15:13:27  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:56:20  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
