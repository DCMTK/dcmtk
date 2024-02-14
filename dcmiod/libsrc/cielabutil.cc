/*
 *
 *  Copyright (C) 2005-2024, Pascal Getreuer, Open Connections GmbH
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
#endif

// Initialize white points of D65 light point (CIELab standard white point)
const double IODCIELabUtil::D65_WHITEPOINT_X = 0.950456;
const double IODCIELabUtil::D65_WHITEPOINT_Y = 1.0;
const double IODCIELabUtil::D65_WHITEPOINT_Z = 1.088754;

void IODCIELabUtil::dicomLab2RGB(double& R, double& G, double& B, double LDicom, double aDicom, double bDicom)
{
    double L, a, b;
    dicomlab2Lab(L, a, b, LDicom, aDicom, bDicom);
    lab2Rgb(R, G, B, L, a, b);
}

void IODCIELabUtil::rgb2DicomLab(double& LDicom, double& aDicom, double& bDicom, double R, double G, double B)
{
    double L, a, b;
    rgb2Lab(L, a, b, R, G, B);
    lab2DicomLab(LDicom, aDicom, bDicom, L, a, b);
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
    if ((n) <= 0.0031306684425005883)
    {
        return 12.92 * (n);
    }
    else
    {
        return (1.055 * pow((n), 0.416666666666666667) - 0.055);
    }
}

double IODCIELabUtil::invGammaCorrection(double n)
{
    if ((n) <= 0.0404482362771076)
    {
        return ((n) / 12.92);
    }
    else
    {
        return (pow(((n) + 0.055) / 1.055, 2.4));
    }
}

void IODCIELabUtil::rgb2Xyz(double& X, double& Y, double& Z, double R, double G, double B)
{
    R = invGammaCorrection(R);
    G = invGammaCorrection(G);
    B = invGammaCorrection(B);
    X = OFstatic_cast(double, (0.4123955889674142161 * R + 0.3575834307637148171 * G + 0.1804926473817015735 * B));
    Y = OFstatic_cast(double, (0.2125862307855955516 * R + 0.7151703037034108499 * G + 0.07220049864333622685 * B));
    Z = OFstatic_cast(double, (0.01929721549174694484 * R + 0.1191838645808485318 * G + 0.9504971251315797660 * B));
}

void IODCIELabUtil::xyz2Lab(double& L, double& a, double& b, double X, double Y, double Z)
{
    X /= D65_WHITEPOINT_X;
    Y /= D65_WHITEPOINT_Y;
    Z /= D65_WHITEPOINT_Z;
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
    X = D65_WHITEPOINT_X * labfInv(a);
    Y = D65_WHITEPOINT_Y * labfInv(L);
    Z = D65_WHITEPOINT_Z * labfInv(b);
}

void IODCIELabUtil::xyz2Rgb(double& R, double& G, double& B, double X, double Y, double Z)
{
    double R1, B1, G1, Min;

    R1 = OFstatic_cast(double, (3.2406 * X - 1.5372 * Y - 0.4986 * Z));
    G1 = OFstatic_cast(double, (-0.9689 * X + 1.8758 * Y + 0.0415 * Z));
    B1 = OFstatic_cast(double, (0.0557 * X - 0.2040 * Y + 1.0570 * Z));

    Min = min3(R1, G1, B1);

    /* Force nonnegative values so that gamma correction is well-defined. */
    if (Min < 0)
    {
        R1 -= Min;
        G1 -= Min;
        B1 -= Min;
    }

    /* Transform from RGB to R'G'B' */
    R = gammaCorrection(R1);
    G = gammaCorrection(G1);
    B = gammaCorrection(B1);
}

double IODCIELabUtil::labf(double n)
{
    if (n >= 8.85645167903563082e-3)
    {
        return (pow(n, 0.333333333333333));
    }
    else
    {
        return ((841.0 / 108.0) * (n) + (4.0 / 29.0));
    }
}

double IODCIELabUtil::labfInv(double n)
{
    if ((n) >= 0.206896551724137931)
    {
        return (n) * (n) * (n);
    }
    else
    {
        return (108.0 / 841.0) * ((n) - (4.0 / 29.0));
    }
}

double IODCIELabUtil::min2(double a, double b)
{
    if (a <= b)
        return a;
    else
        return b;
}

double IODCIELabUtil::min3(double a, double b, double c)
{
    if (a <= b)
        return min2(a, c);
    else
        return min2(b, c);
}
