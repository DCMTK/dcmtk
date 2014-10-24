/*
 *
 *  Copyright (C) 1998-2014, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomGSDFunction (Header)
 *
 */


#ifndef DIGSDFN_H
#define DIGSDFN_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/didispfn.h"

#define INCLUDE_CSTDDEF               /* For NULL */
#include "dcmtk/ofstd/ofstdinc.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to manage GSDF LUTs (for calibration)
 */
class DCMTK_DCMIMGLE_EXPORT DiGSDFunction
  : public DiDisplayFunction
{

 public:

    /** constructor, read device characteristics file.
     *  Keywords: "max" for maximum DDL (Digital Driving Level, required at first position)
     *            "amb" for ambient light and "lum" for illumination (both optional)
     *            "ord" for the order of the polynomial curve fitting algorithm used to interpolate
     *                  the given base points (0 or absent = use cubic spline interpolation)
     *
     ** @param  filename    name of the characteristics file (luminance/OD for each DDL)
     *  @param  deviceType  type of the output device (default: monitor)
     *  @param  ord         order of the polynomial curve fitting algorithm used to interpolate
     *                      the given base points (-1 = use file setting, 0 = cubic spline)
     */
    DiGSDFunction(const char *filename,
                  const E_DeviceType deviceType = EDT_Monitor,
                  const signed int ord = -1);

    /** constructor, use given array of luminance/OD values. UNTESTED
     *  Values must be sorted and complete (i.e. there must be an entry for each DDL).
     *  The given arrays are copied internally.
     *
     ** @param  val_tab     pointer to array with luminance/OD values
     *  @param  count       number of array elements (should be equal to 'max + 1')
     *  @param  max         maximum DDL (digital driving level)
     *  @param  deviceType  type of the output device (default: monitor)
     *  @param  ord         order of the polynomial curve fitting algorithm used to interpolate
     *                      the given base points (0 or negative = use cubic spline interpolation)
     */
    DiGSDFunction(const double *val_tab,
                  const unsigned long count,
                  const Uint16 max = 255,
                  const E_DeviceType deviceType = EDT_Monitor,
                  const signed int ord = 0);

    /** constructor, use given array of DDL and luminance/OD values. UNTESTED
     *  Values will be automatically sorted and missing values will be interpolated.
     *  The given arrays are copied internally.
     *
     ** @param  ddl_tab     pointer to array with DDL values (must be with the interval 0..max)
     *  @param  val_tab     pointer to array with luminance/OD values
     *  @param  count       number of array elements (2..65536)
     *  @param  max         maximum DDL (digital driving level)
     *  @param  deviceType  type of the output device (default: monitor)
     *  @param  ord         order of the polynomial curve fitting algorithm used to interpolate
     *                      the given base points (0 or negative = use cubic spline interpolation)
     */
    DiGSDFunction(const Uint16 *ddl_tab,
                  const double *val_tab,
                  const unsigned long count,
                  const Uint16 max = 255,
                  const E_DeviceType deviceType = EDT_Monitor,
                  const signed int ord = 0);

    /** constructor, compute luminance/OD values automatically within the specified range.
     *
     ** @param  val_min     minimum luminance/OD value
     *  @param  val_max     maximum luminance/OD value
     *  @param  count       number of DDLs (digital driving level, 1..65536))
     *  @param  deviceType  type of the output device (default: monitor)
     *  @param  ord         order of the polynomial curve fitting algorithm used to interpolate
     *                      the given base points (0 or negative = use cubic spline interpolation)
     */
    DiGSDFunction(const double val_min,
                  const double val_max,
                  const unsigned long count = 256,
                  const E_DeviceType deviceType = EDT_Monitor,
                  const signed int ord = 0);

    /** destructor
     */
    virtual ~DiGSDFunction();

    /** write curve data to a text file
     *
     ** @param  filename  name of the text file to which the data should be written
     *  @param  mode      write CC and PSC to file if OFTrue
     *
     ** @return status, true if successful, false otherwise
     */
    int writeCurveData(const char *filename,
                       const OFBool mode = OFTrue);

    /** set (reflected) ambient light value.
     *  measured in cd/m^2. applicable to softcopy and hardcopy devices.
     *  typical values: 0.5-5 for softcopy devices, 10 for transmissive hardcopy
     *  printer and 0 for reflective hardcopy printers.
     *
     ** @param  value  ambient light value to be set (>= 0)
     *
     ** @return status, true if successful, false otherwise
     */
    int setAmbientLightValue(const double value);

    /** set illumination value.
     *  measured in cd/m^2. applicable to hardcopy devices only.
     *  typical values: 2000 for transmissive hardcopy printer and 150 for
     *  reflective hardcopy printers.
     *
     ** @param  value  illumination value to be set (>= 0)
     *
     ** @return status, true if successful, false otherwise
     */
    int setIlluminationValue(const double value);

    /** set minimum optical density value "Dmin".
     *  measured in optical density (OD). applicable to printers only.
     *  typical value: 0.2
     *
     ** @param  value  Dmin value to be set (or < 0 to unset)
     *
     ** @return status, true if successful (1 = Dmax set, 2 = Dmax unset), false otherwise
     */
    int setMinDensityValue(const double value);

    /** set maximum optical density value "Dmax".
     *  measured in optical density (OD). applicable to printers only.
     *  typical value: 3.0
     *
     ** @param  value  Dmax value to be set (or < 0 to unset)
     *
     ** @return status, true if successful (1 = Dmax set, 2 = Dmax unset), false otherwise
     */
    int setMaxDensityValue(const double value);

    /** calculate the JND index for a given luminance value
     *
     ** @param  lum  luminance value
     *
     ** @return JND index if successful, -1 otherwise
     */
    static double getJNDIndex(const double lum);


 protected:

    /** create GSDF LUT with specified number of entries
     *
     ** @param  count  number of LUT entries
     *
     ** @return pointer to created LUT if successful, NULL otherwise
     */
    DiDisplayLUT *getDisplayLUT(unsigned long count);

    /** calculate GSDF (array of 1023 luminance/OD values)
     *
     ** @return status, true if successful, false otherwise
     */
    int calculateGSDF();

    /** calculate helper function for GSDF interpolation
     *
     ** @return status, true if successful, false otherwise
     */
    int calculateGSDFSpline();

    /** calculate 'JNDMin' and 'JNDMax' for the given luminance/OD range
     *
     ** @return status, true if successful, false otherwise
     */
    int calculateJNDBoundaries();


 private:

    /// minimum JND index value for the given display system
    double JNDMin;
    /// maximum JND index value for the given display system
    double JNDMax;

    /// constant defining the number JND indexes for the maximum luminance/OD range (1023)
    static const unsigned int GSDFCount;
    /// array of luminance/OD values defining the GSDF
    double *GSDFValue;
    /// array of values used for the interpolation of the GSDF
    double *GSDFSpline;

 // --- declarations to avoid compiler warnings

    DiGSDFunction(const DiGSDFunction &);
    DiGSDFunction &operator=(const DiGSDFunction &);
};


#endif
