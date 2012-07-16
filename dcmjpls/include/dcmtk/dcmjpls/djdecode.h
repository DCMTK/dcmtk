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
 *  Author:  Martin Willkomm
 *
 *  Purpose: singleton class that registers the decoder for all supported JPEG-LS processes.
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
class DCMTK_DCMJPLS_EXPORT DJLSDecoderRegistration
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

  /** get version information of the CharLS library.
   *  Typical output format: "CharLS, Revision 55020 (modified)"
   *  @return name and version number of the CharLS library
   */
  static OFString getLibraryVersionString();

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
