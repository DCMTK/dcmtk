/*
 *
 *  Copyright (C) 2015-2024, OFFIS e.V.
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: tests for dcmseg's DcmSegUtils functionalities
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmseg/segutils.h"
#include "dcmtk/ofstd/oftest.h"

#define bufLen 4

static void resetBuf(Uint8* buf, const size_t length)
{
    if (length != bufLen)
        return;
    buf[0] = 170; // 10101010
    buf[1] = 204; // 11001100
    buf[2] = 240; // 11110000
    buf[3] = 15;  // 00001111
}

OFTEST(dcmseg_utils)
{
    // buf, always initialized/reset with: 10101010 11001100 11110000 00001111
    Uint8 buf[bufLen];

    resetBuf(buf, bufLen);
    DcmSegUtils::alignFrameOnBitPosition(buf, 4, 2);
    OFCHECK(buf[0] == 168); // 10101000
    OFCHECK(buf[1] == 50);  // 00110010
    OFCHECK(buf[2] == 195); // 11000011
    OFCHECK(buf[3] == 63);  // 00111111

    resetBuf(buf, bufLen);
    DcmSegUtils::alignFrameOnByteBoundary(buf, 4, 2);
    OFCHECK(buf[0] == 42);  // 00101010
    OFCHECK(buf[1] == 51);  // 00110011
    OFCHECK(buf[2] == 252); // 11111100
    OFCHECK(buf[3] == 3);   // 00000011

    resetBuf(buf, bufLen);
    DcmSegUtils::alignFrameOnBitPosition(buf, 4, 7);
    OFCHECK(buf[0] == 0);
    OFCHECK(buf[1] == 85);  // 01010101
    OFCHECK(buf[2] == 102); // 01100110
    OFCHECK(buf[3] == 248); // 11111000

    resetBuf(buf, bufLen);
    DcmSegUtils::alignFrameOnByteBoundary(buf, 4, 7);
    OFCHECK(buf[0] == 153); // 10011001
    OFCHECK(buf[1] == 225); // 11100001
    OFCHECK(buf[2] == 31);  // 00011111
    OFCHECK(buf[3] == 0);
}
