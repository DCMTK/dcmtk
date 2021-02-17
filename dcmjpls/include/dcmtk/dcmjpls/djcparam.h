/*
 *
 *  Copyright (C) 1997-2020, OFFIS e.V.
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
 *  Purpose: codec parameter class JPEG-LS codecs
 *
 */

#ifndef DCMJPLS_DJCPARAM_H
#define DCMJPLS_DJCPARAM_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dccodec.h" /* for DcmCodecParameter */
#include "dcmtk/dcmjpls/djlsutil.h" /* for enums */

/** codec parameter for JPEG-LS codecs
 */
class DCMTK_DCMJPLS_EXPORT DJLSCodecParameter: public DcmCodecParameter
{
public:

  /** Constants describing the interleave mode which the encoder will use
   */
  enum interleaveMode
  {
    /// Use same interleave mode as the DICOM header says in planarConfiguration,
    /// if possible. The cooked encoder will force interleaveLine.
    interleaveDefault,
    /// Sample-interleaved (color-by-pixel)
    interleaveSample,
    /// Line-interleaved (color-by-line)
    interleaveLine
#ifdef ENABLE_DCMJPLS_INTERLEAVE_NONE
    ,
    /// Uninterleaved (color-by-plane)
    interleaveNone
#endif
  };

  /** constructor, for use with encoders.
   *  @param preferCookedEncoding      true if the "cooked" lossless encoder should be preferred over the "raw" one
   *                                   (which should be the default)
   *  @param jpls_t1                   JPEG-LS parameter "Threshold 1" (used for quantization)
   *  @param jpls_t2                   JPEG-LS parameter "Threshold 2"
   *  @param jpls_t3                   JPEG-LS parameter "Threshold 3"
   *  @param jpls_reset                JPEG-LS parameter "RESET", i.e., value at which the counters A, B, and N are halved.
   *  @param fragmentSize              maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @param createOffsetTable         create offset table during image compression
   *  @param uidCreation               mode for SOP Instance UID creation
   *  @param convertToSC               flag indicating whether image should be converted to Secondary Capture upon compression
   *  @param planarConfiguration       flag describing how planar configuration of decompressed color images should be handled
   *  @param ignoreOffsetTable         flag indicating whether to ignore the offset table when decompressing multiframe images
   *  @param jplsInterleaveMode        flag describing which interleave the JPEG-LS datastream should use
   *  @param useFFbitstreamPadding     flag indicating whether the JPEG-LS bitstream should be FF padded as required by DICOM.
   */
   DJLSCodecParameter(
     OFBool preferCookedEncoding,
     Uint16 jpls_t1 = 0,
     Uint16 jpls_t2 = 0,
     Uint16 jpls_t3 = 0,
     Uint16 jpls_reset = 0,
     Uint32 fragmentSize = 0,
     OFBool createOffsetTable = OFTrue,
     JLS_UIDCreation uidCreation = EJLSUC_default,
     OFBool convertToSC = OFFalse,
     JLS_PlanarConfiguration planarConfiguration = EJLSPC_restore,
     OFBool ignoreOffsetTable = OFFalse,
     interleaveMode jplsInterleaveMode = interleaveLine,
     OFBool useFFbitstreamPadding = OFTrue );

  /** constructor, for use with decoders. Initializes all encoder options to defaults.
   *  @param uidCreation                 mode for SOP Instance UID creation (used both for encoding and decoding)
   *  @param planarConfiguration         flag describing how planar configuration of decompressed color images should be handled
   *  @param ignoreOffsetTable           flag indicating whether to ignore the offset table when decompressing multiframe images
   *  @param forceSingleFragmentPerFrame while decompressing a multiframe image, assume one fragment per frame even if the JPEG
   *                                     data for some frame is incomplete
   */
  DJLSCodecParameter(
    JLS_UIDCreation uidCreation = EJLSUC_default,
    JLS_PlanarConfiguration planarConfiguration = EJLSPC_restore,
    OFBool ignoreOffsetTable = OFFalse,
    OFBool forceSingleFragmentPerFrame = OFFalse);

  /// copy constructor
  DJLSCodecParameter(const DJLSCodecParameter& arg);

  /// destructor
  virtual ~DJLSCodecParameter();

  /** this methods creates a copy of type DcmCodecParameter *
   *  it must be overwritten in every subclass.
   *  @return copy of this object
   */
  virtual DcmCodecParameter *clone() const;

  /** returns the class name as string.
   *  can be used as poor man's RTTI replacement.
   */
  virtual const char *className() const;

  /** returns secondary capture conversion flag
   *  @return secondary capture conversion flag
   */
  OFBool getConvertToSC() const
  {
    return convertToSC_;
  }

  /** returns create offset table flag
   *  @return create offset table flag
   */
  OFBool getCreateOffsetTable() const
  {
   return createOffsetTable_;
  }

  /** returns mode for SOP Instance UID creation
   *  @return mode for SOP Instance UID creation
   */
  JLS_UIDCreation getUIDCreation() const
  {
    return uidCreation_;
  }

  /** returns mode for handling planar configuration
   *  @return mode for handling planar configuration
   */
  JLS_PlanarConfiguration getPlanarConfiguration() const
  {
    return planarConfiguration_;
  }

  /** returns flag indicating whether or not the "cooked" lossless encoder
   *  should be preferred over the "raw" one
   *  @return raw/cooked lossless encoding flag
   */
  OFBool cookedEncodingPreferred() const
  {
    return preferCookedEncoding_;
  }

  /** returns maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @return maximum fragment size for compression
   */
  Uint32 getFragmentSize() const
  {
    return fragmentSize_;
  }

  /** returns JPEG-LS parameter T1
   *  @return JPEG-LS parameter T1
   */
  Uint16 getT1() const
  {
    return jpls_t1_;
  }

  /** returns JPEG-LS parameter T2
   *  @return JPEG-LS parameter T2
   */
  Uint16 getT2() const
  {
    return jpls_t2_;
  }

  /** returns JPEG-LS parameter T3
   *  @return JPEG-LS parameter T3
   */
  Uint16 getT3() const
  {
    return jpls_t3_;
  }

  /** returns JPEG-LS parameter RESET
   *  @return JPEG-LS parameter RESET
   */
  Uint16 getReset() const
  {
    return jpls_reset_;
  }

  /** returns true if the offset table should be ignored when decompressing multiframe images
   *  @return true if the offset table should be ignored when decompressing multiframe images
   */
  OFBool ignoreOffsetTable() const
  {
    return ignoreOffsetTable_;
  }

  /** returns the interleave mode which the encoder should use
   *  @return the interleave mode which the encoder should use
   */
  interleaveMode getJplsInterleaveMode() const
  {
    return jplsInterleaveMode_;
  }

  /** returns flag indicating whether one fragment per frame should be enforced while decoding
   *  @return flag indicating whether one fragment per frame should be enforced while decoding
   */
  OFBool getForceSingleFragmentPerFrame() const
  {
    return forceSingleFragmentPerFrame_;
  }

  /** returns flag indicating whether odd-length bitstreams should be padded as FF FF D9
   *  @return flag indicating whether odd-length bitstreams should be padded as FF FF D9
   */
  OFBool getUseFFbitstreamPadding() const
  {
    return useFFbitstreamPadding_;
  }

private:

  /// private undefined copy assignment operator
  DJLSCodecParameter& operator=(const DJLSCodecParameter&);

  // ****************************************************
  // **** Parameters describing the encoding process ****

  /// flag indicating if the "cooked" lossless encoder should be preferred over the "raw" one
  OFBool preferCookedEncoding_;

  /// JPEG-LS parameter "Threshold 1" (used for quantization), 0 if unused
  Uint16 jpls_t1_;

  /// JPEG-LS parameter "Threshold 2", 0 if unused
  Uint16 jpls_t2_;

  /// JPEG-LS parameter "Threshold 3", 0 if unused
  Uint16 jpls_t3_;

  /// JPEG-LS parameter "RESET", i.e., value at which the counters A, B, and N are halved. 0 if unused.
  Uint16 jpls_reset_;

  /// maximum fragment size (in kbytes) for compression, 0 for unlimited.
  Uint32 fragmentSize_;

  /// create offset table during image compression
  OFBool createOffsetTable_;

  /// mode for SOP Instance UID creation (used both for encoding and decoding)
  JLS_UIDCreation uidCreation_;

  /// flag indicating whether image should be converted to Secondary Capture upon compression
  OFBool convertToSC_;

  /// Flag describing the interleave mode which the encoder will use
  interleaveMode jplsInterleaveMode_;

  /** When true, a JPEG-LS bitstream of odd length is padded by extending the
   *  FF D9 "end of image" marker to FF FF D9, as required by DICOM. When false,
   *  the bitstream is written as FF D9 00, which is not standard compliant, but
   *  required for interoperability with the HP LOCO reference implementation,
   *  which does not support FF padded markers.
   */
  OFBool useFFbitstreamPadding_;

  // ****************************************************
  // **** Parameters describing the decoding process ****

  /// flag describing how planar configuration of decompressed color images should be handled
  JLS_PlanarConfiguration planarConfiguration_;

  /// flag indicating if temporary files should be kept, false if they should be deleted after use
  OFBool ignoreOffsetTable_;

  /** while decompressing a multiframe image,
   *  assume one fragment per frame even if the JPEG data for some frame is incomplete
   */
  OFBool forceSingleFragmentPerFrame_;

};


#endif
