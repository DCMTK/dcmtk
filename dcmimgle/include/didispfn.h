/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Update Date:      $Date: 2002-07-03 13:50:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/didispfn.h,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
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

/** Class to handle monitor and printer characteristics file
 *  and manage display LUTs (for calibration)
 */
class DiDisplayFunction
{

 public:

    /** output device type
     */
    enum E_DeviceType
    {
        /// monitor (softcopy device), input expected to be in luminance (cd/m^2)
        EDT_Monitor,
        /// printer (hardcopy device), input expected to be in optical density (OD)
        EDT_Printer
    };

    /** constructor, read monitor/printer characteristics file.
     *  Supported keywords: "max" for maximum DDL (Device Driving Level, required)
     *                      "amb" for ambient light and "lum" for illumination (both optional)
     *
     ** @param  filename    name of the characteristics file (luminance/OD for each DDL)
     *  @param  deviceType  type of the output device (default: monitor)
     */
    DiDisplayFunction(const char *filename,
                      const E_DeviceType deviceType = EDT_Monitor);

    /** constructor, use given array of luminance/OD values. UNTESTED
     *  Values must be sorted and complete (i.e. there must be an entry for each DDL).
     *  Input table of values is copied.
     *
     ** @param  val_tab     pointer to array with luminance/OD values
     *  @param  count       number of array elements (should be equal to 'max + 1')
     *  @param  max         maximum DDL (device driving level)
     *  @param  deviceType  type of the output device (default: monitor)
     */
    DiDisplayFunction(const double *val_tab,
                      const unsigned long count,
                      const Uint16 max = 255,
                      const E_DeviceType deviceType = EDT_Monitor);

    /** constructor, use given array of DDL and luminance/OD values. UNTESTED
     *  Values will be automatically sorted and missing values will be calculated by means of
     *  a cubic spline interpolation.  Input table of values is copied.
     *
     ** @param  ddl_tab     pointer to array with DDL values (must be with the interval 0..max)
     *  @param  val_tab     pointer to array with luminance/OD values
     *  @param  count       number of array elements
     *  @param  max         maximum DDL (device driving level)
     *  @param  deviceType  type of the output device (default: monitor)
     */
    DiDisplayFunction(const Uint16 *ddl_tab,
                      const double *val_tab,
                      const unsigned long count,
                      const Uint16 max = 255,
                      const E_DeviceType deviceType = EDT_Monitor);

    /** constructor, compute luminance/OD values automatically within the specified range.
     *
     ** @param  val_min     minimum luminance/OD value
     *  @param  val_max     maximum luminance/OD value
     *  @param  count       number of DDLs (device driving level)
     *  @param  deviceType  type of the output device (default: monitor)
     */
    DiDisplayFunction(const double val_min,
                      const double val_max,
                      const unsigned long count = 256,
                      const E_DeviceType deviceType = EDT_Monitor);

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

    /** get output device type (monitor or printer)
     *
     ** @return output device type
     */
    inline E_DeviceType getDeviceType() const
    {
        return DeviceType;
    }

    /** get maximum DDL value
     *
     ** @return maximum DDL value
     */
    inline Uint16 getMaxDDLValue() const
    {
        return MaxDDLValue;
    }

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
     *  measured in cd/m^2. applicable to softcopy (monitor) and hardcopy (printer) devices.
     *  typical values: 0.5-5 for softcopy devices, 10 for transmissive hardcopy printer
     *  and 0 for reflective hardcopy printers.
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
     *  measured in cd/m^2. applicable to hardcopy (printer) devices only.
     *  typical values: 2000 for transmissive hardcopy printer and 150 for
     *  reflective hardcopy printers.
     *
     ** @param  value  illumination value to be set (>= 0)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int setIlluminationValue(const double value);

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

    /** read the given monitor/printer characteristics file
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
                                const OFBool useAmb = OFTrue);

    /** interpolate monitor/printer characteristic curve by means of a cubic spline interpolation
     */
    int interpolateValues();

    /** calculate minimum and maximum luminance/OD values
     *
     ** @return status, true if successful, false otherwise
     */
    int calculateMinMax();

    /// status flag, indicating whether display function is valid
    int Valid;

    /// output device type (monitor or printer)
    const E_DeviceType DeviceType;

    /// number of DDL and luminance/OD values
    unsigned long ValueCount;
    /// maximum DDL value (usually 255)
    Uint16 MaxDDLValue;

    /// (reflected) ambient light value
    double AmbientLight;
    /// illumination value
    double Illumination;

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


/*
 *
 * CVS/RCS Log:
 * $Log: didispfn.h,v $
 * Revision 1.15  2002-07-03 13:50:24  joergr
 * Fixed inconsistencies regarding the handling of ambient light.
 *
 * Revision 1.14  2002/07/02 16:23:42  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.13  2001/06/01 15:49:40  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/03/08 16:24:15  meichel
 * Updated copyright header.
 *
 * Revision 1.11  2000/03/06 18:19:35  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.10  1999/10/18 15:05:51  joergr
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
