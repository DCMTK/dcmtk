/*
 *
 *  Copyright (C) 2025, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg, based on code by Brad Conte (brad AT bradconte.com) in FreeBSD
 *
 *  Purpose: Implementation of SHA-256
 *
 */

#ifndef OFSHA256_H
#define OFSHA256_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/oftypes.h"


#define SHA256_BLOCK_SIZE 32            // SHA-256 outputs a 32 byte digest

class DCMTK_OFSTD_EXPORT OFSHA256
{
public:
    /// default constructor
    OFSHA256();

    /// initialize instance, i.e. reset to initial state
    void init();

    /** feed bytes to the hash algorithm
     *  @param data bytes to feed
     *  @param len number of bytes
     */
    void update(const void *data, size_t len);

   /** finalize and return hash
    *  @param hash pointer to memory block of at least 32 bytes.
    *    The SHA-256 hash is copied into this block.
    */
    void final(void *hash);

private:

    /// transform hash key state based on the content of the data buffer
    void transform();

    /// data buffer
    Uint8 data_[64];

    /// number of bytes in data buffer
    Uint32 datalen_;

    /// total number of bits that have been fed into the hash algorithm
    unsigned long long bitlen_;

    /// state information for the hash key
    Uint32 state_[8];
};

#endif
