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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: singleton class that registers decoders for all supported JPEG processes.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:56:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/include/Attic/djdecode.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJDECODE_H
#define DJDECODE_H

#include "osconfig.h"
#include "oftypes.h"  /* for OFBool */
#include "djutils.h"

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
   */   
  static void registerCodecs(
    E_DecompressionColorSpaceConversion pDecompressionCSConversion = EDC_photometricInterpretation,
    E_UIDCreation pCreateSOPInstanceUID = EUC_default,
    E_PlanarConfiguration pPlanarConfiguration = EPC_default);

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
 * Revision 1.1  2001-11-13 15:56:20  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
