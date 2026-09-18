/*
 *
 *  Copyright (C) 2005-2026, Pascal Getreuer, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmiod
 *
 *  Author:  Pascal Getreuer, Michael Onken
 *
 *  Purpose: Static helper functionality for CIE<->RGB color conversions
 *
 */
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/cielabutil.h"
#include "dcmtk/ofstd/ofstdinc.h" /* for STD_NAMESPACE */
#include "dcmtk/ofstd/ofcast.h"

// workaround for SunPro not defining these C functions in the global namespace
#ifdef __sun
using STD_NAMESPACE pow;
using STD_NAMESPACE cbrt;
#endif

// White points, defined by their 4-figure CIE x,y chromaticities as in the
// CSS Color Module Level 4 sample code. The D50 white point is the ICC PCS
// illuminant and thus the reference white for CIELab values in DICOM. The
// D65 white point is the native white point of sRGB (and identical to the row
// sums of the linear sRGB to XYZ matrix used in rgb2Xyz()).
const double IODCIELabUtil::D50_WHITEPOINT_X = 0.3457 / 0.3585;
const double IODCIELabUtil::D50_WHITEPOINT_Y = 1.0;
const double IODCIELabUtil::D50_WHITEPOINT_Z = (1.0 - 0.3457 - 0.3585) / 0.3585;
const double IODCIELabUtil::D65_WHITEPOINT_X = 0.3127 / 0.3290;
const double IODCIELabUtil::D65_WHITEPOINT_Y = 1.0;
const double IODCIELabUtil::D65_WHITEPOINT_Z = (1.0 - 0.3127 - 0.3290) / 0.3290;

void IODCIELabUtil::dicomLab2RGB(double& R, double& G, double& B, double LDicom, double aDicom, double bDicom)
{
    double L, a, b;
    dicomlab2Lab(L, a, b, LDicom, aDicom, bDicom);
    lab2Rgb(R, G, B, L, a, b);
}

void IODCIELabUtil::dicomLab2RGB(Uint8& R, Uint8& G, Uint8& B, Uint16 LDicom, Uint16 aDicom, Uint16 bDicom)
{
    double dR, dG, dB;
    dicomLab2RGB(dR, dG, dB, LDicom, aDicom, bDicom);
    R = OFstatic_cast(Uint8, roundClamp(dR * 255.0, 255.0));
    G = OFstatic_cast(Uint8, roundClamp(dG * 255.0, 255.0));
    B = OFstatic_cast(Uint8, roundClamp(dB * 255.0, 255.0));
}

void IODCIELabUtil::rgb2DicomLab(double& LDicom, double& aDicom, double& bDicom, double R, double G, double B)
{
    double L, a, b;
    rgb2Lab(L, a, b, R, G, B);
    lab2DicomLab(LDicom, aDicom, bDicom, L, a, b);
}

void IODCIELabUtil::rgb2DicomLab(Uint16& LDicom, Uint16& aDicom, Uint16& bDicom, Uint8 R, Uint8 G, Uint8 B)
{
    double dL, da, db;
    rgb2DicomLab(dL, da, db, R / 255.0, G / 255.0, B / 255.0);
    LDicom = OFstatic_cast(Uint16, roundClamp(dL, 65535.0));
    aDicom = OFstatic_cast(Uint16, roundClamp(da, 65535.0));
    bDicom = OFstatic_cast(Uint16, roundClamp(db, 65535.0));
}

void IODCIELabUtil::dicomlab2Lab(double& L, double& a, double& b, double LDicom, double aDicom, double bDicom)
{
    L = ((LDicom * 100.0) / 65535.0);       // results in 0 <= L <= 100
    a = ((aDicom * 255.0) / 65535.0) - 128; // results in -128 <= a <= 127
    b = ((bDicom * 255.0) / 65535.0) - 128; // results in -128 <= b <= 127
}

void IODCIELabUtil::lab2DicomLab(double& LDicom, double& aDicom, double& bDicom, double L, double a, double b)
{
    LDicom = L * 65535.0 / 100.0;         // results in 0 <= L <= 65535
    aDicom = (a + 128) * 65535.0 / 255.0; // results in 0 <= a <= 65535
    bDicom = (b + 128) * 65535.0 / 255.0; // results in 0 <= b <= 65535
}

void IODCIELabUtil::rgb2Lab(double& L, double& a, double& b, double R, double G, double B)
{
    double X, Y, Z;
    rgb2Xyz(X, Y, Z, R, G, B);
    xyz2Lab(L, a, b, X, Y, Z);
}

double IODCIELabUtil::gammaCorrection(double n)
{
    // sRGB transfer function (IEC 61966-2-1), linear to gamma-encoded
    if (n <= 0.0031308)
    {
        return 12.92 * n;
    }
    else
    {
        return 1.055 * pow(n, 1.0 / 2.4) - 0.055;
    }
}

double IODCIELabUtil::invGammaCorrection(double n)
{
    // sRGB transfer function (IEC 61966-2-1), gamma-encoded to linear
    if (n <= 0.04045)
    {
        return n / 12.92;
    }
    else
    {
        return pow((n + 0.055) / 1.055, 2.4);
    }
}

void IODCIELabUtil::rgb2Xyz(double& X, double& Y, double& Z, double R, double G, double B)
{
    // sRGB to linear RGB
    R = invGammaCorrection(R);
    G = invGammaCorrection(G);
    B = invGammaCorrection(B);

    // linear RGB to CIE XYZ (D65), matrix from CSS Color Module Level 4
    const double X65 = (506752.0 / 1228815.0) * R + (87881.0 / 245763.0) * G + (12673.0 / 70218.0) * B;
    const double Y65 = (87098.0 / 409605.0) * R + (175762.0 / 245763.0) * G + (12673.0 / 175545.0) * B;
    const double Z65 = (7918.0 / 409605.0) * R + (87881.0 / 737289.0) * G + (1001167.0 / 1053270.0) * B;

    // chromatic adaptation D65 to D50 (linear Bradford), matrix from CSS Color Module Level 4
    X = 1.0479297925449969 * X65 + 0.022946870601609652 * Y65 - 0.05019226628920524 * Z65;
    Y = 0.02962780877005599 * X65 + 0.9904344267538799 * Y65 - 0.017073799063418826 * Z65;
    Z = -0.009243040646204504 * X65 + 0.015055191490298152 * Y65 + 0.7518742814281371 * Z65;
}

void IODCIELabUtil::xyz2Lab(double& L, double& a, double& b, double X, double Y, double Z)
{
    X /= D50_WHITEPOINT_X;
    Y /= D50_WHITEPOINT_Y;
    Z /= D50_WHITEPOINT_Z;
    X = labf(X);
    Y = labf(Y);
    Z = labf(Z);
    L = 116 * Y - 16;
    a = 500 * (X - Y);
    b = 200 * (Y - Z);
}

void IODCIELabUtil::lab2Rgb(double& R, double& G, double& B, double L, double a, double b)
{
    double X, Y, Z;
    lab2Xyz(X, Y, Z, L, a, b);
    xyz2Rgb(R, G, B, X, Y, Z);
}

void IODCIELabUtil::lab2Xyz(double& X, double& Y, double& Z, double L, double a, double b)
{
    L = (L + 16) / 116;
    a = L + a / 500;
    b = L - b / 200;
    X = D50_WHITEPOINT_X * labfInv(a);
    Y = D50_WHITEPOINT_Y * labfInv(L);
    Z = D50_WHITEPOINT_Z * labfInv(b);
}

void IODCIELabUtil::xyz2Rgb(double& R, double& G, double& B, double X, double Y, double Z)
{
    // chromatic adaptation D50 to D65 (linear Bradford), matrix from CSS Color Module Level 4
    const double X65 = 0.955473421488075 * X - 0.02309845494876471 * Y + 0.06325924320057072 * Z;
    const double Y65 = -0.0283697093338637 * X + 1.0099953980813041 * Y + 0.021041441191917323 * Z;
    const double Z65 = 0.012314014864481998 * X - 0.020507649298898964 * Y + 1.330365926242124 * Z;

    // CIE XYZ (D65) to linear RGB, matrix from CSS Color Module Level 4
    const double R1 = (12831.0 / 3959.0) * X65 - (329.0 / 214.0) * Y65 - (1974.0 / 3959.0) * Z65;
    const double G1 = (-851781.0 / 878810.0) * X65 + (1648619.0 / 878810.0) * Y65 + (36519.0 / 878810.0) * Z65;
    const double B1 = (705.0 / 12673.0) * X65 - (2585.0 / 12673.0) * Y65 + (705.0 / 667.0) * Z65;

    // Clip to the sRGB gamut (also makes sure that gamma encoding is
    // well-defined), then transform from linear RGB to sRGB
    R = gammaCorrection(clamp01(R1));
    G = gammaCorrection(clamp01(G1));
    B = gammaCorrection(clamp01(B1));
}

double IODCIELabUtil::labf(double n)
{
    // epsilon = (6/29)^3 = 216/24389, kappa = (29/3)^3 = 24389/27
    if (n > 216.0 / 24389.0)
    {
        return cbrt(n);
    }
    else
    {
        return ((24389.0 / 27.0) * n + 16.0) / 116.0;
    }
}

double IODCIELabUtil::labfInv(double n)
{
    // n > 6/29 <=> n^3 > epsilon
    if (n > 6.0 / 29.0)
    {
        return n * n * n;
    }
    else
    {
        return (116.0 * n - 16.0) / (24389.0 / 27.0);
    }
}

double IODCIELabUtil::clamp01(double n)
{
    if (n < 0.0)
        return 0.0;
    if (n > 1.0)
        return 1.0;
    return n;
}

double IODCIELabUtil::roundClamp(double n, double max)
{
    if (n < 0.0)
        return 0.0;
    if (n > max)
        return max;
    return floor(n + 0.5);
}
