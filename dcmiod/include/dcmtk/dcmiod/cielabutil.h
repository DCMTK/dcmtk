/*
 *
 *  Copyright (C) 2016-2026, Pascal Getreuer, Open Connections GmbH
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

#ifndef CIELABUTIL_H
#define CIELABUTIL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/ofstd/oftypes.h"

#include <cmath>

/** Class supporting color space conversions from and to CIELab. In some IODs
 *  DICOM stores CIELab color values (e.g. Recommended Display CIELab Value)
 *  which must often be converted to RGB for display or other purposes. This
 *  is supported by the functions dicomLab2RGB() and rgb2DicomLab(). The
 *  CIELab value range in DICOM is [0,65535] for all three values. The RGB
 *  value range in this class is [0,1] (or [0,255] for the 8 bit integer
 *  variants). Further functions are available in order to convert between
 *  RGB, "normal" CIELab value range as well as CIEXYZ. The class uses double
 *  floating point precision for calculations.
 *
 *  DICOM (part 3, C.10.7.1.1) encodes CIELab values in the same way as the
 *  ICC Profile Connection Space (PCS), i.e. the CIELab values are relative
 *  to the D50 illuminant. The RGB values handled by this class are sRGB
 *  values, whose native white point is D65. This class therefore converts
 *  sRGB to linear RGB, to CIEXYZ (D65), chromatically adapts the CIEXYZ
 *  values to D50 using the linear Bradford transform, and finally computes
 *  CIELab relative to the D50 white point (and vice versa). All CIEXYZ values
 *  exposed by this class are D50 (PCS) values. The matrices and constants
 *  used are those of the CSS Color Module Level 4 sample code
 *  (https://www.w3.org/TR/css-color-4/#color-conversion-code), so that
 *  results are consistent with other DICOM implementations following the
 *  same approach.
 *
 *  Note that up to and including DCMTK 3.7.0 the CIELab values computed by
 *  this class were relative to D65, i.e. no chromatic adaptation to the PCS
 *  illuminant took place. For neutral (gray) colors the results are the
 *  same, for chromatic colors the a* and b* values differ slightly.
 */
class DCMTK_DCMIOD_EXPORT IODCIELabUtil
{
public:
    /// D50 white point X component (ICC PCS illuminant, CIELab reference white)
    static const double D50_WHITEPOINT_X;
    /// D50 white point Y component (ICC PCS illuminant, CIELab reference white)
    static const double D50_WHITEPOINT_Y;
    /// D50 white point Z component (ICC PCS illuminant, CIELab reference white)
    static const double D50_WHITEPOINT_Z;

    /// D65 white point X component (native white point of sRGB)
    static const double D65_WHITEPOINT_X;
    /// D65 white point Y component (native white point of sRGB)
    static const double D65_WHITEPOINT_Y;
    /// D65 white point Z component (native white point of sRGB)
    static const double D65_WHITEPOINT_Z;

    /** Convert CIELab color representation as found in DICOM to sRGB value
     *  representation. See DICOM part 3 for details. Colors outside the sRGB
     *  gamut are clipped, i.e. the resulting R, G and B values are always
     *  within the range 0 <= value <= 1.
     *  @param  R Output sRGB "R" component (red) with 0 <= R <= 1
     *  @param  G Output sRGB "G" component (green) with 0 <= G <= 1
     *  @param  B Output sRGB "B" component (blue) with 0 <= B <= 1
     *  @param  LDicom Input DICOM CIELab luminance component with 0 <= L <= 65535
     *  @param  aDicom Input DICOM CIELab "a" component (red<->green) with 0 <= a <= 65535
     *  @param  bDicom Input DICOM CIELab "b" component (blue<->yellow) with 0 <= b <= 65535
     */
    static void dicomLab2RGB(double& R, double& G, double& B, double LDicom, double aDicom, double bDicom);

    /** Convert CIELab color representation as found in DICOM to 8 bit sRGB
     *  value representation. This is a convenience variant of
     *  dicomLab2RGB(double&, double&, double&, double, double, double) that
     *  rounds the result to the nearest integer and clamps it to the range
     *  0 <= value <= 255.
     *  @param  R Output sRGB "R" component (red) with 0 <= R <= 255
     *  @param  G Output sRGB "G" component (green) with 0 <= G <= 255
     *  @param  B Output sRGB "B" component (blue) with 0 <= B <= 255
     *  @param  LDicom Input DICOM CIELab luminance component with 0 <= L <= 65535
     *  @param  aDicom Input DICOM CIELab "a" component (red<->green) with 0 <= a <= 65535
     *  @param  bDicom Input DICOM CIELab "b" component (blue<->yellow) with 0 <= b <= 65535
     */
    static void dicomLab2RGB(Uint8& R, Uint8& G, Uint8& B, Uint16 LDicom, Uint16 aDicom, Uint16 bDicom);

    /** Convert sRGB color representation to CIELab color representation as found
     *  in DICOM. See DICOM part 3 for details.
     *  @param  LDicom Output CIELab luminance component with 0 <= L <= 65535 as found
     *          in DICOM
     *  @param  aDicom Output CIELab "a" component (red<->green) with 0 <= a <= 65535
     *          as found in DICOM
     *  @param  bDicom Output CIELab "b" component (blue<->yellow) with 0 <= b <= 65535
     *          as found in DICOM
     *  @param  R Input sRGB "R" component (red) with 0 <= R <= 1
     *  @param  G Input sRGB "G" component (green) with 0 <= G <= 1
     *  @param  B Input sRGB "B" component (blue) with 0 <= B <= 1
     *
     */
    static void rgb2DicomLab(double& LDicom, double& aDicom, double& bDicom, double R, double G, double B);

    /** Convert 8 bit sRGB color representation to CIELab color representation
     *  as found in DICOM. This is a convenience variant of
     *  rgb2DicomLab(double&, double&, double&, double, double, double) that
     *  rounds the result to the nearest integer and clamps it to the range
     *  0 <= value <= 65535.
     *  @param  LDicom Output CIELab luminance component with 0 <= L <= 65535 as found
     *          in DICOM
     *  @param  aDicom Output CIELab "a" component (red<->green) with 0 <= a <= 65535
     *          as found in DICOM
     *  @param  bDicom Output CIELab "b" component (blue<->yellow) with 0 <= b <= 65535
     *          as found in DICOM
     *  @param  R Input sRGB "R" component (red) with 0 <= R <= 255
     *  @param  G Input sRGB "G" component (green) with 0 <= G <= 255
     *  @param  B Input sRGB "B" component (blue) with 0 <= B <= 255
     */
    static void rgb2DicomLab(Uint16& LDicom, Uint16& aDicom, Uint16& bDicom, Uint8 R, Uint8 G, Uint8 B);

    /** Convert CIELab color representation as found in DICOM to CIELab
     *  representation. See DICOM part 3 for details.
     *  @param  L Output CIELab luminance component with 0 <= L <= 100
     *  @param  a Output CIELab "a" component (red<->green) with -128 <= a <= 127
     *  @param  b Output CIELab "b" component (blue<->yellow) with -128 <= b <= 127
     *  @param  LDicom Input CIELab luminance component with 0 <= L <= 65535 as found
     *          in DICOM
     *  @param  aDicom Input CIELab "a" component (red<->green) with 0 <= a <= 65535
     *          as found in DICOM
     *  @param  bDicom Input CIELab "b" component (blue<->yellow) with 0 <= b <= 65535
     *          as found in DICOM
     */
    static void dicomlab2Lab(double& L, double& a, double& b, double LDicom, double aDicom, double bDicom);

    /** Convert CIELab color representation to CIELab color representation found
     *  in DICOM. See DICOM part 3 for details.
     *  @param  LDicom Output CIELab luminance component with 0 <= L <= 65535 as found
     *          in DICOM
     *  @param  aDicom Output CIELab "a" component (red<->green) with 0 <= a <= 65535
     *          as found in DICOM
     *  @param  bDicom Output CIELab "b" component (blue<->yellow) with 0 <= b <= 65535
     *          as found in DICOM
     *  @param  L Input CIELab luminance component with 0 <= L <= 100
     *  @param  a Input CIELab "a" component (red<->green) with -128 <= a <= 127
     *  @param  b Input CIELab "b" component (blue<->yellow) with -128 <= b <= 127
     *
     */
    static void lab2DicomLab(double& LDicom, double& aDicom, double& bDicom, double L, double a, double b);

    /** Convert sRGB color representation to CIELab representation (relative
     *  to the D50 white point, i.e. ICC PCS CIELab)
     *  @param  L Output CIELab luminance component with 0 <= L <= 100
     *  @param  a Output CIELab "a" component (red<->green) with -128 <= a <= 127
     *  @param  b Output CIELab "b" component (blue<->yellow) with -128 <= b <= 127
     *  @param  R Input sRGB "R" component (red) with 0 <= R <= 1
     *  @param  G Input sRGB "G" component (green) with 0 <= G <= 1
     *  @param  B Input sRGB "B" component (blue) with 0 <= B <= 1
     */
    static void rgb2Lab(double& L, double& a, double& b, double R, double G, double B);

    /** Convert sRGB color representation to CIE XYZ representation relative
     *  to the D50 white point (ICC PCS XYZ), i.e. the sRGB values are
     *  linearized, converted to CIE XYZ (D65) and then chromatically adapted
     *  to D50 using the linear Bradford transform.
     *  @param  X Output CIE XYZ "X" component with 0 <= X <= D50_WHITEPOINT_X
     *  @param  Y Output CIE XYZ "Y" component with 0 <= Y <= 1
     *  @param  Z Output CIE XYZ "Z" component with 0 <= Z <= D50_WHITEPOINT_Z
     *  @param  R Input sRGB "R" component (red) with 0 <= R <= 1
     *  @param  G Input sRGB "G" component (green) with 0 <= G <= 1
     *  @param  B Input sRGB "B" component (blue) with 0 <= B <= 1
     */
    static void rgb2Xyz(double& X, double& Y, double& Z, double R, double G, double B);

    /** Convert CIE XYZ color representation (relative to D50) to CIELab
     *  representation (relative to D50)
     *  @param  L Output CIELab luminance component with 0 <= L <= 100
     *  @param  a Output CIELab "a" component (red<->green) with -128 <= a <= 127
     *  @param  b Output CIELab "b" component (blue<->yellow) with -128 <= b <= 127
     *  @param  X Input CIE XYZ "X" component with 0 <= X <= D50_WHITEPOINT_X
     *  @param  Y Input CIE XYZ "Y" component with 0 <= Y <= 1
     *  @param  Z Input CIE XYZ "Z" component with 0 <= Z <= D50_WHITEPOINT_Z
     */
    static void xyz2Lab(double& L, double& a, double& b, double X, double Y, double Z);

    /** Convert CIELab color representation (relative to D50) to sRGB
     *  representation. Colors outside the sRGB gamut are clipped, i.e. the
     *  resulting R, G and B values are always within the range
     *  0 <= value <= 1.
     *  @param  R Output sRGB "R" component (red) with 0 <= R <= 1
     *  @param  G Output sRGB "G" component (green) with 0 <= G <= 1
     *  @param  B Output sRGB "B" component (blue) with 0 <= B <= 1
     *  @param  L Input CIELab luminance component with 0 <= L <= 100
     *  @param  a Input CIELab "a" component (red<->green) with -128 <= a <= 127
     *  @param  b Input CIELab "b" component (blue<->yellow) with -128 <= b <= 127
     */
    static void lab2Rgb(double& R, double& G, double& B, double L, double a, double b);

    /** Convert CIELab color representation (relative to D50) to CIE XYZ
     *  representation (relative to D50)
     *  @param  X Output CIE XYZ "X" component with 0 <= X <= D50_WHITEPOINT_X
     *  @param  Y Output CIE XYZ "Y" component with 0 <= Y <= 1
     *  @param  Z Output CIE XYZ "Z" component with 0 <= Z <= D50_WHITEPOINT_Z
     *  @param  L Input CIELab luminance component with 0 <= L <= 100
     *  @param  a Input CIELab "a" component (red<->green) with -128 <= a <= 127
     *  @param  b Input CIELab "b" component (blue<->yellow) with -128 <= b <= 127
     */
    static void lab2Xyz(double& X, double& Y, double& Z, double L, double a, double b);

    /** Convert CIE XYZ color representation (relative to D50, ICC PCS XYZ)
     *  to sRGB representation, i.e. the XYZ values are chromatically adapted
     *  from D50 to D65 using the linear Bradford transform, converted to
     *  linear RGB and finally gamma-encoded. Colors outside the sRGB gamut
     *  are clipped, i.e. the resulting R, G and B values are always within
     *  the range 0 <= value <= 1.
     *  @param  R Output sRGB "R" component (red) with 0 <= R <= 1
     *  @param  G Output sRGB "G" component (green) with 0 <= G <= 1
     *  @param  B Output sRGB "B" component (blue) with 0 <= B <= 1
     *  @param  X Input CIE XYZ "X" component with 0 <= X <= D50_WHITEPOINT_X
     *  @param  Y Input CIE XYZ "Y" component with 0 <= Y <= 1
     *  @param  Z Input CIE XYZ "Z" component with 0 <= Z <= D50_WHITEPOINT_Z
     */
    static void xyz2Rgb(double& R, double& G, double& B, double X, double Y, double Z);

protected:
    /** Perform sRGB gamma encoding, transforms linear R to R'
     *  @param  n The value to encode, 0 <= n <= 1
     *  @return The gamma-encoded value
     */
    static double gammaCorrection(double n);

    /** Perform inverse sRGB gamma encoding, transforms R' to linear R
     *  @param  n The value to decode, 0 <= n <= 1
     *  @return The linear value
     */
    static double invGammaCorrection(double n);

    /** CIE L*a*b* f function (used to convert XYZ to L*a*b*)
     *  @param  n value to convert
     *  @return The converted value
     */
    static double labf(double n);

    /** CIE L*a*b* inverse f function
     *  @param n The value to compute the inverse for
     *  @return The resulting inverse
     */
    static double labfInv(double n);

    /** Clamp a value to the range 0 <= value <= 1
     *  @param  n The value to clamp
     *  @return The clamped value
     */
    static double clamp01(double n);

    /** Round a value to the nearest integer and clamp it to the range
     *  0 <= value <= max.
     *  @param  n The value to round and clamp
     *  @param  max The maximum permitted value
     *  @return The rounded and clamped value
     */
    static double roundClamp(double n, double max);
};

#endif // CIELABUTIL_H
