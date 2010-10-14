/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:25 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DiCIELABFunction
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
    DiCIELABFunction(const char *filename,
                     const E_DeviceType deviceType = EDT_Monitor,
                     const signed int ord = -1);

    /** constructor, use given array of luminance/OD values. UNTESTED
     *  Values must be sorted and complete (i.e. there must be an entry for each DDL)
     *  The given arrays are copied internally.
     *
     ** @param  val_tab     pointer to array with luminance/OD values
     *  @param  count       number of array elements (should be equal to 'max + 1')
     *  @param  max         maximum DDL (device driving level)
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
     *  @param  max         maximum DDL (device driving level)
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
     *  @param  count       number of DDLs (device driving level, 1..65536))
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


/*
 *
 * CVS/RCS Log:
 * $Log: diciefn.h,v $
 * Revision 1.13  2010-10-14 13:16:25  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2005/12/08 16:47:33  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.11  2003/12/08 18:15:58  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.10  2002/07/19 08:24:21  joergr
 * Enhanced/corrected comments.
 *
 * Revision 1.9  2002/07/18 12:26:30  joergr
 * Added support for hardcopy and softcopy input devices (camera and scanner).
 * Added polygonal curve fitting algorithm as an alternate interpolation
 * method.
 *
 * Revision 1.8  2002/07/02 16:23:41  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.7  2001/06/01 15:49:39  meichel
 * Updated copyright header
 *
 * Revision 1.6  2000/03/08 16:24:13  meichel
 * Updated copyright header.
 *
 * Revision 1.5  2000/03/06 18:18:59  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.4  1999/10/18 15:05:49  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.3  1999/10/18 10:15:49  joergr
 * Moved min/max value determination to display function base class. Now the
 * actual min/max values are also used for GSDFunction (instead of first and
 * last luminance value).
 *
 * Revision 1.2  1999/09/17 12:08:22  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.1  1999/09/10 08:50:22  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 */
