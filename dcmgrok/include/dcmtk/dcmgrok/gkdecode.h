/*
 *
 *  Copyright (C) 2026, Grok Image Compression Inc.
 *  All rights reserved.
 *
 *  This software and supporting documentation were developed by
 *  Grok Image Compression Inc.
 *
 *  Module:  dcmgrok
 *
 *  Author:  Aaron Boxer
 *
 *  Purpose: singleton class that registers decoders for
 *    JPEG 2000 and HTJ2K transfer syntaxes via the Grok library.
 *
 */

#ifndef DCMGROK_GKDECODE_H
#define DCMGROK_GKDECODE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmgrok/gkutils.h"
#include "dcmtk/dcmgrok/gkdefine.h"

class DGRKCodecParameter;
class DGRKJ2KLosslessDecoder;
class DGRKJ2KDecoder;
class DGRKHTJ2KLosslessDecoder;
class DGRKHTJ2KDecoder;

/** singleton class that registers decoders for all Grok-supported
 *  JPEG 2000 transfer syntaxes.
 */
class DCMTK_DCMGROK_EXPORT DGRKDecoderRegistration
{
public:

    /** registers decoders for JPEG 2000 and HTJ2K transfer syntaxes.
     *  If already registered, call is ignored unless cleanup() has
     *  been performed before.
     *  @param uidCreation flag indicating whether or not
     *    a new SOP Instance UID should be assigned upon decompression.
     *  @param planarConfiguration flag indicating how planar configuration
     *    of color images should be handled upon decompression.
     *  @param ignoreOffsetTable flag indicating whether to ignore the
     *    offset table when decompressing multiframe images
     *  @param numThreads number of threads to use for Grok decompression
     *    (0 = use all available CPUs)
     */
    static void registerCodecs(
        GRK_UIDCreation uidCreation = EGRKUC_default,
        GRK_PlanarConfiguration planarConfiguration = EGRKPC_restore,
        OFBool ignoreOffsetTable = OFFalse,
        Uint32 numThreads = 0);

    /** deregisters decoders.
     *  Attention: Must not be called while other threads might still use
     *  the registered codecs.
     */
    static void cleanup();

    /** get version information of the Grok library.
     *  @return name and version number of the Grok library
     */
    static OFString getLibraryVersionString();

private:

    /// flag indicating whether the decoders are already registered
    static OFBool registered_;

    /// pointer to codec parameter shared by all decoders
    static DGRKCodecParameter *cp_;

    /// pointer to JPEG 2000 lossless decoder
    static DGRKJ2KLosslessDecoder *j2kLosslessDecoder_;

    /// pointer to JPEG 2000 lossy decoder
    static DGRKJ2KDecoder *j2kDecoder_;

    /// pointer to HTJ2K lossless decoder
    static DGRKHTJ2KLosslessDecoder *htj2kLosslessDecoder_;

    /// pointer to HTJ2K lossy decoder
    static DGRKHTJ2KDecoder *htj2kDecoder_;
};

#endif
