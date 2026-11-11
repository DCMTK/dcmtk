/*
 *
 *  Copyright (C) 2017-2026, OFFIS e.V.
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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for dcmiod's color conversion functionalities
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmiod/cielabutil.h"
#include "dcmtk/ofstd/ofrand.h"
#include "dcmtk/ofstd/oftest.h"

/** Reference colors: sRGB (8 bit) and the corresponding D50 CIELab (ICC PCS)
 *  values, as computed by the CSS Color Module Level 4 sample code.
 *  The first part are CSS named colors, the second part are colors from
 *  3D Slicer's "General Anatomy" color table.
 */
struct ReferenceColor
{
    const char* name;
    Uint8 r, g, b;
    double L, a, bb;
};

static const ReferenceColor referenceColors[] = {
    { "black",          0,   0,   0,   0.000,   0.000,    0.000 },
    { "white",          255, 255, 255, 100.000, 0.000,    0.000 },
    { "gray",           128, 128, 128, 53.585,  0.000,    0.000 },
    { "silver",         192, 192, 192, 77.704,  0.000,    0.000 },
    { "red",            255, 0,   0,   54.291,  80.805,   69.891 },
    { "lime",           0,   255, 0,   87.819,  -79.271,  80.995 },
    { "blue",           0,   0,   255, 29.568,  68.287,   -112.030 },
    { "yellow",         255, 255, 0,   97.607,  -15.750,  93.394 },
    { "cyan",           0,   255, 255, 90.666,  -50.656,  -14.962 },
    { "magenta",        255, 0,   255, 60.169,  93.540,   -60.501 },
    { "maroon",         128, 0,   0,   26.165,  48.473,   39.439 },
    { "navy",           0,   0,   128, 11.335,  40.964,   -67.203 },
    { "olive",          128, 128, 0,   52.150,  -9.448,   56.024 },
    { "teal",           0,   128, 128, 47.986,  -30.387,  -8.975 },
    { "purple",         128, 0,   128, 29.692,  56.112,   -36.293 },
    { "orange",         255, 165, 0,   75.590,  27.516,   79.121 },
    { "cornflowerblue", 100, 149, 237, 61.233,  3.047,    -50.188 },
    { "darkslategray",  47,  79,  79,  31.141,  -12.261,  -3.937 },
    { "hotpink",        255, 105, 180, 65.860,  63.258,   -9.644 },
    { "indigo",         75,  0,   130, 19.715,  47.029,   -54.278 },
    { "lavender",       230, 230, 250, 91.742,  2.775,    -9.724 },
    { "midnightblue",   25,  25,  112, 14.929,  25.955,   -50.904 },
    { "peru",           205, 133, 63,  62.253,  23.948,   48.413 },
    { "springgreen",    0,   255, 127, 88.436,  -72.499,  45.977 },
    { "CSF space",      85,  188, 255, 72.318,  -15.277,  -42.679 },
    { "aorta",          224, 97,  76,  57.648,  49.583,   37.620 },
    { "bile",           0,   145, 30,  52.261,  -51.068,  46.791 },
    { "bone",           241, 214, 145, 86.748,  2.835,    37.696 },
    { "brain",          250, 250, 225, 97.780,  -3.106,   12.048 },
    { "capillary",      183, 156, 220, 68.539,  19.645,   -28.946 },
    { "cartilage",      111, 184, 210, 70.737,  -18.519,  -20.706 },
    { "gray matter",    200, 200, 235, 81.452,  5.183,    -17.387 },
    { "liver",          221, 130, 101, 64.063,  33.878,   31.516 },
    { "muscle",         192, 104, 88,  54.284,  34.766,   25.456 },
    { "pancreas",       249, 180, 111, 78.903,  20.532,   45.266 },
    { "skin",           177, 122, 101, 56.608,  20.206,   20.612 },
    { "spleen",         157, 108, 162, 52.334,  27.009,   -21.229 },
    { "thyroid gland",  62,  162, 114, 59.883,  -39.102,  16.066 },
    { "vein",           0,   151, 206, 57.968,  -19.204,  -38.353 },
    { "white matter",   250, 250, 210, 97.515,  -4.805,   19.293 }
};

/** Check that an 8 bit RGB triple survives the round trip through the
 *  16 bit DICOM CIELab encoding without any change
 *  @param  r red component
 *  @param  g green component
 *  @param  b blue component
 *  @return OFTrue if round trip was exact, OFFalse otherwise
 */
static OFBool checkRoundTrip8(Uint8 r, Uint8 g, Uint8 b)
{
    Uint16 L, a, bb;
    Uint8 r2, g2, b2;
    IODCIELabUtil::rgb2DicomLab(L, a, bb, r, g, b);
    IODCIELabUtil::dicomLab2RGB(r2, g2, b2, L, a, bb);
    return (r == r2) && (g == g2) && (b == b2);
}

/** Check an 8 bit RGB result against the expected values
 *  @param  what description of the test case
 *  @param  R red component
 *  @param  G green component
 *  @param  B blue component
 *  @param  eR expected red component
 *  @param  eG expected green component
 *  @param  eB expected blue component
 */
static void checkRGB8(const char* what, Uint8 R, Uint8 G, Uint8 B, unsigned int eR, unsigned int eG, unsigned int eB)
{
    if ((R != eR) || (G != eG) || (B != eB))
    {
        OFCHECK_FAIL("RGB mismatch for " << what << ": got " << OFstatic_cast(unsigned int, R) << ","
            << OFstatic_cast(unsigned int, G) << "," << OFstatic_cast(unsigned int, B)
            << ", expected " << eR << "," << eG << "," << eB);
    }
}

/** Check a 16 bit DICOM CIELab result against the expected values
 *  @param  what description of the test case
 *  @param  L luminance component
 *  @param  a "a" component
 *  @param  b "b" component
 *  @param  eL expected luminance component
 *  @param  ea expected "a" component
 *  @param  eb expected "b" component
 */
static void checkLab16(const char* what, Uint16 L, Uint16 a, Uint16 b, unsigned int eL, unsigned int ea, unsigned int eb)
{
    if ((L != eL) || (a != ea) || (b != eb))
    {
        OFCHECK_FAIL("DICOM CIELab mismatch for " << what << ": got " << L << "," << a << "," << b
            << ", expected " << eL << "," << ea << "," << eb);
    }
}

OFTEST(dcmiod_tcielabutil)
{
    // Result variables for testing
    double r1, r2, r3;
    r1 = r2 = r3 = 0.0;

    // sRGB white maps exactly onto the D50 white point of the ICC PCS
    IODCIELabUtil::rgb2Lab(r1, r2, r3 /* L,a,b */, 1, 1, 1);
    OFCHECK(fabs(100 - r1) < 0.000001);
    OFCHECK(fabs(0 - r2) < 0.000001);
    OFCHECK(fabs(0 - r3) < 0.000001);
    r1 = r2 = r3 = 0;
    IODCIELabUtil::rgb2Xyz(r1, r2, r3 /* X,Y,Z */, 1, 1, 1);
    OFCHECK(fabs(IODCIELabUtil::D50_WHITEPOINT_X - r1) < 0.000001);
    OFCHECK(fabs(IODCIELabUtil::D50_WHITEPOINT_Y - r2) < 0.000001);
    OFCHECK(fabs(IODCIELabUtil::D50_WHITEPOINT_Z - r3) < 0.000001);

    // Test RGB -> CIELab conversion with medium values (neutral gray, a = b = 0)
    r1 = r2 = r3 = 0;
    IODCIELabUtil::rgb2Lab(r1, r2, r3 /* L,a,b */, 0.5, 0.5, 0.5);
    OFCHECK(fabs(53.389 - r1) < 0.001);
    OFCHECK(fabs(0 - r2) < 0.001);
    OFCHECK(fabs(0 - r3) < 0.001);

    // Test RGB -> CIEXYZ (D50) conversion with medium values
    r1 = r2 = r3 = 0;
    IODCIELabUtil::rgb2Xyz(r1, r2, r3 /* X,Y,Z */, 0.5, 0.5, 0.5);
    OFCHECK(fabs(0.206 - r1) < 0.001);
    OFCHECK(fabs(0.214 - r2) < 0.001);
    OFCHECK(fabs(0.177 - r3) < 0.001);

    // Test RGB -> CIELab conversion with minimum value
    r1 = r2 = r3 = 0;
    IODCIELabUtil::rgb2Lab(r1, r2, r3 /* L,a,b */, 0, 0, 0);
    OFCHECK(fabs(0 - r1) < 0.001);
    OFCHECK(fabs(0 - r2) < 0.001);
    OFCHECK(fabs(0 - r3) < 0.001);

    // Test RGB -> XYZ conversion with minimum values
    r1 = r2 = r3 = 0;
    IODCIELabUtil::rgb2Xyz(r1, r2, r3 /* X,Y,Z */, 0, 0, 0);
    OFCHECK(fabs(0 - r1) < 0.001);
    OFCHECK(fabs(0 - r2) < 0.001);
    OFCHECK(fabs(0 - r3) < 0.001);

    // Test XYZ (D50 white point, ICC v4.3 Table 14) -> RGB conversion
    r1 = r2 = r3 = 0;
    IODCIELabUtil::xyz2Rgb(r1, r2, r3 /* R,G,B */, 0.96422, 1.0, 0.82521);
    OFCHECK(fabs(1 - r1) < 0.001);
    OFCHECK(fabs(1 - r2) < 0.001);
    OFCHECK(fabs(1 - r3) < 0.001);

    // Test DICOM CIELab to "normalized" CIELab conversion with maximum values
    r1 = r2 = r3 = 0;
    IODCIELabUtil::dicomlab2Lab(r1, r2, r3, 65535, 65535, 65535);
    OFCHECK(fabs(100 - r1) < 0.001);
    OFCHECK(fabs(127 - r2) < 0.001);
    OFCHECK(fabs(127 - r3) < 0.001);

    // Test DICOM CIELab to "normalized" CIELab conversion with minimum values
    r1 = r2 = r3 = 0;
    IODCIELabUtil::dicomlab2Lab(r1, r2, r3, 0, 0, 0);
    OFCHECK(fabs(0 - r1) < 0.001);
    OFCHECK(fabs(-128 - r2) < 0.001);
    OFCHECK(fabs(-128 - r3) < 0.001);

    // Test CIELab white point -> DICOM CIELab (ICC v4.3 Table 14)
    r1 = r2 = r3 = 0;
    IODCIELabUtil::lab2DicomLab(r1, r2, r3, 100, 0, 0);
    OFCHECK(fabs(0xFFFF - r1) < 0.000001);
    OFCHECK(fabs(0x8080 - r2) < 0.000001);
    OFCHECK(fabs(0x8080 - r3) < 0.000001);

    // Test integer conversions: white and black (see also ICC v4.3 Table 14)
    Uint16 L, a, b;
    Uint8 R, G, B;
    IODCIELabUtil::rgb2DicomLab(L, a, b, 255, 255, 255);
    checkLab16("white", L, a, b, 0xFFFF, 0x8080, 0x8080);
    IODCIELabUtil::dicomLab2RGB(R, G, B, 0xFFFF, 0x8080, 0x8080);
    checkRGB8("white", R, G, B, 255, 255, 255);
    IODCIELabUtil::rgb2DicomLab(L, a, b, 0, 0, 0);
    checkLab16("black", L, a, b, 0, 0x8080, 0x8080);
    IODCIELabUtil::dicomLab2RGB(R, G, B, 0, 0x8080, 0x8080);
    checkRGB8("black", R, G, B, 0, 0, 0);

    // Test integer conversion of specific values (consistent with PixelMed)
    IODCIELabUtil::dicomLab2RGB(R, G, B, 35732, 48892, 14692);
    checkRGB8("Lab 35732/48892/14692", R, G, B, 181, 82, 255);
    IODCIELabUtil::dicomLab2RGB(R, G, B, 0, 0x8000, 0x8000);
    checkRGB8("Lab 0/32768/32768", R, G, B, 0, 0, 1);

    // Test clipping of colors outside the sRGB gamut: results must stay
    // within the valid range (and not wrap around in the integer variant)
    const double labCorners[4][3] = { { 0, 0, 0 }, { 65535, 65535, 65535 }, { 65535, 0, 0 }, { 0, 65535, 65535 } };
    for (size_t i = 0; i < 4; i++)
    {
        r1 = r2 = r3 = -1;
        IODCIELabUtil::dicomLab2RGB(r1, r2, r3, labCorners[i][0], labCorners[i][1], labCorners[i][2]);
        OFCHECK((r1 >= 0) && (r1 <= 1));
        OFCHECK((r2 >= 0) && (r2 <= 1));
        OFCHECK((r3 >= 0) && (r3 <= 1));
    }
    r1 = r2 = r3 = -1;
    IODCIELabUtil::lab2Rgb(r1, r2, r3, 50, 127, -128);
    OFCHECK((r1 >= 0) && (r1 <= 1));
    OFCHECK((r2 >= 0) && (r2 <= 1));
    OFCHECK((r3 >= 0) && (r3 <= 1));
    // saturated blue that is (slightly) outside the sRGB gamut in linear RGB
    IODCIELabUtil::dicomLab2RGB(R, G, B, 19378, 50557, 3680);
    checkRGB8("Lab 19378/50557/3680", R, G, B, 0, 0, 255);

    // Test reference colors: RGB -> CIELab must match the D50 reference
    // values, and the 8 bit round trip must be exact
    const size_t numRefColors = sizeof(referenceColors) / sizeof(referenceColors[0]);
    for (size_t i = 0; i < numRefColors; i++)
    {
        const ReferenceColor& c = referenceColors[i];
        r1 = r2 = r3 = 0;
        IODCIELabUtil::rgb2Lab(r1, r2, r3, c.r / 255.0, c.g / 255.0, c.b / 255.0);
        if ((fabs(c.L - r1) >= 0.03) || (fabs(c.a - r2) >= 0.03) || (fabs(c.bb - r3) >= 0.03))
        {
            OFCHECK_FAIL("CIELab mismatch for reference color " << c.name << ": got "
                << r1 << "," << r2 << "," << r3 << ", expected " << c.L << "," << c.a << "," << c.bb);
        }
        if (!checkRoundTrip8(c.r, c.g, c.b))
        {
            OFCHECK_FAIL("8 bit RGB round trip not exact for reference color " << c.name);
        }
        // Reference CIELab -> RGB must yield the reference RGB. The reference
        // values are rounded to 3 decimals, which may shift a component that
        // lies exactly on a gamut boundary by one, so accept a deviation of 1.
        r1 = r2 = r3 = 0;
        IODCIELabUtil::lab2Rgb(r1, r2, r3, c.L, c.a, c.bb);
        const long rr = OFstatic_cast(long, floor(r1 * 255.0 + 0.5));
        const long gg = OFstatic_cast(long, floor(r2 * 255.0 + 0.5));
        const long bl = OFstatic_cast(long, floor(r3 * 255.0 + 0.5));
        if ((labs(rr - c.r) > 1) || (labs(gg - c.g) > 1) || (labs(bl - c.b) > 1))
        {
            OFCHECK_FAIL("RGB mismatch for reference color " << c.name << ": got "
                << rr << "," << gg << "," << bl << ", expected " << OFstatic_cast(int, c.r) << ","
                << OFstatic_cast(int, c.g) << "," << OFstatic_cast(int, c.b));
        }
    }

    // Test specific RGB values (consistent with PixelMed's test suite) for an
    // exact 8 bit round trip
    const Uint8 specificRGB[][3] = { { 0, 0, 0 }, { 255, 0, 0 }, { 0, 255, 0 }, { 0, 0, 255 }, { 255, 255, 0 },
        { 0, 255, 255 }, { 255, 0, 255 }, { 255, 255, 255 }, { 225, 190, 150 }, { 200, 200, 200 },
        { 128, 174, 128 }, { 221, 130, 101 }, { 0x51, 0x5d, 0xe5 }, { 0x4c, 0x6e, 0xda } };
    for (size_t i = 0; i < sizeof(specificRGB) / sizeof(specificRGB[0]); i++)
    {
        if (!checkRoundTrip8(specificRGB[i][0], specificRGB[i][1], specificRGB[i][2]))
        {
            OFCHECK_FAIL("8 bit RGB round trip not exact for " << OFstatic_cast(int, specificRGB[i][0]) << ","
                << OFstatic_cast(int, specificRGB[i][1]) << "," << OFstatic_cast(int, specificRGB[i][2]));
        }
    }

    // Test DICOM CIELab -> 8 bit RGB -> DICOM CIELab (consistent with PixelMed's
    // test suite). This direction cannot be exact since one 8 bit RGB step
    // corresponds to up to about 100 DICOM CIELab steps.
    const Uint16 specificLab[][3] = { { 29333, 40332, 14387 }, { 31660, 36600, 17469 }, { 33481, 31446, 22863 } };
    for (size_t i = 0; i < sizeof(specificLab) / sizeof(specificLab[0]); i++)
    {
        IODCIELabUtil::dicomLab2RGB(R, G, B, specificLab[i][0], specificLab[i][1], specificLab[i][2]);
        IODCIELabUtil::rgb2DicomLab(L, a, b, R, G, B);
        if ((labs(OFstatic_cast(long, L) - specificLab[i][0]) >= 100) || (labs(OFstatic_cast(long, a) - specificLab[i][1]) >= 100)
            || (labs(OFstatic_cast(long, b) - specificLab[i][2]) >= 100))
        {
            OFCHECK_FAIL("DICOM CIELab round trip deviates too much for " << specificLab[i][0] << ","
                << specificLab[i][1] << "," << specificLab[i][2] << ": got " << L << "," << a << "," << b);
        }
    }

    // Convert between colorspaces back and forth and check whether deviation after
    // roundtrip is less than around 1 promille

    // Initialize random numbers
    OFRandom rnd;

    // We do 1000 runs
    for (size_t numRun = 0; numRun < 1000; numRun++)
    {
        // i1, i2, i3: Input for conversion
        // r1, r2, r3: Results of conversion
        // o1, o2, o3: Output of inverse conversion (should equal i1, i2, i3)
        double i1, i2, i3, o1, o2, o3;
        i1 = i2 = i3 = r1 = r2 = r3 = o1 = o2 = o3 = 0.0;

        // Roundtrip RGB -> CIELab -> RGB
        i1 = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1);
        i2 = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1);
        i3 = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1);
        IODCIELabUtil::rgb2Lab(r1, r2, r3, i1, i2, i3);
        IODCIELabUtil::lab2Rgb(o1, o2, o3, r1, r2, r3);
        OFCHECK(fabs(i1 - o1) < 0.001);
        OFCHECK(fabs(i2 - o2) < 0.001);
        OFCHECK(fabs(i3 - o3) < 0.001);

        // Roundtrip CIELab -> DICOM CIELab -> CIELab
        i1 = i2 = i3 = r1 = r2 = r3 = o1 = o2 = o3 = 0.0;
        i1 = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1) * 100.0;
        i2 = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1) * 255.0 - 128;
        i3 = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1) * 255.0 - 128;
        IODCIELabUtil::lab2DicomLab(r1, r2, r3, i1, i2, i3);
        IODCIELabUtil::dicomlab2Lab(o1, o2, o3, r1, r2, r3);
        OFCHECK(fabs(i1 - o1) < 0.001);
        OFCHECK(fabs(i2 - o2) < 0.001);
        OFCHECK(fabs(i3 - o3) < 0.001);

        // Roundtrip RGB -> CIEXYZ -> CIELab -> dicomCIELab -> RGB
        i1 = i2 = i3 = r1 = r2 = r3 = o1 = o2 = o3 = 0.0;
        i1                                         = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1);
        i2                                         = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1);
        i3                                         = OFstatic_cast(double, rnd.getRND32()) / OFstatic_cast(Uint32, -1);
        IODCIELabUtil::rgb2Xyz(r1, r2, r3, i1, i2, i3);
        IODCIELabUtil::xyz2Lab(o1, o2, o3, r1, r2, r3);
        IODCIELabUtil::lab2DicomLab(r1, r2, r3, o1, o2, o3);
        IODCIELabUtil::dicomLab2RGB(o1, o2, o3, r1, r2, r3);

        OFCHECK(fabs(i1 - o1) < 0.001);
        OFCHECK(fabs(i2 - o2) < 0.001);
        OFCHECK(fabs(i3 - o3) < 0.001);
    }

    // 8 bit RGB -> 16 bit DICOM CIELab -> 8 bit RGB must be exact for a
    // sample of the RGB cube (the full cube is covered by the slow test)
    size_t mismatches = 0;
    for (unsigned int r = 0; r < 256; r += 5)
    {
        for (unsigned int g = 0; g < 256; g += 5)
        {
            for (unsigned int bl = 0; bl < 256; bl += 5)
            {
                if (!checkRoundTrip8(OFstatic_cast(Uint8, r), OFstatic_cast(Uint8, g), OFstatic_cast(Uint8, bl)))
                    mismatches++;
            }
        }
    }
    OFCHECK_EQUAL(mismatches, 0);
}

OFTEST_FLAGS(dcmiod_tcielabutil_roundtrip_exhaustive, EF_Slow)
{
    // 8 bit RGB -> 16 bit DICOM CIELab -> 8 bit RGB must be exact for
    // every one of the 16777216 RGB triples
    size_t mismatches = 0;
    for (unsigned int r = 0; r < 256; r++)
    {
        for (unsigned int g = 0; g < 256; g++)
        {
            for (unsigned int b = 0; b < 256; b++)
            {
                if (!checkRoundTrip8(OFstatic_cast(Uint8, r), OFstatic_cast(Uint8, g), OFstatic_cast(Uint8, b)))
                    mismatches++;
            }
        }
    }
    OFCHECK_EQUAL(mismatches, 0);
}
