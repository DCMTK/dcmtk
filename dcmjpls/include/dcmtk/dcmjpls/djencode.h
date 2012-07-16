/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm, Uli Schlachter
 *
 *  Purpose: singleton class that registers encoders for all supported JPEG-LS processes.
 *
 */

#ifndef DCMJPLS_DJENCODE_H
#define DCMJPLS_DJENCODE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"  /* for OFBool */
#include "dcmtk/dcmjpls/djlsutil.h"
#include "dcmtk/dcmdata/dctypes.h"  /* for Uint32 */
#include "dcmtk/dcmjpls/djcparam.h" /* for class DJLSCodecParameter */

class DJLSCodecParameter;
class DJLSLosslessEncoder;
class DJLSNearLosslessEncoder;

/** singleton class that registers encoders for all supported JPEG-LS processes.
 */
class DCMTK_DCMJPLS_EXPORT DJLSEncoderRegistration
{
public:

  /** registers encoders for all supported JPEG-LS processes. 
   *  If already registered, call is ignored unless cleanup() has
   *  been performed before.
   *  @param jpls_optionsEnabled       enable/disable use of all five JPEG-LS parameters
   *  @param jpls_t1                   JPEG-LS parameter "Threshold 1" (used for quantization)
   *  @param jpls_t2                   JPEG-LS parameter "Threshold 2"
   *  @param jpls_t3                   JPEG-LS parameter "Threshold 3"
   *  @param jpls_reset                JPEG-LS parameter "RESET", i.e., value at which the counters A, B, and N are halved.
   *  @param jpls_limit                JPEG-LS parameter "LIMIT"
   *  @param preferCookedEncoding      true if the "cooked" lossless encoder should be preferred over the "raw" one
   *  @param fragmentSize              maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @param createOffsetTable         create offset table during image compression
   *  @param uidCreation               mode for SOP Instance UID creation
   *  @param convertToSC               flag indicating whether image should be converted to Secondary Capture upon compression
   *  @param jplsInterleaveMode        flag describing which interleave the JPEG-LS datastream should use
   */
  static void registerCodecs(
    OFBool jpls_optionsEnabled = OFFalse,
    Uint16 jpls_t1 = 3,
    Uint16 jpls_t2 = 7,
    Uint16 jpls_t3 = 21,
    Uint16 jpls_reset = 64,
    Uint16 jpls_limit = 0,
    OFBool preferCookedEncoding = OFTrue,
    Uint32 fragmentSize = 0,
    OFBool createOffsetTable = OFTrue,
    JLS_UIDCreation uidCreation = EJLSUC_default,
    OFBool convertToSC = OFFalse,
    DJLSCodecParameter::interleaveMode jplsInterleaveMode = DJLSCodecParameter::interleaveDefault);

  /** deregisters encoders.
   *  Attention: Must not be called while other threads might still use
   *  the registered codecs, e.g. because they are currently encoding
   *  DICOM data sets through dcmdata.
   */
  static void cleanup();

  /** get version information of the CharLS library.
   *  Typical output format: "CharLS, Revision 55020 (modified)"
   *  @return name and version number of the CharLS library
   */
  static OFString getLibraryVersionString();

private:

  /// flag indicating whether the encoders are already registered.
  static OFBool registered_;

  /// pointer to codec parameter shared by all encoders
  static DJLSCodecParameter *cp_;

  /// pointer to encoder for lossless JPEG-LS
  static DJLSLosslessEncoder  *losslessencoder_;

  /// pointer to encoder for lossy JPEG-LS
  static DJLSNearLosslessEncoder *nearlosslessencoder_;

};

#endif
