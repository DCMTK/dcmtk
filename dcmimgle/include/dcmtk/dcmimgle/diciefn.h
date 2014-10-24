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
 *  Purpose: DicomCIELABFunction (Header)
 *
 */


#ifndef DICIEFN_H
#define DICIEFN_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/didispfn.h"
#include "dcmtk/dcmimgle/dicielut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to manage CIELAB LUTs (for calibration)
 */
class DCMTK_DCMIMGLE_EXPORT DiCIELABFunction
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
    DiCIELABFunction(const char *filename,
                     const E_DeviceType deviceType = EDT_Monitor,
                     const signed int ord = -1);

    /** constructor, use given array of luminance/OD values. UNTESTED
     *  Values must be sorted and complete (i.e. there must be an entry for each DDL)
     *  The given arrays are copied internally.
     *
     ** @param  val_tab     pointer to array with luminance/OD values
     *  @param  count       number of array elements (should be equal to 'max + 1')
     *  @param  max         maximum DDL (digital driving level)
     *  @param  deviceType  type of the output device (default: monitor)
     *  @param  ord         order of the polynomial curve fitting algorithm used to interpolate
     *                      the given base points (0 or negative = use cubic spline interpolation)
     */
    DiCIELABFunction(const double *val_tab,
                     const unsigned long count,
                     const Uint16 max = 255,
                     const E_DeviceType deviceType = EDT_Monitor,
                     const signed int ord = 0);

    /** constructor, use given array of DDL and luminance values. UNTESTED
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
    DiCIELABFunction(const Uint16 *ddl_tab,
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
    DiCIELABFunction(const double val_min,
                     const double val_max,
                     const unsigned long count = 256,
                     const E_DeviceType deviceType = EDT_Monitor,
                     const signed int ord = 0);

    /** destructor
     */
    virtual ~DiCIELABFunction();

    /** write curve data to a text file
     *
     ** @param  filename  name of the text file to which the data should be written
     *  @param  mode      write CC and PSC to file if OFTrue
     *
     ** @return status, true if successful, false otherwise
     */
    int writeCurveData(const char *filename,
                       const OFBool mode = OFTrue);


 protected:

    /** create CIELAB LUT with specified number of entries
     *
     ** @param  count  number of LUT entries
     *
     ** @return pointer to created LUT if successful, NULL otherwise
     */
    DiDisplayLUT *getDisplayLUT(unsigned long count);


 private:

 // --- declarations to avoid compiler warnings

    DiCIELABFunction(const DiDisplayFunction &);
    DiCIELABFunction &operator=(const DiDisplayFunction &);
};


#endif
