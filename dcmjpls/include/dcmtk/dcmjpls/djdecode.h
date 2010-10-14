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
 *  Author:  Martin Willkomm
 *
 *  Purpose: singleton class that registers the decoder for all supported JPEG-LS processes.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:19 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMJPLS_DJDECODE_H
#define DCMJPLS_DJDECODE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"      /* for OFBool */
#include "dcmtk/dcmjpls/djlsutil.h"  /* for enums */

class DJLSCodecParameter;
class DJLSLosslessDecoder;
class DJLSNearLosslessDecoder;

/** singleton class that registers decoders for all supported JPEG-LS processes.
 */
class DJLSDecoderRegistration
{
public:

  /** registers decoder for all supported JPEG-LS processes.
   *  If already registered, call is ignored unless cleanup() has
   *  been performed before.
   *  @param uidcreation flag indicating whether or not
   *    a new SOP Instance UID should be assigned upon decompression.
   *  @param planarconfig flag indicating how planar configuration
   *    of color images should be encoded upon decompression.
   *  @param ignoreOffsetTable flag indicating whether to ignore the offset table when decompressing multiframe images
   */
  static void registerCodecs(
    JLS_UIDCreation uidcreation = EJLSUC_default,
    JLS_PlanarConfiguration planarconfig = EJLSPC_restore,
    OFBool ignoreOffsetTable = OFFalse);

  /** deregisters decoders.
   *  Attention: Must not be called while other threads might still use
   *  the registered codecs, e.g. because they are currently decoding
   *  DICOM data sets through dcmdata.
   */
  static void cleanup();

private:

  /// flag indicating whether the decoders are already registered.
  static OFBool registered_;

  /// pointer to codec parameter shared by all decoders
  static DJLSCodecParameter *cp_;

  /// pointer to losslessdecoder
  static DJLSLosslessDecoder *losslessdecoder_;

  /// pointer to near-lossless encoder
  static DJLSNearLosslessDecoder *nearlosslessdecoder_;

};

#endif

/*
 * CVS/RCS Log:
 * $Log: djdecode.h,v $
 * Revision 1.5  2010-10-14 13:17:19  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2010-03-01 10:35:28  uli
 * Renamed include guards to avoid name clash with e.g. dcmjpeg.
 *
 * Revision 1.3  2010-02-25 10:17:14  uli
 * Fix doxygen comments in a couple of places.
 *
 * Revision 1.2  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
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
