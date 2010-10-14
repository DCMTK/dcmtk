/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Purpose: DicomCIELABFunction (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:17 $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diciefn.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CMATH
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiCIELABFunction::DiCIELABFunction(const char *filename,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(filename, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY file ... ignoring");
    }
}


DiCIELABFunction::DiCIELABFunction(const double *val_tab,             // UNTESTED !!
                                   const unsigned long count,
                                   const Uint16 max,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(val_tab, count, max, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


DiCIELABFunction::DiCIELABFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                                   const double *val_tab,
                                   const unsigned long count,
                                   const Uint16 max,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(ddl_tab, val_tab, count, max, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


DiCIELABFunction::DiCIELABFunction(const double val_min,
                                   const double val_max,
                                   const unsigned long count,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(val_min, val_max, count, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiCIELABFunction::~DiCIELABFunction()
{
}


/********************************************************************/


DiDisplayLUT *DiCIELABFunction::getDisplayLUT(unsigned long count)
{
    DiDisplayLUT *lut = NULL;
    if (count <= MAX_TABLE_ENTRY_COUNT)
    {
        if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
        {
            /* hardcopy: values are in optical density, first convert them to luminance */
            double *tmp_tab = convertODtoLumTable(LODValue, ValueCount, OFFalse /*useAmb*/);
            if (tmp_tab != NULL)
            {
                checkMinMaxDensity();
                /* create new CIELAB LUT */
                lut = new DiCIELABLUT(count, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                    convertODtoLum(MaxValue, OFFalse /*useAmb*/), convertODtoLum(MinValue, OFFalse /*useAmb*/),
                    getMinLuminanceValue(), getMaxLuminanceValue(), AmbientLight, (DeviceType == EDT_Scanner));
                /* delete temporary table */
                delete[] tmp_tab;
            }
        } else {
            /* softcopy: values are already in luminance */
            lut = new DiCIELABLUT(count, MaxDDLValue, DDLValue, LODValue, ValueCount,
                MinValue, MaxValue, -1 /*Lmin*/, -1 /*Lmax*/, AmbientLight, (DeviceType == EDT_Camera));
        }
    }
    return lut;
}


int DiCIELABFunction::writeCurveData(const char *filename,
                                     const OFBool mode)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        STD_NAMESPACE ofstream file(filename);
        if (file)
        {
            const OFBool inverseLUT = (DeviceType == EDT_Scanner) || (DeviceType == EDT_Camera);
            /* comment header */
            file << "# Display function       : CIELAB" << OFendl;
            if (DeviceType == EDT_Printer)
                file << "# Type of output device  : Printer (hardcopy)" << OFendl;
            else if (DeviceType == EDT_Scanner)
                file << "# Type of output device  : Scanner (hardcopy)" << OFendl;
            else if (DeviceType == EDT_Camera)
                file << "# Type of output device  : Camera (softcopy)" << OFendl;
            else
                file << "# Type of output device  : Monitor (softcopy)" << OFendl;
            file << "# Device driving levels  : " << ValueCount << OFendl;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
                file << "# Illumination  [cd/m^2] : " << Illumination << OFendl;
            file << "# Ambient light [cd/m^2] : " << AmbientLight << OFendl;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
            {
                const double min_lum = getMinLuminanceValue();
                const double max_lum = getMaxLuminanceValue();
                file << "# Luminance w/o [cd/m^2] : " << convertODtoLum(MaxValue, OFFalse /*useAmb*/) << " - "
                                                      << convertODtoLum(MinValue, OFFalse /*useAmb*/);
                if ((min_lum >= 0) || (max_lum >= 0))
                {
                    file << " (Lmin = ";
                    if (min_lum >= 0)
                        file << min_lum;
                    else
                        file << "n/s";
                    file << ", Lmax = ";
                    if (max_lum >= 0)
                        file << max_lum;
                    else
                        file << "n/s";
                    file << ")";
                }
                file << OFendl;
                file << "# Optical density   [OD] : " << MinValue << " - " << MaxValue;
                if ((MinDensity >= 0) || (MaxDensity >= 0))
                {
                    file << " (Dmin = ";
                    if (MinDensity >= 0)
                        file << MinDensity;
                    else
                        file << "n/s";
                    file << ", Dmax = ";
                    if (MaxDensity >= 0)
                        file << MaxDensity;
                    else
                        file << "n/s";
                    file << ")";
                }
                file << OFendl;
            } else
                file << "# Luminance w/o [cd/m^2] : " << MinValue << " - " << MaxValue << OFendl;
            file << "# Interpolation method   : ";
            if (getPolynomialOrder() > 0)
                file << "Curve fitting algorithm with order " << getPolynomialOrder() << OFendl << OFendl;
            else
                file << "Cubic spline interpolation" << OFendl << OFendl;
            /* print headings of the table */
            if (mode)
            {
                file << "# NB: values for CC, CIELAB and PSC";
                if (inverseLUT)
                    file << "'";                // add ' to PSC
                file << " are specified in cd/m^2" << OFendl << OFendl;
                file << "DDL\tCC\tCIELAB\tPSC";
                if (inverseLUT)
                    file << "'";                // add ' to PSC
                file << OFendl;
            } else {
                file << "# NB: values for CC and CIELAB are specified in cd/m^2" << OFendl << OFendl;
                file << "DDL\tCIELAB" << OFendl;
            }
            /* create CIELAB LUT and write curve data to file */
            DiCIELABLUT *lut = NULL;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
            {
                /* hardcopy: values are in optical density, first convert them to luminance */
                double *tmp_tab = convertODtoLumTable(LODValue, ValueCount, OFFalse /*useAmb*/);
                if (tmp_tab != NULL)
                {
                    checkMinMaxDensity();
                    lut = new DiCIELABLUT(ValueCount, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                        convertODtoLum(MaxValue, OFFalse /*useAmb*/), convertODtoLum(MinValue, OFFalse /*useAmb*/),
                        getMinLuminanceValue(), getMaxLuminanceValue(), AmbientLight, inverseLUT, &file, mode);
                    /* delete temporary table */
                    delete[] tmp_tab;
                }
            } else {
                /* softcopy: values are already in luminance */
                lut = new DiCIELABLUT(ValueCount, MaxDDLValue, DDLValue, LODValue, ValueCount,
                    MinValue, MaxValue, -1 /*Lmin*/, -1 /*Lmax*/, AmbientLight, inverseLUT, &file, mode);
            }
            int status = (lut != NULL) && (lut->isValid());
            delete lut;
            return status;
        }
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diciefn.cc,v $
 * Revision 1.27  2010-10-14 13:14:17  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.26  2010-06-25 09:15:19  uli
 * Fixed issues with compiling with HAVE_STD_STRING.
 *
 * Revision 1.25  2009-11-25 16:11:03  joergr
 * Removed inclusion of header file "ofconsol.h".
 *
 * Revision 1.24  2009-10-28 14:26:01  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.23  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.22  2006-08-15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.21  2005/12/08 15:42:44  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.20  2004/01/05 14:58:42  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.19  2003/12/08 17:40:54  joergr
 * Updated CVS header.
 *
 * Revision 1.18  2003/02/12 11:37:14  joergr
 * Added Dmin/max support to CIELAB calibration routines.
 *
 * Revision 1.17  2002/11/27 14:08:10  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.16  2002/07/19 13:08:26  joergr
 * Enhanced/corrected comments.
 *
 * Revision 1.15  2002/07/18 12:32:37  joergr
 * Added support for hardcopy and softcopy input devices (camera and scanner).
 * Added polynomial curve fitting algorithm as an alternate interpolation
 * method.
 *
 * Revision 1.14  2002/07/03 13:50:58  joergr
 * Fixed inconsistencies regarding the handling of ambient light.
 *
 * Revision 1.13  2002/07/02 16:24:36  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.12  2002/04/16 13:53:31  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.11  2001/06/01 15:49:53  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/04/28 12:33:41  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.9  2000/04/27 13:10:24  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.8  2000/03/08 16:24:26  meichel
 * Updated copyright header.
 *
 * Revision 1.7  2000/03/06 18:20:34  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.6  2000/03/03 14:09:16  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.5  2000/02/02 11:04:52  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.4  1999/10/18 15:06:23  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.3  1999/10/18 10:14:26  joergr
 * Moved min/max value determination to display function base class. Now the
 * actual min/max values are also used for GSDFunction (instead of first and
 * last luminance value).
 *
 * Revision 1.2  1999/09/17 13:13:29  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.1  1999/09/10 08:54:47  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 */
