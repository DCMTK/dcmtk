/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for the recommended display value handling of DVPSGraphicLayer
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/cielabutil.h"
#include "dcmtk/dcmpstat/dvpsgl.h"

/* The 16 bit RGB round trip through 16 bit CIELab is not exact: for dark
 * red next to saturated green/blue, one LSB of a*, b* and L* moves R by up
 * to ~39, ~20 and ~8 (of 65535). With rounding errors of up to 0.5 LSB per
 * component the deviation reaches ~33 (observed maximum), 64 leaves room
 * for platform specific floating point differences. This bound does not
 * catch truncation instead of rounding (max. deviation 65); that case is
 * covered by the exact comparison with dcmiod's 8 bit conversion below.
 */
static const int RGB16_TOLERANCE = 64;

/* Maximum absolute difference of two RGB triples */
static int rgbDiff(Uint16 r1, Uint16 g1, Uint16 b1, Uint16 r2, Uint16 g2, Uint16 b2)
{
  int diff = OFstatic_cast(int, r1) - OFstatic_cast(int, r2);
  if (diff < 0) diff = -diff;
  int d = OFstatic_cast(int, g1) - OFstatic_cast(int, g2);
  if (d < 0) d = -d;
  if (d > diff) diff = d;
  d = OFstatic_cast(int, b1) - OFstatic_cast(int, b2);
  if (d < 0) d = -d;
  if (d > diff) diff = d;
  return diff;
}

/* Convert a 16 bit P-value to 8 bit, rounding to nearest */
static Uint16 to8Bit(Uint16 v)
{
  return OFstatic_cast(Uint16, (OFstatic_cast(Uint32, v) + 128) / 257);
}

OFTEST(dcmpstat_graphic_layer_rgb_cielab)
{
  DVPSGraphicLayer gl;
  Uint16 r, g, b, L, a, bb;

  OFCHECK(!gl.haveGLRecommendedDisplayValue());
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).bad());
  OFCHECK(gl.getGLRecommendedDisplayValueCIELab(L, a, bb).bad());
  OFCHECK(gl.getGLRecommendedDisplayValueGray(L).bad());

  // white and black must be encoded exactly (see ICC v4.3 Table 14)
  // and must survive the round trip through CIELab exactly
  gl.setGLRecommendedDisplayValueRGB(65535, 65535, 65535);
  OFCHECK(gl.haveGLRecommendedDisplayValue());
  OFCHECK(gl.getGLRecommendedDisplayValueCIELab(L, a, bb).good());
  OFCHECK_EQUAL(L, 0xFFFF);
  OFCHECK_EQUAL(a, 0x8080);
  OFCHECK_EQUAL(bb, 0x8080);
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK_EQUAL(r, 65535);
  OFCHECK_EQUAL(g, 65535);
  OFCHECK_EQUAL(b, 65535);
  // the L* component is used as the gray value
  OFCHECK(gl.getGLRecommendedDisplayValueGray(L).good());
  OFCHECK_EQUAL(L, 0xFFFF);

  gl.setGLRecommendedDisplayValueRGB(0, 0, 0);
  OFCHECK(gl.getGLRecommendedDisplayValueCIELab(L, a, bb).good());
  OFCHECK_EQUAL(L, 0);
  OFCHECK_EQUAL(a, 0x8080);
  OFCHECK_EQUAL(bb, 0x8080);
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK_EQUAL(r, 0);
  OFCHECK_EQUAL(g, 0);
  OFCHECK_EQUAL(b, 0);

  // CIELab white must decode to RGB white without wrapping around
  gl.setGLRecommendedDisplayValueCIELab(0xFFFF, 0x8080, 0x8080);
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK_EQUAL(r, 65535);
  OFCHECK_EQUAL(g, 65535);
  OFCHECK_EQUAL(b, 65535);

  // CIELab values outside the sRGB gamut must decode to something in range
  // (implicitly true for Uint16) without wrapping around, i.e. a very light
  // color must not become dark and vice versa
  gl.setGLRecommendedDisplayValueCIELab(0xFFFF, 0, 0);       // L* = 100, a* = b* = -128
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK_EQUAL(g, 65535);
  OFCHECK_EQUAL(b, 65535);
  gl.setGLRecommendedDisplayValueCIELab(0, 0x8080, 0x8080);  // L* = 0, a* = b* = 0
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK_EQUAL(r, 0);
  OFCHECK_EQUAL(g, 0);
  OFCHECK_EQUAL(b, 0);

  // For RGB values that are exact multiples of 257 (i.e. 8 bit values scaled
  // to 16 bit), the CIELab encoding must be identical to the one produced by
  // the 8 bit conversion in dcmiod, and the round trip must reproduce the
  // 8 bit value exactly.
  size_t labMismatches = 0;
  size_t rgbMismatches = 0;
  for (unsigned int r8 = 0; r8 < 256; r8 += 15)
  {
    for (unsigned int g8 = 0; g8 < 256; g8 += 15)
    {
      for (unsigned int b8 = 0; b8 < 256; b8 += 15)
      {
        Uint16 L2, a2, b2;
        IODCIELabUtil::rgb2DicomLab(L2, a2, b2, OFstatic_cast(Uint8, r8), OFstatic_cast(Uint8, g8), OFstatic_cast(Uint8, b8));
        gl.setGLRecommendedDisplayValueRGB(OFstatic_cast(Uint16, r8 * 257), OFstatic_cast(Uint16, g8 * 257), OFstatic_cast(Uint16, b8 * 257));
        OFCHECK(gl.getGLRecommendedDisplayValueCIELab(L, a, bb).good());
        if ((L != L2) || (a != a2) || (bb != b2)) labMismatches++;
        OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
        if ((to8Bit(r) != r8) || (to8Bit(g) != g8) || (to8Bit(b) != b8)) rgbMismatches++;
      }
    }
  }
  OFCHECK_EQUAL(labMismatches, 0);
  OFCHECK_EQUAL(rgbMismatches, 0);

  // Round trip of arbitrary 16 bit RGB values through CIELab must stay
  // within the tolerance of the 16 bit CIELab encoding
  int maxDiff = 0;
  for (unsigned int r16 = 0; r16 < 65536; r16 += 1285)
  {
    for (unsigned int g16 = 0; g16 < 65536; g16 += 1285)
    {
      for (unsigned int b16 = 0; b16 < 65536; b16 += 1285)
      {
        gl.setGLRecommendedDisplayValueRGB(OFstatic_cast(Uint16, r16), OFstatic_cast(Uint16, g16), OFstatic_cast(Uint16, b16));
        OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
        const int diff = rgbDiff(r, g, b, OFstatic_cast(Uint16, r16), OFstatic_cast(Uint16, g16), OFstatic_cast(Uint16, b16));
        if (diff > maxDiff) maxDiff = diff;
      }
    }
  }
  OFCHECK(maxDiff <= RGB16_TOLERANCE);

  // Grayscale display value: returned as is, and used for all RGB components
  gl.removeRecommendedDisplayValue(OFTrue, OFTrue);
  OFCHECK(!gl.haveGLRecommendedDisplayValue());
  gl.setGLRecommendedDisplayValueGray(0x8000);
  OFCHECK(gl.haveGLRecommendedDisplayValue());
  OFCHECK(gl.getGLRecommendedDisplayValueGray(L).good());
  OFCHECK_EQUAL(L, 0x8000);
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK_EQUAL(r, 0x8000);
  OFCHECK_EQUAL(g, 0x8000);
  OFCHECK_EQUAL(b, 0x8000);
}

OFTEST(dcmpstat_graphic_layer_read_write)
{
  DVPSGraphicLayer gl;
  Uint16 r, g, b, L, a, bb;
  Uint16 L2, a2, b2;
  IODCIELabUtil::rgb2DicomLab(L2, a2, b2, 255, 0, 0);

  // A graphic layer with the retired RGB display value is converted to
  // CIELab when read
  DcmItem item;
  const Uint16 red[3] = { 65535, 0, 0 };
  OFCHECK(item.putAndInsertString(DcmTag(DCM_GraphicLayer, EVR_CS), "LAYER").good());
  OFCHECK(item.putAndInsertString(DcmTag(DCM_GraphicLayerOrder, EVR_IS), "1").good());
  OFCHECK(item.putAndInsertUint16Array(DcmTag(DCM_RETIRED_GraphicLayerRecommendedDisplayRGBValue, EVR_US), red, 3).good());
  OFCHECK(gl.read(item).good());
  OFCHECK(gl.haveGLRecommendedDisplayValue());
  OFCHECK(gl.getGLRecommendedDisplayValueCIELab(L, a, bb).good());
  OFCHECK_EQUAL(L, L2);
  OFCHECK_EQUAL(a, a2);
  OFCHECK_EQUAL(bb, b2);
  OFCHECK(gl.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK(rgbDiff(r, g, b, 65535, 0, 0) <= RGB16_TOLERANCE);

  // When written, only the CIELab value is stored, and reading it back
  // yields the same values
  DcmItem written;
  OFCHECK(gl.write(written).good());
  OFCHECK(!written.tagExists(DCM_RETIRED_GraphicLayerRecommendedDisplayRGBValue));
  const Uint16* lab = NULL;
  unsigned long count = 0;
  OFCHECK(written.findAndGetUint16Array(DCM_GraphicLayerRecommendedDisplayCIELabValue, lab, &count).good());
  OFCHECK_EQUAL(count, 3);
  if (lab && (count == 3))
  {
    OFCHECK_EQUAL(lab[0], L2);
    OFCHECK_EQUAL(lab[1], a2);
    OFCHECK_EQUAL(lab[2], b2);
  }
  DVPSGraphicLayer gl2;
  OFCHECK(gl2.read(written).good());
  OFCHECK(gl2.getGLRecommendedDisplayValueCIELab(L, a, bb).good());
  OFCHECK_EQUAL(L, L2);
  OFCHECK_EQUAL(a, a2);
  OFCHECK_EQUAL(bb, b2);

  // An existing CIELab value takes precedence over the retired RGB value
  const Uint16 white[3] = { 0xFFFF, 0x8080, 0x8080 };
  OFCHECK(item.putAndInsertUint16Array(DcmTag(DCM_GraphicLayerRecommendedDisplayCIELabValue, EVR_US), white, 3).good());
  DVPSGraphicLayer gl3;
  OFCHECK(gl3.read(item).good());
  OFCHECK(gl3.getGLRecommendedDisplayValueRGB(r, g, b).good());
  OFCHECK_EQUAL(r, 65535);
  OFCHECK_EQUAL(g, 65535);
  OFCHECK_EQUAL(b, 65535);
}
