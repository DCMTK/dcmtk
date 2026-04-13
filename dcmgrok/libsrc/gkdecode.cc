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
 *  Purpose: singleton class that registers Grok JPEG 2000 decoders.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmgrok/gkdecode.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/dcmgrok/gkcparam.h"
#include "dcmtk/dcmgrok/gkcodecd.h"

#include <grok.h>

/* initialization of static members */
OFBool DGRKDecoderRegistration::registered_                       = OFFalse;
DGRKCodecParameter *DGRKDecoderRegistration::cp_                  = NULL;
DGRKJ2KLosslessDecoder *DGRKDecoderRegistration::j2kLosslessDecoder_   = NULL;
DGRKJ2KDecoder *DGRKDecoderRegistration::j2kDecoder_              = NULL;
DGRKHTJ2KLosslessDecoder *DGRKDecoderRegistration::htj2kLosslessDecoder_ = NULL;
DGRKHTJ2KDecoder *DGRKDecoderRegistration::htj2kDecoder_          = NULL;


void DGRKDecoderRegistration::registerCodecs(
    GRK_UIDCreation uidCreation,
    GRK_PlanarConfiguration planarConfiguration,
    OFBool ignoreOffsetTable,
    Uint32 numThreads)
{
    if (!registered_)
    {
        /* initialize Grok library */
        grk_initialize(NULL, numThreads, NULL);

        cp_ = new DGRKCodecParameter(uidCreation, planarConfiguration,
                                     ignoreOffsetTable, numThreads);
        if (cp_)
        {
            /* JPEG 2000 Lossless Only */
            j2kLosslessDecoder_ = new DGRKJ2KLosslessDecoder();
            if (j2kLosslessDecoder_)
                DcmCodecList::registerCodec(j2kLosslessDecoder_, NULL, cp_);

            /* JPEG 2000 (lossy) */
            j2kDecoder_ = new DGRKJ2KDecoder();
            if (j2kDecoder_)
                DcmCodecList::registerCodec(j2kDecoder_, NULL, cp_);

            /* HTJ2K Lossless Only */
            htj2kLosslessDecoder_ = new DGRKHTJ2KLosslessDecoder();
            if (htj2kLosslessDecoder_)
                DcmCodecList::registerCodec(htj2kLosslessDecoder_, NULL, cp_);

            /* HTJ2K (lossy) */
            htj2kDecoder_ = new DGRKHTJ2KDecoder();
            if (htj2kDecoder_)
                DcmCodecList::registerCodec(htj2kDecoder_, NULL, cp_);

            registered_ = OFTrue;
        }
    }
}


void DGRKDecoderRegistration::cleanup()
{
    if (registered_)
    {
        DcmCodecList::deregisterCodec(j2kLosslessDecoder_);
        DcmCodecList::deregisterCodec(j2kDecoder_);
        DcmCodecList::deregisterCodec(htj2kLosslessDecoder_);
        DcmCodecList::deregisterCodec(htj2kDecoder_);
        delete j2kLosslessDecoder_;
        delete j2kDecoder_;
        delete htj2kLosslessDecoder_;
        delete htj2kDecoder_;
        delete cp_;
        registered_ = OFFalse;

        /* deinitialize Grok library */
        grk_deinitialize();

#ifdef DEBUG
        j2kLosslessDecoder_ = NULL;
        j2kDecoder_ = NULL;
        htj2kLosslessDecoder_ = NULL;
        htj2kDecoder_ = NULL;
        cp_ = NULL;
#endif
    }
}


OFString DGRKDecoderRegistration::getLibraryVersionString()
{
    OFString result = "Grok JPEG 2000 codec, version ";
    result += grk_version();
    return result;
}
