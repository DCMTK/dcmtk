/*
 *
 *  Copyright (C) 1996-2003, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomGSDFunction (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-02-11 10:01:14 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/digsdfn.h,v $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIGSDFN_H
#define __DIGSDFN_H

#include "osconfig.h"

#include "didispfn.h"
#include "digsdlut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to manage GSDF LUTs (for calibration)
 */
class DiGSDFunction
  : public DiDisplayFunction
{

 public:

    /** constructor, read device characteristics file.
     *  Keywords: "max" for maximum DDL (Device Driving Level, required at first position)
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
     *  @param  max         maximum DDL (device driving level)
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
     *  @param  max         maximum DDL (device driving level)
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
     *  @param  count       number of DDLs (device driving level, 1..65536))
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
     ** @param  value  Dmin value to be set (or -1 to unset)
     *
     ** @return status, true if successful, false otherwise
     */
    int setMinDensityValue(const double value);

    /** set maximum optical density value "Dmax".
     *  measured in optical density (OD). applicable to printers only.
     *  typical value: 3.0
     *
     ** @param  value  Dmax value to be set (or -1 to unset)
     *
     ** @return status, true if successful, false otherwise
     */
    int setMaxDensityValue(const double value);

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

    /** check whether Dmin and Dmax are properly specified.
     *  report a warning message if "Dmin >= Dmax".
     *
     ** @return status, true if successful, false otherwise
     */
    int checkMinMaxDensity() const;


 private:

    /// minimum JND index value for the given display system
    double JNDMin;
    /// maximum JND index value for the given display system
    double JNDMax;
    
    /// costant defining the number JND indexes for the maximum luminance/OD range (1023)
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


/*
 *
 * CVS/RCS Log:
 * $Log: digsdfn.h,v $
 * Revision 1.12  2003-02-11 10:01:14  joergr
 * Added support for Dmin/max to calibration routines (required for printers).
 *
 * Revision 1.11  2002/07/19 08:24:21  joergr
 * Enhanced/corrected comments.
 *
 * Revision 1.10  2002/07/18 12:30:01  joergr
 * Added support for hardcopy and softcopy input devices (camera and scanner).
 * Added polygonal curve fitting algorithm as an alternate interpolation
 * method.
 *
 * Revision 1.9  2002/07/02 16:23:42  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.8  2001/06/01 15:49:41  meichel
 * Updated copyright header
 *
 * Revision 1.7  2000/07/17 14:37:12  joergr
 * Moved method getJNDIndex to public part of the interface.
 *
 * Revision 1.6  2000/03/08 16:24:15  meichel
 * Updated copyright header.
 *
 * Revision 1.5  2000/03/06 18:19:36  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.4  1999/10/18 15:05:51  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.3  1999/09/17 12:11:31  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.2  1999/09/10 12:17:56  joergr
 * Changed parameter type of copy constructor and assignment operator to avoid
 * compiler warnings reported by gcc/ecgs on Solaris (with additional flags).
 *
 * Revision 1.1  1999/09/10 08:50:23  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 */
