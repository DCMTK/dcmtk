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
 *  Author:     Marco Eichelberg, based on code by Brad Conte (brad AT bradconte.com) in FreeBSD.
 *  Disclaimer: This code is presented "as is" without any guarantees.
 *  Details:    Implementation of the SHA-256 hashing algorithm.
 *              SHA-256 is one of the three algorithms in the SHA2
 *              specification. The others, SHA-384 and SHA-512, are not
 *              offered in this implementation.
 *              Algorithm specification can be found here:
 *              http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
 *              This implementation internally uses little endian byte order.
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofsha256.h"
#include <cstring>

#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

static const Uint32 k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};


void OFSHA256::transform()
{
    Uint32 a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (OFstatic_cast(Uint32, data_[j]) << 24) | (OFstatic_cast(Uint32, data_[j + 1]) << 16) | (OFstatic_cast(Uint32, data_[j + 2]) << 8) | (OFstatic_cast(Uint32, data_[j + 3]));
    for ( ; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = state_[0];
    b = state_[1];
    c = state_[2];
    d = state_[3];
    e = state_[4];
    f = state_[5];
    g = state_[6];
    h = state_[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
    state_[5] += f;
    state_[6] += g;
    state_[7] += h;
}


OFSHA256::OFSHA256()
{
    init();
}

void OFSHA256::init()
{
    datalen_ = 0;
    bitlen_ = 0;
    state_[0] = 0x6a09e667;
    state_[1] = 0xbb67ae85;
    state_[2] = 0x3c6ef372;
    state_[3] = 0xa54ff53a;
    state_[4] = 0x510e527f;
    state_[5] = 0x9b05688c;
    state_[6] = 0x1f83d9ab;
    state_[7] = 0x5be0cd19;
}

void OFSHA256::update(const void *data, size_t len)
{
    if (data == NULL) return;
    const Uint8 *uint8data = OFreinterpret_cast(const Uint8 *, data);
    for (size_t i = 0; i < len; ++i) {
        data_[datalen_] = uint8data[i];
        datalen_++;
        if (datalen_ == 64) {
            transform();
            bitlen_ += 512;
            datalen_ = 0;
        }
    }
}

void OFSHA256::final(void *hash)
{
    if (hash == NULL) return;

    Uint8 *uint8hash = OFreinterpret_cast(Uint8 *, hash);
    Uint32 i, j;
    i = datalen_;

    // Pad whatever data is left in the buffer.
    if (datalen_ < 56) {
        data_[i++] = 0x80;
        while (i < 56)
            data_[i++] = 0x00;
    }
    else {
        data_[i++] = 0x80;
        while (i < 64)
            data_[i++] = 0x00;
        transform();
        memset(data_, 0, 56);
    }

    // Append to the padding the total message's length in bits and transform.
    bitlen_ += datalen_ * 8;
    data_[63] = OFstatic_cast(Uint8, bitlen_);
    data_[62] = OFstatic_cast(Uint8,(bitlen_ >> 8));
    data_[61] = OFstatic_cast(Uint8,(bitlen_ >> 16));
    data_[60] = OFstatic_cast(Uint8,(bitlen_ >> 24));
    data_[59] = OFstatic_cast(Uint8,(bitlen_ >> 32));
    data_[58] = OFstatic_cast(Uint8,(bitlen_ >> 40));
    data_[57] = OFstatic_cast(Uint8,(bitlen_ >> 48));
    data_[56] = OFstatic_cast(Uint8,(bitlen_ >> 56));
    transform();

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    for (i = j = 0; i < 8; ++i, ++j) {
        uint8hash[j++] = (state_[i] >> 24) & 0xff;
        uint8hash[j++] = (state_[i] >> 16) & 0xff;
        uint8hash[j++] = (state_[i] >> 8) & 0xff;
        uint8hash[j]   = state_[i] & 0xff;
    }
}
