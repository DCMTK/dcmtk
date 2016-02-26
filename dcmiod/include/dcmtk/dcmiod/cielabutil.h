/*
 *
 *  Copyright (C) 2016, Pascal Getreuer, Open Connections GmbH
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
#define INCLUDE_CMATH                // for pow() function
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmiod/ioddef.h"


/** Class supporting color space conversions from and to CIELab. In some IODs
 *  DICOM stores CIELab color values which must often be converted to RGB for
 *  display or other purposes. This is supported by the functions dicomLab2RGB()
 *  and rgb2DicomLab(). The CIELab value range in DICOM is [0,65535] for all
 *  three values. The RGB value range in this class is [0;1]. Further functions
 *  are available in order to convert between RGB, "normal" CIELab value range
 *  as well as CIEXYZ. The class uses double floating point precision for
 *  calculations.
 */
class DCMTK_DCMIOD_EXPORT IODCIELabUtil
{
public:

  /// D65 standard lightpoint X component for conversion from CIEXYZ to CIELab
  static const double D65_WHITEPOINT_X;
  /// D65 standard lightpoint Y component for conversion from CIEXYZ to CIELab
  static const double D65_WHITEPOINT_Y;
  /// D65 standard lightpoint Z component for conversion from CIEXYZ to CIELab
  static const double D65_WHITEPOINT_Z;

  /** Convert CIELab color representation as found in DICOM to sRGB value
   *  representation. See DICOM part 3 for details.
   *  @param  R Output sRGB "R" component (red) with 0 <= R <= 1
   *  @param  G Output sRGB "G" component (green) with 0 <= G <= 1
   *  @param  B Output sRGB "R" component (blue) with 0 <= B <= 1
   *  @param  LDicom Input DICOM CIELab luminance component with 0 <= L <= 65535
   *  @param  aDicom Input DICOM CIELab "a" component (red<->green) with 0 <= a <= 65535
   *  @param  bDicom Input DIOCM CIELab "b" component (blue<->yellow) with 0 <= a <= 65535
   */
  static void dicomLab2RGB(double& R, double& G, double& B, double LDicom, double aDicom, double bDicom);

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
   *  @param  B Input sRGB "R" component (blue) with 0 <= B <= 1
   *
   */
  static void rgb2DicomLab(double& LDicom, double& aDicom, double& bDicom, double R, double G, double B);

  /** Convert CIELab color representation as found in DICOM to CIELab
   *  representation. See DICOM part 3 for details.
   *  @param  L Output CIELab luminance component with 0 <= L <= 100
   *  @param  a Output CIELab "a" component (red<->green) with -127 <= a <= 128
   *  @param  b Output CIELab "b" component (blue<->yellow) with -127 <= b <= 128
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
   *  @param  a Input CIELab "a" component (red<->green) with -127 <= a <= 128
   *  @param  b Input CIELab "b" component (blue<->yellow) with -127 <= b <= 128
   *
   */
  static void lab2DicomLab(double& LDicom, double& aDicom, double& bDicom, double L, double a, double b);

  /** Convert sRGB color representation to CIELab representation
   *  @param  L Output CIELab luminance component with 0 <= L <= 100
   *  @param  a Output CIELab "a" component (red<->green) with -127 <= a <= 128
   *  @param  b Output CIELab "b" component (blue<->yellow) with -127 <= b <= 128
   *  @param  R Input sRGB "R" component (red) with 0 <= R <= 1
   *  @param  G Input sRGB "G" component (green) with 0 <= G <= 1
   *  @param  B Input sRGB "R" component (blue) with 0 <= B <= 1
   */
  static void rgb2Lab(double& L, double& a, double& b, double R, double G, double B);

  /** Convert sRGB color representation to CIE XYZ representation
   *  @param  X Output CIELab XYZ "X" component with 0 <= X <= 1
   *  @param  Y Output CIELab XYZ "Y" component with 0 <= Y <= 1
   *  @param  Z Output CIELab XYZ "Z" component with 0 <= Z <= 1
   *  @param  R Input sRGB "R" component (red) with 0 <= R <= 1
   *  @param  G Input sRGB "G" component (green) with 0 <= G <= 1
   *  @param  B Input sRGB "R" component (blue) with 0 <= B <= 1
   */
  static void rgb2Xyz(double& X, double& Y, double& Z, double R, double G, double B);


  /** Convert CIELAB XYZ color representation to CIELab representation
   *  @param  L Output CIELab luminance component with 0 <= L <= 100
   *  @param  a Output CIELab "a" component (red<->green) with -127 <= a <= 128
   *  @param  b Output CIELab "b" component (blue<->yellow) with -127 <= b <= 128
   *  @param  X Input CIELab XYZ "X" component with 0 <= X <= 1
   *  @param  Y Input CIELab XYZ "Y" component with 0 <= Y <= 1
   *  @param  Z Input CIELab XYZ "Z" component with 0 <= Z <= 1
   */
  static void xyz2Lab(double& L, double& a, double& b, double X, double Y, double Z);

  /** Convert CIELab color representation to sRGB representation
   *  @param  R Output sRGB "R" component (red) with 0 <= R <= 1
   *  @param  G Output sRGB "G" component (green) with 0 <= G <= 1
   *  @param  B Output sRGB "R" component (blue) with 0 <= B <= 1
   *  @param  L Input CIELab luminance component with 0 <= L <= 100
   *  @param  a Input CIELab "a" component (red<->green) with -127 <= a <= 128
   *  @param  b Input CIELab "b" component (blue<->yellow) with -127 <= b <= 128
   */
  static void lab2Rgb(double& R, double& G, double& B, double L, double a, double b);

  /** Convert CIELab color representation to CIE XYZ representation
   *  @param  X Output CIELab XYZ "X" component with 0 <= X <= 1
   *  @param  Y Output CIELab XYZ "Y" component with 0 <= Y <= 1
   *  @param  Z Output CIELab XYZ "Z" component with 0 <= Z <= 1
   *  @param  L Input CIELab luminance component with 0 <= L <= 100
   *  @param  a Input CIELab "a" component (red<->green) with -127 <= a <= 128
   *  @param  b Input CIELab "b" component (blue<->yellow) with -127 <= b <= 128
   */
  static void lab2Xyz(double& X, double& Y, double& Z, double L, double a, double b);

  /** Convert CIE XYZ color representation to sRGB representation
   *  @param  R Output sRGB "R" component (red) with 0 <= R <= 1
   *  @param  G Output sRGB "G" component (green) with 0 <= G <= 1
   *  @param  B Output sRGB "R" component (blue) with 0 <= B <= 1
   *  @param  X Output CIELab XYZ "X" component with 0 <= X <= 1
   *  @param  Y Output CIELab XYZ "Y" component with 0 <= Y <= 1
   *  @param  Z Output CIELab XYZ "Z" component with 0 <= Z <= 1
   */
  static void xyz2Rgb(double& R, double& G, double& B, double X, double Y, double Z);

protected:

  /** Perform sRGB gamma correction, transforms R to R'
   *  @param  n The value to correct
   *  @return The gamma-corrected value
   */
  static double gammaCorrection(double n);

  /** Perform inverse sRGB gamma correction, transforms R' to R
   *  @param  n The value to invert
   *  @return The gamma-inverted value
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

  /** Get the minimum of two numbers
   *  @param  a First number
   *  @param  b Second number
   *  @return The minimum of a, b. a if a and b are equal.
   */
  static double min(double a, double b);

  /** Get the minimum of three numbers
   *  @param  a First number
   *  @param  b Second number
   *  @param  c Third number
   *  @return The minimum of a, b and c. If a value occurs more than once,
   *          then preference order then a is preferred if possible, b
   *          otherwise
   */
  static double min3(double a, double b, double c);

};

#endif // CIELABUTIL_H
