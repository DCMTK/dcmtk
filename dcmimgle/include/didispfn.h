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
 *  Purpose: DicomDisplayFunction (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-10-18 15:05:51 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/didispfn.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIDISPFN_H
#define __DIDISPFN_H

#include "osconfig.h"

#include "didislut.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_DISPLAY_FUNCTIONS 2
#define MAX_NUMBER_OF_TABLES 15
#define WIDTH_OF_PVALUES     16


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle monitor characteristics file 
 *  and manage display LUTs (for calibration)
 */
class DiDisplayFunction
{

 public:

    /** constructor, read monitor characteristics file
     *
     ** @param  filename  name of the monitor characteristics file (luminance for each DDL)
     */
    DiDisplayFunction(const char *filename);

    /** constructor, use given array of luminance values. UNTESTED
     *  Values must be sorted and complete (i.e. there must be an entry for each DDL)
     *
     ** @param  lum_tab  pointer to array with luminance values (measuredin cd/m^2)
     *  @param  count    number of array elements (should be equal to 'max + 1')
     *  @param  max      maximum DDL (device driving level)
     */
    DiDisplayFunction(const double *lum_tab,
                      const unsigned long count,
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
    DiDisplayFunction(const Uint16 *ddl_tab,
                      const double *lum_tab,
                      const unsigned long count,
                      const Uint16 max = 255);

    /** constructor, compute luminance values automatically within the specified range.
     *
     ** @param  lum_min  minimum luminance value
     *  @param  lum_max  maximum luminance value
     *  @param  count    number of DDLs (device driving level)
     */
    DiDisplayFunction(const double lum_min,
                      const double lum_max,
                      const unsigned long count = 256);

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

    /** get maximum DDL value
     *
     ** @return maximum DDL value
     */
    inline Uint16 getMaxDDLValue() const
    {
        return MaxDDLValue;
    }

    /** create CIELAB LUT with specified number of entries
     *
     ** @param  bits   depth of input values
     *  @param  count  number of LUT entries (default: 0 = auto)
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
     ** @param  filename  name of the text fileto which the data should be written
     *  @param  mode      write CC and PSC to file if OFTrue
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int writeCurveData(const char *filename,
                               const OFBool mode = OFTrue) = 0;
    
    /** get ambient light value.
     *  (measured in cd/m^2)
     *
     ** @return ambient light value
     */
    inline double getAmbientLightValue() const
    {
        return AmbientLight;
    }
    
    /** set ambient light value.
     *  (measured in cd/m^2)
     *
     ** @param  value  ambient light value to be set (> 0)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int setAmbientLightValue(const double value);


 protected:

    /** create display LUT with specified number of entries (abstract method)
     *
     ** @param  count  number of LUT entries
     *
     ** @return pointer to created LUT if successful, NULL otherwise
     */
    virtual DiDisplayLUT *getLookupTable(unsigned long count) = 0;

    /** read the given monitor characteristics file
     *
     ** @param  filename  name of the monitor characteristics file
     *
     ** @return status, true if successful, false otherwise
     */
    int readConfigFile(const char *filename);

    /** create a sorted (by DDL) table from the given DDL/luminance tables
     *
     ** @param  ddl_tab  pointer to array with DDL values
     *  @param  lum_tab  pointer to array with luminance values
     *
     ** @return status, true if successful, false otherwise
     */
    int createSortedTable(const Uint16 *ddl_tab,
                          const double *lum_tab);

    /** interpolate monitor characteristic curve by means of a cubic spline interpolation
     */
    int interpolateValues();

    /** calculate minimum and maximum luminance values
     *
     ** @return status, true if successful, false otherwise
     */
    int calculateMinMax();

    /// status flag, indicating whether display function is valid
    int Valid;

    /// number of DDL/luminance values
    unsigned long ValueCount;
    /// maximum DDL value (normally 255)
    Uint16 MaxDDLValue;
    
    /// ambient light values
    double AmbientLight;

    /// pointer to array of DDL values
    Uint16 *DDLValue;
    /// pointer to array of corresponding luminance values
    double *LumValue;

    /// minimum luminance value
    double MinLumValue;
    /// maximum luminance value
    double MaxLumValue;

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


/*
 *
 * CVS/RCS Log:
 * $Log: didispfn.h,v $
 * Revision 1.10  1999-10-18 15:05:51  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.9  1999/10/18 10:15:50  joergr
 * Moved min/max value determination to display function base class. Now the
 * actual min/max values are also used for GSDFunction (instead of first and
 * last luminance value).
 *
 * Revision 1.8  1999/09/17 12:08:24  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.7  1999/09/10 08:45:18  joergr
 * Added support for CIELAB display function.
 *
 * Revision 1.6  1999/03/24 17:19:20  joergr
 * Added/Modified comments and formatting.
 * Added support for Barten transformation from 2 to 7 bits input (now: 2-16).
 *
 * Revision 1.5  1999/03/03 11:47:42  joergr
 * Added support to specify ambient light value (re: Barten transformation).
 *
 * Revision 1.4  1999/02/23 16:55:32  joergr
 * Added tool to export display curves to a text file.
 *
 * Revision 1.3  1999/02/11 15:44:22  joergr
 * Removed unused parameter / member variable.
 *
 * Revision 1.2  1999/02/05 16:43:12  joergr
 * Added optional parameter to method convertPValueToDDL to specify width
 * of output data (number of bits).
 *
 * Revision 1.1  1999/02/03 17:46:37  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 */
