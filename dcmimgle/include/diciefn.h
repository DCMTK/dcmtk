/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Purpose: DicomCIELABFunction (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-10-18 10:15:49 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diciefn.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICIEFN_H
#define __DICIEFN_H

#include "osconfig.h"

#include "didispfn.h"
#include "dicielut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to manage CIELAB LUTs (for calibration)
 */
class DiCIELABFunction
  : public DiDisplayFunction
{

 public:

    /** constructor, read monitor characteristics file
     *
     ** @param  filename  name of the monitor characteristics file (luminance for each DDL)
     */
    DiCIELABFunction(const char *filename);

    /** constructor, use given array of luminance values. UNTESTED
     *  Values must be sorted and complete (i.e. there must be an entry for each DDL)
     *
     ** @param  lum_tab  pointer to array with luminance values (measuredin cd/m^2)
     *  @param  count    number of array elements (should be equal to 'max + 1')
     *  @param  max      maximum DDL (device driving level)
     */
    DiCIELABFunction(const double *lum_tab,
                     const Uint16 count,
                     const Uint16 max = 255);

    /** constructor, use given array of DDL and luminance values. UNTESTED
     *  Values will be automatically sorted and missing values will be calculated by means of
     *  a cubic spline interpolation.
     *
     ** @param  ddl_tab  pointer to array with DDL values (must be with the interval 0..max)
     *  @param  lum_tab  pointer to array with luminance values (measuredin cd/m^2)
     *  @param  count    number of array elements
     *  @param  max      maximum DDL (device driving level)
     */
    DiCIELABFunction(const Uint16 *ddl_tab,
                     const double *lum_tab,
                     const Uint16 count,
                     const Uint16 max = 255);

    /** destructor
     */
    virtual ~DiCIELABFunction();
    
    /** write curve data to a text file
     *
     ** @param  filename  name of the text fileto which the data should be written
     *
     ** @return status, true if successful, false otherwise
     */
    int writeCurveData(const char *filename);


 protected:

    /** create CIELAB LUT with specified number of entries
     *
     ** @param  count  number of LUT entries
     *
     ** @return pointer to created LUT if successful, NULL otherwise
     */
    DiDisplayLUT *getLookupTable(unsigned long count);


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
 * Revision 1.3  1999-10-18 10:15:49  joergr
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
