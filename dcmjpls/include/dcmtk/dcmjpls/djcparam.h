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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm, Uli Schlachter
 *
 *  Purpose: codec parameter class JPEG-LS codecs
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:19 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMJPLS_DJCPARAM_H
#define DCMJPLS_DJCPARAM_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dccodec.h" /* for DcmCodecParameter */
#include "dcmtk/dcmjpls/djlsutil.h" /* for enums */

/** codec parameter for JPEG-LS codecs
 */
class DJLSCodecParameter: public DcmCodecParameter
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
    interleaveLine,
    /// Uninterleaved (color-by-plane)
    interleaveNone
  };

  /** constructor, for use with encoders.
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
   *  @param planarConfiguration       flag describing how planar configuration of decompressed color images should be handled
   *  @param ignoreOffsetTable         flag indicating whether to ignore the offset table when decompressing multiframe images
   *  @param jplsInterleaveMode        flag describing which interleave the JPEG-LS datastream should use
   */
   DJLSCodecParameter(
     OFBool jpls_optionsEnabled,
     Uint16 jpls_t1 = 3, // these are the defaults for 8bpp in lossless mode
     Uint16 jpls_t2 = 7,
     Uint16 jpls_t3 = 21,
     Uint16 jpls_reset = 64,
     Uint16 jpls_limit = 0,
     OFBool preferCookedEncoding = OFTrue,
     Uint32 fragmentSize = 0,
     OFBool createOffsetTable = OFTrue,
     JLS_UIDCreation uidCreation = EJLSUC_default,
     OFBool convertToSC = OFFalse,
     JLS_PlanarConfiguration planarConfiguration = EJLSPC_restore,
     OFBool ignoreOffsetTable = OFFalse,
     interleaveMode jplsInterleaveMode = interleaveLine);

  /** constructor, for use with decoders. Initializes all encoder options to defaults.
   *  @param uidCreation               mode for SOP Instance UID creation (used both for encoding and decoding)
   *  @param planarConfiguration       flag describing how planar configuration of decompressed color images should be handled
   *  @param ignoreOffsetTable         flag indicating whether to ignore the offset table when decompressing multiframe images
   */
  DJLSCodecParameter(
    JLS_UIDCreation uidCreation = EJLSUC_default,
    JLS_PlanarConfiguration planarConfiguration = EJLSPC_restore,
    OFBool ignoreOffsetTable = OFFalse);

  /// copy constructor
  DJLSCodecParameter(const DJLSCodecParameter& arg);

  /// destructor
  virtual ~DJLSCodecParameter();

  /** this methods creates a copy of type DcmCodecParameter *
   *  it must be overweritten in every subclass.
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

  /** returns JPEG-LS parameter LIMIT
   *  @return JPEG-LS parameter LIMIT
   */
  Uint16 getLimit() const
  {
    return jpls_t1_;
  }
  
  /** returns true if JPEG-LS parameters T1-T3, RESET and LIMIT are enabled, false otherwise
   *  @return true if JPEG-LS parameters T1-T3, RESET and LIMIT are enabled, false otherwise
   */
  OFBool getUseCustomOptions() const
  {
    return jpls_optionsEnabled_;
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

private:

  /// private undefined copy assignment operator
  DJLSCodecParameter& operator=(const DJLSCodecParameter&);

  // ****************************************************
  // **** Parameters describing the encoding process ****

  /// enable/disable use of all five JPEG-LS parameters
  OFBool jpls_optionsEnabled_;

  /// JPEG-LS parameter "Threshold 1" (used for quantization)
  Uint16 jpls_t1_;

  /// JPEG-LS parameter "Threshold 2"
  Uint16 jpls_t2_;

  /// JPEG-LS parameter "Threshold 3"
  Uint16 jpls_t3_;

  /// JPEG-LS parameter "RESET", i.e., value at which the counters A, B, and N are halved.
  Uint16 jpls_reset_;

  /// JPEG-LS parameter "LIMIT"
  Uint16 jpls_limit_;

  /// maximum fragment size (in kbytes) for compression, 0 for unlimited.
  Uint32 fragmentSize_;

  /// create offset table during image compression
  OFBool createOffsetTable_;

  /// Flag indicating if the "cooked" lossless encoder should be preferred over the "raw" one
  OFBool preferCookedEncoding_;

  /// mode for SOP Instance UID creation (used both for encoding and decoding)
  JLS_UIDCreation uidCreation_;

  /// flag indicating whether image should be converted to Secondary Capture upon compression
  OFBool convertToSC_;

  /// Flag describing the interleave mode which the encoder will use
  interleaveMode jplsInterleaveMode_;

  // ****************************************************
  // **** Parameters describing the decoding process ****

  /// flag describing how planar configuration of decompressed color images should be handled
  JLS_PlanarConfiguration planarConfiguration_;

  /// flag indicating if temporary files should be kept, false if they should be deleted after use
  OFBool ignoreOffsetTable_;

};


#endif

/*
 * CVS/RCS Log:
 * $Log: djcparam.h,v $
 * Revision 1.7  2010-10-14 13:17:19  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2010-03-01 10:35:28  uli
 * Renamed include guards to avoid name clash with e.g. dcmjpeg.
 *
 * Revision 1.5  2010-02-25 10:17:14  uli
 * Fix doxygen comments in a couple of places.
 *
 * Revision 1.4  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.3  2009-07-31 10:18:37  meichel
 * Line interleaved JPEG-LS mode now default. This mode works correctly
 *   when decompressing images with the LOCO-I reference implementation.
 *
 * Revision 1.2  2009-07-31 09:14:53  meichel
 * Added codec parameter and command line options that allow to control
 *   the interleave mode used in the JPEG-LS bitstream when compressing
 *   color images.
 *
 * Revision 1.1  2009-07-29 14:46:46  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.1  2007-06-15 14:35:45  meichel
 * Renamed CMake project and include directory from dcmjpgls to dcmjpls
 *
 * Revision 1.4  2007/06/15 10:39:15  meichel
 * Completed implementation of decoder, which now correctly processes all
 *   of the NEMA JPEG-LS sample images, including fragmented frames.
 *
 * Revision 1.3  2007/06/14 12:36:14  meichel
 * Further code clean-up. Updated doxygen comments.
 *
 * Revision 1.2  2007/06/13 16:41:07  meichel
 * Code clean-up and removal of dead code
 *
 *
 */
