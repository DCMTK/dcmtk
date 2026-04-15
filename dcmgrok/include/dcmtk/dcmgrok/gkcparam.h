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
 *  Purpose: codec parameter class for Grok JPEG 2000 codecs
 *
 */

#ifndef DCMGROK_GKCPARAM_H
#define DCMGROK_GKCPARAM_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecParameter */
#include "dcmtk/dcmgrok/gkutils.h"
#include "dcmtk/dcmgrok/gkdefine.h"

/** codec parameter for Grok JPEG 2000 codecs
 */
class DCMTK_DCMGROK_EXPORT DGRKCodecParameter : public DcmCodecParameter
{
public:

    /** constructor, for use with decoders.
     *  @param uidCreation mode for SOP Instance UID creation
     *  @param planarConfiguration flag describing how planar configuration
     *    of decompressed color images should be handled
     *  @param ignoreOffsetTable flag indicating whether to ignore the offset
     *    table when decompressing multiframe images
     *  @param numThreads number of threads for Grok decompression (0 = all CPUs)
     */
    DGRKCodecParameter(
        GRK_UIDCreation uidCreation = EGRKUC_default,
        GRK_PlanarConfiguration planarConfiguration = EGRKPC_restore,
        OFBool ignoreOffsetTable = OFFalse,
        Uint32 numThreads = 0);

    /// copy constructor
    DGRKCodecParameter(const DGRKCodecParameter& arg);

    /// destructor
    virtual ~DGRKCodecParameter();

    /** this methods creates a copy of type DcmCodecParameter *
     *  it must be overwritten in every subclass.
     *  @return copy of this object
     */
    virtual DcmCodecParameter *clone() const;

    /** returns the class name as string.
     *  can be used as poor man's RTTI replacement.
     */
    virtual const char *className() const;

    /** returns UID creation flag
     *  @return UID creation flag
     */
    GRK_UIDCreation getUIDCreation() const { return uidCreation_; }

    /** returns planar configuration flag
     *  @return planar configuration flag
     */
    GRK_PlanarConfiguration getPlanarConfiguration() const { return planarConfiguration_; }

    /** returns ignore offset table flag
     *  @return ignore offset table flag
     */
    OFBool ignoreOffsetTable() const { return ignoreOffsetTable_; }

    /** returns number of threads for decompression
     *  @return number of threads (0 = all CPUs)
     */
    Uint32 getNumThreads() const { return numThreads_; }

private:

    /// flag indicating how SOP Instance UIDs should be handled
    GRK_UIDCreation uidCreation_;

    /// flag indicating how planar configuration should be handled
    GRK_PlanarConfiguration planarConfiguration_;

    /// flag indicating whether to ignore the offset table
    OFBool ignoreOffsetTable_;

    /// number of threads for Grok decompression
    Uint32 numThreads_;
};

#endif
