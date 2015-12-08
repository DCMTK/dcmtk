/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmseg/segutils.h"

OFTEST(dcmseg_utils)
{
  // buf = 10101010 11001100 11110000 00001111
  Uint8 buf[4];
  buf[0] = 0xAA; // 10101010
  buf[1] = 0xCC; // 11001100
  buf[2] = 0xF0; // 11110000
  buf[3] = 0x0F; // 00001111

  // Shift and check, should be 00101010 10110011 00111000 00000011
  DcmSegUtils::shiftRight(buf, 4, 2);
  OFCHECK(buf[0] == 0x2A); // 00101010 = 42 = 0x2A
  OFCHECK(buf[1] == 0xB3); // 10110011 = 179 = 0xB3
  OFCHECK(buf[2] == 0x3C); // 00111100 = 60 = 0x3C
  OFCHECK(buf[3] == 0x03); // 00000011 = 0 = 0x03

  // Shift and check, should be 10101010 11001100 11110000 00001100
  DcmSegUtils::shiftLeft(buf, 4, 2);
  OFCHECK(buf[0] == 0xAA); // 10101010
  OFCHECK(buf[1] == 0xCC); // 11001100
  OFCHECK(buf[2] == 0xF0); // 11110000
  OFCHECK(buf[3] == 0x0C); // 00001100

  // Shift and check, should be 00000001 01010101 10011001 11100000
  DcmSegUtils::shiftRight(buf, 4, 7);
  OFCHECK(buf[0] == 0x01); // 00000001
  OFCHECK(buf[1] == 0x55); // 01010101
  OFCHECK(buf[2] == 0x99); // 10011001
  OFCHECK(buf[3] == 0xE0); // 11100000

  // Shift and check, should be 10101010 11001100 11110000 00000000
  DcmSegUtils::shiftLeft(buf, 4, 7);
  OFCHECK(buf[0] == 0xAA); // 10101010
  OFCHECK(buf[1] == 0xCC); // 11001100
  OFCHECK(buf[2] == 0xF0); // 11110000
  OFCHECK(buf[3] == 0x00); // 00000000

}
