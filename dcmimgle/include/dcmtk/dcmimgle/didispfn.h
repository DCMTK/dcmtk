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
 *  Purpose: DicomDisplayFunction (Header)
 *
 */


#ifndef DIDISPFN_H
#define DIDISPFN_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftypes.h"

#include "dcmtk/dcmimgle/didefine.h"

/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDisplayLUT;


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_DISPLAY_FUNCTIONS 2
#define MAX_NUMBER_OF_TABLES 15
#define WIDTH_OF_PVALUES     16


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle hardcopy and softcopy device characteristics file
 *  and manage display LUTs (for calibration)
 */
class DCMTK_DCMIMGLE_EXPORT DiDisplayFunction
{

 public:

    /** output device type
     */
    enum E_DeviceType
    {
        /// monitor (softcopy output device), values expected to be in luminance (cd/m^2)
        EDT_Monitor,
        /// camera (softcopy input device), values expected to be in luminance (cd/m^2)
        EDT_Camera,
        /// printer (hardcopy output device), values expected to be in optical density (OD)
        EDT_Printer,
        /// scanner (hardcopy input device), values expected to be in optical density (OD)
        EDT_Scanner
    };

    /** constructor, read device characteristics file.
     *  Keywords: "max" for maximum DDL (digital driving level, required at first position)
     *            "amb" for ambient light and "lum" for illumination (both optional)
     *            "ord" for the order of the polynomial curve fitting algorithm used to interpolate
     *                  the given base points (0 or absent = use cubic spline interpolation)
     *
     ** @param  filename    name of the characteristics file (luminance/OD for each DDL)
     *  @param  deviceType  type of the output device (default: monitor)
     *  @param  ord         order of the polynomial curve fitting algorithm used to interpolate
     *                      the given base points (-1 = use file setting, 0 = cubic spline)
     */
    DiDisplayFunction(const char *filename,
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
    DiDisplayFunction(const double *val_tab,
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
    DiDisplayFunction(const Uint16 *ddl_tab,
                      const double *val_tab,
                      const unsigned long count,
                      const Uint16 max = 255,
                      const E_DeviceType deviceType = EDT_Monitor,
                      const signed int ord = 0);

    /** constructor, compute luminance/OD values automatically within the specified range.
     *  Initial values for hardcopy: "lum" = 2000, "amb" = 10, for softcopy: "amb" = 0.
     *
     ** @param  val_min     minimum luminance/OD value
     *  @param  val_max     maximum luminance/OD value
     *  @param  count       number of DDLs (digital driving level, 1..65536)
     *  @param  deviceType  type of the output device (default: monitor)
     *  @param  ord         order of the polynomial curve fitting algorithm used to interpolate
     *                      the given base points (0 or negative = use cubic spline interpolation)
     */
    DiDisplayFunction(const double val_min,
                      const double val_max,
                      const unsigned long count = 256,
                      const E_DeviceType deviceType = EDT_Monitor,
                      const signed int ord = 0);

    /** destructor
     */
    virtual ~DiDisplayFunction();

    /** check whether DisplayFunction is valid
     *
     ** @return status, true if valid, false otherwise
     */
    inline int isValid() const
    {
        return Valid;
    }

    /** get output device type (monitor, camera, printer or scanner)
     *
     ** @return output device type
     */
    inline E_DeviceType getDeviceType() const
    {
        return DeviceType;
    }

    /** get maximum DDL value.
     *  The minimum DDL value is always 0.
     *
     ** @return maximum DDL value
     */
    inline Uint16 getMaxDDLValue() const
    {
        return MaxDDLValue;
    }

    /** get minimum luminance/OD value from the characteristic curve.
     *  In case of a usually monotonous characteristic curve the value is
     *  equivalent to the first/last entry of the array.
     *
     ** @return minimum luminance/OD value, 0 in case of error
     */
    inline double getMinValue() const
    {
        return MinValue;
    }

    /** get maximum luminance/OD value from the characteristic curve.
     *  In case of a usually monotonous characteristic curve the value is
     *  equivalent to the last/first entry of the array.
     *
     ** @return maximum luminance/OD value, 0 in case of error
     */
    inline double getMaxValue() const
    {
        return MaxValue;
    }

    /** get the luminance/OD value for a given DDL.
     *  Looks up a luminance/OD value in the device's characteristic curve.
     *  Please note that neither ambient light/illumination nor min/max
     *  density are used.
     *
     ** @param  ddl  DDL (digital driving level) to be looked up
     *
     ** @return luminance/OD value if successful, -1 otherwise
     */
    double getValueforDDL(const Uint16 ddl) const;

    /** get the DDL for a given luminance/OD value.
     *  Determines the DDL from the device's characteristic curve which is
     *  mapped to the closest possible luminance/OD value.
     *  Please note that neither ambient light/illumination nor min/max
     *  density are used.
     *
     ** @param  value  luminance/OD value to be looked up
     *
     ** @return DDL (digital driving level) if successful, 0 otherwise
     */
    Uint16 getDDLforValue(const double value) const;

    /** create look-up table with specified number of entries
     *
     ** @param  bits   depth of input values
     *  @param  count  number of LUT entries (default: 0 = computed automatically)
     *
     ** @return pointer to created LUT if successful, NULL otherwise
     */
    const DiDisplayLUT *getLookupTable(const int bits,
                                       unsigned long count = 0);

    /** delete specified LUT
     *
     ** @param  bits  depth of input values of the LUT to be deleted
     *
     ** @return status, true if valid, false otherwise
     */
    int deleteLookupTable(const int bits);

    /** write curve data to a text file (abstract method)
     *
     ** @param  filename  name of the text file to which the data should be written
     *  @param  mode      write CC and PSC to file if OFTrue
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int writeCurveData(const char *filename,
                               const OFBool mode = OFTrue) = 0;

    /** get (reflected) ambient light value.
     *  measured in cd/m^2.
     *
     ** @return current ambient light value
     */
    inline double getAmbientLightValue() const
    {
        return AmbientLight;
    }

    /** set (reflected) ambient light value.
     *  measured in cd/m^2. applicable to softcopy and hardcopy devices.
     *  typical values: 0.5-5 for softcopy devices, 10 for transmissive hardcopy
     *  printer and 0 for reflective hardcopy printers.
     *
     ** @param  value  ambient light value to be set (>= 0)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int setAmbientLightValue(const double value);

    /** get illumination value.
     *  measured in cd/m^2.
     *
     ** @return current illumination value
     */
    inline double getIlluminationValue() const
    {
        return Illumination;
    }

    /** set illumination value.
     *  measured in cd/m^2. applicable to hardcopy devices only.
     *  typical values: 2000 for transmissive hardcopy printer and 150 for
     *  reflective hardcopy printers.
     *
     ** @param  value  illumination value to be set (>= 0)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int setIlluminationValue(const double value);

    /** get minimum optical density value "Dmin".
     *  measured in optical density (OD).
     *
     ** @return current Dmin value or -1 if not set
     */
    inline double getMinDensityValue() const
    {
        return MinDensity;
    }

    /** set minimum optical density value "Dmin".
     *  measured in optical density (OD). applicable to printers only.
     *  typical value: 0.2
     *
     ** @param  value  Dmin value to be set (or < 0 to unset)
     *
     ** @return status, true if successful (1 = Dmin set, 2 = Dmin unset), false otherwise
     */
    virtual int setMinDensityValue(const double value);

    /** get maximum optical density value "Dmax".
     *  measured in optical density (OD).
     *
     ** @return current Dmax value or -1 if not set
     */
    inline double getMaxDensityValue() const
    {
        return MaxDensity;
    }

    /** set maximum optical density value "Dmax".
     *  measured in optical density (OD). applicable to printers only.
     *  typical value: 3.0
     *
     ** @param  value  Dmax value to be set (or < 0 to unset)
     *
     ** @return status, true if successful (1 = Dmax set, 2 = Dmax unset), false otherwise
     */
    virtual int setMaxDensityValue(const double value);

    /** get minimum luminance value "Lmin".
     *  measured in cd/m^2. value is computed from "Dmax".
     *
     ** @return current Lmin value or -1 if Dmax not set
     */
    double getMinLuminanceValue() const;

    /** get maximum luminance value "Lmax".
     *  measured in cd/m^2. value is computed from "Dmin".
     *
     ** @return current Lmax value or -1 if Dmin not set
     */
    double getMaxLuminanceValue() const;

    /** get order of the polynomial curve fitting algorithm.
     *  used to interpolate the given base points.
     *
     ** @return polynomial order (0 = use cubic spline interpolation, -1 = not specified)
     */
    inline signed int getPolynomialOrder() const
    {
        return Order;
    }

    /** convert the given OD value to luminance.
     *  This function uses the currently set ambient light and illumination values.
     *
     ** @param  value   optical density value to be converted (>= 0)
     *  @param  useAmb  use ambient light value if OFTrue
     *
     ** @return luminance value if successful, -1 otherwise
     */
    double convertODtoLum(const double value,
                          const OFBool useAmb = OFTrue) const;

    /** convert the given OD value to luminance.
     *  This function uses the specified ambient light and illumination values.
     *
     ** @param  value    optical density value to be converted (>= 0)
     *  @param  ambient  ambient light value used for the conversion (>= 0)
     *  @param  illum    illumination value used for the conversion (>= 0)
     *
     ** @return luminance value if successful, -1 otherwise
     */
    static double convertODtoLum(const double value,
                                 const double ambient,
                                 const double illum);


 protected:

    /** create display LUT with specified number of entries (abstract method)
     *
     ** @param  count  number of LUT entries
     *
     ** @return pointer to created LUT if successful, NULL otherwise
     */
    virtual DiDisplayLUT *getDisplayLUT(unsigned long count) = 0;

    /** read the given device characteristics file
     *
     ** @param  filename  name of the characteristics file
     *
     ** @return status, true if successful, false otherwise
     */
    int readConfigFile(const char *filename);

    /** create a sorted (by DDL) table from the given DDL and luminance/OD tables
     *
     ** @param  ddl_tab  pointer to array with DDL values
     *  @param  val_tab  pointer to array with luminance/OD values
     *
     ** @return status, true if successful, false otherwise
     */
    int createSortedTable(const Uint16 *ddl_tab,
                          const double *val_tab);

    /** create a table with luminance values from the given OD table.
     *  Uses the currently set ambient light and illumination value.  The resulting luminance
     *  table has to be deleted by the caller.
     *
     ** @param  od_tab  pointer to array with optical density values
     *  @param  count   number of entries in the array
     *  @param  useAmb  use ambient light value if OFTrue
     *
     ** @return pointer to luminance table if successful, NULL otherwise
     */
    double *convertODtoLumTable(const double *od_tab,
                                const unsigned long count,
                                const OFBool useAmb);

    /** interpolate device characteristic curve by means of a cubic spline interpolation
     */
    int interpolateValues();

    /** calculate minimum and maximum luminance/OD values
     *
     ** @return status, true if successful, false otherwise
     */
    int calculateMinMax();

    /** check whether Dmin and Dmax are properly specified.
     *  report a warning message if "Dmin >= Dmax".
     *
     ** @return status, true if successful, false otherwise
     */
    int checkMinMaxDensity() const;

    /// status flag, indicating whether display function is valid
    int Valid;

    /// output device type (monitor, camera, printer or scanner)
    const E_DeviceType DeviceType;

    /// number of DDL and luminance/OD values
    unsigned long ValueCount;
    /// maximum DDL value (usually 255)
    Uint16 MaxDDLValue;
    /// order of the polynomial curve fitting algorithm
    signed int Order;

    /// (reflected) ambient light value
    double AmbientLight;
    /// illumination value
    double Illumination;

    /// minimum optical density (-1 if unset)
    double MinDensity;
    /// maximum optical density (-1 if unset)
    double MaxDensity;

    /// pointer to array of DDL values
    Uint16 *DDLValue;
    /// pointer to array of corresponding luminance/OD values
    double *LODValue;

    /// minimum luminance/OD value
    double MinValue;
    /// maximum luminance/OD value
    double MaxValue;

    /// constant defining minimum value for number of bits for LUT input (here: 8)
    static const int MinBits;
    /// constant defining maximum value for number of bits for LUT input (here: 16)
    static const int MaxBits;

    /// array with pointer to the different lookup tables (here: 8-16 bits)
    DiDisplayLUT *LookupTable[MAX_NUMBER_OF_TABLES];


 private:

 // --- declarations to avoid compiler warnings

    DiDisplayFunction(const DiDisplayFunction &);
    DiDisplayFunction &operator=(const DiDisplayFunction &);
};


#endif
